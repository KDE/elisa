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
class NotificationItem;

class MusicListenersManager : public QObject
{

    Q_OBJECT

    Q_PROPERTY(DatabaseInterface* viewDatabase
               READ viewDatabase
               NOTIFY viewDatabaseChanged)

    Q_PROPERTY(int importedTracksCount
               READ importedTracksCount
               NOTIFY importedTracksCountChanged)

    Q_PROPERTY(bool indexingRunning
               READ isIndexingRunning
               NOTIFY indexingRunningChanged)

public:

    explicit MusicListenersManager(QObject *parent = 0);

    virtual ~MusicListenersManager();

    DatabaseInterface* viewDatabase() const;

    void subscribeForTracks(MediaPlayList *client);

    int importedTracksCount() const;

    bool isIndexingRunning() const;

Q_SIGNALS:

    void viewDatabaseChanged();

    void artistAdded(const MusicArtist &newArtist);

    void albumAdded(const MusicAlbum &newAlbum);

    void trackAdded(qulonglong id);

    void tracksAdded(const QList<MusicAudioTrack> &allTracks);

    void artistRemoved(const MusicArtist &removedArtist);

    void albumRemoved(const MusicAlbum &removedAlbum, qulonglong removedAlbumId);

    void trackRemoved(qulonglong id);

    void artistModified(const MusicArtist &modifiedArtist);

    void albumModified(const MusicAlbum &modifiedAlbum, qulonglong modifiedAlbumId);

    void trackModified(const MusicAudioTrack &modifiedTrack);

    void applicationIsTerminating();

    void databaseIsReady();

    void indexingStarted();

    void indexingFinished();

    void listenerNotification(NotificationItem *notification);

    void importedTracksCountChanged();

    void indexingRunningChanged();

public Q_SLOTS:

    void databaseReady();

    void applicationAboutToQuit();

    void showConfiguration();

private Q_SLOTS:

    void configChanged();

    void computeImportedTracksCount();

    void monitorStartingListeners();

    void monitorEndingListeners();

private:

    MusicListenersManagerPrivate *d;

};

#endif // MUSICLISTENERSMANAGER_H
