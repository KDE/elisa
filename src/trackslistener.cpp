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

#include "databaseinterface.h"
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
            if (std::get<0>(*itTrack) != oneTrack.title()) {
                ++itTrack;
                continue;
            }

            if (std::get<1>(*itTrack) != oneTrack.artist()) {
                ++itTrack;
                continue;
            }

            if (std::get<2>(*itTrack) != oneTrack.album()) {
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

void TracksListener::trackByNameInList(const QString &title, const QString &artist, const QString &album, int trackNumber, int discNumber)
{
    auto newTrackId = d->mDatabase->trackIdFromTitleAlbumTrackDiscNumber(title, artist, album, trackNumber, discNumber);
    if (newTrackId == 0) {
        auto newTrack = std::tuple<QString, QString, QString, int, int>(title, artist, album, trackNumber, discNumber);
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
            auto oneData = DatabaseInterface::TrackDataType{};

            oneData[DatabaseInterface::TrackDataType::key_type::TitleRole] = newTrack.title();
            oneData[DatabaseInterface::TrackDataType::key_type::ArtistRole] = newTrack.artist();
            oneData[DatabaseInterface::TrackDataType::key_type::AlbumRole] = newTrack.albumName();
            oneData[DatabaseInterface::TrackDataType::key_type::AlbumIdRole] = newTrack.albumId();
            oneData[DatabaseInterface::TrackDataType::key_type::TrackNumberRole] = newTrack.trackNumber();
            oneData[DatabaseInterface::TrackDataType::key_type::DiscNumberRole] = newTrack.discNumber();
            oneData[DatabaseInterface::TrackDataType::key_type::DurationRole] = newTrack.duration();
            oneData[DatabaseInterface::TrackDataType::key_type::MilliSecondsDurationRole] = newTrack.duration().msecsSinceStartOfDay();
            oneData[DatabaseInterface::TrackDataType::key_type::ResourceRole] = newTrack.resourceURI();
            oneData[DatabaseInterface::TrackDataType::key_type::ImageUrlRole] = newTrack.albumCover();

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
    Q_EMIT tracksListAdded(newDatabaseId, entryTitle, ElisaUtils::Album, d->mDatabase->albumData(newDatabaseId));
}

void TracksListener::newEntryInList(qulonglong newDatabaseId,
                                    const QString &entryTitle,
                                    ElisaUtils::PlayListEntryType databaseIdType)
{
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
