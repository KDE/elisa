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

#include "managemediaplayercontroltest.h"

#include "managemediaplayercontrol.h"
#include "mediaplaylist.h"
#include "databaseinterface.h"
#include "musicalbum.h"
#include "musicaudiotrack.h"

#include <QVariant>
#include <QList>

#include <QStringListModel>

#include <QtTest/QtTest>

ManageMediaPlayerControlTest::ManageMediaPlayerControlTest(QObject *parent) : QObject(parent)
{

}

void ManageMediaPlayerControlTest::initTestCase()
{
    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>");
    qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
    qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
}

void ManageMediaPlayerControlTest::simpleInitialCase()
{
    ManageMediaPlayerControl myControl;

    QCOMPARE(myControl.playControlEnabled(), false);
    QCOMPARE(myControl.skipBackwardControlEnabled(), false);
    QCOMPARE(myControl.skipForwardControlEnabled(), false);
    QCOMPARE(myControl.playControlPosition(), 0);
    QCOMPARE(myControl.musicPlaying(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
    QCOMPARE(myControl.currentTrack().isValid(), false);
}

void ManageMediaPlayerControlTest::addItemInModelCase()
{
    QStringListModel myPlayList;

    ManageMediaPlayerControl myControl;

    QCOMPARE(myControl.playControlEnabled(), false);
    QCOMPARE(myControl.skipBackwardControlEnabled(), false);
    QCOMPARE(myControl.skipForwardControlEnabled(), false);
    QCOMPARE(myControl.playControlPosition(), 0);
    QCOMPARE(myControl.musicPlaying(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
    QCOMPARE(myControl.currentTrack().isValid(), false);

    QSignalSpy playControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::playControlEnabledChanged);
    QSignalSpy skipBackwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipBackwardControlEnabledChanged);
    QSignalSpy skipForwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipForwardControlEnabledChanged);
    QSignalSpy playControlPositionChangedSpy(&myControl, &ManageMediaPlayerControl::playControlPositionChanged);
    QSignalSpy musicPlayingChangedSpy(&myControl, &ManageMediaPlayerControl::musicPlayingChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageMediaPlayerControl::playListModelChanged);
    QSignalSpy currentTrackChangedSpy(&myControl, &ManageMediaPlayerControl::currentTrackChanged);

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    myPlayList.setStringList({QStringLiteral("tutu")});

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.playControlEnabled(), true);
    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControl.playerPlaying();

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.musicPlaying(), true);

    QCOMPARE(myPlayList.insertRows(1, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipForwardControlEnabled(), true);

    QCOMPARE(myPlayList.insertRows(0, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipBackwardControlEnabled(), true);

    QCOMPARE(myPlayList.removeRows(3, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipForwardControlEnabled(), false);

    QCOMPARE(myPlayList.removeRows(0, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipBackwardControlEnabled(), false);

    QCOMPARE(myPlayList.removeRows(0, 1, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);

    QCOMPARE(myControl.playControlEnabled(), false);
    QCOMPARE(myControl.currentTrack().isValid(), false);

    myControl.playerStopped();

    QCOMPARE(playControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);

    QCOMPARE(myControl.musicPlaying(), false);
}

void ManageMediaPlayerControlTest::addItemInModelCaseV2()
{
    QStringListModel myPlayList;

    ManageMediaPlayerControl myControl;

    QCOMPARE(myControl.playControlEnabled(), false);
    QCOMPARE(myControl.skipBackwardControlEnabled(), false);
    QCOMPARE(myControl.skipForwardControlEnabled(), false);
    QCOMPARE(myControl.playControlPosition(), 0);
    QCOMPARE(myControl.musicPlaying(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
    QCOMPARE(myControl.currentTrack().isValid(), false);

    QSignalSpy playControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::playControlEnabledChanged);
    QSignalSpy skipBackwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipBackwardControlEnabledChanged);
    QSignalSpy skipForwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipForwardControlEnabledChanged);
    QSignalSpy playControlPositionChangedSpy(&myControl, &ManageMediaPlayerControl::playControlPositionChanged);
    QSignalSpy musicPlayingChangedSpy(&myControl, &ManageMediaPlayerControl::musicPlayingChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageMediaPlayerControl::playListModelChanged);
    QSignalSpy currentTrackChangedSpy(&myControl, &ManageMediaPlayerControl::currentTrackChanged);

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    myPlayList.setStringList({QStringLiteral("tutu")});

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.playControlEnabled(), true);
    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControl.playerPlaying();

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.musicPlaying(), true);

    QCOMPARE(myPlayList.insertRows(1, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipForwardControlEnabled(), true);

    QCOMPARE(myPlayList.insertRows(0, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipBackwardControlEnabled(), true);

    QCOMPARE(myPlayList.removeRows(2, 1, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);

    QCOMPARE(myControl.playControlEnabled(), false);
    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.skipBackwardControlEnabled(), false);
    QCOMPARE(myControl.skipForwardControlEnabled(), false);

    myControl.playerStopped();

    QCOMPARE(playControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);

    QCOMPARE(myControl.musicPlaying(), false);

    QCOMPARE(myPlayList.removeRows(0, 4, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(playControlPositionChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);
}

QTEST_MAIN(ManageMediaPlayerControlTest)


#include "moc_managemediaplayercontroltest.cpp"
