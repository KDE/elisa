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

    if (role < ColumnsRoles::IsValidRole || role > ColumnsRoles::IsPlayingRole) {
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

    if (role < ColumnsRoles::IsValidRole || role > ColumnsRoles::IsPlayingRole) {
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

    return roles;
}

bool MediaPlayList::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = row, cpt = 0; cpt < count; ++i, ++cpt) {
        d->mData.removeAt(i);
    }
    endRemoveRows();

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
}

void MediaPlayList::move(int from, int to, int n)
{
    QModelIndex moveParent;
    beginResetModel();
    for (auto cptItem = 0; cptItem < n; ++cptItem) {
        d->mData.move(from, to);
    }
    endResetModel();

    emit persistentStateChanged();
}

void MediaPlayList::enqueue(QString albumName, QString artistName)
{
    if (!d->mMusicDatabase) {
        return;
    }

    auto currentAlbum = d->mMusicDatabase->albumFromTitleAndAuthor(albumName, artistName);

    for (auto oneTrackId : currentAlbum.tracksKeys()) {
        enqueue(oneTrackId);
    }
}

void MediaPlayList::clearAndEnqueue(QString albumName, QString artistName)
{
    if (!d->mMusicDatabase) {
        return;
    }

    clearPlayList();

    auto currentAlbum = d->mMusicDatabase->albumFromTitleAndAuthor(albumName, artistName);

    for (auto oneTrackId : currentAlbum.tracksKeys()) {
        enqueue(oneTrackId);
    }
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

    for (auto oneEntry : d->mData) {
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

    for (auto oneData : persistentState) {
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


#include "moc_mediaplaylist.cpp"
