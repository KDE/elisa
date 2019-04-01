/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TRACKMETADATAMODEL_H
#define TRACKMETADATAMODEL_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "databaseinterface.h"
#include "modeldataloader.h"

#include <QUrl>
#include <QAbstractListModel>

class MusicListenersManager;

class ELISALIB_EXPORT TrackMetadataModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QUrl coverUrl
               READ coverUrl
               NOTIFY coverUrlChanged)

    Q_PROPERTY(QString fileUrl
               READ fileUrl
               NOTIFY fileUrlChanged)

    Q_PROPERTY(MusicListenersManager* manager
               READ manager
               WRITE setManager
               NOTIFY managerChanged)

public:

    enum ColumnRoles
    {
        ItemNameRole = Qt::UserRole + 1,
        ItemTypeRole,
    };

    enum ItemType
    {
        TextEntry,
        IntegerEntry,
        RatingEntry,
        DateEntry,
    };

    Q_ENUM(ItemType)

    using TrackDataType = DatabaseInterface::TrackDataType;

    explicit TrackMetadataModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    QHash<int, QByteArray> roleNames() const override;

    const QUrl& coverUrl() const;

    QString fileUrl() const;

    MusicListenersManager* manager() const;

Q_SIGNALS:

    void needDataByDatabaseId(ElisaUtils::PlayListEntryType dataType, qulonglong databaseId);

    void needDataByFileName(ElisaUtils::PlayListEntryType dataType, const QUrl &fileName);

    void coverUrlChanged();

    void fileUrlChanged();

    void managerChanged();

public Q_SLOTS:

    void trackData(const TrackMetadataModel::TrackDataType &trackData);

    void initializeByTrackId(qulonglong databaseId);

    void initializeByTrackFileName(const QUrl &fileName);

    void setManager(MusicListenersManager *newManager);

protected:

    void fillDataFromTrackData(const TrackMetadataModel::TrackDataType &trackData);

    virtual void filterDataFromTrackData();

    void removeMetaData(DatabaseInterface::ColumnsRoles metaData);

    TrackDataType::mapped_type dataFromType(TrackDataType::key_type metaData) const;

private:

    TrackDataType mFullData;

    TrackDataType mTrackData;

    QUrl mCoverImage;

    QString mFileUrl;

    QList<TrackDataType::key_type> mTrackKeys;

    ModelDataLoader mDataLoader;

    MusicListenersManager *mManager = nullptr;

};

#endif // TRACKMETADATAMODEL_H
