/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef UPNPDISCOVERALLMUSIC_H
#define UPNPDISCOVERALLMUSIC_H

#include <QObject>
#include <QSharedPointer>

#include <memory>

class DatabaseInterface;
class UpnpDiscoveryResult;
class UpnpDiscoverAllMusicPrivate;

class UpnpDiscoverAllMusic : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QString deviceId
               READ deviceId
               WRITE setDeviceId
               NOTIFY deviceIdChanged)

    Q_PROPERTY(QString browseFlag
               READ browseFlag
               WRITE setBrowseFlag
               NOTIFY browseFlagChanged)

    Q_PROPERTY(QString filter
               READ filter
               WRITE setFilter
               NOTIFY filterChanged)

    Q_PROPERTY(QString sortCriteria
               READ sortCriteria
               WRITE setSortCriteria
               NOTIFY sortCriteriaChanged)

    Q_PROPERTY(DatabaseInterface* albumDatabase
               READ albumDatabase
               WRITE setAlbumDatabase
               NOTIFY albumDatabaseChanged)

public:

    explicit UpnpDiscoverAllMusic(QObject *parent = nullptr);

    ~UpnpDiscoverAllMusic() override;

    QString deviceId() const;

    const QString& browseFlag() const;

    const QString& filter() const;

    const QString& sortCriteria() const;

    DatabaseInterface* albumDatabase() const;

Q_SIGNALS:

    void deviceIdChanged(QString deviceId);

    void browseFlagChanged();

    void filterChanged();

    void sortCriteriaChanged();

    void albumDatabaseChanged();

public Q_SLOTS:

    void newDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery);

    void removedDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery);

    void setDeviceId(QString deviceId);

    void setBrowseFlag(const QString &flag);

    void setFilter(const QString &flag);

    void setSortCriteria(const QString &criteria);

    void setAlbumDatabase(DatabaseInterface* albumDatabase);

private Q_SLOTS:

    void deviceDescriptionChanged(const QString &uuid);

    void descriptionParsed(const QString &UDN);

    void contentChanged(const QString &uuid, const QString &parentId);

private:

    std::unique_ptr<UpnpDiscoverAllMusicPrivate> d;

};

#endif // UPNPDISCOVERALLMUSIC_H
