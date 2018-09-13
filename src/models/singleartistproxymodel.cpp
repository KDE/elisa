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

#include <QReadLocker>
#include <QtConcurrentRun>

SingleArtistProxyModel::SingleArtistProxyModel(QObject *parent) : AbstractMediaProxyModel(parent)
{
    this->setSortRole(ElisaUtils::ColumnsRoles::TitleRole);
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

    for (int column = 0, columnCount = sourceModel()->columnCount(source_parent); column < columnCount; ++column) {
        auto currentIndex = sourceModel()->index(source_row, column, source_parent);

        const auto &genreValue = sourceModel()->data(currentIndex, ElisaUtils::ColumnsRoles::GenreRole);

        if (!genreFilterText().isNull() && !genreValue.isValid()) {
            continue;
        }

        if (!genreFilterText().isNull() && !genreValue.canConvert<QStringList>()) {
            continue;
        }

        if (!genreFilterText().isNull() && !genreValue.toStringList().contains(genreFilterText())) {
            continue;
        }

        const auto &titleValue = sourceModel()->data(currentIndex, ElisaUtils::ColumnsRoles::TitleRole).toString();
        const auto &artistValue = sourceModel()->data(currentIndex, ElisaUtils::ColumnsRoles::AllArtistsRole).toString();
        const auto maximumRatingValue = sourceModel()->data(currentIndex, ElisaUtils::ColumnsRoles::HighestTrackRating).toInt();

        if (maximumRatingValue < mFilterRating) {
            result = false;
            continue;
        }

        if (mArtistExpression.match(artistValue).hasMatch()) {
            if (mFilterExpression.match(titleValue).hasMatch()) {
                result = true;
                continue;
            }

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

void SingleArtistProxyModel::enqueueToPlayList()
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allAlbums = QList<MusicAlbum>();
        allAlbums.reserve(rowCount());
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            allAlbums.push_back(data(currentIndex, ElisaUtils::ColumnsRoles::ContainerDataRole). value<MusicAlbum>());
        }
        Q_EMIT albumToEnqueue(allAlbums,
                              ElisaUtils::AppendPlayList,
                              ElisaUtils::DoNotTriggerPlay);
    });
}

void SingleArtistProxyModel::replaceAndPlayOfPlayList()
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allAlbums = QList<MusicAlbum>();
        allAlbums.reserve(rowCount());
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            allAlbums.push_back(data(currentIndex, ElisaUtils::ColumnsRoles::ContainerDataRole). value<MusicAlbum>());
        }
        Q_EMIT albumToEnqueue(allAlbums,
                              ElisaUtils::ReplacePlayList,
                              ElisaUtils::TriggerPlay);
    });
}


#include "moc_singleartistproxymodel.cpp"
