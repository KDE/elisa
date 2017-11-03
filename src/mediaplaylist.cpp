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

#include "mediaplaylist.h"
#include "databaseinterface.h"
#include "musicaudiotrack.h"
#include "musiclistenersmanager.h"

#include <QUrl>
#include <QPersistentModelIndex>
#include <QList>
#include <QDebug>

#include <algorithm>

class MediaPlayListPrivate
{
public:

    QList<MediaPlayListEntry> mData;

    QList<MusicAudioTrack> mTrackData;

    MusicListenersManager* mMusicListenersManager = nullptr;

};

MediaPlayList::MediaPlayList(QObject *parent) : QAbstractListModel(parent), d(new MediaPlayListPrivate)
{
}

MediaPlayList::~MediaPlayList()
= default;

int MediaPlayList::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return d->mData.size();
}

QVariant MediaPlayList::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    if (!index.isValid()) {
        return result;
    }

    if (index.row() < 0 || index.row() >= d->mData.size()) {
        return result;
    }

    if (role < ColumnsRoles::IsValidRole || role > ColumnsRoles::IsSingleDiscAlbumHeader) {
        return result;
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    if (d->mData[index.row()].mIsValid) {
        switch(convertedRole)
        {
        case ColumnsRoles::IsValidRole:
            result = d->mData[index.row()].mIsValid;
            break;
        case ColumnsRoles::TitleRole:
            result = d->mTrackData[index.row()].title();
            break;
        case ColumnsRoles::DurationRole:
        {
            const QTime &trackDuration = d->mTrackData[index.row()].duration();
            if (trackDuration.hour() == 0) {
                result = trackDuration.toString(QStringLiteral("mm:ss"));
            } else {
                result = trackDuration.toString();
            }
            break;
        }
        case ColumnsRoles::MilliSecondsDurationRole:
            result = d->mTrackData[index.row()].duration().msecsSinceStartOfDay();
            break;
        case ColumnsRoles::ArtistRole:
            result = d->mTrackData[index.row()].artist();
            break;
        case ColumnsRoles::AlbumArtistRole:
            result = d->mTrackData[index.row()].albumArtist();
            break;
        case ColumnsRoles::AlbumRole:
            result = d->mTrackData[index.row()].albumName();
            break;
        case ColumnsRoles::TrackNumberRole:
            result = d->mTrackData[index.row()].trackNumber();
            break;
        case ColumnsRoles::DiscNumberRole:
            if (d->mTrackData[index.row()].discNumber() > 0) {
                result = d->mTrackData[index.row()].discNumber();
            }
            break;
        case ColumnsRoles::IsSingleDiscAlbumHeader:
            result = d->mTrackData[index.row()].isSingleDiscAlbum();
            break;
        case ColumnsRoles::ResourceRole:
            result = d->mTrackData[index.row()].resourceURI();
            break;
        case ColumnsRoles::ImageRole:
        {
            auto albumArt = d->mTrackData[index.row()].albumCover();
            if (albumArt.isValid()) {
                result = albumArt;
            }
            break;
        }
        case ColumnsRoles::HasAlbumHeader:
            result = rowHasHeader(index.row());
            break;
        case ColumnsRoles::RatingRole:
            result = d->mTrackData[index.row()].rating();
            break;
        case ColumnsRoles::CountRole:
            break;
        case ColumnsRoles::CreatorRole:
            break;
        case ColumnsRoles::IsPlayingRole:
            result = d->mData[index.row()].mIsPlaying;
            break;
        }
    } else {
        switch(convertedRole)
        {
        case ColumnsRoles::IsValidRole:
            result = d->mData[index.row()].mIsValid;
            break;
        case ColumnsRoles::TitleRole:
            result = d->mData[index.row()].mTitle;
            break;
        case ColumnsRoles::IsPlayingRole:
            result = d->mData[index.row()].mIsPlaying;
            break;
        case ColumnsRoles::ArtistRole:
            result = d->mData[index.row()].mArtist;
            break;
        case ColumnsRoles::AlbumArtistRole:
            result = d->mData[index.row()].mArtist;
            break;
        case ColumnsRoles::AlbumRole:
            result = d->mData[index.row()].mAlbum;
            break;
        case ColumnsRoles::TrackNumberRole:
            result = -1;
            break;
        case ColumnsRoles::HasAlbumHeader:
            result = rowHasHeader(index.row());
            break;
        case ColumnsRoles::DurationRole:
            break;
        case ColumnsRoles::DiscNumberRole:
            break;
        case ColumnsRoles::IsSingleDiscAlbumHeader:
            break;
        case ColumnsRoles::MilliSecondsDurationRole:
            break;
        case ColumnsRoles::ResourceRole:
            break;
        case ColumnsRoles::RatingRole:
            break;
        case ColumnsRoles::CountRole:
            break;
        case ColumnsRoles::CreatorRole:
            break;
        case ColumnsRoles::ImageRole:
            result = QStringLiteral("");
            break;
        }
    }

    return result;
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
    {
        modelModified = true;
        PlayState newState = static_cast<PlayState>(value.toInt());
        d->mData[index.row()].mIsPlaying = newState;
        Q_EMIT dataChanged(index, index, {role});
        break;
    }
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
    roles[static_cast<int>(ColumnsRoles::AlbumArtistRole)] = "albumArtist";
    roles[static_cast<int>(ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(ColumnsRoles::DiscNumberRole)] = "discNumber";
    roles[static_cast<int>(ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::CountRole)] = "count";
    roles[static_cast<int>(ColumnsRoles::IsPlayingRole)] = "isPlaying";
    roles[static_cast<int>(ColumnsRoles::HasAlbumHeader)] = "hasAlbumHeader";
    roles[static_cast<int>(ColumnsRoles::IsSingleDiscAlbumHeader)] = "isSingleDiscAlbum";

    return roles;
}

bool MediaPlayList::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);

    bool hadAlbumHeader = false;

    if (rowCount() > row + count) {
        hadAlbumHeader = rowHasHeader(row + count);
    }

    for (int i = row, cpt = 0; cpt < count; ++i, ++cpt) {
        d->mData.removeAt(i);
        d->mTrackData.removeAt(i);
    }
    endRemoveRows();

    Q_EMIT tracksCountChanged();

    if (hadAlbumHeader != rowHasHeader(row)) {
        Q_EMIT dataChanged(index(row, 0), index(row, 0), {ColumnsRoles::HasAlbumHeader});
    }

    Q_EMIT persistentStateChanged();

    return false;
}

void MediaPlayList::enqueue(qulonglong newTrackId)
{
    enqueue(MediaPlayListEntry(newTrackId));
}

void MediaPlayList::enqueue(const MusicAudioTrack &newTrack)
{
    enqueue(MediaPlayListEntry(newTrack), newTrack);
}

void MediaPlayList::clearAndEnqueue(qulonglong newTrackId)
{
    clearPlayList();
    enqueue(MediaPlayListEntry(newTrackId));
}

void MediaPlayList::clearAndEnqueue(const MusicAudioTrack &newTrack)
{
    clearPlayList();
    enqueue(newTrack);
}

void MediaPlayList::enqueue(const MediaPlayListEntry &newEntry, const MusicAudioTrack &audioTrack)
{
    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size());
    d->mData.push_back(newEntry);
    if (audioTrack.isValid()) {
        d->mTrackData.push_back(audioTrack);
    } else {
        d->mTrackData.push_back({});
    }
    endInsertRows();

    Q_EMIT tracksCountChanged();
    Q_EMIT persistentStateChanged();

    if (!newEntry.mIsValid) {
        Q_EMIT newTrackByNameInList(newEntry.mTitle, newEntry.mArtist, newEntry.mAlbum);
    } else {
        Q_EMIT newTrackByIdInList(newEntry.mId);
    }

    Q_EMIT trackHasBeenAdded(data(index(d->mData.size() - 1, 0), ColumnsRoles::TitleRole).toString(), data(index(d->mData.size() - 1, 0), ColumnsRoles::ImageRole).toUrl());

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
    auto nextTrackHasHeader = rowHasHeader(sourceRow + count);
    auto futureNextTrackHasHeader = rowHasHeader(destinationChild);
    if (sourceRow < destinationChild) {
        nextTrackHasHeader = rowHasHeader(sourceRow + count);
    }

    for (auto cptItem = 0; cptItem < count; ++cptItem) {
        if (sourceRow < destinationChild) {
            d->mData.move(sourceRow, destinationChild - 1);
            d->mTrackData.move(sourceRow, destinationChild - 1);
        } else {
            d->mData.move(sourceRow, destinationChild);
            d->mTrackData.move(sourceRow, destinationChild);
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
        if (nextTrackHasHeader != rowHasHeader(sourceRow + count)) {
            Q_EMIT dataChanged(index(sourceRow + count, 0), index(sourceRow + count, 0), {ColumnsRoles::HasAlbumHeader});
        }
    }

    if (sourceRow < destinationChild) {
        if (futureNextTrackHasHeader != rowHasHeader(destinationChild + count - 1)) {
            Q_EMIT dataChanged(index(destinationChild + count - 1, 0), index(destinationChild + count - 1, 0), {ColumnsRoles::HasAlbumHeader});
        }
    } else {
        if (futureNextTrackHasHeader != rowHasHeader(destinationChild + count)) {
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

void MediaPlayList::enqueue(const MusicAlbum &album)
{
    for (auto oneTrackIndex = 0; oneTrackIndex < album.tracksCount(); ++oneTrackIndex) {
        enqueue(album.trackFromIndex(oneTrackIndex));
    }
}

void MediaPlayList::enqueue(const QString &artistName)
{
    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size());
    d->mData.push_back(MediaPlayListEntry{artistName});
    d->mTrackData.push_back({});
    endInsertRows();

    Q_EMIT tracksCountChanged();
    Q_EMIT newArtistInList(artistName);
    Q_EMIT persistentStateChanged();
}

void MediaPlayList::clearAndEnqueue(const MusicAlbum &album)
{
    clearPlayList();
    enqueue(album);
}

void MediaPlayList::clearAndEnqueue(const QString &artistName)
{
    clearPlayList();
    enqueue(artistName);
}

void MediaPlayList::clearPlayList()
{
    beginRemoveRows({}, 0, d->mData.count());
    d->mData.clear();
    d->mTrackData.clear();
    endRemoveRows();

    Q_EMIT tracksCountChanged();
}

QList<QVariant> MediaPlayList::persistentState() const
{
    auto result = QList<QVariant>();

    for (int trackIndex = 0; trackIndex < d->mData.size(); ++trackIndex) {
        auto oneData = QList<QString>();
        const auto &oneEntry = d->mData[trackIndex];
        if (oneEntry.mIsValid) {
            const auto &oneTrack = d->mTrackData[trackIndex];

            oneData.push_back(oneTrack.title());
            oneData.push_back(oneTrack.albumName());
            oneData.push_back(oneTrack.artist());

            result.push_back(QVariant(oneData));
        }
    }

    return result;
}

MusicListenersManager *MediaPlayList::musicListenersManager() const
{
    return d->mMusicListenersManager;
}

int MediaPlayList::tracksCount() const
{
    return rowCount();
}

void MediaPlayList::setPersistentState(const QList<QVariant> &persistentState)
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

void MediaPlayList::albumAdded(const QList<MusicAudioTrack> &tracks)
{
    for (int playListIndex = 0; playListIndex < d->mData.size(); ++playListIndex) {
        auto &oneEntry = d->mData[playListIndex];

        if (!oneEntry.mIsArtist || oneEntry.mIsValid) {
            continue;
        }

        if (oneEntry.mArtist != tracks.first().artist()) {
            continue;
        }

        d->mTrackData[playListIndex] = tracks.first();
        oneEntry.mId = tracks.first().databaseId();
        oneEntry.mIsValid = true;
        oneEntry.mIsArtist = false;

        Q_EMIT dataChanged(index(playListIndex, 0), index(playListIndex, 0), {});

        beginInsertRows(QModelIndex(), playListIndex + 1, playListIndex - 1 + tracks.size());
        for (int trackIndex = 1; trackIndex < tracks.size(); ++trackIndex) {
            d->mData.push_back(MediaPlayListEntry{tracks[trackIndex].databaseId()});
            d->mTrackData.push_back(tracks[trackIndex]);
        }
        endInsertRows();

        Q_EMIT tracksCountChanged();
        Q_EMIT persistentStateChanged();
    }
}

void MediaPlayList::trackChanged(const MusicAudioTrack &track)
{
    for (int i = 0; i < d->mData.size(); ++i) {
        auto &oneEntry = d->mData[i];

        if (!oneEntry.mIsArtist && oneEntry.mIsValid) {
            if (track.databaseId() != oneEntry.mId) {
                continue;
            }

            if (d->mTrackData[i] != track) {
                d->mTrackData[i] = track;

                Q_EMIT dataChanged(index(i, 0), index(i, 0), {});
            }
            continue;
        } else if (!oneEntry.mIsArtist && !oneEntry.mIsValid) {
            if (track.title() != oneEntry.mTitle) {
                continue;
            }

            if (track.albumName() != oneEntry.mAlbum) {
                continue;
            }

            if (track.artist() != oneEntry.mArtist) {
                continue;
            }

            d->mTrackData[i] = track;
            oneEntry.mId = track.databaseId();
            oneEntry.mIsValid = true;

            Q_EMIT dataChanged(index(i, 0), index(i, 0), {});
            break;
        }
    }
}

void MediaPlayList::trackRemoved(const MusicAudioTrack &track)
{
    for (int i = 0; i < d->mData.size(); ++i) {
        auto &oneEntry = d->mData[i];

        if (oneEntry.mIsValid) {
            if (oneEntry.mId == track.databaseId()) {
                oneEntry.mTitle = track.title();
                oneEntry.mArtist = track.artist();
                oneEntry.mAlbum = track.albumName();

                oneEntry.mIsValid = false;

                Q_EMIT dataChanged(index(i, 0), index(i, 0), {});
            }
        }
    }
}

void MediaPlayList::setMusicListenersManager(MusicListenersManager *musicListenersManager)
{
    if (d->mMusicListenersManager == musicListenersManager) {
        return;
    }

    d->mMusicListenersManager = musicListenersManager;

    if (d->mMusicListenersManager) {
        d->mMusicListenersManager->subscribeForTracks(this);
    }

    Q_EMIT musicListenersManagerChanged();
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

    auto currentAlbumTitle = QString();
    auto currentAlbumArtist = QString();
    if (d->mData[row].mIsValid) {
        currentAlbumTitle = d->mTrackData[row].albumName();
        currentAlbumArtist = d->mTrackData[row].albumArtist();
    } else {
        currentAlbumTitle = d->mData[row].mAlbum;
        currentAlbumArtist = d->mData[row].mArtist;
    }

    auto previousAlbumTitle = QString();
    auto previousAlbumArtist = QString();
    if (d->mData[row - 1].mIsValid) {
        previousAlbumTitle = d->mTrackData[row - 1].albumName();
        previousAlbumArtist = d->mTrackData[row - 1].albumArtist();
    } else {
        previousAlbumTitle = d->mData[row - 1].mAlbum;
        previousAlbumArtist = d->mData[row - 1].mArtist;
    }

    if (currentAlbumTitle == previousAlbumTitle && currentAlbumArtist == previousAlbumArtist) {
        return false;
    }

    return true;
}


#include "moc_mediaplaylist.cpp"
