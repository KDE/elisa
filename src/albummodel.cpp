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

#include "albummodel.h"
#include "musicstatistics.h"
#include "databaseinterface.h"

#include <QUrl>
#include <QTimer>
#include <QPointer>
#include <QVector>

class AlbumModelPrivate
{
public:

    AlbumModelPrivate()
    {
    }

    bool mUseLocalIcons = false;

    QString mTitle;

    QString mAuthor;

    MusicAlbum mCurrentAlbum;

};

AlbumModel::AlbumModel(QObject *parent) : QAbstractItemModel(parent), d(new AlbumModelPrivate)
{
}

AlbumModel::~AlbumModel()
{
    delete d;
}

int AlbumModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
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
    roles[static_cast<int>(ColumnsRoles::AlbumArtistRole)] = "albumArtist";
    roles[static_cast<int>(ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(ColumnsRoles::DiscNumberRole)] = "discNumber";
    roles[static_cast<int>(ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::DatabaseIdRole)] = "databaseId";
    roles[static_cast<int>(ColumnsRoles::DiscFirstTrackRole)] = "isFirstTrackOfDisc";

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

    if (index.row() >= d->mCurrentAlbum.tracksCount()) {
        return result;
    }

    const auto &currentTrack = d->mCurrentAlbum.trackFromIndex(index.row());

    if (!currentTrack.isValid()) {
        return result;
    }

    result = internalDataTrack(currentTrack, role, index.row());

    return result;
}

QVariant AlbumModel::internalDataTrack(const MusicAudioTrack &track, int role, int rowIndex) const
{
    auto result = QVariant();

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
        result = track.title();
        break;
    case ColumnsRoles::MilliSecondsDurationRole:
        result = track.duration().msecsSinceStartOfDay();
        break;
    case ColumnsRoles::DurationRole:
    {
        QTime trackDuration = track.duration();
        if (trackDuration.hour() == 0) {
            result = trackDuration.toString(QStringLiteral("mm:ss"));
        } else {
            result = trackDuration.toString();
        }
        break;
    }
    case ColumnsRoles::CreatorRole:
        result = track.artist();
        break;
    case ColumnsRoles::ArtistRole:
        result = track.artist();
        break;
    case ColumnsRoles::AlbumRole:
        result = track.albumName();
        break;
    case ColumnsRoles::AlbumArtistRole:
        result = track.albumArtist();
        break;
    case ColumnsRoles::TrackNumberRole:
        result = track.trackNumber();
        break;
    case ColumnsRoles::DiscNumberRole:
        if (track.discNumber() > 0) {
            result = track.discNumber();
        }
        break;
    case ColumnsRoles::DiscFirstTrackRole:
        if (rowIndex == 0) {
            result = true;
        } else {
            auto previousTrack = d->mCurrentAlbum.trackFromIndex(rowIndex - 1);
            result = (previousTrack.discNumber() != track.discNumber());
        }
        break;
    case ColumnsRoles::RatingRole:
        result = 0;
        break;
    case ColumnsRoles::ImageRole:
    {
        if (d->mCurrentAlbum.albumArtURI().isValid()) {
            result = d->mCurrentAlbum.albumArtURI();
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
        result = track.resourceURI();
        break;
    case ColumnsRoles::IdRole:
        result = track.title();
        break;
    case ColumnsRoles::DatabaseIdRole:
        result = track.databaseId();
        break;
    }

    return result;
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

    if (row >= d->mCurrentAlbum.tracksCount()) {
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

MusicAlbum AlbumModel::albumData() const
{
    return d->mCurrentAlbum;
}

QString AlbumModel::title() const
{
    return d->mTitle;
}

QString AlbumModel::author() const
{
    return d->mAuthor;
}

void AlbumModel::setAlbumData(MusicAlbum album)
{
    if (d->mCurrentAlbum == album) {
        return;
    }

    if (d->mCurrentAlbum.tracksCount() > 0) {
        beginRemoveRows({}, 0, d->mCurrentAlbum.tracksCount() - 1);
        d->mCurrentAlbum = {};
        endRemoveRows();
    }

    beginInsertRows({}, 0, album.tracksCount() - 1);
    d->mCurrentAlbum = album;
    endInsertRows();

    Q_EMIT albumDataChanged();
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

void AlbumModel::trackAdded(MusicAudioTrack newTrack)
{
    if (newTrack.albumName() != d->mCurrentAlbum.title()) {
        return;
    }

    auto trackIndex = d->mCurrentAlbum.trackIndexFromId(newTrack.databaseId());

    if (trackIndex != -1) {
        return;
    }

    bool trackInserted = false;
    for (int trackIndex = 0; trackIndex < d->mCurrentAlbum.tracksCount(); ++trackIndex) {
        const auto &oneTrack = d->mCurrentAlbum.trackFromIndex(trackIndex);

        if (oneTrack.discNumber() == newTrack.discNumber() && oneTrack.trackNumber() > newTrack.trackNumber()) {
            beginInsertRows({}, trackIndex, trackIndex);
            d->mCurrentAlbum.insertTrack(newTrack, trackIndex);
            endInsertRows();
            trackInserted = true;
            break;
        }
    }

    if (!trackInserted) {
        beginInsertRows({}, d->mCurrentAlbum.tracksCount(), d->mCurrentAlbum.tracksCount());
        d->mCurrentAlbum.insertTrack(newTrack, d->mCurrentAlbum.tracksCount());
        endInsertRows();
    }
}

void AlbumModel::trackModified(MusicAudioTrack modifiedTrack)
{
    if (modifiedTrack.albumName() != d->mCurrentAlbum.title()) {
        return;
    }

    auto trackIndex = d->mCurrentAlbum.trackIndexFromId(modifiedTrack.databaseId());

    if (trackIndex == -1) {
        return;
    }

    d->mCurrentAlbum.updateTrack(modifiedTrack, trackIndex);
    Q_EMIT dataChanged(index(trackIndex, 0), index(trackIndex, 0));
}

void AlbumModel::trackRemoved(MusicAudioTrack removedTrack)
{
    if (removedTrack.albumName() != d->mCurrentAlbum.title()) {
        return;
    }

    auto trackIndex = d->mCurrentAlbum.trackIndexFromId(removedTrack.databaseId());

    if (trackIndex == -1) {
        return;
    }

    beginRemoveRows({}, trackIndex, trackIndex);
    d->mCurrentAlbum.removeTrackFromIndex(trackIndex);
    endRemoveRows();
}


#include "moc_albummodel.cpp"
