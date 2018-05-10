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
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include <QAtomicInt>

#include <QDebug>

class ModelDataCachePrivate
{
public:

    QReadWriteLock mLock;

    QList<QMap<DatabaseInterface::PropertyType, QVariant>> mPartialData;

    DatabaseInterface *mDatabase = nullptr;

    QAtomicInt mDataCount;

    ElisaUtils::DataType mDataType = ElisaUtils::UnknownType;

    bool mIsConnected = false;

};

ModelDataCache::ModelDataCache(QObject *parent)
    : QObject(parent), d(std::make_unique<ModelDataCachePrivate>())
{
}

ElisaUtils::DataType ModelDataCache::dataType() const
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
    QReadLocker vLocker(&d->mLock);

    auto result = QVariant{};

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
    };

    return result;
}

DatabaseInterface *ModelDataCache::database() const
{
    return d->mDatabase;
}

void ModelDataCache::neededData(qulonglong databaseId) const
{
}

void ModelDataCache::setDataType(ElisaUtils::DataType dataType)
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

    if (d->mDataType == ElisaUtils::UnknownType) {
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

    {
        QWriteLocker vLocker(&d->mLock);

        d->mPartialData = d->mDatabase->allData(d->mDataType);

        d->mDataCount = d->mPartialData.count();
    }

    Q_EMIT dataChanged();
}

void ModelDataCache::connectDatabase()
{
    if (d->mDataType == ElisaUtils::UnknownType) {
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
    case ElisaUtils::AllAlbums:
        connect(d->mDatabase, &DatabaseInterface::albumsAdded,
                this, &ModelDataCache::databaseContentChanged,
                Qt::QueuedConnection);
        break;
    case ElisaUtils::AllArtists:
        connect(d->mDatabase, &DatabaseInterface::artistsAdded,
                this, &ModelDataCache::databaseContentChanged,
                Qt::QueuedConnection);
        break;
    case ElisaUtils::AllTracks:
        connect(d->mDatabase, &DatabaseInterface::tracksAdded,
                this, &ModelDataCache::databaseContentChanged,
                Qt::QueuedConnection);
        break;
    case ElisaUtils::AllGenres:
        connect(d->mDatabase, &DatabaseInterface::genreAdded,
                this, &ModelDataCache::databaseContentChanged,
                Qt::QueuedConnection);
        break;
    case ElisaUtils::AllComposers:
        connect(d->mDatabase, &DatabaseInterface::composerAdded,
                this, &ModelDataCache::databaseContentChanged,
                Qt::QueuedConnection);
        break;
    case ElisaUtils::AllLyricists:
        connect(d->mDatabase, &DatabaseInterface::lyricistAdded,
                this, &ModelDataCache::databaseContentChanged,
                Qt::QueuedConnection);
        break;
    case ElisaUtils::UnknownType:
        break;
    }

    d->mIsConnected = true;
}


#include "moc_modeldatacache.cpp"
