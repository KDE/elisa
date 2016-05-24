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

#include "config-upnp-qt.h"

#if defined UPNPQT_FOUND && UPNPQT_FOUND
#include "upnpdevicedescription.h"
#include "upnpdiscoveryresult.h"
#include "upnpdevicedescriptionparser.h"
#include "upnpcontrolcontentdirectory.h"
#endif

#include "upnpalbummodel.h"
#include "databaseinterface.h"
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

#if defined UPNPQT_FOUND && UPNPQT_FOUND
    QList<QSharedPointer<UpnpDiscoveryResult> > mAllDevices;

    QHash<QString, QSharedPointer<UpnpDiscoveryResult> > mAllDeviceDiscoveryResults;

    QHash<QString, QSharedPointer<UpnpDeviceDescription> > mAllHostsDescription;

    QHash<QString, QSharedPointer<UpnpDeviceDescriptionParser> > mDeviceDescriptionParsers;

    QList<QString> mAllHostsUUID;

    QList<QSharedPointer<RemoteServerEntry> > mRemoteServers;

    QNetworkAccessManager mNetworkAccess;
#endif

    bool mWithPlaylist = true;

    bool mUseLocalIcons = false;

    QString mDeviceId;

    QString mBrowseFlag;

    QString mFilter;

    QString mSortCriteria;

    DatabaseInterface* mAlbumDatabase = nullptr;

};

ViewPagesModel::ViewPagesModel(QObject *parent)
    : QAbstractListModel(parent), d(new ViewPagesModelPrivate)
{
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

#if defined UPNPQT_FOUND && UPNPQT_FOUND
    if (!d->mWithPlaylist) {
        return d->mAllDevices.size();
    }

    if (!d->mWithPlaylist) {
        return d->mAllDevices.size() + 1;
    }

    return d->mAllDevices.size() + 2;
#else
    return d->mWithPlaylist;
#endif
}

QVariant ViewPagesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (role < ColumnsRoles::NameRole || role > ColumnsRoles::UDNRole) {
        return {};
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

#if defined UPNPQT_FOUND && UPNPQT_FOUND
    if (index.row() < 0 || (index.row() >= d->mAllDevices.size() + 2 && d->mWithPlaylist)) {
        return {};
    }

    if (!d->mWithPlaylist && index.row() >= d->mAllDevices.size() + 1) {
        return {};
    }

    if (index.row() < d->mAllDevices.size()) {
        switch(convertedRole)
        {
        case ColumnsRoles::NameRole:
            return QString(d->mAllHostsDescription[d->mAllHostsUUID[index.row()]]->friendlyName() + QStringLiteral(" - Albums"));
        case ColumnsRoles::UDNRole:
            return d->mAllHostsUUID[index.row()];
        }
    } else {
        if (index.row() - d->mAllDevices.size() == 0) {
            switch(convertedRole)
            {
            case ColumnsRoles::NameRole:
                return QStringLiteral("Local Albums");
            default:
                return {};
            }
        } else {
            if (d->mWithPlaylist && index.row() - d->mAllDevices.size() == 1) {
                switch(convertedRole)
                {
                case ColumnsRoles::NameRole:
                    return QStringLiteral("Play List");
                default:
                    return {};
                }
            }
        }
    }
#else
    if (index.row() == 0) {
        switch(convertedRole)
        {
        case ColumnsRoles::NameRole:
            return QStringLiteral("Local Albums");
        default:
            return {};
        }
    } else {
        if (d->mWithPlaylist && index.row() == 1) {
            switch(convertedRole)
            {
            case ColumnsRoles::NameRole:
                return QStringLiteral("Play List");
            default:
                return {};
            }
        }
    }
#endif

    return {};
}

QHash<int, QByteArray> ViewPagesModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::NameRole)] = "name";
    roles[static_cast<int>(ColumnsRoles::UDNRole)] = "udn";

    return roles;
}

UpnpAlbumModel *ViewPagesModel::remoteAlbumModel(int index) const
{
#if defined UPNPQT_FOUND && UPNPQT_FOUND
    if (index < 0 || index > d->mRemoteServers.size() - 1) {
        return nullptr;
    }

    return d->mRemoteServers.at(index)->albumModel();
#else
    Q_UNUSED(index);

    return nullptr;
#endif
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

QString ViewPagesModel::deviceId() const
{
    return d->mDeviceId;
}

const QString &ViewPagesModel::browseFlag() const
{
    return d->mBrowseFlag;
}

const QString &ViewPagesModel::filter() const
{
    return d->mFilter;
}

const QString &ViewPagesModel::sortCriteria() const
{
    return d->mSortCriteria;
}

bool ViewPagesModel::useLocalIcons() const
{
    return d->mUseLocalIcons;
}

DatabaseInterface *ViewPagesModel::albumDatabase() const
{
    return d->mAlbumDatabase;
}

void ViewPagesModel::newDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery)
{
#if defined UPNPQT_FOUND && UPNPQT_FOUND
    if (serviceDiscovery->nt() == d->mDeviceId) {
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

            d->mRemoteServers.last()->albumModel()->setBrowseFlag(browseFlag());
            d->mRemoteServers.last()->albumModel()->setFilter(filter());
            d->mRemoteServers.last()->albumModel()->setSortCriteria(sortCriteria());
            d->mRemoteServers.last()->albumModel()->setUseLocalIcons(d->mUseLocalIcons);

            connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, d->mDeviceDescriptionParsers[decodedUdn].data(), &UpnpDeviceDescriptionParser::finishedDownload);
            connect(d->mDeviceDescriptionParsers[decodedUdn].data(), &UpnpDeviceDescriptionParser::descriptionParsed, this, &ViewPagesModel::descriptionParsed);

            d->mDeviceDescriptionParsers[decodedUdn]->downloadDeviceDescription(QUrl(serviceDiscovery->location()));

            endInsertRows();
        }
    }
#else
    Q_UNUSED(serviceDiscovery);
#endif
}

void ViewPagesModel::removedDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery)
{
#if defined UPNPQT_FOUND && UPNPQT_FOUND
    if (serviceDiscovery->nt() == d->mDeviceId) {
        qDebug() << "nt" << serviceDiscovery->nt();
        qDebug() << "usn" << serviceDiscovery->usn();
    }
#else
    Q_UNUSED(serviceDiscovery);
#endif
}

void ViewPagesModel::setDeviceId(QString deviceId)
{
    if (d->mDeviceId == deviceId)
        return;

    d->mDeviceId = deviceId;
    Q_EMIT deviceIdChanged(deviceId);
}

void ViewPagesModel::setBrowseFlag(const QString &flag)
{
    d->mBrowseFlag = flag;
    Q_EMIT browseFlagChanged();
}

void ViewPagesModel::setFilter(const QString &flag)
{
    d->mFilter = flag;
    Q_EMIT filterChanged();
}

void ViewPagesModel::setSortCriteria(const QString &criteria)
{
    d->mSortCriteria = criteria;
    Q_EMIT sortCriteriaChanged();
}

void ViewPagesModel::setUseLocalIcons(bool useLocalIcons)
{
    if (d->mUseLocalIcons == useLocalIcons)
        return;

    d->mUseLocalIcons = useLocalIcons;
    Q_EMIT useLocalIconsChanged();
}

void ViewPagesModel::setAlbumDatabase(DatabaseInterface *albumDatabase)
{
    if (d->mAlbumDatabase == albumDatabase)
        return;

    d->mAlbumDatabase = albumDatabase;
    Q_EMIT albumDatabaseChanged();
}

void ViewPagesModel::deviceDescriptionChanged(const QString &uuid)
{
#if defined UPNPQT_FOUND && UPNPQT_FOUND
    int deviceIndex = d->mAllHostsUUID.indexOf(uuid);

    d->mRemoteServers[deviceIndex]->albumModel()->setServerName(d->mAllHostsDescription[d->mAllHostsUUID[deviceIndex]]->friendlyName());


    if (deviceIndex != -1) {
        Q_EMIT dataChanged(index(deviceIndex), index(deviceIndex));
    }
#else
    Q_UNUSED(uuid);
#endif
}

void ViewPagesModel::descriptionParsed(const QString &UDN)
{
#if defined UPNPQT_FOUND && UPNPQT_FOUND
    d->mDeviceDescriptionParsers.remove(UDN.mid(5));
#else
    Q_UNUSED(UDN);
#endif
}


#include "moc_viewpagesmodel.cpp"
