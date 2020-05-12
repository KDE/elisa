/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "gridviewproxymodel.h"

#include "datatypes.h"
#include "elisautils.h"

#include <QReadLocker>
#include <QtConcurrent>

GridViewProxyModel::GridViewProxyModel(QObject *parent) : AbstractMediaProxyModel(parent)
{
    setSortRole(Qt::DisplayRole);
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

GridViewProxyModel::~GridViewProxyModel() = default;

bool GridViewProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    auto currentIndex = sourceModel()->index(source_row, 0, source_parent);

    const auto &mainValue = sourceModel()->data(currentIndex, Qt::DisplayRole).toString();
    const auto &artistValue = sourceModel()->data(currentIndex, DataTypes::ArtistRole).toString();
    const auto &allArtistsValue = sourceModel()->data(currentIndex, DataTypes::AllArtistsRole).toStringList();
    bool collectionMaximumRatingValueIsValid = false;
    const auto collectionMaximumRatingValue = sourceModel()->data(currentIndex, DataTypes::HighestTrackRating).toInt(&collectionMaximumRatingValueIsValid);
    bool maximumRatingValueIsValid = false;
    const auto maximumRatingValue = sourceModel()->data(currentIndex, DataTypes::RatingRole).toInt(&maximumRatingValueIsValid);

    if ((collectionMaximumRatingValueIsValid && maximumRatingValueIsValid &&
            collectionMaximumRatingValue < mFilterRating && maximumRatingValue < mFilterRating) ||
        (collectionMaximumRatingValueIsValid && !maximumRatingValueIsValid && collectionMaximumRatingValue < mFilterRating) ||
        (!collectionMaximumRatingValueIsValid && maximumRatingValueIsValid && maximumRatingValue < mFilterRating) ||
        (!collectionMaximumRatingValueIsValid && !maximumRatingValueIsValid && mFilterRating)) {
        result = false;
        return result;
    }

    if (mFilterExpression.match(mainValue).hasMatch()) {
        result = true;
        return result;
    }

    if (mFilterExpression.match(artistValue).hasMatch()) {
        result = true;
        return result;
    }

    for (const auto &oneArtist : allArtistsValue) {
        if (mFilterExpression.match(oneArtist).hasMatch()) {
            result = true;
            return result;
        }
    }

    return result;
}


#include "moc_gridviewproxymodel.cpp"
