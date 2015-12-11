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

#include "viewpagesmodel.h"

#include "upnpdevicedescription.h"
#include "upnpdiscoveryresult.h"
#include "upnpdevicedescriptionparser.h"
#include "upnpcontrolcontentdirectory.h"

#include "remoteserverentry.h"

#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include <QtCore/QScopedPointer>
#include <QtCore/QDebug>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class ViewPagesModelPrivate
{
public:

    QList<QSharedPointer<UpnpDiscoveryResult> > mAllDevices;

    QHash<QString, QSharedPointer<UpnpDiscoveryResult> > mAllDeviceDiscoveryResults;

    QHash<QString, QSharedPointer<UpnpDeviceDescription> > mAllHostsDescription;

    QHash<QString, QSharedPointer<UpnpDeviceDescriptionParser> > mDeviceDescriptionParsers;

    QList<QString> mAllHostsUUID;

    QList<QSharedPointer<RemoteServerEntry> > mRemoteServers;

    QNetworkAccessManager mNetworkAccess;

    bool mWithPlaylist;
};

ViewPagesModel::ViewPagesModel(QObject *parent)
    : QAbstractListModel(parent), d(new ViewPagesModelPrivate)
{
    d->mWithPlaylist = true;
}

ViewPagesModel::~ViewPagesModel()
{
    delete d;
}

int ViewPagesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    if (!d->mWithPlaylist) {
        return d->mAllDevices.size();
    }

    return d->mAllDevices.size() + 1;
}

QVariant ViewPagesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= d->mAllDevices.size() + 1) {
        return QVariant();
    }

    if (!d->mWithPlaylist && index.row() >= d->mAllDevices.size()) {
        return QVariant();
    }

    if (role < ColumnsRoles::NameRole || role > ColumnsRoles::UDNRole) {
        return QVariant();
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    if (index.row() < d->mAllDevices.size()) {
        switch(convertedRole)
        {
        case ColumnsRoles::NameRole:
            return d->mAllHostsDescription[d->mAllHostsUUID[index.row()]]->friendlyName();
        case ColumnsRoles::UDNRole:
            return d->mAllHostsUUID[index.row()];
        }
    } else if (d->mWithPlaylist) {
        switch(convertedRole)
        {
        case ColumnsRoles::NameRole:
            return QStringLiteral("Play List");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QHash<int, QByteArray> ViewPagesModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::NameRole)] = "name";
    roles[static_cast<int>(ColumnsRoles::UDNRole)] = "udn";

    return roles;
}

RemoteServerEntry* ViewPagesModel::remoteServer(int index) const
{
    if (index < 0 || index > d->mRemoteServers.size() - 1) {
        return nullptr;
    }

    return d->mRemoteServers.at(index).data();
}

RemoteServerEntry *ViewPagesModel::remoteServer(QModelIndex index) const
{
    return remoteServer(index.row());
}

void ViewPagesModel::setWithPlaylist(bool value)
{
    d->mWithPlaylist = value;
    Q_EMIT withPlaylistChanged();
}

bool ViewPagesModel::withPlaylist() const
{
    return d->mWithPlaylist;
}

void ViewPagesModel::newDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery)
{
    if (serviceDiscovery->nt() == QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1")) {
        const QString &deviceUuid = serviceDiscovery->usn().mid(5, 36);
        if (!d->mAllDeviceDiscoveryResults.contains(deviceUuid)) {
            beginInsertRows(QModelIndex(), d->mAllDevices.size(), d->mAllDevices.size());

            d->mAllDevices.append(serviceDiscovery);
            d->mAllDeviceDiscoveryResults[deviceUuid] = serviceDiscovery;

            const QString &decodedUdn(deviceUuid);

            d->mAllHostsUUID.push_back(decodedUdn);
            d->mAllHostsDescription[decodedUdn].reset(new UpnpDeviceDescription);
            d->mAllHostsDescription[decodedUdn]->setUDN(decodedUdn);

            connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::friendlyNameChanged, this, &ViewPagesModel::deviceDescriptionChanged);

            d->mDeviceDescriptionParsers[decodedUdn].reset(new UpnpDeviceDescriptionParser(&d->mNetworkAccess, d->mAllHostsDescription[decodedUdn]));

            d->mRemoteServers.push_back(QSharedPointer<RemoteServerEntry>(new RemoteServerEntry(d->mAllHostsDescription[decodedUdn],
                                                                                                d->mDeviceDescriptionParsers[decodedUdn],
                                                                                                this)));

            connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, d->mDeviceDescriptionParsers[decodedUdn].data(), &UpnpDeviceDescriptionParser::finishedDownload);
            connect(d->mDeviceDescriptionParsers[decodedUdn].data(), &UpnpDeviceDescriptionParser::descriptionParsed, this, &ViewPagesModel::descriptionParsed);

            d->mDeviceDescriptionParsers[decodedUdn]->downloadDeviceDescription(QUrl(serviceDiscovery->location()));

            endInsertRows();
        }
    }
}

void ViewPagesModel::removedDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery)
{
    if (serviceDiscovery->nt() == QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1")) {
        qDebug() << "nt" << serviceDiscovery->nt();
        qDebug() << "usn" << serviceDiscovery->usn();
    }
}

void ViewPagesModel::deviceDescriptionChanged(const QString &uuid)
{
    int deviceIndex = d->mAllHostsUUID.indexOf(uuid);

    if (deviceIndex != -1) {
        Q_EMIT dataChanged(index(deviceIndex), index(deviceIndex));
    }
}

void ViewPagesModel::descriptionParsed(const QString &UDN)
{
    d->mDeviceDescriptionParsers.remove(UDN.mid(5));
}


#include "moc_viewpagesmodel.cpp"
