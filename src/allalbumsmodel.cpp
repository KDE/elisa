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

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

class AllAlbumsModelPrivate
{
public:

    AllAlbumsModelPrivate()
    {
    }

    DatabaseInterface *mMusicDatabase = nullptr;

    bool mUseLocalIcons = false;
};

AllAlbumsModel::AllAlbumsModel(QObject *parent) : QAbstractItemModel(parent), d(new AllAlbumsModelPrivate)
{
    Q_EMIT refreshContent();
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

    albumCount = d->mMusicDatabase->albumCount();

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
    roles[static_cast<int>(ColumnsRoles::ItemClassRole)] = "itemClass";
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

    const auto albumCount = d->mMusicDatabase->albumCount();

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
        result = d->mMusicDatabase->albumDataFromIndex(albumIndex, DatabaseInterface::AlbumData::Title);
        break;
    case ColumnsRoles::DurationRole:
        break;
    case ColumnsRoles::MilliSecondsDurationRole:
        break;
    case ColumnsRoles::CreatorRole:
        break;
    case ColumnsRoles::ArtistRole:
        result = d->mMusicDatabase->albumDataFromIndex(albumIndex, DatabaseInterface::AlbumData::Artist);
        break;
    case ColumnsRoles::AlbumRole:
        break;
    case ColumnsRoles::TrackNumberRole:
        break;
    case ColumnsRoles::RatingRole:
        break;
    case ColumnsRoles::ImageRole:
    {
        auto albumArt = d->mMusicDatabase->albumDataFromIndex(albumIndex, DatabaseInterface::AlbumData::Image);
        if (albumArt.isValid() && albumArt.toUrl().isValid()) {
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
    case ColumnsRoles::ItemClassRole:
        break;
    case ColumnsRoles::CountRole:
        result = d->mMusicDatabase->albumDataFromIndex(albumIndex, DatabaseInterface::AlbumData::TracksCount);
        break;
    case ColumnsRoles::IdRole:
        result = d->mMusicDatabase->albumDataFromIndex(albumIndex, DatabaseInterface::AlbumData::Id);
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

    result = createIndex(row, column, nullptr);
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
        connect(d->mMusicDatabase, &DatabaseInterface::resetModel, this, &AllAlbumsModel::databaseReset);
    }

    emit databaseInterfaceChanged();
}

void AllAlbumsModel::albumsList(const QVector<MusicAlbum> &allAlbums)
{
    beginResetModel();
    if (d->mMusicDatabase) {
        d->mMusicDatabase->insertAlbumsList(allAlbums);
    }
    endResetModel();

    return;
}

void AllAlbumsModel::tracksList(QHash<QString, QVector<MusicAudioTrack> > tracks, QHash<QString, QString> covers)
{
    if (tracks.size() > 1) {
        beginResetModel();
    }
    if (d->mMusicDatabase) {
        d->mMusicDatabase->insertTracksList(tracks, covers);
    }
    if (tracks.size() > 1) {
        endResetModel();
    }

    return;
}

void AllAlbumsModel::databaseReset()
{
    beginResetModel();
    endResetModel();
}

#include "moc_allalbumsmodel.cpp"
