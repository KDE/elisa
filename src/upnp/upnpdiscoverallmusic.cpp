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

#include "upnpdiscoverallmusic.h"

#include "config-upnp-qt.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "upnpdevicedescription.h"
#include "upnpdiscoveryresult.h"
#include "upnpdevicedescriptionparser.h"
#include "upnpcontrolcontentdirectory.h"

#include "databaseinterface.h"

#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>

class UpnpDiscoverAllMusicPrivate
{
public:

    QHash<QString, QSharedPointer<UpnpDiscoveryResult> > mAllDeviceDiscoveryResults;

    QHash<QString, QSharedPointer<UpnpDeviceDescription> > mAllHostsDescription;

    QHash<QString, QSharedPointer<UpnpDeviceDescriptionParser> > mDeviceDescriptionParsers;

    QList<QString> mAllHostsUUID;

    QNetworkAccessManager mNetworkAccess;

    QString mDeviceId;

    QString mBrowseFlag;

    QString mFilter;

    QString mSortCriteria;

    DatabaseInterface* mAlbumDatabase = nullptr;

};

UpnpDiscoverAllMusic::UpnpDiscoverAllMusic(QObject *parent) : QObject(parent), d(new UpnpDiscoverAllMusicPrivate)
{
}

UpnpDiscoverAllMusic::~UpnpDiscoverAllMusic()
{
    delete d;
}

QString UpnpDiscoverAllMusic::deviceId() const
{
    return d->mDeviceId;
}

const QString &UpnpDiscoverAllMusic::browseFlag() const
{
    return d->mBrowseFlag;
}

const QString &UpnpDiscoverAllMusic::filter() const
{
    return d->mFilter;
}

const QString &UpnpDiscoverAllMusic::sortCriteria() const
{
    return d->mSortCriteria;
}

DatabaseInterface *UpnpDiscoverAllMusic::albumDatabase() const
{
    return d->mAlbumDatabase;
}

void UpnpDiscoverAllMusic::newDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery)
{
    if (serviceDiscovery->nt() == d->mDeviceId) {
        const QString &deviceUuid = serviceDiscovery->usn().mid(5, 36);
        if (!d->mAllDeviceDiscoveryResults.contains(deviceUuid)) {
            d->mAllDeviceDiscoveryResults[deviceUuid] = serviceDiscovery;

            const QString &decodedUdn(deviceUuid);

            d->mAllHostsUUID.push_back(decodedUdn);
            d->mAllHostsDescription[decodedUdn].reset(new UpnpDeviceDescription);
            d->mAllHostsDescription[decodedUdn]->setUDN(decodedUdn);

            connect(d->mAllHostsDescription[decodedUdn].data(), &UpnpDeviceDescription::friendlyNameChanged, this, &UpnpDiscoverAllMusic::deviceDescriptionChanged);

            d->mDeviceDescriptionParsers[decodedUdn].reset(new UpnpDeviceDescriptionParser(&d->mNetworkAccess, d->mAllHostsDescription[decodedUdn]));

            connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, d->mDeviceDescriptionParsers[decodedUdn].data(), &UpnpDeviceDescriptionParser::finishedDownload);
            connect(d->mDeviceDescriptionParsers[decodedUdn].data(), &UpnpDeviceDescriptionParser::descriptionParsed, this, &UpnpDiscoverAllMusic::descriptionParsed);

            d->mDeviceDescriptionParsers[decodedUdn]->downloadDeviceDescription(QUrl(serviceDiscovery->location()));
        }
    }
}

void UpnpDiscoverAllMusic::removedDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery)
{
    if (serviceDiscovery->nt() == d->mDeviceId) {
        qDebug() << "nt" << serviceDiscovery->nt();
        qDebug() << "usn" << serviceDiscovery->usn();
    }
}

void UpnpDiscoverAllMusic::setDeviceId(QString deviceId)
{
    if (d->mDeviceId == deviceId)
        return;

    d->mDeviceId = deviceId;
    Q_EMIT deviceIdChanged(deviceId);
}

void UpnpDiscoverAllMusic::setBrowseFlag(const QString &flag)
{
    d->mBrowseFlag = flag;
    Q_EMIT browseFlagChanged();
}

void UpnpDiscoverAllMusic::setFilter(const QString &flag)
{
    d->mFilter = flag;
    Q_EMIT filterChanged();
}

void UpnpDiscoverAllMusic::setSortCriteria(const QString &criteria)
{
    d->mSortCriteria = criteria;
    Q_EMIT sortCriteriaChanged();
}

void UpnpDiscoverAllMusic::setAlbumDatabase(DatabaseInterface *albumDatabase)
{
    if (d->mAlbumDatabase == albumDatabase)
        return;

    d->mAlbumDatabase = albumDatabase;
    Q_EMIT albumDatabaseChanged();
}

void UpnpDiscoverAllMusic::deviceDescriptionChanged(const QString &uuid)
{
    int deviceIndex = d->mAllHostsUUID.indexOf(uuid);

    //d->mRemoteServers[deviceIndex]->albumModel()->setServerName(d->mAllHostsDescription[d->mAllHostsUUID[deviceIndex]]->friendlyName());
}

void UpnpDiscoverAllMusic::descriptionParsed(const QString &UDN)
{
    d->mDeviceDescriptionParsers.remove(UDN.mid(5));
}


#include "moc_upnpdiscoverallmusic.cpp"
