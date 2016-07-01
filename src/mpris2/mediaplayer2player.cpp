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

#include "playlistcontroler.h"
#include "mediaplayer2tracklist.h"

#include <QCryptographicHash>
#include <QStringList>
#include <QMetaClassInfo>
#include <QDBusMessage>
#include <QDBusConnection>

static const double MAX_RATE = 32.0;
static const double MIN_RATE = 0.0;

MediaPlayer2Player::MediaPlayer2Player(PlayListControler *playListControler,
                                       MediaPlayer2Tracklist *playerPlayList,
                                       QObject* parent)
    : QDBusAbstractAdaptor(parent), m_playListControler(playListControler),
      m_playerPlayList(playerPlayList)
{
    if (!m_playListControler) {
        return;
    }

    connect(m_playListControler, &PlayListControler::playerSourceChanged,
            this, &MediaPlayer2Player::playerSourceChanged);
    connect(m_playListControler, &PlayListControler::playControlEnabledChanged,
            this, &MediaPlayer2Player::playControlEnabledChanged);
    connect(m_playListControler, &PlayListControler::skipBackwardControlEnabledChanged,
            this, &MediaPlayer2Player::skipBackwardControlEnabledChanged);
    connect(m_playListControler, &PlayListControler::skipForwardControlEnabledChanged,
            this, &MediaPlayer2Player::skipForwardControlEnabledChanged);
    connect(m_playListControler, &PlayListControler::musicPlayingChanged,
            this, &MediaPlayer2Player::musicPlayingChanged);
    connect(m_playListControler, &PlayListControler::musicPlayerStoppedChanged,
            this, &MediaPlayer2Player::musicPlayerStoppedChanged);
    connect(m_playListControler, &PlayListControler::currentTrackPositionChanged,
            this, &MediaPlayer2Player::currentTrackPositionChanged);

    m_mediaPlayerPresent = 1;
}

MediaPlayer2Player::~MediaPlayer2Player()
{
}

QString MediaPlayer2Player::PlaybackStatus() const
{
    if (!m_playListControler) {
        return QLatin1String("Stopped");
    }

    if (m_playListControler->musicPlayerStopped()) {
        return QStringLiteral("Stopped");
    } else if (m_playListControler->musicPlaying()) {
        return QStringLiteral("Playing");
    } else {
        return QStringLiteral("Paused");
    }
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
        m_playListControler->playPause();
    }
}

void MediaPlayer2Player::PlayPause()
{
    emit playPause();

    if (m_playListControler) {
        m_playListControler->playPause();
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
        m_playListControler->playPause();
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
    m_position = qlonglong(newPositionInMs)*1000;
    //PMC stores postion in milli-seconds, Mpris likes it in micro-seconds
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
    return mediaPlayerPresent();
}

bool MediaPlayer2Player::CanControl() const
{
    return true;
}

void MediaPlayer2Player::Seek(qlonglong Offset) const
{
    if (mediaPlayerPresent()) {
        //The seekTo function (to which this signal is linked to) accepts offset in milliseconds
        int offset = Offset/1000;
        emit seek(offset);
    }
}

void MediaPlayer2Player::emitSeeked(int pos)
{
    emit Seeked(qlonglong(pos)*1000);
}

void MediaPlayer2Player::SetPosition(const QDBusObjectPath& trackId, qlonglong pos)
{
    if (trackId.path() == m_playerPlayList->currentTrackId()) {
        emit seek((pos - Position()) / 1000);
    }
}

void MediaPlayer2Player::OpenUri(QString uri) const
{
    QUrl url(uri);
    if (url.isLocalFile()) {
        emit playUrl(url);
    }
}

void MediaPlayer2Player::playerSourceChanged()
{
    if (!m_playListControler) {
        return;
    }

    setCurrentTrack(m_playListControler->playerSource().toString());
}

void MediaPlayer2Player::playControlEnabledChanged()
{
    if (!m_playListControler) {
        return;
    }

    m_canPlay = m_playListControler->playControlEnabled();

    signalPropertiesChange(QStringLiteral("CanPause"), CanPause());
    signalPropertiesChange(QStringLiteral("CanPlay"), CanPlay());
}

void MediaPlayer2Player::skipBackwardControlEnabledChanged()
{
    if (!m_playListControler) {
        return;
    }

    m_canGoPrevious = m_playListControler->skipBackwardControlEnabled();

    signalPropertiesChange(QStringLiteral("CanGoPrevious"), CanGoPrevious());
}

void MediaPlayer2Player::skipForwardControlEnabledChanged()
{
    if (!m_playListControler) {
        return;
    }

    m_canGoNext = m_playListControler->skipForwardControlEnabled();

    signalPropertiesChange(QStringLiteral("CanGoNext"), CanGoNext());
}

void MediaPlayer2Player::musicPlayingChanged()
{
    signalPropertiesChange(QStringLiteral("PlaybackStatus"), PlaybackStatus());
}

void MediaPlayer2Player::musicPlayerStoppedChanged()
{
    signalPropertiesChange(QStringLiteral("PlaybackStatus"), PlaybackStatus());
}

void MediaPlayer2Player::currentTrackPositionChanged()
{
    m_playerPlayList->setCurrentIndex(m_playListControler->currentTrackPosition());
    m_metadata = m_playerPlayList->getMetadataOf(m_currentTrack);

    signalPropertiesChange(QStringLiteral("Metadata"), Metadata());
}

QString MediaPlayer2Player::currentTrack() const
{
    return m_currentTrack;
}

void MediaPlayer2Player::setCurrentTrack(QString newTrack)
{
    m_currentTrack = newTrack;
    m_metadata = m_playerPlayList->getMetadataOf(m_currentTrack);

    signalPropertiesChange(QStringLiteral("Metadata"), Metadata());
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
        signalPropertiesChange(QStringLiteral("CanSeek"), CanSeek());
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
