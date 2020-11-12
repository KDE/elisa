/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MODELDATALOADER_H
#define MODELDATALOADER_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "databaseinterface.h"
#include "datatypes.h"
#include "models/datamodel.h"

#include <QObject>

#include <memory>

class ModelDataLoaderPrivate;

class ELISALIB_EXPORT ModelDataLoader : public QObject
{

    Q_OBJECT

public:

    using ListAlbumDataType = DataTypes::ListAlbumDataType;
    using ListArtistDataType = DataTypes::ListArtistDataType;
    using ListGenreDataType = DataTypes::ListGenreDataType;
    using ListTrackDataType = DataTypes::ListTrackDataType;
    using ListRadioDataType = DataTypes::ListRadioDataType;
    using TrackDataType = DataTypes::TrackDataType;
    using AlbumDataType = DataTypes::AlbumDataType;

    using FilterType = ElisaUtils::FilterType;

    explicit ModelDataLoader(QObject *parent = nullptr);

    ~ModelDataLoader() override;

    void setDatabase(DatabaseInterface *database);

Q_SIGNALS:

    void allAlbumsData(const ModelDataLoader::ListAlbumDataType &allData);

    void allArtistsData(const ModelDataLoader::ListArtistDataType &allData);

    void allGenresData(const ModelDataLoader::ListGenreDataType &allData);

    void allTracksData(const ModelDataLoader::ListTrackDataType &allData);

    void allRadiosData(const ModelDataLoader::ListRadioDataType &radiosData);

    void radioAdded(const ModelDataLoader::TrackDataType &radiosData);

    void radioModified(const ModelDataLoader::TrackDataType &radiosData);

    void allTrackData(const ModelDataLoader::TrackDataType &allData);

    void allRadioData(const ModelDataLoader::TrackDataType &allData);

    void tracksAdded(const ModelDataLoader::ListTrackDataType &newData);

    void trackModified(const ModelDataLoader::TrackDataType &modifiedTrack);

    void trackRemoved(qulonglong removedTrackId);

    void genresAdded(const ModelDataLoader::ListGenreDataType &newData);

    void artistsAdded(const ModelDataLoader::ListArtistDataType &newData);

    void artistRemoved(qulonglong removedDatabaseId);

    void albumsAdded(const ModelDataLoader::ListAlbumDataType &newData);

    void albumRemoved(qulonglong removedDatabaseId);

    void albumModified(const ModelDataLoader::AlbumDataType &modifiedAlbum);

    void saveTrackModified(const ModelDataLoader::ListTrackDataType &trackDataType, const QHash<QString, QUrl> &covers);

    void removeRadio(qulonglong radioId);

    void radioRemoved(qulonglong radioId);

    void clearedDatabase();

public Q_SLOTS:

    void loadData(ElisaUtils::PlayListEntryType dataType);

    void loadDataByAlbumId(ElisaUtils::PlayListEntryType dataType, qulonglong databaseId);

    void loadDataByGenre(ElisaUtils::PlayListEntryType dataType,
                         const QString &genre);

    void loadDataByArtist(ElisaUtils::PlayListEntryType dataType,
                          const QString &artist);

    void loadDataByGenreAndArtist(ElisaUtils::PlayListEntryType dataType,
                                  const QString &genre, const QString &artist);

    void loadDataByDatabaseIdAndUrl(ElisaUtils::PlayListEntryType dataType,
                                    qulonglong databaseId, const QUrl &url);

    void loadDataByUrl(ElisaUtils::PlayListEntryType dataType,
                       const QUrl &url);

    void loadRecentlyPlayedData(ElisaUtils::PlayListEntryType dataType);

    void loadFrequentlyPlayedData(ElisaUtils::PlayListEntryType dataType);

    void updateFileMetaData(const DataTypes::TrackDataType &trackDataType, const QUrl &url);

    void updateSingleFileMetaData(const QUrl &url, DataTypes::ColumnsRoles role, const QVariant &data);

    void trackHasBeenModified(ListTrackDataType trackDataType, const QHash<QString, QUrl> &covers);

private Q_SLOTS:

    void databaseTracksAdded(const ModelDataLoader::ListTrackDataType &newData);

    void databaseArtistsAdded(const ModelDataLoader::ListArtistDataType &newData);

    void databaseAlbumsAdded(const ModelDataLoader::ListAlbumDataType &newData);

private:

    std::unique_ptr<ModelDataLoaderPrivate> d;

};

#endif // MODELDATALOADER_H
