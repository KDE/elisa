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

#include "genericdatamodel.h"

#include "databaseinterface.h"

#include <QList>
#include <QMap>
#include <QVariant>

class GenericDataModelPrivate
{
public:

    QList<QMap<DatabaseInterface::PropertyType, QVariant>> mPartialData;

    QHash<qulonglong, int> mDataPositionCache;

    QHash<qulonglong, QMap<ElisaUtils::ColumnsRoles, QVariant>> mDataCache;

};

GenericDataModel::GenericDataModel(QObject *parent)
    : QAbstractListModel(parent), d(std::make_unique<GenericDataModelPrivate>())
{
}

GenericDataModel::~GenericDataModel() = default;

int GenericDataModel::rowCount(const QModelIndex &parent) const
{
    auto rowCount = 0;

    if (parent.isValid()) {
        return rowCount;
    }

    rowCount = d->mPartialData.count();

    return rowCount;
}

QHash<int, QByteArray> GenericDataModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    roles[static_cast<int>(ElisaUtils::ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ShadowForImageRole)] = "shadowForImage";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ChildModelRole)] = "childModel";

    return roles;
}

QVariant GenericDataModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    const auto albumCount = d->mPartialData.size();

    if (!index.isValid()) {
        return result;
    }

    if (index.column() != 0) {
        return result;
    }

    if (index.row() < 0) {
        return result;
    }

    if (index.parent().isValid()) {
        return result;
    }

    if (index.row() < 0 || index.row() >= albumCount) {
        return result;
    }

    if (role != Qt::DisplayRole && (role < ElisaUtils::SecondaryTextRole || role > ElisaUtils::ContainerDataRole)) {
        return result;
    }

    switch(role)
    {
    case Qt::DisplayRole:
        result = d->mPartialData[index.row()][DatabaseInterface::DisplayRole];
        break;
    case ElisaUtils::SecondaryTextRole:
        result = d->mPartialData[index.row()][DatabaseInterface::SecondaryRole];
        break;
    default:
    {
        auto realRole = static_cast<ElisaUtils::ColumnsRoles>(role);
        auto databaseId = d->mPartialData[index.row()][DatabaseInterface::DatabaseId].toULongLong();
        auto itCacheData = d->mDataCache.find(databaseId);
        if (itCacheData == d->mDataCache.end()) {
            d->mDataPositionCache[databaseId] = index.row();
            Q_EMIT neededData(databaseId);
        } else {
            result = itCacheData.value()[realRole];
        }
    }
    };

    return result;
}

QModelIndex GenericDataModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    return {};
}

int GenericDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 1;
}

void GenericDataModel::receiveData(qulonglong databaseId, QMap<ElisaUtils::ColumnsRoles, QVariant> cacheData)
{
    d->mDataCache[databaseId] = cacheData;

    auto modifiedIndex = index(d->mDataPositionCache[databaseId]);

    Q_EMIT dataChanged(modifiedIndex, modifiedIndex, {});
}


#include "moc_genericdatamodel.cpp"
