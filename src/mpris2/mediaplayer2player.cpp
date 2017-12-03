/***************************************************************************
 *   Copyright 2014 Sujith Haridasan <sujith.haridasan@kdemail.net>        *
 *   Copyright 2014 Ashish Madeti <ashishmadeti@gmail.com>                 *
 *   Copyright 2016 Matthieu Gallien <mgallien@mgallien.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "mediaplayer2player.h"
#include "mpris2.h"

#include "mediaplaylist.h"
#include "manageaudioplayer.h"
#include "managemediaplayercontrol.h"
#include "manageheaderbar.h"
#include "audiowrapper.h"

#include <QCryptographicHash>
#include <QStringList>
#include <QMetaClassInfo>
#include <QDBusMessage>
#include <QDBusConnection>

#include <QDebug>

static const double MAX_RATE = 1.0;
static const double MIN_RATE = 1.0;

MediaPlayer2Player::MediaPlayer2Player(MediaPlayList *playListControler, ManageAudioPlayer *manageAudioPlayer,
                                       ManageMediaPlayerControl *manageMediaPlayerControl, ManageHeaderBar *manageHeaderBar, AudioWrapper *audioPlayer, QObject* parent)
    : QDBusAbstractAdaptor(parent), m_playListControler(playListControler), m_manageAudioPlayer(manageAudioPlayer),
      m_manageMediaPlayerControl(manageMediaPlayerControl), m_manageHeaderBar(manageHeaderBar), m_audioPlayer(audioPlayer)
{
    if (!m_playListControler) {
        return;
    }

    connect(m_manageAudioPlayer, &ManageAudioPlayer::playerSourceChanged,
            this, &MediaPlayer2Player::playerSourceChanged, Qt::QueuedConnection);
    connect(m_manageMediaPlayerControl, &ManageMediaPlayerControl::playControlEnabledChanged,
            this, &MediaPlayer2Player::playControlEnabledChanged);
    connect(m_manageMediaPlayerControl, &ManageMediaPlayerControl::skipBackwardControlEnabledChanged,
            this, &MediaPlayer2Player::skipBackwardControlEnabledChanged);
    connect(m_manageMediaPlayerControl, &ManageMediaPlayerControl::skipForwardControlEnabledChanged,
            this, &MediaPlayer2Player::skipForwardControlEnabledChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::playerPlaybackStateChanged,
            this, &MediaPlayer2Player::playerPlaybackStateChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::playerIsSeekableChanged,
            this, &MediaPlayer2Player::playerIsSeekableChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::playerPositionChanged,
            this, &MediaPlayer2Player::audioPositionChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::audioDurationChanged,
            this, &MediaPlayer2Player::audioDurationChanged);
    connect(m_audioPlayer, &AudioWrapper::volumeChanged,
            this, &MediaPlayer2Player::playerVolumeChanged);

    m_mediaPlayerPresent = 1;
}

MediaPlayer2Player::~MediaPlayer2Player()
= default;

QString MediaPlayer2Player::PlaybackStatus() const
{
    QString result;

    if (!m_playListControler) {
        result = QStringLiteral("Stopped");
        return result;
    }

    if (m_manageAudioPlayer->playerPlaybackState() == ManageAudioPlayer::StoppedState) {
        result = QStringLiteral("Stopped");
    } else if (m_manageAudioPlayer->playerPlaybackState() == ManageAudioPlayer::PlayingState) {
        result = QStringLiteral("Playing");
    } else {
        result = QStringLiteral("Paused");
    }

    return result;
}

bool MediaPlayer2Player::CanGoNext() const
{
    return m_canGoNext;
}

void MediaPlayer2Player::Next() const
{
    emit next();

    if (m_playListControler) {
        m_playListControler->skipNextTrack();
    }
}

bool MediaPlayer2Player::CanGoPrevious() const
{
    return m_canGoPrevious;
}

void MediaPlayer2Player::Previous() const
{
    emit previous();

    if (m_playListControler) {
        m_playListControler->skipPreviousTrack();
    }
}

bool MediaPlayer2Player::CanPause() const
{
    return !m_canPlay;
}

void MediaPlayer2Player::Pause() const
{
    if (m_playListControler) {
        m_manageAudioPlayer->playPause();
    }
}

void MediaPlayer2Player::PlayPause()
{
    emit playPause();

    if (m_playListControler) {
        m_manageAudioPlayer->playPause();
    }
}

void MediaPlayer2Player::Stop() const
{
    emit stop();
}

bool MediaPlayer2Player::CanPlay() const
{
    return m_canPlay;
}

void MediaPlayer2Player::Play() const
{
    if (m_playListControler) {
        m_manageAudioPlayer->playPause();
    }
}

double MediaPlayer2Player::Volume() const
{
    return m_volume;
}

void MediaPlayer2Player::setVolume(double volume)
{
    m_volume= qBound(0.0, volume, 1.0);
    emit volumeChanged(m_volume);

    m_audioPlayer->setVolume(100 * m_volume);

    signalPropertiesChange(QStringLiteral("Volume"), Volume());
}

QVariantMap MediaPlayer2Player::Metadata() const
{
    return m_metadata;
}

qlonglong MediaPlayer2Player::Position() const
{
    return m_position;
}

void MediaPlayer2Player::setPropertyPosition(int newPositionInMs)
{
    m_position = qlonglong(newPositionInMs) * 1000;
    signalPropertiesChange(QStringLiteral("Position"), Position());

    Q_EMIT Seeked(m_position);
}

double MediaPlayer2Player::Rate() const
{
    return m_rate;
}

void MediaPlayer2Player::setRate(double newRate)
{
    if (newRate == 0) {
        Pause();
    } else {
        m_rate = qBound(MinimumRate(), newRate, MaximumRate());
        emit rateChanged(m_rate);

        signalPropertiesChange(QStringLiteral("Rate"), Rate());
    }
}

double MediaPlayer2Player::MinimumRate() const
{
    return MIN_RATE;
}

double MediaPlayer2Player::MaximumRate() const
{
    return MAX_RATE;
}

bool MediaPlayer2Player::CanSeek() const
{
    return m_playerIsSeekableChanged;
}

bool MediaPlayer2Player::CanControl() const
{
    return true;
}

void MediaPlayer2Player::Seek(qlonglong Offset) const
{
    if (mediaPlayerPresent()) {
        int offset = (m_position + Offset) / 1000;
        m_manageAudioPlayer->playerSeek(offset);
    }
}

void MediaPlayer2Player::emitSeeked(int pos)
{
    emit Seeked(qlonglong(pos) * 1000);
}

void MediaPlayer2Player::SetPosition(const QDBusObjectPath &trackId, qlonglong pos) const
{
    if (trackId.path() == m_currentTrackId) {
        m_manageAudioPlayer->playerSeek(pos / 1000);
    }
}

void MediaPlayer2Player::OpenUri(const QString &uri) const
{
    Q_UNUSED(uri);
}

void MediaPlayer2Player::playerSourceChanged()
{
    if (!m_playListControler) {
        return;
    }

    setCurrentTrack(m_manageAudioPlayer->playListPosition());
}

void MediaPlayer2Player::playControlEnabledChanged()
{
    if (!m_playListControler) {
        return;
    }

    m_canPlay = m_manageMediaPlayerControl->playControlEnabled();

    signalPropertiesChange(QStringLiteral("CanPause"), CanPause());
    signalPropertiesChange(QStringLiteral("CanPlay"), CanPlay());
}

void MediaPlayer2Player::skipBackwardControlEnabledChanged()
{
    if (!m_playListControler) {
        return;
    }

    m_canGoPrevious = m_manageMediaPlayerControl->skipBackwardControlEnabled();

    signalPropertiesChange(QStringLiteral("CanGoPrevious"), CanGoPrevious());
}

void MediaPlayer2Player::skipForwardControlEnabledChanged()
{
    if (!m_playListControler) {
        return;
    }

    m_canGoNext = m_manageMediaPlayerControl->skipForwardControlEnabled();

    signalPropertiesChange(QStringLiteral("CanGoNext"), CanGoNext());
}

void MediaPlayer2Player::playerPlaybackStateChanged()
{
    signalPropertiesChange(QStringLiteral("PlaybackStatus"), PlaybackStatus());

    playerIsSeekableChanged();
}

void MediaPlayer2Player::playerIsSeekableChanged()
{
    m_playerIsSeekableChanged = m_manageAudioPlayer->playerIsSeekable();

    signalPropertiesChange(QStringLiteral("CanSeek"), CanSeek());
}

void MediaPlayer2Player::audioPositionChanged()
{
    setPropertyPosition(m_manageAudioPlayer->playerPosition());
}

void MediaPlayer2Player::audioDurationChanged()
{
    m_metadata = getMetadataOfCurrentTrack();
    signalPropertiesChange(QStringLiteral("Metadata"), Metadata());

    skipBackwardControlEnabledChanged();
    skipForwardControlEnabledChanged();
    playerPlaybackStateChanged();
    playerIsSeekableChanged();
    setPropertyPosition(m_manageAudioPlayer->playerPosition());
}

void MediaPlayer2Player::playerVolumeChanged()
{
    setVolume(m_audioPlayer->volume() / 100.0);
}

int MediaPlayer2Player::currentTrack() const
{
    return m_manageAudioPlayer->playListPosition();
}

void MediaPlayer2Player::setCurrentTrack(int newTrackPosition)
{
    m_currentTrack = m_manageAudioPlayer->playerSource().toString();
    m_currentTrackId = QDBusObjectPath(QStringLiteral("/org/kde/elisa/playlist/") + QString::number(newTrackPosition)).path();
}

QVariantMap MediaPlayer2Player::getMetadataOfCurrentTrack()
{
    auto result = QVariantMap();

    result[QStringLiteral("mpris:trackid")] = QVariant::fromValue<QDBusObjectPath>(QDBusObjectPath(m_currentTrackId));
    result[QStringLiteral("mpris:length")] = qlonglong(m_manageAudioPlayer->audioDuration()) * 1000;
    //convert milli-seconds into micro-seconds
    result[QStringLiteral("xesam:title")] = m_manageHeaderBar->title();
    result[QStringLiteral("xesam:url")] = m_manageAudioPlayer->playerSource().toString();
    result[QStringLiteral("xesam:album")] = m_manageHeaderBar->album();
    result[QStringLiteral("xesam:artist")] = QStringList{m_manageHeaderBar->artist().toString()};
    result[QStringLiteral("mpris:artUrl")] = m_manageHeaderBar->image().toString();

    return result;
}

int MediaPlayer2Player::mediaPlayerPresent() const
{
    return m_mediaPlayerPresent;
}

void MediaPlayer2Player::setMediaPlayerPresent(int status)
{
    if (m_mediaPlayerPresent != status) {
        m_mediaPlayerPresent = status;

        signalPropertiesChange(QStringLiteral("CanGoNext"), CanGoNext());
        signalPropertiesChange(QStringLiteral("CanGoPrevious"), CanGoPrevious());
        signalPropertiesChange(QStringLiteral("CanPause"), CanPause());
        signalPropertiesChange(QStringLiteral("CanPlay"), CanPlay());
    }
}

void MediaPlayer2Player::signalPropertiesChange(const QString &property, const QVariant &value)
{
    QVariantMap properties;
    properties[property] = value;
    const int ifaceIndex = metaObject()->indexOfClassInfo("D-Bus Interface");
    QDBusMessage msg = QDBusMessage::createSignal(QStringLiteral("/org/mpris/MediaPlayer2"),
        QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("PropertiesChanged"));

    msg << QLatin1String(metaObject()->classInfo(ifaceIndex).value());
    msg << properties;
    msg << QStringList();

    QDBusConnection::sessionBus().send(msg);
}

#include "moc_mediaplayer2player.cpp"
