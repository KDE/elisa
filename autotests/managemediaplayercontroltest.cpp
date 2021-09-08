/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "managemediaplayercontroltest.h"

#include "managemediaplayercontrol.h"
#include "mediaplaylist.h"
#include "databaseinterface.h"
#include "trackslistener.h"

#include <QHash>
#include <QString>
#include <QUrl>
#include <QVector>

#include <QStringListModel>

#include <QtTest>

ManageMediaPlayerControlTest::ManageMediaPlayerControlTest(QObject *parent) : QObject(parent)
{

}

void ManageMediaPlayerControlTest::initTestCase()
{
    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
    qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
}

void ManageMediaPlayerControlTest::simpleInitialCase()
{
    ManageMediaPlayerControl myControl;

    QVERIFY(!myControl.playControlEnabled());
    QVERIFY(!myControl.skipBackwardControlEnabled());
    QVERIFY(!myControl.skipForwardControlEnabled());
    QVERIFY(!myControl.musicPlaying());
    QVERIFY(!myControl.currentTrack().isValid());
}

void ManageMediaPlayerControlTest::testPlayingCase()
{
    ManageMediaPlayerControl myControl;

    QSignalSpy musicPlayingChangedSpy(&myControl, &ManageMediaPlayerControl::musicPlayingChanged);

    QVERIFY(!myControl.playControlEnabled());
    QVERIFY(!myControl.skipBackwardControlEnabled());
    QVERIFY(!myControl.skipForwardControlEnabled());
    QVERIFY(!myControl.musicPlaying());
    QVERIFY(!myControl.currentTrack().isValid());

    QCOMPARE(musicPlayingChangedSpy.count(), 0);

    myControl.playerPlaying();

    QCOMPARE(musicPlayingChangedSpy.count(), 1);

    QVERIFY(myControl.musicPlaying());

    myControl.playerPausedOrStopped();

    QCOMPARE(musicPlayingChangedSpy.count(), 2);

    QVERIFY(!myControl.musicPlaying());
}

void ManageMediaPlayerControlTest::testTracksCase()
{
    QStringListModel myPlayList;
    myPlayList.setStringList({QStringLiteral("first"), QStringLiteral("second"), QStringLiteral("third")});

    ManageMediaPlayerControl myControl;

    QSignalSpy playControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::playControlEnabledChanged);
    QSignalSpy skipBackwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipBackwardControlEnabledChanged);
    QSignalSpy skipForwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipForwardControlEnabledChanged);
    QSignalSpy previousTrackChangedSpy(&myControl, &ManageMediaPlayerControl::previousTrackChanged);
    QSignalSpy currentTrackChangedSpy(&myControl, &ManageMediaPlayerControl::currentTrackChanged);
    QSignalSpy nextTrackChangedSpy(&myControl, &ManageMediaPlayerControl::nextTrackChanged);

    QVERIFY(!myControl.playControlEnabled());
    QVERIFY(!myControl.skipBackwardControlEnabled());
    QVERIFY(!myControl.skipForwardControlEnabled());
    QVERIFY(!myControl.musicPlaying());
    QVERIFY(!myControl.currentTrack().isValid());

    myControl.setCurrentTrack(QPersistentModelIndex(myPlayList.index(1, 0)));

    QCOMPARE(previousTrackChangedSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(nextTrackChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QVERIFY(myControl.playControlEnabled());
    QVERIFY(!myControl.skipBackwardControlEnabled());
    QVERIFY(!myControl.skipForwardControlEnabled());

    myControl.setPreviousTrack(QPersistentModelIndex(myPlayList.index(0, 0)));

    QCOMPARE(previousTrackChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(nextTrackChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QVERIFY(myControl.playControlEnabled());
    QVERIFY(myControl.skipBackwardControlEnabled());
    QVERIFY(!myControl.skipForwardControlEnabled());

    myControl.playerPlaying();

    QCOMPARE(previousTrackChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(nextTrackChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QVERIFY(myControl.playControlEnabled());
    QVERIFY(myControl.skipBackwardControlEnabled());
    QVERIFY(!myControl.skipForwardControlEnabled());

    myControl.playerPausedOrStopped();
    myControl.setNextTrack(QPersistentModelIndex(myPlayList.index(2, 0)));

    QCOMPARE(previousTrackChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(nextTrackChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QVERIFY(myControl.playControlEnabled());
    QVERIFY(myControl.skipBackwardControlEnabled());
    QVERIFY(myControl.skipForwardControlEnabled());

    myControl.playerPlaying();

    QCOMPARE(previousTrackChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(nextTrackChangedSpy.count(), 1);
    QCOMPARE(nextTrackChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QVERIFY(myControl.playControlEnabled());
    QVERIFY(myControl.skipBackwardControlEnabled());
    QVERIFY(myControl.skipForwardControlEnabled());
}

QTEST_GUILESS_MAIN(ManageMediaPlayerControlTest)


#include "moc_managemediaplayercontroltest.cpp"
