/***************************************************************************
   SPDX-FileCopyrightText: 2014 (c) Sujith Haridasan <sujith.haridasan@kdemail.net>
   SPDX-FileCopyrightText: 2014 (c) Ashish Madeti <ashishmadeti@gmail.com>
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: GPL-3.0-or-later
 ***************************************************************************/

#include "mediaplayer2.h"

#include <QCoreApplication>

#include "config-upnp-qt.h"

#include <KCoreAddons/KAboutData>

MediaPlayer2::MediaPlayer2(QObject* parent)
    : QDBusAbstractAdaptor(parent)
{
}

MediaPlayer2::~MediaPlayer2()
= default;

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

void MediaPlayer2::Quit()
{
    QCoreApplication::quit();
}

void MediaPlayer2::Raise()
{
    Q_EMIT raisePlayer();
}

QString MediaPlayer2::Identity() const
{
    return KAboutData::applicationData().displayName();
}

QString MediaPlayer2::DesktopEntry() const
{
    return KAboutData::applicationData().desktopFileName();
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
