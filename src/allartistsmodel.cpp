/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "allartistsmodel.h"
#include "databaseinterface.h"
#include "musicartist.h"

#include <QUrl>
#include <QTimer>
#include <QPointer>
#include <QVector>

class AllArtistsModelPrivate
{
public:

    AllArtistsModelPrivate()
    {
    }

    QVector<MusicArtist> mAllArtists;

    int mArtistsCount = 0;

    bool mUseLocalIcons = false;

};

AllArtistsModel::AllArtistsModel(QObject *parent) : QAbstractItemModel(parent), d(std::make_unique<AllArtistsModelPrivate>())
{
}

AllArtistsModel::~AllArtistsModel()
= default;

int AllArtistsModel::rowCount(const QModelIndex &parent) const
{
    auto artistCount = 0;

    if (parent.isValid()) {
        return artistCount;
    }

    artistCount = d->mArtistsCount;

    return artistCount;
}

QHash<int, QByteArray> AllArtistsModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    roles[static_cast<int>(ColumnsRoles::NameRole)] = "name";
    roles[static_cast<int>(ColumnsRoles::ArtistsCountRole)] = "albumsCount";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::IdRole)] = "id";

    return roles;
}

Qt::ItemFlags AllArtistsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant AllArtistsModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    const auto artistsCount = d->mArtistsCount;

    if (!index.isValid()) {
        return result;
    }

    if (index.column() != 0) {
        return result;
    }

    if (index.row() < 0) {
        return result;
    }

    if (index.parent().isValid()) {
        return result;
    }

    if (index.internalId() != 0) {
        return result;
    }

    if (index.row() < 0 || index.row() >= artistsCount) {
        return result;
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::NameRole:
        result = d->mAllArtists[index.row()].name();
        break;
    case ColumnsRoles::ArtistsCountRole:
        result = d->mAllArtists[index.row()].albumsCount();
        break;
    case ColumnsRoles::ImageRole:
        break;
    case ColumnsRoles::IdRole:
        break;
    }

    return result;
}

QModelIndex AllArtistsModel::index(int row, int column, const QModelIndex &parent) const
{
    auto result = QModelIndex();

    if (column != 0) {
        return result;
    }

    if (parent.isValid()) {
        return result;
    }

    result = createIndex(row, column);

    return result;
}

QModelIndex AllArtistsModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    auto result = QModelIndex();

    return result;
}

int AllArtistsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

void AllArtistsModel::artistAdded(const MusicArtist &newArtist)
{
    if (newArtist.isValid()) {
        beginInsertRows({}, d->mAllArtists.size(), d->mAllArtists.size());
        d->mAllArtists.push_back(newArtist);
        ++d->mArtistsCount;
        endInsertRows();
    }
}

void AllArtistsModel::artistRemoved(const MusicArtist &removedArtist)
{
    auto removedArtistIterator = std::find(d->mAllArtists.begin(), d->mAllArtists.end(), removedArtist);

    if (removedArtistIterator == d->mAllArtists.end()) {
        return;
    }

    int artistIndex = removedArtistIterator - d->mAllArtists.begin();

    beginRemoveRows({}, artistIndex, artistIndex);
    d->mAllArtists.erase(removedArtistIterator);
    --d->mArtistsCount;
    endRemoveRows();
}

void AllArtistsModel::artistModified(const MusicArtist &modifiedArtist)
{
    Q_UNUSED(modifiedArtist);
}

#include "moc_allartistsmodel.cpp"
