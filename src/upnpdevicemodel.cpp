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

#include "upnpdevicemodel.h"
#include "upnpssdpengine.h"
#include "upnpdevicedescriptionparser.h"

#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QtXml/QDomDocument>

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QDebug>
#include <QtCore/QPointer>
#include <QtCore/QUrl>
#include <QtCore/QIODevice>

class UpnpDeviceModelPrivate
{
public:

    QHash<QString, QSharedPointer<UpnpDiscoveryResult> > mAllHosts;

    QHash<QString, QSharedPointer<UpnpDeviceDescription> > mAllHostsDescription;

    QHash<QString, QSharedPointer<UpnpDeviceDescriptionParser> > mDeviceDescriptionParsers;

    QList<QString> mAllHostsUUID;

    UpnpSsdpEngine *mListenner;

    QNetworkAccessManager mNetworkAccess;

};

UpnpDeviceModel::UpnpDeviceModel(QObject *parent)
    : QAbstractListModel(parent), d(new UpnpDeviceModelPrivate)
{
    d->mListenner = nullptr;
}

UpnpDeviceModel::~UpnpDeviceModel()
{
    delete d;
}

int UpnpDeviceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.parent().isValid()) {
        return 0;
    } else {
        return d->mAllHosts.size();
    }
}

QHash<int, QByteArray> UpnpDeviceModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::NameRole)] = "name";
    roles[static_cast<int>(ColumnsRoles::TypeRole)] = "upnpType";
    roles[static_cast<int>(ColumnsRoles::uuidRole)] = "uuid";

    return roles;
}

Qt::ItemFlags UpnpDeviceModel::flags(const QModelIndex &index) const
{
    if (index.parent().isValid()) {
        return Qt::NoItemFlags;
    }

    if (index.column() != 0) {
        return Qt::NoItemFlags;
    }

    if (index.row() < 0 || index.row() > d->mAllHosts.size() - 1) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant UpnpDeviceModel::data(const QModelIndex &index, int role) const
{
    if (index.parent().isValid()) {
        return QVariant();
    }

    if (index.column() != 0) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() > d->mAllHosts.size() - 1) {
        return QVariant();
    }

    if (!d->mAllHostsDescription[d->mAllHostsUUID[index.row()]]) {
        return QVariant();
    }

    switch(role)
    {
    case ColumnsRoles::NameRole:
        return d->mAllHostsDescription[d->mAllHostsUUID[index.row()]]->friendlyName();
    case ColumnsRoles::TypeRole:
        return d->mAllHostsDescription[d->mAllHostsUUID[index.row()]]->deviceType();
    case ColumnsRoles::uuidRole:
        return d->mAllHostsUUID[index.row()];
    }

    return QVariant();
}

UpnpSsdpEngine *UpnpDeviceModel::listenner() const
{
    return d->mListenner;
}

void UpnpDeviceModel::setListenner(UpnpSsdpEngine *listenner)
{
    if (d->mListenner) {
        disconnect(d->mListenner);
    }
    d->mListenner = listenner;
    connect(d->mListenner, &UpnpSsdpEngine::newService, this, &UpnpDeviceModel::newDevice);
    connect(d->mListenner, &UpnpSsdpEngine::removedService, this, &UpnpDeviceModel::removedDevice);
}

UpnpDeviceDescription *UpnpDeviceModel::getDeviceDescription(const QString &uuid) const
{
    int deviceIndex = d->mAllHostsUUID.indexOf(uuid);

    if (deviceIndex == -1) {
        return nullptr;
    }

    return d->mAllHostsDescription[uuid].data();
}

QVariant UpnpDeviceModel::get(int row, const QString &roleName) const
{
    int role = Qt::DisplayRole;

    if (roleName == QStringLiteral("name")) {
        role = ColumnsRoles::NameRole;
    }
    if (roleName == QStringLiteral("upnpType")) {
        role = ColumnsRoles::TypeRole;
    }
    if (roleName == QStringLiteral("uuid")) {
        role = ColumnsRoles::uuidRole;
    }

    return data(index(row), role);
}

void UpnpDeviceModel::newDevice(QSharedPointer<UpnpDiscoveryResult> deviceDiscovery)
{
    const QString &deviceUuid = deviceDiscovery->usn().mid(5, 36);
    if (!d->mAllHostsUUID.contains(deviceUuid) && deviceDiscovery->nt().startsWith(QStringLiteral("urn:schemas-upnp-org:device:"))) {
        beginInsertRows(QModelIndex(), d->mAllHostsUUID.size(), d->mAllHostsUUID.size());

        const QString &decodedUdn(deviceUuid);

        d->mAllHostsUUID.push_back(decodedUdn);
        d->mAllHosts[decodedUdn] = deviceDiscovery;
        d->mAllHostsDescription[decodedUdn].reset(new UpnpDeviceDescription);
        d->mAllHostsDescription[decodedUdn]->setUDN(decodedUdn);

        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::UDNChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::UPCChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::deviceTypeChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::friendlyNameChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::manufacturerChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::manufacturerURLChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::modelDescriptionChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::modelNameChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::modelNumberChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::modelURLChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::serialNumberChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);
        connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::URLBaseChanged, this, &UpnpDeviceModel::deviceDescriptionChanged);

        endInsertRows();

        d->mDeviceDescriptionParsers[decodedUdn].reset(new UpnpDeviceDescriptionParser(&d->mNetworkAccess, d->mAllHostsDescription[decodedUdn]));

        connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, d->mDeviceDescriptionParsers[decodedUdn].data(), &UpnpDeviceDescriptionParser::finishedDownload);
        connect(d->mDeviceDescriptionParsers[decodedUdn].data(), &UpnpDeviceDescriptionParser::descriptionParsed, this, &UpnpDeviceModel::descriptionParsed);

        d->mDeviceDescriptionParsers[decodedUdn]->downloadDeviceDescription(QUrl(deviceDiscovery->location()));
    }
}

void UpnpDeviceModel::removedDevice(QSharedPointer<UpnpDiscoveryResult> deviceDiscovery)
{
    genericRemovedDevice(deviceDiscovery->usn());
}

void UpnpDeviceModel::genericRemovedDevice(const QString &usn)
{
    const QString &deviceUuid = usn.mid(5, 36);
    auto deviceIndex = d->mAllHostsUUID.indexOf(deviceUuid);
    if (deviceIndex != -1) {
        beginRemoveRows(QModelIndex(), deviceIndex, deviceIndex);
        d->mAllHostsUUID.removeAt(deviceIndex);
        d->mAllHosts.remove(deviceUuid);
        d->mAllHostsDescription.remove(deviceUuid);
        endRemoveRows();
    }
}

void UpnpDeviceModel::deviceDescriptionChanged(const QString &uuid)
{
    int deviceIndex = d->mAllHostsUUID.indexOf(uuid);

    if (deviceIndex != -1) {
        Q_EMIT dataChanged(index(deviceIndex), index(deviceIndex));
    }
}

void UpnpDeviceModel::deviceInError()
{
#if 0
    if (sender()) {
        UpnpControlAbstractDevice *device = qobject_cast<UpnpControlAbstractDevice*>(sender());
        if (device) {
            genericRemovedDevice(device->UDN());
        }
    }
#endif
}

void UpnpDeviceModel::descriptionParsed(const QString &UDN)
{
    d->mDeviceDescriptionParsers.remove(UDN.mid(5));
}

#include "moc_upnpdevicemodel.cpp"
