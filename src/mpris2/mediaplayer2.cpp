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

#include "mediaplayer2.h"

#include <QCoreApplication>

#include "config-upnp-qt.h"

#if defined KF5CoreAddons_FOUND && KF5CoreAddons_FOUND
#include <KCoreAddons/KAboutData>
#endif

MediaPlayer2::MediaPlayer2(QObject* parent)
    : QDBusAbstractAdaptor(parent)
{
}

MediaPlayer2::~MediaPlayer2()
{
}

bool MediaPlayer2::CanQuit() const
{
    return true;
}

bool MediaPlayer2::CanRaise() const
{
    return true;
}
bool MediaPlayer2::HasTrackList() const
{
    return false;
}

void MediaPlayer2::Quit() const
{
    QCoreApplication::quit();
}

void MediaPlayer2::Raise() const
{
    emit raisePlayer();
}

QString MediaPlayer2::Identity() const
{
#if defined KF5CoreAddons_FOUND && KF5CoreAddons_FOUND
    return KAboutData::applicationData().displayName();
#else
    return QString();
#endif
}

QString MediaPlayer2::DesktopEntry() const
{
#if defined KF5CoreAddons_FOUND && KF5CoreAddons_FOUND
    return KAboutData::applicationData().desktopFileName();
#else
    return QString();
#endif
}

QStringList MediaPlayer2::SupportedUriSchemes() const
{
    return QStringList() << QStringLiteral("file");
}

QStringList MediaPlayer2::SupportedMimeTypes() const
{
//    KService::Ptr app = KService::serviceByDesktopName(KCmdLineArgs::aboutData()->appName());

//    if (app) {
//        return app->mimeTypes();
//    }

    return QStringList();
}

#include "moc_mediaplayer2.cpp"
