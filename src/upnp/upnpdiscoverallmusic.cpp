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
#include "didlparser.h"

#include "databaseinterface.h"

#include <QList>
#include <QHash>
#include <QString>
#include <QSharedPointer>

class UpnpDiscoverAllMusicPrivate
{
public:

    QHash<QString, QSharedPointer<UpnpDiscoveryResult> > mAllDeviceDiscoveryResults;

    QHash<QString, QSharedPointer<UpnpDeviceDescription> > mAllHostsDescription;

    QHash<QString, QSharedPointer<UpnpDeviceDescriptionParser> > mDeviceDescriptionParsers;

    QHash<QString, QSharedPointer<UpnpControlContentDirectory> > mControlContentDirectory;

    QHash<QString, QSharedPointer<DidlParser>> mDidlParsers;

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
    Q_UNUSED(uuid);
}

void UpnpDiscoverAllMusic::descriptionParsed(const QString &UDN)
{
    QString uuid = UDN.mid(5);

    d->mDeviceDescriptionParsers.remove(uuid);

    int deviceIndex = d->mAllHostsUUID.indexOf(UDN.mid(5));

    d->mControlContentDirectory[uuid] = QSharedPointer<UpnpControlContentDirectory>(new UpnpControlContentDirectory);
    auto serviceDescription = d->mAllHostsDescription[d->mAllHostsUUID[deviceIndex]]->serviceById(QStringLiteral("urn:upnp-org:serviceId:ContentDirectory"));
    d->mControlContentDirectory[uuid]->setDescription(serviceDescription.data());
    d->mDidlParsers[uuid] = QSharedPointer<DidlParser>(new DidlParser);

    auto currentDidlParser = d->mDidlParsers[uuid].data();
    currentDidlParser->setSearchCriteria(QStringLiteral("upnp:class = \"object.item.audioItem.musicTrack\""));
    currentDidlParser->setParentId(QStringLiteral("0"));
    currentDidlParser->setContentDirectory(d->mControlContentDirectory[uuid].data());

    connect(currentDidlParser, &DidlParser::isDataValidChanged, this, &UpnpDiscoverAllMusic::contentChanged);

    currentDidlParser->search(0, 0);
}

void UpnpDiscoverAllMusic::contentChanged(const QString &uuid, const QString &parentId)
{
    if (!d->mAlbumDatabase) {
        return;
    }

    if (parentId == QStringLiteral("0")) {
        auto currentDidlParser = d->mDidlParsers[uuid];
        if (!currentDidlParser) {
            return;
        }

        const auto &allNewTracks = currentDidlParser->newMusicTracks();

        d->mAlbumDatabase->insertTracksList(allNewTracks, currentDidlParser->covers());
    }
}


#include "moc_upnpdiscoverallmusic.cpp"
