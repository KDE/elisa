/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "elisaapplication.h"
#include "config-upnp-qt.h"
#include "kiofusehelper.h"
#include "mediaplaylist.h"
#include "mediaplaylistproxymodel.h"
#include "mockkiofuse.h"

#include <QApplication>
#include <QFile>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QString>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTest>
#include <QUrl>

class ElisaApplicationTests: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
        qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QList<qlonglong>>("QList<qlonglong>");
        qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
        qRegisterMetaType<QList<QUrl>>("QList<QUrl>");
        qRegisterMetaType<DataTypes::EntryDataList>("DataTypes::EntryDataList");
        qRegisterMetaType<ElisaUtils::PlayListEntryType>("ElisaUtils::PlayListEntryType");
        qRegisterMetaType<ElisaUtils::PlayListEnqueueMode>("ElisaUtils::PlayListEnqueueMode");
        qRegisterMetaType<ElisaUtils::PlayListEnqueueTriggerPlay>("ElisaUtils::PlayListEnqueueTriggerPlay");
    }

    void testMostLocalUrlBypassSchemes()
    {
        const QUrl localUrl = QUrl::fromLocalFile(QStringLiteral("/music/track.mp3"));
        QCOMPARE(KioFuseHelper::mostLocalUrl(localUrl), localUrl);

        const QUrl emptySchemeUrl(QStringLiteral("relative/track.mp3"));
        QCOMPARE(KioFuseHelper::mostLocalUrl(emptySchemeUrl), emptySchemeUrl);

        const QUrl httpUrl(QStringLiteral("http://example.com/stream"));
        QCOMPARE(KioFuseHelper::mostLocalUrl(httpUrl), httpUrl);

        const QUrl httpsUrl(QStringLiteral("https://example.com/stream"));
        QCOMPARE(KioFuseHelper::mostLocalUrl(httpsUrl), httpsUrl);

        const QUrl audioCdUrl(QStringLiteral("audiocd:/Track%201.wav"));
        QCOMPARE(KioFuseHelper::mostLocalUrl(audioCdUrl), audioCdUrl);
    }

    void testMostLocalUrlKioFuse()
    {
#if QtDBus_FOUND
        ScopedMockKioFuse scopedKioFuse;
        if (!scopedKioFuse.isRegistered) {
            QSKIP("D-Bus session bus unavailable for mock KIOFuse service");
        }

        const QString remoteStr = QStringLiteral("smb://nas/music/song.mp3");
        const QString mountedStr = QStringLiteral("/run/user/1000/kio-fuse-xyz/smb/nas/music/song.mp3");
        scopedKioFuse.mock.mountMappings[remoteStr] = mountedStr;

        QUrl resolved = KioFuseHelper::mostLocalUrl(QUrl(remoteStr));
        QCOMPARE(resolved, QUrl::fromLocalFile(mountedStr));

        // When remote URL has no mapping, it falls back to source URL
        const QUrl unmappedRemote(QStringLiteral("sftp://otherhost/music.mp3"));
        QCOMPARE(KioFuseHelper::mostLocalUrl(unmappedRemote), unmappedRemote);
#endif
    }

    void testOpenFilesRadioStream()
    {
        ElisaApplication app;
        app.initialize();

        QSignalSpy enqueueSpy(&app, &ElisaApplication::enqueue);

        const QUrl streamUrl(QStringLiteral("http://stream.example.com:8000/live"));
        const QUrl streamUrlHttps(QStringLiteral("https://radio.example.com/stream"));
        bool result = app.openFiles({streamUrl, streamUrlHttps});

        QVERIFY(result);
        QCOMPARE(enqueueSpy.count(), 1);

        const auto entries = enqueueSpy.at(0).at(0).value<DataTypes::EntryDataList>();
        QCOMPARE(entries.count(), 2);
        QCOMPARE(entries.at(0).musicData[DataTypes::ElementTypeRole].value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Radio);
        QCOMPARE(entries.at(0).musicData[DataTypes::ResourceRole].toUrl(), streamUrl);

        QCOMPARE(entries.at(1).musicData[DataTypes::ElementTypeRole].value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Radio);
        QCOMPARE(entries.at(1).musicData[DataTypes::ResourceRole].toUrl(), streamUrlHttps);

        const auto enqueueMode = enqueueSpy.at(0).at(1).value<ElisaUtils::PlayListEnqueueMode>();
        QCOMPARE(enqueueMode, ElisaUtils::PlayListEnqueueMode::AppendPlayList);

        const auto triggerPlay = enqueueSpy.at(0).at(2).value<ElisaUtils::PlayListEnqueueTriggerPlay>();
        QCOMPARE(triggerPlay, ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);
    }

    void testOpenFilesLocalAudio()
    {
        ElisaApplication app;
        app.initialize();

        QTemporaryFile audioFile(QDir::tempPath() + QStringLiteral("/elisatest_XXXXXX.ogg"));
        QVERIFY(audioFile.open());
        const QUrl audioUrl = QUrl::fromLocalFile(audioFile.fileName());

        QSignalSpy enqueueSpy(&app, &ElisaApplication::enqueue);
        bool result = app.openFiles({audioUrl});

        QVERIFY(result);
        QCOMPARE(enqueueSpy.count(), 1);

        const auto entries = enqueueSpy.at(0).at(0).value<DataTypes::EntryDataList>();
        QCOMPARE(entries.count(), 1);
        QCOMPARE(entries.at(0).musicData[DataTypes::ElementTypeRole].value<ElisaUtils::PlayListEntryType>(), ElisaUtils::FileName);
        QCOMPARE(entries.at(0).musicData[DataTypes::ResourceRole].toUrl(), audioUrl);
    }

    void testOpenFilesKioAudio()
    {
#if QtDBus_FOUND
        ScopedMockKioFuse scopedKioFuse;
        if (!scopedKioFuse.isRegistered) {
            QSKIP("D-Bus session bus unavailable for mock KIOFuse service");
        }

        QTemporaryFile localFile(QDir::tempPath() + QStringLiteral("/kio_test_XXXXXX.ogg"));
        QVERIFY(localFile.open());

        const QUrl remoteKioUrl(QStringLiteral("smb://nas/music/song.ogg"));
        scopedKioFuse.mock.mountMappings[remoteKioUrl.toString()] = localFile.fileName();

        ElisaApplication app;
        app.initialize();

        QSignalSpy enqueueSpy(&app, &ElisaApplication::enqueue);
        bool result = app.openFiles({remoteKioUrl});

        QVERIFY(result);
        QCOMPARE(enqueueSpy.count(), 1);

        const auto entries = enqueueSpy.at(0).at(0).value<DataTypes::EntryDataList>();
        QCOMPARE(entries.count(), 1);
        QCOMPARE(entries.at(0).musicData[DataTypes::ElementTypeRole].value<ElisaUtils::PlayListEntryType>(), ElisaUtils::FileName);
        QCOMPARE(entries.at(0).musicData[DataTypes::ResourceRole].toUrl(), remoteKioUrl);
#endif
    }

    void testOpenFilesLocalPlaylist()
    {
        ElisaApplication app;
        app.initialize();

        QTemporaryFile playlistFile(QDir::tempPath() + QStringLiteral("/playlist_XXXXXX.m3u"));
        QVERIFY(playlistFile.open());
        playlistFile.write("http://stream.example.com:8000/test\n");
        playlistFile.close();

        const QUrl playlistUrl = QUrl::fromLocalFile(playlistFile.fileName());

        QSignalSpy playListLoadedSpy(app.mediaPlayListProxyModel(), &MediaPlayListProxyModel::playListLoaded);
        app.openFiles({playlistUrl});

        QTRY_COMPARE(playListLoadedSpy.count(), 1);
#if KFKIO_FOUND
        QVERIFY(app.mediaPlayListProxyModel()->canOpenLoadedPlaylist());
#else
        QVERIFY(!app.mediaPlayListProxyModel()->canOpenLoadedPlaylist());
#endif
        QCOMPARE(app.mediaPlayListProxyModel()->rowCount(), 1);
    }

    void testOpenFilesKioPlaylist()
    {
#if QtDBus_FOUND
        ScopedMockKioFuse scopedKioFuse;
        if (!scopedKioFuse.isRegistered) {
            QSKIP("D-Bus session bus unavailable for mock KIOFuse service");
        }

        QTemporaryFile localPlaylist(QDir::tempPath() + QStringLiteral("/kioplaylist_XXXXXX.m3u"));
        QVERIFY(localPlaylist.open());
        localPlaylist.write("http://stream.example.com:8000/kio_test\n");
        localPlaylist.close();

        const QUrl remoteKioUrl(QStringLiteral("smb://nas/music/stream_playlist.m3u"));
        scopedKioFuse.mock.mountMappings[remoteKioUrl.toString()] = localPlaylist.fileName();

        ElisaApplication app;
        app.initialize();

        QSignalSpy playListLoadedSpy(app.mediaPlayListProxyModel(), &MediaPlayListProxyModel::playListLoaded);
        app.openFiles({remoteKioUrl});

        QTRY_COMPARE(playListLoadedSpy.count(), 1);
#if KFKIO_FOUND
        QVERIFY(app.mediaPlayListProxyModel()->canOpenLoadedPlaylist());
#else
        QVERIFY(!app.mediaPlayListProxyModel()->canOpenLoadedPlaylist());
#endif
        QCOMPARE(app.mediaPlayListProxyModel()->rowCount(), 1);
#endif
    }

    void testOpenFilesUnsupportedIgnored()
    {
        ElisaApplication app;
        app.initialize();

        QTemporaryFile textFile(QDir::tempPath() + QStringLiteral("/notes_XXXXXX.txt"));
        QVERIFY(textFile.open());
        textFile.write("plain text file\n");
        textFile.close();

        QSignalSpy enqueueSpy(&app, &ElisaApplication::enqueue);
        app.openFiles({QUrl::fromLocalFile(textFile.fileName())});

        QCOMPARE(enqueueSpy.count(), 0);
    }

    void testOpenFilesMixedList()
    {
        ElisaApplication app;
        app.initialize();

        QTemporaryFile audioFile(QDir::tempPath() + QStringLiteral("/song_XXXXXX.ogg"));
        QVERIFY(audioFile.open());

        const QUrl audioUrl = QUrl::fromLocalFile(audioFile.fileName());
        const QUrl streamUrl(QStringLiteral("http://stream.example.com/radio"));

        QSignalSpy enqueueSpy(&app, &ElisaApplication::enqueue);
        bool result = app.openFiles({streamUrl, audioUrl});

        QVERIFY(result);
        QCOMPARE(enqueueSpy.count(), 1);

        const auto entries = enqueueSpy.at(0).at(0).value<DataTypes::EntryDataList>();
        QCOMPARE(entries.count(), 2);
        QCOMPARE(entries.at(0).musicData[DataTypes::ElementTypeRole].value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Radio);
        QCOMPARE(entries.at(0).musicData[DataTypes::ResourceRole].toUrl(), streamUrl);

        QCOMPARE(entries.at(1).musicData[DataTypes::ElementTypeRole].value<ElisaUtils::PlayListEntryType>(), ElisaUtils::FileName);
        QCOMPARE(entries.at(1).musicData[DataTypes::ResourceRole].toUrl(), audioUrl);
    }
};

QTEST_MAIN(ElisaApplicationTests)

#include "elisaapplicationtest.moc"
