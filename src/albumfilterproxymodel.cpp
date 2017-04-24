/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "albumfilterproxymodel.h"

#include "allalbumsmodel.h"

AlbumFilterProxyModel::AlbumFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent), mFilterText()
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

AlbumFilterProxyModel::~AlbumFilterProxyModel()
{
}

QString AlbumFilterProxyModel::filterText() const
{
    return mFilterText;
}

int AlbumFilterProxyModel::filterRating() const
{
    return mFilterRating;
}

void AlbumFilterProxyModel::setFilterText(const QString &filterText)
{
    if (mFilterText == filterText)
        return;

    mFilterText = filterText;

    mFilterExpression.setPattern(mFilterText);
    mFilterExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    mFilterExpression.optimize();

    invalidate();

    Q_EMIT filterTextChanged(mFilterText);
}

void AlbumFilterProxyModel::setFilterRating(int filterRating)
{
    if (mFilterRating == filterRating) {
        return;
    }

    mFilterRating = filterRating;

    invalidate();

    Q_EMIT filterRatingChanged(filterRating);
}

bool AlbumFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    for (int column = 0, columnCount = sourceModel()->columnCount(source_parent); column < columnCount; ++column) {
        auto currentIndex = sourceModel()->index(source_row, column, source_parent);

        const auto &titleValue = sourceModel()->data(currentIndex, AllAlbumsModel::TitleRole).toString();
        const auto &artistValue = sourceModel()->data(currentIndex, AllAlbumsModel::ArtistRole).toString();
        const auto &allArtistsValue = sourceModel()->data(currentIndex, AllAlbumsModel::AllArtistsRole).toStringList();
        const auto maximumRatingValue = sourceModel()->data(currentIndex, AllAlbumsModel::HighestTrackRating).toInt();

        if (maximumRatingValue < mFilterRating) {
            result = false;
            continue;
        }

        if (mFilterExpression.match(titleValue).hasMatch()) {
            result = true;
            continue;
        }

        if (mFilterExpression.match(artistValue).hasMatch()) {
            result = true;
            continue;
        }

        for (const auto &oneArtist : allArtistsValue) {
            if (mFilterExpression.match(oneArtist).hasMatch()) {
                result = true;
                break;
            }
        }

        if (result) {
            continue;
        }

        if (!result) {
            break;
        }
    }

    return result;
}


#include "moc_albumfilterproxymodel.cpp"
