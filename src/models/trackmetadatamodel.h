/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef TRACKMETADATAMODEL_H
#define TRACKMETADATAMODEL_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "trackslistener.h"
#include "datatypes.h"
#include "modeldataloader.h"
#include "filescanner.h"

#include <QQmlEngine>
#include <QUrl>
#include <QAbstractListModel>
#include <QFutureWatcher>
#include <QMutex>

class MusicListenersManager;

class ELISALIB_EXPORT TrackMetadataModel : public QAbstractListModel
{
    Q_OBJECT

    QML_ELEMENT

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
        HasDataRole,
    };

    Q_ENUM(ColumnRoles)

    enum ItemType
    {
        TextEntry,
        UrlEntry,
        IntegerEntry,
        RatingEntry,
        DateEntry,
        DurationEntry,
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
    QString lyricsLocation() const;

    qulonglong databaseId() const;

Q_SIGNALS:

    void needDataByDatabaseIdAndUrl(ElisaUtils::PlayListEntryType dataType, qulonglong databaseId, const QUrl &url);

    void needDataByUrl(ElisaUtils::PlayListEntryType dataType, const QUrl &fileName);

    void coverUrlChanged();

    void fileUrlChanged();

    void managerChanged();

    void lyricsChanged();

    void databaseIdChanged();

public Q_SLOTS:

    void trackData(const TrackMetadataModel::TrackDataType &trackData);

    void initializeByIdAndUrl(ElisaUtils::PlayListEntryType type, qulonglong databaseId, const QUrl &url);

    void initializeByUrl(ElisaUtils::PlayListEntryType type, const QUrl &url);

    void initializeForNewRadio();

    void setManager(MusicListenersManager *newManager);

    void setDatabase(DatabaseInterface *trackDatabase);

    void radioData(const TrackMetadataModel::TrackDataType &radiosData);

protected:

    void saveChanges();

    void undoChanges();

    virtual void fillDataFromTrackData(const TrackMetadataModel::TrackDataType &trackData);

    virtual void fillDataForNewRadio();

    virtual void filterDataFromTrackData();

    void removeMetaData(DataTypes::ColumnsRoles metaData);

    TrackDataType::mapped_type dataFromType(TrackDataType::key_type metaData) const;

    virtual void fillLyricsDataFromTrack();

    const TrackDataType& allTrackData() const;

    virtual void initialize(MusicListenersManager *newManager,
                            DatabaseInterface *trackDatabase);

    ModelDataLoader& modelDataLoader();

    const TrackDataType& displayedTrackData() const;

    DataTypes::ColumnsRoles trackKey(int index) const;

    void removeDataByIndex(int index);

    void addDataByIndex(int index);

    static QString nameFromRole(DataTypes::ColumnsRoles role);

    bool metadataExists(DataTypes::ColumnsRoles metadataRole) const;

private Q_SLOTS:

    void lyricsValueIsReady();

private:

    void fetchLyrics();

    void resetDisplayData();

    TrackDataType mFullData;

    TrackDataType mDisplayData;

    QUrl mCoverImage;

    QUrl mFileUrl;

    qulonglong mDatabaseId = 0;

    QList<TrackDataType::key_type> mDisplayKeys;

    ModelDataLoader mDataLoader;

    MusicListenersManager *mManager = nullptr;

    FileScanner mFileScanner;

    QMutex mFileScannerMutex;

    // std::pair<LyricsContent, LyricsLocation>, LyricsLocation should be empty if lyrics is embedded
    QFutureWatcher<std::pair<QString, QString>> mLyricsValueWatcher;
};

#endif // TRACKMETADATAMODEL_H
