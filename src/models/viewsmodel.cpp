/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
