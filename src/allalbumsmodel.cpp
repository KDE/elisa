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

#include "allalbumsmodel.h"
#include "musicstatistics.h"
#include "databaseinterface.h"
#include "allartistsmodel.h"
#include "albummodel.h"

#include <QUrl>
#include <QTimer>
#include <QPointer>
#include <QVector>

#include <algorithm>

class AllAlbumsModelPrivate
{
public:

    AllAlbumsModelPrivate()
    {
    }

    QVector<qulonglong> mAllAlbums;

    QHash<qulonglong, MusicAlbum> mAlbumsData;

    AllArtistsModel *mAllArtistsModel = nullptr;

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

    roles[static_cast<int>(ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(ColumnsRoles::AllTracksTitleRole)] = "tracksTitle";
    roles[static_cast<int>(ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ColumnsRoles::AllArtistsRole)] = "allArtists";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::CountRole)] = "count";
    roles[static_cast<int>(ColumnsRoles::IsSingleDiscAlbumRole)] = "isSingleDiscAlbum";
    roles[static_cast<int>(ColumnsRoles::ContainerDataRole)] = "containerData";
    roles[static_cast<int>(ColumnsRoles::HighestTrackRating)] = "highestTrackRating";
    roles[static_cast<int>(ColumnsRoles::AlbumDatabaseIdRole)] = "databaseId";
    roles[static_cast<int>(ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(ColumnsRoles::ShadowForImageRole)] = "shadowForImage";
    roles[static_cast<int>(ColumnsRoles::ChildModelRole)] = "childModel";
    roles[static_cast<int>(ColumnsRoles::IsTracksContainerRole)] = "isTracksContainer";

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

    if (index.row() < 0 || index.row() >= albumCount) {
        return result;
    }

    result = internalDataAlbum(index.row(), role);
    return result;
}

QVariant AllAlbumsModel::internalDataAlbum(int albumIndex, int role) const
{
    auto result = QVariant();

    switch(role)
    {
    case ColumnsRoles::TitleRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].title();
        break;
    case ColumnsRoles::AllTracksTitleRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].allTracksTitle();
        break;
    case ColumnsRoles::ArtistRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].artist();
        break;
    case ColumnsRoles::AllArtistsRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].allArtists().join(QStringLiteral(", "));
        break;
    case ColumnsRoles::ImageRole:
    {
        auto albumArt = d->mAlbumsData[d->mAllAlbums[albumIndex]].albumArtURI();
        if (albumArt.isValid()) {
            result = albumArt;
        }
        break;
    }
    case ColumnsRoles::CountRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].tracksCount();
        break;
    case ColumnsRoles::IdRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].id();
        break;
    case ColumnsRoles::IsSingleDiscAlbumRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].isSingleDiscAlbum();
        break;
    case ColumnsRoles::ContainerDataRole:
        result = QVariant::fromValue(d->mAlbumsData[d->mAllAlbums[albumIndex]]);
        break;
    case ColumnsRoles::AlbumDatabaseIdRole:
        result = QVariant::fromValue(d->mAlbumsData[d->mAllAlbums[albumIndex]].databaseId());
        break;
    case ColumnsRoles::HighestTrackRating:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].highestTrackRating();
        break;
    case Qt::DisplayRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].title();
        break;
    case ColumnsRoles::SecondaryTextRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].artist();
        break;
    case ColumnsRoles::ImageUrlRole:
    {
        auto albumArt = d->mAlbumsData[d->mAllAlbums[albumIndex]].albumArtURI();
        if (albumArt.isValid()) {
            result = albumArt;
        } else {
            result = QUrl(QStringLiteral("image://icon/media-optical-audio"));
        }
        break;
    }
    case ColumnsRoles::ShadowForImageRole:
        result = d->mAlbumsData[d->mAllAlbums[albumIndex]].albumArtURI().isValid();
        break;
    case ColumnsRoles::ChildModelRole:
    {
        auto newModel = new AlbumModel();
        newModel->setAlbumData(d->mAlbumsData[d->mAllAlbums[albumIndex]]);
        result = QVariant::fromValue(newModel);
        break;
    }
    case ColumnsRoles::IsTracksContainerRole:
        result = true;
        break;
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

void AllAlbumsModel::albumAdded(const MusicAlbum &newAlbum)
{
    if (newAlbum.isValid()) {
        beginInsertRows({}, d->mAllAlbums.size(), d->mAllAlbums.size());
        d->mAllAlbums.push_back(newAlbum.databaseId());
        d->mAlbumsData[newAlbum.databaseId()] = newAlbum;
        endInsertRows();

        Q_EMIT albumCountChanged();
    }
}

void AllAlbumsModel::albumRemoved(const MusicAlbum &removedAlbum)
{
    auto removedAlbumIterator = std::find(d->mAllAlbums.begin(), d->mAllAlbums.end(), removedAlbum.databaseId());

    if (removedAlbumIterator == d->mAllAlbums.end()) {
        return;
    }

    int albumIndex = removedAlbumIterator - d->mAllAlbums.begin();

    beginRemoveRows({}, albumIndex, albumIndex);
    d->mAlbumsData.remove(removedAlbum.databaseId());
    d->mAllAlbums.erase(removedAlbumIterator);
    endRemoveRows();

    Q_EMIT albumCountChanged();
}

void AllAlbumsModel::albumModified(const MusicAlbum &modifiedAlbum)
{
    auto modifiedAlbumIterator = std::find(d->mAllAlbums.begin(), d->mAllAlbums.end(), modifiedAlbum.databaseId());

    if (modifiedAlbumIterator == d->mAllAlbums.end()) {
        return;
    }

    int albumIndex = modifiedAlbumIterator - d->mAllAlbums.begin();
    d->mAlbumsData[modifiedAlbum.databaseId()] = modifiedAlbum;

    Q_EMIT dataChanged(index(albumIndex, 0), index(albumIndex, 0));
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
