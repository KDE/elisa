/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "audiowrapper.h"
#include "powermanagementinterface.h"

#include "qtMultimediaLogging.h"

#include <QTimer>
#include <QAudio>
#include <QAudioOutput>

#include "config-upnp-qt.h"

class AudioWrapperPrivate
{

public:

    PowerManagementInterface mPowerInterface;

    QMediaPlayer mPlayer;

#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    QAudioOutput mOutput;
#else
#define mOutput mPlayer
#define QAudioOutput QMediaPlayer
#endif

    qint64 mSavedPosition = 0.0;

    qint64 mUndoSavedPosition = 0.0;

    bool mHasSavedPosition = false;

};

AudioWrapper::AudioWrapper(QObject *parent) : QObject(parent), d(std::make_unique<AudioWrapperPrivate>())
{
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    d->mPlayer.setAudioOutput(&d->mOutput);
#endif
    connect(&d->mOutput, &QAudioOutput::mutedChanged, this, &AudioWrapper::playerMutedChanged);
    connect(&d->mOutput, &QAudioOutput::volumeChanged, this, &AudioWrapper::playerVolumeChanged);
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    connect(&d->mPlayer, &QMediaPlayer::sourceChanged, this, &AudioWrapper::sourceChanged);
    connect(&d->mPlayer, &QMediaPlayer::playbackStateChanged, this, &AudioWrapper::playbackStateChanged);
    connect(&d->mPlayer, &QMediaPlayer::playbackStateChanged, this, &AudioWrapper::playerStateChanged);
    connect(&d->mPlayer, QOverload<QMediaPlayer::Error, const QString &>::of(&QMediaPlayer::errorOccurred), this, &AudioWrapper::errorChanged);
#else
    connect(&d->mPlayer, &QMediaPlayer::mediaChanged, this, &AudioWrapper::sourceChanged);
    connect(&d->mPlayer, &QMediaPlayer::stateChanged, this, &AudioWrapper::playbackStateChanged);
    connect(&d->mPlayer, &QMediaPlayer::stateChanged, this, &AudioWrapper::playerStateChanged);
    connect(&d->mPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &AudioWrapper::errorChanged);
#endif
    connect(&d->mPlayer, &QMediaPlayer::mediaStatusChanged, this, &AudioWrapper::statusChanged);
    connect(&d->mPlayer, &QMediaPlayer::mediaStatusChanged, this, &AudioWrapper::mediaStatusChanged);
    connect(&d->mPlayer, &QMediaPlayer::durationChanged, this, &AudioWrapper::durationChanged);
    connect(&d->mPlayer, &QMediaPlayer::positionChanged, this, &AudioWrapper::positionChanged);
    connect(&d->mPlayer, &QMediaPlayer::seekableChanged, this, &AudioWrapper::seekableChanged);
}

AudioWrapper::~AudioWrapper()
{
    d->mPowerInterface.setPreventSleep(false);
}

bool AudioWrapper::muted() const
{
    return d->mOutput.isMuted();
}

qreal AudioWrapper::volume() const
{
    auto realVolume = static_cast<qreal>(d->mOutput.volume() / 100.0);
    auto userVolume = static_cast<qreal>(QAudio::convertVolume(realVolume, QAudio::LinearVolumeScale, QAudio::LogarithmicVolumeScale));

    return userVolume * 100.0;
}

QUrl AudioWrapper::source() const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    return d->mPlayer.source();
#else
    return d->mPlayer.media().request().url();
#endif
#else
    return d->mPlayer.media().canonicalUrl();
#endif
}

QMediaPlayer::Error AudioWrapper::error() const
{
    if (d->mPlayer.error() != QMediaPlayer::NoError) {
        qDebug() << "AudioWrapper::error" << d->mPlayer.errorString();
    }

    return d->mPlayer.error();
}

qint64 AudioWrapper::duration() const
{
    return d->mPlayer.duration();
}

qint64 AudioWrapper::position() const
{
    return d->mPlayer.position();
}

bool AudioWrapper::seekable() const
{
    return d->mPlayer.isSeekable();
}

QMediaPlayer::PlaybackState AudioWrapper::playbackState() const
{
    return d->mPlayer.playbackState();
}

QMediaPlayer::MediaStatus AudioWrapper::status() const
{
    return d->mPlayer.mediaStatus();
}

void AudioWrapper::setMuted(bool muted)
{
    d->mOutput.setMuted(muted);
}

void AudioWrapper::setVolume(qreal volume)
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::setVolume" << volume;

    auto realVolume = static_cast<qreal>(QAudio::convertVolume(volume / 100.0, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale));
    d->mOutput.setVolume(qRound(realVolume * 100));
}

void AudioWrapper::setSource(const QUrl &source)
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::setSource" << source;
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    d->mPlayer.setSource({source});
#else
    d->mPlayer.setMedia({source});
#endif
}

void AudioWrapper::setPosition(qint64 position)
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::setPosition" << position;

    if (d->mPlayer.duration() <= 0) {
        savePosition(position);
        return;
    }

    d->mPlayer.setPosition(position);
}

void AudioWrapper::play()
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::play";

    d->mPlayer.play();

    if (d->mHasSavedPosition) {
        qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::playerDurationSignalChanges" << "restore old position" << d->mSavedPosition;

        setPosition(d->mSavedPosition);
        d->mHasSavedPosition = false;
    }
}

void AudioWrapper::pause()
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::pause";

    d->mPlayer.pause();
}

void AudioWrapper::stop()
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::stop";

    d->mPlayer.stop();
}

void AudioWrapper::seek(qint64 position)
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::seek" << position;

    d->mPlayer.setPosition(position);
}

void AudioWrapper::mediaStatusChanged()
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::mediaStatusChanged" << d->mPlayer.mediaStatus();
}

void AudioWrapper::playerStateChanged()
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::playerStateChanged" << d->mPlayer.playbackState();

    switch(d->mPlayer.playbackState())
    {
    case QMediaPlayer::PlaybackState::StoppedState:
        Q_EMIT stopped();
        d->mPowerInterface.setPreventSleep(false);
        break;
    case QMediaPlayer::PlaybackState::PlayingState:
        Q_EMIT playing();
        d->mPowerInterface.setPreventSleep(true);
        break;
    case QMediaPlayer::PlaybackState::PausedState:
        Q_EMIT paused();
        d->mPowerInterface.setPreventSleep(false);
        break;
    }
}

void AudioWrapper::playerVolumeChanged()
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::playerVolumeChanged" << d->mOutput.volume();

    QTimer::singleShot(0, [this]() {Q_EMIT volumeChanged();});
}

void AudioWrapper::playerMutedChanged()
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::playerMutedChanged";

    QTimer::singleShot(0, [this]() {Q_EMIT mutedChanged(muted());});
}

void AudioWrapper::playerStateSignalChanges(QMediaPlayer::PlaybackState newState)
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::playerStateSignalChanges" << newState;

    QMetaObject::invokeMethod(this, [this, newState]() {Q_EMIT playbackStateChanged(newState);}, Qt::QueuedConnection);
}

void AudioWrapper::mediaStatusSignalChanges(QMediaPlayer::MediaStatus newStatus)
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::mediaStatusSignalChanges" << newStatus;

    QMetaObject::invokeMethod(this, [this, newStatus]() {Q_EMIT statusChanged(newStatus);}, Qt::QueuedConnection);
}

void AudioWrapper::playerDurationSignalChanges(qint64 newDuration)
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::playerDurationSignalChanges" << newDuration;

    QMetaObject::invokeMethod(this, [this, newDuration]() {Q_EMIT durationChanged(newDuration);}, Qt::QueuedConnection);
}

void AudioWrapper::playerPositionSignalChanges(qint64 newPosition)
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::playerPositionSignalChanges" << newPosition;

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

void AudioWrapper::saveUndoPosition(qint64 position)
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::saveUndoPosition" << position;

    d->mUndoSavedPosition = position;
}

void AudioWrapper::restoreUndoPosition()
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::restoreUndoPosition";

    d->mHasSavedPosition = true;
    d->mSavedPosition = d->mUndoSavedPosition;
}

void AudioWrapper::savePosition(qint64 position)
{
    qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::savePosition" << position;

    if (!d->mHasSavedPosition) {
        d->mHasSavedPosition = true;
        d->mSavedPosition = position;
        qCDebug(orgKdeElisaPlayerQtMultimedia) << "AudioWrapper::savePosition" << "restore old position" << d->mSavedPosition;
    }
}


#include "moc_audiowrapper.cpp"
