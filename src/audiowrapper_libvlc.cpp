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

    libvlc_instance_t *mInstance = nullptr;

    libvlc_media_player_t *mPlayer = nullptr;

    libvlc_media_t *mMedia = nullptr;

    QTimer mStateRefreshTimer;



};


AudioWrapper::AudioWrapper(QObject *parent) : QObject(parent), d(std::make_unique<AudioWrapperPrivate>())
{
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

    auto realVolume = static_cast<qreal>(QAudio::convertVolume(volume / 100.0, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale));
    libvlc_audio_set_volume(d->mPlayer, qRound(realVolume * 100));
}

void AudioWrapper::setSource(const QUrl &source)
{
    d->mMedia = libvlc_media_new_path(d->mInstance, source.toLocalFile().toUtf8().constData());
    if (!d->mMedia) {
        return;
    }

    d->mPlayer = libvlc_media_player_new_from_media(d->mMedia);

    Q_EMIT statusChanged(QMediaPlayer::LoadingMedia);
    Q_EMIT playbackStateChanged(QMediaPlayer::StoppedState);

    d->mStateRefreshTimer.start();

    libvlc_media_release(d->mMedia);
    d->mMedia = nullptr;
}

void AudioWrapper::setPosition(qint64 position)
{
    if (!d->mPlayer) {
        return;
    }

    qDebug() << "AudioWrapper::setPosition";

    libvlc_media_player_set_position(d->mPlayer, position);
}

void AudioWrapper::play()
{
    if (!d->mPlayer) {
        return;
    }

    qDebug() << "AudioWrapper::play";

    libvlc_media_player_play(d->mPlayer);
}

void AudioWrapper::pause()
{
    if (!d->mPlayer) {
        return;
    }

    qDebug() << "AudioWrapper::pause";

    libvlc_media_player_pause(d->mPlayer);
}

void AudioWrapper::stop()
{
    if (!d->mPlayer) {
        return;
    }

    qDebug() << "AudioWrapper::stop";

    libvlc_media_player_stop(d->mPlayer);
}

void AudioWrapper::seek(qint64 position)
{
    if (!d->mPlayer) {
        return;
    }

    libvlc_media_player_set_position(d->mPlayer, position);
}

void AudioWrapper::setAudioRole(QAudio::Role audioRole)
{
    //    d->mPlayer.setAudioRole(audioRole);
}

void AudioWrapper::playerStateChanged()
{
    if (!d->mPlayer) {
        return;
    }

    qDebug() << "AudioWrapper::playerStateChanged";

    switch(libvlc_media_player_get_state(d->mPlayer))
    {
    case libvlc_Stopped:
        qDebug() << "AudioWrapper::playerStateChanged" << "Stopped";
        Q_EMIT playbackStateChanged(QMediaPlayer::StoppedState);
        Q_EMIT stopped();
        break;
    case libvlc_Playing:
        qDebug() << "AudioWrapper::playerStateChanged" << "Playing";
        Q_EMIT playbackStateChanged(QMediaPlayer::PlayingState);
        Q_EMIT playing();
        break;
    case libvlc_Paused:
        qDebug() << "AudioWrapper::playerStateChanged" << "Paused";
        Q_EMIT playbackStateChanged(QMediaPlayer::PausedState);
        Q_EMIT paused();
        break;
    case libvlc_Opening:
        qDebug() << "AudioWrapper::playerStateChanged" << "Opening";
        Q_EMIT statusChanged(QMediaPlayer::LoadingMedia);
        break;
    case libvlc_Buffering:
        qDebug() << "AudioWrapper::playerStateChanged" << "Buffering";
        Q_EMIT statusChanged(QMediaPlayer::BufferingMedia);
        break;
    case libvlc_NothingSpecial:
        qDebug() << "AudioWrapper::playerStateChanged" << "NothingSpecial";
        break;
    case libvlc_Ended:
        qDebug() << "AudioWrapper::playerStateChanged" << "Ended";
        Q_EMIT statusChanged(QMediaPlayer::BufferedMedia);
        Q_EMIT statusChanged(QMediaPlayer::NoMedia);
        Q_EMIT statusChanged(QMediaPlayer::EndOfMedia);
        mediaIsEnded();
        break;
    case libvlc_Error:
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

void AudioWrapper::mediaIsEnded()
{
    qDebug() << "AudioWrapper::mediaIsEnded";

    d->mStateRefreshTimer.stop();

    libvlc_media_player_release(d->mPlayer);

    d->mPlayer = nullptr;
}


#include "moc_audiowrapper.cpp"
