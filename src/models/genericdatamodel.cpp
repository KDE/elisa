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

#include "genericdatamodel.h"

#include "databaseinterface.h"

#include <QList>
#include <QMap>
#include <QVariant>

class GenericDataModelPrivate
{
public:

    QHash<qulonglong, int> mDataPositionCache;

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

    return rowCount;
}

QHash<int, QByteArray> GenericDataModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ShadowForImageRole)] = "shadowForImage";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ChildModelRole)] = "childModel";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::IsPartialDataRole)] = "isPartial";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ContainerDataRole)] = "containerData";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::DatabaseIdRole)] = "databaseId";

    return roles;
}

QVariant GenericDataModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    const auto albumCount = 0;

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

    if (role != Qt::DisplayRole && (role < DatabaseInterface::SecondaryTextRole || role > DatabaseInterface::IsPartialDataRole)) {
        return result;
    }

    switch(role)
    {
    case Qt::DisplayRole:
        break;
    default:
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
