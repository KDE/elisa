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

class ModelDataCachePrivate
{
public:

    QReadWriteLock mLock;

    QList<QMap<DatabaseInterface::PropertyType, QVariant>> mPartialData;

    DatabaseInterface *mDatabase = nullptr;

    QAtomicInt mDataCount;

    ElisaUtils::DataType mDataType;

};

ModelDataCache::ModelDataCache(QObject *parent)
    : QObject(parent), d(std::make_unique<ModelDataCachePrivate>())
{
}

ElisaUtils::DataType ModelDataCache::dataType() const
{
    return d->mDataType;
}

int ModelDataCache::dataCount()
{
    int result = d->mDataCount;

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

    fetchPartialData();
}

void ModelDataCache::setDatabase(DatabaseInterface *database)
{
    if (d->mDatabase == database) {
        return;
    }

    d->mDatabase = database;
    Q_EMIT databaseChanged(d->mDatabase);
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


#include "moc_modeldatacache.cpp"
