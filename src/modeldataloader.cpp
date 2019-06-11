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

#include "modeldataloader.h"

#include "filescanner.h"

#include <QMimeDatabase>

class ModelDataLoaderPrivate
{
public:

    DatabaseInterface *mDatabase = nullptr;

    QMimeDatabase mMimeDatabase;

    FileScanner mFileScanner;

    ElisaUtils::PlayListEntryType mModelType = ElisaUtils::Unknown;

    ModelDataLoader::FilterType mFilterType = ModelDataLoader::FilterType::UnknownFilter;

    QString mArtist;

    QString mAlbumTitle;

    QString mAlbumArtist;

    QString mGenre;

    qulonglong mDatabaseId = 0;

};

ModelDataLoader::ModelDataLoader(QObject *parent) : QObject(parent), d(std::make_unique<ModelDataLoaderPrivate>())
{
}

ModelDataLoader::~ModelDataLoader() = default;

void ModelDataLoader::setDatabase(DatabaseInterface *database)
{
    d->mDatabase = database;

    connect(database, &DatabaseInterface::genresAdded,
            this, &ModelDataLoader::databaseGenresAdded);
    connect(database, &DatabaseInterface::albumsAdded,
            this, &ModelDataLoader::databaseAlbumsAdded);
    connect(database, &DatabaseInterface::albumModified,
            this, &ModelDataLoader::databaseAlbumModified);
    connect(database, &DatabaseInterface::albumRemoved,
            this, &ModelDataLoader::databaseAlbumRemoved);
    connect(database, &DatabaseInterface::tracksAdded,
            this, &ModelDataLoader::databaseTracksAdded);
    connect(database, &DatabaseInterface::trackModified,
            this, &ModelDataLoader::databaseTrackModified);
    connect(database, &DatabaseInterface::trackRemoved,
            this, &ModelDataLoader::databaseTrackRemoved);
    connect(database, &DatabaseInterface::artistsAdded,
            this, &ModelDataLoader::databaseArtistsAdded);
    connect(database, &DatabaseInterface::artistRemoved,
            this, &ModelDataLoader::databaseArtistRemoved);
}

void ModelDataLoader::loadData(ElisaUtils::PlayListEntryType dataType)
{
    if (!d->mDatabase) {
        return;
    }

    d->mFilterType = ModelDataLoader::FilterType::NoFilter;

    switch (dataType)
    {
    case ElisaUtils::Album:
        Q_EMIT allAlbumsData(d->mDatabase->allAlbumsData());
        break;
    case ElisaUtils::Artist:
        Q_EMIT allArtistsData(d->mDatabase->allArtistsData());
        break;
    case ElisaUtils::Composer:
        break;
    case ElisaUtils::Genre:
        Q_EMIT allGenresData(d->mDatabase->allGenresData());
        break;
    case ElisaUtils::Lyricist:
        break;
    case ElisaUtils::Track:
        Q_EMIT allTracksData(d->mDatabase->allTracksData());
        break;
    case ElisaUtils::FileName:
    case ElisaUtils::Unknown:
        break;
    }
}

void ModelDataLoader::loadDataByAlbumId(ElisaUtils::PlayListEntryType dataType, qulonglong databaseId)
{
    if (!d->mDatabase) {
        return;
    }

    d->mFilterType = ModelDataLoader::FilterType::FilterById;
    d->mDatabaseId = databaseId;

    switch (dataType)
    {
    case ElisaUtils::Album:
        break;
    case ElisaUtils::Artist:
        break;
    case ElisaUtils::Composer:
        break;
    case ElisaUtils::Genre:
        break;
    case ElisaUtils::Lyricist:
        break;
    case ElisaUtils::Track:
        Q_EMIT allTracksData(d->mDatabase->albumData(databaseId));
        break;
    case ElisaUtils::FileName:
    case ElisaUtils::Unknown:
        break;
    }
}

void ModelDataLoader::loadDataByGenre(ElisaUtils::PlayListEntryType dataType, const QString &genre)
{
    if (!d->mDatabase) {
        return;
    }

    d->mFilterType = ModelDataLoader::FilterType::FilterByGenre;
    d->mGenre = genre;

    switch (dataType)
    {
    case ElisaUtils::Artist:
        Q_EMIT allArtistsData(d->mDatabase->allArtistsDataByGenre(genre));
        break;
    case ElisaUtils::Album:
    case ElisaUtils::Composer:
    case ElisaUtils::Track:
    case ElisaUtils::Genre:
    case ElisaUtils::Lyricist:
    case ElisaUtils::FileName:
    case ElisaUtils::Unknown:
        break;
    }
}

void ModelDataLoader::loadDataByArtist(ElisaUtils::PlayListEntryType dataType, const QString &artist)
{
    if (!d->mDatabase) {
        return;
    }

    d->mFilterType = ModelDataLoader::FilterType::FilterByArtist;
    d->mArtist = artist;

    switch (dataType)
    {
    case ElisaUtils::Album:
        Q_EMIT allAlbumsData(d->mDatabase->allAlbumsDataByArtist(artist));
        break;
    case ElisaUtils::Artist:
    case ElisaUtils::Composer:
    case ElisaUtils::Track:
    case ElisaUtils::Genre:
    case ElisaUtils::Lyricist:
    case ElisaUtils::FileName:
    case ElisaUtils::Unknown:
        break;
    }
}

void ModelDataLoader::loadDataByGenreAndArtist(ElisaUtils::PlayListEntryType dataType, const QString &genre, const QString &artist)
{
    if (!d->mDatabase) {
        return;
    }

    d->mFilterType = ModelDataLoader::FilterType::FilterByGenreAndArtist;
    d->mArtist = artist;
    d->mGenre = genre;

    switch (dataType)
    {
    case ElisaUtils::Album:
        Q_EMIT allAlbumsData(d->mDatabase->allAlbumsDataByGenreAndArtist(genre, artist));
        break;
    case ElisaUtils::Artist:
    case ElisaUtils::Composer:
    case ElisaUtils::Genre:
    case ElisaUtils::Lyricist:
    case ElisaUtils::Track:
    case ElisaUtils::FileName:
    case ElisaUtils::Unknown:
        break;
    }
}

void ModelDataLoader::loadDataByDatabaseId(ElisaUtils::PlayListEntryType dataType, qulonglong databaseId)
{
    if (!d->mDatabase) {
        return;
    }

    d->mFilterType = ModelDataLoader::FilterType::FilterById;
    d->mDatabaseId = databaseId;

    switch (dataType)
    {
    case ElisaUtils::Track:
        Q_EMIT allTrackData(d->mDatabase->trackDataFromDatabaseId(databaseId));
        break;
    case ElisaUtils::Album:
    case ElisaUtils::Artist:
    case ElisaUtils::Composer:
    case ElisaUtils::Genre:
    case ElisaUtils::Lyricist:
    case ElisaUtils::FileName:
    case ElisaUtils::Unknown:
        break;
    }
}

void ModelDataLoader::loadDataByFileName(ElisaUtils::PlayListEntryType dataType, const QUrl &fileName)
{
    if (!d->mDatabase) {
        return;
    }

    d->mFilterType = ModelDataLoader::FilterType::UnknownFilter;

    switch (dataType)
    {
    case ElisaUtils::FileName:
    {
        auto result = d->mFileScanner.scanOneFile(fileName, d->mMimeDatabase);
        Q_EMIT allTrackData(result.toTrackData());
        break;
    }
    case ElisaUtils::Track:
    case ElisaUtils::Album:
    case ElisaUtils::Artist:
    case ElisaUtils::Composer:
    case ElisaUtils::Genre:
    case ElisaUtils::Lyricist:
    case ElisaUtils::Unknown:
        break;
    }
}

void ModelDataLoader::loadRecentlyPlayedData(ElisaUtils::PlayListEntryType dataType)
{
    if (!d->mDatabase) {
        return;
    }

    d->mFilterType = ModelDataLoader::FilterType::FilterByRecentlyPlayed;

    switch (dataType)
    {
    case ElisaUtils::Track:
        Q_EMIT allTracksData(d->mDatabase->recentlyPlayedTracksData(50));
        break;
    case ElisaUtils::Album:
    case ElisaUtils::Artist:
    case ElisaUtils::Composer:
    case ElisaUtils::Genre:
    case ElisaUtils::Lyricist:
    case ElisaUtils::FileName:
    case ElisaUtils::Unknown:
        break;
    }
}

void ModelDataLoader::loadFrequentlyPlayedData(ElisaUtils::PlayListEntryType dataType)
{
    if (!d->mDatabase) {
        return;
    }

    d->mFilterType = ModelDataLoader::FilterType::FilterByFrequentlyPlayed;

    switch (dataType)
    {
    case ElisaUtils::Track:
        Q_EMIT allTracksData(d->mDatabase->frequentlyPlayedTracksData(50));
        break;
    case ElisaUtils::Album:
    case ElisaUtils::Artist:
    case ElisaUtils::Composer:
    case ElisaUtils::Genre:
    case ElisaUtils::Lyricist:
    case ElisaUtils::FileName:
    case ElisaUtils::Unknown:
        break;
    }
}

void ModelDataLoader::databaseTracksAdded(const ListTrackDataType &newData)
{
    switch(d->mFilterType) {
    case ModelDataLoader::FilterType::NoFilter:
        Q_EMIT tracksAdded(newData);
        break;
    case ModelDataLoader::FilterType::FilterById:
    {
        auto filteredData = newData;

        auto new_end = std::remove_if(filteredData.begin(), filteredData.end(),
                                      [&](const auto &oneTrack) { return oneTrack.albumId() != d->mDatabaseId; });
        filteredData.erase(new_end, filteredData.end());

        Q_EMIT tracksAdded(filteredData);
        break;
    }
    case ModelDataLoader::FilterType::FilterByGenre:
    case ModelDataLoader::FilterType::FilterByGenreAndArtist:
    case ModelDataLoader::FilterType::FilterByArtist:
    case ModelDataLoader::FilterType::FilterByRecentlyPlayed:
    case ModelDataLoader::FilterType::FilterByFrequentlyPlayed:
    case ModelDataLoader::FilterType::UnknownFilter:
        break;
    }
}

void ModelDataLoader::databaseTrackModified(const TrackDataType &modifiedTrack)
{
    Q_EMIT trackModified(modifiedTrack);
}

void ModelDataLoader::databaseTrackRemoved(qulonglong removedTrackId)
{
    Q_EMIT trackRemoved(removedTrackId);
}

void ModelDataLoader::databaseGenresAdded(const ListGenreDataType &newData)
{
    Q_EMIT genresAdded(newData);
}

void ModelDataLoader::databaseArtistsAdded(const ListArtistDataType &newData)
{
    switch(d->mFilterType) {
    case ModelDataLoader::FilterType::FilterByGenre:
    {
        auto filteredData = newData;
        auto new_end = std::remove_if(filteredData.begin(), filteredData.end(),
                                      [&](const auto &oneArtist){return !d->mDatabase->internalArtistMatchGenre(oneArtist.databaseId(), d->mGenre);});
        filteredData.erase(new_end, filteredData.end());

        Q_EMIT artistsAdded(filteredData);

        break;
    }
    case ModelDataLoader::FilterType::NoFilter:
        Q_EMIT artistsAdded(newData);
        break;
    case ModelDataLoader::FilterType::FilterByGenreAndArtist:
    case ModelDataLoader::FilterType::FilterByArtist:
    case ModelDataLoader::FilterType::FilterById:
    case ModelDataLoader::FilterType::FilterByRecentlyPlayed:
    case ModelDataLoader::FilterType::FilterByFrequentlyPlayed:
    case ModelDataLoader::FilterType::UnknownFilter:
        break;
    }
}

void ModelDataLoader::databaseArtistRemoved(qulonglong removedDatabaseId)
{
    Q_EMIT artistRemoved(removedDatabaseId);
}

void ModelDataLoader::databaseAlbumsAdded(const ListAlbumDataType &newData)
{
    switch(d->mFilterType) {
    case ModelDataLoader::FilterType::FilterByArtist:
    {
        auto filteredData = newData;
        auto new_end = std::remove_if(filteredData.begin(), filteredData.end(),
                                      [&](const auto &oneAlbum){return oneAlbum.artist() != d->mArtist;});
        filteredData.erase(new_end, filteredData.end());

        Q_EMIT albumsAdded(filteredData);

        break;
    }
    case ModelDataLoader::FilterType::NoFilter:
        Q_EMIT albumsAdded(newData);
        break;
    case ModelDataLoader::FilterType::FilterByGenreAndArtist:
    {
        auto filteredData = newData;
        auto new_end = std::remove_if(filteredData.begin(), filteredData.end(),
                                      [&](const auto &oneAlbum){
                                        const auto &allGenres = oneAlbum.genres();
                                        return oneAlbum.artist() != d->mArtist || !allGenres.contains(d->mGenre);
                                      });
        filteredData.erase(new_end, filteredData.end());

        Q_EMIT albumsAdded(filteredData);

        break;
    }
    case ModelDataLoader::FilterType::FilterByGenre:
    case ModelDataLoader::FilterType::FilterById:
    case ModelDataLoader::FilterType::FilterByRecentlyPlayed:
    case ModelDataLoader::FilterType::FilterByFrequentlyPlayed:
    case ModelDataLoader::FilterType::UnknownFilter:
        break;
    }
}

void ModelDataLoader::databaseAlbumRemoved(qulonglong removedDatabaseId)
{
    Q_EMIT albumRemoved(removedDatabaseId);
}

void ModelDataLoader::databaseAlbumModified(const AlbumDataType &modifiedAlbum)
{
    Q_EMIT albumModified(modifiedAlbum);
}


#include "moc_modeldataloader.cpp"
