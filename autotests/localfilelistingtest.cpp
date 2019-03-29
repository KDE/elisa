/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "databasetestdata.h"

#include "file/localfilelisting.h"
#include "musicaudiotrack.h"

#include "config-upnp-qt.h"

#include <QObject>
#include <QUrl>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QVector>
#include <QThread>
#include <QMetaObject>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

#include <QDebug>

#include <QtTest>
#include <QTest>

#include <algorithm>
#include <filesystem>

class LocalFileListingTests: public QObject, public DatabaseTestData
{
    Q_OBJECT

public:

    LocalFileListingTests(QObject *parent = nullptr) : QObject(parent)
    {
    }

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QList<MusicAudioTrack>>("QList<MusicAudioTrack>");
        qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
        qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
        qRegisterMetaType<QList<QUrl>>("QList<QUrl>");
        qRegisterMetaType<NotificationItem>("NotificationItem");
    }

    void initialTestWithNoTrack()
    {
        LocalFileListing myListing;

        QSignalSpy tracksListSpy(&myListing, &LocalFileListing::tracksList);
        QSignalSpy removedTracksListSpy(&myListing, &LocalFileListing::removedTracksList);

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        myListing.init();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        myListing.setAllRootPaths({QStringLiteral("/directoryNotExist")});

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        QCOMPARE(myListing.allRootPaths(), QStringList{QStringLiteral("/directoryNotExist")});

        myListing.refreshContent();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);
    }

    void initialTestWithTracks()
    {
        LocalFileListing myListing;

        QString musicPath = QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/music");

        QSignalSpy tracksListSpy(&myListing, &LocalFileListing::tracksList);
        QSignalSpy removedTracksListSpy(&myListing, &LocalFileListing::removedTracksList);

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        myListing.init();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        myListing.setAllRootPaths({musicPath});

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        //QCOMPARE(myListing.rootPath(), musicPath);

        myListing.refreshContent();

        QCOMPARE(tracksListSpy.count(), 2);
        QCOMPARE(removedTracksListSpy.count(), 0);

        const auto &firstNewTracksSignal = tracksListSpy.at(0);
        auto firstNewTracks = firstNewTracksSignal.at(0).value<QList<MusicAudioTrack>>();
        const auto &secondNewTracksSignal = tracksListSpy.at(1);
        auto secondNewTracks = secondNewTracksSignal.at(0).value<QList<MusicAudioTrack>>();
        auto newCovers = secondNewTracksSignal.at(1).value<QHash<QString, QUrl>>();

        QCOMPARE(firstNewTracks.count() + secondNewTracks.count(), 3);
        QCOMPARE(newCovers.count(), 3);
    }

    void addAndRemoveTracks()
    {
        LocalFileListing myListing;

        QString musicOriginPath = QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/music");

        QString musicPath = QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH) + QStringLiteral("/music2/data/innerData");

        QString musicParentPath = QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH) + QStringLiteral("/music2");
        QDir musicParentDirectory(musicParentPath);
        QDir rootDirectory(QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH));

        QCOMPARE(musicParentDirectory.removeRecursively(), true);
        rootDirectory.mkpath(QStringLiteral("music2/data/innerData"));

        QSignalSpy tracksListSpy(&myListing, &LocalFileListing::tracksList);
        QSignalSpy removedTracksListSpy(&myListing, &LocalFileListing::removedTracksList);
        QSignalSpy errorWatchingFileSystemChangesSpy(&myListing, &LocalFileListing::errorWatchingFileSystemChanges);

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        myListing.init();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        myListing.setAllRootPaths({musicParentPath});

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        QCOMPARE(myListing.allRootPaths(), QStringList{musicParentPath});

        myListing.refreshContent();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        QFile myTrack(musicOriginPath + QStringLiteral("/test.ogg"));
        myTrack.copy(musicPath + QStringLiteral("/test.ogg"));
        QFile myCover(musicOriginPath + QStringLiteral("/cover.jpg"));
        myCover.copy(musicPath + QStringLiteral("/cover.jpg"));

        QCOMPARE(tracksListSpy.wait(), true);

        QCOMPARE(tracksListSpy.count(), 1);
        QCOMPARE(removedTracksListSpy.count(), 0);

        auto newTracksSignal = tracksListSpy.at(0);
        auto newTracks = newTracksSignal.at(0).value<QList<MusicAudioTrack>>();
        auto newCovers = newTracksSignal.at(1).value<QHash<QString, QUrl>>();

        QCOMPARE(newTracks.count(), 1);
        QCOMPARE(newCovers.count(), 1);

        QDir musicDirectory(musicPath);
        QCOMPARE(musicDirectory.removeRecursively(), true);

        auto removedFilesWorking = removedTracksListSpy.wait();

        if (!removedFilesWorking && errorWatchingFileSystemChangesSpy.count()) {
            QEXPECT_FAIL("", "Impossible watching file system for changes", Abort);
        }
        QCOMPARE(removedFilesWorking, true);

        QCOMPARE(tracksListSpy.count(), 1);
        QCOMPARE(removedTracksListSpy.count(), 1);

        auto removeSignal = removedTracksListSpy.at(0);
        auto removedTracks = removeSignal.at(0).value<QList<QUrl>>();
        QCOMPARE(removedTracks.isEmpty(), false);

        QCOMPARE(rootDirectory.mkpath(QStringLiteral("music2/data/innerData")), true);
        QCOMPARE(myTrack.copy(musicPath + QStringLiteral("/test.ogg")), true);
        QCOMPARE(myCover.copy(musicPath + QStringLiteral("/cover.jpg")), true);

        if (tracksListSpy.count() == 1) {
            QCOMPARE(tracksListSpy.wait(), true);
        }

        QCOMPARE(tracksListSpy.count(), 2);
        QCOMPARE(removedTracksListSpy.count(), 1);

        auto newTracksSignalLast = tracksListSpy.at(1);
        auto newTracksLast = newTracksSignalLast.at(0).value<QList<MusicAudioTrack>>();
        auto newCoversLast = newTracksSignalLast.at(1).value<QHash<QString, QUrl>>();

        QCOMPARE(newTracksLast.count(), 1);
        QCOMPARE(newCoversLast.count(), 1);
    }

    void addTracksAndRemoveDirectory()
    {
        LocalFileListing myListing;

        QString musicOriginPath = QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/music");

        QString musicPath = QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH) + QStringLiteral("/music2/data/innerData");

        QString innerMusicPath = QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH) + QStringLiteral("/music2/data");

        QString musicParentPath = QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH) + QStringLiteral("/music2");
        QDir musicParentDirectory(musicParentPath);
        QDir rootDirectory(QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH));

        musicParentDirectory.removeRecursively();
        rootDirectory.mkpath(QStringLiteral("music2/data/innerData"));

        QSignalSpy tracksListSpy(&myListing, &LocalFileListing::tracksList);
        QSignalSpy removedTracksListSpy(&myListing, &LocalFileListing::removedTracksList);
        QSignalSpy errorWatchingFileSystemChangesSpy(&myListing, &LocalFileListing::errorWatchingFileSystemChanges);

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        myListing.init();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        myListing.setAllRootPaths({musicParentPath});

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        QCOMPARE(myListing.allRootPaths(), QStringList{musicParentPath});

        myListing.refreshContent();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);

        QFile myTrack(musicOriginPath + QStringLiteral("/test.ogg"));
        myTrack.copy(musicPath + QStringLiteral("/test.ogg"));
        QFile myCover(musicOriginPath + QStringLiteral("/cover.jpg"));
        myCover.copy(musicPath + QStringLiteral("/cover.jpg"));

        QCOMPARE(tracksListSpy.wait(), true);

        QCOMPARE(tracksListSpy.count(), 1);
        QCOMPARE(removedTracksListSpy.count(), 0);

        auto newTracksSignal = tracksListSpy.at(0);
        auto newTracks = newTracksSignal.at(0).value<QList<MusicAudioTrack>>();
        auto newCovers = newTracksSignal.at(1).value<QHash<QString, QUrl>>();

        QCOMPARE(newTracks.count(), 1);
        QCOMPARE(newCovers.count(), 1);

        QDir musicDirectory(innerMusicPath);
        musicDirectory.removeRecursively();

        auto removedFilesWorking = removedTracksListSpy.wait();

        if (!removedFilesWorking && errorWatchingFileSystemChangesSpy.count()) {
            QEXPECT_FAIL("", "Impossible watching file system for changes", Abort);
        }
        QCOMPARE(removedFilesWorking, true);

        QCOMPARE(tracksListSpy.count(), 1);
        QCOMPARE(removedTracksListSpy.count(), 1);

        auto removeSignal = removedTracksListSpy.at(0);
        auto removedTracks = removeSignal.at(0).value<QList<QUrl>>();
        QCOMPARE(removedTracks.isEmpty(), false);

        QCOMPARE(rootDirectory.mkpath(QStringLiteral("music2/data/innerData")), true);
        QCOMPARE(myTrack.copy(musicPath + QStringLiteral("/test.ogg")), true);
        QCOMPARE(myCover.copy(musicPath + QStringLiteral("/cover.jpg")), true);

        if (tracksListSpy.count() == 1) {
            QCOMPARE(tracksListSpy.wait(), true);
        }

        QCOMPARE(tracksListSpy.count(), 2);
        QCOMPARE(removedTracksListSpy.count(), 1);

        auto newTracksSignalLast = tracksListSpy.at(1);
        auto newTracksLast = newTracksSignalLast.at(0).value<QList<MusicAudioTrack>>();
        auto newCoversLast = newTracksSignalLast.at(1).value<QHash<QString, QUrl>>();

        QCOMPARE(newTracksLast.count(), 1);
        QCOMPARE(newCoversLast.count(), 1);
    }

    void addAndMoveTracks()
    {
        LocalFileListing myListing;

        QString musicOriginPath = QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/music");

        QString musicPath = QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH) + QStringLiteral("/music2/data/innerData");
        QDir musicDirectory(musicPath);

        QString musicParentPath = QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH) + QStringLiteral("/music2");
        QDir musicParentDirectory(musicParentPath);

        QString musicFriendPath = QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH) + QStringLiteral("/music3");
        QDir musicFriendDirectory(musicFriendPath);

        QCOMPARE(musicFriendDirectory.removeRecursively(), true);
        QCOMPARE(musicParentDirectory.removeRecursively(), true);

        musicFriendDirectory.mkpath(musicFriendPath);
        musicDirectory.mkpath(musicPath);

        QSignalSpy tracksListSpy(&myListing, &LocalFileListing::tracksList);
        QSignalSpy removedTracksListSpy(&myListing, &LocalFileListing::removedTracksList);
        QSignalSpy modifiedTracksListSpy(&myListing, &LocalFileListing::modifyTracksList);
        QSignalSpy errorWatchingFileSystemChangesSpy(&myListing, &LocalFileListing::errorWatchingFileSystemChanges);

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        myListing.init();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        myListing.setAllRootPaths({musicParentPath});

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        QCOMPARE(myListing.allRootPaths(), QStringList{musicParentPath});

        myListing.refreshContent();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        QFile myTrack(musicOriginPath + QStringLiteral("/test.ogg"));
        myTrack.copy(musicPath + QStringLiteral("/test.ogg"));
        QFile myCover(musicOriginPath + QStringLiteral("/cover.jpg"));
        myCover.copy(musicPath + QStringLiteral("/cover.jpg"));

        QCOMPARE(tracksListSpy.wait(), true);

        QCOMPARE(tracksListSpy.count(), 1);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        auto newTracksSignal = tracksListSpy.at(0);
        auto newTracks = newTracksSignal.at(0).value<QList<MusicAudioTrack>>();
        auto newCovers = newTracksSignal.at(1).value<QHash<QString, QUrl>>();

        QCOMPARE(newTracks.count(), 1);
        QCOMPARE(newCovers.count(), 1);

        std::rename(musicPath.toStdString().c_str(), musicFriendPath.toStdString().c_str());

        auto removedFilesWorking = removedTracksListSpy.wait();

        if (!removedFilesWorking && errorWatchingFileSystemChangesSpy.count()) {
            QEXPECT_FAIL("", "Impossible watching file system for changes", Abort);
        }
        QCOMPARE(removedFilesWorking, true);

        QCOMPARE(tracksListSpy.count(), 1);
        QCOMPARE(removedTracksListSpy.count(), 1);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        auto removeSignal = removedTracksListSpy.at(0);
        auto removedTracks = removeSignal.at(0).value<QList<QUrl>>();
        QCOMPARE(removedTracks.isEmpty(), false);

        QCOMPARE(musicFriendDirectory.mkpath(musicFriendPath), true);
        QCOMPARE(musicDirectory.mkpath(musicPath), true);
        QCOMPARE(myTrack.copy(musicPath + QStringLiteral("/test.ogg")), true);
        QCOMPARE(myCover.copy(musicPath + QStringLiteral("/cover.jpg")), true);

        if (tracksListSpy.count() == 1) {
            QCOMPARE(tracksListSpy.wait(), true);
        }

        QCOMPARE(tracksListSpy.count(), 2);
        QCOMPARE(removedTracksListSpy.count(), 1);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        auto newTracksSignalLast = tracksListSpy.at(1);
        auto newTracksLast = newTracksSignalLast.at(0).value<QList<MusicAudioTrack>>();
        auto newCoversLast = newTracksSignalLast.at(1).value<QHash<QString, QUrl>>();

        QCOMPARE(newTracksLast.count(), 1);
        QCOMPARE(newCoversLast.count(), 1);
    }

    void restoreRemovedTracks()
    {
        LocalFileListing myListing;

        QSignalSpy tracksListSpy(&myListing, &LocalFileListing::tracksList);
        QSignalSpy removedTracksListSpy(&myListing, &LocalFileListing::removedTracksList);
        QSignalSpy modifiedTracksListSpy(&myListing, &LocalFileListing::modifyTracksList);
        QSignalSpy indexingStartedSpy(&myListing, &LocalFileListing::indexingStarted);
        QSignalSpy indexingFinishedSpy(&myListing, &LocalFileListing::indexingFinished);
        QSignalSpy newNotificationSpy(&myListing, &LocalFileListing::newNotification);
        QSignalSpy closeNotificationSpy(&myListing, &LocalFileListing::closeNotification);
        QSignalSpy askRestoredTracksSpy(&myListing, &LocalFileListing::askRestoredTracks);

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);
        QCOMPARE(indexingStartedSpy.count(), 0);
        QCOMPARE(indexingFinishedSpy.count(), 0);
        QCOMPARE(newNotificationSpy.count(), 0);
        QCOMPARE(closeNotificationSpy.count(), 0);
        QCOMPARE(askRestoredTracksSpy.count(), 0);

        myListing.setAllRootPaths({QStringLiteral("/does/not/exists")});

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);
        QCOMPARE(indexingStartedSpy.count(), 0);
        QCOMPARE(indexingFinishedSpy.count(), 0);
        QCOMPARE(newNotificationSpy.count(), 0);
        QCOMPARE(closeNotificationSpy.count(), 0);
        QCOMPARE(askRestoredTracksSpy.count(), 0);

        myListing.init();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);
        QCOMPARE(indexingStartedSpy.count(), 0);
        QCOMPARE(indexingFinishedSpy.count(), 0);
        QCOMPARE(newNotificationSpy.count(), 0);
        QCOMPARE(closeNotificationSpy.count(), 0);
        QCOMPARE(askRestoredTracksSpy.count(), 1);

        myListing.restoredTracks({{QUrl::fromLocalFile(QStringLiteral("/removed/files1")), QDateTime::fromMSecsSinceEpoch(1)},
                                  {QUrl::fromLocalFile(QStringLiteral("/removed/files2")), QDateTime::fromMSecsSinceEpoch(2)}});

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 1);
        QCOMPARE(modifiedTracksListSpy.count(), 0);
        QCOMPARE(indexingStartedSpy.count(), 1);
        QCOMPARE(indexingFinishedSpy.count(), 0);
        QCOMPARE(newNotificationSpy.count(), 0);
        QCOMPARE(closeNotificationSpy.count(), 0);
        QCOMPARE(askRestoredTracksSpy.count(), 1);

        auto removedTracksSignal = removedTracksListSpy.at(0);

        QCOMPARE(removedTracksSignal.count(), 1);

        auto removedTracks = removedTracksSignal.at(0).value<QList<QUrl>>();

        QCOMPARE(removedTracks.count(), 2);

        std::sort(removedTracks.begin(), removedTracks.end());

        QCOMPARE(removedTracks[0], QUrl::fromLocalFile(QStringLiteral("/removed/files1")));
        QCOMPARE(removedTracks[1], QUrl::fromLocalFile(QStringLiteral("/removed/files2")));
    }
};

QTEST_GUILESS_MAIN(LocalFileListingTests)


#include "localfilelistingtest.moc"
