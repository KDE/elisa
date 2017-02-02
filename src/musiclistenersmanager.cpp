/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "musiclistenersmanager.h"

#include "config-upnp-qt.h"

#include "databaseinterface.h"
#include "mediaplaylist.h"

#if defined UPNPQT_FOUND && UPNPQT_FOUND
#include "upnp/upnplistener.h"
#endif

#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
#include "baloo/baloolistener.h"
#endif

#include "file/filelistener.h"

#include <QThread>
#include <QMutex>

#include "trackslistener.h"

class MusicListenersManagerPrivate
{
public:

    QThread mDatabaseThread;

#if defined UPNPQT_FOUND && UPNPQT_FOUND
    UpnpListener mUpnpListener;
#endif

#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    BalooListener mBalooListener;
#endif

    FileListener mFileListener;

    DatabaseInterface mDatabaseInterface;

};

MusicListenersManager::MusicListenersManager(QObject *parent)
    : QObject(parent), d(new MusicListenersManagerPrivate)
{
    d->mDatabaseThread.start();

    d->mDatabaseInterface.moveToThread(&d->mDatabaseThread);

    connect(&d->mDatabaseInterface, &DatabaseInterface::requestsInitDone,
            this, &MusicListenersManager::databaseReady);

    QMetaObject::invokeMethod(&d->mDatabaseInterface, "init", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("listeners")));

    connect(&d->mDatabaseInterface, &DatabaseInterface::artistAdded,
               this, &MusicListenersManager::artistAdded);
    connect(&d->mDatabaseInterface, &DatabaseInterface::albumAdded,
               this, &MusicListenersManager::albumAdded);
    connect(&d->mDatabaseInterface, &DatabaseInterface::trackAdded,
               this, &MusicListenersManager::trackAdded);
}

MusicListenersManager::~MusicListenersManager()
{
    delete d;
}

DatabaseInterface *MusicListenersManager::viewDatabase() const
{
    return &d->mDatabaseInterface;
}

void MusicListenersManager::subscribeForTracks(MediaPlayList *client)
{
    auto helper = new TracksListener(&d->mDatabaseInterface);

    helper->moveToThread(&d->mDatabaseThread);

    connect(helper, &TracksListener::trackChanged, client, &MediaPlayList::trackChanged);
    connect(helper, &TracksListener::albumAdded, client, &MediaPlayList::albumAdded);
    connect(client, &MediaPlayList::newTrackByIdInList, helper, &TracksListener::trackByIdInList);
    connect(client, &MediaPlayList::newTrackByNameInList, helper, &TracksListener::trackByNameInList);
    connect(client, &MediaPlayList::newArtistInList, helper, &TracksListener::newArtistInList);
    connect(&d->mDatabaseInterface, &DatabaseInterface::trackAdded, helper, &TracksListener::trackAdded);
}

void MusicListenersManager::databaseReady()
{
#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    d->mBalooListener.setDatabaseInterface(&d->mDatabaseInterface);
    d->mBalooListener.moveToThread(&d->mDatabaseThread);
#endif
#if defined UPNPQT_FOUND && UPNPQT_FOUND
    d->mUpnpListener.setDatabaseInterface(&d->mDatabaseInterface);
    d->mUpnpListener.moveToThread(&d->mDatabaseThread);
#endif

    d->mFileListener.setDatabaseInterface(&d->mDatabaseInterface);
    d->mFileListener.moveToThread(&d->mDatabaseThread);
}


#include "moc_musiclistenersmanager.cpp"
