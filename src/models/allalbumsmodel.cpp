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
#include "databaseinterface.h"
#include "allartistsmodel.h"
#include "albummodel.h"
#include "elisautils.h"

#include <QUrl>
#include <QTimer>
#include <QPointer>
#include <QVector>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include <QtConcurrentRun>
#include <QThreadPool>

#include <algorithm>

class AllAlbumsModelPrivate
{
public:

    AllAlbumsModelPrivate()
    {
        mThreadPool.setMaxThreadCount(1);
    }

    QVector<qulonglong> mAllAlbums;

    QHash<qulonglong, MusicAlbum> mAlbumsData;

    AllArtistsModel *mAllArtistsModel = nullptr;

    QReadWriteLock mDataLock;

    QThreadPool mThreadPool;

};

AllAlbumsModel::AllAlbumsModel(QObject *parent) : QAbstractItemModel(parent), d(std::make_unique<AllAlbumsModelPrivate>())
{
}

AllAlbumsModel::~AllAlbumsModel()
= default;

int AllAlbumsModel::albumCount() const
{
    QReadLocker locker(&d->mDataLock);
    return d->mAllAlbums.size();
}

int AllAlbumsModel::rowCount(const QModelIndex &parent) const
{
    auto albumCount = 0;

    if (parent.isValid()) {
        return albumCount;
    }

    QReadLocker locker(&d->mDataLock);

    albumCount = d->mAllAlbums.size();

    return albumCount;
}

QHash<int, QByteArray> AllAlbumsModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    roles[static_cast<int>(ElisaUtils::ColumnsRoles::TitleRole)] = "title";
    //roles[static_cast<int>(ElisaUtils::ColumnsRoles::AllTracksTitleRole)] = "tracksTitle";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::AllArtistsRole)] = "allArtists";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ImageRole)] = "image";
    //roles[static_cast<int>(ElisaUtils::ColumnsRoles::CountRole)] = "count";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::IsSingleDiscAlbumRole)] = "isSingleDiscAlbum";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ContainerDataRole)] = "containerData";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::HighestTrackRating)] = "highestTrackRating";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::DatabaseIdRole)] = "databaseId";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ShadowForImageRole)] = "shadowForImage";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ChildModelRole)] = "childModel";
    //roles[static_cast<int>(ElisaUtils::ColumnsRoles::IsTracksContainerRole)] = "isTracksContainer";

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

    QReadLocker locker(&d->mDataLock);

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
    case ElisaUtils::ColumnsRoles::TitleRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].title();
        break;
    /*case ElisaUtils::ColumnsRoles::AllTracksTitleRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].allTracksTitle();
        break;*/
    case ElisaUtils::ColumnsRoles::ArtistRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].artist();
        break;
    case ElisaUtils::ColumnsRoles::AllArtistsRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].allArtists().join(QStringLiteral(", "));
        break;
    case ElisaUtils::ColumnsRoles::ImageRole:
    {
        auto albumArt = d->mAlbumsData[d->mAllAlbums[albumIndex]].albumArtURI();
        if (albumArt.isValid()) {
            result = albumArt;
        }
        break;
    }
    /*case ElisaUtils::ColumnsRoles::CountRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].tracksCount();
        break;*/
    case ElisaUtils::ColumnsRoles::IdRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].id();
        break;
    case ElisaUtils::ColumnsRoles::IsSingleDiscAlbumRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].isSingleDiscAlbum();
        break;
    case ElisaUtils::ColumnsRoles::ContainerDataRole:
        result = QVariant::fromValue(d->mAlbumsData[d->mAllAlbums[albumIndex]]);
        break;
    case ElisaUtils::ColumnsRoles::DatabaseIdRole:
        result = QVariant::fromValue(d->mAlbumsData[d->mAllAlbums[albumIndex]].databaseId());
        break;
    case ElisaUtils::ColumnsRoles::HighestTrackRating:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].highestTrackRating();
        break;
    case ElisaUtils::ColumnsRoles::SecondaryTextRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].artist();
        break;
    case ElisaUtils::ColumnsRoles::ImageUrlRole:
    {
        auto albumArt = d->mAlbumsData[d->mAllAlbums[albumIndex]].albumArtURI();
        if (albumArt.isValid()) {
            result = albumArt;
        } else {
            result = QUrl(QStringLiteral("image://icon/media-optical-audio"));
        }
        break;
    }
    case ElisaUtils::ColumnsRoles::ShadowForImageRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].albumArtURI().isValid();
        break;
    case ElisaUtils::ColumnsRoles::ChildModelRole:
    {
        auto albumData = d->mAlbumsData[d->mAllAlbums[albumIndex]];
        result = QVariant::fromValue(albumData);
        break;
    }
    case ElisaUtils::ColumnsRoles::GenreRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].genres();
        break;
    /*case ElisaUtils::ColumnsRoles::IsTracksContainerRole:
        result = true;
        break;*/
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

AllArtistsModel *AllAlbumsModel::allArtists() const
{
    return d->mAllArtistsModel;
}

void AllAlbumsModel::albumsAdded(const QList<MusicAlbum> &newAlbums)
{
    QtConcurrent::run(&d->mThreadPool, [=] () {
        for (const auto &newAlbum : newAlbums) {
            if (newAlbum.isValid()) {
                beginInsertRows({}, d->mAllAlbums.size(), d->mAllAlbums.size());

                {
                    QWriteLocker locker(&d->mDataLock);

                    d->mAllAlbums.push_back(newAlbum.databaseId());
                    d->mAlbumsData[newAlbum.databaseId()] = newAlbum;
                }

                endInsertRows();

                Q_EMIT albumCountChanged();
            }
        }
    });
}

void AllAlbumsModel::albumRemoved(const MusicAlbum &removedAlbum)
{
    QtConcurrent::run(&d->mThreadPool, [=] () {
        auto removedAlbumIterator = d->mAllAlbums.end();

        {
            QReadLocker locker(&d->mDataLock);

            removedAlbumIterator = std::find(d->mAllAlbums.begin(), d->mAllAlbums.end(), removedAlbum.databaseId());

            if (removedAlbumIterator == d->mAllAlbums.end()) {
                return;
            }

            int albumIndex = removedAlbumIterator - d->mAllAlbums.begin();

            beginRemoveRows({}, albumIndex, albumIndex);

        }

        {
            QWriteLocker writeLocker(&d->mDataLock);
            d->mAlbumsData.remove(removedAlbum.databaseId());
            d->mAllAlbums.erase(removedAlbumIterator);
        }

        endRemoveRows();

        Q_EMIT albumCountChanged();
    });
}

void AllAlbumsModel::albumModified(const MusicAlbum &modifiedAlbum)
{
    QtConcurrent::run(&d->mThreadPool, [=] () {
        auto modifiedAlbumIterator = d->mAllAlbums.end();

        {
            QReadLocker locker(&d->mDataLock);

            modifiedAlbumIterator = std::find(d->mAllAlbums.begin(), d->mAllAlbums.end(), modifiedAlbum.databaseId());
        }

        int albumIndex = 0;

        {
            QWriteLocker writeLocker(&d->mDataLock);

            if (modifiedAlbumIterator == d->mAllAlbums.end()) {
                return;
            }

            albumIndex = modifiedAlbumIterator - d->mAllAlbums.begin();

            d->mAlbumsData[modifiedAlbum.databaseId()] = modifiedAlbum;
        }

        Q_EMIT dataChanged(index(albumIndex, 0), index(albumIndex, 0));
    });
}

void AllAlbumsModel::setAllArtists(AllArtistsModel *model)
{
    if (d->mAllArtistsModel == model) {
        return;
    }

    d->mAllArtistsModel = model;
    Q_EMIT allArtistsChanged();
}

#include "moc_allalbumsmodel.cpp"
