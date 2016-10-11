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

void PlayListControlerTest::testBringUpCase()
{
    PlayListControler myControler;
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    DatabaseInterface myDatabaseView;

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));
    myDatabaseContent.initDatabase();
    myDatabaseContent.initRequest();

    myDatabaseView.init(QStringLiteral("testDbDirectView"));
    myDatabaseView.initDatabase();
    myDatabaseView.initRequest();

    connect(&myDatabaseContent, &DatabaseInterface::databaseChanged,
            &myDatabaseView, &DatabaseInterface::databaseHasChanged);

    myPlayList.setDatabaseInterface(&myDatabaseView);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myControler.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.playListModel(), &myPlayList);

    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.isValidRole(), static_cast<int>(MediaPlayList::ColumnsRoles::IsValidRole));

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

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);
}

void PlayListControlerTest::testBringUpAndDownCase()
{
    PlayListControler myControler;
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    DatabaseInterface myDatabaseView;

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));
    myDatabaseContent.initDatabase();
    myDatabaseContent.initRequest();

    myDatabaseView.init(QStringLiteral("testDbDirectView"));
    myDatabaseView.initDatabase();
    myDatabaseView.initRequest();

    connect(&myDatabaseContent, &DatabaseInterface::databaseChanged,
            &myDatabaseView, &DatabaseInterface::databaseHasChanged);

    myPlayList.setDatabaseInterface(&myDatabaseView);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myControler.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.playListModel(), &myPlayList);

    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.isValidRole(), static_cast<int>(MediaPlayList::ColumnsRoles::IsValidRole));

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

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.removeRow(0);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex());
}

void PlayListControlerTest::testBringUpAndRemoveCase()
{
    PlayListControler myControler;

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.playListModel(), &myPlayList);

    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.isValidRole(), static_cast<int>(MediaPlayList::ColumnsRoles::IsValidRole));

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

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.removeRow(0);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex());
}

void PlayListControlerTest::testBringUpAndRemoveMultipleCase()
{
    PlayListControler myControler;

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.playListModel(), &myPlayList);

    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.isValidRole(), static_cast<int>(MediaPlayList::ColumnsRoles::IsValidRole));

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

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.removeRow(0);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void PlayListControlerTest::testBringUpAndRemoveMultipleNotBeginCase()
{
    PlayListControler myControler;

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.playListModel(), &myPlayList);

    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.isValidRole(), static_cast<int>(MediaPlayList::ColumnsRoles::IsValidRole));

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

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track4"), QStringLiteral("album2"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayList.removeRow(1);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
}

void PlayListControlerTest::testBringUpAndPlayCase()
{
    PlayListControler myControler;

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.playListModel(), &myPlayList);

    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.isValidRole(), static_cast<int>(MediaPlayList::ColumnsRoles::IsValidRole));

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

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
}

void PlayListControlerTest::testBringUpAndSkipNextCase()
{
    PlayListControler myControler;

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.playListModel(), &myPlayList);

    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.isValidRole(), static_cast<int>(MediaPlayList::ColumnsRoles::IsValidRole));

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

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
}

void PlayListControlerTest::testBringUpAndSkipPreviousCase()
{
    PlayListControler myControler;

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.playListModel(), &myPlayList);

    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.isValidRole(), static_cast<int>(MediaPlayList::ColumnsRoles::IsValidRole));

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

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myControler.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void PlayListControlerTest::finishPlayList()
{
    PlayListControler myControler;

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.playListModel(), &myPlayList);

    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.isValidRole(), static_cast<int>(MediaPlayList::ColumnsRoles::IsValidRole));

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

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void PlayListControlerTest::randomPlayList()
{
    PlayListControler myControler;

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.playListModel(), &myPlayList);

    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.isValidRole(), static_cast<int>(MediaPlayList::ColumnsRoles::IsValidRole));

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

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myControler.seedRandomGenerator(0);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track5"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControler.setRandomPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(randomPlayControlChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.randomPlay(), true);
    QCOMPARE(myControler.randomPlayControl(), true);

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(randomPlayControlChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(randomPlayControlChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(3, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(randomPlayControlChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(randomPlayControlChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void PlayListControlerTest::continuePlayList()
{
    PlayListControler myControler;

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.playListModel(), &myPlayList);

    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.isValidRole(), static_cast<int>(MediaPlayList::ColumnsRoles::IsValidRole));

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

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myControler.seedRandomGenerator(0);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track5"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControler.setRepeatPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.repeatPlay(), true);
    QCOMPARE(myControler.repeatPlayControl(), true);

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(3, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void PlayListControlerTest::testRestoreSettings()
{
    PlayListControler myControler;

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.playListModel(), &myPlayList);

    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.isValidRole(), static_cast<int>(MediaPlayList::ColumnsRoles::IsValidRole));

    QVariantMap settings;
    settings[QStringLiteral("currentTrack")] = 2;
    settings[QStringLiteral("randomPlay")] = true;
    settings[QStringLiteral("repeatPlay")] = true;

    myControler.setPersistentState(settings);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myControler.seedRandomGenerator(0);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

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

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track5"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));
}

void PlayListControlerTest::removeBeforeCurrentTrack()
{
    PlayListControler myControler;

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.playListModel(), &myPlayList);

    myControler.setIsValidRole(MediaPlayList::ColumnsRoles::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.isValidRole(), static_cast<int>(MediaPlayList::ColumnsRoles::IsValidRole));

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

    myDatabaseContent.insertTracksList(newTracks, newCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track5"), QStringLiteral("album2"), QStringLiteral("artist1")});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(3, 0)));

    myPlayList.removeRow(1);

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);
}


QTEST_MAIN(PlayListControlerTest)


#include "moc_playlistcontrolertest.cpp"
