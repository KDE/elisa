/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "audiowrapper.h"

#include <QTimer>
#include <QAudio>

#include "config-upnp-qt.h"

class AudioWrapperPrivate
{

public:

    QMediaPlayer mPlayer;

};


AudioWrapper::AudioWrapper(QObject *parent) : QObject(parent), d(std::make_unique<AudioWrapperPrivate>())
{
    connect(&d->mPlayer, &QMediaPlayer::mutedChanged, this, &AudioWrapper::playerMutedChanged);
    connect(&d->mPlayer, &QMediaPlayer::volumeChanged, this, &AudioWrapper::playerVolumeChanged);
    connect(&d->mPlayer, &QMediaPlayer::mediaChanged, this, &AudioWrapper::sourceChanged);
    connect(&d->mPlayer, &QMediaPlayer::mediaStatusChanged, this, &AudioWrapper::statusChanged);
    connect(&d->mPlayer, &QMediaPlayer::stateChanged, this, &AudioWrapper::playbackStateChanged);
    connect(&d->mPlayer, &QMediaPlayer::stateChanged, this, &AudioWrapper::playerStateChanged);
    connect(&d->mPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SIGNAL(errorChanged()));
    connect(&d->mPlayer, &QMediaPlayer::durationChanged, this, &AudioWrapper::durationChanged);
    connect(&d->mPlayer, &QMediaPlayer::positionChanged, this, &AudioWrapper::positionChanged);
    connect(&d->mPlayer, &QMediaPlayer::seekableChanged, this, &AudioWrapper::seekableChanged);
}

AudioWrapper::~AudioWrapper()
= default;

bool AudioWrapper::muted() const
{
    return d->mPlayer.isMuted();
}

qreal AudioWrapper::volume() const
{
    auto realVolume = static_cast<qreal>(d->mPlayer.volume() / 100.0);
    auto userVolume = static_cast<qreal>(QAudio::convertVolume(realVolume, QAudio::LinearVolumeScale, QAudio::LogarithmicVolumeScale));
    auto decibelVolume = static_cast<qreal>(QAudio::convertVolume(realVolume, QAudio::LinearVolumeScale, QAudio::DecibelVolumeScale));

    return userVolume * 100.0;
}

QUrl AudioWrapper::source() const
{
    return d->mPlayer.media().canonicalUrl();
}

QString AudioWrapper::error() const
{
    return d->mPlayer.errorString();
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

QAudio::Role AudioWrapper::audioRole() const
{
    return d->mPlayer.audioRole();
}

QMediaPlayer::State AudioWrapper::playbackState() const
{
    return d->mPlayer.state();
}

QMediaPlayer::MediaStatus AudioWrapper::status() const
{
    return d->mPlayer.mediaStatus();
}

void AudioWrapper::setMuted(bool muted)
{
    d->mPlayer.setMuted(muted);
}

void AudioWrapper::setVolume(qreal volume)
{
    auto realVolume = static_cast<qreal>(QAudio::convertVolume(volume / 100.0, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale));
    auto decibelVolume = static_cast<qreal>(QAudio::convertVolume(realVolume, QAudio::LinearVolumeScale, QAudio::DecibelVolumeScale));
    d->mPlayer.setVolume(qRound(realVolume * 100));
}

void AudioWrapper::setSource(const QUrl &source)
{
    d->mPlayer.setMedia({source});
}

void AudioWrapper::setPosition(qint64 position)
{
    d->mPlayer.setPosition(position);
}

void AudioWrapper::play()
{
    d->mPlayer.play();
}

void AudioWrapper::pause()
{
    d->mPlayer.pause();
}

void AudioWrapper::stop()
{
    d->mPlayer.stop();
}

void AudioWrapper::seek(int position)
{
    d->mPlayer.setPosition(position);
}

void AudioWrapper::setAudioRole(QAudio::Role audioRole)
{
    d->mPlayer.setAudioRole(audioRole);
}

void AudioWrapper::playerStateChanged()
{
    switch(d->mPlayer.state())
    {
    case QMediaPlayer::State::StoppedState:
        Q_EMIT stopped();
        break;
    case QMediaPlayer::State::PlayingState:
        Q_EMIT playing();
        break;
    case QMediaPlayer::State::PausedState:
        Q_EMIT paused();
        break;
    }
}

void AudioWrapper::playerVolumeChanged()
{
    QTimer::singleShot(0, [this]() {Q_EMIT volumeChanged();});
}

void AudioWrapper::playerMutedChanged()
{
    QTimer::singleShot(0, [this]() {Q_EMIT mutedChanged();});
}


#include "moc_audiowrapper.cpp"
