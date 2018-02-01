/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
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

#include "singleartistproxymodel.h"

#include "allalbumsmodel.h"

QString SingleArtistProxyModel::artistFilter() const
{
    return mArtistFilter;
}


void SingleArtistProxyModel::setArtistFilterText(const QString &filterText)
{
    if (mArtistFilter == filterText)
        return;

    mArtistFilter = filterText;

    mArtistExpression.setPattern(QStringLiteral(".*") + mArtistFilter + QStringLiteral(".*"));
    mArtistExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    mArtistExpression.optimize();

    invalidate();

    Q_EMIT artistFilterTextChanged(mArtistFilter);
}

bool SingleArtistProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    for (int column = 0, columnCount = sourceModel()->columnCount(source_parent); column < columnCount; ++column) {
        auto currentIndex = sourceModel()->index(source_row, column, source_parent);

        const auto &titleValue = sourceModel()->data(currentIndex, AllAlbumsModel::TitleRole).toString();
        const auto &artistValue = sourceModel()->data(currentIndex, AllAlbumsModel::AllArtistsRole).toString();
        const auto maximumRatingValue = sourceModel()->data(currentIndex, AllAlbumsModel::HighestTrackRating).toInt();

        if (maximumRatingValue < mFilterRating) {
            result = false;
            continue;
        }

        if (mArtistExpression.match(artistValue).hasMatch()) {
            if (mFilterExpression.match(titleValue).hasMatch()) {
                result = true;
                continue;
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

void SingleArtistProxyModel::enqueueToPlayList()
{
    MediaPlayList *playList = AbstractMediaProxyModel::mediaPlayList();
    for (int columnIndex = 0, columnCount = this->columnCount(); columnIndex < columnCount; ++columnIndex) {
        for (int rowIndex = 0, rowCount = this->rowCount(); rowIndex < rowCount; ++rowIndex) {
            auto currentIndex = this->index(rowIndex, columnIndex);
            playList->enqueue(this->data(currentIndex,AllAlbumsModel::ContainerDataRole).value<MusicAlbum>());
        }
    }

}

#include "moc_singleartistproxymodel.cpp"
