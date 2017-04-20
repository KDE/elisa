/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "trackslistener.h"

#include "databaseinterface.h"

#include <QSet>
#include <QList>

#include <array>
#include <algorithm>

class TracksListenerPrivate
{
public:

    QSet<qulonglong> mTracksByIdSet;

    QList<std::array<QString, 3>> mTracksByNameSet;

    DatabaseInterface *mDatabase = nullptr;

};

TracksListener::TracksListener(DatabaseInterface *database, QObject *parent) : QObject(parent), d(new TracksListenerPrivate)
{
    d->mDatabase = database;
}

void TracksListener::trackAdded(qulonglong id)
{
    if (d->mTracksByIdSet.find(id) != d->mTracksByIdSet.end()) {
        const auto &newTrack = d->mDatabase->trackFromDatabaseId(id);

        Q_EMIT trackHasChanged(newTrack);
    }

    if (d->mTracksByNameSet.isEmpty()) {
        return;
    }

    const auto &newTrack = d->mDatabase->trackFromDatabaseId(id);

    for (auto itTrack = d->mTracksByNameSet.begin(); itTrack != d->mTracksByNameSet.end(); ++itTrack) {
        if ((*itTrack)[0] != newTrack.title()) {
            continue;
        }

        if ((*itTrack)[1] != newTrack.artist()) {
            continue;
        }

        if ((*itTrack)[2] != newTrack.albumName()) {
            continue;
        }

        Q_EMIT trackHasChanged(newTrack);

        d->mTracksByIdSet.insert(newTrack.databaseId());
        d->mTracksByNameSet.erase(itTrack);
        continue;
    }
}

void TracksListener::trackRemoved(qulonglong id)
{
    if (d->mTracksByIdSet.find(id) != d->mTracksByIdSet.end()) {
        const auto &newTrack = d->mDatabase->trackFromDatabaseId(id);

        Q_EMIT trackHasBeenRemoved(newTrack);
    }
}

void TracksListener::trackModified(qulonglong id)
{
    if (d->mTracksByIdSet.find(id) != d->mTracksByIdSet.end()) {
        const auto &newTrack = d->mDatabase->trackFromDatabaseId(id);

        Q_EMIT trackHasChanged(newTrack);
    }
}

void TracksListener::trackByNameInList(QString title, QString artist, QString album)
{
    d->mTracksByNameSet.push_back({{title, artist, album}});

    auto newTrackId = d->mDatabase->trackIdFromTitleAlbumArtist(title, album, artist);
    if (newTrackId == 0) {
        return;
    }

    auto newTrack = d->mDatabase->trackFromDatabaseId(newTrackId);

    if (newTrack.isValid()) {
        Q_EMIT trackHasChanged(newTrack);
    }
}

void TracksListener::trackByIdInList(qulonglong newTrackId)
{
    d->mTracksByIdSet.insert(newTrackId);

    auto newTrack = d->mDatabase->trackFromDatabaseId(newTrackId);
    if (newTrack.isValid()) {
        Q_EMIT trackHasChanged(newTrack);
    }
}

void TracksListener::newArtistInList(QString artist)
{
    auto newTracks = d->mDatabase->tracksFromAuthor(artist);
    if (newTracks.isEmpty()) {
        return;
    }

    Q_EMIT albumAdded(newTracks);
}


#include "moc_trackslistener.cpp"
