/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "databasetestdata.h"

#include "file/localfilelisting.h"
#include "elisa_settings.h"

#include "config-upnp-qt.h"

#include <QObject>
#include <QUrl>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QList>
#include <QThread>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

#include <QSignalSpy>
#include <QTest>

#include <algorithm>

using namespace Qt::Literals::StringLiterals;

class LocalFileListingTests: public QObject, public DatabaseTestData
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QList<qlonglong>>("QList<qlonglong>");
        qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
        qRegisterMetaType<QList<QUrl>>("QList<QUrl>");
        Elisa::ElisaConfiguration::instance(QStringLiteral("scanAtStartupFeature"));
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

    void initialTestWithEnabledScanOnStartupSetting()
    {
        LocalFileListing myListing;

        Elisa::ElisaConfiguration::self()->setDefaults();

        QSignalSpy tracksListSpy(&myListing, &LocalFileListing::tracksList);
        QSignalSpy askRestoredTracksSpy(&myListing, &LocalFileListing::askRestoredTracks);

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(askRestoredTracksSpy.count(), 0);

        myListing.init();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(askRestoredTracksSpy.count(), 1);
    }

    void initialTestWithDisabledScanOnStartupSetting()
    {
        LocalFileListing myListing;

        Elisa::ElisaConfiguration::self()->setDefaults();
        Elisa::ElisaConfiguration::self()->setScanAtStartup(false);

        QSignalSpy tracksListSpy(&myListing, &LocalFileListing::tracksList);
        QSignalSpy askRestoredTracksSpy(&myListing, &LocalFileListing::askRestoredTracks);

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(askRestoredTracksSpy.count(), 0);

        myListing.init();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(askRestoredTracksSpy.count(), 0);
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
        auto firstNewTracks = firstNewTracksSignal.at(0).value<DataTypes::ListTrackDataType>();
        const auto &secondNewTracksSignal = tracksListSpy.at(1);
        auto secondNewTracks = secondNewTracksSignal.at(0).value<DataTypes::ListTrackDataType>();

        QCOMPARE(firstNewTracks.count() + secondNewTracks.count(), 5);

        tracksListSpy.clear();
        removedTracksListSpy.clear();

        myListing.resetAndRefreshContent();

        QCOMPARE(tracksListSpy.count(), 1);
        QCOMPARE(removedTracksListSpy.count(), 0);

        auto newTracks = tracksListSpy.at(0).at(0).value<DataTypes::ListTrackDataType>();

        QCOMPARE(newTracks.count(), 5);

        tracksListSpy.clear();
        removedTracksListSpy.clear();

        myListing.refreshContent();

        QVERIFY(tracksListSpy.isEmpty());
        QVERIFY(removedTracksListSpy.isEmpty());
    }

    void addAndRemoveTracks()
    {
        LocalFileListing myListing;

        QString musicOriginPath = QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/music");

        QString musicPath = QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH) + QStringLiteral("/music2/data/innerData");

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
        auto newTracks = newTracksSignal.at(0).value<DataTypes::ListTrackDataType>();

        QCOMPARE(newTracks.count(), 1);

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
        auto newTracksLast = newTracksSignalLast.at(0).value<DataTypes::ListTrackDataType>();

        QCOMPARE(newTracksLast.count(), 1);
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
        auto newTracks = newTracksSignal.at(0).value<DataTypes::ListTrackDataType>();

        QCOMPARE(newTracks.count(), 1);

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
        auto newTracksLast = newTracksSignalLast.at(0).value<DataTypes::ListTrackDataType>();

        QCOMPARE(newTracksLast.count(), 1);
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
        auto newTracks = newTracksSignal.at(0).value<DataTypes::ListTrackDataType>();

        QCOMPARE(newTracks.count(), 1);

        QDir().rename(musicPath, musicFriendPath);

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
        auto newTracksLast = newTracksSignalLast.at(0).value<DataTypes::ListTrackDataType>();

        QCOMPARE(newTracksLast.count(), 1);
    }

    void restoreRemovedTracks()
    {
        LocalFileListing myListing;

        Elisa::ElisaConfiguration::self()->setDefaults();

        QSignalSpy tracksListSpy(&myListing, &LocalFileListing::tracksList);
        QSignalSpy removedTracksListSpy(&myListing, &LocalFileListing::removedTracksList);
        QSignalSpy modifiedTracksListSpy(&myListing, &LocalFileListing::modifyTracksList);
        QSignalSpy indexingStartedSpy(&myListing, &LocalFileListing::indexingStarted);
        QSignalSpy indexingFinishedSpy(&myListing, &LocalFileListing::indexingFinished);
        QSignalSpy askRestoredTracksSpy(&myListing, &LocalFileListing::askRestoredTracks);

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);
        QCOMPARE(indexingStartedSpy.count(), 0);
        QCOMPARE(indexingFinishedSpy.count(), 0);
        QCOMPARE(askRestoredTracksSpy.count(), 0);

        myListing.setAllRootPaths({QStringLiteral("/does/not/exists")});

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);
        QCOMPARE(indexingStartedSpy.count(), 0);
        QCOMPARE(indexingFinishedSpy.count(), 0);
        QCOMPARE(askRestoredTracksSpy.count(), 0);

        myListing.init();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);
        QCOMPARE(indexingStartedSpy.count(), 0);
        QCOMPARE(indexingFinishedSpy.count(), 0);
        QCOMPARE(askRestoredTracksSpy.count(), 1);

        myListing.setIndexedTracks({
            {QUrl::fromLocalFile(QStringLiteral("/removed/files1")), QDateTime::fromMSecsSinceEpoch(1)},
            {QUrl::fromLocalFile(QStringLiteral("/removed/files2")), QDateTime::fromMSecsSinceEpoch(2)},
        });

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 1);
        QCOMPARE(modifiedTracksListSpy.count(), 0);
        QCOMPARE(indexingStartedSpy.count(), 1);
        QCOMPARE(indexingFinishedSpy.count(), 1);
        QCOMPARE(askRestoredTracksSpy.count(), 1);

        auto removedTracksSignal = removedTracksListSpy.at(0);

        QCOMPARE(removedTracksSignal.count(), 1);

        auto removedTracks = removedTracksSignal.at(0).value<QList<QUrl>>();

        QCOMPARE(removedTracks.count(), 2);

        std::sort(removedTracks.begin(), removedTracks.end());

        QCOMPARE(removedTracks[0], QUrl::fromLocalFile(QStringLiteral("/removed/files1")));
        QCOMPARE(removedTracks[1], QUrl::fromLocalFile(QStringLiteral("/removed/files2")));
    }

    void refreshIndex()
    {
        LocalFileListing myListing;

        const QString musicOriginPath = QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + u"/music"_s;

        const QString musicParentPath = QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH) + u"/music2"_s;

        const QString musicPath = musicParentPath + u"/data/innerData"_s;

        const QString trackOggPath = musicPath + u"/test.ogg"_s;

        const QString trackM4aPath = musicPath + u"/test.m4a"_s;

        const QString trackMp3Path = musicPath + u"/test.mp3"_s;

        QDir musicParentDirectory(musicParentPath);
        QDir musicDirectory(musicPath);
        QFile trackOgg(musicOriginPath + u"/test.ogg"_s);
        QFile trackMp3(musicOriginPath + u"/test.mp3"_s);
        QFile trackM4a(musicOriginPath + u"/test.m4a"_s);

        QVERIFY(musicParentDirectory.removeRecursively());

        QVERIFY(!musicDirectory.exists());
        QVERIFY(musicDirectory.mkpath(musicPath));
        QVERIFY(musicDirectory.exists());
        QVERIFY(musicDirectory.isEmpty());
        QVERIFY(trackOgg.copy(trackOggPath));
        QVERIFY(trackMp3.copy(trackMp3Path));
        QVERIFY(trackM4a.copy(trackM4aPath));

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

        QCOMPARE(tracksListSpy.count(), 2);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        auto newTracks = tracksListSpy.at(0).at(0).value<DataTypes::ListTrackDataType>();

        QCOMPARE(newTracks.count(), 2);

        newTracks = tracksListSpy.at(1).at(0).value<DataTypes::ListTrackDataType>();

        QCOMPARE(newTracks.count(), 1);

        tracksListSpy.clear();

        myListing.refreshContent();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        myListing.resetAndRefreshContent();

        QCOMPARE(tracksListSpy.count(), 1);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        newTracks = tracksListSpy.at(0).at(0).value<DataTypes::ListTrackDataType>();

        QCOMPARE(newTracks.count(), 3);

        tracksListSpy.clear();

        // Set the file modified time to some point in the future to ensure
        // the file does not get re-indexed
        myListing.setIndexedTracks({
            {QUrl::fromLocalFile(trackOggPath), QDateTime::currentDateTime().addYears(1)}
        });

        QCOMPARE(tracksListSpy.count(), 1);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        newTracks = tracksListSpy.at(0).at(0).value<DataTypes::ListTrackDataType>();

        QCOMPARE(newTracks.count(), 2);

        tracksListSpy.clear();

        myListing.setAllRootPaths({musicParentPath + u"/"_s});

        myListing.refreshContent();

        QCOMPARE(tracksListSpy.count(), 0);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        myListing.resetAndRefreshContent();

        QCOMPARE(tracksListSpy.count(), 1);
        QCOMPARE(removedTracksListSpy.count(), 0);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        newTracks = tracksListSpy.at(0).at(0).value<DataTypes::ListTrackDataType>();

        QCOMPARE(newTracks.count(), 3);

        tracksListSpy.clear();

        myListing.setIndexedTracks({
            {QUrl::fromLocalFile(trackM4aPath), QDateTime::currentDateTime().addYears(1)},
            {QUrl::fromLocalFile(trackMp3Path), QDateTime::fromMSecsSinceEpoch(1)},
            {QUrl::fromLocalFile(u"/does/not/exist.mp3"_s), QDateTime::currentDateTime()},
        });

        QCOMPARE(tracksListSpy.count(), 1);
        QCOMPARE(removedTracksListSpy.count(), 1);
        QCOMPARE(modifiedTracksListSpy.count(), 0);

        newTracks = tracksListSpy.at(0).at(0).value<DataTypes::ListTrackDataType>();
        auto removedTracks = removedTracksListSpy.at(0).at(0).value<QList<QUrl>>();

        QCOMPARE(newTracks.count(), 2);
        QCOMPARE(removedTracks.count(), 1);
    }
};

QTEST_GUILESS_MAIN(LocalFileListingTests)


#include "localfilelistingtest.moc"
