/*
   SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "audiowrapper.h"

#include "dragonLogging.h"
#include "powermanagementinterface.h"

#include <QLoggingCategory>
#include <QTimer>

#include <DragonAudioOutput>
#include <DragonIcyMetadata>
#include <DragonPlayer>

#include <optional>

#include <chrono>

using namespace std::chrono_literals;

class AudioWrapperPrivate
{
public:
    PowerManagementInterface mPowerInterface;

    DragonPlayer mPlayer;

    std::optional<qint64> mSavedPosition;

    qint64 mUndoSavedPosition = 0.0;

    QMediaPlayer::PlaybackState mCurrentPlaybackState = QMediaPlayer::StoppedState;

    QMediaPlayer::MediaStatus mCurrentMediaStatus = QMediaPlayer::NoMedia;

    bool mQueuedStatusUpdate = false;
};

static constexpr QMediaPlayer::PlaybackState mapPlaybackState(DragonPlayer::PlaybackState state)
{
    switch (state) {
    case DragonPlayer::PlaybackState::StoppedState:
        return QMediaPlayer::StoppedState;
    case DragonPlayer::PlaybackState::PlayingState:
        return QMediaPlayer::PlayingState;
    case DragonPlayer::PlaybackState::PausedState:
        return QMediaPlayer::PausedState;
    }
    return QMediaPlayer::StoppedState;
}

static constexpr QMediaPlayer::MediaStatus mapMediaStatus(DragonPlayer::MediaStatus status)
{
    switch (status) {
    case DragonPlayer::MediaStatus::NoMedia:
        return QMediaPlayer::NoMedia;
    case DragonPlayer::MediaStatus::LoadingMedia:
        return QMediaPlayer::LoadingMedia;
    case DragonPlayer::MediaStatus::LoadedMedia:
        return QMediaPlayer::LoadedMedia;
    case DragonPlayer::MediaStatus::BufferingMedia:
        return QMediaPlayer::BufferingMedia;
    case DragonPlayer::MediaStatus::StalledMedia:
        return QMediaPlayer::StalledMedia;
    case DragonPlayer::MediaStatus::BufferedMedia:
        return QMediaPlayer::BufferedMedia;
    case DragonPlayer::MediaStatus::EndOfMedia:
        return QMediaPlayer::EndOfMedia;
    case DragonPlayer::MediaStatus::InvalidMedia:
        return QMediaPlayer::InvalidMedia;
    }
    return QMediaPlayer::NoMedia;
}

static constexpr QMediaPlayer::Error mapError(DragonPlayer::Error error)
{
    switch (error) {
    case DragonPlayer::Error::NoError:
        return QMediaPlayer::NoError;
    case DragonPlayer::Error::ResourceError:
        return QMediaPlayer::ResourceError;
    case DragonPlayer::Error::FormatError:
        return QMediaPlayer::FormatError;
    case DragonPlayer::Error::NetworkError:
        return QMediaPlayer::NetworkError;
    case DragonPlayer::Error::AccessDenied:
        return QMediaPlayer::AccessDeniedError;
    }
    return QMediaPlayer::NoError;
}

AudioWrapper::AudioWrapper(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<AudioWrapperPrivate>())
{
    QLoggingCategory::setFilterRules(QStringLiteral("org.kde.dragonmediabackend.*.debug=true"));
    connect(&d->mPlayer, &DragonPlayer::stateChanged, this, [this](DragonPlayer::PlaybackState, DragonPlayer::PlaybackState) {
        queueStatusChanged();
    });
    connect(&d->mPlayer, &DragonPlayer::statusChanged, this, [this](DragonPlayer::MediaStatus) {
        queueStatusChanged();
    });
    connect(&d->mPlayer, &DragonPlayer::errorChanged, this, [this](DragonPlayer::Error error) {
        Q_EMIT errorChanged(mapError(error));
    });
    connect(&d->mPlayer, &DragonPlayer::durationChanged, this, [this](std::optional<std::chrono::milliseconds> duration) {
        Q_EMIT durationChanged(duration ? duration->count() : 0);
    });
    connect(&d->mPlayer, &DragonPlayer::positionChanged, this, [this](std::chrono::milliseconds position) {
        Q_EMIT positionChanged(position.count());
    });
    connect(&d->mPlayer, &DragonPlayer::seekableChanged, this, &AudioWrapper::seekableChanged);
    connect(&d->mPlayer, &DragonPlayer::sourceChanged, this, &AudioWrapper::sourceChanged);
    connect(d->mPlayer.audioOutput(), &DragonAudioOutput::volumeChanged, this, &AudioWrapper::playerVolumeChanged);
    connect(d->mPlayer.audioOutput(), &DragonAudioOutput::mutedChanged, this, &AudioWrapper::playerMutedChanged);
    connect(&d->mPlayer, &DragonPlayer::currentPlayingForRadiosChanged, this, [this](const DragonIcyMetadata &metadata) {
        Q_EMIT currentPlayingForRadiosChanged(metadata.streamTitle(), QString{});
    });
}

AudioWrapper::~AudioWrapper()
{
    d->mPowerInterface.setPreventSleep(false);
}

bool AudioWrapper::muted() const
{
    return d->mPlayer.audioOutput()->muted();
}

qreal AudioWrapper::volume() const
{
    // DragonMediaBackend returns 0.0-1.0 slider position, convert to Elisa's 0-100 range
    return d->mPlayer.audioOutput()->volume() * 100.0;
}

QUrl AudioWrapper::source() const
{
    return d->mPlayer.source();
}

QMediaPlayer::MediaStatus AudioWrapper::status() const
{
    return d->mCurrentMediaStatus;
}

QMediaPlayer::PlaybackState AudioWrapper::playbackState() const
{
    return d->mCurrentPlaybackState;
}

QMediaPlayer::Error AudioWrapper::error() const
{
    if (d->mPlayer.error() != DragonPlayer::Error::NoError) {
        qCWarning(orgKdeElisaPlayerDragon) << "AudioWrapper::error" << static_cast<int>(d->mPlayer.error()) << d->mPlayer.errorString();
    }

    return mapError(d->mPlayer.error());
}

qint64 AudioWrapper::duration() const
{
    return d->mPlayer.duration().value_or(std::chrono::milliseconds{0}).count();
}

qint64 AudioWrapper::position() const
{
    return static_cast<qint64>(d->mPlayer.position().count());
}

bool AudioWrapper::seekable() const
{
    return d->mPlayer.seekable();
}

void AudioWrapper::setMuted(bool muted)
{
    d->mPlayer.audioOutput()->setMuted(muted);
}

void AudioWrapper::setVolume(qreal volume)
{
    qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::setVolume" << volume;

    // DragonMediaBackend expects 0.0-1.0 slider position and handles dB conversion internally
    d->mPlayer.audioOutput()->setVolume(volume / 100.0);
}

void AudioWrapper::setSource(const QUrl &source)
{
    qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::setSource" << source;

    d->mPlayer.setSource(source);
    d->mSavedPosition.reset();
}

void AudioWrapper::setPosition(qint64 position)
{
    qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::setPosition" << position;

    if (d->mPlayer.duration().value_or(std::chrono::milliseconds{0}) <= 0ms) {
        savePosition(position);
        return;
    }

    d->mPlayer.setPosition(std::chrono::milliseconds{position});
}

void AudioWrapper::play()
{
    qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::play";

    d->mPlayer.play();

    if (d->mSavedPosition) {
        qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::play" << "restore old position" << *d->mSavedPosition;

        setPosition(*d->mSavedPosition);
        d->mSavedPosition.reset();
    }
}

void AudioWrapper::pause()
{
    qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::pause";

    d->mPlayer.pause();
}

void AudioWrapper::stop()
{
    qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::stop";

    if (d->mPlayer.playbackState() == DragonPlayer::PlaybackState::StoppedState && d->mPlayer.status() != DragonPlayer::MediaStatus::LoadingMedia) {
        return;
    }

    d->mPlayer.stop();
}

void AudioWrapper::seek(qint64 position)
{
    qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::seek" << position;

    d->mPlayer.setPosition(std::chrono::milliseconds{position});
}

void AudioWrapper::mediaStatusChanged()
{
    qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::mediaStatusChanged" << static_cast<int>(d->mPlayer.status());
}

void AudioWrapper::notifyStatusChanges()
{
    d->mQueuedStatusUpdate = false;

    const auto currentState = mapPlaybackState(d->mPlayer.playbackState());
    const auto currentStatus = mapMediaStatus(d->mPlayer.status());

    if (currentStatus != d->mCurrentMediaStatus) {
        d->mCurrentMediaStatus = currentStatus;
        Q_EMIT statusChanged(currentStatus);
        mediaStatusChanged();
    }
    if (currentState != d->mCurrentPlaybackState) {
        d->mCurrentPlaybackState = currentState;
        Q_EMIT playbackStateChanged(currentState);
        playerStateChanged();
    }
}

void AudioWrapper::queueStatusChanged()
{
    if (!d->mQueuedStatusUpdate) {
        QTimer::singleShot(0, this, &AudioWrapper::notifyStatusChanges);
        d->mQueuedStatusUpdate = true;
    }
}

void AudioWrapper::playerStateSignalChanges(QMediaPlayer::PlaybackState currentState)
{
    if (currentState != d->mCurrentPlaybackState) {
        d->mCurrentPlaybackState = currentState;
        Q_EMIT playbackStateChanged(currentState);
        playerStateChanged();
    }
}

void AudioWrapper::mediaStatusSignalChanges(QMediaPlayer::MediaStatus currentStatus)
{
    if (currentStatus != d->mCurrentMediaStatus) {
        d->mCurrentMediaStatus = currentStatus;
        Q_EMIT statusChanged(currentStatus);
        mediaStatusChanged();
    }
}

void AudioWrapper::playerStateChanged()
{
    qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::playerStateChanged" << static_cast<int>(d->mPlayer.playbackState());

    switch (d->mPlayer.playbackState()) {
    case DragonPlayer::PlaybackState::StoppedState:
        Q_EMIT stopped();
        d->mPowerInterface.setPreventSleep(false);
        break;
    case DragonPlayer::PlaybackState::PlayingState:
        Q_EMIT playing();
        d->mPowerInterface.setPreventSleep(true);
        break;
    case DragonPlayer::PlaybackState::PausedState:
        Q_EMIT paused();
        d->mPowerInterface.setPreventSleep(false);
        break;
    }
}

void AudioWrapper::saveUndoPosition(qint64 position)
{
    qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::saveUndoPosition" << position;

    d->mUndoSavedPosition = position;
}

void AudioWrapper::restoreUndoPosition()
{
    qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::restoreUndoPosition";

    d->mSavedPosition = d->mUndoSavedPosition;
}

void AudioWrapper::savePosition(qint64 position)
{
    qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::savePosition" << position;

    if (!d->mSavedPosition) {
        d->mSavedPosition = position;
        qCDebug(orgKdeElisaPlayerDragon) << "AudioWrapper::savePosition" << "restore old position" << *d->mSavedPosition;
    }
}

void AudioWrapper::playerVolumeChanged()
{
    Q_EMIT volumeChanged();
}

void AudioWrapper::playerMutedChanged()
{
    Q_EMIT mutedChanged(muted());
}

#include "moc_audiowrapper.cpp"
