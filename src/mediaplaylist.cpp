/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
    roles[static_cast<int>(ColumnsRoles::MetadataModifiableRole)] = "metadataModifiableRole";

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
        case ColumnsRoles::ElementTypeRole:
            result = QVariant::fromValue(d->mData[index.row()].mEntryType);
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
        case ColumnsRoles::MetadataModifiableRole:
            switch (d->mData[index.row()].mEntryType)
            {
            case ElisaUtils::Album:
            case ElisaUtils::Artist:
            case ElisaUtils::Composer:
            case ElisaUtils::Genre:
            case ElisaUtils::Unknown:
            case ElisaUtils::Lyricist:
            case ElisaUtils::Container:
                result = false;
                break;
            case ElisaUtils::Radio:
                result = true;
                break;
            case ElisaUtils::FileName:
            case ElisaUtils::Track:
                result = d->mTrackData[index.row()].resourceURI().isLocalFile();
                break;
            }
            break;
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

        break;
    }
    case ColumnsRoles::TitleRole:
    {
        modelModified = true;
        d->mData[index.row()].mTitle = value;
        d->mTrackData[index.row()][static_cast<TrackDataType::key_type>(role)] = value;
        Q_EMIT dataChanged(index, index, {role});

        break;
    }
    case ColumnsRoles::ArtistRole:
    {
        modelModified = true;
        d->mData[index.row()].mArtist = value;
        d->mTrackData[index.row()][static_cast<TrackDataType::key_type>(role)] = value;
        Q_EMIT dataChanged(index, index, {role});

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

    return true;
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

    return true;
}

void MediaPlayList::enqueueRestoredEntries(const QVariantList &newEntries)
{
    if (newEntries.isEmpty()) {
        return;
    }

    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size() + newEntries.size() - 1);
    for (auto &oneData : newEntries) {
        auto trackData = oneData.toStringList();
        if (trackData.size() != 7 && trackData.size() != 8) {
            continue;
        }

        auto restoredId = trackData[0].toULongLong();
        auto restoredTitle = trackData[1];
        auto restoredArtist = trackData[2];
        auto restoredAlbum = trackData[3];
        auto restoredTrackNumber = trackData[4];
        auto restoredDiscNumber = trackData[5];
        auto restoredFileUrl = QVariant{};
        if (trackData.size() == 8) {
            restoredFileUrl = QUrl{trackData[7]};
        }

        auto mEntryType = static_cast<ElisaUtils::PlayListEntryType>(trackData[6].toInt());
        auto newEntry = MediaPlayListEntry({restoredId, restoredTitle, restoredArtist, restoredAlbum, restoredFileUrl, restoredTrackNumber, restoredDiscNumber, mEntryType});

        d->mData.push_back(newEntry);
        d->mTrackData.push_back({});

        if (newEntry.mEntryType == ElisaUtils::Radio) {
            Q_EMIT newEntryInList(newEntry.mId, {}, ElisaUtils::Radio);
        } else if (newEntry.mTitle.toString().isEmpty() && newEntry.mTrackUrl.isValid()) {
            auto entryURL = newEntry.mTrackUrl.toUrl();
            if (entryURL.isLocalFile()) {
                auto entryString =  entryURL.toLocalFile();
                QFileInfo newTrackFile(entryString);
                if (newTrackFile.exists()) {
                    d->mData.last().mIsValid = true;
                }
                Q_EMIT newEntryInList(0, entryString, ElisaUtils::FileName);
            } else {
                d->mData.last().mIsValid = true;
            }
        } else {
            Q_EMIT newTrackByNameInList(newEntry.mTitle,
                                        newEntry.mArtist,
                                        newEntry.mAlbum,
                                        newEntry.mTrackNumber,
                                        newEntry.mDiscNumber);
        }
    }
    endInsertRows();
}

void MediaPlayList::enqueueFilesList(const DataTypes::EntryDataList &newEntries)
{
    qCDebug(orgKdeElisaPlayList()) << "MediaPlayList::enqueueFilesList";

    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size() + newEntries.size() - 1);
    for (const auto &oneTrackUrl : newEntries) {
        const auto &trackUrl = std::get<2>(oneTrackUrl);
        auto newEntry = MediaPlayListEntry(trackUrl);
        newEntry.mEntryType = ElisaUtils::FileName;
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
    }
    endInsertRows();
}

void MediaPlayList::enqueueOneEntry(const DataTypes::EntryData &entryData)
{
    if (!std::get<0>(entryData).isEmpty() || !std::get<1>(entryData).isEmpty() || !std::get<2>(entryData).isEmpty()) {
        beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size());
        if (!std::get<0>(entryData).databaseId() && std::get<2>(entryData).isValid()) {
            auto newEntry = MediaPlayListEntry{std::get<2>(entryData)};
            newEntry.mEntryType = ElisaUtils::FileName;
            d->mData.push_back(std::move(newEntry));
            d->mTrackData.push_back({});
        } else {
            qCDebug(orgKdeElisaPlayList()) << "MediaPlayList::enqueueOneEntry" << std::get<0>(entryData) << std::get<1>(entryData) << std::get<0>(entryData).elementType();

            d->mData.push_back(MediaPlayListEntry{std::get<0>(entryData).databaseId(), std::get<1>(entryData), std::get<0>(entryData).elementType()});
            const auto &data = std::get<0>(entryData);
            switch (data.elementType())
            {
            case ElisaUtils::Track:
            case ElisaUtils::Radio:
                d->mTrackData.push_back(static_cast<const DataTypes::TrackDataType&>(data));
                break;
            default:
                d->mTrackData.push_back({});
            }
        }
        if (std::get<2>(entryData).isValid()) {
            Q_EMIT newUrlInList(std::get<2>(entryData), std::get<0>(entryData).elementType());
        } else {
            Q_EMIT newEntryInList(std::get<0>(entryData).databaseId(), std::get<1>(entryData), std::get<0>(entryData).elementType());
        }
        endInsertRows();
    }
}

void MediaPlayList::enqueueMultipleEntries(const DataTypes::EntryDataList &entriesData)
{
    qCDebug(orgKdeElisaPlayList()) << "MediaPlayList::enqueueMultipleEntries" << entriesData.size();

    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size() + entriesData.size() - 1);
    for (const auto &entryData : entriesData) {
        qCDebug(orgKdeElisaPlayList()) << "MediaPlayList::enqueueMultipleEntries" << std::get<0>(entryData);
        auto trackUrl = std::get<0>(entryData)[DataTypes::ResourceRole].toUrl();
        if (!std::get<0>(entryData).databaseId() && trackUrl.isValid()) {
            auto newEntry = MediaPlayListEntry{trackUrl};
            newEntry.mEntryType = ElisaUtils::FileName;
            d->mData.push_back(std::move(newEntry));
            d->mTrackData.push_back({});
        } else {
            d->mData.push_back(MediaPlayListEntry{std::get<0>(entryData).databaseId(), std::get<1>(entryData), std::get<0>(entryData).elementType()});
            const auto &data = std::get<0>(entryData);
            switch (data.elementType())
            {
            case ElisaUtils::Track:
            case ElisaUtils::Radio:
            case ElisaUtils::FileName:
                d->mTrackData.push_back(static_cast<const DataTypes::TrackDataType&>(data));
                break;
            default:
                d->mTrackData.push_back({});
            }
        }

        if (trackUrl.isValid()) {
            qCDebug(orgKdeElisaPlayList()) << "MediaPlayList::enqueueMultipleEntries" << "new url" << trackUrl
                                           << std::get<0>(entryData).elementType();
            Q_EMIT newUrlInList(trackUrl, std::get<0>(entryData).elementType());
        } else {
            Q_EMIT newEntryInList(std::get<0>(entryData).databaseId(), std::get<1>(entryData), std::get<0>(entryData).elementType());
        }
    }
    endInsertRows();
}

void MediaPlayList::clearPlayList()
{
    if (d->mData.isEmpty()) {
        return;
    }

    beginRemoveRows({}, 0, d->mData.count() - 1);
    d->mData.clear();
    d->mTrackData.clear();
    endRemoveRows();
}

QVariantList MediaPlayList::getEntriesForRestore() const
{
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
            oneData.push_back(oneTrack.resourceURI().toString());

            result.push_back(QVariant(oneData));
        }
    }
    return result;
}

void MediaPlayList::tracksListAdded(qulonglong newDatabaseId,
                                    const QString &entryTitle,
                                    ElisaUtils::PlayListEntryType databaseIdType,
                                    const ListTrackDataType &tracks)
{
    if (tracks.isEmpty()) {
        qCDebug(orgKdeElisaPlayList()) << "MediaPlayList::tracksListAdded" << " empty tracks list";
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

        beginRemoveRows(QModelIndex(),playListIndex,playListIndex);
        d->mData.removeAt(playListIndex);
        d->mTrackData.removeAt(playListIndex);
        endRemoveRows();

        beginInsertRows(QModelIndex(), playListIndex, playListIndex - 1 + tracks.size());
        for (int trackIndex = 0; trackIndex < tracks.size(); ++trackIndex) {
            auto newEntry = MediaPlayListEntry{tracks[trackIndex]};
            newEntry.mEntryType = ElisaUtils::Track;
            d->mData.insert(playListIndex + trackIndex, newEntry);
            d->mTrackData.insert(playListIndex + trackIndex, tracks[trackIndex]);
        }
        endInsertRows();
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
            continue;
        } else if (oneEntry.mEntryType == ElisaUtils::Radio ) {
            if (track.databaseId() != oneEntry.mId) {
                continue;
            }

            d->mTrackData[i] = track;
            oneEntry.mId = track.databaseId();
            oneEntry.mIsValid = true;

            Q_EMIT dataChanged(index(i, 0), index(i, 0), {});

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

            break;
        } else if (oneEntry.mEntryType != ElisaUtils::Artist && !oneEntry.mIsValid && oneEntry.mTrackUrl.isValid()) {
            if (track.resourceURI() != oneEntry.mTrackUrl) {
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

            }
        }
    }
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

QDebug operator<<(const QDebug &stream, const MediaPlayListEntry &data)
{
    stream << data.mTitle << data.mAlbum << data.mArtist << data.mTrackUrl << data.mTrackNumber << data.mDiscNumber << data.mId << data.mIsValid;
    return stream;
}

#include "moc_mediaplaylist.cpp"
