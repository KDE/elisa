/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "gridviewproxymodel.h"

#include "datatypes.h"
#include "elisautils.h"


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
    const auto &albumValue = sourceModel()->data(currentIndex, DataTypes::AlbumRole).toString();
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

    if (mDataType == ElisaUtils::Radio) {
        if (mFilterExpression.match(mainValue.normalized(QString::NormalizationForm_KC)).hasMatch()) {
            result = true;
        }
        return result;
    }

    if (mFilterExpression.match(mainValue.normalized(QString::NormalizationForm_KC)).hasMatch()) {
        result = true;
        return result;
    }

    if (mFilterExpression.match(artistValue.normalized(QString::NormalizationForm_KC)).hasMatch()) {
        result = true;
        return result;
    }

    if (mFilterExpression.match(albumValue.normalized(QString::NormalizationForm_KC)).hasMatch()) {
        result = true;
        return result;
    }

    for (const auto &oneArtist : allArtistsValue) {
        if (mFilterExpression.match(oneArtist.normalized(QString::NormalizationForm_KC)).hasMatch()) {
            result = true;
            return result;
        }
    }

    return result;
}

bool GridViewProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QAbstractItemModel *srcModel = sourceModel();
    int sr = sortRole();
    QVariant leftData = srcModel->data(source_left, sr);
    QVariant rightData = srcModel->data(source_right, sr);

    // Fallback to default implementation when keys are different
    if (leftData.typeId() != rightData.typeId() || leftData != rightData)
        return QSortFilterProxyModel::lessThan(source_left, source_right);

    // Primary sort keys are equal: order by album, then disc number, then track number
    const auto isLessThan = [this](bool lessThanResult) {
        // The proxy swaps the operands when sorting in descending order: invert the
        // comparison so that tracks of a same album stay in their natural order
        return (sortOrder() == Qt::AscendingOrder) ? lessThanResult : !lessThanResult;
    };

    const auto leftAlbumId = srcModel->data(source_left, DataTypes::AlbumIdRole).toULongLong();
    const auto rightAlbumId = srcModel->data(source_right, DataTypes::AlbumIdRole).toULongLong();
    if (leftAlbumId != rightAlbumId) {
        return isLessThan(leftAlbumId < rightAlbumId);
    }

    const auto leftDiscNumber = srcModel->data(source_left, DataTypes::DiscNumberRole).toInt();
    const auto rightDiscNumber = srcModel->data(source_right, DataTypes::DiscNumberRole).toInt();
    if (leftDiscNumber != rightDiscNumber) {
        return isLessThan(leftDiscNumber < rightDiscNumber);
    }

    const auto leftTrackNumber = srcModel->data(source_left, DataTypes::TrackNumberRole).toInt();
    const auto rightTrackNumber = srcModel->data(source_right, DataTypes::TrackNumberRole).toInt();
    if (leftTrackNumber != rightTrackNumber) {
        return isLessThan(leftTrackNumber < rightTrackNumber);
    }

    // Every key is equal: neither item is less than the other
    return false;
}

int GridViewProxyModel::tracksCount() const
{
    int count = 0;

    for (int rowIndex = 0, maxRowCount = sourceModel()->rowCount(); rowIndex < maxRowCount; ++rowIndex) {
        auto currentIndex = sourceModel()->index(rowIndex, 0);
        count += sourceModel()->data(currentIndex, DataTypes::TracksCountRole).toInt();
    }

    return count;
}

#include "moc_gridviewproxymodel.cpp"
