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

#include "notificationitem.h"

#include <QObject>

#include <QMediaPlayer>

#include "musicalbum.h"
#include "musicartist.h"
#include "musicaudiotrack.h"

#include <memory>

class MusicListenersManagerPrivate;
class DatabaseInterface;
class MediaPlayList;
class NotificationItem;
class ElisaApplication;
class QAbstractItemModel;

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

    Q_PROPERTY(ElisaApplication* elisaApplication
               READ elisaApplication
               WRITE setElisaApplication
               NOTIFY elisaApplicationChanged)

    Q_PROPERTY(QAbstractItemModel* allAlbumsModel
               READ allAlbumsModel
               NOTIFY allAlbumsModelChanged)

    Q_PROPERTY(QAbstractItemModel* allArtistsModel
               READ allArtistsModel
               NOTIFY allArtistsModelChanged)

    Q_PROPERTY(QAbstractItemModel* allTracksModel
               READ allTracksModel
               NOTIFY allTracksModelChanged)

    Q_PROPERTY(bool indexerBusy
               READ indexerBusy
               NOTIFY indexerBusyChanged)

public:

    explicit MusicListenersManager(QObject *parent = nullptr);

    ~MusicListenersManager() override;

    DatabaseInterface* viewDatabase() const;

    void subscribeForTracks(MediaPlayList *client);

    int importedTracksCount() const;

    bool isIndexingRunning() const;

    ElisaApplication* elisaApplication() const;

    QAbstractItemModel *allAlbumsModel() const;

    QAbstractItemModel *allArtistsModel() const;

    QAbstractItemModel *allTracksModel() const;

    bool indexerBusy() const;

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

    void importedTracksCountChanged();

    void indexingRunningChanged();

    void newNotification(NotificationItem notification);

    void closeNotification(QString notificationId);

    void elisaApplicationChanged();

    void removeTracksInError(QList<QUrl> tracks);

    void displayTrackError(const QString &fileName);

    void allAlbumsModelChanged();

    void allArtistsModelChanged();

    void allTracksModelChanged();

    void indexerBusyChanged();

public Q_SLOTS:

    void databaseReady();

    void applicationAboutToQuit();

    void showConfiguration();

    void resetImportedTracksCounter();

    void setElisaApplication(ElisaApplication* elisaApplication);

    void playBackError(QUrl sourceInError, QMediaPlayer::Error playerError);

private Q_SLOTS:

    void configChanged();

    void computeImportedTracksCount();

    void monitorStartingListeners();

    void monitorEndingListeners(int tracksCount);

private:

    std::unique_ptr<MusicListenersManagerPrivate> d;

};

#endif // MUSICLISTENERSMANAGER_H
