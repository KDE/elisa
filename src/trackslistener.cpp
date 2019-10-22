/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "trackslistener.h"

#include "playListLogging.h"
#include "databaseinterface.h"
#include "datatypes.h"
#include "filescanner.h"

#include <QMimeDatabase>
#include <QSet>
#include <QList>
#include <QDebug>

#include <array>
#include <algorithm>

class TracksListenerPrivate
{
public:

    QSet<qulonglong> mTracksByIdSet;

    QSet<qulonglong> mRadiosByIdSet;

    QList<std::tuple<QString, QString, QString, int, int>> mTracksByNameSet;

    QList<QUrl> mTracksByFileNameSet;

    DatabaseInterface *mDatabase = nullptr;

    FileScanner mFileScanner;

    QMimeDatabase mMimeDb;

};

TracksListener::TracksListener(DatabaseInterface *database, QObject *parent) : QObject(parent), d(std::make_unique<TracksListenerPrivate>())
{
    d->mDatabase = database;
}

TracksListener::~TracksListener()
= default;

void TracksListener::tracksAdded(const ListTrackDataType &allTracks)
{
    for (const auto &oneTrack : allTracks) {
        if (d->mTracksByIdSet.contains(oneTrack.databaseId())) {
            Q_EMIT trackHasChanged(oneTrack);
        }

        if (d->mTracksByNameSet.isEmpty()) {
            return;
        }

        for (auto itTrack = d->mTracksByNameSet.begin(); itTrack != d->mTracksByNameSet.end(); ) {
            if (!std::get<0>(*itTrack).isEmpty() && std::get<0>(*itTrack) != oneTrack.title()) {
                ++itTrack;
                continue;
            }

            if (!std::get<1>(*itTrack).isEmpty() && std::get<1>(*itTrack) != oneTrack.artist()) {
                ++itTrack;
                continue;
            }

            if (!std::get<2>(*itTrack).isEmpty() && std::get<2>(*itTrack) != oneTrack.album()) {
                ++itTrack;
                continue;
            }

            if (std::get<3>(*itTrack) != oneTrack.trackNumber()) {
                ++itTrack;
                continue;
            }

            if (std::get<4>(*itTrack) != oneTrack.discNumber()) {
                ++itTrack;
                continue;
            }

            Q_EMIT trackHasChanged(TrackDataType(oneTrack));

            d->mTracksByIdSet.insert(oneTrack.databaseId());
            itTrack = d->mTracksByNameSet.erase(itTrack);
        }
    }
}

void TracksListener::trackRemoved(qulonglong id)
{
    if (d->mTracksByIdSet.contains(id)) {
        Q_EMIT trackHasBeenRemoved(id);
    }
}

void TracksListener::trackModified(const TrackDataType &modifiedTrack)
{
    if (d->mTracksByIdSet.contains(modifiedTrack.databaseId())) {
        Q_EMIT trackHasChanged(modifiedTrack);
    }
}

void TracksListener::trackByNameInList(const QVariant &title, const QVariant &artist, const QVariant &album,
                                       const QVariant &trackNumber, const QVariant &discNumber)
{
    const auto realTitle = title.toString();
    const auto realArtist = artist.toString();
    const auto albumIsValid = !album.isNull() && album.isValid() && !album.toString().isEmpty();
    auto realAlbum = std::optional<QString>{};
    if (albumIsValid) {
        realAlbum = album.toString();
    }
    auto trackNumberIsValid = bool{};
    const auto trackNumberValue = trackNumber.toInt(&trackNumberIsValid);
    auto realTrackNumber = std::optional<int>{};
    if (trackNumberIsValid) {
        realTrackNumber = trackNumberValue;
    }
    auto discNumberIsValid = bool{};
    const auto discNumberValue = discNumber.toInt(&discNumberIsValid);
    auto realDiscNumber = std::optional<int>{};
    if (discNumberIsValid) {
        realDiscNumber = discNumberValue;
    }

    auto newTrackId = d->mDatabase->trackIdFromTitleAlbumTrackDiscNumber(realTitle, realArtist, realAlbum,
                                                                         realTrackNumber, realDiscNumber);
    if (newTrackId == 0) {
        auto newTrack = std::tuple<QString, QString, QString, int, int>(realTitle, realArtist, album.toString(), trackNumber.toInt(), discNumber.toInt());
        d->mTracksByNameSet.push_back(newTrack);

        return;
    }

    d->mTracksByIdSet.insert(newTrackId);

    auto newTrack = d->mDatabase->trackDataFromDatabaseId(newTrackId);

    if (!newTrack.isEmpty()) {
        Q_EMIT trackHasChanged(newTrack);
    }
}

void TracksListener::trackByFileNameInList(const QUrl &fileName)
{
    auto newTrackId = d->mDatabase->trackIdFromFileName(fileName);
    if (newTrackId == 0) {
        auto newTrack = scanOneFile(fileName);

        if (newTrack.isValid()) {
            auto oneData = DataTypes::TrackDataType{};

            if (!newTrack.title().isEmpty()) {
                oneData[DataTypes::TrackDataType::key_type::TitleRole] = newTrack.title();
            } else {
                const auto &fileUrl = newTrack.resourceURI();
                oneData[DataTypes::TrackDataType::key_type::TitleRole] = fileUrl.fileName();
            }

            oneData[DataTypes::TrackDataType::key_type::ArtistRole] = newTrack.artist();
            oneData[DataTypes::TrackDataType::key_type::AlbumRole] = newTrack.albumName();
            oneData[DataTypes::TrackDataType::key_type::AlbumIdRole] = newTrack.albumId();
            oneData[DataTypes::TrackDataType::key_type::TrackNumberRole] = newTrack.trackNumber();
            oneData[DataTypes::TrackDataType::key_type::DiscNumberRole] = newTrack.discNumber();
            oneData[DataTypes::TrackDataType::key_type::DurationRole] = newTrack.duration();
            oneData[DataTypes::TrackDataType::key_type::MilliSecondsDurationRole] = newTrack.duration().msecsSinceStartOfDay();
            oneData[DataTypes::TrackDataType::key_type::ResourceRole] = newTrack.resourceURI();
            oneData[DataTypes::TrackDataType::key_type::ImageUrlRole] = newTrack.albumCover();

            Q_EMIT trackHasChanged(oneData);

            return;
        }

        d->mTracksByFileNameSet.push_back(fileName);

        return;
    }

    d->mTracksByIdSet.insert(newTrackId);

    auto newTrack = d->mDatabase->trackDataFromDatabaseId(newTrackId);

    if (!newTrack.isEmpty()) {
        Q_EMIT trackHasChanged({newTrack});
    }
}

void TracksListener::newAlbumInList(qulonglong newDatabaseId, const QString &entryTitle)
{
    qCDebug(orgKdeElisaPlayList()) << "TracksListener::newAlbumInList" << newDatabaseId << entryTitle << d->mDatabase->albumData(newDatabaseId);
    Q_EMIT tracksListAdded(newDatabaseId, entryTitle, ElisaUtils::Album, d->mDatabase->albumData(newDatabaseId));
}

void TracksListener::newEntryInList(qulonglong newDatabaseId,
                                    const QString &entryTitle,
                                    ElisaUtils::PlayListEntryType databaseIdType)
{
    qCDebug(orgKdeElisaPlayList()) << "TracksListener::newEntryInList" << newDatabaseId << entryTitle << databaseIdType;
    switch (databaseIdType)
    {
    case ElisaUtils::Track:
    {
        d->mTracksByIdSet.insert(newDatabaseId);

        auto newTrack = d->mDatabase->trackDataFromDatabaseId(newDatabaseId);
        if (!newTrack.isEmpty()) {
            Q_EMIT trackHasChanged(newTrack);
        }
        break;
    }
    case ElisaUtils::Radio:
    {
        d->mRadiosByIdSet.insert(newDatabaseId);

        auto newRadio = d->mDatabase->radioDataFromDatabaseId(newDatabaseId);
        if (!newRadio.isEmpty()) {
            Q_EMIT trackHasChanged(newRadio);
        }
        break;
    }
    case ElisaUtils::Artist:
        newArtistInList(newDatabaseId, entryTitle);
        break;
    case ElisaUtils::FileName:
        trackByFileNameInList(QUrl::fromLocalFile(entryTitle));
        break;
    case ElisaUtils::Album:
        newAlbumInList(newDatabaseId, entryTitle);
        break;
    case ElisaUtils::Lyricist:
    case ElisaUtils::Composer:
    case ElisaUtils::Genre:
    case ElisaUtils::Unknown:
        break;
    }
}

void TracksListener::newArtistInList(qulonglong newDatabaseId, const QString &artist)
{
    auto newTracks = d->mDatabase->tracksDataFromAuthor(artist);
    if (newTracks.isEmpty()) {
        return;
    }

    for (const auto &oneTrack : newTracks) {
        d->mTracksByIdSet.insert(oneTrack.databaseId());
    }

    Q_EMIT tracksListAdded(newDatabaseId, artist, ElisaUtils::Artist, newTracks);
}

MusicAudioTrack TracksListener::scanOneFile(const QUrl &scanFile)
{
    return d->mFileScanner.scanOneFile(scanFile, d->mMimeDb);
}


#include "moc_trackslistener.cpp"
