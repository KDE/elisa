/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "allartistsmodel.h"
#include "databaseinterface.h"
#include "allalbumsmodel.h"

#include <QUrl>
#include <QTimer>
#include <QPointer>
#include <QVector>

class AllArtistsModelPrivate
{
public:

    AllArtistsModel::ListArtistDataType mAllArtists;

    AllAlbumsModel *mAllAlbumsModel = nullptr;

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

    artistCount = d->mAllArtists.size();

    return artistCount;
}

QHash<int, QByteArray> AllArtistsModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::DatabaseIdRole)] = "databaseId";

    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ImageUrlRole)] = "imageUrl";

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

    const auto artistsCount = d->mAllArtists.size();

    Q_ASSERT(index.isValid());
    Q_ASSERT(index.column() == 0);
    Q_ASSERT(index.row() >= 0 && index.row() < artistsCount);
    Q_ASSERT(!index.parent().isValid());
    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalId() == 0);

    switch(role)
    {
    case Qt::DisplayRole:
        result = d->mAllArtists[index.row()][ArtistDataType::key_type::TitleRole];
        break;
    default:
        result = d->mAllArtists[index.row()][static_cast<ArtistDataType::key_type>(role)];
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

AllAlbumsModel *AllArtistsModel::allAlbums() const
{
    return d->mAllAlbumsModel;
}

void AllArtistsModel::artistsAdded(ListArtistDataType newArtists)
{
    if (d->mAllArtists.isEmpty()) {
        beginInsertRows({}, d->mAllArtists.size(), d->mAllArtists.size() + newArtists.size() - 1);
        d->mAllArtists.swap(newArtists);
        endInsertRows();
    } else {
        beginInsertRows({}, d->mAllArtists.size(), d->mAllArtists.size() + newArtists.size() - 1);
        d->mAllArtists.append(newArtists);
        endInsertRows();
    }
}

void AllArtistsModel::artistRemoved(qulonglong removedArtistId)
{
    auto removedArtistIterator = d->mAllArtists.end();

    removedArtistIterator = std::find_if(d->mAllArtists.begin(), d->mAllArtists.end(),
                                        [removedArtistId](auto album) {return album.databaseId() == removedArtistId;});

    if (removedArtistIterator == d->mAllArtists.end()) {
        return;
    }

    int artistIndex = removedArtistIterator - d->mAllArtists.begin();

    beginRemoveRows({}, artistIndex, artistIndex);

    d->mAllArtists.erase(removedArtistIterator);

    endRemoveRows();
}

void AllArtistsModel::setAllAlbums(AllAlbumsModel *model)
{
    if (d->mAllAlbumsModel == model) {
        return;
    }

    d->mAllAlbumsModel = model;

    Q_EMIT allAlbumsChanged();
}

#include "moc_allartistsmodel.cpp"
