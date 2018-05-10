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

#ifndef MODELDATACACHE_H
#define MODELDATACACHE_H

#include "elisautils.h"

#include <QObject>

#include <memory>

class ModelDataCachePrivate;
class DatabaseInterface;
class MusicAlbum;

class ModelDataCache : public QObject
{

    Q_OBJECT

    Q_PROPERTY(ElisaUtils::DataType dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)

    Q_PROPERTY(DatabaseInterface* database READ database WRITE setDatabase NOTIFY databaseChanged)

public:

    enum CallMode {
        CannotBlock,
        CanBlock,
    };

    explicit ModelDataCache(QObject *parent = nullptr);

    ElisaUtils::DataType dataType() const;

    int dataCount() const;

    QVariant data(int row, ElisaUtils::ColumnsRoles role) const;

    DatabaseInterface* database() const;

Q_SIGNALS:

    void dataTypeChanged(ElisaUtils::DataType dataType);

    void receiveData(qulonglong databaseId, QMap<ElisaUtils::ColumnsRoles, QVariant> cacheData);

    void databaseChanged(DatabaseInterface* database);

    void dataChanged();

public Q_SLOTS:

    void neededData(qulonglong databaseId) const;

    void setDataType(ElisaUtils::DataType dataType);

    void setDatabase(DatabaseInterface* database);

private Q_SLOTS:

    void databaseContentChanged();

private:

    void fetchPartialData();

    void connectDatabase();

    std::unique_ptr<ModelDataCachePrivate> d;

};

#endif // MODELDATACACHE_H
