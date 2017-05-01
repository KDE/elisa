/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#if defined UPNPQT_FOUND && UPNPQT_FOUND
#include "upnp/upnplistener.h"
#endif

#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
#include "baloo/baloolistener.h"
#endif

#include "databaseinterface.h"
#include "mediaplaylist.h"
#include "file/filelistener.h"
#include "trackslistener.h"

#include <QThread>
#include <QMutex>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>

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

#if (!defined KF5Baloo_FOUND || !KF5Baloo_FOUND) && defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    FileListener mFileListener;
#endif

    DatabaseInterface mDatabaseInterface;

};

MusicListenersManager::MusicListenersManager(QObject *parent)
    : QObject(parent), d(new MusicListenersManagerPrivate)
{
    d->mDatabaseThread.start();

    d->mDatabaseInterface.moveToThread(&d->mDatabaseThread);

    connect(&d->mDatabaseInterface, &DatabaseInterface::requestsInitDone,
            this, &MusicListenersManager::databaseReady);

    const auto &localDataPaths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    auto databaseFileName = QString();
    if (!localDataPaths.isEmpty()) {
        QDir myDataDirectory;
        myDataDirectory.mkpath(localDataPaths.first());
        databaseFileName = localDataPaths.first() + QStringLiteral("/elisaDatabase.db");
    }

    QMetaObject::invokeMethod(&d->mDatabaseInterface, "init", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("listeners")), Q_ARG(QString, databaseFileName));

    connect(&d->mDatabaseInterface, &DatabaseInterface::artistAdded,
               this, &MusicListenersManager::artistAdded);
    connect(&d->mDatabaseInterface, &DatabaseInterface::albumAdded,
               this, &MusicListenersManager::albumAdded);
    connect(&d->mDatabaseInterface, &DatabaseInterface::trackAdded,
               this, &MusicListenersManager::trackAdded);
    connect(&d->mDatabaseInterface, &DatabaseInterface::artistRemoved,
               this, &MusicListenersManager::artistRemoved);
    connect(&d->mDatabaseInterface, &DatabaseInterface::albumRemoved,
               this, &MusicListenersManager::albumRemoved);
    connect(&d->mDatabaseInterface, &DatabaseInterface::trackRemoved,
               this, &MusicListenersManager::trackRemoved);
    connect(&d->mDatabaseInterface, &DatabaseInterface::artistModified,
               this, &MusicListenersManager::artistModified);
    connect(&d->mDatabaseInterface, &DatabaseInterface::albumModified,
               this, &MusicListenersManager::albumModified);
    connect(&d->mDatabaseInterface, &DatabaseInterface::trackModified,
               this, &MusicListenersManager::trackModified);

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
            this, &MusicListenersManager::applicationAboutToQuit);
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

    connect(this, &MusicListenersManager::trackRemoved, helper, &TracksListener::trackRemoved);
    connect(this, &MusicListenersManager::trackAdded, helper, &TracksListener::trackAdded);
    connect(this, &MusicListenersManager::trackModified, helper, &TracksListener::trackModified);
    connect(helper, &TracksListener::trackHasChanged, client, &MediaPlayList::trackChanged);
    connect(helper, &TracksListener::trackHasBeenRemoved, client, &MediaPlayList::trackRemoved);
    connect(helper, &TracksListener::albumAdded, client, &MediaPlayList::albumAdded);
    connect(client, &MediaPlayList::newTrackByIdInList, helper, &TracksListener::trackByIdInList);
    connect(client, &MediaPlayList::newTrackByNameInList, helper, &TracksListener::trackByNameInList);
    connect(client, &MediaPlayList::newArtistInList, helper, &TracksListener::newArtistInList);
}

void MusicListenersManager::databaseReady()
{
#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    d->mBalooListener.setDatabaseInterface(&d->mDatabaseInterface);
    d->mBalooListener.moveToThread(&d->mDatabaseThread);
    connect(this, &MusicListenersManager::applicationIsTerminating,
            &d->mBalooListener, &BalooListener::applicationAboutToQuit, Qt::BlockingQueuedConnection);
    connect(this, &MusicListenersManager::databaseIsReady,
            &d->mBalooListener, &BalooListener::databaseReady);
#endif
#if defined UPNPQT_FOUND && UPNPQT_FOUND
    d->mUpnpListener.setDatabaseInterface(&d->mDatabaseInterface);
    d->mUpnpListener.moveToThread(&d->mDatabaseThread);
    connect(this, &MusicListenersManager::applicationIsTerminating,
            &d->mUpnpListener, &UpnpListener::applicationAboutToQuit, Qt::BlockingQueuedConnection);
    connect(this, &MusicListenersManager::databaseIsReady,
            &d->mUpnpListener, &UpnpListener::databaseReady);
#endif

#if (!defined KF5Baloo_FOUND || !KF5Baloo_FOUND) && defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    d->mFileListener.setDatabaseInterface(&d->mDatabaseInterface);
    d->mFileListener.moveToThread(&d->mDatabaseThread);
    connect(this, &MusicListenersManager::applicationIsTerminating,
            &d->mFileListener, &FileListener::applicationAboutToQuit, Qt::BlockingQueuedConnection);
    connect(this, &MusicListenersManager::databaseIsReady,
            &d->mFileListener, &FileListener::databaseReady);
#endif

    Q_EMIT databaseIsReady();
}

void MusicListenersManager::applicationAboutToQuit()
{
    d->mDatabaseInterface.applicationAboutToQuit();

    Q_EMIT applicationIsTerminating();

    d->mDatabaseThread.exit();
    d->mDatabaseThread.wait();
}


#include "moc_musiclistenersmanager.cpp"
