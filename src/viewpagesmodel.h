
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

#ifndef VIEWPAGESMODEL_H
#define VIEWPAGESMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <QtCore/QByteArray>

class ViewPagesModelPrivate;
class UpnpDiscoveryResult;
class UpnpControlContentDirectory;
class RemoteServerEntry;
class DatabaseInterface;

class ViewPagesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool withPlaylist
               READ withPlaylist
               WRITE setWithPlaylist
               NOTIFY withPlaylistChanged)

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

    Q_PROPERTY(bool useLocalIcons
               READ useLocalIcons
               WRITE setUseLocalIcons
               NOTIFY useLocalIconsChanged)

    Q_PROPERTY(DatabaseInterface* albumDatabase
               READ albumDatabase
               WRITE setAlbumDatabase
               NOTIFY albumDatabaseChanged)

public:

    enum ColumnsRoles {
        NameRole = Qt::UserRole + 1,
        UDNRole,
    };
    Q_ENUM(ColumnsRoles)

    explicit ViewPagesModel(QObject *parent = 0);

    ~ViewPagesModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void setWithPlaylist(bool value);

    bool withPlaylist() const;

    QString deviceId() const;

    const QString& browseFlag() const;

    const QString& filter() const;

    const QString& sortCriteria() const;

    bool useLocalIcons() const;

    DatabaseInterface* albumDatabase() const;

public Q_SLOTS:

    void newDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery);

    void removedDevice(QSharedPointer<UpnpDiscoveryResult> serviceDiscovery);

    void setDeviceId(QString deviceId);

    void setBrowseFlag(const QString &flag);

    void setFilter(const QString &flag);

    void setSortCriteria(const QString &criteria);

    void setUseLocalIcons(bool useLocalIcons);

    void setAlbumDatabase(DatabaseInterface* albumDatabase);

private Q_SLOTS:

    void deviceDescriptionChanged(const QString &uuid);

    void descriptionParsed(const QString &UDN);

Q_SIGNALS:

    void withPlaylistChanged();

    void deviceIdChanged(QString deviceId);

    void browseFlagChanged();

    void filterChanged();

    void sortCriteriaChanged();

    void useLocalIconsChanged();

    void albumDatabaseChanged();

private:

    ViewPagesModelPrivate *d;
};

#endif // VIEWPAGESMODEL_H
