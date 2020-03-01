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
    sortModel(Qt::AscendingOrder);
}

ElisaUtils::PlayListEntryType GridViewProxyModel::dataType() const
{
    return mDataType;
}

GridViewProxyModel::~GridViewProxyModel() = default;

bool GridViewProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    auto currentIndex = sourceModel()->index(source_row, 0, source_parent);

    const auto &mainValue = sourceModel()->data(currentIndex, Qt::DisplayRole).toString();
    const auto &artistValue = sourceModel()->data(currentIndex, DataTypes::ArtistRole).toString();
    const auto &allArtistsValue = sourceModel()->data(currentIndex, DataTypes::AllArtistsRole).toStringList();
    const auto maximumRatingValue = sourceModel()->data(currentIndex, DataTypes::HighestTrackRating).toInt();

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

void GridViewProxyModel::genericEnqueueToPlayList(ElisaUtils::PlayListEnqueueMode enqueueMode,
                                                  ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allData = DataTypes::EntryDataList{};
        allData.reserve(rowCount());
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);

            allData.push_back(DataTypes::EntryData{data(currentIndex, DataTypes::FullDataRole).value<DataTypes::MusicDataType>(),
                                                   data(currentIndex, Qt::DisplayRole).toString(), {}});
        }
        Q_EMIT entriesToEnqueue(allData, enqueueMode, triggerPlay);
    });
}

void GridViewProxyModel::enqueueToPlayList()
{
    genericEnqueueToPlayList(ElisaUtils::AppendPlayList, ElisaUtils::DoNotTriggerPlay);
}

void GridViewProxyModel::replaceAndPlayOfPlayList()
{
    genericEnqueueToPlayList(ElisaUtils::ReplacePlayList, ElisaUtils::TriggerPlay);
}

void GridViewProxyModel::setDataType(ElisaUtils::PlayListEntryType newDataType)
{
    if (mDataType == newDataType) {
        return;
    }

    mDataType = newDataType;

    Q_EMIT dataTypeChanged();
}


#include "moc_gridviewproxymodel.cpp"
