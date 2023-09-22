/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "viewsproxymodel.h"
#include "viewsmodel.h"

class ViewsProxyModelPrivate
{
public:
    ElisaUtils::PlayListEntryType mEmbeddedCategory = ElisaUtils::Unknown;
};

ViewsProxyModel::ViewsProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , d{std::make_unique<ViewsProxyModelPrivate>()}
{
    setSortCaseSensitivity(Qt::CaseInsensitive);
    sort(0, Qt::AscendingOrder);
}

ViewsProxyModel::~ViewsProxyModel() = default;

ElisaUtils::PlayListEntryType ViewsProxyModel::embeddedCategory() const
{
    return d->mEmbeddedCategory;
}

void ViewsProxyModel::setEmbeddedCategory(const ElisaUtils::PlayListEntryType category)
{
    if (d->mEmbeddedCategory != category) {
        d->mEmbeddedCategory = category;
        invalidate();
        Q_EMIT embeddedCategoryChanged();
    }
}

int ViewsProxyModel::mapRowToSource(int row) const
{
    return mapToSource(index(row, 0)).row();
}

int ViewsProxyModel::mapRowFromSource(int row) const
{
    return mapFromSource(sourceModel()->index(row, 0)).row();
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

bool ViewsProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const auto currentIndex = sourceModel()->index(source_row, 0, source_parent);

    const bool isBaseView = sourceModel()->data(currentIndex, ViewsModel::EntryCategoryRole).toString() == QStringLiteral("default");
    if (!isBaseView) {
        return true;
    }

    const bool haveEmbeddedCategory = d->mEmbeddedCategory != ElisaUtils::Unknown;
    if (!haveEmbeddedCategory) {
        return true;
    }

    const auto dataType = sourceModel()->data(currentIndex, ViewsModel::DataTypeRole).value<ElisaUtils::PlayListEntryType>();
    const bool isEmbeddedCategory = dataType != d->mEmbeddedCategory;
    return isEmbeddedCategory;
}


#include "moc_viewsproxymodel.cpp"
