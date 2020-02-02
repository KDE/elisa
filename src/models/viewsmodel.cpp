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

#include "viewsmodel.h"

#include "viewslistdata.h"

class ViewsModelPrivate
{

public:

    ViewsListData *mViewsData = nullptr;

};

ViewsModel::ViewsModel(QObject *parent)
    : QAbstractListModel(parent), d(std::make_unique<ViewsModelPrivate>())
{
}

ViewsModel::~ViewsModel() = default;

QHash<int, QByteArray> ViewsModel::roleNames() const
{
    auto result = QAbstractListModel::roleNames();

    result[ImageNameRole] = "image";

    return result;
}

Qt::ItemFlags ViewsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

int ViewsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    if (!d->mViewsData) {
        return 0;
    }

    return d->mViewsData->count();
}

QVariant ViewsModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant{};

    if (!d->mViewsData) {
        return result;
    }

    if (!index.isValid()) {
        return result;
    }

    switch(role)
    {
    case Qt::DisplayRole:
        result = d->mViewsData->title(index.row());
        break;
    case ColumnRoles::ImageNameRole:
        result = d->mViewsData->iconUrl(index.row());
        break;
    }

    return result;
}

QModelIndex ViewsModel::index(int row, int column, const QModelIndex &parent) const
{
    auto result = QModelIndex();

    if (column != 0) {
        return result;
    }

    if (parent.isValid()) {
        return result;
    }

    result = createIndex(row, column);

    return result;
}

QModelIndex ViewsModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    auto result = QModelIndex();

    return result;
}

ViewsListData *ViewsModel::viewsData() const
{
    return d->mViewsData;
}

void ViewsModel::setViewsData(ViewsListData *viewsData)
{
    if (d->mViewsData == viewsData) {
        return;
    }

    beginResetModel();
    d->mViewsData = viewsData;
    Q_EMIT viewsDataChanged();
    endResetModel();
}


#include "moc_viewsmodel.cpp"
