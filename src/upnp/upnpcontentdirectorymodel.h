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

#ifndef UPNPCONTENTDIRECTORYMODEL_H
#define UPNPCONTENTDIRECTORYMODEL_H

#include "upnpQt_export.h"

#include <QAbstractItemModel>

#include <memory>

class UpnpContentDirectoryModelPrivate;
class UpnpSsdpEngine;
class UpnpControlAbstractDevice;
class UpnpControlContentDirectory;
class UpnpDiscoveryResult;

class UPNPQT_EXPORT UpnpContentDirectoryModel : public QAbstractItemModel
{
    Q_OBJECT

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

    Q_PROPERTY(UpnpControlContentDirectory* contentDirectory
               READ contentDirectory
               WRITE setContentDirectory
               NOTIFY contentDirectoryChanged)

    Q_PROPERTY(bool useLocalIcons
               READ useLocalIcons
               WRITE setUseLocalIcons
               NOTIFY useLocalIconsChanged)

public:

    enum ItemClass {
        Container = 0,
        Album = 1,
        AudioTrack = 2,
    };

    Q_ENUM(ItemClass)

    enum ColumnsRoles {
        TitleRole = Qt::UserRole + 1,
        DurationRole = TitleRole + 1,
        CreatorRole = DurationRole + 1,
        ArtistRole = CreatorRole + 1,
        AlbumRole = ArtistRole + 1,
        RatingRole = AlbumRole + 1,
        ImageRole = RatingRole + 1,
        ResourceRole = ImageRole + 1,
        ItemClassRole = ResourceRole + 1,
        CountRole = ItemClassRole + 1,
        IdRole = CountRole + 1,
        ParentIdRole = IdRole + 1,
        IsPlayingRole = ParentIdRole + 1,
    };

    Q_ENUM(ColumnsRoles)

    explicit UpnpContentDirectoryModel(QObject *parent = 0);

    virtual ~UpnpContentDirectoryModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent) const override;

    void fetchMore(const QModelIndex &parent) override;

    const QString& browseFlag() const;

    void setBrowseFlag(const QString &flag);

    const QString& filter() const;

    void setFilter(const QString &flag);

    const QString& sortCriteria() const;

    void setSortCriteria(const QString &criteria);

    UpnpControlContentDirectory* contentDirectory() const;

    void setContentDirectory(UpnpControlContentDirectory *directory);

    bool useLocalIcons() const;

    void setUseLocalIcons(bool value);

    Q_INVOKABLE QString objectIdByIndex(const QModelIndex &index) const;

    Q_INVOKABLE QVariant getUrl(const QModelIndex &index) const;

    Q_INVOKABLE QModelIndex indexFromId(const QString &id) const;

Q_SIGNALS:

    void browseFlagChanged();

    void filterChanged();

    void sortCriteriaChanged();

    void contentDirectoryChanged();

    void useLocalIconsChanged();

public Q_SLOTS:

    void browseFinished(const QString &result, int numberReturned, int totalMatches, int systemUpdateID, bool success);

private Q_SLOTS:

private:

    QModelIndex indexFromInternalId(quintptr internalId) const;

    std::unique_ptr<UpnpContentDirectoryModelPrivate> d;

};

Q_DECLARE_METATYPE(UpnpContentDirectoryModel::ItemClass)

#endif // UPNPCONTENTDIRECTORYMODEL_H

