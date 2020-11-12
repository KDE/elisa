/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "modeldataloader.h"

#include "filescanner.h"
#include "filewriter.h"

#include <QFileInfo>

class ModelDataLoaderPrivate
{
public:

    DatabaseInterface *mDatabase = nullptr;

    ElisaUtils::PlayListEntryType mModelType = ElisaUtils::Unknown;

    ModelDataLoader::FilterType mFilterType = ModelDataLoader::FilterType::UnknownFilter;

    QString mArtist;

    QString mAlbumTitle;

    QString mAlbumArtist;

    QString mGenre;

    qulonglong mDatabaseId = 0;

    FileScanner mFileScanner;

    FileWriter mFileWriter;
};

ModelDataLoader::ModelDataLoader(QObject *parent) : QObject(parent), d(std::make_unique<ModelDataLoaderPrivate>())
{
}

ModelDataLoader::~ModelDataLoader() = default;

void ModelDataLoader::setDatabase(DatabaseInterface *database)
{
    d->mDatabase = database;

    connect(database, &DatabaseInterface::genresAdded,
            this, &ModelDataLoader::genresAdded);
    connect(database, &DatabaseInterface::albumsAdded,
            this, &ModelDataLoader::databaseAlbumsAdded);
    connect(database, &DatabaseInterface::albumModified,
            this, &ModelDataLoader::albumModified);
    connect(database, &DatabaseInterface::albumRemoved,
            this, &ModelDataLoader::albumRemoved);
    connect(database, &DatabaseInterface::tracksAdded,
            this, &ModelDataLoader::databaseTracksAdded);
    connect(database, &DatabaseInterface::trackModified,
            this, &ModelDataLoader::trackModified);
    connect(database, &DatabaseInterface::trackRemoved,
            this, &ModelDataLoader::trackRemoved);
    connect(database, &DatabaseInterface::artistsAdded,
            this, &ModelDataLoader::databaseArtistsAdded);
    connect(database, &DatabaseInterface::artistRemoved,
            this, &ModelDataLoader::artistRemoved);
    connect(this, &ModelDataLoader::saveTrackModified,
            database, &DatabaseInterface::insertTracksList);
    connect(this, &ModelDataLoader::removeRadio,
            database, &DatabaseInterface::removeRadio);
    connect(database, &DatabaseInterface::radioAdded,
            this, &ModelDataLoader::radioAdded);
    connect(database, &DatabaseInterface::radioModified,
            this, &ModelDataLoader::radioModified);
    connect(database, &DatabaseInterface::radioRemoved,
            this, &ModelDataLoader::radioRemoved);
    connect(database, &DatabaseInterface::cleanedDatabase,
            this, &ModelDataLoader::clearedDatabase);
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
    case ElisaUtils::Container:
        break;
    case ElisaUtils::Radio:
        Q_EMIT allRadiosData(d->mDatabase->allRadiosData());
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
    case ElisaUtils::Radio:
    case ElisaUtils::Container:
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
    case ElisaUtils::Radio:
    case ElisaUtils::Container:
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
    case ElisaUtils::Radio:
    case ElisaUtils::Container:
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
    case ElisaUtils::Radio:
    case ElisaUtils::Container:
        break;
    }
}

void ModelDataLoader::loadDataByDatabaseIdAndUrl(ElisaUtils::PlayListEntryType dataType,
                                                 qulonglong databaseId, const QUrl &url)
{
    if (!d->mDatabase) {
        return;
    }

    d->mFilterType = ModelDataLoader::FilterType::FilterById;
    d->mDatabaseId = databaseId;

    switch (dataType)
    {
    case ElisaUtils::Track:
        Q_EMIT allTrackData(d->mDatabase->trackDataFromDatabaseIdAndUrl(databaseId, url));
        break;
    case ElisaUtils::Radio:
        Q_EMIT allRadioData(d->mDatabase->radioDataFromDatabaseId(databaseId));
        break;
    case ElisaUtils::Album:
    case ElisaUtils::Artist:
    case ElisaUtils::Composer:
    case ElisaUtils::Genre:
    case ElisaUtils::Lyricist:
    case ElisaUtils::FileName:
    case ElisaUtils::Unknown:
    case ElisaUtils::Container:
        break;
    }
}

void ModelDataLoader::loadDataByUrl(ElisaUtils::PlayListEntryType dataType, const QUrl &url)
{
    if (!d->mDatabase) {
        return;
    }

    d->mFilterType = ModelDataLoader::FilterType::UnknownFilter;

    switch (dataType)
    {
    case ElisaUtils::FileName:
    case ElisaUtils::Track:
    {
        auto databaseId = d->mDatabase->trackIdFromFileName(url);
        if (databaseId != 0) {
            Q_EMIT allTrackData(d->mDatabase->trackDataFromDatabaseIdAndUrl(databaseId, url));
        } else {
            auto result = d->mFileScanner.scanOneFile(url);
            Q_EMIT allTrackData(result);
        }
        break;
    }
    case ElisaUtils::Radio:
    {
        auto databaseId = d->mDatabase->radioIdFromFileName(url);
        if (databaseId != 0) {
            Q_EMIT allRadioData(d->mDatabase->radioDataFromDatabaseId(databaseId));
        } else {
            auto result = d->mFileScanner.scanOneFile(url);
            Q_EMIT allRadioData(result);
        }
        break;
    }
    case ElisaUtils::Album:
    case ElisaUtils::Artist:
    case ElisaUtils::Composer:
    case ElisaUtils::Genre:
    case ElisaUtils::Lyricist:
    case ElisaUtils::Unknown:
    case ElisaUtils::Container:
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
    case ElisaUtils::Radio:
    case ElisaUtils::Container:
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
    case ElisaUtils::Radio:
    case ElisaUtils::Container:
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
    case ModelDataLoader::FilterType::FilterByPath:
    case ModelDataLoader::FilterType::UnknownFilter:
        break;
    }
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
    case ModelDataLoader::FilterType::FilterByPath:
    case ModelDataLoader::FilterType::UnknownFilter:
        break;
    }
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
    case ModelDataLoader::FilterType::FilterByPath:
    case ModelDataLoader::FilterType::UnknownFilter:
        break;
    }
}

void ModelDataLoader::trackHasBeenModified(ModelDataLoader::ListTrackDataType trackDataType, const QHash<QString, QUrl> &covers)
{
    for(auto &oneTrack : trackDataType) {
        if (oneTrack.elementType() == ElisaUtils::Track) {
            d->mFileWriter.writeAllMetaDataToFile(oneTrack.resourceURI(), oneTrack);

            QFileInfo trackFile{oneTrack.resourceURI().toLocalFile()};

            oneTrack[DataTypes::FileModificationTime] = trackFile.fileTime(QFileDevice::FileModificationTime);

            for (auto itData = oneTrack.begin(); itData != oneTrack.end();) {
                if (itData->isNull()) {
                    itData = oneTrack.erase(itData);
                } else {
                    ++itData;
                }
            }
        }
    }

    Q_EMIT saveTrackModified(trackDataType, covers);
}

void ModelDataLoader::updateFileMetaData(const DataTypes::TrackDataType &trackDataType, const QUrl &url)
{
    d->mFileWriter.writeAllMetaDataToFile(url, trackDataType);
}

void ModelDataLoader::updateSingleFileMetaData(const QUrl &url, DataTypes::ColumnsRoles role, const QVariant &data)
{
    d->mFileWriter.writeSingleMetaDataToFile(url, role, data);
}

#include "moc_modeldataloader.cpp"
