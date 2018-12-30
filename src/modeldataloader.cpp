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

};

ModelDataLoader::ModelDataLoader(QObject *parent) : QObject(parent), d(std::make_unique<ModelDataLoaderPrivate>())
{
}

ModelDataLoader::~ModelDataLoader() = default;

void ModelDataLoader::setDatabase(DatabaseInterface *database)
{
    d->mDatabase = database;
}

void ModelDataLoader::loadData(ElisaUtils::PlayListEntryType dataType)
{
    if (!d->mDatabase) {
        return;
    }

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

void ModelDataLoader::loadDataByGenre(ElisaUtils::PlayListEntryType dataType, const QString &genre)
{
    if (!d->mDatabase) {
        return;
    }

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


#include "moc_modeldataloader.cpp"
