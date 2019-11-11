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

#include "singlealbumproxymodel.h"

#include "datatypes.h"

#include <QReadLocker>
#include <QtConcurrentRun>

SingleAlbumProxyModel::SingleAlbumProxyModel(QObject *parent) : AbstractMediaProxyModel(parent)
{
}

SingleAlbumProxyModel::~SingleAlbumProxyModel() = default;

bool SingleAlbumProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    auto currentIndex = sourceModel()->index(source_row, 0, source_parent);

    const auto &titleValue = sourceModel()->data(currentIndex, DataTypes::ColumnsRoles::TitleRole).toString();
    const auto maximumRatingValue = sourceModel()->data(currentIndex, DataTypes::ColumnsRoles::RatingRole).toInt();

    if (maximumRatingValue < mFilterRating) {
        return result;
    }

    if (mFilterExpression.match(titleValue).hasMatch()) {
        result = true;
    }

    return result;
}

void SingleAlbumProxyModel::genericEnqueueToPlayList(ElisaUtils::PlayListEnqueueMode enqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allTracks = ElisaUtils::EntryDataList{};
        allTracks.reserve(rowCount());
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            allTracks.push_back(ElisaUtils::EntryData{data(currentIndex, DataTypes::FullDataRole).value<DataTypes::TrackDataType>(),
                                                      data(currentIndex, DataTypes::ColumnsRoles::TitleRole).toString(), {}});
        }
        Q_EMIT entriesToEnqueue(allTracks, ElisaUtils::Track, enqueueMode, triggerPlay);
    });
}

void SingleAlbumProxyModel::enqueueToPlayList()
{
    genericEnqueueToPlayList(ElisaUtils::AppendPlayList, ElisaUtils::DoNotTriggerPlay);
}

void SingleAlbumProxyModel::replaceAndPlayOfPlayList()
{
    genericEnqueueToPlayList(ElisaUtils::ReplacePlayList, ElisaUtils::TriggerPlay);
}

#include "moc_singlealbumproxymodel.cpp"
