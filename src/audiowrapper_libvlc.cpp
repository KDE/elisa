/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "audiowrapper.h"

#include <QTimer>
#include <QAudio>

#include <vlc.h>

#include "config-upnp-qt.h"

class AudioWrapperPrivate
{

public:

    AudioWrapper *mParent = nullptr;

    libvlc_instance_t *mInstance = nullptr;

    libvlc_media_player_t *mPlayer = nullptr;

    libvlc_media_t *mMedia = nullptr;

    qint64 mMediaDuration = 0;

    QTimer mStateRefreshTimer;

    QMediaPlayer::State mPreviousPlayerState = QMediaPlayer::StoppedState;

    QMediaPlayer::MediaStatus mPreviousMediaStatus = QMediaPlayer::NoMedia;

    qreal mPreviousVolume = 100.0;

    qint64 mPreviousPosition = 0;

    void mediaIsEnded();

    bool signalPlaybackChange(QMediaPlayer::State newPlayerState);

    void signalMediaStatusChange(QMediaPlayer::MediaStatus newMediaStatus);

    void signalVolumeChange(int newVolume);

    void signalPositionChange(qint64 newPosition);

};


AudioWrapper::AudioWrapper(QObject *parent) : QObject(parent), d(std::make_unique<AudioWrapperPrivate>())
{
    d->mParent = this;
    d->mInstance = libvlc_new(0, nullptr);
    d->mStateRefreshTimer.setInterval(100);
    connect(&d->mStateRefreshTimer, &QTimer::timeout,
            this, &AudioWrapper::playerStateChanged);
}

AudioWrapper::~AudioWrapper()
{
    if (d->mInstance) {
        libvlc_release(d->mInstance);
    }
}

bool AudioWrapper::muted() const
{
    if (!d->mPlayer) {
        return false;
    }

    return false/*d->mPlayer.isMuted()*/;
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

    return {}/*d->mPlayer.media().canonicalUrl()*/;
}

QMediaPlayer::Error AudioWrapper::error() const
{
    if (!d->mPlayer) {
        return {};
    }

#if 0
    if (d->mPlayer.error() != QMediaPlayer::NoError) {
        qDebug() << "AudioWrapper::error" << d->mPlayer.errorString();
    }
#endif

    return {}/*d->mPlayer.error()*/;
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
    if (!d->mPlayer) {
        return false;
    }

    return libvlc_media_player_is_seekable(d->mPlayer);
}

QAudio::Role AudioWrapper::audioRole() const
{
    if (!d->mPlayer) {
        return {};
    }

    return {}/*d->mPlayer.audioRole()*/;
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
    d->mMedia = libvlc_media_new_path(d->mInstance, source.toLocalFile().toUtf8().constData());
    if (!d->mMedia) {
        return;
    }

    d->mPlayer = libvlc_media_player_new_from_media(d->mMedia);

    if (d->signalPlaybackChange(QMediaPlayer::StoppedState)) {
        Q_EMIT stopped();
    }

    d->signalMediaStatusChange(QMediaPlayer::LoadingMedia);
    d->signalMediaStatusChange(QMediaPlayer::LoadedMedia);
    d->signalMediaStatusChange(QMediaPlayer::BufferedMedia);

    d->mStateRefreshTimer.start();
}

void AudioWrapper::setPosition(qint64 position)
{
    if (!d->mPlayer) {
        return;
    }

    if (d->mMediaDuration == -1 || d->mMediaDuration == 0) {
        return;
    }

    libvlc_media_player_set_position(d->mPlayer, static_cast<float>(position) / d->mMediaDuration);
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

void AudioWrapper::setAudioRole(QAudio::Role audioRole)
{
    //    d->mPlayer.setAudioRole(audioRole);
}

void AudioWrapper::mediaStatusChanged()
{
}

void AudioWrapper::playerStateChanged()
{
    if (!d->mPlayer) {
        return;
    }

    auto newPlayerState = QMediaPlayer::State{};

    switch(libvlc_media_player_get_state(d->mPlayer))
    {
    case libvlc_Stopped:
        newPlayerState = QMediaPlayer::StoppedState;
        break;
    case libvlc_Playing:
        newPlayerState = QMediaPlayer::PlayingState;
        break;
    case libvlc_Paused:
        newPlayerState = QMediaPlayer::PausedState;
        break;
    case libvlc_Opening:
        d->signalMediaStatusChange(QMediaPlayer::LoadedMedia);
        break;
    case libvlc_Buffering:
        d->signalMediaStatusChange(QMediaPlayer::BufferedMedia);
        break;
    case libvlc_NothingSpecial:
        break;
    case libvlc_Ended:
        newPlayerState = QMediaPlayer::StoppedState;
        d->signalMediaStatusChange(QMediaPlayer::BufferedMedia);
        d->signalMediaStatusChange(QMediaPlayer::NoMedia);
        d->signalMediaStatusChange(QMediaPlayer::EndOfMedia);
        d->mediaIsEnded();
        break;
    case libvlc_Error:
        break;
    }

    if (d->mPlayer) {
        d->signalVolumeChange(libvlc_audio_get_volume(d->mPlayer));

        if (d->mMediaDuration != -1) {
            d->signalPositionChange(qRound64(libvlc_media_player_get_position(d->mPlayer) * d->mMediaDuration));
        }
    }

    if (d->signalPlaybackChange(newPlayerState)) {
        switch (d->mPreviousPlayerState)
        {
        case QMediaPlayer::StoppedState:
            Q_EMIT stopped();
            break;
        case QMediaPlayer::PlayingState:
            Q_EMIT playing();

            d->mMediaDuration = libvlc_media_get_duration(d->mMedia);
            Q_EMIT durationChanged(d->mMediaDuration);

            Q_EMIT seekableChanged(seekable());

            break;
        case QMediaPlayer::PausedState:
            Q_EMIT paused();
            break;
        }
    }
}

void AudioWrapper::playerMutedChanged()
{
}

void AudioWrapper::playerVolumeChanged()
{
}

void AudioWrapper::playerStateSignalChanges(QMediaPlayer::State newState)
{
    QTimer::singleShot(0, [this, newState]() {Q_EMIT playbackStateChanged(newState);});
}

void AudioWrapper::mediaStatusSignalChanges(QMediaPlayer::MediaStatus newStatus)
{
    QTimer::singleShot(0, [this, newStatus]() {Q_EMIT statusChanged(newStatus);});
}

void AudioWrapper::playerPositionSignalChanges(qint64 newPosition)
{
    QTimer::singleShot(0, [this, newPosition]() {Q_EMIT positionChanged(newPosition);});
}

void AudioWrapper::playerVolumeSignalChanges()
{
    QTimer::singleShot(0, [this]() {Q_EMIT volumeChanged();});
}

void AudioWrapper::playerMutedSignalChanges()
{
    QTimer::singleShot(0, [this]() {Q_EMIT mutedChanged();});
}

void AudioWrapperPrivate::mediaIsEnded()
{
    libvlc_media_release(mMedia);
    mMedia = nullptr;

    mStateRefreshTimer.stop();

    libvlc_media_player_release(mPlayer);

    mPlayer = nullptr;
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

    if (abs(mPreviousVolume - newVolume) > 0.01) {
        mPreviousVolume = newVolume;

        mParent->playerVolumeSignalChanges();
    }
}

void AudioWrapperPrivate::signalPositionChange(qint64 newPosition)
{
    if (mPreviousPosition != newPosition) {
        mPreviousPosition = newPosition;

        mParent->playerPositionSignalChanges(mPreviousPosition);
    }
}


#include "moc_audiowrapper.cpp"
