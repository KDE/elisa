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

#include "mediaplaylisttest.h"

#include "mediaplaylist.h"
#include "databaseinterface.h"
#include "musicalbum.h"
#include "musicaudiotrack.h"
#include "trackslistener.h"

#include <QtTest>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QUrl>
#include <QTime>

MediaPlayListTest::MediaPlayListTest(QObject *parent) : QObject(parent)
{
}

void MediaPlayListTest::initTestCase()
{
    mNewTracks = {
        {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 1, 1, QTime::fromMSecsSinceStartOfDay(1000), {QUrl::fromLocalFile(QStringLiteral("$1"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$1"))}, 1},
        {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 2, 2, QTime::fromMSecsSinceStartOfDay(2000), {QUrl::fromLocalFile(QStringLiteral("$2"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$2"))}, 2},
        {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 3, 3, QTime::fromMSecsSinceStartOfDay(3000), {QUrl::fromLocalFile(QStringLiteral("$3"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$3"))}, 3},
        {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist4"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 4, 4, QTime::fromMSecsSinceStartOfDay(4000), {QUrl::fromLocalFile(QStringLiteral("$4"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$4"))}, 4},
        {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 3, 1, QTime::fromMSecsSinceStartOfDay(7000), {QUrl::fromLocalFile(QStringLiteral("$7"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$7"))}, 5},
        {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 2, 1, QTime::fromMSecsSinceStartOfDay(6000), {QUrl::fromLocalFile(QStringLiteral("$6"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 1},
        {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 4, 1, QTime::fromMSecsSinceStartOfDay(8000), {QUrl::fromLocalFile(QStringLiteral("$8"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$8"))}, 2},
        {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 5, 1, QTime::fromMSecsSinceStartOfDay(9000), {QUrl::fromLocalFile(QStringLiteral("$9"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$9"))}, 3},
        {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, 1, QTime::fromMSecsSinceStartOfDay(5000), {QUrl::fromLocalFile(QStringLiteral("$5"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$5"))}, 4},
        {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
            QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), QStringLiteral("artist1"), 6, 1, QTime::fromMSecsSinceStartOfDay(10000), {QUrl::fromLocalFile(QStringLiteral("$10"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$10"))}, 5},
        {true, QStringLiteral("$11"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 1, 1, QTime::fromMSecsSinceStartOfDay(11000), {QUrl::fromLocalFile(QStringLiteral("$11"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$11"))}, 1},
        {true, QStringLiteral("$12"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 2, 1, QTime::fromMSecsSinceStartOfDay(12000), {QUrl::fromLocalFile(QStringLiteral("$12"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$12"))}, 2},
        {true, QStringLiteral("$13"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 3, 1, QTime::fromMSecsSinceStartOfDay(13000), {QUrl::fromLocalFile(QStringLiteral("$13"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$13"))}, 3},
        {true, QStringLiteral("$14"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 1, 1, QTime::fromMSecsSinceStartOfDay(14000), {QUrl::fromLocalFile(QStringLiteral("$14"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$14"))}, 4},
        {true, QStringLiteral("$15"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 2, 1, QTime::fromMSecsSinceStartOfDay(15000), {QUrl::fromLocalFile(QStringLiteral("$15"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$15"))}, 5},
        {true, QStringLiteral("$16"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 3, 1, QTime::fromMSecsSinceStartOfDay(16000), {QUrl::fromLocalFile(QStringLiteral("$16"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$16"))}, 1},
        {true, QStringLiteral("$17"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 4, 1, QTime::fromMSecsSinceStartOfDay(17000), {QUrl::fromLocalFile(QStringLiteral("$17"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$17"))}, 2},
        {true, QStringLiteral("$18"), QStringLiteral("0"), QStringLiteral("track5"),
            QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 5, 1, QTime::fromMSecsSinceStartOfDay(18000), {QUrl::fromLocalFile(QStringLiteral("$18"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$18"))}, 3}
    };

    mNewCovers[QStringLiteral("album1")] = QUrl::fromLocalFile(QStringLiteral("album1"));
    mNewCovers[QStringLiteral("album2")] = QUrl::fromLocalFile(QStringLiteral("album2"));
    mNewCovers[QStringLiteral("album3")] = QUrl::fromLocalFile(QStringLiteral("album3"));
    mNewCovers[QStringLiteral("album4")] = QUrl::fromLocalFile(QStringLiteral("album4"));

    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>");
    qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
    qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
}

void MediaPlayListTest::simpleInitialCase()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track6"), QStringLiteral("album2"), QStringLiteral("artist1 and artist2"));
    myPlayList.enqueue(newTrackID);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);
}

void MediaPlayListTest::enqueueAlbumCase()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    myPlayList.enqueue(myDatabaseContent.albumFromTitle(QStringLiteral("album2")));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 6);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 6);
    QCOMPARE(trackHasBeenAddedSpy.count(), 6);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 6);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 6);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 6);
    QCOMPARE(trackHasBeenAddedSpy.count(), 6);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 6);
    QCOMPARE(newTrackByIdInListSpy.count(), 6);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.rowCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:05"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:06"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:07"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:08"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:09"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:10"));
}

void MediaPlayListTest::enqueueArtistCase()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    myPlayList.enqueue(QStringLiteral("artist1"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 1);

    QCOMPARE(rowsAboutToBeInsertedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:01"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:05"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:06"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:07"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:08"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:09"));
}

void MediaPlayListTest::removeFirstTrackOfAlbum()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    myPlayList.enqueue(myDatabaseContent.albumFromTitle(QStringLiteral("album2")));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 6);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 6);
    QCOMPARE(trackHasBeenAddedSpy.count(), 6);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 6);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 6);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 6);
    QCOMPARE(trackHasBeenAddedSpy.count(), 6);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 6);
    QCOMPARE(newTrackByIdInListSpy.count(), 6);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    myPlayList.removeRow(0);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 6);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 6);
    QCOMPARE(trackHasBeenAddedSpy.count(), 6);
    QCOMPARE(persistentStateChangedSpy.count(), 7);
    QCOMPARE(dataChangedSpy.count(), 7);
    QCOMPARE(newTrackByIdInListSpy.count(), 6);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
}

void MediaPlayListTest::testHasHeader()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(firstTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));
    myPlayList.enqueue(secondTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track2"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(thirdTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track3"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(fourthTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    myPlayList.removeRows(2, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 5);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
}

void MediaPlayListTest::testHasHeaderWithRestore()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContentHeaderWithRestore"));

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1")});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 1);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1")});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 2);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("album2"), QStringLiteral("artist1")});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 3);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("album2"), QStringLiteral("artist1")});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 8);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:05"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:01"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:06"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:07"));
}

void MediaPlayListTest::testHasHeaderWithRemove()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(firstTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track2"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(secondTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));
    myPlayList.enqueue(thirdTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track3"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(fourthTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    myPlayList.removeRows(2, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 5);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    myPlayList.removeRows(0, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 2);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 6);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
}

void MediaPlayListTest::testHasHeaderMoveFirst()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(firstTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track2"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(secondTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track3"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(thirdTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track4"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(fourthTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    QCOMPARE(myPlayList.moveRows({}, 0, 1, {}, 4), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 6);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    const auto &firstDataChanged = dataChangedSpy[4];
    QCOMPARE(firstDataChanged.count(), 3);
    QCOMPARE(firstDataChanged.at(0).toModelIndex(), myPlayList.index(3, 0));
    QCOMPARE(firstDataChanged.at(1).toModelIndex(), myPlayList.index(3, 0));
    QCOMPARE(firstDataChanged.at(2).value<QVector<int>>(), {MediaPlayList::HasAlbumHeader});

    const auto &secondDataChanged = dataChangedSpy[5];
    QCOMPARE(secondDataChanged.count(), 3);
    QCOMPARE(secondDataChanged.at(0).toModelIndex(), myPlayList.index(0, 0));
    QCOMPARE(secondDataChanged.at(1).toModelIndex(), myPlayList.index(0, 0));
    QCOMPARE(secondDataChanged.at(2).value<QVector<int>>(), {MediaPlayList::HasAlbumHeader});

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
}

void MediaPlayListTest::testHasHeaderMoveAnother()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(firstTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track2"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(secondTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track3"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(thirdTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track4"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(fourthTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    QCOMPARE(myPlayList.moveRows({}, 3, 1, {}, 0), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 6);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    const auto &firstDataChanged = dataChangedSpy[4];
    QCOMPARE(firstDataChanged.count(), 3);
    QCOMPARE(firstDataChanged.at(0).toModelIndex(), myPlayList.index(0, 0));
    QCOMPARE(firstDataChanged.at(1).toModelIndex(), myPlayList.index(0, 0));
    QCOMPARE(firstDataChanged.at(2).value<QVector<int>>(), {MediaPlayList::HasAlbumHeader});

    const auto &secondDataChanged = dataChangedSpy[5];
    QCOMPARE(secondDataChanged.count(), 3);
    QCOMPARE(secondDataChanged.at(0).toModelIndex(), myPlayList.index(1, 0));
    QCOMPARE(secondDataChanged.at(1).toModelIndex(), myPlayList.index(1, 0));
    QCOMPARE(secondDataChanged.at(2).value<QVector<int>>(), {MediaPlayList::HasAlbumHeader});

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
}

void MediaPlayListTest::testHasHeaderMoveFirstLikeQml()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(firstTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track2"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(secondTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track3"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(thirdTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track4"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(fourthTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    myPlayList.move(0, 3, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 6);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    const auto &firstDataChanged = dataChangedSpy[4];
    QCOMPARE(firstDataChanged.count(), 3);
    QCOMPARE(firstDataChanged.at(0).toModelIndex(), myPlayList.index(3, 0));
    QCOMPARE(firstDataChanged.at(1).toModelIndex(), myPlayList.index(3, 0));
    QCOMPARE(firstDataChanged.at(2).value<QVector<int>>(), {MediaPlayList::HasAlbumHeader});

    const auto &secondDataChanged = dataChangedSpy[5];
    QCOMPARE(secondDataChanged.count(), 3);
    QCOMPARE(secondDataChanged.at(0).toModelIndex(), myPlayList.index(0, 0));
    QCOMPARE(secondDataChanged.at(1).toModelIndex(), myPlayList.index(0, 0));
    QCOMPARE(secondDataChanged.at(2).value<QVector<int>>(), {MediaPlayList::HasAlbumHeader});

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
}

void MediaPlayListTest::testHasHeaderMoveAnotherLikeQml()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(firstTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track2"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(secondTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track3"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(thirdTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track4"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(fourthTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    myPlayList.move(3, 0, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 6);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    const auto &firstDataChanged = dataChangedSpy[4];
    QCOMPARE(firstDataChanged.count(), 3);
    QCOMPARE(firstDataChanged.at(0).toModelIndex(), myPlayList.index(0, 0));
    QCOMPARE(firstDataChanged.at(1).toModelIndex(), myPlayList.index(0, 0));
    QCOMPARE(firstDataChanged.at(2).value<QVector<int>>(), {MediaPlayList::HasAlbumHeader});

    const auto &secondDataChanged = dataChangedSpy[5];
    QCOMPARE(secondDataChanged.count(), 3);
    QCOMPARE(secondDataChanged.at(0).toModelIndex(), myPlayList.index(1, 0));
    QCOMPARE(secondDataChanged.at(1).toModelIndex(), myPlayList.index(1, 0));
    QCOMPARE(secondDataChanged.at(2).value<QVector<int>>(), {MediaPlayList::HasAlbumHeader});

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
}

void MediaPlayListTest::testHasHeaderYetAnotherMoveLikeQml()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(firstTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track2"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(secondTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));
    myPlayList.enqueue(thirdTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track4"), QStringLiteral("album2"), QStringLiteral("artist1"));
    myPlayList.enqueue(fourthTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    myPlayList.move(0, 2, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 6);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    const auto &firstDataChanged = dataChangedSpy[4];
    QCOMPARE(firstDataChanged.count(), 3);
    QCOMPARE(firstDataChanged.at(0).toModelIndex(), myPlayList.index(0, 0));
    QCOMPARE(firstDataChanged.at(1).toModelIndex(), myPlayList.index(0, 0));
    QCOMPARE(firstDataChanged.at(2).value<QVector<int>>(), {MediaPlayList::HasAlbumHeader});

    const auto &secondDataChanged = dataChangedSpy[5];
    QCOMPARE(secondDataChanged.count(), 3);
    QCOMPARE(secondDataChanged.at(0).toModelIndex(), myPlayList.index(3, 0));
    QCOMPARE(secondDataChanged.at(1).toModelIndex(), myPlayList.index(3, 0));
    QCOMPARE(secondDataChanged.at(2).value<QVector<int>>(), {MediaPlayList::HasAlbumHeader});

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    myPlayList.move(2, 0, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 2);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 8);
    QCOMPARE(newTrackByIdInListSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    const auto &newFirstDataChanged = dataChangedSpy[6];
    QCOMPARE(newFirstDataChanged.count(), 3);
    QCOMPARE(newFirstDataChanged.at(0).toModelIndex(), myPlayList.index(3, 0));
    QCOMPARE(newFirstDataChanged.at(1).toModelIndex(), myPlayList.index(3, 0));
    QCOMPARE(newFirstDataChanged.at(2).value<QVector<int>>(), {MediaPlayList::HasAlbumHeader});

    const auto &newSecondDataChanged = dataChangedSpy[7];
    QCOMPARE(newSecondDataChanged.count(), 3);
    QCOMPARE(newSecondDataChanged.at(0).toModelIndex(), myPlayList.index(1, 0));
    QCOMPARE(newSecondDataChanged.at(1).toModelIndex(), myPlayList.index(1, 0));
    QCOMPARE(newSecondDataChanged.at(2).value<QVector<int>>(), {MediaPlayList::HasAlbumHeader});

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
}

void MediaPlayListTest::enqueueClearAndEnqueue()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    auto firstTrackID = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track6"), QStringLiteral("album2"), QStringLiteral("artist1 and artist2"));
    myPlayList.enqueue(firstTrackID);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:10"));

    auto secondTrackID = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));
    myPlayList.enqueue(secondTrackID);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);

    myPlayList.clearAndEnqueue(myDatabaseContent.albumFromTitle(QStringLiteral("album1")));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 6);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 6);
    QCOMPARE(trackHasBeenAddedSpy.count(), 6);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 6);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 6);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 6);
    QCOMPARE(trackHasBeenAddedSpy.count(), 6);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 6);
    QCOMPARE(newTrackByIdInListSpy.count(), 6);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:01"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:02"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:03"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:04"));
}

void MediaPlayListTest::crashOnEnqueue()
{
    MediaPlayList myPlayList;
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
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumArtist(QStringLiteral("track6"), QStringLiteral("album2"), QStringLiteral("artist1 and artist2"));
    myPlayList.enqueue(newTrackID);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);
}

void MediaPlayListTest::restoreMultipleIdenticalTracks()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayList, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayList, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayList, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayList, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpy(&myPlayList, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpy(&myPlayList, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpy(&myPlayList, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpy(&myPlayList, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContentHeaderWithRestore"));

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
    connect(&myDatabaseContent, &DatabaseInterface::trackAdded,
            &myListener, &TracksListener::trackAdded);

    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist3")});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 1);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist3")});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 2);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist3")});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 3);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist3")});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(trackHasBeenAddedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 8);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:03"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:03"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:03"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DurationRole).toString(), QStringLiteral("00:03"));
}




CrashEnqueuePlayList::CrashEnqueuePlayList(MediaPlayList *list, QObject *parent) : QObject(parent), mList(list)
{
}

void CrashEnqueuePlayList::crashMediaPlayList()
{
    mList->data(mList->index(0, 0), MediaPlayList::ResourceRole);
}

QTEST_MAIN(MediaPlayListTest)


#include "moc_mediaplaylisttest.cpp"
