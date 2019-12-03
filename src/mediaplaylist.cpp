/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "mediaplaylist.h"

#include "playListLogging.h"
#include "datatypes.h"

#include <QUrl>
#include <QPersistentModelIndex>
#include <QList>
#include <QMediaPlaylist>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QRandomGenerator>

#include <algorithm>

class MediaPlayListPrivate
{
public:

    QList<MediaPlayListEntry> mData;

    QList<DataTypes::TrackDataType> mTrackData;

    QPersistentModelIndex mPreviousTrack;

    QPersistentModelIndex mCurrentTrack;

    QPersistentModelIndex mNextTrack;

    QMediaPlaylist mLoadPlaylist;

    int mCurrentPlayListPosition = 0;

    bool mRandomPlay = false;

    bool mRepeatPlay = false;

    bool mForceUndo = false;

    QList<int> mRandomPositions = {0, 0, 0};

};

MediaPlayList::MediaPlayList(QObject *parent) : QAbstractListModel(parent), d(new MediaPlayListPrivate), dOld(new MediaPlayListPrivate)
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

QHash<int, QByteArray> MediaPlayList::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    roles[static_cast<int>(ColumnsRoles::IsValidRole)] = "isValid";
    roles[static_cast<int>(ColumnsRoles::DatabaseIdRole)] = "databaseId";
    roles[static_cast<int>(ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(ColumnsRoles::StringDurationRole)] = "duration";
    roles[static_cast<int>(ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ColumnsRoles::AlbumArtistRole)] = "albumArtist";
    roles[static_cast<int>(ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(ColumnsRoles::DiscNumberRole)] = "discNumber";
    roles[static_cast<int>(ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ColumnsRoles::GenreRole)] = "genre";
    roles[static_cast<int>(ColumnsRoles::LyricistRole)] = "lyricist";
    roles[static_cast<int>(ColumnsRoles::ComposerRole)] = "composer";
    roles[static_cast<int>(ColumnsRoles::CommentRole)] = "comment";
    roles[static_cast<int>(ColumnsRoles::YearRole)] = "year";
    roles[static_cast<int>(ColumnsRoles::ChannelsRole)] = "channels";
    roles[static_cast<int>(ColumnsRoles::BitRateRole)] = "bitRate";
    roles[static_cast<int>(ColumnsRoles::SampleRateRole)] = "sampleRate";
    roles[static_cast<int>(ColumnsRoles::CountRole)] = "count";
    roles[static_cast<int>(ColumnsRoles::IsPlayingRole)] = "isPlaying";
    roles[static_cast<int>(ColumnsRoles::IsSingleDiscAlbumRole)] = "isSingleDiscAlbum";
    roles[static_cast<int>(ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(ColumnsRoles::ShadowForImageRole)] = "shadowForImage";
    roles[static_cast<int>(ColumnsRoles::ResourceRole)] = "trackResource";
    roles[static_cast<int>(ColumnsRoles::FullDataRole)] = "trackData";
    roles[static_cast<int>(ColumnsRoles::AlbumIdRole)] = "albumId";
    roles[static_cast<int>(ColumnsRoles::AlbumSectionRole)] = "albumSection";
    roles[static_cast<int>(ColumnsRoles::ElementTypeRole)] = "entryType";

    return roles;
}

QVariant MediaPlayList::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    if (!index.isValid()) {
        return result;
    }

    if (d->mData[index.row()].mIsValid) {
        switch(role)
        {
        case ColumnsRoles::IsValidRole:
            result = d->mData[index.row()].mIsValid;
            break;
        case ColumnsRoles::IsPlayingRole:
            result = d->mData[index.row()].mIsPlaying;
            break;
        case ColumnsRoles::StringDurationRole:
        {
            QTime trackDuration = d->mTrackData[index.row()][TrackDataType::key_type::DurationRole].toTime();
            if (trackDuration.hour() == 0) {
                result = trackDuration.toString(QStringLiteral("mm:ss"));
            } else {
                result = trackDuration.toString();
            }
            break;
        }
        case ColumnsRoles::AlbumSectionRole:
            result = QJsonDocument{QJsonArray{d->mTrackData[index.row()][TrackDataType::key_type::AlbumRole].toString(),
                    d->mTrackData[index.row()][TrackDataType::key_type::AlbumArtistRole].toString(),
                    d->mTrackData[index.row()][TrackDataType::key_type::ImageUrlRole].toUrl().toString()}}.toJson();
            break;
        case ColumnsRoles::TitleRole:
        {
            const auto &trackData = d->mTrackData[index.row()];
            auto titleData = trackData[TrackDataType::key_type::TitleRole];
            if (titleData.toString().isEmpty()) {
                result = trackData[TrackDataType::key_type::ResourceRole].toUrl().fileName();
            } else {
                result = titleData;
            }
            break;
        }
        default:
            const auto &trackData = d->mTrackData[index.row()];
            auto roleEnum = static_cast<TrackDataType::key_type>(role);
            auto itData = trackData.find(roleEnum);
            if (itData != trackData.end()) {
                result = itData.value();
            } else {
                result = {};
            }
        }
    } else {
        switch(role)
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
        case ColumnsRoles::IsSingleDiscAlbumRole:
            result = false;
            break;
        case Qt::DisplayRole:
            result = d->mData[index.row()].mTitle;
            break;
        case ColumnsRoles::ImageUrlRole:
            result = QUrl(QStringLiteral("image://icon/error"));
            break;
        case ColumnsRoles::ShadowForImageRole:
            result = false;
            break;
        case ColumnsRoles::AlbumSectionRole:
            result = QJsonDocument{QJsonArray{d->mData[index.row()].mAlbum.toString(),
                    d->mData[index.row()].mArtist.toString(),
                    QUrl(QStringLiteral("image://icon/error")).toString()}}.toJson();
            break;
        default:
            result = {};
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

    if (role < ColumnsRoles::IsValidRole || role > ColumnsRoles::IsPlayingRole) {
        return modelModified;
    }

    auto convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::IsPlayingRole:
    {
        modelModified = true;
        auto newState = static_cast<PlayState>(value.toInt());
        d->mData[index.row()].mIsPlaying = newState;
        Q_EMIT dataChanged(index, index, {role});

        if (!d->mCurrentTrack.isValid()) {
            resetCurrentTrack();
        }

        break;
    }
    case ColumnsRoles::TitleRole:
    {
        modelModified = true;
        d->mData[index.row()].mTitle = value;
        d->mTrackData[index.row()][static_cast<TrackDataType::key_type>(role)] = value;
        Q_EMIT dataChanged(index, index, {role});

        if (!d->mCurrentTrack.isValid()) {
            resetCurrentTrack();
        }

        break;
    }
    case ColumnsRoles::ArtistRole:
    {
        modelModified = true;
        d->mData[index.row()].mArtist = value;
        d->mTrackData[index.row()][static_cast<TrackDataType::key_type>(role)] = value;
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

bool MediaPlayList::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);

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
    if (!d->mNextTrack.isValid() || !d->mPreviousTrack.isValid()) {
        notifyPreviousAndNextTracks();
    }

    if (!d->mCurrentTrack.isValid() && rowCount(parent) <= row) {
        resetCurrentTrack();
    }

    if (d->mRandomPlay) {
        createRandomList();
    }

    Q_EMIT tracksCountChanged();
    Q_EMIT remainingTracksChanged();
    Q_EMIT persistentStateChanged();

    return false;
}

bool MediaPlayList::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    if (sourceParent != destinationParent) {
        return false;
    }

    if (!beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild)) {
        return false;
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

void MediaPlayList::enqueueRestoredEntries(const QVariantList &newEntries)
{
    enqueueCommon();
    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size() + newEntries.size() - 1);
    for (auto &oneData : newEntries) {
        auto trackData = oneData.toStringList();
        if (trackData.size() != 7) {
            continue;
        }

        auto restoredId = trackData[0].toULongLong();
        auto restoredTitle = trackData[1];
        auto restoredArtist = trackData[2];
        auto restoredAlbum = trackData[3];
        auto restoredTrackNumber = trackData[4];
        auto restoredDiscNumber = trackData[5];

        auto mEntryType = static_cast<ElisaUtils::PlayListEntryType>(trackData[6].toInt());
        auto newEntry = MediaPlayListEntry({restoredId, restoredTitle, restoredArtist, restoredAlbum, restoredTrackNumber, restoredDiscNumber, mEntryType});

        d->mData.push_back(newEntry);
        d->mTrackData.push_back({});

        if (!d->mCurrentTrack.isValid()) {
            resetCurrentTrack();
        }


        if (!newEntry.mIsValid) {
            if (newEntry.mEntryType == ElisaUtils::Radio) {
                Q_EMIT newEntryInList(newEntry.mId, {}, ElisaUtils::Radio);
            } else if (newEntry.mTrackUrl.isValid()) {
                auto entryURL = newEntry.mTrackUrl.toUrl();
                if (entryURL.isLocalFile()) {
                    auto entryString =  entryURL.toLocalFile();
                    QFileInfo newTrackFile(entryString);
                    if (newTrackFile.exists()) {
                        d->mData.last().mIsValid = true;
                    }
                    Q_EMIT newEntryInList(0, entryString, ElisaUtils::FileName);
                }
            } else {
                Q_EMIT newTrackByNameInList(newEntry.mTitle,
                                            newEntry.mArtist,
                                            newEntry.mAlbum,
                                            newEntry.mTrackNumber,
                                            newEntry.mDiscNumber);
            }
        } else {
            Q_EMIT newEntryInList(newEntry.mId, {}, ElisaUtils::Track);
        }

        if (!newEntry.mIsValid) {
            Q_EMIT dataChanged(index(rowCount() - 1, 0), index(rowCount() - 1, 0), {MediaPlayList::IsPlayingRole});

            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            }
        }
    }
    endInsertRows();

    Q_EMIT tracksCountChanged();
    Q_EMIT remainingTracksChanged();
}

void MediaPlayList::enqueueArtist(const QString &artistName)
{
    qCDebug(orgKdeElisaPlayList()) << "MediaPlayList::enqueueArtist" << artistName;
    enqueueCommon();

    auto newEntry = MediaPlayListEntry{artistName};
    newEntry.mEntryType = ElisaUtils::Artist;

    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size());
    d->mData.push_back(newEntry);
    d->mTrackData.push_back({});
    endInsertRows();

    if (!d->mCurrentTrack.isValid()) {
        resetCurrentTrack();
    }

    Q_EMIT tracksCountChanged();
    Q_EMIT remainingTracksChanged();
    Q_EMIT newEntryInList(0, artistName, newEntry.mEntryType);
    Q_EMIT persistentStateChanged();
}

void MediaPlayList::enqueueFilesList(const ElisaUtils::EntryDataList &newEntries, ElisaUtils::PlayListEntryType databaseIdType)
{
    qCDebug(orgKdeElisaPlayList()) << "MediaPlayList::enqueueFilesList";
    enqueueCommon();

    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size() + newEntries.size() - 1);
    for (const auto &oneTrackUrl : newEntries) {
        switch (databaseIdType)
        {
        case ElisaUtils::Radio:
        case ElisaUtils::Track:
        case ElisaUtils::FileName:
        {
            const auto &trackUrl = std::get<2>(oneTrackUrl);
            auto newEntry = MediaPlayListEntry(trackUrl);
            newEntry.mEntryType = databaseIdType;
            d->mData.push_back(newEntry);
            if (trackUrl.isValid()) {
                if (trackUrl.isLocalFile()) {
                    d->mTrackData.push_back({{DataTypes::ColumnsRoles::ResourceRole, trackUrl}});
                    auto entryString =  trackUrl.toLocalFile();
                    QFileInfo newTrackFile(entryString);
                    if (newTrackFile.exists()) {
                        d->mData.last().mIsValid = true;
                    }
                } else {
                    d->mTrackData.push_back({{DataTypes::ColumnsRoles::ResourceRole, trackUrl},
                                             {DataTypes::ColumnsRoles::TitleRole, trackUrl.fileName()}});
                    d->mData.last().mIsValid = true;
                }
                Q_EMIT newUrlInList(trackUrl, newEntry.mEntryType);
            } else {
                d->mTrackData.push_back({});
            }
            break;
        }
        case ElisaUtils::Album:
        case ElisaUtils::Artist:
        case ElisaUtils::Genre:
        case ElisaUtils::Lyricist:
        case ElisaUtils::Composer:
        case ElisaUtils::Unknown:
            break;
        }
    }
    endInsertRows();

    if (!d->mCurrentTrack.isValid()) {
        resetCurrentTrack();
    }

    Q_EMIT tracksCountChanged();
    Q_EMIT remainingTracksChanged();
    Q_EMIT persistentStateChanged();

    Q_EMIT dataChanged(index(rowCount() - 1, 0), index(rowCount() - 1, 0), {MediaPlayList::IsPlayingRole});
}

void MediaPlayList::enqueueTracksListById(const ElisaUtils::EntryDataList &newEntries, ElisaUtils::PlayListEntryType type)
{
    qCDebug(orgKdeElisaPlayList()) << "MediaPlayList::enqueueTracksListById" << newEntries.size() << type;
    enqueueCommon();

    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size() + newEntries.size() - 1);
    for (const auto &newTrack : newEntries) {
        auto newMediaPlayListEntry = MediaPlayListEntry{std::get<0>(newTrack).databaseId(), std::get<1>(newTrack), type};
        d->mData.push_back(newMediaPlayListEntry);
        d->mTrackData.push_back(std::get<0>(newTrack));
        Q_EMIT newEntryInList(newMediaPlayListEntry.mId, newMediaPlayListEntry.mTitle.toString(), newMediaPlayListEntry.mEntryType);
    }
    endInsertRows();

    if (!d->mCurrentTrack.isValid()) {
        resetCurrentTrack();
    }

    Q_EMIT tracksCountChanged();
    Q_EMIT remainingTracksChanged();
    Q_EMIT persistentStateChanged();

    Q_EMIT dataChanged(index(rowCount() - 1, 0), index(rowCount() - 1, 0), {MediaPlayList::IsPlayingRole});
}

void MediaPlayList::enqueueOneEntry(const ElisaUtils::EntryData &entryData, ElisaUtils::PlayListEntryType type)
{
    qCDebug(orgKdeElisaPlayList()) << "MediaPlayList::enqueueOneEntry" << std::get<0>(entryData) << std::get<1>(entryData) << type;
    enqueueCommon();

    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size());
    d->mData.push_back(MediaPlayListEntry{std::get<0>(entryData).databaseId(), std::get<1>(entryData), type});
    d->mTrackData.push_back(std::get<0>(entryData));
    Q_EMIT newEntryInList(std::get<0>(entryData).databaseId(), std::get<1>(entryData), type);
    endInsertRows();

    Q_EMIT tracksCountChanged();
    Q_EMIT remainingTracksChanged();
    Q_EMIT persistentStateChanged();
}

void MediaPlayList::enqueueMultipleEntries(const ElisaUtils::EntryDataList &entriesData, ElisaUtils::PlayListEntryType type)
{
    qCDebug(orgKdeElisaPlayList()) << "MediaPlayList::enqueueMultipleEntries" << entriesData.size() << type;
    enqueueCommon();

    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size() + entriesData.size() - 1);
    for (const auto &entryData : entriesData) {
        if (std::get<2>(entryData).isValid()) {
            d->mData.push_back(MediaPlayListEntry{std::get<2>(entryData)});
            d->mTrackData.push_back({});
        } else {
            d->mData.push_back(MediaPlayListEntry{std::get<0>(entryData).databaseId(), std::get<1>(entryData), type});
            d->mTrackData.push_back(std::get<0>(entryData));
        }
        if (std::get<2>(entryData).isValid()) {
            Q_EMIT newUrlInList(std::get<2>(entryData), type);
        } else {
            Q_EMIT newEntryInList(std::get<0>(entryData).databaseId(), std::get<1>(entryData), type);
        }
    }
    endInsertRows();

    Q_EMIT tracksCountChanged();
    Q_EMIT remainingTracksChanged();
    Q_EMIT persistentStateChanged();
}

void MediaPlayList::clearPlayList(bool prepareUndo)
{
    if (d->mData.isEmpty()) {
        return;
    }

    if(prepareUndo){
        Q_EMIT clearPlayListPlayer();
        this->copyD();
    }

    beginRemoveRows({}, 0, d->mData.count() - 1);
    d->mData.clear();
    d->mTrackData.clear();
    endRemoveRows();

    d->mCurrentPlayListPosition = 0;
    d->mCurrentTrack = QPersistentModelIndex{};
    notifyCurrentTrackChanged();

    displayOrHideUndoInline(true);
    Q_EMIT tracksCountChanged();
    Q_EMIT remainingTracksChanged();
    Q_EMIT persistentStateChanged();
}

void MediaPlayList::clearPlayList()
{
    this->clearPlayList(true);
}

void MediaPlayList::undoClearPlayList()
{
    clearPlayList(false);

    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size() + dOld->mData.size() - 1);
    for (auto &newTrack : dOld->mData) {
        d->mData.push_back(newTrack);
        d->mTrackData.push_back({});

        if (ElisaUtils::FileName == newTrack.mEntryType  && newTrack.mTrackUrl.isValid()) {
            auto entryURL = newTrack.mTrackUrl.toUrl();
            if (entryURL.isLocalFile()) {
                auto entryString =  entryURL.toLocalFile();
                QFileInfo newTrackFile(entryString);
                if (newTrackFile.exists()) {
                    d->mData.last().mIsValid = true;
                }
                Q_EMIT newEntryInList(0, entryString, newTrack.mEntryType);
            }
        }
        else if(ElisaUtils::Artist == newTrack.mEntryType){
            Q_EMIT newEntryInList(0, newTrack.mArtist.toString(), newTrack.mEntryType);
        }
        else{
            Q_EMIT newEntryInList(newTrack.mId, newTrack.mTitle.toString(), newTrack.mEntryType);
        }
    }
    endInsertRows();

    d->mCurrentPlayListPosition = dOld->mCurrentPlayListPosition;
    d->mRandomPlay = dOld->mRandomPlay;
    d->mRepeatPlay = dOld->mRepeatPlay;

    auto candidateTrack = index(dOld->mCurrentPlayListPosition, 0);

    if (candidateTrack.isValid() && candidateTrack.data(ColumnsRoles::IsValidRole).toBool()) {
        d->mCurrentTrack = candidateTrack;
        notifyCurrentTrackChanged();
    }

    Q_EMIT tracksCountChanged();
    Q_EMIT remainingTracksChanged();
    Q_EMIT persistentStateChanged();

    Q_EMIT dataChanged(index(rowCount() - 1, 0), index(rowCount() - 1, 0), {MediaPlayList::IsPlayingRole});
    displayOrHideUndoInline(false);
    Q_EMIT undoClearPlayListPlayer();
}

void MediaPlayList::enqueueCommon()
{
    displayOrHideUndoInline(false);
}

void MediaPlayList::copyD()
{
    dOld->mData = d->mData;
    dOld->mTrackData = d->mTrackData;
    dOld->mCurrentTrack = d->mCurrentTrack;
    dOld->mCurrentPlayListPosition = d->mCurrentPlayListPosition;
    dOld->mRandomPlay = d->mRandomPlay;
    dOld->mRepeatPlay = d->mRepeatPlay;
}

void MediaPlayList::loadPlaylist(const QString &localFileName)
{
    d->mLoadPlaylist.clear();
    d->mLoadPlaylist.load(QUrl::fromLocalFile(localFileName), "m3u");
}

void MediaPlayList::loadPlaylist(const QUrl &fileName)
{
    d->mLoadPlaylist.clear();
    d->mLoadPlaylist.load(fileName, "m3u");
}

void MediaPlayList::enqueue(const ElisaUtils::EntryData &newEntry, ElisaUtils::PlayListEntryType databaseIdType)
{
    enqueue(newEntry, databaseIdType, ElisaUtils::PlayListEnqueueMode::AppendPlayList, ElisaUtils::PlayListEnqueueTriggerPlay::DoNotTriggerPlay);
}

void MediaPlayList::enqueue(const ElisaUtils::EntryDataList &newEntries, ElisaUtils::PlayListEntryType databaseIdType)
{
    enqueue(newEntries, databaseIdType, ElisaUtils::PlayListEnqueueMode::AppendPlayList, ElisaUtils::PlayListEnqueueTriggerPlay::DoNotTriggerPlay);
}

void MediaPlayList::enqueue(qulonglong newEntryDatabaseId,
                            const QString &newEntryTitle,
                            ElisaUtils::PlayListEntryType databaseIdType,
                            ElisaUtils::PlayListEnqueueMode enqueueMode,
                            ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    enqueue(ElisaUtils::EntryData{{{DataTypes::DatabaseIdRole, newEntryDatabaseId}}, newEntryTitle, {}}, databaseIdType, enqueueMode, triggerPlay);
}

void MediaPlayList::enqueue(const QUrl &entryUrl, ElisaUtils::PlayListEntryType databaseIdType,
                            ElisaUtils::PlayListEnqueueMode enqueueMode,
                            ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    enqueue(ElisaUtils::EntryData{{}, {}, entryUrl}, databaseIdType, enqueueMode, triggerPlay);
}

void MediaPlayList::enqueue(const ElisaUtils::EntryData &newEntry,
                            ElisaUtils::PlayListEntryType databaseIdType,
                            ElisaUtils::PlayListEnqueueMode enqueueMode,
                            ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    if (enqueueMode == ElisaUtils::ReplacePlayList) {
        if(d->mData.size()>0){
            d->mForceUndo = true;
        }
        clearPlayList();
    }

    enqueueCommon();

    switch (databaseIdType)
    {
    case ElisaUtils::Album:
    case ElisaUtils::Artist:
    case ElisaUtils::Genre:
        enqueueOneEntry(newEntry, databaseIdType);
        break;
    case ElisaUtils::Track:
    case ElisaUtils::Radio:
        if (std::get<2>(newEntry).isValid()) {
            enqueueFilesList({newEntry}, databaseIdType);
        } else {
            enqueueOneEntry(newEntry, databaseIdType);
        }
        break;
    case ElisaUtils::FileName:
        enqueueFilesList({newEntry}, databaseIdType);
        break;
    case ElisaUtils::Lyricist:
    case ElisaUtils::Composer:
    case ElisaUtils::Unknown:
        break;
    }

    if (triggerPlay == ElisaUtils::TriggerPlay) {
        Q_EMIT ensurePlay();
    }

    if (enqueueMode == ElisaUtils::ReplacePlayList) {
        d->mForceUndo = false;
    }
}

void MediaPlayList::enqueue(const ElisaUtils::EntryDataList &newEntries,
                            ElisaUtils::PlayListEntryType databaseIdType,
                            ElisaUtils::PlayListEnqueueMode enqueueMode,
                            ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    if (newEntries.isEmpty()) {
        return;
    }

    if (enqueueMode == ElisaUtils::ReplacePlayList) {
        if(d->mData.size()>0){
            d->mForceUndo = true;
        }
        clearPlayList();
    }

    enqueueCommon();

    switch (databaseIdType)
    {
    case ElisaUtils::Track:
    case ElisaUtils::Radio:
        enqueueMultipleEntries(newEntries, databaseIdType);
        break;
    case ElisaUtils::FileName:
        enqueueFilesList(newEntries, databaseIdType);
        break;
    case ElisaUtils::Album:
    case ElisaUtils::Artist:
    case ElisaUtils::Genre:
        enqueueMultipleEntries(newEntries, databaseIdType);
        break;
    case ElisaUtils::Lyricist:
    case ElisaUtils::Composer:
    case ElisaUtils::Unknown:
        break;
    }

    if (triggerPlay == ElisaUtils::TriggerPlay) {
        Q_EMIT ensurePlay();
    }

    if (enqueueMode == ElisaUtils::ReplacePlayList) {
        d->mForceUndo = false;
    }
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
    QVariantMap currentState;
    QVariantList result;

    for (int trackIndex = 0; trackIndex < d->mData.size(); ++trackIndex) {
        QStringList oneData;
        const auto &oneEntry = d->mData[trackIndex];
        if (oneEntry.mIsValid) {
            const auto &oneTrack = d->mTrackData[trackIndex];

            oneData.push_back(QString::number(oneTrack.databaseId()));
            oneData.push_back(oneTrack.title());
            oneData.push_back(oneTrack.artist());
            if (oneTrack.hasAlbum()) {
                oneData.push_back(oneTrack.album());
            } else {
                oneData.push_back({});
            }
            if (oneTrack.hasTrackNumber()) {
                oneData.push_back(QString::number(oneTrack.trackNumber()));
            } else {
                oneData.push_back({});
            }
            if (oneTrack.hasDiscNumber()) {
                oneData.push_back(QString::number(oneTrack.discNumber()));
            } else {
                oneData.push_back({});
            }
            oneData.push_back(QString::number(oneEntry.mEntryType));

            result.push_back(QVariant(oneData));
        }
    }

    currentState[QStringLiteral("playList")] = result;
    currentState[QStringLiteral("currentTrack")] = d->mCurrentPlayListPosition;
    currentState[QStringLiteral("randomPlay")] = d->mRandomPlay;
    currentState[QStringLiteral("repeatPlay")] = d->mRepeatPlay;

    return currentState;
}

int MediaPlayList::tracksCount() const
{
    return rowCount();
}

QPersistentModelIndex MediaPlayList::previousTrack() const
{
    return d->mPreviousTrack;
}

QPersistentModelIndex MediaPlayList::currentTrack() const
{
    return d->mCurrentTrack;
}

QPersistentModelIndex MediaPlayList::nextTrack() const
{
    return d->mNextTrack;
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
    qCDebug(orgKdeElisaPlayList()) << "MediaPlayList::setPersistentState" << persistentStateValue;

    auto playListIt = persistentStateValue.find(QStringLiteral("playList"));
    if (playListIt != persistentStateValue.end()) {
        const auto playList = playListIt.value().toList();
        enqueueRestoredEntries(playList);
    }

    auto playerCurrentTrack = persistentStateValue.find(QStringLiteral("currentTrack"));
    if (playerCurrentTrack != persistentStateValue.end()) {
        auto newIndex = index(playerCurrentTrack->toInt(), 0);
        if (newIndex.isValid() && (newIndex != d->mCurrentTrack)) {
            d->mCurrentTrack = newIndex;
            notifyCurrentTrackChanged();
        }
    }

    auto randomPlayStoredValue = persistentStateValue.find(QStringLiteral("randomPlay"));
    if (randomPlayStoredValue != persistentStateValue.end()) {
        setRandomPlay(randomPlayStoredValue->toBool());
    }
    auto repeatPlayStoredValue = persistentStateValue.find(QStringLiteral("repeatPlay"));
    if (repeatPlayStoredValue != persistentStateValue.end()) {
        setRepeatPlay(repeatPlayStoredValue->toBool());
    }

    Q_EMIT persistentStateChanged();
}

void MediaPlayList::removeSelection(QList<int> selection)
{
    std::sort(selection.begin(), selection.end());
    std::reverse(selection.begin(), selection.end());
    for (auto oneItem : selection) {
        removeRow(oneItem);
    }
    if (d->mRandomPlay) {
        createRandomList();
    }
}

void MediaPlayList::tracksListAdded(qulonglong newDatabaseId,
                                    const QString &entryTitle,
                                    ElisaUtils::PlayListEntryType databaseIdType,
                                    const ListTrackDataType &tracks)
{
    if (tracks.isEmpty()) {
        qDebug() << "empty tracks list";
        return;
    }

    for (int playListIndex = 0; playListIndex < d->mData.size(); ++playListIndex) {
        auto &oneEntry = d->mData[playListIndex];
        if (oneEntry.mEntryType != databaseIdType) {
            continue;
        }

        if (oneEntry.mTitle != entryTitle) {
            continue;
        }

        if (newDatabaseId != 0 && oneEntry.mId != newDatabaseId) {
            continue;
        }

        d->mTrackData[playListIndex] = tracks.first();
        oneEntry.mId = tracks.first().databaseId();
        oneEntry.mIsValid = true;
        oneEntry.mEntryType = ElisaUtils::Track;

        Q_EMIT dataChanged(index(playListIndex, 0), index(playListIndex, 0), {});

        if (!d->mCurrentTrack.isValid()) {
            resetCurrentTrack();
        }

        if (tracks.size() > 1) {
            beginInsertRows(QModelIndex(), playListIndex + 1, playListIndex - 1 + tracks.size());
            for (int trackIndex = 1; trackIndex < tracks.size(); ++trackIndex) {
                auto newEntry = MediaPlayListEntry{tracks[trackIndex]};
                newEntry.mEntryType = ElisaUtils::Track;
                d->mData.insert(playListIndex + trackIndex, newEntry);
                d->mTrackData.insert(playListIndex + trackIndex, tracks[trackIndex]);
            }
            endInsertRows();

            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            }

            Q_EMIT tracksCountChanged();
            Q_EMIT remainingTracksChanged();
        }
        Q_EMIT persistentStateChanged();
    }
}

void MediaPlayList::trackChanged(const TrackDataType &track)
{
    qCDebug(orgKdeElisaPlayList()) << "MediaPlayList::trackChanged" << track[DataTypes::TitleRole];

    for (int i = 0; i < d->mData.size(); ++i) {
        auto &oneEntry = d->mData[i];

        if (oneEntry.mEntryType != ElisaUtils::Artist && oneEntry.mIsValid) {
            if (oneEntry.mTrackUrl.toUrl().isValid() && track.resourceURI() != oneEntry.mTrackUrl.toUrl()) {
                continue;
            }

            if (!oneEntry.mTrackUrl.toUrl().isValid() && (oneEntry.mId == 0 || track.databaseId() != oneEntry.mId)) {
                continue;
            }

            const auto &trackData = d->mTrackData[i];

            if (!trackData.empty()) {
                bool sameData = true;
                for (auto oneKeyIterator = track.constKeyValueBegin(); oneKeyIterator != track.constKeyValueEnd(); ++oneKeyIterator) {
                    if (trackData[(*oneKeyIterator).first] != (*oneKeyIterator).second) {
                        sameData = false;
                        break;
                    }
                }
                if (sameData) {
                    continue;
                }
            }

            d->mTrackData[i] = track;

            Q_EMIT dataChanged(index(i, 0), index(i, 0), {});

            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            }
            continue;
        } else if (oneEntry.mEntryType == ElisaUtils::Radio ) {
            if (track.databaseId() != oneEntry.mId) {
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
        } else if (oneEntry.mEntryType != ElisaUtils::Artist && !oneEntry.mIsValid && !oneEntry.mTrackUrl.isValid()) {
            if (track.find(TrackDataType::key_type::TitleRole) != track.end() &&
                    track.title() != oneEntry.mTitle) {
                continue;
            }

            if (track.find(TrackDataType::key_type::AlbumRole) != track.end() &&
                    track.album() != oneEntry.mAlbum) {
                continue;
            }

            if (track.find(TrackDataType::key_type::TrackNumberRole) != track.end() &&
                    track.trackNumber() != oneEntry.mTrackNumber) {
                continue;
            }

            if (track.find(TrackDataType::key_type::DiscNumberRole) != track.end() &&
                    track.discNumber() != oneEntry.mDiscNumber) {
                continue;
            }

            d->mTrackData[i] = track;
            oneEntry.mId = track.databaseId();
            oneEntry.mIsValid = true;

            Q_EMIT dataChanged(index(i, 0), index(i, 0), {});

            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            } else if (i == d->mCurrentTrack.row()) {
                notifyCurrentTrackChanged();
            } else if (i == d->mNextTrack.row() || i == d->mPreviousTrack.row()) {
                notifyPreviousAndNextTracks();
            }

            break;
        } else if (oneEntry.mEntryType != ElisaUtils::Artist && !oneEntry.mIsValid && oneEntry.mTrackUrl.isValid()) {
            if (track.resourceURI() != oneEntry.mTrackUrl) {
                continue;
            }

            d->mTrackData[i] = track;
            oneEntry.mId = track.databaseId();
            oneEntry.mIsValid = true;

            Q_EMIT dataChanged(index(i, 0), index(i, 0), {});

            if (!d->mCurrentTrack.isValid()) {
                resetCurrentTrack();
            } else if (i == d->mCurrentTrack.row()) {
                notifyCurrentTrackChanged();
            } else if (i == d->mNextTrack.row() || i == d->mPreviousTrack.row()) {
                notifyPreviousAndNextTracks();
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
                oneEntry.mAlbum = d->mTrackData[i].album();
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

void MediaPlayList::setRandomPlay(bool value)
{
    if (d->mRandomPlay != value) {
        d->mRandomPlay = value;
        createRandomList();
        Q_EMIT randomPlayChanged();
        Q_EMIT remainingTracksChanged();
        notifyPreviousAndNextTracks();
    }
}

void MediaPlayList::setRepeatPlay(bool value)
{
    if (d->mRepeatPlay != value) {
        d->mRepeatPlay = value;
        Q_EMIT repeatPlayChanged();
        Q_EMIT remainingTracksChanged();
        notifyPreviousAndNextTracks();
    }
}

void MediaPlayList::displayOrHideUndoInline(bool value)
{
    if(value){
        Q_EMIT displayUndoInline();
    }else {
        if(!d->mForceUndo){
            Q_EMIT hideUndoInline();
        }
    }
}

void MediaPlayList::skipNextTrack()
{
    if (!d->mCurrentTrack.isValid()) {
        return;
    }

    if (d->mRandomPlay) {
        d->mRandomPositions.removeFirst();
        d->mCurrentTrack = index(d->mRandomPositions.at(1), 0);
        if (rowCount()) {
            d->mRandomPositions.append(QRandomGenerator::global()->bounded(rowCount()));
        }
    } else {
        if (d->mCurrentTrack.row() >= rowCount() - 1) {
            d->mCurrentTrack = index(0, 0);
            if (!d->mRepeatPlay) {
                Q_EMIT playListFinished();
            }
        } else {
            d->mCurrentTrack = index(d->mCurrentTrack.row() + 1, 0);
        }
    }

    notifyCurrentTrackChanged();
}

void MediaPlayList::skipPreviousTrack()
{
    if (!d->mCurrentTrack.isValid()) {
        return;
    }

    if (d->mRandomPlay) {
        d->mRandomPositions.removeLast();
        d->mCurrentTrack = index(d->mRandomPositions.at(0), 0);
        if (rowCount()) {
            d->mRandomPositions.prepend(QRandomGenerator::global()->bounded(rowCount()));
        }
    } else {
        if (d->mCurrentTrack.row() == 0) {
            if (d->mRepeatPlay) {
                d->mCurrentTrack = index(rowCount() - 1, 0);
            } else {
                return;
            }
        } else {
            d->mCurrentTrack = index(d->mCurrentTrack.row() - 1, 0);
        }
    }

    notifyCurrentTrackChanged();
}

void MediaPlayList::switchTo(int row)
{
    if (!d->mCurrentTrack.isValid()) {
        return;
    }

    if (d->mRandomPlay) {
        d->mCurrentTrack = index(row, 0);
        d->mRandomPositions.replace(1, row);
    } else {
        d->mCurrentTrack = index(row, 0);
    }

    notifyCurrentTrackChanged();
}

void MediaPlayList::trackInError(const QUrl &sourceInError, QMediaPlayer::Error playerError)
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

void MediaPlayList::loadPlayListLoaded()
{
    clearPlayList();

    for (int i = 0; i < d->mLoadPlaylist.mediaCount(); ++i) {
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        enqueue(ElisaUtils::EntryData{{}, {}, d->mLoadPlaylist.media(i).canonicalUrl()}, ElisaUtils::FileName);
#else
        enqueue(ElisaUtils::EntryData{{}, {}, d->mLoadPlaylist.media(i).request().url()}, ElisaUtils::FileName);
#endif
    }

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

void MediaPlayList::notifyPreviousAndNextTracks()
{
    if (!d->mCurrentTrack.isValid()) {
        d->mPreviousTrack = QPersistentModelIndex();
        d->mNextTrack = QPersistentModelIndex();
    }
    auto mOldPreviousTrack = d->mPreviousTrack;
    auto mOldNextTrack = d->mNextTrack;
    // use random list for previous and next types
    if (d->mRandomPlay) {
        d->mPreviousTrack = index(d->mRandomPositions.first(), 0);
        d->mNextTrack = index(d->mRandomPositions.last(), 0);
    } else if (d->mRepeatPlay) {
        // forward to end or begin when repeating
        if (d->mCurrentTrack.row() == 0) {
            d->mPreviousTrack = index(rowCount() - 1, 0);
        } else {
            d->mPreviousTrack = index(d->mCurrentTrack.row() - 1, 0);
        }
        if (d->mCurrentTrack.row() == rowCount() - 1) {
            d->mNextTrack = index(0, 0);
        } else {
            d->mNextTrack = index(d->mCurrentTrack.row() + 1, 0);
        }
    } else {
        // return nothing if no tracks available
        if (d->mCurrentTrack.row() == 0) {
            d->mPreviousTrack = QPersistentModelIndex();
        } else {
            d->mPreviousTrack = index(d->mCurrentTrack.row() - 1, 0);
        }
        if (d->mCurrentTrack.row() == rowCount() - 1) {
            d->mNextTrack = QPersistentModelIndex();
        } else {
            d->mNextTrack = index(d->mCurrentTrack.row() + 1, 0);
        }
    }
    if (d->mPreviousTrack != mOldPreviousTrack) {
        Q_EMIT previousTrackChanged(d->mPreviousTrack);
    }
    if (d->mNextTrack != mOldNextTrack) {
        Q_EMIT nextTrackChanged(d->mNextTrack);
    }
}

void MediaPlayList::notifyCurrentTrackChanged()
{
    // determine previous and next tracks first
    notifyPreviousAndNextTracks();

    Q_EMIT currentTrackChanged(d->mCurrentTrack);
    Q_EMIT currentTrackRowChanged();
    Q_EMIT remainingTracksChanged();
    bool currentTrackIsValid = d->mCurrentTrack.isValid();
    if (currentTrackIsValid) {
        d->mCurrentPlayListPosition = d->mCurrentTrack.row();
    }
}

QDebug operator<<(const QDebug &stream, const MediaPlayListEntry &data)
{
    stream << data.mTitle << data.mAlbum << data.mArtist << data.mTrackUrl << data.mTrackNumber << data.mDiscNumber << data.mId << data.mIsValid;
    return stream;
}

int MediaPlayList::remainingTracks() const
{
    if (!d->mCurrentTrack.isValid()) {
        return -1;
    }

    if (d->mRandomPlay || d->mRepeatPlay) {
        return -1;
    } else {
        return rowCount() - d->mCurrentTrack.row() - 1;
    }
}

void MediaPlayList::createRandomList()
{
    if (rowCount()) {
        for (auto& position : d->mRandomPositions) {
            position = QRandomGenerator::global()->bounded(rowCount());
        }
    }
    if (d->mCurrentTrack.isValid()) {
        d->mRandomPositions.replace(1, d->mCurrentTrack.row());
    }
}

#include "moc_mediaplaylist.cpp"
