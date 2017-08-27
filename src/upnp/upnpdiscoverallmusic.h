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

    explicit UpnpDiscoverAllMusic(QObject *parent = 0);

    ~UpnpDiscoverAllMusic();

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
