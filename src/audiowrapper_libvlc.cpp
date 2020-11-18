/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "audiowrapper.h"

#include "elisa-version.h"

#include "vlcLogging.h"
#include "powermanagementinterface.h"

#include <QAudio>
#include <QDir>

#if defined Q_OS_WIN

#include <basetsd.h>
typedef SSIZE_T ssize_t;

#endif

#include <vlc/vlc.h>

#include "config-upnp-qt.h"

class AudioWrapperPrivate
{

public:

    PowerManagementInterface mPowerInterface;

    AudioWrapper *mParent = nullptr;

    libvlc_instance_t *mInstance = nullptr;

    libvlc_media_player_t *mPlayer = nullptr;

    libvlc_event_manager_t *mPlayerEventManager = nullptr;

    libvlc_media_t *mMedia = nullptr;

    qint64 mMediaDuration = 0;

    QMediaPlayer::State mPreviousPlayerState = QMediaPlayer::StoppedState;

    QMediaPlayer::MediaStatus mPreviousMediaStatus = QMediaPlayer::NoMedia;

    qreal mPreviousVolume = 100.0;

    qint64 mSavedPosition = 0.0;

    qint64 mUndoSavedPosition = 0.0;

    qint64 mPreviousPosition = 0;

    QMediaPlayer::Error mError = QMediaPlayer::NoError;

    bool mIsMuted = false;

    bool mIsSeekable = false;

    bool mHasSavedPosition = false;

    void vlcEventCallback(const struct libvlc_event_t *p_event);

    void mediaIsEnded();

    bool signalPlaybackChange(QMediaPlayer::State newPlayerState);

    void signalMediaStatusChange(QMediaPlayer::MediaStatus newMediaStatus);

    void signalVolumeChange(int newVolume);

    void signalMutedChange(bool isMuted);

    void signalDurationChange(libvlc_time_t newDuration);

    void signalPositionChange(float newPosition);

    void signalSeekableChange(bool isSeekable);

    void signalErrorChange(QMediaPlayer::Error errorCode);

};

static void vlc_callback(const struct libvlc_event_t *p_event, void *p_data)
{
    reinterpret_cast<AudioWrapperPrivate*>(p_data)->vlcEventCallback(p_event);
}

AudioWrapper::AudioWrapper(QObject *parent) : QObject(parent), d(std::make_unique<AudioWrapperPrivate>())
{
    d->mParent = this;
    d->mInstance = libvlc_new(0, nullptr);
    libvlc_set_user_agent(d->mInstance, "elisa", "Elisa Music Player");
    libvlc_set_app_id(d->mInstance, "org.kde.elisa", ELISA_VERSION_STRING, "elisa");

    d->mPlayer = libvlc_media_player_new(d->mInstance);

    if (!d->mPlayer) {
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapper::AudioWrapper" << "failed creating player" << libvlc_errmsg();
        return;
    }

    d->mPlayerEventManager = libvlc_media_player_event_manager(d->mPlayer);

    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerOpening, &vlc_callback, d.get());
    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerBuffering, &vlc_callback, d.get());
    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerPlaying, &vlc_callback, d.get());
    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerPaused, &vlc_callback, d.get());
    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerStopped, &vlc_callback, d.get());
    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerEndReached, &vlc_callback, d.get());
    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerEncounteredError, &vlc_callback, d.get());
    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerPositionChanged, &vlc_callback, d.get());
    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerSeekableChanged, &vlc_callback, d.get());
    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerLengthChanged, &vlc_callback, d.get());
    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerMuted, &vlc_callback, d.get());
    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerUnmuted, &vlc_callback, d.get());
    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerAudioVolume, &vlc_callback, d.get());
    libvlc_event_attach(d->mPlayerEventManager, libvlc_MediaPlayerAudioDevice, &vlc_callback, d.get());
}

AudioWrapper::~AudioWrapper()
{
    if (d->mInstance) {
        d->mPowerInterface.setPreventSleep(false);
        if (d->mPlayer && d->mPreviousPlayerState != QMediaPlayer::StoppedState) {
            libvlc_media_player_stop(d->mPlayer);
        }
        libvlc_release(d->mInstance);
    }
}

bool AudioWrapper::muted() const
{
    return d->mIsMuted;
}

qreal AudioWrapper::volume() const
{
    if (!d->mPlayer) {
        return 100.0;
    }

    return d->mPreviousVolume;
}

QUrl AudioWrapper::source() const
{
    if (!d->mPlayer) {
        return {};
    }
    if (d->mMedia) {
        auto filePath = QString::fromUtf8(libvlc_media_get_mrl(d->mMedia));
        return QUrl::fromUserInput(filePath);
    }
    return {};
}

QMediaPlayer::Error AudioWrapper::error() const
{
    return d->mError;
}

qint64 AudioWrapper::duration() const
{
    return d->mMediaDuration;
}

qint64 AudioWrapper::position() const
{
    if (!d->mPlayer) {
        return 0;
    }

    if (d->mMediaDuration == -1) {
        return 0;
    }

    return qRound64(libvlc_media_player_get_position(d->mPlayer) * d->mMediaDuration);
}

bool AudioWrapper::seekable() const
{
    return d->mIsSeekable;
}

QMediaPlayer::State AudioWrapper::playbackState() const
{
    return d->mPreviousPlayerState;
}

QMediaPlayer::MediaStatus AudioWrapper::status() const
{
    return d->mPreviousMediaStatus;
}

void AudioWrapper::setMuted(bool muted)
{
    if (!d->mPlayer) {
        return;
    }

    libvlc_audio_set_mute(d->mPlayer, muted);
}

void AudioWrapper::setVolume(qreal volume)
{
    if (!d->mPlayer) {
        return;
    }

    //auto realVolume = static_cast<qreal>(QAudio::convertVolume(volume / 100.0, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale));
    libvlc_audio_set_volume(d->mPlayer, qRound(volume));
}

void AudioWrapper::setSource(const QUrl &source)
{
    if (source.isLocalFile()) {
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapper::setSource reading local resource";
        d->mMedia = libvlc_media_new_path(d->mInstance, QDir::toNativeSeparators(source.toLocalFile()).toUtf8().constData());
    } else {
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapper::setSource reading remote resource";
        d->mMedia = libvlc_media_new_location(d->mInstance, source.url().toUtf8().constData());
    }

    if (!d->mMedia) {
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapper::setSource"
                 << "failed creating media"
                 << libvlc_errmsg()
                 << QDir::toNativeSeparators(source.toLocalFile()).toUtf8().constData();

        d->mMedia = libvlc_media_new_path(d->mInstance, QDir::toNativeSeparators(source.toLocalFile()).toLatin1().constData());
        if (!d->mMedia) {
            qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapper::setSource"
                     << "failed creating media"
                     << libvlc_errmsg()
                     << QDir::toNativeSeparators(source.toLocalFile()).toLatin1().constData();
            return;
        }
    }

    libvlc_media_player_set_media(d->mPlayer, d->mMedia);

    if (d->signalPlaybackChange(QMediaPlayer::StoppedState)) {
        Q_EMIT stopped();
    }

    d->signalMediaStatusChange(QMediaPlayer::LoadingMedia);
    d->signalMediaStatusChange(QMediaPlayer::LoadedMedia);
    d->signalMediaStatusChange(QMediaPlayer::BufferedMedia);

    d->mHasSavedPosition = false;
}

void AudioWrapper::setPosition(qint64 position)
{
    if (!d->mPlayer) {
        return;
    }

    if (d->mMediaDuration == -1 || d->mMediaDuration == 0) {
        savePosition(position);
        return;
    }

    libvlc_media_player_set_position(d->mPlayer, static_cast<float>(position) / d->mMediaDuration);
}

void AudioWrapper::savePosition(qint64 position)
{
    if (!d->mHasSavedPosition) {
        d->mHasSavedPosition = true;
        d->mSavedPosition = position;
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapper::savePosition" << "restore old position" << d->mSavedPosition;
    }
}

void AudioWrapper::saveUndoPosition(qint64 position)
{
    d->mUndoSavedPosition = position;
}

void AudioWrapper::restoreUndoPosition()
{
    d->mHasSavedPosition = true;
    d->mSavedPosition = d->mUndoSavedPosition;
}

void AudioWrapper::play()
{
    if (!d->mPlayer) {
        return;
    }

    libvlc_media_player_play(d->mPlayer);
}

void AudioWrapper::pause()
{
    if (!d->mPlayer) {
        return;
    }

    libvlc_media_player_pause(d->mPlayer);
}

void AudioWrapper::stop()
{
    if (!d->mPlayer) {
        return;
    }

    libvlc_media_player_stop(d->mPlayer);
}

void AudioWrapper::seek(qint64 position)
{
    setPosition(position);
}

void AudioWrapper::mediaStatusChanged()
{
}

void AudioWrapper::playerStateChanged()
{
}

void AudioWrapper::playerMutedChanged()
{
}

void AudioWrapper::playerVolumeChanged()
{
}

void AudioWrapper::playerStateSignalChanges(QMediaPlayer::State newState)
{
    QMetaObject::invokeMethod(this, [this, newState]() {
        Q_EMIT playbackStateChanged(newState);
        switch (newState)
        {
        case QMediaPlayer::StoppedState:
            Q_EMIT stopped();
            d->mPowerInterface.setPreventSleep(false);
            break;
        case QMediaPlayer::PlayingState:
            Q_EMIT playing();
            d->mPowerInterface.setPreventSleep(true);
            break;
        case QMediaPlayer::PausedState:
            Q_EMIT paused();
            d->mPowerInterface.setPreventSleep(false);
            break;
        }
    }, Qt::QueuedConnection);
}

void AudioWrapper::mediaStatusSignalChanges(QMediaPlayer::MediaStatus newStatus)
{
    QMetaObject::invokeMethod(this, [this, newStatus]() {Q_EMIT statusChanged(newStatus);}, Qt::QueuedConnection);
}

void AudioWrapper::playerErrorSignalChanges(QMediaPlayer::Error error)
{
    QMetaObject::invokeMethod(this, [this, error]() {Q_EMIT errorChanged(error);}, Qt::QueuedConnection);
}

void AudioWrapper::playerDurationSignalChanges(qint64 newDuration)
{
    QMetaObject::invokeMethod(this, [this, newDuration]() {Q_EMIT durationChanged(newDuration);}, Qt::QueuedConnection);
}

void AudioWrapper::playerPositionSignalChanges(qint64 newPosition)
{
    QMetaObject::invokeMethod(this, [this, newPosition]() {Q_EMIT positionChanged(newPosition);}, Qt::QueuedConnection);
}

void AudioWrapper::playerVolumeSignalChanges()
{
    QMetaObject::invokeMethod(this, [this]() {Q_EMIT volumeChanged();}, Qt::QueuedConnection);
}

void AudioWrapper::playerMutedSignalChanges(bool isMuted)
{
    QMetaObject::invokeMethod(this, [this, isMuted]() {Q_EMIT mutedChanged(isMuted);}, Qt::QueuedConnection);
}

void AudioWrapper::playerSeekableSignalChanges(bool isSeekable)
{
    QMetaObject::invokeMethod(this, [this, isSeekable]() {Q_EMIT seekableChanged(isSeekable);}, Qt::QueuedConnection);
}

void AudioWrapperPrivate::vlcEventCallback(const struct libvlc_event_t *p_event)
{
    const auto eventType = static_cast<libvlc_event_e>(p_event->type);

    switch(eventType)
    {
    case libvlc_MediaPlayerOpening:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerOpening";
        signalMediaStatusChange(QMediaPlayer::LoadedMedia);
        break;
    case libvlc_MediaPlayerBuffering:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerBuffering";
        signalMediaStatusChange(QMediaPlayer::BufferedMedia);
        break;
    case libvlc_MediaPlayerPlaying:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerPlaying";
        signalPlaybackChange(QMediaPlayer::PlayingState);
        break;
    case libvlc_MediaPlayerPaused:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerPaused";
        signalPlaybackChange(QMediaPlayer::PausedState);
        break;
    case libvlc_MediaPlayerStopped:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerStopped";
        signalPlaybackChange(QMediaPlayer::StoppedState);
        break;
    case libvlc_MediaPlayerEndReached:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerEndReached";
        signalMediaStatusChange(QMediaPlayer::BufferedMedia);
        signalMediaStatusChange(QMediaPlayer::NoMedia);
        signalMediaStatusChange(QMediaPlayer::EndOfMedia);
        mediaIsEnded();
        break;
    case libvlc_MediaPlayerEncounteredError:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerEncounteredError";
        signalErrorChange(QMediaPlayer::ResourceError);
        mediaIsEnded();
        signalMediaStatusChange(QMediaPlayer::InvalidMedia);
        break;
    case libvlc_MediaPlayerPositionChanged:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerPositionChanged";
        signalPositionChange(p_event->u.media_player_position_changed.new_position);
        break;
    case libvlc_MediaPlayerSeekableChanged:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerSeekableChanged";
        signalSeekableChange(p_event->u.media_player_seekable_changed.new_seekable);
        break;
    case libvlc_MediaPlayerLengthChanged:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerLengthChanged";
        signalDurationChange(p_event->u.media_player_length_changed.new_length);
        if (mHasSavedPosition) {
            mParent->setPosition(mSavedPosition);
            mHasSavedPosition = false;
        }
        break;
    case libvlc_MediaPlayerMuted:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerMuted";
        signalMutedChange(true);
        break;
    case libvlc_MediaPlayerUnmuted:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerUnmuted";
        signalMutedChange(false);
        break;
    case libvlc_MediaPlayerAudioVolume:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerAudioVolume";
        signalVolumeChange(qRound(p_event->u.media_player_audio_volume.volume * 100));
        break;
    case libvlc_MediaPlayerAudioDevice:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "libvlc_MediaPlayerAudioDevice";
        break;
    default:
        qCDebug(orgKdeElisaPlayerVlc) << "AudioWrapperPrivate::vlcEventCallback" << "eventType" << eventType;
        break;
    }
}

void AudioWrapperPrivate::mediaIsEnded()
{
    libvlc_media_release(mMedia);
    mMedia = nullptr;
}

bool AudioWrapperPrivate::signalPlaybackChange(QMediaPlayer::State newPlayerState)
{
    if (mPreviousPlayerState != newPlayerState) {
        mPreviousPlayerState = newPlayerState;

        mParent->playerStateSignalChanges(mPreviousPlayerState);

        return true;
    }

    return false;
}

void AudioWrapperPrivate::signalMediaStatusChange(QMediaPlayer::MediaStatus newMediaStatus)
{
    if (mPreviousMediaStatus != newMediaStatus) {
        mPreviousMediaStatus = newMediaStatus;

        mParent->mediaStatusSignalChanges(mPreviousMediaStatus);
    }
}

void AudioWrapperPrivate::signalVolumeChange(int newVolume)
{
    if (newVolume == -1) {
        return;
    }

    if (mPreviousPlayerState == QMediaPlayer::StoppedState) {
        return;
    }

    if (abs(int(mPreviousVolume - newVolume)) > 0.01) {
        mPreviousVolume = newVolume;

        mParent->playerVolumeSignalChanges();
    }
}

void AudioWrapperPrivate::signalMutedChange(bool isMuted)
{
    if (mIsMuted != isMuted) {
        mIsMuted = isMuted;

        mParent->playerMutedSignalChanges(mIsMuted);
    }
}

void AudioWrapperPrivate::signalDurationChange(libvlc_time_t newDuration)
{
    if (mMediaDuration != newDuration) {
        mMediaDuration = newDuration;

        mParent->playerDurationSignalChanges(mMediaDuration);
    }
}

void AudioWrapperPrivate::signalPositionChange(float newPosition)
{
    if (mMediaDuration == -1) {
        return;
    }

    auto computedPosition = qRound64(newPosition * mMediaDuration);

    if (mPreviousPosition != computedPosition) {
        mPreviousPosition = computedPosition;

        mParent->playerPositionSignalChanges(mPreviousPosition);
    }

    if (this->mMedia) {
        QString title = QLatin1String(libvlc_media_get_meta(this->mMedia, libvlc_meta_Title));
        QString nowPlaying = QLatin1String(libvlc_media_get_meta(this->mMedia, libvlc_meta_NowPlaying));

        Q_EMIT mParent->currentPlayingForRadiosChanged(title, nowPlaying);
    }
}

void AudioWrapperPrivate::signalSeekableChange(bool isSeekable)
{
    if (mIsSeekable != isSeekable) {
        mIsSeekable = isSeekable;

        mParent->playerSeekableSignalChanges(isSeekable);
    }
}

void AudioWrapperPrivate::signalErrorChange(QMediaPlayer::Error errorCode)
{
    if (mError != errorCode) {
        mError = errorCode;

        mParent->playerErrorSignalChanges(errorCode);
    }
}


#include "moc_audiowrapper.cpp"
