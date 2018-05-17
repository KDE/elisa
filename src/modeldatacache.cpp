/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "modeldatacache.h"

#include "databaseinterface.h"

#include <QList>
#include <QMap>
#include <QVariant>
#include <QAtomicInt>
#include <QMetaObject>

#include <QDebug>

class ModelDataCachePrivate
{
public:

    QList<QMap<DatabaseInterface::PropertyType, QVariant>> mPartialData;

    QHash<qulonglong, QMap<ElisaUtils::ColumnsRoles, QVariant>> mFullData;

    QHash<qulonglong, int> mRows;

    DatabaseInterface *mDatabase = nullptr;

    QAtomicInt mDataCount;

    QAtomicInt mDataChangedLowerBound;

    QAtomicInt mDataChangedUpperBound;

    DataUtils::DataType mDataType = DataUtils::UnknownType;

    QAtomicInt mHasFullData = false;

    bool mIsConnected = false;

};

ModelDataCache::ModelDataCache(QObject *parent)
    : QObject(parent), d(std::make_unique<ModelDataCachePrivate>())
{
    d->mFullData.reserve(CACHE_SIZE);
}

DataUtils::DataType ModelDataCache::dataType() const
{
    return d->mDataType;
}

int ModelDataCache::dataCount() const
{
    int result = d->mDataCount;

    return result;
}

QVariant ModelDataCache::data(int row, ElisaUtils::ColumnsRoles role) const
{
    auto result = QVariant{};

    bool databaseIdIsOk = false;
    auto databaseId = d->mPartialData[row][DatabaseInterface::DatabaseId].toULongLong(&databaseIdIsOk);
    if (!databaseIdIsOk || databaseId == 0) {
        return result;
    }

    d->mRows[databaseId] = row;

    switch (role)
    {
    case ElisaUtils::TitleRole:
        result = d->mPartialData[row][DatabaseInterface::DisplayRole];
        break;
    case ElisaUtils::SecondaryTextRole:
        result = d->mPartialData[row][DatabaseInterface::SecondaryRole];
        break;
    case ElisaUtils::DatabaseIdRole:
        result = d->mPartialData[row][DatabaseInterface::DatabaseId];
        break;
    case ElisaUtils::IsPartialDataRole:
        result = !d->mHasFullData;
        break;
    default:
        if (d->mHasFullData) {
            result = d->mFullData[databaseId][role];
        } else {
            const auto currentRow = d->mRows[databaseId];
            if (d->mDataChangedLowerBound > currentRow) {
                d->mDataChangedLowerBound = currentRow;
            }
            if (d->mDataChangedUpperBound < currentRow) {
                d->mDataChangedUpperBound = currentRow;
            }
        }
        break;
    };

    return result;
}

DatabaseInterface *ModelDataCache::database() const
{
    return d->mDatabase;
}

void ModelDataCache::neededData()
{
    qDebug() << "ModelDataCache::neededData";

    if (d->mHasFullData) {
        return;
    }

    switch (d->mDataType)
    {
    case DataUtils::AllAlbums:
    {
        auto allData = d->mDatabase->allAlbums();
        for (const auto &data : allData) {
            auto &fullData = d->mFullData[data.databaseId()];
            const auto &albumArt = data.albumArtURI();

            if (albumArt.isValid()) {
                fullData[ElisaUtils::ImageUrlRole] = albumArt;
                fullData[ElisaUtils::ShadowForImageRole] = true;
            } else {
                fullData[ElisaUtils::ImageUrlRole] = QUrl(QStringLiteral("image://icon/media-optical-audio"));
                fullData[ElisaUtils::ShadowForImageRole] = false;
            }

            fullData[ElisaUtils::ContainerDataRole] = QVariant::fromValue(data);
            fullData[ElisaUtils::ArtistRole] = data.artist();
            fullData[ElisaUtils::AllArtistsRole] = data.allArtists();
            fullData[ElisaUtils::HighestTrackRating] = data.highestTrackRating();
        }

        d->mHasFullData = true;
        Q_EMIT dataChanged(d->mDataChangedLowerBound, d->mDataChangedUpperBound);

        break;
    }
    case DataUtils::AllArtists:
    {
        auto allData = d->mDatabase->allArtists();
        for (const auto &data : allData) {
            auto &fullData = d->mFullData[data.databaseId()];

            fullData[ElisaUtils::ContainerDataRole] = QVariant::fromValue(data);
            fullData[ElisaUtils::ImageUrlRole] = QUrl(QStringLiteral("image://icon/view-media-artist"));
            fullData[ElisaUtils::ShadowForImageRole] = false;
        }

        d->mHasFullData = true;
        Q_EMIT dataChanged(d->mDataChangedLowerBound, d->mDataChangedUpperBound);

        break;
    }
    case DataUtils::AllComposers:
    {
        auto allData = d->mDatabase->allComposers();
        for (const auto &data : allData) {
            auto &fullData = d->mFullData[data.databaseId()];

            fullData[ElisaUtils::ContainerDataRole] = QVariant::fromValue(data);
            fullData[ElisaUtils::ImageUrlRole] = QUrl(QStringLiteral("image://icon/view-media-artist"));
            fullData[ElisaUtils::ShadowForImageRole] = false;
        }

        d->mHasFullData = true;
        Q_EMIT dataChanged(d->mDataChangedLowerBound, d->mDataChangedUpperBound);

        break;
    }
    case DataUtils::AllLyricists:
    {
        auto allData = d->mDatabase->allLyricists();
        for (const auto &data : allData) {
            auto &fullData = d->mFullData[data.databaseId()];

            fullData[ElisaUtils::ContainerDataRole] = QVariant::fromValue(data);
            fullData[ElisaUtils::ImageUrlRole] = QUrl(QStringLiteral("image://icon/view-media-artist"));
            fullData[ElisaUtils::ShadowForImageRole] = false;
        }

        d->mHasFullData = true;
        Q_EMIT dataChanged(d->mDataChangedLowerBound, d->mDataChangedUpperBound);

        break;
    }
    case DataUtils::AllGenres:
    {
        auto allData = d->mDatabase->allGenres();
        for (const auto &data : allData) {
            auto &fullData = d->mFullData[data.databaseId()];

            fullData[ElisaUtils::ContainerDataRole] = QVariant::fromValue(data);
            fullData[ElisaUtils::ImageUrlRole] = QUrl(QStringLiteral("image://icon/view-media-genre"));
            fullData[ElisaUtils::ShadowForImageRole] = false;
        }

        d->mHasFullData = true;
        Q_EMIT dataChanged(d->mDataChangedLowerBound, d->mDataChangedUpperBound);

        break;
    }
    case DataUtils::AllTracks:
    {
        auto allData = d->mDatabase->allTracks();
        for (const auto &data : allData) {
            auto &fullData = d->mFullData[data.databaseId()];
            const auto &albumArt = data.albumCover();

            if (albumArt.isValid()) {
                fullData[ElisaUtils::ImageUrlRole] = albumArt;
                fullData[ElisaUtils::ShadowForImageRole] = true;
            } else {
                fullData[ElisaUtils::ImageUrlRole] = QUrl(QStringLiteral("image://icon/media-optical-audio"));
                fullData[ElisaUtils::ShadowForImageRole] = false;
            }

            fullData[ElisaUtils::ContainerDataRole] = QVariant::fromValue(data);
            fullData[ElisaUtils::ArtistRole] = data.artist();
            fullData[ElisaUtils::RatingRole] = data.rating();
        }

        d->mHasFullData = true;
        Q_EMIT dataChanged(d->mDataChangedLowerBound, d->mDataChangedUpperBound);

        break;
    }
    case DataUtils::UnknownType:
        break;
    }
}

void ModelDataCache::setDataType(DataUtils::DataType dataType)
{
    if (d->mDataType == dataType) {
        return;
    }

    d->mDataType = dataType;
    Q_EMIT dataTypeChanged(d->mDataType);

    if (!d->mDatabase) {
        return;
    }

    connectDatabase();
    fetchPartialData();
}

void ModelDataCache::setDatabase(DatabaseInterface *database)
{
    if (d->mDatabase == database) {
        return;
    }

    d->mDatabase = database;
    Q_EMIT databaseChanged(d->mDatabase);

    if (d->mDataType == DataUtils::UnknownType) {
        return;
    }

    connectDatabase();
    fetchPartialData();
}

void ModelDataCache::databaseContentChanged()
{
    fetchPartialData();
}

void ModelDataCache::fetchPartialData()
{
    if (!d->mDatabase) {
        return;
    }

    d->mPartialData = d->mDatabase->allData(d->mDataType);

    d->mDataCount = d->mPartialData.count();

    d->mDataChangedLowerBound = d->mDataCount;
    d->mDataChangedUpperBound = 0;

    d->mHasFullData = false;

    QMetaObject::invokeMethod(const_cast<ModelDataCache*>(this), "neededData",
                              Qt::QueuedConnection);

    Q_EMIT dataChanged(-1, -1);
}

void ModelDataCache::connectDatabase()
{
    if (d->mDataType == DataUtils::UnknownType) {
        return;
    }

    if (!d->mDatabase) {
        return;
    }

    if (d->mIsConnected) {
        disconnect(d->mDatabase, nullptr, this, nullptr);
        d->mIsConnected = false;
    }

    switch(d->mDataType)
    {
    case DataUtils::AllAlbums:
        connect(d->mDatabase, &DatabaseInterface::albumsAdded,
                this, &ModelDataCache::databaseContentChanged,
                Qt::QueuedConnection);
        break;
    case DataUtils::AllArtists:
        connect(d->mDatabase, &DatabaseInterface::artistsAdded,
                this, &ModelDataCache::databaseContentChanged,
                Qt::QueuedConnection);
        break;
    case DataUtils::AllTracks:
        connect(d->mDatabase, &DatabaseInterface::tracksAdded,
                this, &ModelDataCache::databaseContentChanged,
                Qt::QueuedConnection);
        break;
    case DataUtils::AllGenres:
        connect(d->mDatabase, &DatabaseInterface::genresAdded,
                this, &ModelDataCache::databaseContentChanged,
                Qt::QueuedConnection);
        break;
    case DataUtils::AllComposers:
        connect(d->mDatabase, &DatabaseInterface::composersAdded,
                this, &ModelDataCache::databaseContentChanged,
                Qt::QueuedConnection);
        break;
    case DataUtils::AllLyricists:
        connect(d->mDatabase, &DatabaseInterface::lyricistsAdded,
                this, &ModelDataCache::databaseContentChanged,
                Qt::QueuedConnection);
        break;
    case DataUtils::UnknownType:
        break;
    }

    d->mIsConnected = true;
}


#include "moc_modeldatacache.cpp"
