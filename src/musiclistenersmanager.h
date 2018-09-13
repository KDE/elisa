/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef MUSICLISTENERSMANAGER_H
#define MUSICLISTENERSMANAGER_H

#include "elisaLib_export.h"

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
class AbstractMediaProxyModel;

class ELISALIB_EXPORT MusicListenersManager : public QObject
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
               CONSTANT)

    Q_PROPERTY(QAbstractItemModel* allArtistsModel
               READ allArtistsModel
               CONSTANT)

    Q_PROPERTY(QAbstractItemModel* allTracksModel
               READ allTracksModel
               CONSTANT)

    Q_PROPERTY(QAbstractItemModel* allGenresModel
               READ allGenresModel
               CONSTANT)

    Q_PROPERTY(QAbstractItemModel* allLyricistsModel
               READ allLyricistsModel
               CONSTANT)

    Q_PROPERTY(QAbstractItemModel* allComposersModel
               READ allComposersModel
               CONSTANT)

    Q_PROPERTY(QAbstractItemModel* albumModel
               READ albumModel
               CONSTANT)

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

    QAbstractItemModel *allGenresModel() const;

    QAbstractItemModel *allLyricistsModel() const;

    QAbstractItemModel *allComposersModel() const;

    QAbstractItemModel *albumModel() const;

    bool indexerBusy() const;

Q_SIGNALS:

    void viewDatabaseChanged();

    void applicationIsTerminating();

    void importedTracksCountChanged();

    void indexingRunningChanged();

    void newNotification(NotificationItem notification);

    void closeNotification(QString notificationId);

    void elisaApplicationChanged();

    void removeTracksInError(QList<QUrl> tracks);

    void displayTrackError(const QString &fileName);

    void indexerBusyChanged();

public Q_SLOTS:

    void databaseReady();

    void applicationAboutToQuit();

    void showConfiguration();

    void setElisaApplication(ElisaApplication* elisaApplication);

    void playBackError(QUrl sourceInError, QMediaPlayer::Error playerError);

private Q_SLOTS:

    void configChanged();

    void increaseImportedTracksCount(const QList<MusicAudioTrack> &allTracks);

    void decreaseImportedTracksCount();

    void monitorStartingListeners();

    void monitorEndingListeners();

private:

    std::unique_ptr<MusicListenersManagerPrivate> d;

};

#endif // MUSICLISTENERSMANAGER_H
