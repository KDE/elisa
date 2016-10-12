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

#include "albummodel.h"
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

class AlbumModelPrivate
{
public:

    AlbumModelPrivate()
    {
    }

    DatabaseInterface *mMusicDatabase = nullptr;

    bool mUseLocalIcons = false;

    QString mTitle;

    QString mAuthor;

    MusicAlbum mCurrentAlbum;

};

AlbumModel::AlbumModel(QObject *parent) : QAbstractItemModel(parent), d(new AlbumModelPrivate)
{
    Q_EMIT refreshContent();
}

AlbumModel::~AlbumModel()
{
    delete d;
}

int AlbumModel::rowCount(const QModelIndex &parent) const
{
    if (!d->mMusicDatabase) {
        return 0;
    }

    if (parent.isValid()) {
        return 0;
    }

    if (!d->mCurrentAlbum.isValid()) {
        const auto &currentAlbum = d->mMusicDatabase->albumFromTitleAndAuthor(d->mTitle, d->mAuthor);
        if (!currentAlbum.isValid()) {
            return 0;
        }

        d->mCurrentAlbum = currentAlbum;
    }

    return d->mCurrentAlbum.tracksCount();
}

QHash<int, QByteArray> AlbumModel::roleNames() const
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
    roles[static_cast<int>(ColumnsRoles::DatabaseIdRole)] = "databaseId";

    return roles;
}

Qt::ItemFlags AlbumModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant AlbumModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    if (!d->mMusicDatabase) {
        return result;
    }

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

    if (!d->mCurrentAlbum.isValid()) {
        const auto &currentAlbum = d->mMusicDatabase->albumFromTitleAndAuthor(d->mTitle, d->mAuthor);
        if (!currentAlbum.isValid()) {
            return result;
        }

        d->mCurrentAlbum = currentAlbum;
    }

    if (index.row() >= d->mCurrentAlbum.tracksCount()) {
        return result;
    }

    const auto &currentTrack = d->mCurrentAlbum.trackFromIndex(index.row());

    if (!currentTrack.isValid()) {
        return result;
    }

    result = internalDataTrack(currentTrack, role);

    return result;
}

QVariant AlbumModel::internalDataTrack(const MusicAudioTrack &track, int role) const
{
    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
        return track.title();
    case ColumnsRoles::MilliSecondsDurationRole:
        return track.duration().msecsSinceStartOfDay();
    case ColumnsRoles::DurationRole:
    {
        QTime trackDuration = track.duration();
        if (trackDuration.hour() == 0) {
            return trackDuration.toString(QStringLiteral("mm:ss"));
        } else {
            return trackDuration.toString();
        }
    }
    case ColumnsRoles::CreatorRole:
        return track.artist();
    case ColumnsRoles::ArtistRole:
        return track.artist();
    case ColumnsRoles::AlbumRole:
        return track.albumName();
    case ColumnsRoles::TrackNumberRole:
        return track.trackNumber();
    case ColumnsRoles::RatingRole:
        return 0;
    case ColumnsRoles::ImageRole:
    {
        QVariant result;

        if (d->mCurrentAlbum.albumArtURI().isValid()) {
            result = d->mCurrentAlbum.albumArtURI();
        } else {
            if (d->mUseLocalIcons) {
                result = QUrl(QStringLiteral("qrc:/media-optical-audio.svg"));
            } else {
                result = QUrl(QStringLiteral("image://icon/media-optical-audio"));
            }
        }
        return result;
    }
    case ColumnsRoles::ResourceRole:
        return track.resourceURI();
    case ColumnsRoles::ItemClassRole:
        return {};
    case ColumnsRoles::CountRole:
        return {};
    case ColumnsRoles::IdRole:
        return track.title();
    case ColumnsRoles::IsPlayingRole:
        return false;
    case ColumnsRoles::DatabaseIdRole:
        return track.databaseId();
    }

    return {};
}

QModelIndex AlbumModel::index(int row, int column, const QModelIndex &parent) const
{
    auto result = QModelIndex();

    if (column != 0) {
        return result;
    }

    if (parent.isValid()) {
        return result;
    }

    const auto &currentAlbum = d->mMusicDatabase->albumFromTitleAndAuthor(d->mTitle, d->mAuthor);
    if (!currentAlbum.isValid()) {
        return result;
    }
    if (currentAlbum.title() != d->mTitle) {
        return result;
    }
    if (currentAlbum.artist() != d->mAuthor) {
        return result;
    }

    if (row >= currentAlbum.tracksCount()) {
        return result;
    }

    return createIndex(row, column);
}

QModelIndex AlbumModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return {};
}

int AlbumModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

DatabaseInterface *AlbumModel::databaseInterface() const
{
    return d->mMusicDatabase;
}

QString AlbumModel::title() const
{
    return d->mTitle;
}

QString AlbumModel::author() const
{
    return d->mAuthor;
}

void AlbumModel::setDatabaseInterface(DatabaseInterface *musicDatabase)
{
    if (d->mMusicDatabase == musicDatabase) {
        return;
    }

    if (d->mMusicDatabase) {
        disconnect(d->mMusicDatabase);
    }

    d->mMusicDatabase = musicDatabase;

    if (d->mMusicDatabase) {
        connect(d->mMusicDatabase, &DatabaseInterface::beginTrackAdded, this, &AlbumModel::beginTrackAdded);
        connect(d->mMusicDatabase, &DatabaseInterface::endTrackAdded, this, &AlbumModel::endTrackAdded);
    }

    emit databaseInterfaceChanged();
}

void AlbumModel::albumsList(const QVector<MusicAlbum> &allAlbums)
{
    beginResetModel();
    if (d->mMusicDatabase) {
        d->mMusicDatabase->insertAlbumsList(allAlbums);
    }
    endResetModel();

    return;
}

void AlbumModel::tracksList(QHash<QString, QVector<MusicAudioTrack> > tracks, QHash<QString, QUrl> covers)
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

void AlbumModel::setTitle(QString title)
{
    if (d->mTitle == title)
        return;

    d->mTitle = title;
    emit titleChanged();
}

void AlbumModel::setAuthor(QString author)
{
    if (d->mAuthor == author)
        return;

    d->mAuthor = author;
    emit authorChanged();
}

void AlbumModel::beginTrackAdded(QVector<qulonglong> newTracks)
{
}

void AlbumModel::endTrackAdded(QVector<qulonglong> newTracks)
{
    beginResetModel();
    endResetModel();
}


#include "moc_albummodel.cpp"
