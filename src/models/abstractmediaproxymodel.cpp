/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "abstractmediaproxymodel.h"

#include "mediaplaylistproxymodel.h"

#include <QWriteLocker>

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
    this->sort(0, order);
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
                   mPlayList, static_cast<void(MediaPlayListProxyModel::*)(const ElisaUtils::EntryDataList&, ElisaUtils::PlayListEntryType, ElisaUtils::PlayListEnqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::enqueue));
    }
}

void AbstractMediaProxyModel::connectPlayList()
{
    if (mPlayList) {
        connect(this, &AbstractMediaProxyModel::entriesToEnqueue,
                mPlayList, static_cast<void(MediaPlayListProxyModel::*)(const ElisaUtils::EntryDataList&, ElisaUtils::PlayListEntryType, ElisaUtils::PlayListEnqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::enqueue));
    }
}

#include "moc_abstractmediaproxymodel.cpp"
