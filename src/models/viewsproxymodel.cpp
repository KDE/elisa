/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "viewsproxymodel.h"
#include "viewsmodel.h"

ViewsProxyModel::ViewsProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    sort(0, Qt::AscendingOrder);
}

bool ViewsProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if (source_right.data(ViewsModel::EntryCategoryRole).toString() == QStringLiteral("default")) {
        switch (sortOrder())
        {
        case Qt::AscendingOrder:
            return false;
        case Qt::DescendingOrder:
            return true;
        }
    }

    if (source_left.data(ViewsModel::EntryCategoryRole).toString() == QStringLiteral("default")) {
        switch (sortOrder())
        {
        case Qt::AscendingOrder:
            return true;
        case Qt::DescendingOrder:
            return false;
        }
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}


#include "moc_viewsproxymodel.cpp"
