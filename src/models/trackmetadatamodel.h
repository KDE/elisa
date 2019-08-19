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
#include "filescanner.h"

#include <QUrl>
#include <QAbstractListModel>
#include <QMimeDatabase>
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

    Q_PROPERTY(MusicListenersManager* manager
               READ manager
               WRITE setManager
               NOTIFY managerChanged)

    Q_PROPERTY(QString lyrics
               READ lyrics
               NOTIFY lyricsChanged)

    Q_PROPERTY(bool isRadio
               READ isRadio
               WRITE setIsRadio)

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

    using TrackDataType = DatabaseInterface::TrackDataType;

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

    bool isRadio();

Q_SIGNALS:

    void needDataByDatabaseId(ElisaUtils::PlayListEntryType dataType, qulonglong databaseId);

    void needDataByFileName(ElisaUtils::PlayListEntryType dataType, const QUrl &fileName);

    void coverUrlChanged();

    void fileUrlChanged();

    void managerChanged();

    void lyricsChanged();

    void saveRadioData(DatabaseInterface::TrackDataType trackDataType);

    void deleteRadioData(qulonglong radioId);

    void disableApplyButton();

    void hideDeleteButton();

    void showDeleteButton();

    void closeWindow();

public Q_SLOTS:

    void trackData(const TrackMetadataModel::TrackDataType &trackData);

    void initializeByTrackId(qulonglong databaseId);

    void initializeByTrackFileName(const QString &fileName);

    void initializeForNewRadio();

    void setManager(MusicListenersManager *newManager);

    void setIsRadio(bool isRadio);

    void setDatabase(DatabaseInterface *trackDatabase);

    void saveData();

    void deleteRadio();

    void radioAdded(TrackMetadataModel::TrackDataType radiosData);

    void radioModified();

    void radioRemoved();

protected:

    void fillDataFromTrackData(const TrackMetadataModel::TrackDataType &trackData);

    void fillDataForNewRadio();

    virtual void filterDataFromTrackData();

    void removeMetaData(DatabaseInterface::ColumnsRoles metaData);

    TrackDataType::mapped_type dataFromType(TrackDataType::key_type metaData) const;

    virtual void fillLyricsDataFromTrack();

private Q_SLOTS:

    void lyricsValueIsReady();

private:

    void initialize(MusicListenersManager *newManager,
                    DatabaseInterface *trackDatabase);

    void fetchLyrics();

    QVariant dataGeneral(const QModelIndex &index, int role) const;

    QVariant dataRadio(const QModelIndex &index, int role) const;

    TrackDataType mFullData;

    TrackDataType mTrackData;

    QUrl mCoverImage;

    QString mFileUrl;

    QList<TrackDataType::key_type> mTrackKeys;

    ModelDataLoader mDataLoader;

    MusicListenersManager *mManager = nullptr;

    bool mIsRadio = false;

    FileScanner mFileScanner;

    QMimeDatabase mMimeDatabase;

    QFutureWatcher<QString> mLyricsValueWatcher;
};

#endif // TRACKMETADATAMODEL_H
