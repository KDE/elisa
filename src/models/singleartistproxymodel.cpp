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

#include "singleartistproxymodel.h"

#include "genericdatamodel.h"
#include "musicalbum.h"
#include "databaseinterface.h"

#include <QReadLocker>
#include <QtConcurrentRun>

SingleArtistProxyModel::SingleArtistProxyModel(QObject *parent) : AbstractMediaProxyModel(parent)
{
    this->setSortRole(DatabaseInterface::ColumnsRoles::TitleRole);
    this->setSortCaseSensitivity(Qt::CaseInsensitive);
    this->sortModel(Qt::AscendingOrder);
}

SingleArtistProxyModel::~SingleArtistProxyModel() = default;

QString SingleArtistProxyModel::artistFilter() const
{
    return mArtistFilter;
}


void SingleArtistProxyModel::setArtistFilterText(const QString &filterText)
{
    if (mArtistFilter == filterText)
        return;

    mArtistFilter = filterText;

    mArtistExpression.setPattern(QStringLiteral(".*") + mArtistFilter + QStringLiteral(".*"));
    mArtistExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    mArtistExpression.optimize();

    invalidate();

    Q_EMIT artistFilterTextChanged(mArtistFilter);
}

bool SingleArtistProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    auto currentIndex = sourceModel()->index(source_row, 0, source_parent);

    const auto &genreValue = sourceModel()->data(currentIndex, DatabaseInterface::ColumnsRoles::GenreRole);

    if (!genreFilterText().isNull() && !genreValue.isValid()) {
        return result;
    }

    if (!genreFilterText().isNull() && !genreValue.canConvert<QStringList>()) {
        return result;
    }

    if (!genreFilterText().isNull() && !genreValue.toStringList().contains(genreFilterText())) {
        return result;
    }

    const auto &titleValue = sourceModel()->data(currentIndex, DatabaseInterface::ColumnsRoles::TitleRole).toString();
    const auto &allArtistsValue = sourceModel()->data(currentIndex, DatabaseInterface::ColumnsRoles::AllArtistsRole).toStringList().join(QLatin1String(", "));
    const auto maximumRatingValue = sourceModel()->data(currentIndex, DatabaseInterface::ColumnsRoles::HighestTrackRating).toInt();

    if (maximumRatingValue < mFilterRating) {
        result = false;
        return result;
    }

    if (mArtistExpression.match(allArtistsValue).hasMatch()) {
        if (mFilterExpression.match(titleValue).hasMatch()) {
            result = true;
        }
    }

    return result;
}

void SingleArtistProxyModel::genericEnqueueToPlayList(ElisaUtils::PlayListEnqueueMode enqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allAlbums = ElisaUtils::EntryDataList{};
        allAlbums.reserve(rowCount());
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            allAlbums.push_back({data(currentIndex, DatabaseInterface::ColumnsRoles::DatabaseIdRole).toULongLong(),
                                 data(currentIndex, DatabaseInterface::ColumnsRoles::TitleRole).toString()});
        }
        Q_EMIT albumToEnqueue(allAlbums, ElisaUtils::Album, enqueueMode, triggerPlay);
    });
}

void SingleArtistProxyModel::enqueueToPlayList()
{
    genericEnqueueToPlayList(ElisaUtils::AppendPlayList, ElisaUtils::DoNotTriggerPlay);
}

void SingleArtistProxyModel::replaceAndPlayOfPlayList()
{
    genericEnqueueToPlayList(ElisaUtils::ReplacePlayList, ElisaUtils::TriggerPlay);
}


#include "moc_singleartistproxymodel.cpp"
