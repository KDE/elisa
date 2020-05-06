/*
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "alltracksproxymodel.h"

#include "datatypes.h"

#include <QReadLocker>
#include <QtConcurrent>

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

void AllTracksProxyModel::genericEnqueueToPlayList(ElisaUtils::PlayListEnqueueMode enqueueMode,
                                                   ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allTracks = ElisaUtils::EntryDataList{};
        allTracks.reserve(rowCount());
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            allTracks.push_back(ElisaUtils::EntryData{data(currentIndex, DataTypes::FullDataRole).value<DataTypes::TrackDataType>(),
                                                      data(currentIndex, DataTypes::ColumnsRoles::TitleRole).toString(),
                                                      data(currentIndex, DataTypes::ColumnsRoles::ResourceRole).toUrl()});
        }
        Q_EMIT entriesToEnqueue(allTracks, ElisaUtils::Track, enqueueMode, triggerPlay);
    });
}

void AllTracksProxyModel::enqueueToPlayList()
{
    genericEnqueueToPlayList(ElisaUtils::AppendPlayList, ElisaUtils::DoNotTriggerPlay);
}

void AllTracksProxyModel::replaceAndPlayOfPlayList()
{
    genericEnqueueToPlayList(ElisaUtils::ReplacePlayList, ElisaUtils::TriggerPlay);
}


#include "moc_alltracksproxymodel.cpp"
