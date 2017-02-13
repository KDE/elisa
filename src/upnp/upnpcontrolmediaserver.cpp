/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "upnpcontrolmediaserver.h"
#include "upnpcontrolconnectionmanager.h"

#include <QDebug>

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
{
    delete d;
}

bool UpnpControlMediaServer::hasAVTransport() const
{
    return d->mHasAVTransport;
}

/*void UpnpControlMediaServer::parseDeviceDescription(QIODevice *deviceDescriptionContent, const QString &fallBackURLBase)
{
    UpnpControlAbstractDevice::parseDeviceDescription(deviceDescriptionContent, fallBackURLBase);

    auto servicesList(servicesName());

    d->mHasAVTransport = servicesList.contains(QStringLiteral("urn:schemas-upnp-org:service:AVTransport:1.0"));
    Q_EMIT hasAVTransportChanged();
}*/

#include "moc_upnpcontrolmediaserver.cpp"
