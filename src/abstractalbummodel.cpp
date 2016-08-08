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

    DatabaseInterface *mMusicDatabase = nullptr;

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
    if (!d->mMusicDatabase) {
        return 0;
    }

    const auto albumCount = d->mMusicDatabase->albumCount();

    if (!parent.isValid()) {
        return albumCount;
    }

    if (parent.row() < 0 || parent.row() >= albumCount) {
        return 0;
    }

    const auto &currentAlbum = d->mMusicDatabase->albumFromIndex(parent.row());
    if (!currentAlbum.isValid()) {
        return 0;
    }

    return currentAlbum.tracksCount();
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
    if (!d->mMusicDatabase) {
        return {};
    }

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

            return internalDataAlbum(index.row(), role);
        }
    }

    const auto &currentAlbum = d->mMusicDatabase->albumFromIndex(index.parent().row());

    if (!currentAlbum.isValid()) {
        return {};
    }

    if (index.row() >= currentAlbum.tracksCount()) {
        return {};
    }

    const auto &currentTrack = currentAlbum.trackFromIndex(index.row());

    if (!currentTrack.isValid()) {
        return {};
    }

    return internalDataTrack(currentTrack, index, role);
}

QVariant AbstractAlbumModel::internalDataAlbum(int albumIndex, int role) const
{
    if (!d->mMusicDatabase) {
        return {};
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
        return d->mMusicDatabase->albumDataFromIndex(albumIndex, DatabaseInterface::AlbumData::Title);
    case ColumnsRoles::DurationRole:
    case ColumnsRoles::MilliSecondsDurationRole:
        return {};
    case ColumnsRoles::CreatorRole:
        return {};
    case ColumnsRoles::ArtistRole:
        return d->mMusicDatabase->albumDataFromIndex(albumIndex, DatabaseInterface::AlbumData::Artist);
    case ColumnsRoles::AlbumRole:
        return {};
    case ColumnsRoles::TrackNumberRole:
        return {};
    case ColumnsRoles::RatingRole:
        return {};
    case ColumnsRoles::ImageRole:
    {
        auto albumArt = d->mMusicDatabase->albumDataFromIndex(albumIndex, DatabaseInterface::AlbumData::Image);
        if (albumArt.isValid() && albumArt.toUrl().isValid()) {
            return albumArt;
        } else {
            if (d->mUseLocalIcons) {
                return QUrl(QStringLiteral("qrc:/media-optical-audio.svg"));
            } else {
                return QUrl(QStringLiteral("image://icon/media-optical-audio"));
            }
        }
    }
    case ColumnsRoles::ResourceRole:
        return {};
    case ColumnsRoles::ItemClassRole:
        return {};
    case ColumnsRoles::CountRole:
        return d->mMusicDatabase->albumDataFromIndex(albumIndex, DatabaseInterface::AlbumData::TracksCount);
    case ColumnsRoles::IdRole:
        return d->mMusicDatabase->albumDataFromIndex(albumIndex, DatabaseInterface::AlbumData::Id);
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
        return data(index.parent(), role);
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

    if (row >= currentAlbum.tracksCount()) {
        return {};
    }

    return createIndex(row, column, d->mMusicDatabase->albumPositionByIndex(currentAlbum.databaseId()));
}

QModelIndex AbstractAlbumModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return {};
    }

    if (child.internalId() == 0) {
        return {};
    }

    return index(child.internalId(), 0);
}

int AbstractAlbumModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

DatabaseInterface *AbstractAlbumModel::databaseInterface() const
{
    return d->mMusicDatabase;
}

void AbstractAlbumModel::setDatabaseInterface(DatabaseInterface *musicDatabase)
{
    if (d->mMusicDatabase == musicDatabase) {
        return;
    }

    if (d->mMusicDatabase) {
        disconnect(d->mMusicDatabase);
    }

    d->mMusicDatabase = musicDatabase;

    if (d->mMusicDatabase) {
        connect(d->mMusicDatabase, &DatabaseInterface::resetModel, this, &AbstractAlbumModel::databaseReset);
    }

    emit databaseInterfaceChanged();
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

void AbstractAlbumModel::databaseReset()
{
    beginResetModel();
    endResetModel();
}

#include "moc_abstractalbummodel.cpp"
