/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "upnplistener.h"

#include "databaseinterface.h"
#include "upnpdiscoverallmusic.h"
#include "upnpdiscoveryresult.h"
#include "upnpssdpengine.h"

class UpnpListenerPrivate
{
public:

    UpnpDiscoverAllMusic mUpnpManager;

    UpnpSsdpEngine mSsdpEngine;

};

UpnpListener::UpnpListener(QObject *parent) : QObject(parent), d(new UpnpListenerPrivate)
{
    d->mSsdpEngine.initialize();
    d->mSsdpEngine.searchAllUpnpDevice();

    d->mUpnpManager.setDeviceId(QStringLiteral("urn:schemas-upnp-org:service:ContentDirectory:1"));

    connect(&d->mSsdpEngine, &UpnpSsdpEngine::newService,
            &d->mUpnpManager, &UpnpDiscoverAllMusic::newDevice);
    connect(&d->mSsdpEngine, &UpnpSsdpEngine::removedService,
            &d->mUpnpManager, &UpnpDiscoverAllMusic::removedDevice);
}

UpnpListener::~UpnpListener()
= default;

DatabaseInterface *UpnpListener::databaseInterface() const
{
    return nullptr;
}

void UpnpListener::setDatabaseInterface(DatabaseInterface *model)
{
    d->mUpnpManager.setAlbumDatabase(model);

    Q_EMIT databaseInterfaceChanged();
}

void UpnpListener::applicationAboutToQuit()
{
}


#include "moc_upnplistener.cpp"
