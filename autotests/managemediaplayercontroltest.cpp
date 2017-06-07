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

#include "managemediaplayercontroltest.h"

#include "managemediaplayercontrol.h"
#include "mediaplaylist.h"
#include "databaseinterface.h"
#include "musicaudiotrack.h"
#include "trackslistener.h"

#include <QHash>
#include <QString>
#include <QUrl>
#include <QVector>
#include <QVariant>
#include <QList>

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

    QCOMPARE(myControl.playControlEnabled(), false);
    QCOMPARE(myControl.skipBackwardControlEnabled(), false);
    QCOMPARE(myControl.skipForwardControlEnabled(), false);
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
    QCOMPARE(myControl.musicPlaying(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
    QCOMPARE(myControl.currentTrack().isValid(), false);

    QSignalSpy playControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::playControlEnabledChanged);
    QSignalSpy skipBackwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipBackwardControlEnabledChanged);
    QSignalSpy skipForwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipForwardControlEnabledChanged);
    QSignalSpy musicPlayingChangedSpy(&myControl, &ManageMediaPlayerControl::musicPlayingChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageMediaPlayerControl::playListModelChanged);
    QSignalSpy currentTrackChangedSpy(&myControl, &ManageMediaPlayerControl::currentTrackChanged);

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    myPlayList.setStringList({QStringLiteral("tutu")});

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.playControlEnabled(), true);
    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControl.playerPlaying();

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.musicPlaying(), true);

    QCOMPARE(myPlayList.insertRows(1, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipForwardControlEnabled(), true);

    QCOMPARE(myPlayList.insertRows(0, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipBackwardControlEnabled(), true);

    QCOMPARE(myPlayList.removeRows(3, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipForwardControlEnabled(), false);

    QCOMPARE(myPlayList.removeRows(0, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipBackwardControlEnabled(), false);

    QCOMPARE(myPlayList.removeRows(0, 1, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);

    QCOMPARE(myControl.playControlEnabled(), false);
    QCOMPARE(myControl.currentTrack().isValid(), false);

    myControl.playerStopped();

    QCOMPARE(playControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
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
    QCOMPARE(myControl.musicPlaying(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
    QCOMPARE(myControl.currentTrack().isValid(), false);

    QSignalSpy playControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::playControlEnabledChanged);
    QSignalSpy skipBackwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipBackwardControlEnabledChanged);
    QSignalSpy skipForwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipForwardControlEnabledChanged);
    QSignalSpy musicPlayingChangedSpy(&myControl, &ManageMediaPlayerControl::musicPlayingChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageMediaPlayerControl::playListModelChanged);
    QSignalSpy currentTrackChangedSpy(&myControl, &ManageMediaPlayerControl::currentTrackChanged);

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    myPlayList.setStringList({QStringLiteral("tutu")});

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.playControlEnabled(), true);
    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControl.playerPlaying();

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.musicPlaying(), true);

    QCOMPARE(myPlayList.insertRows(1, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipForwardControlEnabled(), true);

    QCOMPARE(myPlayList.insertRows(0, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipBackwardControlEnabled(), true);

    QCOMPARE(myPlayList.removeRows(2, 1, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
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
    QCOMPARE(musicPlayingChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);

    QCOMPARE(myControl.musicPlaying(), false);

    QCOMPARE(myPlayList.removeRows(0, 4, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(musicPlayingChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);
}
void ManageMediaPlayerControlTest::nextAndPreviousButtonAndRandomPlay()
{
    QStringListModel myPlayList;

    ManageMediaPlayerControl myControl;

    QCOMPARE(myControl.playControlEnabled(), false);
    QCOMPARE(myControl.skipBackwardControlEnabled(), false);
    QCOMPARE(myControl.skipForwardControlEnabled(), false);
    QCOMPARE(myControl.musicPlaying(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
    QCOMPARE(myControl.currentTrack().isValid(), false);

    QSignalSpy playControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::playControlEnabledChanged);
    QSignalSpy skipBackwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipBackwardControlEnabledChanged);
    QSignalSpy skipForwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipForwardControlEnabledChanged);
    QSignalSpy musicPlayingChangedSpy(&myControl, &ManageMediaPlayerControl::musicPlayingChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageMediaPlayerControl::playListModelChanged);
    QSignalSpy currentTrackChangedSpy(&myControl, &ManageMediaPlayerControl::currentTrackChanged);

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    myPlayList.setStringList({QStringLiteral("tutu")});

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.playControlEnabled(), true);
    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControl.setRandomOrContinuePlay(true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    myControl.playerPlaying();

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.musicPlaying(), true);
    QCOMPARE(myControl.skipForwardControlEnabled(), true);
    QCOMPARE(myControl.skipBackwardControlEnabled(), true);

    QCOMPARE(myPlayList.insertRows(1, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myPlayList.insertRows(0, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myPlayList.removeRows(3, 2, {}), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);
}

void ManageMediaPlayerControlTest::moveCurrentTrack()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::albumAdded,
            &myPlayList, &MediaPlayList::albumAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByIdInList,
            &myListener, &TracksListener::trackByIdInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newArtistInList,
            &myListener, &TracksListener::newArtistInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    ManageMediaPlayerControl myControl;

    QCOMPARE(myControl.playControlEnabled(), false);
    QCOMPARE(myControl.skipBackwardControlEnabled(), false);
    QCOMPARE(myControl.skipForwardControlEnabled(), false);
    QCOMPARE(myControl.musicPlaying(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
    QCOMPARE(myControl.currentTrack().isValid(), false);

    QSignalSpy playControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::playControlEnabledChanged);
    QSignalSpy skipBackwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipBackwardControlEnabledChanged);
    QSignalSpy skipForwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipForwardControlEnabledChanged);
    QSignalSpy musicPlayingChangedSpy(&myControl, &ManageMediaPlayerControl::musicPlayingChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageMediaPlayerControl::playListModelChanged);
    QSignalSpy currentTrackChangedSpy(&myControl, &ManageMediaPlayerControl::currentTrackChanged);

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    auto newTracks = QList<MusicAudioTrack>();
    auto newCovers = QHash<QString, QUrl>();

    newTracks = {
        {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$1"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$1"))}, 1},
        {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$2"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$2"))}, 2},
        {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$3"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$3"))}, 3},
        {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$4"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$4"))}, 4},
        {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$5"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$5"))}, 5},
        {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$6"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 1},
        {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$7"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$7"))}, 2},
        {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$8"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$8"))}, 3},
        {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$9"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$9"))}, 4},
        {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$10"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$10"))}, 5}
    };

    newCovers[QStringLiteral("album1")] = QUrl::fromLocalFile(QStringLiteral("album1"));
    newCovers[QStringLiteral("album2")] = QUrl::fromLocalFile(QStringLiteral("album2"));

    myDatabaseContent.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.playControlEnabled(), true);
    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControl.playerPlaying();

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.musicPlaying(), true);
    QCOMPARE(myControl.skipForwardControlEnabled(), true);

    QCOMPARE(myPlayList.moveRows({}, 0, 1, {}, 2), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipForwardControlEnabled(), false);
    QCOMPARE(myControl.skipBackwardControlEnabled(), true);
}

void ManageMediaPlayerControlTest::moveAnotherTrack()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::albumAdded,
            &myPlayList, &MediaPlayList::albumAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByIdInList,
            &myListener, &TracksListener::trackByIdInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newArtistInList,
            &myListener, &TracksListener::newArtistInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    ManageMediaPlayerControl myControl;

    QCOMPARE(myControl.playControlEnabled(), false);
    QCOMPARE(myControl.skipBackwardControlEnabled(), false);
    QCOMPARE(myControl.skipForwardControlEnabled(), false);
    QCOMPARE(myControl.musicPlaying(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
    QCOMPARE(myControl.currentTrack().isValid(), false);

    QSignalSpy playControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::playControlEnabledChanged);
    QSignalSpy skipBackwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipBackwardControlEnabledChanged);
    QSignalSpy skipForwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipForwardControlEnabledChanged);
    QSignalSpy musicPlayingChangedSpy(&myControl, &ManageMediaPlayerControl::musicPlayingChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageMediaPlayerControl::playListModelChanged);
    QSignalSpy currentTrackChangedSpy(&myControl, &ManageMediaPlayerControl::currentTrackChanged);

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    auto newTracks = QList<MusicAudioTrack>();
    auto newCovers = QHash<QString, QUrl>();

    newTracks = {
        {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$1"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$1"))}, 1},
        {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$2"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$2"))}, 2},
        {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$3"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$3"))}, 3},
        {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$4"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$4"))}, 4},
        {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$5"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$5"))}, 5},
        {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$6"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 1},
        {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$7"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$7"))}, 2},
        {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$8"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$8"))}, 3},
        {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$9"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$9"))}, 4},
        {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$10"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$10"))}, 5}
    };

    newCovers[QStringLiteral("album1")] = QUrl::fromLocalFile(QStringLiteral("album1"));
    newCovers[QStringLiteral("album2")] = QUrl::fromLocalFile(QStringLiteral("album2"));

    myDatabaseContent.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.playControlEnabled(), true);
    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControl.playerPlaying();

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.musicPlaying(), true);
    QCOMPARE(myControl.skipForwardControlEnabled(), true);

    QCOMPARE(myPlayList.moveRows({}, 1, 1, {}, 0), true);

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 2);
    QCOMPARE(musicPlayingChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.skipForwardControlEnabled(), false);
    QCOMPARE(myControl.skipBackwardControlEnabled(), true);
}

void ManageMediaPlayerControlTest::setCurrentTrackTest()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::albumAdded,
            &myPlayList, &MediaPlayList::albumAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByIdInList,
            &myListener, &TracksListener::trackByIdInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newArtistInList,
            &myListener, &TracksListener::newArtistInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    ManageMediaPlayerControl myControl;

    QCOMPARE(myControl.playControlEnabled(), false);
    QCOMPARE(myControl.skipBackwardControlEnabled(), false);
    QCOMPARE(myControl.skipForwardControlEnabled(), false);
    QCOMPARE(myControl.musicPlaying(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
    QCOMPARE(myControl.currentTrack().isValid(), false);

    QSignalSpy playControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::playControlEnabledChanged);
    QSignalSpy skipBackwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipBackwardControlEnabledChanged);
    QSignalSpy skipForwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipForwardControlEnabledChanged);
    QSignalSpy musicPlayingChangedSpy(&myControl, &ManageMediaPlayerControl::musicPlayingChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageMediaPlayerControl::playListModelChanged);
    QSignalSpy currentTrackChangedSpy(&myControl, &ManageMediaPlayerControl::currentTrackChanged);

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    auto newTracks = QList<MusicAudioTrack>();
    auto newCovers = QHash<QString, QUrl>();

    newTracks = {
        {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$1"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$1"))}, 1},
        {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$2"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$2"))}, 2},
        {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$3"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$3"))}, 3},
        {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$4"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$4"))}, 4},
        {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$5"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$5"))}, 5},
        {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$6"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 1},
        {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$7"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$7"))}, 2},
        {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$8"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$8"))}, 3},
        {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$9"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$9"))}, 4},
        {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$10"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$10"))}, 5}
    };

    newCovers[QStringLiteral("album1")] = QUrl::fromLocalFile(QStringLiteral("album1"));
    newCovers[QStringLiteral("album2")] = QUrl::fromLocalFile(QStringLiteral("album2"));

    myDatabaseContent.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myControl.playControlEnabled(), true);
    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(playControlEnabledChangedSpy.count(), 1);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);
}

void ManageMediaPlayerControlTest::setPlayListModelTest()
{
    QStringListModel myPlayList;

    ManageMediaPlayerControl myControl;

    QCOMPARE(myControl.playControlEnabled(), false);
    QCOMPARE(myControl.skipBackwardControlEnabled(), false);
    QCOMPARE(myControl.skipForwardControlEnabled(), false);
    QCOMPARE(myControl.musicPlaying(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
    QCOMPARE(myControl.currentTrack().isValid(), false);

    QSignalSpy playControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::playControlEnabledChanged);
    QSignalSpy skipBackwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipBackwardControlEnabledChanged);
    QSignalSpy skipForwardControlEnabledChangedSpy(&myControl, &ManageMediaPlayerControl::skipForwardControlEnabledChanged);
    QSignalSpy musicPlayingChangedSpy(&myControl, &ManageMediaPlayerControl::musicPlayingChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageMediaPlayerControl::playListModelChanged);
    QSignalSpy currentTrackChangedSpy(&myControl, &ManageMediaPlayerControl::currentTrackChanged);

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    myControl.setPlayListModel(nullptr);

    QCOMPARE(playControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipBackwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(skipForwardControlEnabledChangedSpy.count(), 0);
    QCOMPARE(musicPlayingChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 2);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), static_cast<QAbstractItemModel*>(nullptr));
}


QTEST_MAIN(ManageMediaPlayerControlTest)


#include "moc_managemediaplayercontroltest.cpp"
