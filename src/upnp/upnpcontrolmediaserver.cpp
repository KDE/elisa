/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "upnpcontrolmediaserver.h"
#include "upnpcontrolconnectionmanager.h"

class UpnpControlMediaServerPrivate
{
public:

    bool mHasAVTransport;
};

UpnpControlMediaServer::UpnpControlMediaServer(QObject *parent) : UpnpControlAbstractDevice(parent), d(new UpnpControlMediaServerPrivate)
{
    d->mHasAVTransport = false;
}

UpnpControlMediaServer::~UpnpControlMediaServer()
= default;

bool UpnpControlMediaServer::hasAVTransport() const
{
    return d->mHasAVTransport;
}

/*void UpnpControlMediaServer::parseDeviceDescription(QIODevice *deviceDescriptionContent, const QString &fallBackURLBase)
{
    UpnpControlAbstractDevice::parseDeviceDescription(deviceDescriptionContent, fallBackURLBase);

    auto servicesList(servicesName());

    d->mHasAVTransport = servicesList.contains(QLatin1String("urn:schemas-upnp-org:service:AVTransport:1.0"));
    Q_EMIT hasAVTransportChanged();
}*/

#include "moc_upnpcontrolmediaserver.cpp"
