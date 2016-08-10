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

#include "upnplistener.h"

#include "databaseinterface.h"
#include "upnpdiscoverallmusic.h"
#include "upnpssdpengine.h"

class UpnpListenerPrivate
{
public:

    DatabaseInterface* mDatabaseInterface = nullptr;

    UpnpDiscoverAllMusic* mUpnpManager = nullptr;

    UpnpSsdpEngine* mSsdpEngine = nullptr;

};

UpnpListener::UpnpListener(QObject *parent) : QObject(parent), d(new UpnpListenerPrivate)
{
}

UpnpListener::~UpnpListener()
{
    delete d;
}

DatabaseInterface *UpnpListener::databaseInterface() const
{
    return d->mDatabaseInterface;
}

UpnpDiscoverAllMusic *UpnpListener::upnpManager() const
{
    return d->mUpnpManager;
}

UpnpSsdpEngine *UpnpListener::ssdpEngine() const
{
    return d->mSsdpEngine;
}

void UpnpListener::setDatabaseInterface(DatabaseInterface *model)
{
    if (d->mDatabaseInterface == model) {
        return;
    }

    if (d->mDatabaseInterface) {
        disconnect(d->mDatabaseInterface);
    }

    d->mDatabaseInterface = model;

    if (d->mDatabaseInterface) {
        //connect(this, &UpnpListener::refreshContent, &d->mFileListing, &LocalBalooFileListing::refreshContent, Qt::QueuedConnection);
        //connect(&d->mFileListing, &LocalBalooFileListing::tracksList, d->mDatabaseInterface, &DatabaseInterface::insertTracksList);

        //QMetaObject::invokeMethod(&d->mFileListing, "init", Qt::QueuedConnection);

        Q_EMIT refreshContent();
    }

    emit databaseInterfaceChanged();
}

void UpnpListener::setUpnpManager(UpnpDiscoverAllMusic *upnpManager)
{
    if (d->mUpnpManager == upnpManager) {
        return;
    }

    if (d->mUpnpManager && d->mSsdpEngine) {
        disconnect(d->mSsdpEngine, 0, d->mUpnpManager, 0);
    }

    d->mUpnpManager = upnpManager;

    if (d->mUpnpManager && d->mSsdpEngine) {
        connect(d->mSsdpEngine, &UpnpSsdpEngine::newService,
                d->mUpnpManager, &UpnpDiscoverAllMusic::newDevice);
        connect(d->mSsdpEngine, &UpnpSsdpEngine::removedService,
                d->mUpnpManager, &UpnpDiscoverAllMusic::removedDevice);
    }

    emit upnpManagerChanged();
}

void UpnpListener::setSsdpEngine(UpnpSsdpEngine *ssdpEngine)
{
    if (d->mSsdpEngine == ssdpEngine) {
        return;
    }

    if (d->mUpnpManager && d->mSsdpEngine) {
        disconnect(d->mSsdpEngine, 0, d->mUpnpManager, 0);
    }

    d->mSsdpEngine = ssdpEngine;

    if (d->mUpnpManager && d->mSsdpEngine) {
        connect(d->mSsdpEngine, &UpnpSsdpEngine::newService,
                d->mUpnpManager, &UpnpDiscoverAllMusic::newDevice);
        connect(d->mSsdpEngine, &UpnpSsdpEngine::removedService,
                d->mUpnpManager, &UpnpDiscoverAllMusic::removedDevice);
    }

    emit ssdpEngineChanged();
}


#include "moc_upnplistener.cpp"
