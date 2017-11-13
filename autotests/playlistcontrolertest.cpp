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

#include "playlistcontrolertest.h"

#include "playlistcontroler.h"
#include "mediaplaylist.h"
#include "databaseinterface.h"
#include "musicalbum.h"
#include "trackslistener.h"

#include <QtTest>

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
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void PlayListControlerTest::testBringUpCaseFromNewAlbum()
{
    PlayListControler myControler;
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                             QStringLiteral("album2"), 1, 2);
    myPlayList.enqueue(newTrackID);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void PlayListControlerTest::testBringUpAndDownCase()
{
    PlayListControler myControler;
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myControler, &PlayListControler::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControler, &PlayListControler::playListModelChanged);
    QSignalSpy isValidRoleChangedSpy(&myControler, &PlayListControler::isValidRoleChanged);
    QSignalSpy randomPlayChangedSpy(&myControler, &PlayListControler::randomPlayChanged);
    QSignalSpy randomPlayControlChangedSpy(&myControler, &PlayListControler::randomPlayControlChanged);
    QSignalSpy repeatPlayChangedSpy(&myControler, &PlayListControler::repeatPlayChanged);
    QSignalSpy repeatPlayControlChangedSpy(&myControler, &PlayListControler::repeatPlayControlChanged);
    QSignalSpy playListFinishedSpy(&myControler, &PlayListControler::playListFinished);

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2});

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1});

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

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

void PlayListControlerTest::testBringUpAndSkipPreviousAndContinueCase()
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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myControler.setRepeatPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album1"), 3, 3});
    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1});
    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

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

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(4, 0)));

    myControler.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 6);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(5, 0)));

    myControler.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 7);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(4, 0)));

    myControler.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 8);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(3, 0)));

    myControler.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 9);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));

    myControler.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 10);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myControler.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 11);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControler.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 12);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(5, 0)));
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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

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

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album1"), 3, 3});
    myPlayList.enqueue({QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

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

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album1"), 3, 3});
    myPlayList.enqueue({QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album1"), 3, 3});
    myPlayList.enqueue({QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album1"), 3, 3});
    myPlayList.enqueue({QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

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

void PlayListControlerTest::switchToTrackTest()
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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album1"), 3, 3});
    myPlayList.enqueue({QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 4});
    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myControler.switchTo(4);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(4, 0)));
}

void PlayListControlerTest::singleTrack()
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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

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

void PlayListControlerTest::testBringUpAndRemoveLastCase()
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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1});

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

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

    myPlayList.removeRow(2);

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(randomPlayControlChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayControlChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myControler.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}


QTEST_GUILESS_MAIN(PlayListControlerTest)


#include "moc_playlistcontrolertest.cpp"
