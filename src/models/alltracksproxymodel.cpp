/*
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

#include "alltracksproxymodel.h"

#include "alltracksmodel.h"

#include <QReadLocker>
#include <QtConcurrentRun>

AllTracksProxyModel::AllTracksProxyModel(QObject *parent) : AbstractMediaProxyModel(parent)
{
}

AllTracksProxyModel::~AllTracksProxyModel()
{
}

bool AllTracksProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    for (int column = 0, columnCount = sourceModel()->columnCount(source_parent); column < columnCount; ++column) {
        auto currentIndex = sourceModel()->index(source_row, column, source_parent);

        const auto &titleValue = sourceModel()->data(currentIndex, AllTracksModel::TitleRole).toString();
        const auto &artistValue = sourceModel()->data(currentIndex, AllTracksModel::ArtistRole).toString();
        const auto maximumRatingValue = sourceModel()->data(currentIndex, AllTracksModel::RatingRole).toInt();

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
            allTracks.push_back(data(currentIndex, AllTracksModel::ContainerDataRole).value<MusicAudioTrack>());
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
            allTracks.push_back(data(currentIndex, AllTracksModel::ContainerDataRole).value<MusicAudioTrack>());
        }
        Q_EMIT trackToEnqueue(allTracks,
                              ElisaUtils::ReplacePlayList,
                              ElisaUtils::TriggerPlay);
    });
}


#include "moc_alltracksproxymodel.cpp"
