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

void TracksListener::tracksAdded(const DatabaseInterface::DataListType &allTracks)
{
    for (const auto &oneTrack : allTracks) {
        if (d->mTracksByIdSet.contains(oneTrack[DataType::key_type::DatabaseIdRole].toULongLong())) {
            //Q_EMIT trackHasChanged(oneTrack);
        }

        if (d->mTracksByNameSet.isEmpty()) {
            return;
        }

        for (auto itTrack = d->mTracksByNameSet.begin(); itTrack != d->mTracksByNameSet.end(); ) {
            if (std::get<0>(*itTrack) != oneTrack[DataType::key_type::TitleRole].toString()) {
                ++itTrack;
                continue;
            }

            if (std::get<1>(*itTrack) != oneTrack[DataType::key_type::ArtistRole].toString()) {
                ++itTrack;
                continue;
            }

            if (std::get<2>(*itTrack) != oneTrack[DataType::key_type::AlbumRole].toString()) {
                ++itTrack;
                continue;
            }

            if (std::get<3>(*itTrack) != oneTrack[DataType::key_type::TrackNumberRole].toInt()) {
                ++itTrack;
                continue;
            }

            if (std::get<4>(*itTrack) != oneTrack[DataType::key_type::DiscNumberRole].toInt()) {
                ++itTrack;
                continue;
            }

            //Q_EMIT trackHasChanged(oneTrack);

            d->mTracksByIdSet.insert(oneTrack[DataType::key_type::DatabaseIdRole].toULongLong());
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

void TracksListener::trackModified(const DataType &modifiedTrack)
{
    if (d->mTracksByIdSet.contains(modifiedTrack[DataType::key_type::DatabaseIdRole].toULongLong())) {
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
        Q_EMIT trackHasChanged({newTrack});
    }
}

void TracksListener::trackByFileNameInList(const QUrl &fileName)
{
    qDebug() << "TracksListener::trackByFileNameInList" << fileName;
    auto newTrackId = d->mDatabase->trackIdFromFileName(fileName);
    qDebug() << "TracksListener::trackByFileNameInList" << fileName << newTrackId;
    if (newTrackId == 0) {
        auto newTrack = scanOneFile(fileName);

        qDebug() << "TracksListener::trackByFileNameInList" << fileName << newTrack;

        if (newTrack.isValid()) {
            qDebug() << "TracksListener::trackByFileNameInList" << "trackHasChanged" << newTrack;
            Q_EMIT trackHasChanged({{DataType::key_type::DatabaseIdRole, newTrack.databaseId()}});

            return;
        }

        d->mTracksByFileNameSet.push_back(fileName);

        return;
    }

    d->mTracksByIdSet.insert(newTrackId);

    auto newTrack = d->mDatabase->trackDataFromDatabaseId(newTrackId);

    qDebug() << "TracksListener::trackByFileNameInList" << newTrackId << newTrack;

    if (!newTrack.isEmpty()) {
        qDebug() << "TracksListener::trackByFileNameInList" << "trackHasChanged" << newTrack;
        Q_EMIT trackHasChanged({newTrack});
    }
}

void TracksListener::trackByIdInList(qulonglong newTrackId)
{
    d->mTracksByIdSet.insert(newTrackId);

    auto newTrack = d->mDatabase->trackDataFromDatabaseId(newTrackId);
    if (!newTrack.isEmpty()) {
        Q_EMIT trackHasChanged({newTrack});
    }
}

void TracksListener::newArtistInList(const QString &artist)
{
    auto newTracks = d->mDatabase->tracksFromAuthor(artist);
    if (newTracks.isEmpty()) {
        return;
    }

    for (const auto &oneTrack : newTracks) {
        d->mTracksByIdSet.insert(oneTrack.databaseId());
    }

    //Q_EMIT albumAdded(newTracks);
}

MusicAudioTrack TracksListener::scanOneFile(const QUrl &scanFile)
{
    return d->mFileScanner.scanOneFile(scanFile, d->mMimeDb);
}


#include "moc_trackslistener.cpp"
