/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "mediaplaylisttest.h"
#include "mediaplaylisttestconfig.h"

#include "mediaplaylist.h"
#include "databaseinterface.h"
#include "musicaudiotrack.h"
#include "trackslistener.h"

#include <QtTest>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QUrl>
#include <QTime>
#include <QTemporaryFile>
#include <QAbstractItemModelTester>

MediaPlayListTest::MediaPlayListTest(QObject *parent) : QObject(parent)
{
}

void MediaPlayListTest::initTestCase()
{
    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>");
    qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
    qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
    qRegisterMetaType<ElisaUtils::PlayListEntryType>("PlayListEntryType");
}

void MediaPlayListTest::simpleInitialCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
    myPlayList.enqueue({newTrackID, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
}

void MediaPlayListTest::enqueueAlbumCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/")),
                        QStringLiteral("album2")},
                       ElisaUtils::Album);

    QVERIFY(dataChangedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 7);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 8);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 9);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 10);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
}

void MediaPlayListTest::clearPlayListCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy tracksCountChangedSpy(&myPlayList, &MediaPlayList::tracksCountChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(tracksCountChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(tracksCountChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/")),
                        QStringLiteral("album2")},
                       ElisaUtils::Album);

    QVERIFY(dataChangedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(tracksCountChangedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 7);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 8);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 9);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 10);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.clearPlayList();

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(tracksCountChangedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);

    QCOMPARE(myPlayList.rowCount(), 0);

    QCOMPARE(myPlayList.currentTrack().isValid(), false);
}

void MediaPlayListTest::undoClearPlayListCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy tracksCountChangedSpy(&myPlayList, &MediaPlayList::tracksCountChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy displayUndoInlineSpy(&myPlayList, &MediaPlayList::displayUndoInline);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(tracksCountChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(tracksCountChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/")),
                        QStringLiteral("album2")},
                       ElisaUtils::Album);

    QVERIFY(dataChangedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(tracksCountChangedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 7);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 8);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 9);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 10);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.clearPlayList();

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(tracksCountChangedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);

    QCOMPARE(myPlayList.rowCount(), 0);

    QCOMPARE(myPlayList.currentTrack().isValid(), false);

    myPlayList.undoClearPlayList();

    QVERIFY(dataChangedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(tracksCountChangedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 8);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 7);
    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(displayUndoInlineSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 7);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 8);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 9);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 10);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void MediaPlayListTest::undoReplacePlayListCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy tracksCountChangedSpy(&myPlayList, &MediaPlayList::tracksCountChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy displayUndoInlineSpy(&myPlayList, &MediaPlayList::displayUndoInline);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(tracksCountChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(tracksCountChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/")),
                        QStringLiteral("album2")},
                       ElisaUtils::Album);

    QVERIFY(dataChangedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(tracksCountChangedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 7);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 8);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 9);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 10);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.enqueue({myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/")),
                        QStringLiteral("album1")},
                       ElisaUtils::Album,
                       ElisaUtils::ReplacePlayList,
                       ElisaUtils::TriggerPlay);

    QVERIFY(dataChangedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 4);

    myPlayList.undoClearPlayList();

    QVERIFY(dataChangedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 5);
    QCOMPARE(rowsRemovedSpy.count(), 2);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 5);
    QCOMPARE(tracksCountChangedSpy.count(), 7);
    QCOMPARE(persistentStateChangedSpy.count(), 7);
    QCOMPARE(dataChangedSpy.count(), 9);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 8);
    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(displayUndoInlineSpy.count(), 2);

    QCOMPARE(myPlayList.rowCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 7);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 8);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 9);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 10);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void MediaPlayListTest::enqueueArtistCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueue({0, QStringLiteral("artist1")}, ElisaUtils::Artist);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 1);

    QCOMPARE(rowsAboutToBeInsertedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumIdRole).toULongLong(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 7);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 8);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 9);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
}

void MediaPlayListTest::removeFirstTrackOfAlbum()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/")),
                        QStringLiteral("album2")},
                       ElisaUtils::Album);

    QVERIFY(dataChangedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    myPlayList.removeRow(0);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
}

void MediaPlayListTest::testHasHeader()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueue({firstTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    myPlayList.enqueue({secondTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueue({thirdTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    myPlayList.enqueue({fourthTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    myPlayList.removeRows(2, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);
}

void MediaPlayListTest::testHasHeaderWithRestore()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContentHeaderWithRestore"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myPlayList.enqueueRestoredEntry({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 1);
    QCOMPARE(newEntryInListSpy.count(), 0);

    QCOMPARE(newEntryInListSpy.wait(50), false);

    myPlayList.enqueueRestoredEntry({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 2);
    QCOMPARE(newEntryInListSpy.count(), 0);

    QCOMPARE(newEntryInListSpy.wait(50), false);

    myPlayList.enqueueRestoredEntry({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 3);
    QCOMPARE(newEntryInListSpy.count(), 0);

    QCOMPARE(newEntryInListSpy.wait(50), false);

    myPlayList.enqueueRestoredEntry({QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newEntryInListSpy.count(), 0);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    while (dataChangedSpy.count() < 8) {
        QCOMPARE(dataChangedSpy.wait(), true);
    }

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 8);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newEntryInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 7);
}

void MediaPlayListTest::testHasHeaderWithRemove()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueue({firstTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueue({secondTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    myPlayList.enqueue({thirdTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    myPlayList.enqueue({fourthTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    myPlayList.removeRows(2, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    myPlayList.removeRows(0, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 2);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);
}

void MediaPlayListTest::testHasHeaderMoveFirst()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueue({firstTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueue({secondTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    myPlayList.enqueue({thirdTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    myPlayList.enqueue({fourthTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    QCOMPARE(myPlayList.moveRows({}, 0, 1, {}, 4), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);
}

void MediaPlayListTest::testHasHeaderMoveAnother()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueue({firstTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueue({secondTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    myPlayList.enqueue({thirdTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    myPlayList.enqueue({fourthTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    QCOMPARE(myPlayList.moveRows({}, 3, 1, {}, 0), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);
}

void MediaPlayListTest::testHasHeaderMoveFirstLikeQml()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueue({firstTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueue({secondTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    myPlayList.enqueue({thirdTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    myPlayList.enqueue({fourthTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    myPlayList.move(0, 3, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);
}

void MediaPlayListTest::testHasHeaderMoveAnotherLikeQml()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueue({firstTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueue({secondTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    myPlayList.enqueue({thirdTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    myPlayList.enqueue({fourthTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    myPlayList.move(3, 0, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);
}

void MediaPlayListTest::testHasHeaderYetAnotherMoveLikeQml()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueue({firstTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueue({secondTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    myPlayList.enqueue({thirdTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    myPlayList.enqueue({fourthTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    myPlayList.move(0, 2, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    myPlayList.move(2, 0, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 2);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);
}

void MediaPlayListTest::enqueueReplaceAndPlay()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"),
                                                                               QStringLiteral("album2"), 6, 1);
    myPlayList.enqueue({firstTrackID, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 10);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    myPlayList.enqueue({secondTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);

    myPlayList.enqueue({myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/")),
                        QStringLiteral("album1")},
                       ElisaUtils::Album,
                       ElisaUtils::ReplacePlayList,
                       ElisaUtils::TriggerPlay);

    QVERIFY(dataChangedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 4);
}

void MediaPlayListTest::crashOnEnqueue()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);
    CrashEnqueuePlayList myCrash(&myPlayList);

    connect(&myPlayList, &MediaPlayList::rowsInserted, &myCrash, &CrashEnqueuePlayList::crashMediaPlayList);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"),
                                                                             QStringLiteral("album2"), 6, 1);
    myPlayList.enqueue({newTrackID, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
}

void MediaPlayListTest::restoreMultipleIdenticalTracks()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContentHeaderWithRestore"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myPlayList.enqueueRestoredEntry({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 1);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueueRestoredEntry({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 2);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueueRestoredEntry({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 3);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueueRestoredEntry({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newEntryInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 8);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newEntryInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 3);
}

void MediaPlayListTest::restoreTrackWithoutAlbum()
{
    QTemporaryFile databaseFile;
    databaseFile.open();

    qDebug() << "restoreTrackWithoutAlbum" << databaseFile.fileName();

    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("restoreTrackWithoutAlbum"), databaseFile.fileName());

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto newTrack = MusicAudioTrack{};

    newTrack.setValid(true);
    newTrack.setId(QStringLiteral("$29"));
    newTrack.setParentId(QStringLiteral("0"));
    newTrack.setTitle(QStringLiteral("track19"));
    newTrack.setArtist(QStringLiteral("artist2"));
    newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(29));
    newTrack.setResourceURI(QUrl::fromLocalFile(QStringLiteral("/$29")));
    newTrack.setFileModificationTime(QDateTime::fromMSecsSinceEpoch(29));
    newTrack.setAlbumCover(QUrl::fromLocalFile(QStringLiteral("withoutAlbum")));
    newTrack.setRating(9);
    newTrack.setIsSingleDiscAlbum(true);
    newTrack.setGenre(QStringLiteral("genre1"));
    newTrack.setComposer(QStringLiteral("composer1"));
    newTrack.setLyricist(QStringLiteral("lyricist1"));
    newTrack.setHasEmbeddedCover(false);

    myDatabaseContent.insertTracksList({newTrack}, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto newEntry = MediaPlayListEntry{};

    newEntry.mTitle = QStringLiteral("track19");
    newEntry.mArtist = QStringLiteral("artist2");

    myPlayList.enqueueRestoredEntry(newEntry);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 1);
    QCOMPARE(newEntryInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 1);
    QCOMPARE(newEntryInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track19"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QString());
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 29);
}

void MediaPlayListTest::testHasHeaderAlbumWithSameTitle()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    myPlayList.enqueue({firstTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
    myPlayList.enqueue({secondTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
    myPlayList.enqueue({thirdTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"),
                                                                                QStringLiteral("album3"), 3, 1);
    myPlayList.enqueue({fourthTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    auto fithTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track9"), QStringLiteral("artist2"),
                                                                              QStringLiteral("album3"), 9, 1);
    myPlayList.enqueue({fithTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 5);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 5);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 5);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 5);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 5);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 5);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 5);

    auto sixthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist7"),
                                                                               QStringLiteral("album3"), 1, 1);
    myPlayList.enqueue({sixthTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 6);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 6);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 5);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 6);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 6);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 6);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 6);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 6);
}

void MediaPlayListTest::testSavePersistentState()
{
    MediaPlayList myPlayListSave;
    QAbstractItemModelTester testModelSave(&myPlayListSave);
    DatabaseInterface myDatabaseContent;
    TracksListener myListenerSave(&myDatabaseContent);
    MediaPlayList myPlayListRead;
    QAbstractItemModelTester testModelRead(&myPlayListRead);
    TracksListener myListenerRead(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpySave(&myPlayListSave, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpySave(&myPlayListSave, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpySave(&myPlayListSave, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpySave(&myPlayListSave, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpySave(&myPlayListSave, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpySave(&myPlayListSave, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpySave(&myPlayListSave, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpySave(&myPlayListSave, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpySave(&myPlayListSave, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpySave(&myPlayListSave, &MediaPlayList::newEntryInList);
    QSignalSpy rowsAboutToBeMovedSpyRead(&myPlayListRead, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpyRead(&myPlayListRead, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpyRead(&myPlayListRead, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpyRead(&myPlayListRead, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpyRead(&myPlayListRead, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpyRead(&myPlayListRead, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpyRead(&myPlayListRead, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpyRead(&myPlayListRead, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpyRead(&myPlayListRead, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpyRead(&myPlayListRead, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 0);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 0);
    QCOMPARE(persistentStateChangedSpySave.count(), 0);
    QCOMPARE(dataChangedSpySave.count(), 0);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newEntryInListSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newEntryInListSpyRead.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListenerSave, &TracksListener::trackHasChanged,
            &myPlayListSave, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListenerSave, &TracksListener::tracksListAdded,
            &myPlayListSave, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayListSave, &MediaPlayList::newTrackByNameInList,
            &myListenerSave, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayListSave, &MediaPlayList::newEntryInList,
            &myListenerSave, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListenerSave, &TracksListener::tracksAdded);
    connect(&myListenerRead, &TracksListener::trackHasChanged,
            &myPlayListRead, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListenerRead, &TracksListener::tracksListAdded,
            &myPlayListRead, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayListRead, &MediaPlayList::newTrackByNameInList,
            &myListenerRead, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayListRead, &MediaPlayList::newEntryInList,
            &myListenerRead, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListenerRead, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 0);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 0);
    QCOMPARE(persistentStateChangedSpySave.count(), 0);
    QCOMPARE(dataChangedSpySave.count(), 0);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newEntryInListSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newEntryInListSpyRead.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    myPlayListSave.enqueue({firstTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 1);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 1);
    QCOMPARE(persistentStateChangedSpySave.count(), 1);
    QCOMPARE(dataChangedSpySave.count(), 0);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newEntryInListSpySave.count(), 1);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newEntryInListSpyRead.count(), 0);

    QCOMPARE(dataChangedSpySave.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 1);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 1);
    QCOMPARE(persistentStateChangedSpySave.count(), 1);
    QCOMPARE(dataChangedSpySave.count(), 1);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newEntryInListSpySave.count(), 1);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newEntryInListSpyRead.count(), 0);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
    myPlayListSave.enqueue({secondTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 2);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 2);
    QCOMPARE(persistentStateChangedSpySave.count(), 2);
    QCOMPARE(dataChangedSpySave.count(), 1);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newEntryInListSpySave.count(), 2);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newEntryInListSpyRead.count(), 0);

    QCOMPARE(dataChangedSpySave.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 2);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 2);
    QCOMPARE(persistentStateChangedSpySave.count(), 2);
    QCOMPARE(dataChangedSpySave.count(), 2);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newEntryInListSpySave.count(), 2);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newEntryInListSpyRead.count(), 0);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
    myPlayListSave.enqueue({thirdTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 3);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 3);
    QCOMPARE(persistentStateChangedSpySave.count(), 3);
    QCOMPARE(dataChangedSpySave.count(), 2);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newEntryInListSpySave.count(), 3);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newEntryInListSpyRead.count(), 0);

    QCOMPARE(dataChangedSpySave.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 3);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 3);
    QCOMPARE(persistentStateChangedSpySave.count(), 3);
    QCOMPARE(dataChangedSpySave.count(), 3);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newEntryInListSpySave.count(), 3);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newEntryInListSpyRead.count(), 0);

    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime(), QTime::fromMSecsSinceStartOfDay(11));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album3")));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$11")));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime(), QTime::fromMSecsSinceStartOfDay(1));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$1")));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime(), QTime::fromMSecsSinceStartOfDay(12));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album3")));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$12")));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), true);

    myPlayListRead.setPersistentState(myPlayListSave.persistentState());

    QCOMPARE(dataChangedSpyRead.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 3);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 3);
    QCOMPARE(persistentStateChangedSpySave.count(), 3);
    QCOMPARE(dataChangedSpySave.count(), 3);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newEntryInListSpySave.count(), 3);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 3);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 3);
    QCOMPARE(persistentStateChangedSpyRead.count(), 4);
    QCOMPARE(dataChangedSpyRead.count(), 6);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 3);
    QCOMPARE(newEntryInListSpyRead.count(), 0);

    QCOMPARE(myPlayListRead.tracksCount(), 3);

    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime(), QTime::fromMSecsSinceStartOfDay(11));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album3")));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$11")));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime(), QTime::fromMSecsSinceStartOfDay(1));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$1")));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime(), QTime::fromMSecsSinceStartOfDay(12));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album3")));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$12")));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), true);
}

void MediaPlayListTest::testReplaceAndPlayArtist()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.replaceAndPlay({0, QStringLiteral("artist3")}, ElisaUtils::Artist);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.tracksCount(), 1);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime(), QTime::fromMSecsSinceStartOfDay(3));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$3")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);

    myPlayList.replaceAndPlay({0, QStringLiteral("artist4")}, ElisaUtils::Artist);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(myPlayList.tracksCount(), 1);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime(), QTime::fromMSecsSinceStartOfDay(4));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$4")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);
}

void MediaPlayListTest::testReplaceAndPlayTrackId()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"),
                                                                               QStringLiteral("album1"), 3, 3);

    QCOMPARE(firstTrackId != 0, true);

    myPlayList.replaceAndPlay(ElisaUtils::EntryData{firstTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.tracksCount(), 1);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime(), QTime::fromMSecsSinceStartOfDay(3));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$3")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist4"),
                                                                                QStringLiteral("album1"), 4, 4);

    QCOMPARE(secondTrackId != 0, true);

    myPlayList.replaceAndPlay(ElisaUtils::EntryData{secondTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(myPlayList.tracksCount(), 1);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime(), QTime::fromMSecsSinceStartOfDay(4));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$4")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);
}

void MediaPlayListTest::testSetData()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    myPlayList.enqueue(ElisaUtils::EntryData{firstTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
    myPlayList.enqueue(ElisaUtils::EntryData{secondTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
    myPlayList.enqueue(ElisaUtils::EntryData{thirdTrackId, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayList.setData(myPlayList.index(2, 0), true, MediaPlayList::ColumnsRoles::IsPlayingRole), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), true);

    QCOMPARE(myPlayList.setData(myPlayList.index(2, 0), true, MediaPlayList::ColumnsRoles::SecondaryTextRole), false);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(myPlayList.setData(myPlayList.index(4, 0), true, MediaPlayList::ColumnsRoles::TitleRole), false);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(myPlayList.setData({}, true, MediaPlayList::ColumnsRoles::TitleRole), false);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);
}

void MediaPlayListTest::testRemoveSelection()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.replaceAndPlay({0, QStringLiteral("artist1")}, ElisaUtils::Artist);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.tracksCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);

    myPlayList.removeSelection({2, 4, 5});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 3);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 3);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.tracksCount(), 3);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
}

void MediaPlayListTest::testTrackBeenRemoved()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.replaceAndPlay({0, QStringLiteral("artist1")}, ElisaUtils::Artist);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.tracksCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);

    auto removedTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"),
                                                                                 QStringLiteral("album2"), 2, 1);

    QCOMPARE(removedTrackId != 0, true);

    auto removedTrack = myDatabaseContent.trackDataFromDatabaseId(removedTrackId);

    QVERIFY(!removedTrack.isEmpty());

    myPlayList.trackRemoved(removedTrack[DataTypes::DatabaseIdRole].toULongLong());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.tracksCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), -1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
}

CrashEnqueuePlayList::CrashEnqueuePlayList(MediaPlayList *list, QObject *parent) : QObject(parent), mList(list)
{
}

void CrashEnqueuePlayList::crashMediaPlayList()
{
    mList->data(mList->index(0, 0), MediaPlayList::ResourceRole);
}

void MediaPlayListTest::testBringUpCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void MediaPlayListTest::testBringUpCaseFromNewAlbum()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                             QStringLiteral("album2"), 1, 1);

    QVERIFY(newTrackID != 0);

    myPlayList.enqueue({newTrackID, QStringLiteral("track1")}, ElisaUtils::Track);

    QVERIFY(currentTrackChangedSpy.wait());

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void MediaPlayListTest::testBringUpAndDownCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.removeRow(0);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex());
}

void MediaPlayListTest::testBringUpAndRemoveCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.removeRow(0);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex());
}

void MediaPlayListTest::testBringUpAndRemoveMultipleCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2),
                        QStringLiteral("track2")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.removeRow(0);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void MediaPlayListTest::testBringUpAndRemoveMultipleNotBeginCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2),
                        QStringLiteral("track2")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 1),
                        QStringLiteral("track4")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayList.removeRow(1);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
}

void MediaPlayListTest::testBringUpAndPlayCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
}

void MediaPlayListTest::testBringUpAndSkipNextCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
}

void MediaPlayListTest::testBringUpAndSkipPreviousCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayList.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void MediaPlayListTest::testBringUpAndSkipPreviousAndContinueCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.setRepeatPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1),
                        QStringLiteral("track2")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2),
                        QStringLiteral("track2")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(3, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(4, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 6);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(5, 0)));

    myPlayList.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 7);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(4, 0)));

    myPlayList.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 8);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(3, 0)));

    myPlayList.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 9);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));

    myPlayList.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 10);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayList.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 11);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 12);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(5, 0)));
}

void MediaPlayListTest::finishPlayList()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void MediaPlayListTest::randomPlayList()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1),
                        QStringLiteral("track5")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.setRandomPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.randomPlay(), true);

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);
}

void MediaPlayListTest::randomAndContinuePlayList()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1),
                        QStringLiteral("track5")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.setRandomPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.randomPlay(), true);

    myPlayList.setRepeatPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.repeatPlay(), true);

    myPlayList.switchTo(3);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(3, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);
}

void MediaPlayListTest::continuePlayList()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1),
                        QStringLiteral("track5")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.setRepeatPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.repeatPlay(), true);

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(3, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void MediaPlayListTest::testRestoreSettings()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    QVariantMap settings;
    settings[QStringLiteral("currentTrack")] = 2;
    settings[QStringLiteral("randomPlay")] = true;
    settings[QStringLiteral("repeatPlay")] = true;

    myPlayList.setPersistentState(settings);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1),
                        QStringLiteral("track5")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QVERIFY(currentTrackChangedSpy.wait());

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));
}

void MediaPlayListTest::testSaveAndRestoreSettings()
{
    MediaPlayList myPlayListSave;
    QAbstractItemModelTester testModelSave(&myPlayListSave);
    DatabaseInterface myDatabaseContent;
    TracksListener myListenerSave(&myDatabaseContent);
    MediaPlayList myPlayListRestore;
    QAbstractItemModelTester testModelRestore(&myPlayListRestore);
    TracksListener myListenerRestore(&myDatabaseContent);

    QSignalSpy currentTrackChangedSaveSpy(&myPlayListSave, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSaveSpy(&myPlayListSave, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSaveSpy(&myPlayListSave, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSaveSpy(&myPlayListSave, &MediaPlayList::playListFinished);
    QSignalSpy currentTrackChangedRestoreSpy(&myPlayListRestore, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedRestoreSpy(&myPlayListRestore, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedRestoreSpy(&myPlayListRestore, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedRestoreSpy(&myPlayListRestore, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListenerSave, &TracksListener::trackHasChanged,
            &myPlayListSave, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListenerSave, &TracksListener::tracksListAdded,
            &myPlayListSave, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayListSave, &MediaPlayList::newTrackByNameInList,
            &myListenerSave, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayListSave, &MediaPlayList::newEntryInList,
            &myListenerSave, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListenerSave, &TracksListener::tracksAdded);

    connect(&myListenerRestore, &TracksListener::trackHasChanged,
            &myPlayListRestore, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListenerRestore, &TracksListener::tracksListAdded,
            &myPlayListRestore, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayListRestore, &MediaPlayList::newTrackByNameInList,
            &myListenerRestore, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayListRestore, &MediaPlayList::newEntryInList,
            &myListenerRestore, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListenerRestore, &TracksListener::tracksAdded);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 0);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 0);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 0);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 0);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 0);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 0);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    myPlayListSave.setRepeatPlay(true);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 0);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    myPlayListSave.setRandomPlay(true);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 0);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    myPlayListSave.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayListSave.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);
    myPlayListSave.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1),
                        QStringLiteral("track5")},
                       ElisaUtils::Track);
    myPlayListSave.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 0);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    QCOMPARE(currentTrackChangedSaveSpy.wait(), true);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 1);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    QCOMPARE(myPlayListSave.currentTrack(), QPersistentModelIndex(myPlayListSave.index(0, 0)));

    myPlayListSave.skipNextTrack();

    QCOMPARE(currentTrackChangedSaveSpy.count(), 2);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    myPlayListSave.skipNextTrack();

    QCOMPARE(currentTrackChangedSaveSpy.count(), 3);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    myPlayListRestore.setPersistentState(myPlayListSave.persistentState());

    QCOMPARE(currentTrackChangedSaveSpy.count(), 3);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 1);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 1);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 1);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    qDebug() << myPlayListRestore.currentTrack();

    QCOMPARE(myPlayListRestore.repeatPlay(), true);
    QCOMPARE(myPlayListRestore.randomPlay(), true);
    const auto indexSavePlaylist = myPlayListRestore.currentTrack();
    QCOMPARE(myPlayListRestore.currentTrack(), QPersistentModelIndex(myPlayListRestore.index(indexSavePlaylist.row(), 0)));
}

void MediaPlayListTest::removeBeforeCurrentTrack()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1),
                        QStringLiteral("track5")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(3, 0)));

    myPlayList.removeRow(1);

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);
}

void MediaPlayListTest::switchToTrackTest()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 4),
                        QStringLiteral("track4")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2),
                        QStringLiteral("track2")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.switchTo(4);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(4, 0)));
}

void MediaPlayListTest::previousAndNextTracksTest()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy previousTrackChangedSpy(&myPlayList, &MediaPlayList::previousTrackChanged);
    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy nextTrackChangedSpy(&myPlayList, &MediaPlayList::nextTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(previousTrackChangedSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(nextTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 4),
                        QStringLiteral("track4")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2),
                        QStringLiteral("track2")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(previousTrackChangedSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(nextTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(previousTrackChangedSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(nextTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.previousTrack(), QPersistentModelIndex());
    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
    QCOMPARE(myPlayList.nextTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(previousTrackChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(nextTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.previousTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
    QCOMPARE(myPlayList.nextTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));

    myPlayList.switchTo(4);

    QCOMPARE(previousTrackChangedSpy.count(), 2);
    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(nextTrackChangedSpy.count(), 3);

    QCOMPARE(myPlayList.previousTrack(), QPersistentModelIndex(myPlayList.index(3, 0)));
    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(4, 0)));
    QCOMPARE(myPlayList.nextTrack(), QPersistentModelIndex());

    myPlayList.setRepeatPlay(true);

    QCOMPARE(previousTrackChangedSpy.count(), 2);
    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(nextTrackChangedSpy.count(), 4);

    QCOMPARE(myPlayList.previousTrack(), QPersistentModelIndex(myPlayList.index(3, 0)));
    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(4, 0)));
    QCOMPARE(myPlayList.nextTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(previousTrackChangedSpy.count(), 3);
    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(nextTrackChangedSpy.count(), 5);

    QCOMPARE(myPlayList.previousTrack(), QPersistentModelIndex(myPlayList.index(4, 0)));
    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
    QCOMPARE(myPlayList.nextTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayList.setRandomPlay(true);

    QVERIFY(myPlayList.previousTrack() != QPersistentModelIndex());
    QVERIFY(myPlayList.currentTrack() != QPersistentModelIndex());
    QVERIFY(myPlayList.nextTrack() != QPersistentModelIndex());
}

void MediaPlayListTest::singleTrack()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void MediaPlayListTest::remainingTracksTest()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy remainingTracksChangedSpy(&myPlayList, &MediaPlayList::remainingTracksChanged);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 4),
                        QStringLiteral("track4")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2),
                        QStringLiteral("track2")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 5);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 6);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
    QCOMPARE(myPlayList.remainingTracks(), 4);

    myPlayList.skipNextTrack();

    QCOMPARE(remainingTracksChangedSpy.count(), 7);
    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
    QCOMPARE(myPlayList.remainingTracks(), 3);

    myPlayList.setRandomPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 8);

    QCOMPARE(myPlayList.remainingTracks(), -1);

    myPlayList.setRandomPlay(false);
    myPlayList.setRepeatPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 2);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 10);

    QCOMPARE(myPlayList.remainingTracks(), -1);
}

void MediaPlayListTest::testBringUpAndRemoveLastCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayList::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1),
                        QStringLiteral("track2")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));

    myPlayList.removeRow(2);

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void MediaPlayListTest::testSaveLoadPlayList()
{
    MediaPlayList myPlayListSave;
    QAbstractItemModelTester testModelSave(&myPlayListSave);
    DatabaseInterface myDatabaseContent;
    TracksListener myListenerSave(&myDatabaseContent);
    MediaPlayList myPlayListRestore;
    QAbstractItemModelTester testModelRestore(&myPlayListRestore);
    TracksListener myListenerRestore(&myDatabaseContent);

    QSignalSpy currentTrackChangedSaveSpy(&myPlayListSave, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedSaveSpy(&myPlayListSave, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedSaveSpy(&myPlayListSave, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedSaveSpy(&myPlayListSave, &MediaPlayList::playListFinished);
    QSignalSpy playListLoadedSaveSpy(&myPlayListSave, &MediaPlayList::playListLoaded);
    QSignalSpy playListLoadFailedSaveSpy(&myPlayListSave, &MediaPlayList::playListLoadFailed);
    QSignalSpy currentTrackChangedRestoreSpy(&myPlayListRestore, &MediaPlayList::currentTrackChanged);
    QSignalSpy randomPlayChangedRestoreSpy(&myPlayListRestore, &MediaPlayList::randomPlayChanged);
    QSignalSpy repeatPlayChangedRestoreSpy(&myPlayListRestore, &MediaPlayList::repeatPlayChanged);
    QSignalSpy playListFinishedRestoreSpy(&myPlayListRestore, &MediaPlayList::playListFinished);
    QSignalSpy playListLoadedRestoreSpy(&myPlayListRestore, &MediaPlayList::playListLoaded);
    QSignalSpy playListLoadFailedRestoreSpy(&myPlayListRestore, &MediaPlayList::playListLoadFailed);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListenerSave, &TracksListener::trackHasChanged,
            &myPlayListSave, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListenerSave, &TracksListener::tracksListAdded,
            &myPlayListSave, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayListSave, &MediaPlayList::newTrackByNameInList,
            &myListenerSave, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayListSave, &MediaPlayList::newEntryInList,
            &myListenerSave, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListenerSave, &TracksListener::tracksAdded);

    connect(&myListenerRestore, &TracksListener::trackHasChanged,
            &myPlayListRestore, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListenerRestore, &TracksListener::tracksListAdded,
            &myPlayListRestore, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayListRestore, &MediaPlayList::newTrackByNameInList,
            &myListenerRestore, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayListRestore, &MediaPlayList::newEntryInList,
            &myListenerRestore, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListenerRestore, &TracksListener::tracksAdded);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 0);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 0);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(playListLoadedSaveSpy.count(), 0);
    QCOMPARE(playListLoadFailedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadFailedRestoreSpy.count(), 0);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 0);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 0);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(playListLoadedSaveSpy.count(), 0);
    QCOMPARE(playListLoadFailedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadFailedRestoreSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 0);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 0);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(playListLoadedSaveSpy.count(), 0);
    QCOMPARE(playListLoadFailedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadFailedRestoreSpy.count(), 0);

    myPlayListSave.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayListSave.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3),
                        QStringLiteral("track3")},
                       ElisaUtils::Track);
    myPlayListSave.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1),
                        QStringLiteral("track5")},
                       ElisaUtils::Track);
    myPlayListSave.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 0);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 0);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(playListLoadedSaveSpy.count(), 0);
    QCOMPARE(playListLoadFailedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadFailedRestoreSpy.count(), 0);

    QCOMPARE(currentTrackChangedSaveSpy.wait(), true);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 1);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 0);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(playListLoadedSaveSpy.count(), 0);
    QCOMPARE(playListLoadFailedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadFailedRestoreSpy.count(), 0);

    QCOMPARE(myPlayListSave.currentTrack(), QPersistentModelIndex(myPlayListSave.index(0, 0)));

    QTemporaryFile playlistFile(QStringLiteral("./myPlaylistXXXXXX.m3u"));
    playlistFile.open();

    QCOMPARE(myPlayListSave.savePlaylist(QUrl::fromLocalFile(playlistFile.fileName())), true);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 1);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 0);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(playListLoadedSaveSpy.count(), 0);
    QCOMPARE(playListLoadFailedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadFailedRestoreSpy.count(), 0);

    myPlayListRestore.loadPlaylist(QUrl::fromLocalFile(playlistFile.fileName()));

    QCOMPARE(currentTrackChangedSaveSpy.count(), 1);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 0);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(playListLoadedSaveSpy.count(), 0);
    QCOMPARE(playListLoadFailedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadedRestoreSpy.count(), 1);
    QCOMPARE(playListLoadFailedRestoreSpy.count(), 0);

    QCOMPARE(currentTrackChangedRestoreSpy.wait(300), true);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 1);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 0);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(playListLoadedSaveSpy.count(), 0);
    QCOMPARE(playListLoadFailedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 1);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadedRestoreSpy.count(), 1);
    QCOMPARE(playListLoadFailedRestoreSpy.count(), 0);

    QCOMPARE(myPlayListRestore.currentTrack(), QPersistentModelIndex(myPlayListRestore.index(0, 0)));
}

void MediaPlayListTest::testEnqueueFiles()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueue({{0, QStringLiteral("/$1")}, {0, QStringLiteral("/$2")}}, ElisaUtils::FileName);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(myPlayList.rowCount(), 2);

    QCOMPARE(dataChangedSpy.wait(300), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 2);
}

void MediaPlayListTest::testEnqueueSampleFiles()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueue({{0, QStringLiteral(MEDIAPLAYLIST_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/test.ogg")},
                        {0, QStringLiteral(MEDIAPLAYLIST_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/test2.ogg")}},
                       ElisaUtils::FileName);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(myPlayList.rowCount(), 2);

    while (dataChangedSpy.count() < 3) {
        QCOMPARE(dataChangedSpy.wait(), true);
    }

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("Title"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("Test"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("Artist"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 1000);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("Title2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("Test2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("Artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::MilliSecondsDurationRole).toInt(), 1000);
}

void MediaPlayListTest::testEmptyEnqueue()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueue(ElisaUtils::EntryDataList{}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueue(ElisaUtils::EntryDataList{}, ElisaUtils::FileName);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueue(ElisaUtils::EntryDataList{}, ElisaUtils::Track, ElisaUtils::AppendPlayList, ElisaUtils::DoNotTriggerPlay);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueue(ElisaUtils::EntryDataList{}, ElisaUtils::Album, ElisaUtils::AppendPlayList, ElisaUtils::DoNotTriggerPlay);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueue(ElisaUtils::EntryDataList{},
                       ElisaUtils::Artist,
                       ElisaUtils::AppendPlayList,
                       ElisaUtils::DoNotTriggerPlay);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
}

QTEST_GUILESS_MAIN(MediaPlayListTest)


#include "moc_mediaplaylisttest.cpp"
