/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "abstractmediaproxymodel.h"

#include "mediaplaylistproxymodel.h"

#include <QWriteLocker>
#include <QReadLocker>
#include <QtConcurrent>

AbstractMediaProxyModel::AbstractMediaProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    mThreadPool.setMaxThreadCount(1);
}

AbstractMediaProxyModel::~AbstractMediaProxyModel()
= default;

QString AbstractMediaProxyModel::filterText() const
{
    return mFilterText;
}

int AbstractMediaProxyModel::filterRating() const
{
    return mFilterRating;
}

void AbstractMediaProxyModel::setFilterText(const QString &filterText)
{
    QWriteLocker writeLocker(&mDataLock);

    if (mFilterText == filterText)
        return;

    mFilterText = filterText;

    mFilterExpression.setPattern(mFilterText);
    mFilterExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    mFilterExpression.optimize();

    invalidate();

    Q_EMIT filterTextChanged(mFilterText);
}

void AbstractMediaProxyModel::setFilterRating(int filterRating)
{
    QWriteLocker writeLocker(&mDataLock);

    if (mFilterRating == filterRating) {
        return;
    }

    mFilterRating = filterRating;

    invalidate();

    Q_EMIT filterRatingChanged(filterRating);
}

bool AbstractMediaProxyModel::sortedAscending() const
{
    return sortOrder() ? false : true;
}

MediaPlayListProxyModel *AbstractMediaProxyModel::playList() const
{
    return mPlayList;
}

void AbstractMediaProxyModel::sortModel(Qt::SortOrder order)
{
    sort(0, order);
    Q_EMIT sortedAscendingChanged();
}

void AbstractMediaProxyModel::setPlayList(MediaPlayListProxyModel *playList)
{
    disconnectPlayList();

    if (mPlayList == playList) {
        return;
    }

    mPlayList = playList;
    Q_EMIT playListChanged();

    connectPlayList();
}

void AbstractMediaProxyModel::disconnectPlayList()
{
    if (mPlayList) {
        disconnect(this, &AbstractMediaProxyModel::entriesToEnqueue,
                   mPlayList, static_cast<void(MediaPlayListProxyModel::*)(const DataTypes::EntryDataList&, ElisaUtils::PlayListEnqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::enqueue));
    }
}

void AbstractMediaProxyModel::connectPlayList()
{
    if (mPlayList) {
        connect(this, &AbstractMediaProxyModel::entriesToEnqueue,
                mPlayList, static_cast<void(MediaPlayListProxyModel::*)(const DataTypes::EntryDataList&, ElisaUtils::PlayListEnqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::enqueue));
    }
}
void AbstractMediaProxyModel::genericEnqueueToPlayList(const QModelIndex &rootIndex,
                                                       ElisaUtils::PlayListEnqueueMode enqueueMode,
                                                       ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allData = DataTypes::EntryDataList{};
        allData.reserve(rowCount());
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0, rootIndex);

            allData.push_back(DataTypes::EntryData{data(currentIndex, DataTypes::FullDataRole).value<DataTypes::MusicDataType>(),
                                                   data(currentIndex, Qt::DisplayRole).toString(), {}});
        }
        Q_EMIT entriesToEnqueue(allData, enqueueMode, triggerPlay);
    });
}

void AbstractMediaProxyModel::enqueueToPlayList(const QModelIndex &rootIndex)
{
    genericEnqueueToPlayList(rootIndex, ElisaUtils::AppendPlayList, ElisaUtils::DoNotTriggerPlay);
}

void AbstractMediaProxyModel::replaceAndPlayOfPlayList(const QModelIndex &rootIndex)
{
    genericEnqueueToPlayList(rootIndex, ElisaUtils::ReplacePlayList, ElisaUtils::TriggerPlay);
}

void AbstractMediaProxyModel::enqueue(const DataTypes::MusicDataType &newEntry, const QString &newEntryTitle, ElisaUtils::PlayListEnqueueMode enqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    auto allData = DataTypes::EntryDataList{};

    allData.push_back(DataTypes::EntryData{newEntry, newEntryTitle, {}});

    Q_EMIT entriesToEnqueue(allData, enqueueMode, triggerPlay);
}


#include "moc_abstractmediaproxymodel.cpp"
