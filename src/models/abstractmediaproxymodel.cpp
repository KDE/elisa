/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "abstractmediaproxymodel.h"

#include "mediaplaylistproxymodel.h"

#include <QWriteLocker>
#include <QReadLocker>
#include <QtConcurrentRun>

AbstractMediaProxyModel::AbstractMediaProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    mThreadPool.setMaxThreadCount(1);

    connect(&mEnqueueWatcher, &QFutureWatcher<void>::finished, this, &AbstractMediaProxyModel::afterPlaylistEnqueue);
}

AbstractMediaProxyModel::~AbstractMediaProxyModel()
{
    disconnect(&mEnqueueWatcher, &QFutureWatcher<void>::finished, this, &AbstractMediaProxyModel::afterPlaylistEnqueue);
};

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

    mFilterExpression.setPattern(mFilterText.normalized(QString::NormalizationForm_KC));
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

ElisaUtils::PlayListEntryType AbstractMediaProxyModel::dataType() const
{
    return mDataType;
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

void AbstractMediaProxyModel::setDataType(ElisaUtils::PlayListEntryType dataType)
{
    if (mDataType == dataType) {
        return;
    }

    mDataType = dataType;
    Q_EMIT dataTypeChanged();
}

void AbstractMediaProxyModel::setPlayList(MediaPlayListProxyModel *playList)
{
    if (mPlayList == playList) {
        return;
    }

    disconnectPlayList();

    mPlayList = playList;
    Q_EMIT playListChanged();

    connectPlayList();
}

void AbstractMediaProxyModel::disconnectPlayList()
{
    if (mPlayList) {
        disconnect(this, &AbstractMediaProxyModel::entriesToEnqueue,
                   mPlayList, static_cast<void(MediaPlayListProxyModel::*)(const DataTypes::EntryDataList&, ElisaUtils::PlayListEnqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::enqueue));
        disconnect(this, &AbstractMediaProxyModel::switchToTrackUrl,
                   mPlayList, static_cast<void(MediaPlayListProxyModel::*)(const QUrl &url, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::switchToTrackUrl));
    }
}

void AbstractMediaProxyModel::connectPlayList()
{
    if (mPlayList) {
        connect(this, &AbstractMediaProxyModel::entriesToEnqueue,
                mPlayList, static_cast<void(MediaPlayListProxyModel::*)(const DataTypes::EntryDataList&, ElisaUtils::PlayListEnqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::enqueue));
        connect(this, &AbstractMediaProxyModel::switchToTrackUrl,
                mPlayList, static_cast<void(MediaPlayListProxyModel::*)(const QUrl &url, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::switchToTrackUrl));
    }
}

QFuture<void> AbstractMediaProxyModel::genericEnqueueToPlayList(const QModelIndex &rootIndex,
                                                       ElisaUtils::PlayListEnqueueMode enqueueMode,
                                                       ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    return QtConcurrent::run(&mThreadPool, [=, this] () {
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

void AbstractMediaProxyModel::enqueueAll(ElisaUtils::PlayListEnqueueMode enqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    genericEnqueueToPlayList(QModelIndex(), enqueueMode, triggerPlay);
}

void AbstractMediaProxyModel::replaceAndPlayOfPlayListFromTrackUrl(const QModelIndex &rootIndex, const QUrl &switchTrackUrl)
{
    auto future = genericEnqueueToPlayList(rootIndex, ElisaUtils::ReplacePlayList, ElisaUtils::DoNotTriggerPlay);

    // Wait until the future is finished before switching tracks
    mEnqueueWatcher.setFuture(future);
    mEnqueueWatcherTrackUrl = switchTrackUrl;
}

void AbstractMediaProxyModel::afterPlaylistEnqueue()
{
    if (mEnqueueWatcherTrackUrl.isEmpty()) {
        return;
    }

    Q_EMIT switchToTrackUrl(mEnqueueWatcherTrackUrl, ElisaUtils::TriggerPlay);

    // Reset
    mEnqueueWatcherTrackUrl = QUrl();
}

void AbstractMediaProxyModel::enqueue(const DataTypes::MusicDataType &newEntry, const QString &newEntryTitle, ElisaUtils::PlayListEnqueueMode enqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    auto allData = DataTypes::EntryDataList{};

    allData.push_back(DataTypes::EntryData{newEntry, newEntryTitle, {}});

    Q_EMIT entriesToEnqueue(allData, enqueueMode, triggerPlay);
}


#include "moc_abstractmediaproxymodel.cpp"
