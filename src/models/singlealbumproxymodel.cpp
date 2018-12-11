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

#include "albummodel.h"
#include "databaseinterface.h"

#include <QReadLocker>
#include <QtConcurrentRun>

SingleAlbumProxyModel::SingleAlbumProxyModel(QObject *parent) : AbstractMediaProxyModel(parent)
{
}

SingleAlbumProxyModel::~SingleAlbumProxyModel() = default;

bool SingleAlbumProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    for (int column = 0, columnCount = sourceModel()->columnCount(source_parent); column < columnCount; ++column) {
        auto currentIndex = sourceModel()->index(source_row, column, source_parent);

        const auto &genreValue = sourceModel()->data(currentIndex, DatabaseInterface::ColumnsRoles::GenreRole);

        if (!genreFilterText().isNull() && !genreValue.isValid()) {
            continue;
        }

        if (!genreFilterText().isNull() && !genreValue.canConvert<QStringList>()) {
            continue;
        }

        if (!genreFilterText().isNull() && !genreValue.toStringList().contains(genreFilterText())) {
            continue;
        }

        const auto &titleValue = sourceModel()->data(currentIndex, AlbumModel::TitleRole).toString();
        const auto maximumRatingValue = sourceModel()->data(currentIndex, AlbumModel::RatingRole).toInt();

        if (maximumRatingValue < mFilterRating) {
            result = false;
            continue;
        }

        if (mFilterExpression.match(titleValue).hasMatch()) {
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

void SingleAlbumProxyModel::genericEnqueueToPlayList(ElisaUtils::PlayListEnqueueMode enqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allTracks = ElisaUtils::EntryDataList{};
        allTracks.reserve(rowCount());
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            allTracks.push_back({data(currentIndex, AlbumModel::DatabaseIdRole).toULongLong(),
                                 data(currentIndex, AlbumModel::TitleRole).toString()});
        }
        Q_EMIT trackToEnqueue(allTracks, ElisaUtils::Track, enqueueMode, triggerPlay);
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

void SingleAlbumProxyModel::loadAlbumData(qulonglong databaseId)
{
    auto albumSourceModel = qobject_cast<AlbumModel *>(this->sourceModel());
    if(albumSourceModel) {
        albumSourceModel->loadAlbumData(databaseId);
    }
}

#include "moc_singlealbumproxymodel.cpp"
