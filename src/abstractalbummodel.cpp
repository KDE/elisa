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

#include "abstractalbummodel.h"
#include "musicstatistics.h"
#include "databaseinterface.h"

#include <QtCore/QUrl>
#include <QtCore/QTimer>
#include <QtCore/QPointer>
#include <QtCore/QVector>

#include <QtCore/QDebug>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

class AbstractAlbumModelPrivate
{
public:

    AbstractAlbumModelPrivate()
    {
    }

    DatabaseInterface *mMusicDatabase = new DatabaseInterface;

    bool mUseLocalIcons = false;
};

AbstractAlbumModel::AbstractAlbumModel(QObject *parent) : QAbstractItemModel(parent), d(new AbstractAlbumModelPrivate)
{
    Q_EMIT refreshContent();
}

AbstractAlbumModel::~AbstractAlbumModel()
{
    delete d;
}

int AbstractAlbumModel::rowCount(const QModelIndex &parent) const
{
    const auto albumCount = d->mMusicDatabase->albumCount();

    if (!parent.isValid()) {
        return albumCount;
    }

    if (parent.row() < 0 || parent.row() >= albumCount) {
        return 0;
    }

    const auto &currentAlbum = d->mMusicDatabase->albumFromIndex(parent.row());
    if (!currentAlbum.mIsValid) {
        return 0;
    }

    return currentAlbum.mTracks.size();
}

QHash<int, QByteArray> AbstractAlbumModel::roleNames() const
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

Qt::ItemFlags AbstractAlbumModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant AbstractAlbumModel::data(const QModelIndex &index, int role) const
{
    const auto albumCount = d->mMusicDatabase->albumCount();

    if (!index.isValid()) {
        return {};
    }

    if (index.column() != 0) {
        return {};
    }

    if (index.row() < 0) {
        return {};
    }

    if (!index.parent().isValid()) {
        if (index.internalId() == 0) {
            if (index.row() < 0 || index.row() >= albumCount) {
                return {};
            }

            const auto &currentAlbum = d->mMusicDatabase->albumFromIndex(index.row());

            if (!currentAlbum.mIsValid) {
                return {};
            }

            return internalDataAlbum(currentAlbum, role);
        }
    }

    const auto &currentAlbum = d->mMusicDatabase->albumFromIndex(index.parent().row());

    if (!currentAlbum.mIsValid) {
        return {};
    }

    const auto &currentTrack = currentAlbum.mTracks[currentAlbum.mTrackIds[index.row()]];

    if (!currentTrack.mIsValid) {
        return {};
    }

    return internalDataTrack(currentTrack, index, role);
}

QVariant AbstractAlbumModel::internalDataAlbum(const MusicAlbum &albumData, int role) const
{
    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
        return albumData.mTitle;
    case ColumnsRoles::DurationRole:
        return {};
    case ColumnsRoles::CreatorRole:
        return {};
    case ColumnsRoles::ArtistRole:
        return albumData.mArtist;
    case ColumnsRoles::AlbumRole:
        return {};
    case ColumnsRoles::TrackNumberRole:
        return {};
    case ColumnsRoles::RatingRole:
        return {};
    case ColumnsRoles::ImageRole:
        if (albumData.mAlbumArtURI.isValid()) {
            return albumData.mAlbumArtURI;
        } else {
            if (d->mUseLocalIcons) {
                return QUrl(QStringLiteral("qrc:/media-optical-audio.svg"));
            } else {
                return QUrl(QStringLiteral("image://icon/media-optical-audio"));
            }
        }
    case ColumnsRoles::ResourceRole:
        return {};
    case ColumnsRoles::ItemClassRole:
        return {};
    case ColumnsRoles::CountRole:
        return albumData.mTracksCount;
    case ColumnsRoles::IdRole:
        return albumData.mTitle;
    case ColumnsRoles::IsPlayingRole:
        return {};
    }

    return {};
}

QVariant AbstractAlbumModel::internalDataTrack(const MusicAudioTrack &track, const QModelIndex &index, int role) const
{
    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
        return track.mTitle;
    case ColumnsRoles::DurationRole:
    {
        QTime trackDuration = track.mDuration;
        if (trackDuration.hour() == 0) {
            return trackDuration.toString(QStringLiteral("mm:ss"));
        } else {
            return trackDuration.toString();
        }
    }
    case ColumnsRoles::CreatorRole:
        return track.mArtist;
    case ColumnsRoles::ArtistRole:
        return track.mArtist;
    case ColumnsRoles::AlbumRole:
        return track.mAlbumName;
    case ColumnsRoles::TrackNumberRole:
        return track.mTrackNumber;
    case ColumnsRoles::RatingRole:
        return 0;
    case ColumnsRoles::ImageRole:
        return data(index.parent(), role);
    case ColumnsRoles::ResourceRole:
        return track.mResourceURI;
    case ColumnsRoles::ItemClassRole:
        return {};
    case ColumnsRoles::CountRole:
        return {};
    case ColumnsRoles::IdRole:
        return track.mTitle;
    case ColumnsRoles::IsPlayingRole:
        return false;
    }

    return {};
}

QModelIndex AbstractAlbumModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column != 0) {
        return {};
    }

    if (!parent.isValid()) {
        return createIndex(row, column, nullptr);
    }

    const auto albumCount = d->mMusicDatabase->albumCount();

    if (parent.row() < 0 || parent.row() >= albumCount) {
        return {};
    }

    const auto &currentAlbum = d->mMusicDatabase->albumFromIndex(parent.row());

    if (row >= currentAlbum.mTracksCount) {
        return {};
    }

    return createIndex(row, column, currentAlbum.mTrackIds[row]);
}

QModelIndex AbstractAlbumModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return {};
    }

    if (child.internalId() == 0) {
        return {};
    }

    const auto albumId = d->mMusicDatabase->albumIdFromTrackId(child.internalId());

    return index(albumId, 0);
}

int AbstractAlbumModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

DatabaseInterface *AbstractAlbumModel::musicDatabase() const
{
    return d->mMusicDatabase;
}

void AbstractAlbumModel::setMusicDatabase(DatabaseInterface *musicDatabase)
{
    if (d->mMusicDatabase == musicDatabase) {
        return;
    }

    emit musicDatabaseChanged();
}

void AbstractAlbumModel::albumsList(const QVector<MusicAlbum> &allAlbums)
{
    beginResetModel();
    if (d->mMusicDatabase) {
        d->mMusicDatabase->insertAlbumsList(allAlbums);
    }
    endResetModel();

    return;
}

void AbstractAlbumModel::tracksList(QHash<QString, QVector<MusicAudioTrack> > tracks, QHash<QString, QString> covers)
{
    beginResetModel();
    if (d->mMusicDatabase) {
        d->mMusicDatabase->insertTracksList(tracks, covers);
    }
    endResetModel();

    return;
}

#include "moc_abstractalbummodel.cpp"
