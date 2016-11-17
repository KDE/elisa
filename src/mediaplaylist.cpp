/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "mediaplaylist.h"
#include "databaseinterface.h"

#include <QtCore/QUrl>
#include <QtCore/QPersistentModelIndex>
#include <QtCore/QList>
#include <QtCore/QDebug>

#include <algorithm>

class MediaPlayListPrivate
{
public:

    QList<MediaPlayListEntry> mData;

    DatabaseInterface *mMusicDatabase = nullptr;

    bool mUseLocalIcons = false;

};

MediaPlayList::MediaPlayList(QObject *parent) : QAbstractListModel(parent), d(new MediaPlayListPrivate)
{
}

MediaPlayList::~MediaPlayList()
{
    delete d;
}

int MediaPlayList::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return d->mData.size();
}

QVariant MediaPlayList::data(const QModelIndex &index, int role) const
{
    if (!d->mMusicDatabase) {
        return {};
    }

    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= d->mData.size()) {
        return QVariant();
    }

    if (role < ColumnsRoles::IsValidRole || role > ColumnsRoles::HasAlbumHeader) {
        return QVariant();
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    if (d->mData[index.row()].mIsValid) {
        switch(convertedRole)
        {
        case ColumnsRoles::IsValidRole:
            return d->mData[index.row()].mIsValid;
        case ColumnsRoles::TitleRole:
            return d->mMusicDatabase->trackDataFromDatabaseId(d->mData[index.row()].mId, DatabaseInterface::TrackData::Title);
        case ColumnsRoles::DurationRole:
        {
            QTime trackDuration = d->mMusicDatabase->trackDataFromDatabaseId(d->mData[index.row()].mId, DatabaseInterface::TrackData::Duration).toTime();
            if (trackDuration.hour() == 0) {
                return trackDuration.toString(QStringLiteral("mm:ss"));
            } else {
                return trackDuration.toString();
            }
        }
        case ColumnsRoles::MilliSecondsDurationRole:
            return d->mMusicDatabase->trackDataFromDatabaseId(d->mData[index.row()].mId, DatabaseInterface::TrackData::MilliSecondsDuration);
        case ColumnsRoles::ArtistRole:
            return d->mMusicDatabase->trackDataFromDatabaseId(d->mData[index.row()].mId, DatabaseInterface::TrackData::Artist);
        case ColumnsRoles::AlbumRole:
            return d->mMusicDatabase->trackDataFromDatabaseId(d->mData[index.row()].mId, DatabaseInterface::TrackData::Album);
        case ColumnsRoles::TrackNumberRole:
            return d->mMusicDatabase->trackDataFromDatabaseId(d->mData[index.row()].mId, DatabaseInterface::TrackData::TrackNumber);
        case ColumnsRoles::ResourceRole:
            return d->mMusicDatabase->trackDataFromDatabaseId(d->mData[index.row()].mId, DatabaseInterface::TrackData::Resource);
        case ColumnsRoles::ImageRole:
        {
            QVariant result;
            auto albumArt = d->mMusicDatabase->trackDataFromDatabaseId(d->mData[index.row()].mId, DatabaseInterface::TrackData::Image);
            if (albumArt.isValid() && albumArt.toUrl().isValid()) {
                result = albumArt;
            } else {
                if (d->mUseLocalIcons) {
                    result = QUrl(QStringLiteral("qrc:/media-optical-audio.svg"));
                } else {
                    result = QUrl(QStringLiteral("image://icon/media-optical-audio"));
                }
            }
            return result;
        }
        case ColumnsRoles::HasAlbumHeader:
            return rowHasHeader(index.row());
        case ColumnsRoles::RatingRole:
        case ColumnsRoles::CountRole:
        case ColumnsRoles::CreatorRole:
            return QVariant();
        case ColumnsRoles::IsPlayingRole:
            return d->mData[index.row()].mIsPlaying;
        }
    } else {
        switch(convertedRole)
        {
        case ColumnsRoles::IsValidRole:
            return d->mData[index.row()].mIsValid;
        case ColumnsRoles::TitleRole:
            return d->mData[index.row()].mTitle;
        case ColumnsRoles::IsPlayingRole:
            return d->mData[index.row()].mIsPlaying;
        case ColumnsRoles::ArtistRole:
            return d->mData[index.row()].mArtist;
        case ColumnsRoles::AlbumRole:
            return d->mData[index.row()].mAlbum;
        case ColumnsRoles::TrackNumberRole:
            return -1;
        case ColumnsRoles::HasAlbumHeader:
            return rowHasHeader(index.row());
        case ColumnsRoles::DurationRole:
        case ColumnsRoles::MilliSecondsDurationRole:
        case ColumnsRoles::ResourceRole:
        case ColumnsRoles::RatingRole:
        case ColumnsRoles::CountRole:
        case ColumnsRoles::CreatorRole:
        case ColumnsRoles::ImageRole:
            return {QStringLiteral("")};
        }
    }

    return QVariant();
}

bool MediaPlayList::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool modelModified = false;

    if (!index.isValid()) {
        return modelModified;
    }

    if (index.row() < 0 || index.row() >= d->mData.size()) {
        return modelModified;
    }

    if (role < ColumnsRoles::IsValidRole || role > ColumnsRoles::HasAlbumHeader) {
        return modelModified;
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::IsPlayingRole:
        modelModified = true;
        d->mData[index.row()].mIsPlaying = value.toBool();
        Q_EMIT dataChanged(index, index, {role});
        break;
    default:
        modelModified = false;
    }

    return modelModified;
}

QHash<int, QByteArray> MediaPlayList::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::IsValidRole)] = "isValid";
    roles[static_cast<int>(ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::CountRole)] = "count";
    roles[static_cast<int>(ColumnsRoles::IsPlayingRole)] = "isPlaying";
    roles[static_cast<int>(ColumnsRoles::HasAlbumHeader)] = "hasAlbumHeader";

    return roles;
}

bool MediaPlayList::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);

    bool willChangeData = false;

    if (rowCount() > row + count && row > 0) {
        auto currentAlbum = d->mMusicDatabase->trackDataFromDatabaseId(d->mData[row - 1].mId, DatabaseInterface::TrackData::Album).toString();
        auto nextAlbum = d->mMusicDatabase->trackDataFromDatabaseId(d->mData[row + count].mId, DatabaseInterface::TrackData::Album).toString();

        if (currentAlbum == nextAlbum) {
            willChangeData = true;
        }
    }

    for (int i = row, cpt = 0; cpt < count; ++i, ++cpt) {
        d->mData.removeAt(i);
    }
    endRemoveRows();

    if (willChangeData) {
        Q_EMIT dataChanged(index(row, 0), index(row, 0), {ColumnsRoles::HasAlbumHeader});
    }

    emit persistentStateChanged();

    return false;
}

void MediaPlayList::enqueue(qulonglong newTrackId)
{
    enqueue(MediaPlayListEntry(newTrackId));
}

void MediaPlayList::clearAndEnqueue(qulonglong newTrackId)
{
    clearPlayList();
    enqueue(MediaPlayListEntry(newTrackId));
}

void MediaPlayList::enqueue(MediaPlayListEntry newEntry)
{
    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size());
    d->mData.push_back(newEntry);
    endInsertRows();

    emit persistentStateChanged();
    Q_EMIT trackHasBeenAdded(data(index(d->mData.size() - 1, 0), ColumnsRoles::TitleRole).toString(), data(index(d->mData.size() - 1, 0), ColumnsRoles::ImageRole).toUrl());

    if (d->mMusicDatabase && !newEntry.mIsValid) {
        auto newTrackId = d->mMusicDatabase->trackIdFromTitleAlbumArtist(newEntry.mTitle, newEntry.mAlbum, newEntry.mArtist);

        if (newTrackId != 0) {
            d->mData.last().mId = newTrackId;
            d->mData.last().mIsValid = true;

            Q_EMIT dataChanged(index(rowCount() - 1, 0), index(rowCount() - 1, 0), {});
        }
    }
    if (!newEntry.mIsValid) {
        Q_EMIT dataChanged(index(rowCount() - 1, 0), index(rowCount() - 1, 0), {MediaPlayList::HasAlbumHeader});
    }
}

bool MediaPlayList::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (sourceParent != destinationParent) {
        return false;
    }

    if (!beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild)) {
        return false;
    }

    auto firstMovedTrackHasHeader = rowHasHeader(sourceRow);
    auto nextTrackHasHeader = rowHasHeader(destinationChild);
    if (sourceRow < destinationChild) {
        nextTrackHasHeader = rowHasHeader(sourceRow + count);
    }

    for (auto cptItem = 0; cptItem < count; ++cptItem) {
        if (sourceRow < destinationChild) {
            d->mData.move(sourceRow, destinationChild - 1);
        } else {
            d->mData.move(sourceRow, destinationChild);
        }
    }

    endMoveRows();

    if (sourceRow < destinationChild) {
        if (firstMovedTrackHasHeader != rowHasHeader(destinationChild - count)) {
            Q_EMIT dataChanged(index(destinationChild - count, 0), index(destinationChild - count, 0), {ColumnsRoles::HasAlbumHeader});
        }
    } else {
        if (firstMovedTrackHasHeader != rowHasHeader(destinationChild)) {
            Q_EMIT dataChanged(index(destinationChild, 0), index(destinationChild, 0), {ColumnsRoles::HasAlbumHeader});
        }
    }

    if (sourceRow < destinationChild) {
        if (nextTrackHasHeader != rowHasHeader(sourceRow)) {
            Q_EMIT dataChanged(index(sourceRow, 0), index(sourceRow, 0), {ColumnsRoles::HasAlbumHeader});
        }
    } else {
        if (nextTrackHasHeader != rowHasHeader(destinationChild + count)) {
            Q_EMIT dataChanged(index(destinationChild + count, 0), index(destinationChild + count, 0), {ColumnsRoles::HasAlbumHeader});
        }
    }

    Q_EMIT persistentStateChanged();

    return true;
}

void MediaPlayList::move(int from, int to, int n)
{
    if (from < to) {
        moveRows({}, from, n, {}, to + 1);
    } else {
        moveRows({}, from, n, {}, to);
    }
}

void MediaPlayList::enqueue(QString albumName, QString artistName)
{
    if (!d->mMusicDatabase) {
        return;
    }

    const auto currentAlbum = d->mMusicDatabase->albumFromTitleAndAuthor(albumName, artistName);

    const auto allTracksKeys = currentAlbum.tracksKeys();

    for (const auto oneTrackId : allTracksKeys) {
        enqueue(oneTrackId);
    }
}

void MediaPlayList::clearAndEnqueue(QString albumName, QString artistName)
{
    if (!d->mMusicDatabase) {
        return;
    }

    clearPlayList();
    enqueue(albumName, artistName);
}

void MediaPlayList::clearPlayList()
{
    beginRemoveRows({}, 0, d->mData.count());
    d->mData.clear();
    endRemoveRows();
}

DatabaseInterface *MediaPlayList::databaseInterface() const
{
    return d->mMusicDatabase;
}

QList<QVariant> MediaPlayList::persistentState() const
{
    auto result = QList<QVariant>();

    for (auto &oneEntry : d->mData) {
        auto oneData = QList<QString>();

        oneData.push_back(d->mMusicDatabase->trackDataFromDatabaseId(oneEntry.mId, DatabaseInterface::TrackData::Title).toString());
        oneData.push_back(d->mMusicDatabase->trackDataFromDatabaseId(oneEntry.mId, DatabaseInterface::TrackData::Album).toString());
        oneData.push_back(d->mMusicDatabase->trackDataFromDatabaseId(oneEntry.mId, DatabaseInterface::TrackData::Artist).toString());

        result.push_back(QVariant(oneData));
    }

    return result;
}

void MediaPlayList::setDatabaseInterface(DatabaseInterface *musicDatabase)
{
    if (d->mMusicDatabase == musicDatabase) {
        return;
    }

    if (d->mMusicDatabase) {
        disconnect(d->mMusicDatabase, &DatabaseInterface::endTrackAdded,
                   this, &MediaPlayList::endTrackAdded);
    }

    d->mMusicDatabase = musicDatabase;

    connect(d->mMusicDatabase, &DatabaseInterface::endTrackAdded,
            this, &MediaPlayList::endTrackAdded);

    emit databaseInterfaceChanged();
}

void MediaPlayList::setPersistentState(QList<QVariant> persistentState)
{
    qDebug() << "MediaPlayList::setPersistentState" << persistentState;

    for (auto &oneData : persistentState) {
        auto trackData = oneData.toStringList();
        if (trackData.size() != 3) {
            continue;
        }

        auto restoredTitle = trackData[0];
        auto restoredAlbum = trackData[1];
        auto restoredArtist = trackData[2];

        enqueue({restoredTitle, restoredAlbum, restoredArtist});
    }

    emit persistentStateChanged();
}

void MediaPlayList::removeSelection(QList<int> selection)
{
    std::sort(selection.begin(), selection.end());
    std::reverse(selection.begin(), selection.end());
    for (auto oneItem : selection) {
        removeRow(oneItem);
    }
}

void MediaPlayList::endTrackAdded(QVector<qulonglong> newTracks)
{
    for (int i = 0; i < d->mData.size(); ++i) {
        auto &oneEntry = d->mData[i];

        if (oneEntry.mIsValid) {
            continue;
        }

        auto restoredTitle = oneEntry.mTitle;
        auto restoredAlbum = oneEntry.mAlbum;
        auto restoredArtist = oneEntry.mArtist;

        auto newTrackId = d->mMusicDatabase->trackIdFromTitleAlbumArtist(restoredTitle, restoredAlbum, restoredArtist);

        if (newTrackId != 0) {
            oneEntry.mId = newTrackId;
            oneEntry.mIsValid = true;

            Q_EMIT dataChanged(index(i, 0), index(i, 0), {});
        }
    }
}

bool MediaPlayList::rowHasHeader(int row) const
{
    if (row >= rowCount()) {
        return false;
    }

    if (row < 0) {
        return false;
    }

    if (row - 1 < 0) {
        return true;
    }

    auto currentAlbum = QString();
    if (d->mData[row].mIsValid) {
        currentAlbum = d->mMusicDatabase->trackDataFromDatabaseId(d->mData[row].mId, DatabaseInterface::TrackData::Album).toString();
    } else {
        currentAlbum = d->mData[row].mAlbum;
    }

    auto previousAlbum = QString();
    if (d->mData[row - 1].mIsValid) {
        previousAlbum = d->mMusicDatabase->trackDataFromDatabaseId(d->mData[row - 1].mId, DatabaseInterface::TrackData::Album).toString();
    } else {
        previousAlbum = d->mData[row - 1].mAlbum;
    }

    if (currentAlbum == previousAlbum) {
        return false;
    }

    return true;
}


#include "moc_mediaplaylist.cpp"
