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

#include "datatypes.h"
#include "elisautils.h"

#include <QObject>
#include <QMediaPlayer>

#include <memory>

class DatabaseInterface;
class MusicListenersManagerPrivate;
class MediaPlayList;
class ElisaApplication;
class ModelDataLoader;
class TracksListener;

class ELISALIB_EXPORT MusicListenersManager : public QObject
{

    Q_OBJECT

    Q_PROPERTY(DatabaseInterface* viewDatabase
               READ viewDatabase
               NOTIFY viewDatabaseChanged)

    Q_PROPERTY(int importedTracksCount
               READ importedTracksCount
               NOTIFY importedTracksCountChanged)

    Q_PROPERTY(ElisaApplication* elisaApplication
               READ elisaApplication
               WRITE setElisaApplication
               NOTIFY elisaApplicationChanged)

    Q_PROPERTY(TracksListener* tracksListener
               READ tracksListener
               NOTIFY tracksListenerChanged)

    Q_PROPERTY(bool indexerBusy
               READ indexerBusy
               NOTIFY indexerBusyChanged)

    Q_PROPERTY(bool fileSystemIndexerActive
               READ fileSystemIndexerActive
               NOTIFY fileSystemIndexerActiveChanged)

    Q_PROPERTY(bool balooIndexerActive
               READ balooIndexerActive
               NOTIFY balooIndexerActiveChanged)

    Q_PROPERTY(bool balooIndexerAvailable
               READ balooIndexerAvailable
               NOTIFY balooIndexerAvailableChanged)

    Q_PROPERTY(bool androidIndexerActive
               READ androidIndexerActive
               NOTIFY androidIndexerActiveChanged)

    Q_PROPERTY(bool androidIndexerAvailable
               READ androidIndexerAvailable
               NOTIFY androidIndexerAvailableChanged)

public:

    explicit MusicListenersManager(QObject *parent = nullptr);

    ~MusicListenersManager() override;

    DatabaseInterface* viewDatabase() const;

    void subscribeForTracks(MediaPlayList *client);

    int importedTracksCount() const;

    ElisaApplication* elisaApplication() const;

    TracksListener* tracksListener() const;

    bool indexerBusy() const;

    bool fileSystemIndexerActive() const;

    bool balooIndexerActive() const;

    bool balooIndexerAvailable() const;

    bool androidIndexerActive() const;

    bool androidIndexerAvailable() const;

Q_SIGNALS:

    void viewDatabaseChanged();

    void applicationIsTerminating();

    void tracksListenerChanged();

    void importedTracksCountChanged();

    void elisaApplicationChanged();

    void removeTracksInError(const QList<QUrl> &tracks);

    void displayTrackError(const QString &fileName);

    void indexerBusyChanged();

    void clearDatabase();

    void fileSystemIndexerActiveChanged();

    void balooIndexerActiveChanged();

    void balooIndexerAvailableChanged();

    void androidIndexerActiveChanged();

    void androidIndexerAvailableChanged();

public Q_SLOTS:

    void databaseReady();

    void applicationAboutToQuit();

    void showConfiguration();

    void setElisaApplication(ElisaApplication* elisaApplication);

    void playBackError(const QUrl &sourceInError, QMediaPlayer::Error playerError);

    void deleteElementById(ElisaUtils::PlayListEntryType entryType, qulonglong databaseId);

    void connectModel(ModelDataLoader *dataLoader);

    void resetMusicData();

private Q_SLOTS:

    void configChanged();

    void increaseImportedTracksCount(const DataTypes::ListTrackDataType &allTracks);

    void decreaseImportedTracksCount();

    void monitorStartingListeners();

    void monitorEndingListeners();

    void cleanedDatabase();

    void balooAvailabilityChanged();

private:

    void testBalooIndexerAvailability();

    void startLocalFileSystemIndexing();

    void startBalooIndexing();

    std::unique_ptr<MusicListenersManagerPrivate> d;

    void createTracksListener();

};

#endif // MUSICLISTENERSMANAGER_H
