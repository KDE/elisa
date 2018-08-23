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

void TracksListener::tracksAdded(const QList<MusicAudioTrack> &allTracks)
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

            if (std::get<2>(*itTrack) != oneTrack.albumName()) {
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

            Q_EMIT trackHasChanged(oneTrack);

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

void TracksListener::trackModified(const MusicAudioTrack &modifiedTrack)
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

    auto newTrack = d->mDatabase->trackFromDatabaseId(newTrackId);

    if (newTrack.isValid()) {
        Q_EMIT trackHasChanged(newTrack);
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
            Q_EMIT trackHasChanged(newTrack);

            return;
        }

        d->mTracksByFileNameSet.push_back(fileName);

        return;
    }

    d->mTracksByIdSet.insert(newTrackId);

    auto newTrack = d->mDatabase->trackFromDatabaseId(newTrackId);

    qDebug() << "TracksListener::trackByFileNameInList" << newTrackId << newTrack;

    if (newTrack.isValid()) {
        qDebug() << "TracksListener::trackByFileNameInList" << "trackHasChanged" << newTrack;
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

void TracksListener::newArtistInList(const QString &artist)
{
    auto newTracks = d->mDatabase->tracksFromAuthor(artist);
    if (newTracks.isEmpty()) {
        return;
    }

    for (const auto &oneTrack : newTracks) {
        d->mTracksByIdSet.insert(oneTrack.databaseId());
    }

    Q_EMIT albumAdded(newTracks);
}

MusicAudioTrack TracksListener::scanOneFile(const QUrl &scanFile)
{
    return d->mFileScanner.scanOneFile(scanFile, d->mMimeDb);
}


#include "moc_trackslistener.cpp"
