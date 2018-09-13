/*
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
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

#include "alltracksproxymodel.h"

#include "genericdatamodel.h"

#include <QReadLocker>
#include <QtConcurrentRun>

AllTracksProxyModel::AllTracksProxyModel(QObject *parent) : AbstractMediaProxyModel(parent)
{
    this->setSortRole(Qt::DisplayRole);
    this->setSortCaseSensitivity(Qt::CaseInsensitive);
    this->sortModel(Qt::AscendingOrder);
}

AllTracksProxyModel::~AllTracksProxyModel()
{
}

bool AllTracksProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    for (int column = 0, columnCount = sourceModel()->columnCount(source_parent); column < columnCount; ++column) {
        auto currentIndex = sourceModel()->index(source_row, column, source_parent);

        const auto &genreValue = sourceModel()->data(currentIndex, ElisaUtils::ColumnsRoles::GenreRole);

        if (!genreFilterText().isNull() && !genreValue.isValid()) {
            continue;
        }

        if (!genreFilterText().isNull() && !genreValue.canConvert<QStringList>()) {
            continue;
        }

        if (!genreFilterText().isNull() && !genreValue.toStringList().contains(genreFilterText())) {
            continue;
        }

        const auto &titleValue = sourceModel()->data(currentIndex, Qt::DisplayRole).toString();
        const auto &artistValue = sourceModel()->data(currentIndex, ElisaUtils::ColumnsRoles::ArtistRole).toString();
        const auto maximumRatingValue = sourceModel()->data(currentIndex, ElisaUtils::ColumnsRoles::RatingRole).toInt();

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

        if (result) {
            continue;
        }

        if (!result) {
            break;
        }
    }

    return result;
}

void AllTracksProxyModel::enqueueToPlayList()
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allTracks = QList<MusicAudioTrack>();
        allTracks.reserve(rowCount());
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            allTracks.push_back(data(currentIndex, ElisaUtils::ColumnsRoles::ContainerDataRole).value<MusicAudioTrack>());
        }
        Q_EMIT trackToEnqueue(allTracks,
                              ElisaUtils::AppendPlayList,
                              ElisaUtils::DoNotTriggerPlay);
    });
}

void AllTracksProxyModel::replaceAndPlayOfPlayList()
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allTracks = QList<MusicAudioTrack>();
        allTracks.reserve(rowCount());
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            allTracks.push_back(data(currentIndex, ElisaUtils::ColumnsRoles::ContainerDataRole).value<MusicAudioTrack>());
        }
        Q_EMIT trackToEnqueue(allTracks,
                              ElisaUtils::ReplacePlayList,
                              ElisaUtils::TriggerPlay);
    });
}


#include "moc_alltracksproxymodel.cpp"
