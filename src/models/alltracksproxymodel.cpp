/*
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "alltracksproxymodel.h"

#include "datatypes.h"

AllTracksProxyModel::AllTracksProxyModel(QObject *parent) : AbstractMediaProxyModel(parent)
{
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

AllTracksProxyModel::~AllTracksProxyModel() = default;

bool AllTracksProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    auto currentIndex = sourceModel()->index(source_row, 0, source_parent);

    const auto &titleValue = sourceModel()->data(currentIndex, Qt::DisplayRole).toString();
    const auto &artistValue = sourceModel()->data(currentIndex, DataTypes::ColumnsRoles::ArtistRole).toString();
    const auto maximumRatingValue = sourceModel()->data(currentIndex, DataTypes::ColumnsRoles::RatingRole).toInt();

    if (maximumRatingValue < mFilterRating) {
        return result;
    }

    if (mFilterExpression.match(titleValue).hasMatch()) {
        result = true;
    }

    if (mFilterExpression.match(artistValue).hasMatch()) {
        result = true;
    }

    return result;
}


#include "moc_alltracksproxymodel.cpp"
