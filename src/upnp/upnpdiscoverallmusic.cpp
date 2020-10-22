/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "upnpdiscoverallmusic.h"

#include "elisautils.h"

#include "config-upnp-qt.h"

#include "upnpLogging.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "upnpdevicedescription.h"
#include "upnpdiscoveryresult.h"
#include "upnpservicedescription.h"
#include "upnpdevicedescriptionparser.h"
#include "upnpcontrolcontentdirectory.h"
#include "didlparser.h"

#include <QList>
#include <QHash>
#include <QString>
#include <QSharedPointer>

#include <QLoggingCategory>

class UpnpDiscoverAllMusicPrivate
{
public:

    QHash<QString, UpnpDiscoveryResult> mAllDeviceDiscoveryResults;

    QHash<QString, UpnpDeviceDescription> mAllHostsDescription;

    QHash<QString, QSharedPointer<UpnpDeviceDescriptionParser>> mDeviceDescriptionParsers;

    QList<QString> mAllHostsUUID;

    QNetworkAccessManager mNetworkAccess;

    DataTypes::ListNetworkServiceDataType mAllServices;

};

UpnpDiscoverAllMusic::UpnpDiscoverAllMusic(QObject *parent) : QObject(parent), d(new UpnpDiscoverAllMusicPrivate)
{
}

const DataTypes::ListNetworkServiceDataType &UpnpDiscoverAllMusic::existingMediaServers() const
{
    return d->mAllServices;
}

const UpnpDeviceDescription &UpnpDiscoverAllMusic::deviceDescriptionByUdn(const QString &udn) const
{
    qDebug() << "UpnpDiscoverAllMusic::deviceDescriptionByUdn" << d->mAllHostsDescription.keys() << udn;
    return d->mAllHostsDescription[udn];
}

UpnpDiscoverAllMusic::~UpnpDiscoverAllMusic()
= default;

void UpnpDiscoverAllMusic::newDevice(const UpnpDiscoveryResult &newService)
{
    qCDebug(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::newDevice" << newService;

    const QString &deviceUuid = newService.usn().mid(5, 36);
    if (!d->mAllDeviceDiscoveryResults.contains(deviceUuid)) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::newDevice" << "new uuid" << deviceUuid;
        d->mAllDeviceDiscoveryResults[deviceUuid] = newService;

        const QString &decodedUdn(deviceUuid.right(36));

        d->mAllHostsUUID.push_back(decodedUdn);
        d->mAllHostsDescription[decodedUdn].setUDN(decodedUdn);

        d->mDeviceDescriptionParsers[decodedUdn].reset(new UpnpDeviceDescriptionParser(&d->mNetworkAccess, d->mAllHostsDescription[decodedUdn]));

        connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, d->mDeviceDescriptionParsers[decodedUdn].data(), &UpnpDeviceDescriptionParser::finishedDownload);
        connect(d->mDeviceDescriptionParsers[decodedUdn].data(), &UpnpDeviceDescriptionParser::descriptionParsed, this, &UpnpDiscoverAllMusic::descriptionParsed);

        d->mDeviceDescriptionParsers[decodedUdn]->downloadDeviceDescription(QUrl(newService.location()));
    } else {
        qCDebug(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::newDevice" << "existing uuid" << deviceUuid;
    }
}

void UpnpDiscoverAllMusic::removedDevice(const UpnpDiscoveryResult &removedService)
{
    qCInfo(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::removedDevice" << removedService;

    const QString &deviceUuid = removedService.usn().mid(5, 36);
    if (!d->mAllDeviceDiscoveryResults.contains(deviceUuid)) {
        qCInfo(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::removedDevice" << removedService << "not found";
        return;
    }

    auto deviceUuidOnly = deviceUuid.right(36);
    std::remove_if(d->mAllServices.begin(), d->mAllServices.end(), [this, deviceUuidOnly](const auto &data) {
        return data.title() == d->mAllHostsDescription[deviceUuidOnly].friendlyName();
    });

    d->mAllDeviceDiscoveryResults.remove(deviceUuid);

    qCInfo(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::removedDevice" << "number of services" << d->mAllServices.count();

    qCInfo(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::removedDevice" << d->mAllHostsDescription[deviceUuid].friendlyName();
    Q_EMIT removedUpnpContentDirectoryService(d->mAllHostsDescription[deviceUuid].friendlyName());
}

void UpnpDiscoverAllMusic::networkChanged()
{
    for (const auto &oneService : qAsConst(d->mAllServices)) {
        Q_EMIT removedUpnpContentDirectoryService(oneService.title());
    }

    d->mAllServices.clear();
    d->mAllHostsUUID.clear();
    d->mAllHostsDescription.clear();
    d->mDeviceDescriptionParsers.clear();
    d->mAllDeviceDiscoveryResults.clear();

    qCInfo(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::networkChanged"
                              << "searchAllMediaServers";
    Q_EMIT searchAllMediaServers(1);
}

void UpnpDiscoverAllMusic::deviceDescriptionChanged(const QString &uuid)
{
    Q_UNUSED(uuid)
}

void UpnpDiscoverAllMusic::descriptionParsed(const QString &UDN)
{
    qCDebug(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::descriptionParsed" << UDN;

    QString uuid = UDN.mid(5);

    d->mDeviceDescriptionParsers.remove(uuid);

    int deviceIndex = d->mAllHostsUUID.indexOf(uuid);

    qCDebug(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::descriptionParsed" << d->mAllHostsUUID;
    qCDebug(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::descriptionParsed" << uuid;
    qCDebug(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::descriptionParsed" << deviceIndex;

    if (deviceIndex == -1) {
        qCDebug(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::descriptionParsed" << d->mAllHostsUUID;
        return;
    }

    qCDebug(orgKdeElisaUpnp()) << "UpnpDiscoverAllMusic::descriptionParsed" << d->mAllHostsDescription[uuid].friendlyName();

    d->mAllServices.push_back({{DataTypes::ElementTypeRole, QVariant::fromValue(ElisaUtils::UpnpMediaServer)},
                               {DataTypes::TitleRole, d->mAllHostsDescription[uuid].friendlyName()},
                               {DataTypes::UUIDRole, uuid},});

    Q_EMIT newUpnpContentDirectoryService(d->mAllHostsDescription[uuid].friendlyName(), uuid);
}

#include "moc_upnpdiscoverallmusic.cpp"
