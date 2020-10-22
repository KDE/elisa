/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef UPNPCONTENTDIRECTORYMODEL_H
#define UPNPCONTENTDIRECTORYMODEL_H

#include "elisautils.h"
#include "datatypes.h"

#include <QAbstractItemModel>

#include <memory>

class UpnpContentDirectoryModelPrivate;
class UpnpControlContentDirectory;
class MusicListenersManager;
class DatabaseInterface;

class UpnpContentDirectoryModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(QString parentId
               READ parentId
               WRITE setParentId
               NOTIFY parentIdChanged)

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

    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:

    enum ItemClass {
        Container = 0,
        Album = 1,
        AudioTrack = 2,
    };

    Q_ENUM(ItemClass)

    explicit UpnpContentDirectoryModel(QObject *parent = nullptr);

    ~UpnpContentDirectoryModel() override;

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] QModelIndex parent(const QModelIndex &child) const override;

    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] bool canFetchMore(const QModelIndex &parent) const override;

    void fetchMore(const QModelIndex &parent) override;

    [[nodiscard]] const QString& parentId() const;

    [[nodiscard]] const QString& browseFlag() const;

    void setBrowseFlag(const QString &flag);

    [[nodiscard]] const QString& filter() const;

    void setFilter(const QString &flag);

    [[nodiscard]] const QString& sortCriteria() const;

    void setSortCriteria(const QString &criteria);

    [[nodiscard]] UpnpControlContentDirectory* contentDirectory() const;

    void setContentDirectory(UpnpControlContentDirectory *directory);

    [[nodiscard]] bool useLocalIcons() const;

    void setUseLocalIcons(bool value);

    [[nodiscard]] bool isBusy() const;

Q_SIGNALS:

    void parentIdChanged();

    void browseFlagChanged();

    void filterChanged();

    void sortCriteriaChanged();

    void contentDirectoryChanged();

    void useLocalIconsChanged();

    void isBusyChanged();

public Q_SLOTS:

    void initializeByData(MusicListenersManager *manager, DatabaseInterface *database,
                          ElisaUtils::PlayListEntryType modelType, ElisaUtils::FilterType filter,
                          const DataTypes::DataType &dataFilter);

    void setParentId(QString parentId);

private Q_SLOTS:

    void contentChanged(const QString &parentId);

private:

    [[nodiscard]] QModelIndex indexFromInternalId(quintptr internalId) const;

    std::unique_ptr<UpnpContentDirectoryModelPrivate> d;

};

Q_DECLARE_METATYPE(UpnpContentDirectoryModel::ItemClass)

#endif // UPNPCONTENTDIRECTORYMODEL_H

