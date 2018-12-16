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

#include "allalbumsmodel.h"
#include "modeldataloader.h"
#include "musiclistenersmanager.h"

#include <QUrl>
#include <QTimer>
#include <QPointer>
#include <QVector>
#include <QDebug>

#include <algorithm>

class AllAlbumsModelPrivate
{
public:

    AllAlbumsModel::ListAlbumDataType mAllAlbums;

    ModelDataLoader mDataLoader;

};

AllAlbumsModel::AllAlbumsModel(QObject *parent) : QAbstractItemModel(parent), d(std::make_unique<AllAlbumsModelPrivate>())
{
}

AllAlbumsModel::~AllAlbumsModel()
= default;

int AllAlbumsModel::albumCount() const
{
    return d->mAllAlbums.size();
}

int AllAlbumsModel::rowCount(const QModelIndex &parent) const
{
    auto albumCount = 0;

    if (parent.isValid()) {
        return albumCount;
    }

    albumCount = d->mAllAlbums.size();

    return albumCount;
}

QHash<int, QByteArray> AllAlbumsModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::DatabaseIdRole)] = "databaseId";

    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::AllArtistsRole)] = "allArtists";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::HighestTrackRating)] = "highestTrackRating";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::GenreRole)] = "genre";

    return roles;
}

Qt::ItemFlags AllAlbumsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant AllAlbumsModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    const auto albumCount = d->mAllAlbums.size();

    Q_ASSERT(index.isValid());
    Q_ASSERT(index.column() == 0);
    Q_ASSERT(index.row() >= 0 && index.row() < albumCount);
    Q_ASSERT(!index.parent().isValid());
    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalId() == 0);

    result = internalDataAlbum(index.row(), role);
    return result;
}

QVariant AllAlbumsModel::internalDataAlbum(int albumIndex, int role) const
{
    auto result = QVariant();

    switch(role)
    {
    case Qt::DisplayRole:
        result = d->mAllAlbums[albumIndex][AlbumDataType::key_type::TitleRole];
        break;
    default:
        result = d->mAllAlbums[albumIndex][static_cast<AlbumDataType::key_type>(role)];
    }

    return result;
}

QModelIndex AllAlbumsModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex AllAlbumsModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    auto result = QModelIndex();

    return result;
}

int AllAlbumsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

void AllAlbumsModel::albumsAdded(ListAlbumDataType newAlbums)
{
    if (d->mAllAlbums.isEmpty()) {
        beginInsertRows({}, d->mAllAlbums.size(), d->mAllAlbums.size() + newAlbums.size() - 1);
        d->mAllAlbums.swap(newAlbums);
        endInsertRows();
    } else {
        beginInsertRows({}, d->mAllAlbums.size(), d->mAllAlbums.size() + newAlbums.size() - 1);
        d->mAllAlbums.append(newAlbums);
        endInsertRows();
    }

    Q_EMIT albumCountChanged();
}

void AllAlbumsModel::albumRemoved(qulonglong removedAlbumId)
{
    auto removedAlbumIterator = d->mAllAlbums.end();

    removedAlbumIterator = std::find_if(d->mAllAlbums.begin(), d->mAllAlbums.end(),
                                        [removedAlbumId](auto album) {return album.databaseId() == removedAlbumId;});

    if (removedAlbumIterator == d->mAllAlbums.end()) {
        return;
    }

    int albumIndex = removedAlbumIterator - d->mAllAlbums.begin();

    beginRemoveRows({}, albumIndex, albumIndex);

    d->mAllAlbums.erase(removedAlbumIterator);

    endRemoveRows();

    Q_EMIT albumCountChanged();
}

void AllAlbumsModel::albumModified(const AlbumDataType &modifiedAlbum)
{
    auto modifiedAlbumIterator = std::find_if(d->mAllAlbums.begin(), d->mAllAlbums.end(),
                                              [modifiedAlbum](auto album) {
        return album.databaseId() == modifiedAlbum.databaseId();
    });

    if (modifiedAlbumIterator == d->mAllAlbums.end()) {
        return;
    }

    auto albumIndex = modifiedAlbumIterator - d->mAllAlbums.begin();

    Q_EMIT dataChanged(index(albumIndex, 0), index(albumIndex, 0));
}

void AllAlbumsModel::initialize(MusicListenersManager *manager)
{
    manager->connectModel(&d->mDataLoader);

    connect(manager->viewDatabase(), &DatabaseInterface::albumsAdded,
            this, &AllAlbumsModel::albumsAdded);
    connect(manager->viewDatabase(), &DatabaseInterface::albumModified,
            this, &AllAlbumsModel::albumModified);
    connect(manager->viewDatabase(), &DatabaseInterface::albumRemoved,
            this, &AllAlbumsModel::albumRemoved);
    connect(this, &AllAlbumsModel::needData,
            &d->mDataLoader, &ModelDataLoader::loadData);
    connect(&d->mDataLoader, &ModelDataLoader::allAlbumsData,
            this, &AllAlbumsModel::albumsAdded);

    Q_EMIT needData(ElisaUtils::Album);
}

void AllAlbumsModel::initializeByArtist(MusicListenersManager *manager, const QString &artist)
{
    manager->connectModel(&d->mDataLoader);

    connect(manager->viewDatabase(), &DatabaseInterface::albumsAdded,
            this, &AllAlbumsModel::albumsAdded);
    connect(manager->viewDatabase(), &DatabaseInterface::albumModified,
            this, &AllAlbumsModel::albumModified);
    connect(manager->viewDatabase(), &DatabaseInterface::albumRemoved,
            this, &AllAlbumsModel::albumRemoved);
    connect(this, &AllAlbumsModel::needDataByArtist,
            &d->mDataLoader, &ModelDataLoader::loadDataByArtist);
    connect(&d->mDataLoader, &ModelDataLoader::allAlbumsData,
            this, &AllAlbumsModel::albumsAdded);

    Q_EMIT needDataByArtist(ElisaUtils::Album, artist);
}

void AllAlbumsModel::initializeByGenreAndArtist(MusicListenersManager *manager,
                                                const QString &genre, const QString &artist)
{
    manager->connectModel(&d->mDataLoader);

    connect(manager->viewDatabase(), &DatabaseInterface::albumsAdded,
            this, &AllAlbumsModel::albumsAdded);
    connect(manager->viewDatabase(), &DatabaseInterface::albumModified,
            this, &AllAlbumsModel::albumModified);
    connect(manager->viewDatabase(), &DatabaseInterface::albumRemoved,
            this, &AllAlbumsModel::albumRemoved);
    connect(this, &AllAlbumsModel::needDataByGenreAndArtist,
            &d->mDataLoader, &ModelDataLoader::loadDataByGenreAndArtist);
    connect(&d->mDataLoader, &ModelDataLoader::allAlbumsData,
            this, &AllAlbumsModel::albumsAdded);

    Q_EMIT needDataByGenreAndArtist(ElisaUtils::Album, genre, artist);
}

#include "moc_allalbumsmodel.cpp"
