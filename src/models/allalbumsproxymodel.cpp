/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "allalbumsproxymodel.h"

#include "databaseinterface.h"

#include <QReadLocker>
#include <QtConcurrentRun>

AllAlbumsProxyModel::AllAlbumsProxyModel(QObject *parent) : AbstractMediaProxyModel(parent)
{
    this->setSortRole(DatabaseInterface::ColumnsRoles::TitleRole);
    this->setSortCaseSensitivity(Qt::CaseInsensitive);
    this->sortModel(Qt::AscendingOrder);
}

AllAlbumsProxyModel::~AllAlbumsProxyModel() = default;

bool AllAlbumsProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    for (int column = 0, columnCount = sourceModel()->columnCount(source_parent); column < columnCount; ++column) {
        auto currentIndex = sourceModel()->index(source_row, column, source_parent);

        const auto &titleValue = sourceModel()->data(currentIndex, DatabaseInterface::ColumnsRoles::TitleRole).toString();
        const auto &artistValue = sourceModel()->data(currentIndex, DatabaseInterface::ColumnsRoles::ArtistRole).toString();
        const auto &allArtistsValue = sourceModel()->data(currentIndex, DatabaseInterface::ColumnsRoles::AllArtistsRole).toStringList();
        const auto maximumRatingValue = sourceModel()->data(currentIndex, DatabaseInterface::ColumnsRoles::HighestTrackRating).toInt();

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

void AllAlbumsProxyModel::genericEnqueueToPlayList(ElisaUtils::PlayListEnqueueMode enqueueMode,
                                                   ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allAlbums = ElisaUtils::EntryDataList{};
        allAlbums.reserve(rowCount());
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            allAlbums.push_back(ElisaUtils::EntryData{data(currentIndex, DatabaseInterface::ColumnsRoles::DatabaseIdRole).toULongLong(),
                                 data(currentIndex, DatabaseInterface::ColumnsRoles::TitleRole).toString()});
        }
        Q_EMIT albumToEnqueue(allAlbums, ElisaUtils::Album, enqueueMode, triggerPlay);
    });
}

void AllAlbumsProxyModel::enqueueToPlayList()
{
    genericEnqueueToPlayList(ElisaUtils::AppendPlayList, ElisaUtils::DoNotTriggerPlay);
}

void AllAlbumsProxyModel::replaceAndPlayOfPlayList()
{
    genericEnqueueToPlayList(ElisaUtils::ReplacePlayList, ElisaUtils::TriggerPlay);
}


#include "moc_allalbumsproxymodel.cpp"
