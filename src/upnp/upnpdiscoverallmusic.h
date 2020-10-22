/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef UPNPDISCOVERALLMUSIC_H
#define UPNPDISCOVERALLMUSIC_H

#include "datatypes.h"

#include <QObject>
#include <QSharedPointer>

#include <memory>

class DatabaseInterface;
class UpnpDiscoveryResult;
class UpnpDiscoverAllMusicPrivate;
class UpnpDeviceDescription;

class UpnpDiscoverAllMusic : public QObject
{

    Q_OBJECT

public:

    explicit UpnpDiscoverAllMusic(QObject *parent = nullptr);

    ~UpnpDiscoverAllMusic() override;

    [[nodiscard]] [[nodiscard]] const DataTypes::ListNetworkServiceDataType& existingMediaServers() const;

    [[nodiscard]] [[nodiscard]] const UpnpDeviceDescription &deviceDescriptionByUdn(const QString &udn) const;

Q_SIGNALS:

    void newUpnpContentDirectoryService(const QString &name, const QString &uuid);

    void removedUpnpContentDirectoryService(const QString &name);

    void searchAllMediaServers(int maxDelay);

public Q_SLOTS:

    void newDevice(const UpnpDiscoveryResult &serviceDiscovery);

    void removedDevice(const UpnpDiscoveryResult &serviceDiscovery);

    void networkChanged();

private Q_SLOTS:

    void deviceDescriptionChanged(const QString &uuid);

    void descriptionParsed(const QString &UDN);

private:

    std::unique_ptr<UpnpDiscoverAllMusicPrivate> d;

};

#endif // UPNPDISCOVERALLMUSIC_H
