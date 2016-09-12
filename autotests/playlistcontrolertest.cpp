/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "playlistcontrolertest.h"

#include "playlistcontroler.h"
#include "mediaplaylist.h"
#include "databaseinterface.h"
#include "musicalbum.h"
#include "musicaudiotrack.h"

#include <QVariant>
#include <QList>

#include <QtTest/QtTest>

PlayListControlerTest::PlayListControlerTest(QObject *parent) : QObject(parent)
{

}

void PlayListControlerTest::initTestCase()
{
    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>");
    qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
    qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
}

void PlayListControlerTest::testRestorePersistentSettingsFromInvalidState()
{
    PlayListControler myControler;

    auto oneSetting = QVariantMap();

    oneSetting[QStringLiteral("currentTrack")] = 5;

    myControler.setPersistentState(oneSetting);
}

void PlayListControlerTest::testRestorePersistentSettingsDirect()
{
    PlayListControler myControler;
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    DatabaseInterface myDatabaseView;

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));
    myDatabaseContent.initDatabase();
    myDatabaseContent.initRequest();

    myDatabaseView.init(QStringLiteral("testDbDirectView"));
    myDatabaseView.initDatabase();
    myDatabaseView.initRequest();

    connect(&myDatabaseContent, &DatabaseInterface::databaseChanged,
            &myDatabaseView, &DatabaseInterface::databaseHasChanged);

    myPlayList.setDatabaseInterface(&myDatabaseView);

    myControler.setPlayListModel(&myPlayList);
    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    auto newTracks = QHash<QString, QVector<MusicAudioTrack>>();
    auto newCovers = QHash<QString, QUrl>();

    newTracks[QStringLiteral("album1")] = {
        {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$1"))}},
        {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$2"))}},
        {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$3"))}},
        {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$4"))}},
    };

    newTracks[QStringLiteral("album2")] = {
        {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$5"))}},
        {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$6"))}},
        {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$7"))}},
        {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$8"))}},
        {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
            QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$9"))}},
        {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
            QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$10"))}}
    };

    newCovers[QStringLiteral("album1")] = QUrl::fromLocalFile(QStringLiteral("album1"));
    newCovers[QStringLiteral("album2")] = QUrl::fromLocalFile(QStringLiteral("album2"));

    QSignalSpy playerSourceChangedSpy(&myControler, &PlayListControler::playerSourceChanged);
    QSignalSpy currentTrackPositionChangedSpy(&myControler, &PlayListControler::currentTrackPositionChanged);
    QSignalSpy artistChangedSpy(&myControler, &PlayListControler::artistChanged);
    QSignalSpy titleChangedSpy(&myControler, &PlayListControler::titleChanged);
    QSignalSpy albumChangedSpy(&myControler, &PlayListControler::albumChanged);
    QSignalSpy imageChangedSpy(&myControler, &PlayListControler::imageChanged);
    QSignalSpy remainingTracksChangedSpy(&myControler, &PlayListControler::remainingTracksChanged);
    QSignalSpy skipBackwardControlEnabledChangedSpy(&myControler, &PlayListControler::skipBackwardControlEnabledChanged);
    QSignalSpy skipForwardControlEnabledChangedSpy(&myControler, &PlayListControler::skipForwardControlEnabledChanged);

    QSignalSpy playMusicSpy(&myControler, &PlayListControler::playMusic);
    QSignalSpy pauseMusicSpy(&myControler, &PlayListControler::pauseMusic);
    QSignalSpy stopMusicSpy(&myControler, &PlayListControler::stopMusic);

    auto oneSettingControler = QVariantMap();

    oneSettingControler[QStringLiteral("currentTrack")] = 4;
    oneSettingControler[QStringLiteral("playControlPosition")] = 500;
    oneSettingControler[QStringLiteral("randomPlay")] = true;
    oneSettingControler[QStringLiteral("repeatPlay")] = false;
    oneSettingControler[QStringLiteral("playerState")] = QVariant::fromValue(PlayListControler::PlayerState::Playing);

    auto playListSetting = QList<QVariant>();

    playListSetting.push_back(QVariant::fromValue<QStringList>({QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1")}));
    playListSetting.push_back(QVariant::fromValue<QStringList>({QStringLiteral("track4"), QStringLiteral("album1"), QStringLiteral("artist1")}));
    playListSetting.push_back(QVariant::fromValue<QStringList>({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")}));
    playListSetting.push_back(QVariant::fromValue<QStringList>({QStringLiteral("track4"), QStringLiteral("album2"), QStringLiteral("artist1")}));
    playListSetting.push_back(QVariant::fromValue<QStringList>({QStringLiteral("track6"), QStringLiteral("album2"), QStringLiteral("artist1")}));

    myPlayList.setPersistentState(playListSetting);

    myControler.setPersistentState(oneSettingControler);

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(currentTrackPositionChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);

    QCOMPARE(playMusicSpy.count(), 1);
    QCOMPARE(pauseMusicSpy.count(), 0);
    QCOMPARE(stopMusicSpy.count(), 0);

    QCOMPARE(myControler.currentTrackPosition(), 4);
}

void PlayListControlerTest::testRestorePersistentSettingsIndirect()
{
    PlayListControler myControler;
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    DatabaseInterface myDatabaseView;

    myDatabaseContent.init(QStringLiteral("testDbIndirectContent"));
    myDatabaseContent.initDatabase();
    myDatabaseContent.initRequest();

    myDatabaseView.init(QStringLiteral("testDbIndirectView"));
    myDatabaseView.initDatabase();
    myDatabaseView.initRequest();

    connect(&myDatabaseContent, &DatabaseInterface::databaseChanged,
            &myDatabaseView, &DatabaseInterface::databaseHasChanged);

    myPlayList.setDatabaseInterface(&myDatabaseView);

    myControler.setPlayListModel(&myPlayList);
    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    auto newTracks = QHash<QString, QVector<MusicAudioTrack>>();
    auto newCovers = QHash<QString, QUrl>();

    newTracks[QStringLiteral("album1")] = {
        {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$1"))}},
        {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$2"))}},
        {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$3"))}},
        {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$4"))}},
    };

    newTracks[QStringLiteral("album2")] = {
        {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$5"))}},
        {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$6"))}},
        {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$7"))}},
        {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$8"))}},
        {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
            QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$9"))}},
        {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
            QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$10"))}}
    };

    newCovers[QStringLiteral("album1")] = QUrl::fromLocalFile(QStringLiteral("album1"));
    newCovers[QStringLiteral("album2")] = QUrl::fromLocalFile(QStringLiteral("album2"));

    QSignalSpy playerSourceChangedSpy(&myControler, &PlayListControler::playerSourceChanged);
    QSignalSpy currentTrackPositionChangedSpy(&myControler, &PlayListControler::currentTrackPositionChanged);
    QSignalSpy artistChangedSpy(&myControler, &PlayListControler::artistChanged);
    QSignalSpy titleChangedSpy(&myControler, &PlayListControler::titleChanged);
    QSignalSpy albumChangedSpy(&myControler, &PlayListControler::albumChanged);
    QSignalSpy imageChangedSpy(&myControler, &PlayListControler::imageChanged);
    QSignalSpy remainingTracksChangedSpy(&myControler, &PlayListControler::remainingTracksChanged);
    QSignalSpy skipBackwardControlEnabledChangedSpy(&myControler, &PlayListControler::skipBackwardControlEnabledChanged);
    QSignalSpy skipForwardControlEnabledChangedSpy(&myControler, &PlayListControler::skipForwardControlEnabledChanged);

    QSignalSpy playMusicSpy(&myControler, &PlayListControler::playMusic);
    QSignalSpy pauseMusicSpy(&myControler, &PlayListControler::pauseMusic);
    QSignalSpy stopMusicSpy(&myControler, &PlayListControler::stopMusic);

    auto oneSettingControler = QVariantMap();

    oneSettingControler[QStringLiteral("currentTrack")] = 4;
    oneSettingControler[QStringLiteral("playControlPosition")] = 500;
    oneSettingControler[QStringLiteral("randomPlay")] = true;
    oneSettingControler[QStringLiteral("repeatPlay")] = false;
    oneSettingControler[QStringLiteral("playerState")] = QVariant::fromValue(PlayListControler::PlayerState::Playing);

    auto playListSetting = QList<QVariant>();

    playListSetting.push_back(QVariant::fromValue<QStringList>({QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1")}));
    playListSetting.push_back(QVariant::fromValue<QStringList>({QStringLiteral("track4"), QStringLiteral("album1"), QStringLiteral("artist1")}));
    playListSetting.push_back(QVariant::fromValue<QStringList>({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")}));
    playListSetting.push_back(QVariant::fromValue<QStringList>({QStringLiteral("track4"), QStringLiteral("album2"), QStringLiteral("artist1")}));
    playListSetting.push_back(QVariant::fromValue<QStringList>({QStringLiteral("track6"), QStringLiteral("album2"), QStringLiteral("artist1")}));

    myControler.setPersistentState(oneSettingControler);

    myPlayList.setPersistentState(playListSetting);

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(currentTrackPositionChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);

    QCOMPARE(playMusicSpy.count(), 1);
    QCOMPARE(pauseMusicSpy.count(), 0);
    QCOMPARE(stopMusicSpy.count(), 0);

    QCOMPARE(myControler.currentTrackPosition(), 4);
}

QTEST_MAIN(PlayListControlerTest)


#include "moc_playlistcontrolertest.cpp"
