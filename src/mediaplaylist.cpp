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
#include <QMediaPlaylist>
#include <QFileInfo>
#include <QDebug>

#include <algorithm>

class MediaPlayListPrivate
{
public:

    QList<MediaPlayListEntry> mData;

    QList<MusicAudioTrack> mTrackData;

    MusicListenersManager* mMusicListenersManager = nullptr;

    QPersistentModelIndex mCurrentTrack;

    int mCurrentPlayListPosition = 0;

    bool mRandomPlay = false;

    bool mRepeatPlay = false;

    QVariantMap mPersistentState;

    QMediaPlaylist mLoadPlaylist;

};

MediaPlayList::MediaPlayList(QObject *parent) : QAbstractListModel(parent), d(new MediaPlayListPrivate)
{
    connect(&d->mLoadPlaylist, &QMediaPlaylist::loaded, this, &MediaPlayList::loadPlayListLoaded);
    connect(&d->mLoadPlaylist, &QMediaPlaylist::loadFailed, this, &MediaPlayList::loadPlayListLoadFailed);
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
            if (!d->mTrackData[index.row()].title().isEmpty()) {
                result = d->mTrackData[index.row()].title();
            } else {
                if (d->mData[index.row()].mTrackUrl.isLocalFile()) {
                    auto localFile = QFileInfo(d->mData[index.row()].mTrackUrl.toLocalFile());
                    result = localFile.fileName();
                } else {
                    result = d->mData[index.row()].mTrackUrl.toString();
                }
            }
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
            if (d->mTrackData[index.row()].resourceURI().isValid()) {
                result = d->mTrackData[index.row()].resourceURI();
            } else {
                result = d->mData[index.row()].mTrackUrl;
            }
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
            if (!d->mData[index.row()].mTitle.isEmpty()) {
                result = d->mData[index.row()].mTitle;
            } else if (d->mData[index.row()].mTrackUrl.isValid()) {
                if (d->mData[index.row()].mTrackUrl.isLocalFile()) {
                    auto localFile = QFileInfo(d->mData[index.row()].mTrackUrl.toLocalFile());
                    result = localFile.fileName();
                } else {
                    result = d->mData[index.row()].mTrackUrl.toString();
                }
            }
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

        if (!d->mCurrentTrack.isValid()) {
            resetCurrentTrack();
        }

        break;
    }
    default:
        modelModified = false;
    }

    return modelModified;
}

QHash<int, QByteArray> MediaPlayList::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

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

    if (!d->mCurrentTrack.isValid()) {
        d->mCurrentTrack = index(d->mCurrentPlayListPosition, 0);

        if (d->mCurrentTrack.isValid()) {
            notifyCurrentTrackChanged();
        }

        if (!d->mCurrentTrack.isValid()) {
            Q_EMIT playListFinished();
            resetCurrentTrack();
            if (!d->mCurrentTrack.isValid()) {
                notifyCurrentTrackChanged();
            }
        }
    }

    if (!d->mCurrentTrack.isValid() && rowCount(parent) <= row) {
        resetCurrentTrack();
    }

    Q_EMIT tracksCountChanged();

    if (hadAlbumHeader != rowHasHeader(row)) {
        Q_EMIT dataChanged(index(row, 0), index(row, 0), {ColumnsRoles::HasAlbumHeader});

        if (!d->mCurrentTrack.isValid()) {
            resetCurrentTrack();
        }
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

    restorePlayListPosition();
    if (!d->mCurrentTrack.isValid()) {
        resetCurrentTrack();
    }

    Q_EMIT tracksCountChanged();
    Q_EMIT persistentStateChanged();

    if (!newEntry.mIsValid) {
        if (newEntry.mTrackUrl.isValid()) {
            qDebug() << "MediaPlayList::enqueue" << "newTrackByFileNameInList" << newEntry.mTrackUrl;
            if (newEntry.mTrackUrl.isLocalFile()) {
                QFileInfo newTrackFile(newEntry.mTrackUrl.toLocalFile());
                if (newTrackFile.exists()) {
                    d->mData.last().mIsValid = true;
                }
                Q_EMIT newTrackByFileNameInList(newEntry.mTrackUrl);
            }
        } else {
            Q_EMIT newTrackByNameInList(newEntry.mTitle, newEntry.mArtist, newEntry.mAlbum, newEntry.mTrackNumber, newEntry.mDiscNumber);
        }
    } else {
        Q_EMIT newTrackByIdInList(newEntry.mId);
    }

    Q_EMIT trackHasBeenAdded(data(index(d->mData.size() - 1, 0), ColumnsRoles::TitleRole).toString(), data(index(d->mData.size() - 1, 0), ColumnsRoles::ImageRole).toUrl());

    if (!newEntry.mIsValid) {
        Q_EMIT dataChanged(index(rowCount() - 1, 0), index(rowCount() - 1, 0), {MediaPlayList::HasAlbumHeader});

        if (!d->mCurrentTrack.isValid()) {
            resetCurrentTrack();
        }
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

            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            }
        }
    } else {
        if (firstMovedTrackHasHeader != rowHasHeader(destinationChild)) {
            Q_EMIT dataChanged(index(destinationChild, 0), index(destinationChild, 0), {ColumnsRoles::HasAlbumHeader});

            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            }
        }
    }

    if (sourceRow < destinationChild) {
        if (nextTrackHasHeader != rowHasHeader(sourceRow)) {
            Q_EMIT dataChanged(index(sourceRow, 0), index(sourceRow, 0), {ColumnsRoles::HasAlbumHeader});

            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            }
        }
    } else {
        if (nextTrackHasHeader != rowHasHeader(sourceRow + count)) {
            Q_EMIT dataChanged(index(sourceRow + count, 0), index(sourceRow + count, 0), {ColumnsRoles::HasAlbumHeader});

            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            }
        }
    }

    if (sourceRow < destinationChild) {
        if (futureNextTrackHasHeader != rowHasHeader(destinationChild + count - 1)) {
            Q_EMIT dataChanged(index(destinationChild + count - 1, 0), index(destinationChild + count - 1, 0), {ColumnsRoles::HasAlbumHeader});

            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            }
        }
    } else {
        if (futureNextTrackHasHeader != rowHasHeader(destinationChild + count)) {
            Q_EMIT dataChanged(index(destinationChild + count, 0), index(destinationChild + count, 0), {ColumnsRoles::HasAlbumHeader});

            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            }
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

void MediaPlayList::enqueue(const MusicArtist &artist)
{
    enqueue(artist.name());
}

void MediaPlayList::enqueue(const QString &artistName)
{
    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size());
    d->mData.push_back(MediaPlayListEntry{artistName});
    d->mTrackData.push_back({});
    endInsertRows();

    restorePlayListPosition();
    if (!d->mCurrentTrack.isValid()) {
        resetCurrentTrack();
    }

    Q_EMIT tracksCountChanged();
    Q_EMIT newArtistInList(artistName);
    Q_EMIT persistentStateChanged();
}

void MediaPlayList::enqueue(const QUrl &fileName)
{
    qDebug() << "MediaPlayList::enqueue" << fileName;
    enqueue(MediaPlayListEntry(fileName));
}

void MediaPlayList::enqueue(const QStringList &files)
{
    qDebug() << "MediaPlayList::enqueue" << files;
    for (const auto &oneFileName : files) {
        enqueue(QUrl::fromLocalFile(oneFileName));
    }
}

void MediaPlayList::clearAndEnqueue(const MusicAlbum &album)
{
    clearPlayList();
    enqueue(album);
}

void MediaPlayList::clearAndEnqueue(const MusicArtist &artist)
{
    clearAndEnqueue(artist.name());
}

void MediaPlayList::clearAndEnqueue(const QString &artistName)
{
    clearPlayList();
    enqueue(artistName);
}

void MediaPlayList::clearAndEnqueue(const QUrl &fileName)
{
    clearPlayList();
    enqueue(fileName);
}

void MediaPlayList::clearPlayList()
{
    if (d->mData.isEmpty()) {
        return;
    }

    beginRemoveRows({}, 0, d->mData.count());
    d->mData.clear();
    d->mTrackData.clear();
    endRemoveRows();

    Q_EMIT tracksCountChanged();
}

void MediaPlayList::loadPlaylist(const QUrl &fileName)
{
    d->mLoadPlaylist.clear();
    d->mLoadPlaylist.load(fileName, "m3u");
}

bool MediaPlayList::savePlaylist(const QUrl &fileName)
{
    QMediaPlaylist savePlaylist;

    for (int i = 0; i < d->mData.size(); ++i) {
        const auto &oneTrack = d->mData.at(i);
        const auto &oneTrackData = d->mTrackData.at(i);
        if (oneTrack.mIsValid) {
            savePlaylist.addMedia(oneTrackData.resourceURI());
        }
    }

    return savePlaylist.save(fileName, "m3u");
}

QVariantMap MediaPlayList::persistentState() const
{
    auto currentState = QVariantMap();
    auto result = QList<QVariant>();

    for (int trackIndex = 0; trackIndex < d->mData.size(); ++trackIndex) {
        auto oneData = QList<QString>();
        const auto &oneEntry = d->mData[trackIndex];
        if (oneEntry.mIsValid) {
            const auto &oneTrack = d->mTrackData[trackIndex];

            oneData.push_back(oneTrack.title());
            oneData.push_back(oneTrack.artist());
            oneData.push_back(oneTrack.albumName());
            oneData.push_back(QString::number(oneTrack.trackNumber()));
            oneData.push_back(QString::number(oneTrack.discNumber()));

            result.push_back(QVariant(oneData));
        }
    }

    currentState[QStringLiteral("playList")] = result;
    currentState[QStringLiteral("currentTrack")] = d->mCurrentPlayListPosition;
    currentState[QStringLiteral("randomPlay")] = d->mRandomPlay;
    currentState[QStringLiteral("repeatPlay")] = d->mRepeatPlay;

    return currentState;
}

MusicListenersManager *MediaPlayList::musicListenersManager() const
{
    return d->mMusicListenersManager;
}

int MediaPlayList::tracksCount() const
{
    return rowCount();
}

QPersistentModelIndex MediaPlayList::currentTrack() const
{
    return d->mCurrentTrack;
}

int MediaPlayList::currentTrackRow() const
{
    return d->mCurrentTrack.row();
}

bool MediaPlayList::randomPlay() const
{
    return d->mRandomPlay;
}

bool MediaPlayList::repeatPlay() const
{
    return d->mRepeatPlay;
}

void MediaPlayList::setPersistentState(const QVariantMap &persistentStateValue)
{
    if (d->mPersistentState == persistentStateValue) {
        return;
    }

    qDebug() << "MediaPlayList::setPersistentState" << persistentStateValue;

    d->mPersistentState = persistentStateValue;

    auto persistentState = d->mPersistentState[QStringLiteral("playList")].toList();

    for (auto &oneData : persistentState) {
        auto trackData = oneData.toStringList();
        if (trackData.size() != 5) {
            continue;
        }

        auto restoredTitle = trackData[0];
        auto restoredArtist = trackData[1];
        auto restoredAlbum = trackData[2];
        auto restoredTrackNumber = trackData[3].toInt();
        auto restoredDiscNumber = trackData[4].toInt();

        enqueue({restoredTitle, restoredArtist, restoredAlbum, restoredTrackNumber, restoredDiscNumber});
    }

    restorePlayListPosition();
    restoreRandomPlay();
    restoreRepeatPlay();

    Q_EMIT persistentStateChanged();
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

        if (!d->mCurrentTrack.isValid()) {
            resetCurrentTrack();
        }

        if (tracks.size() > 1) {
            beginInsertRows(QModelIndex(), playListIndex + 1, playListIndex - 1 + tracks.size());
            for (int trackIndex = 1; trackIndex < tracks.size(); ++trackIndex) {
                d->mData.push_back(MediaPlayListEntry{tracks[trackIndex].databaseId()});
                d->mTrackData.push_back(tracks[trackIndex]);
            }
            endInsertRows();

            restorePlayListPosition();
            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            }

            Q_EMIT tracksCountChanged();
        }

        Q_EMIT persistentStateChanged();
    }
}

void MediaPlayList::trackChanged(const MusicAudioTrack &track)
{
    for (int i = 0; i < d->mData.size(); ++i) {
        auto &oneEntry = d->mData[i];

        if (!oneEntry.mIsArtist && oneEntry.mIsValid) {
            if (oneEntry.mTrackUrl.isValid() && track.resourceURI() != oneEntry.mTrackUrl) {
                continue;
            }

            if (!oneEntry.mTrackUrl.isValid() && (oneEntry.mId == 0 || track.databaseId() != oneEntry.mId)) {
                continue;
            }

            if (d->mTrackData[i] != track) {
                d->mTrackData[i] = track;

                Q_EMIT dataChanged(index(i, 0), index(i, 0), {});

                if (!d->mCurrentTrack.isValid()) {
                    resetCurrentTrack();
                }
            }
            continue;
        } else if (!oneEntry.mIsArtist && !oneEntry.mIsValid && !oneEntry.mTrackUrl.isValid()) {
            if (track.title() != oneEntry.mTitle) {
                continue;
            }

            if (track.albumName() != oneEntry.mAlbum) {
                continue;
            }

            if (track.trackNumber() != oneEntry.mTrackNumber) {
                continue;
            }

            if (track.discNumber() != oneEntry.mDiscNumber) {
                continue;
            }

            d->mTrackData[i] = track;
            oneEntry.mId = track.databaseId();
            oneEntry.mIsValid = true;

            Q_EMIT dataChanged(index(i, 0), index(i, 0), {});

            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            }

            break;
        } else if (!oneEntry.mIsArtist && !oneEntry.mIsValid && oneEntry.mTrackUrl.isValid()) {
            qDebug() << "MediaPlayList::trackChanged" << oneEntry << track;
            qDebug() << "MediaPlayList::trackChanged" << track.resourceURI() << oneEntry.mTrackUrl;
            if (track.resourceURI() != oneEntry.mTrackUrl) {
                continue;
            }

            d->mTrackData[i] = track;
            oneEntry.mId = track.databaseId();
            oneEntry.mIsValid = true;

            Q_EMIT dataChanged(index(i, 0), index(i, 0), {});

            restorePlayListPosition();

            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            }

            break;
        }
    }
}

void MediaPlayList::trackRemoved(qulonglong trackId)
{
    for (int i = 0; i < d->mData.size(); ++i) {
        auto &oneEntry = d->mData[i];

        if (oneEntry.mIsValid) {
            if (oneEntry.mId == trackId) {
                oneEntry.mIsValid = false;
                oneEntry.mTitle = d->mTrackData[i].title();
                oneEntry.mArtist = d->mTrackData[i].artist();
                oneEntry.mAlbum = d->mTrackData[i].albumName();
                oneEntry.mTrackNumber = d->mTrackData[i].trackNumber();
                oneEntry.mDiscNumber = d->mTrackData[i].discNumber();

                Q_EMIT dataChanged(index(i, 0), index(i, 0), {});

                if (!d->mCurrentTrack.isValid()) {
                    resetCurrentTrack();
                }
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

void MediaPlayList::setRandomPlay(bool value)
{
    d->mRandomPlay = value;
    Q_EMIT randomPlayChanged();
}

void MediaPlayList::setRepeatPlay(bool value)
{
    d->mRepeatPlay = value;
    Q_EMIT repeatPlayChanged();
}

void MediaPlayList::skipNextTrack()
{
    if (!d->mCurrentTrack.isValid()) {
        return;
    }

    if (!d->mRandomPlay && (d->mCurrentTrack.row() >= (rowCount() - 1))) {
        if (!d->mRepeatPlay) {
            Q_EMIT playListFinished();
        }

        if (rowCount() == 1) {
            d->mCurrentTrack = QPersistentModelIndex{};
            notifyCurrentTrackChanged();
        }

        resetCurrentTrack();

        return;
    }

    if (d->mRandomPlay) {
        int randomValue = qrand();
        randomValue = randomValue % (rowCount());
        d->mCurrentTrack = index(randomValue, 0);
    } else {
        d->mCurrentTrack = index(d->mCurrentTrack.row() + 1, 0);
    }

    notifyCurrentTrackChanged();
}

void MediaPlayList::skipPreviousTrack()
{
    if (!d->mCurrentTrack.isValid()) {
        return;
    }

    if (!d->mRandomPlay && !d->mRepeatPlay && d->mCurrentTrack.row() <= 0) {
        return;
    }

    if (d->mRandomPlay) {
        int randomValue = qrand();
        randomValue = randomValue % (rowCount());
        d->mCurrentTrack = index(randomValue, 0);
    } else {
        if (d->mRepeatPlay) {
            if (d->mCurrentTrack.row() == 0) {
                d->mCurrentTrack = index(rowCount() - 1, 0);
            } else {
                d->mCurrentTrack = index(d->mCurrentTrack.row() - 1, 0);
            }
        } else {
            d->mCurrentTrack = index(d->mCurrentTrack.row() - 1, d->mCurrentTrack.column(), d->mCurrentTrack.parent());
        }
    }
    notifyCurrentTrackChanged();
}

void MediaPlayList::seedRandomGenerator(uint seed)
{
    qsrand(seed);
}

void MediaPlayList::switchTo(int row)
{
    if (!d->mCurrentTrack.isValid()) {
        return;
    }

    d->mCurrentTrack = index(row, 0);

    notifyCurrentTrackChanged();
}

void MediaPlayList::trackInError(QUrl sourceInError, QMediaPlayer::Error playerError)
{
    Q_UNUSED(playerError)

    for (int i = 0; i < d->mData.size(); ++i) {
        auto &oneTrack = d->mData[i];
        if (oneTrack.mIsValid) {
            const auto &oneTrackData = d->mTrackData.at(i);

            if (oneTrackData.resourceURI() == sourceInError) {
                oneTrack.mIsValid = false;
                Q_EMIT dataChanged(index(i, 0), index(i, 0), {ColumnsRoles::IsValidRole});
            }
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

void MediaPlayList::loadPlayListLoaded()
{
    clearPlayList();

    for (int i = 0; i < d->mLoadPlaylist.mediaCount(); ++i) {
        enqueue(d->mLoadPlaylist.media(i).canonicalUrl());
    }

    restorePlayListPosition();
    restoreRandomPlay();
    restoreRepeatPlay();

    Q_EMIT persistentStateChanged();

    d->mLoadPlaylist.clear();
    Q_EMIT playListLoaded();
}

void MediaPlayList::loadPlayListLoadFailed()
{
    d->mLoadPlaylist.clear();
    Q_EMIT playListLoadFailed();
}

void MediaPlayList::resetCurrentTrack()
{
    for(int row = 0; row < rowCount(); ++row) {
        auto candidateTrack = index(row, 0);

        if (candidateTrack.isValid() && candidateTrack.data(ColumnsRoles::IsValidRole).toBool()) {
            d->mCurrentTrack = candidateTrack;
            notifyCurrentTrackChanged();
            break;
        }
    }
}

void MediaPlayList::notifyCurrentTrackChanged()
{
    Q_EMIT currentTrackChanged();
    Q_EMIT currentTrackRowChanged();
    bool currentTrackIsValid = d->mCurrentTrack.isValid();
    if (currentTrackIsValid) {
        d->mCurrentPlayListPosition = d->mCurrentTrack.row();
    }
}

void MediaPlayList::restorePlayListPosition()
{
    auto playerCurrentTrack = d->mPersistentState.find(QStringLiteral("currentTrack"));
    if (playerCurrentTrack != d->mPersistentState.end()) {
        auto newIndex = index(playerCurrentTrack->toInt(), 0);
        if (newIndex.isValid() && (newIndex != d->mCurrentTrack)) {
            d->mCurrentTrack = newIndex;
            notifyCurrentTrackChanged();

            if (d->mCurrentTrack.isValid()) {
                d->mPersistentState.erase(playerCurrentTrack);
            }
        }
    }
}

void MediaPlayList::restoreRandomPlay()
{
    auto randomPlayStoredValue = d->mPersistentState.find(QStringLiteral("randomPlay"));
    if (randomPlayStoredValue != d->mPersistentState.end()) {
        setRandomPlay(randomPlayStoredValue->toBool());
        d->mPersistentState.erase(randomPlayStoredValue);
    }
}

void MediaPlayList::restoreRepeatPlay()
{
    auto repeatPlayStoredValue = d->mPersistentState.find(QStringLiteral("repeatPlay"));
    if (repeatPlayStoredValue != d->mPersistentState.end()) {
        setRepeatPlay(repeatPlayStoredValue->toBool());
        d->mPersistentState.erase(repeatPlayStoredValue);
    }
}

QDebug operator<<(QDebug stream, const MediaPlayListEntry &data)
{
    stream << data.mTitle << data.mAlbum << data.mArtist << data.mTrackUrl << data.mTrackNumber << data.mDiscNumber << data.mId << data.mIsValid;
    return stream;
}

#include "moc_mediaplaylist.cpp"
