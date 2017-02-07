/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef MUSICLISTENERSMANAGER_H
#define MUSICLISTENERSMANAGER_H

#include <QObject>

#include "musicalbum.h"
#include "musicartist.h"
#include "musicaudiotrack.h"

class MusicListenersManagerPrivate;
class DatabaseInterface;
class MediaPlayList;

class MusicListenersManager : public QObject
{

    Q_OBJECT

    Q_PROPERTY(DatabaseInterface* viewDatabase
               READ viewDatabase
               NOTIFY viewDatabaseChanged)

public:

    explicit MusicListenersManager(QObject *parent = 0);

    virtual ~MusicListenersManager();

    DatabaseInterface* viewDatabase() const;

    void subscribeForTracks(MediaPlayList *client);

Q_SIGNALS:

    void viewDatabaseChanged();

    void artistAdded(MusicArtist newArtist);

    void albumAdded(MusicAlbum newAlbum);

    void trackAdded(MusicAudioTrack newTrack);

    void artistRemoved(MusicArtist removedArtist);

    void albumRemoved(MusicAlbum removedAlbum);

    void trackRemoved(MusicAudioTrack removedTrack);

    void artistModified(MusicArtist modifiedArtist);

    void albumModified(MusicAlbum modifiedAlbum);

    void trackModified(MusicAudioTrack modifiedTrack);

public Q_SLOTS:

    void databaseReady();

private:

    MusicListenersManagerPrivate *d;

};

#endif // MUSICLISTENERSMANAGER_H
