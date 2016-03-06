/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "albumfilterproxymodel.h"

#include "upnpalbummodel.h"

AlbumFilterProxyModel::AlbumFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent), mFilterText()
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

AlbumFilterProxyModel::~AlbumFilterProxyModel()
{
}

QString AlbumFilterProxyModel::filterText() const
{
    return mFilterText;
}

QModelIndex AlbumFilterProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return QSortFilterProxyModel::mapToSource(proxyIndex);
}

QModelIndex AlbumFilterProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return QSortFilterProxyModel::mapFromSource(sourceIndex);
}

void AlbumFilterProxyModel::setFilterText(QString filterText)
{
    if (mFilterText == filterText)
        return;

    mFilterText = filterText;

    mFilterExpression.setPattern(mFilterText);
    mFilterExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    mFilterExpression.optimize();

    invalidate();

    Q_EMIT filterTextChanged(mFilterText);
}

bool AlbumFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = true;

    for (int column = 0, columnCount = sourceModel()->columnCount(source_parent); result && column < columnCount; ++column) {
        auto currentIndex = sourceModel()->index(source_row, column, source_parent);

        const auto &titleValue = sourceModel()->data(currentIndex, UpnpAlbumModel::TitleRole).toString();
        const auto &artistValue = sourceModel()->data(currentIndex, UpnpAlbumModel::ArtistRole).toString();

        if (!mFilterExpression.match(titleValue).hasMatch() && !mFilterExpression.match(artistValue).hasMatch()) {
            result = false;
        }
    }

    return result;
}


#include "moc_albumfilterproxymodel.cpp"
