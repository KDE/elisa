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
#include "modeldatacache.h"

#include <QList>
#include <QMap>
#include <QVariant>

class GenericDataModelPrivate
{
public:

    QHash<qulonglong, int> mDataPositionCache;

    ModelDataCache *mModelCache = nullptr;

    DataUtils::DataType mDataType = DataUtils::DataType::UnknownType;

    bool mIsBusy = true;

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

    rowCount = d->mModelCache->dataCount();

    return rowCount;
}

QHash<int, QByteArray> GenericDataModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    roles[static_cast<int>(ElisaUtils::ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ShadowForImageRole)] = "shadowForImage";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ChildModelRole)] = "childModel";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::IsPartialDataRole)] = "isPartial";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ContainerDataRole)] = "containerData";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::DatabaseIdRole)] = "databaseId";

    return roles;
}

QVariant GenericDataModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    const auto albumCount = d->mModelCache->dataCount();

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

    if (role != Qt::DisplayRole && (role < ElisaUtils::SecondaryTextRole || role > ElisaUtils::IsPartialDataRole)) {
        return result;
    }

    switch(role)
    {
    case Qt::DisplayRole:
        result = d->mModelCache->data(index.row(), ElisaUtils::TitleRole);
        break;
    default:
        result = d->mModelCache->data(index.row(), static_cast<ElisaUtils::ColumnsRoles>(role));
        break;
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

DataUtils::DataType GenericDataModel::dataType() const
{
    return d->mDataType;
}

ModelDataCache *GenericDataModel::modelCache() const
{
    return d->mModelCache;
}

bool GenericDataModel::isBusy() const
{
    return d->mIsBusy;
}

void GenericDataModel::setDataType(DataUtils::DataType dataType)
{
    if (d->mDataType == dataType) {
        return;
    }

    resetModelType();

    d->mDataType = dataType;
    Q_EMIT dataTypeChanged(d->mDataType);
}

void GenericDataModel::setModelCache(ModelDataCache *modelCache)
{
    if (d->mModelCache == modelCache) {
        return;
    }

    d->mModelCache = modelCache;
    Q_EMIT modelCacheChanged(d->mModelCache);

    connect(this, &GenericDataModel::neededData,
            d->mModelCache, &ModelDataCache::neededData);
    connect(d->mModelCache, &ModelDataCache::receiveData,
            this, &GenericDataModel::receiveData);
    connect(this, &GenericDataModel::dataTypeChanged,
            d->mModelCache, &ModelDataCache::setDataType);
    connect(d->mModelCache, &ModelDataCache::dataChanged,
            this, &GenericDataModel::modelDataChanged);
}

void GenericDataModel::modelDataChanged(int lowerBound, int upperBound)
{
    if (lowerBound == -1 && upperBound == -1) {
        beginResetModel();
        endResetModel();

        d->mIsBusy = false;
        Q_EMIT isBusyChanged(d->mIsBusy);
    }

    Q_EMIT dataChanged(index(lowerBound), index(upperBound), {});
}

void GenericDataModel::resetModelType()
{
    beginResetModel();
    d->mDataPositionCache.clear();

    d->mIsBusy = true;
    Q_EMIT isBusyChanged(d->mIsBusy);

    endResetModel();
}

void GenericDataModel::receiveData(int row)
{
    Q_EMIT dataChanged(index(row), index(row), {});
}


#include "moc_genericdatamodel.cpp"
