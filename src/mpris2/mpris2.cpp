/***************************************************************************
 *   Copyright 2014 Sujith Haridasan <sujith.haridasan@kdemail.net>        *
 *   Copyright 2014 Ashish Madeti <ashishmadeti@gmail.com>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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

#include "mpris2.h"
#include "mediaplayer2.h"
#include "mediaplayer2player.h"
#include "mediaplayer2tracklist.h"

#include <QDBusConnection>
#include <QDir>
#include <unistd.h>

static const QString tmpPmcDirPath(QDir::tempPath() + QLatin1String("/plasma-mediacenter/covers/"));

Mpris2::Mpris2(QObject* parent)
    : QObject(parent)
{
}

void Mpris2::initDBusService()
{
    QString mspris2Name(QStringLiteral("org.mpris.MediaPlayer2.") + m_playerName);

    bool success = QDBusConnection::sessionBus().registerService(mspris2Name);

    // If the above failed, it's likely because we're not the first instance
    // or the name is already taken. In that event the MPRIS2 spec wants the
    // following:
    if (!success) {
        success = QDBusConnection::sessionBus().registerService(mspris2Name + QStringLiteral(".instance") + QString::number(getpid()));
    }

    if (success) {
        QDir tmpPmcDir;
        tmpPmcDir.mkpath(tmpPmcDirPath);

        m_mp2 = new MediaPlayer2(this);
        m_mp2p = new MediaPlayer2Player(this);
        m_mp2tl = new MediaPlayer2Tracklist(this);

        QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), this, QDBusConnection::ExportAdaptors);

        connect(m_mp2, SIGNAL(raisePMC()), this, SIGNAL(raisePMC()));
    }
}

Mpris2::~Mpris2()
{
}

MediaPlayer2Player* Mpris2::getMediaPlayer2Player()
{
    return m_mp2p;
}

QString Mpris2::getCurrentTrackId()
{
    if (m_mp2tl->currentIndex() != -1) {
        return m_mp2tl->currentTrackId().path();
    }

#if 0
    QSharedPointer<PmcMedia> media = SingletonFactory::instanceFor<MediaLibrary>()->mediaForUrl(m_mp2p->currentTrack());
    if (media) {
        return QStringLiteral("/org/kde/plasmamediacenter/tid_") + media->sha();
    }
#endif

    return QString(QStringLiteral("/org/mpris/MediaPlayer2/TrackList/NoTrack"));
}

QVariantMap Mpris2::getMetadataOf(const QString &url)
{
    QVariantMap metadata = getMetadataOf(url, getCurrentTrackId());
    return metadata;
}

QVariantMap Mpris2::getMetadataOf(const QString &url, const QString& trackId)
{
    QVariantMap metadata;
    /*QSharedPointer<PmcMedia> media = SingletonFactory::instanceFor<MediaLibrary>()->mediaForUrl(url);
    PmcImageCache *imageCache = SingletonFactory::instanceFor<PmcImageCache>();
    if (media) {
        metadata["mpris:trackid"] = QVariant::fromValue<QDBusObjectPath>(QDBusObjectPath(trackId));
        metadata["mpris:length"] = qlonglong(media->duration())*1000000;
        //convert seconds into micro-seconds
        metadata["xesam:title"] = media->title();
        metadata["xesam:url"] = media->url();
        metadata["xesam:album"] = media->album();
        metadata["xesam:artist"] = QStringList(media->artist());
        metadata["xesam:genre"] = QStringList(media->genre());
        if (imageCache->containsAlbumCover(media->album())) {
            if (imageCache->getImage(imageCache->imageIdForAlbumCover(media->album()))
                    .save(tmpPmcDirPath + media->album(), "PNG")) {
            metadata["mpris:artUrl"] = QString(tmpPmcDirPath + media->album());
            }
        }
    }*/

    return metadata;
}

QString Mpris2::playerName() const
{
    return m_playerName;
}

void Mpris2::setPlayerName(QString playerName)
{
    if (m_playerName == playerName)
        return;

    m_playerName = playerName;

    if (!m_mp2) {
        initDBusService();
    }

    emit playerNameChanged();
}
