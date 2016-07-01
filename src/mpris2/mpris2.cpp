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

#include "mpris2.h"
#include "mediaplayer2.h"
#include "mediaplayer2player.h"
#include "mediaplayer2tracklist.h"
#include "playlistcontroler.h"

#include <QDBusConnection>
#include <QDir>
#include <QAbstractItemModel>

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
        m_mp2tl = new MediaPlayer2Tracklist(m_playListModel, this);
        m_mp2p = new MediaPlayer2Player(m_playListControler, m_mp2tl, this);

        QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/mpris/MediaPlayer2"), this, QDBusConnection::ExportAdaptors);

        connect(m_mp2, SIGNAL(raisePMC()), this, SIGNAL(raisePMC()));
    }
}

Mpris2::~Mpris2()
{
}

QString Mpris2::playerName() const
{
    return m_playerName;
}

QAbstractItemModel *Mpris2::playListModel() const
{
    return m_playListModel;
}

PlayListControler *Mpris2::playListControler() const
{
    return m_playListControler;
}

void Mpris2::setPlayerName(QString playerName)
{
    if (m_playerName == playerName)
        return;

    m_playerName = playerName;

    if (m_playListModel && m_playListControler && !m_playerName.isEmpty()) {
        if (!m_mp2) {
            initDBusService();
        }
    }

    emit playerNameChanged();
}

void Mpris2::setPlayListModel(QAbstractItemModel *playListModel)
{
    if (m_playListModel == playListModel)
        return;

    m_playListModel = playListModel;

    if (m_playListModel && m_playListControler && !m_playerName.isEmpty()) {
        if (!m_mp2) {
            initDBusService();
        }
    }

    emit playListModelChanged();
}

void Mpris2::setPlayListControler(PlayListControler *playListControler)
{
    if (m_playListControler == playListControler)
        return;

    m_playListControler = playListControler;

    if (m_playListModel && m_playListControler && !m_playerName.isEmpty()) {
        if (!m_mp2) {
            initDBusService();
        }
    }

    emit playListControlerChanged();
}
