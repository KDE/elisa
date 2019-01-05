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
#include "elisautils.h"

#include <QStringList>
#include <QReadLocker>
#include <QtConcurrentRun>

AllAlbumsProxyModel::AllAlbumsProxyModel(QObject *parent) : AbstractMediaProxyModel(parent)
{
    setSortRole(Qt::DisplayRole);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    sortModel(Qt::AscendingOrder);
}

AllAlbumsProxyModel::~AllAlbumsProxyModel() = default;

bool AllAlbumsProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    auto currentIndex = sourceModel()->index(source_row, 0, source_parent);

    const auto &mainValue = sourceModel()->data(currentIndex, Qt::DisplayRole).toString();
    const auto &artistValue = sourceModel()->data(currentIndex, DatabaseInterface::ArtistRole).toString();
    const auto &allArtistsValue = sourceModel()->data(currentIndex, DatabaseInterface::AllArtistsRole).toStringList();
    const auto maximumRatingValue = sourceModel()->data(currentIndex, DatabaseInterface::HighestTrackRating).toInt();

    if (maximumRatingValue < mFilterRating) {
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

void AllAlbumsProxyModel::genericEnqueueToPlayList(ElisaUtils::PlayListEnqueueMode enqueueMode,
                                                   ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allData = ElisaUtils::EntryDataList{};
        allData.reserve(rowCount());
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            allData.push_back(ElisaUtils::EntryData{data(currentIndex, DatabaseInterface::DatabaseIdRole).toULongLong(),
                                                    data(currentIndex, Qt::DisplayRole).toString()});
        }
        Q_EMIT entriesToEnqueue(allData, ElisaUtils::Album, enqueueMode, triggerPlay);
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
