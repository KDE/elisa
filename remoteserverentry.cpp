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

#include "remoteserverentry.h"

#include "upnpcontrolcontentdirectory.h"
#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"
#include "upnpdevicedescriptionparser.h"

#include <QtCore/QDebug>

RemoteServerEntry::RemoteServerEntry(QSharedPointer<UpnpDeviceDescription> device,
                                     QSharedPointer<UpnpDeviceDescriptionParser> deviceParser,
                                     QObject *parent)
    : QObject(parent), mIsReady(false), mDevice(device), mDeviceParser(deviceParser)
{
    connect(mDeviceParser.data(), &UpnpDeviceDescriptionParser::descriptionParsed, this, &RemoteServerEntry::descriptionParsed);
}

UpnpControlContentDirectory *RemoteServerEntry::contentDirectory() const
{
    QScopedPointer<UpnpControlContentDirectory> newService(new UpnpControlContentDirectory());

    const auto &allServices = mDevice->servicesName();
    auto serviceIndex = allServices.indexOf(QStringLiteral("urn:schemas-upnp-org:service:ContentDirectory:1"));
    if (serviceIndex == -1) {
        return nullptr;
    }

    newService->setDescription(mDevice->serviceByIndex(serviceIndex).data());

    return newService.take();
}

void RemoteServerEntry::setContentDirectory(UpnpControlContentDirectory *value)
{
    Q_UNUSED(value);
}

bool RemoteServerEntry::isReady() const
{
    return mIsReady;
}

void RemoteServerEntry::setIsReady(bool value)
{
    mIsReady = value;
    Q_EMIT isReadyChanged();
}

void RemoteServerEntry::descriptionParsed(const QString &UDN)
{
    Q_UNUSED(UDN);
    mDeviceParser.clear();
    setIsReady(true);
    Q_EMIT contentDirectoryChanged();
}

#include "moc_remoteserverentry.cpp"
