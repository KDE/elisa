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
    setFilterRole(UpnpAlbumModel::TitleRole);
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

    setFilterRegExp(mFilterText);

    Q_EMIT filterTextChanged(mFilterText);
}


#include "moc_albumfilterproxymodel.cpp"
