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
#include "trackslistener.h"
#include "datatypes.h"
#include "modeldataloader.h"
#include "filescanner.h"

#include <QUrl>
#include <QAbstractListModel>
#include <QFutureWatcher>

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

    Q_PROPERTY(qulonglong databaseId
               READ databaseId
               NOTIFY databaseIdChanged)

    Q_PROPERTY(MusicListenersManager* manager
               READ manager
               WRITE setManager
               NOTIFY managerChanged)

    Q_PROPERTY(QString lyrics
               READ lyrics
               NOTIFY lyricsChanged)

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
        LongTextEntry,
    };

    Q_ENUM(ItemType)

    using TrackDataType = DataTypes::TrackDataType;

    explicit TrackMetadataModel(QObject *parent = nullptr);

    ~TrackMetadataModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    QHash<int, QByteArray> roleNames() const override;

    QUrl coverUrl() const;

    QString fileUrl() const;

    MusicListenersManager* manager() const;

    QString lyrics() const;

    qulonglong databaseId() const;

Q_SIGNALS:

    void needDataByDatabaseIdAndUrl(ElisaUtils::PlayListEntryType dataType, qulonglong databaseId, const QUrl &url);

    void needDataByUrl(ElisaUtils::PlayListEntryType dataType, const QUrl &fileName);

    void coverUrlChanged();

    void fileUrlChanged();

    void managerChanged();

    void lyricsChanged();

    void saveRadioData(const DataTypes::TrackDataType &trackDataType);

    void deleteRadioData(qulonglong radioId);

    void databaseIdChanged();

public Q_SLOTS:

    void trackData(const TrackMetadataModel::TrackDataType &trackData);

    void initializeByIdAndUrl(ElisaUtils::PlayListEntryType type, qulonglong databaseId, const QUrl &url);

    void initializeByUrl(ElisaUtils::PlayListEntryType type, const QUrl &url);

    void initializeForNewRadio();

    void setManager(MusicListenersManager *newManager);

    void setDatabase(DatabaseInterface *trackDatabase);

    void saveData();

    void deleteRadio();

    void radioData(const TrackMetadataModel::TrackDataType &radiosData);

protected:

    void fillDataFromTrackData(const TrackMetadataModel::TrackDataType &trackData,
                               const QList<DataTypes::ColumnsRoles> &fieldsForTrack);

    void fillDataForNewRadio();

    virtual void filterDataFromTrackData();

    void removeMetaData(DataTypes::ColumnsRoles metaData);

    TrackDataType::mapped_type dataFromType(TrackDataType::key_type metaData) const;

    virtual void fillLyricsDataFromTrack();

private Q_SLOTS:

    void lyricsValueIsReady();

private:

    void initialize(MusicListenersManager *newManager,
                    DatabaseInterface *trackDatabase);

    void fetchLyrics();

    TrackDataType mFullData;

    TrackDataType mTrackData;

    QUrl mCoverImage;

    QString mFileUrl;

    qulonglong mDatabaseId = 0;

    QList<TrackDataType::key_type> mTrackKeys;

    ModelDataLoader mDataLoader;

    MusicListenersManager *mManager = nullptr;

    FileScanner mFileScanner;

    QFutureWatcher<QString> mLyricsValueWatcher;
};

#endif // TRACKMETADATAMODEL_H
