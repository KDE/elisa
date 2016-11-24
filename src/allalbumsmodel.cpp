/*
 * Copyright 2015-2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include <QtCore/QUrl>
#include <QtCore/QTimer>
#include <QtCore/QPointer>
#include <QtCore/QVector>

class AllAlbumsModelPrivate
{
public:

    AllAlbumsModelPrivate()
    {
    }

    QString mArtist;

    DatabaseInterface *mMusicDatabase = nullptr;

    bool mUseLocalIcons = false;

    int mAlbumCount = 0;

    QVector<MusicAlbum> mAllAlbums;
};

AllAlbumsModel::AllAlbumsModel(QObject *parent) : QAbstractItemModel(parent), d(new AllAlbumsModelPrivate)
{
}

AllAlbumsModel::~AllAlbumsModel()
{
    delete d;
}

int AllAlbumsModel::rowCount(const QModelIndex &parent) const
{
    auto albumCount = 0;

    if (!d->mMusicDatabase) {
        return albumCount;
    }

    if (parent.isValid()) {
        return albumCount;
    }

    albumCount = d->mAlbumCount;

    return albumCount;
}

QHash<int, QByteArray> AllAlbumsModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::CountRole)] = "count";
    roles[static_cast<int>(ColumnsRoles::IsPlayingRole)] = "isPlaying";

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

    if (!d->mMusicDatabase) {
        return result;
    }

    const auto albumCount = d->mAlbumCount;

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

    if (!d->mMusicDatabase) {
        return result;
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
        result = d->mAllAlbums[albumIndex].title();
        break;
    case ColumnsRoles::DurationRole:
        break;
    case ColumnsRoles::MilliSecondsDurationRole:
        break;
    case ColumnsRoles::CreatorRole:
        break;
    case ColumnsRoles::ArtistRole:
        result = d->mAllAlbums[albumIndex].artist();
        break;
    case ColumnsRoles::AlbumRole:
        break;
    case ColumnsRoles::TrackNumberRole:
        break;
    case ColumnsRoles::RatingRole:
        break;
    case ColumnsRoles::ImageRole:
    {
        auto albumArt = d->mAllAlbums[albumIndex].albumArtURI();
        if (albumArt.isValid()) {
            result = albumArt;
        } else {
            if (d->mUseLocalIcons) {
                result = QUrl(QStringLiteral("qrc:/media-optical-audio.svg"));
            } else {
                result = QUrl(QStringLiteral("image://icon/media-optical-audio"));
            }
        }
        break;
    }
    case ColumnsRoles::ResourceRole:
        break;
    case ColumnsRoles::CountRole:
        result = d->mAllAlbums[albumIndex].tracksCount();
        break;
    case ColumnsRoles::IdRole:
        result = d->mAllAlbums[albumIndex].id();
        break;
    case ColumnsRoles::IsPlayingRole:
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

DatabaseInterface *AllAlbumsModel::databaseInterface() const
{
    return d->mMusicDatabase;
}

QString AllAlbumsModel::artist() const
{
    return d->mArtist;
}

void AllAlbumsModel::setDatabaseInterface(DatabaseInterface *musicDatabase)
{
    if (d->mMusicDatabase == musicDatabase) {
        return;
    }

    if (d->mMusicDatabase) {
        disconnect(d->mMusicDatabase);
    }

    d->mMusicDatabase = musicDatabase;

    if (d->mMusicDatabase) {
        connect(d->mMusicDatabase, &DatabaseInterface::beginAlbumAdded, this, &AllAlbumsModel::beginAlbumAdded);
        connect(d->mMusicDatabase, &DatabaseInterface::endAlbumAdded, this, &AllAlbumsModel::endAlbumAdded);

        d->mAlbumCount = d->mMusicDatabase->albumCount({});
    }

    beginResetModel();
    d->mAllAlbums = d->mMusicDatabase->allAlbums(d->mArtist);
    endResetModel();

    emit databaseInterfaceChanged();
}

void AllAlbumsModel::setArtist(QString artist)
{
    if (d->mArtist == artist) {
        return;
    }

    beginResetModel();

    d->mArtist = artist;
    d->mAllAlbums = d->mMusicDatabase->allAlbums(d->mArtist);
    d->mAlbumCount = d->mAllAlbums.count();

    endResetModel();

    emit artistChanged();
}

void AllAlbumsModel::beginAlbumAdded(QVector<qulonglong> newAlbums)
{
    Q_UNUSED(newAlbums);
}

void AllAlbumsModel::endAlbumAdded(QVector<qulonglong> newAlbums)
{
    beginResetModel();
    d->mAllAlbums = d->mMusicDatabase->allAlbums(d->mArtist);
    d->mAlbumCount = d->mMusicDatabase->albumCount({});
    endResetModel();
}

#include "moc_allalbumsmodel.cpp"
