/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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

    [[nodiscard]] DatabaseInterface* viewDatabase() const;

    void subscribeForTracks(MediaPlayList *client);

    [[nodiscard]] int importedTracksCount() const;

    [[nodiscard]] ElisaApplication* elisaApplication() const;

    [[nodiscard]] TracksListener* tracksListener() const;

    [[nodiscard]] bool indexerBusy() const;

    [[nodiscard]] bool fileSystemIndexerActive() const;

    [[nodiscard]] bool balooIndexerActive() const;

    [[nodiscard]] bool balooIndexerAvailable() const;

    [[nodiscard]] bool androidIndexerActive() const;

    [[nodiscard]] bool androidIndexerAvailable() const;

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

    void clearedDatabase();

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

    void startAndroidIndexing();

    void startBalooIndexing();

    auto initializeRootPath();

    std::unique_ptr<MusicListenersManagerPrivate> d;

    void createTracksListener();

};

#endif // MUSICLISTENERSMANAGER_H
