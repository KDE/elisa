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

    void saveRadioModified(const ModelDataLoader::TrackDataType &trackDataType);

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

private Q_SLOTS:

    void databaseTracksAdded(const ListTrackDataType &newData);

    void databaseArtistsAdded(const ListArtistDataType &newData);

    void databaseAlbumsAdded(const ListAlbumDataType &newData);

private:

    std::unique_ptr<ModelDataLoaderPrivate> d;

};

#endif // MODELDATALOADER_H
