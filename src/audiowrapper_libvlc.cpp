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

    libvlc_instance_t *mVlcInstance = nullptr;

    libvlc_media_player_t *mp = nullptr;

    libvlc_media_t *m = nullptr;

};


AudioWrapper::AudioWrapper(QObject *parent) : QObject(parent), d(std::make_unique<AudioWrapperPrivate>())
{
    d->mVlcInstance = libvlc_new(0, nullptr);
}

AudioWrapper::~AudioWrapper()
= default;

bool AudioWrapper::muted() const
{
    return false/*d->mPlayer.isMuted()*/;
}

qreal AudioWrapper::volume() const
{
    auto realVolume = static_cast<qreal>(100/*d->mPlayer.volume()*/ / 100.0);
    auto userVolume = static_cast<qreal>(QAudio::convertVolume(realVolume, QAudio::LinearVolumeScale, QAudio::LogarithmicVolumeScale));

    return userVolume * 100.0;
}

QUrl AudioWrapper::source() const
{
    return {}/*d->mPlayer.media().canonicalUrl()*/;
}

QMediaPlayer::Error AudioWrapper::error() const
{
#if 0
    if (d->mPlayer.error() != QMediaPlayer::NoError) {
        qDebug() << "AudioWrapper::error" << d->mPlayer.errorString();
    }
#endif

    return {}/*d->mPlayer.error()*/;
}

qint64 AudioWrapper::duration() const
{
    return {}/*d->mPlayer.duration()*/;
}

qint64 AudioWrapper::position() const
{
    return {}/*d->mPlayer.position()*/;
}

bool AudioWrapper::seekable() const
{
    return {}/*d->mPlayer.isSeekable()*/;
}

QAudio::Role AudioWrapper::audioRole() const
{
    return {}/*d->mPlayer.audioRole()*/;
}

QMediaPlayer::State AudioWrapper::playbackState() const
{
    return {}/*d->mPlayer.state()*/;
}

QMediaPlayer::MediaStatus AudioWrapper::status() const
{
    return {}/*d->mPlayer.mediaStatus()*/;
}

void AudioWrapper::setMuted(bool muted)
{
//    d->mPlayer.setMuted(muted);
}

void AudioWrapper::setVolume(qreal volume)
{
    auto realVolume = static_cast<qreal>(QAudio::convertVolume(volume / 100.0, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale));
//    d->mPlayer.setVolume(qRound(realVolume * 100));
}

void AudioWrapper::setSource(const QUrl &source)
{
//    d->mPlayer.setMedia({source});
}

void AudioWrapper::setPosition(qint64 position)
{
//    d->mPlayer.setPosition(position);
}

void AudioWrapper::play()
{
//    d->mPlayer.play();
}

void AudioWrapper::pause()
{
//    d->mPlayer.pause();
}

void AudioWrapper::stop()
{
//    d->mPlayer.stop();
}

void AudioWrapper::seek(int position)
{
//    d->mPlayer.setPosition(position);
}

void AudioWrapper::setAudioRole(QAudio::Role audioRole)
{
//    d->mPlayer.setAudioRole(audioRole);
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
