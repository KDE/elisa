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
    {QUrl::fromLocalFile(QStringLiteral("file://image$1"))}, 1, false},
        {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 2, 2, QTime::fromMSecsSinceStartOfDay(2000), {QUrl::fromLocalFile(QStringLiteral("$2"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$2"))}, 2, false},
        {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 3, 3, QTime::fromMSecsSinceStartOfDay(3000), {QUrl::fromLocalFile(QStringLiteral("$3"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$3"))}, 3, false},
        {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist4"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 4, 4, QTime::fromMSecsSinceStartOfDay(4000), {QUrl::fromLocalFile(QStringLiteral("$4"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$4"))}, 4, false},
        {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 3, 1, QTime::fromMSecsSinceStartOfDay(7000), {QUrl::fromLocalFile(QStringLiteral("$7"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$7"))}, 5, true},
        {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 2, 1, QTime::fromMSecsSinceStartOfDay(6000), {QUrl::fromLocalFile(QStringLiteral("$6"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 1, true},
        {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 4, 1, QTime::fromMSecsSinceStartOfDay(8000), {QUrl::fromLocalFile(QStringLiteral("$8"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$8"))}, 2, true},
        {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 5, 1, QTime::fromMSecsSinceStartOfDay(9000), {QUrl::fromLocalFile(QStringLiteral("$9"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$9"))}, 3, true},
        {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, 1, QTime::fromMSecsSinceStartOfDay(5000), {QUrl::fromLocalFile(QStringLiteral("$5"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$5"))}, 4, true},
        {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
            QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), QStringLiteral("artist1"), 6, 1, QTime::fromMSecsSinceStartOfDay(10000), {QUrl::fromLocalFile(QStringLiteral("$10"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$10"))}, 5, true},
        {true, QStringLiteral("$11"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 1, 1, QTime::fromMSecsSinceStartOfDay(11000), {QUrl::fromLocalFile(QStringLiteral("$11"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$11"))}, 1, true},
        {true, QStringLiteral("$12"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 2, 1, QTime::fromMSecsSinceStartOfDay(12000), {QUrl::fromLocalFile(QStringLiteral("$12"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$12"))}, 2, true},
        {true, QStringLiteral("$13"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 3, 1, QTime::fromMSecsSinceStartOfDay(13000), {QUrl::fromLocalFile(QStringLiteral("$13"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$13"))}, 3, true},
        {true, QStringLiteral("$14"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 1, 1, QTime::fromMSecsSinceStartOfDay(14000), {QUrl::fromLocalFile(QStringLiteral("$14"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$14"))}, 4, true},
        {true, QStringLiteral("$15"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 2, 1, QTime::fromMSecsSinceStartOfDay(15000), {QUrl::fromLocalFile(QStringLiteral("$15"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$15"))}, 5, true},
        {true, QStringLiteral("$16"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 3, 1, QTime::fromMSecsSinceStartOfDay(16000), {QUrl::fromLocalFile(QStringLiteral("$16"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$16"))}, 1, true},
        {true, QStringLiteral("$17"), QStringLiteral("0"), QStringLiteral("track4"),
            QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 4, 1, QTime::fromMSecsSinceStartOfDay(17000), {QUrl::fromLocalFile(QStringLiteral("$17"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$17"))}, 2, true},
        {true, QStringLiteral("$18"), QStringLiteral("0"), QStringLiteral("track5"),
            QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 5, 1, QTime::fromMSecsSinceStartOfDay(18000), {QUrl::fromLocalFile(QStringLiteral("$18"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$18"))}, 3, true},
        {true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track1"),
            QStringLiteral("artist7"), QStringLiteral("album3"), QStringLiteral("artist7"), 1, 1, QTime::fromMSecsSinceStartOfDay(19000), {QUrl::fromLocalFile(QStringLiteral("$19"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$19"))}, 1, true},
        {true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track2"),
            QStringLiteral("artist7"), QStringLiteral("album3"), QStringLiteral("artist7"), 2, 1, QTime::fromMSecsSinceStartOfDay(20000), {QUrl::fromLocalFile(QStringLiteral("$20"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$20"))}, 2, true},
        {true, QStringLiteral("$21"), QStringLiteral("0"), QStringLiteral("track3"),
            QStringLiteral("artist7"), QStringLiteral("album3"), QStringLiteral("artist7"), 3, 1, QTime::fromMSecsSinceStartOfDay(21000), {QUrl::fromLocalFile(QStringLiteral("$21"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$21"))}, 3, true},
        {true, QStringLiteral("$22"), QStringLiteral("0"), QStringLiteral("track9"),
            QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist7"), 9, 1, QTime::fromMSecsSinceStartOfDay(22000), {QUrl::fromLocalFile(QStringLiteral("$22"))},
    {QUrl::fromLocalFile(QStringLiteral("file://image$22"))}, 9, true},
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    myPlayList.enqueue(myDatabaseContent.albumFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1")));

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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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
    QCOMPARE(persistentStateChangedSpy.count(), 1);
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
    QCOMPARE(persistentStateChangedSpy.count(), 2);
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    myPlayList.enqueue(myDatabaseContent.albumFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1")));

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
    QCOMPARE(dataChangedSpy.count(), 1);
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
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

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
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

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
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

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

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

    QCOMPARE(newArtistInListSpy.wait(50), false);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

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

    QCOMPARE(newArtistInListSpy.wait(50), false);

    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1});

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

    QCOMPARE(newArtistInListSpy.wait(50), false);

    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1});

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(dataChangedSpy.wait(), true);
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
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

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
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

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
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

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
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

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
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

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
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

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
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

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
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

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
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

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
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

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
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

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
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

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
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

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
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

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
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

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
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

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
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

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
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

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto firstTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"),
                                                                               QStringLiteral("album2"), 6, 1);
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

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
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

    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);

    myPlayList.clearAndEnqueue(myDatabaseContent.albumFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists")));

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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"),
                                                                             QStringLiteral("album2"), 6, 1);
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});

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

    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});

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

    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});

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

    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});

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

void MediaPlayListTest::testHasHeaderAlbumWithSameTitle()
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
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

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
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

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
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

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"),
                                                                                QStringLiteral("album3"), 3, 1);
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

    auto fithTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track9"), QStringLiteral("artist2"),
                                                                              QStringLiteral("album3"), 9, 1);
    myPlayList.enqueue(fithTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 5);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 5);
    QCOMPARE(trackHasBeenAddedSpy.count(), 5);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 5);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 5);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 5);
    QCOMPARE(trackHasBeenAddedSpy.count(), 5);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 5);
    QCOMPARE(newTrackByIdInListSpy.count(), 5);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto sixthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist7"),
                                                                               QStringLiteral("album3"), 1, 1);
    myPlayList.enqueue(sixthTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 6);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 6);
    QCOMPARE(trackHasBeenAddedSpy.count(), 6);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 5);
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
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), false);
}

void MediaPlayListTest::testSavePersistentState()
{
    MediaPlayList myPlayListSave;
    DatabaseInterface myDatabaseContent;
    TracksListener myListenerSave(&myDatabaseContent);
    MediaPlayList myPlayListRead;
    TracksListener myListenerRead(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpySave(&myPlayListSave, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpySave(&myPlayListSave, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpySave(&myPlayListSave, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpySave(&myPlayListSave, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpySave(&myPlayListSave, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpySave(&myPlayListSave, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpySave(&myPlayListSave, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpySave(&myPlayListSave, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpySave(&myPlayListSave, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpySave(&myPlayListSave, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpySave(&myPlayListSave, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpySave(&myPlayListSave, &MediaPlayList::newArtistInList);
    QSignalSpy rowsAboutToBeMovedSpyRead(&myPlayListRead, &MediaPlayList::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpyRead(&myPlayListRead, &MediaPlayList::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpyRead(&myPlayListRead, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpyRead(&myPlayListRead, &MediaPlayList::rowsMoved);
    QSignalSpy rowsRemovedSpyRead(&myPlayListRead, &MediaPlayList::rowsRemoved);
    QSignalSpy rowsInsertedSpyRead(&myPlayListRead, &MediaPlayList::rowsInserted);
    QSignalSpy trackHasBeenAddedSpyRead(&myPlayListRead, &MediaPlayList::trackHasBeenAdded);
    QSignalSpy persistentStateChangedSpyRead(&myPlayListRead, &MediaPlayList::persistentStateChanged);
    QSignalSpy dataChangedSpyRead(&myPlayListRead, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByIdInListSpyRead(&myPlayListRead, &MediaPlayList::newTrackByIdInList);
    QSignalSpy newTrackByNameInListSpyRead(&myPlayListRead, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newArtistInListSpyRead(&myPlayListRead, &MediaPlayList::newArtistInList);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 0);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 0);
    QCOMPARE(trackHasBeenAddedSpySave.count(), 0);
    QCOMPARE(persistentStateChangedSpySave.count(), 0);
    QCOMPARE(dataChangedSpySave.count(), 0);
    QCOMPARE(newTrackByIdInListSpySave.count(), 0);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newArtistInListSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(trackHasBeenAddedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByIdInListSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newArtistInListSpyRead.count(), 0);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListenerSave, &TracksListener::trackHasChanged,
            &myPlayListSave, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListenerSave, &TracksListener::albumAdded,
            &myPlayListSave, &MediaPlayList::albumAdded,
            Qt::QueuedConnection);
    connect(&myPlayListSave, &MediaPlayList::newTrackByIdInList,
            &myListenerSave, &TracksListener::trackByIdInList,
            Qt::QueuedConnection);
    connect(&myPlayListSave, &MediaPlayList::newTrackByNameInList,
            &myListenerSave, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayListSave, &MediaPlayList::newArtistInList,
            &myListenerSave, &TracksListener::newArtistInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListenerSave, &TracksListener::tracksAdded);
    connect(&myListenerRead, &TracksListener::trackHasChanged,
            &myPlayListRead, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListenerRead, &TracksListener::albumAdded,
            &myPlayListRead, &MediaPlayList::albumAdded,
            Qt::QueuedConnection);
    connect(&myPlayListRead, &MediaPlayList::newTrackByIdInList,
            &myListenerRead, &TracksListener::trackByIdInList,
            Qt::QueuedConnection);
    connect(&myPlayListRead, &MediaPlayList::newTrackByNameInList,
            &myListenerRead, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayListRead, &MediaPlayList::newArtistInList,
            &myListenerRead, &TracksListener::newArtistInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListenerRead, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 0);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 0);
    QCOMPARE(trackHasBeenAddedSpySave.count(), 0);
    QCOMPARE(persistentStateChangedSpySave.count(), 0);
    QCOMPARE(dataChangedSpySave.count(), 0);
    QCOMPARE(newTrackByIdInListSpySave.count(), 0);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newArtistInListSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(trackHasBeenAddedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByIdInListSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newArtistInListSpyRead.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    myPlayListSave.enqueue(firstTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 1);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 1);
    QCOMPARE(trackHasBeenAddedSpySave.count(), 1);
    QCOMPARE(persistentStateChangedSpySave.count(), 1);
    QCOMPARE(dataChangedSpySave.count(), 0);
    QCOMPARE(newTrackByIdInListSpySave.count(), 1);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newArtistInListSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(trackHasBeenAddedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByIdInListSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newArtistInListSpyRead.count(), 0);

    QCOMPARE(dataChangedSpySave.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 1);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 1);
    QCOMPARE(trackHasBeenAddedSpySave.count(), 1);
    QCOMPARE(persistentStateChangedSpySave.count(), 1);
    QCOMPARE(dataChangedSpySave.count(), 1);
    QCOMPARE(newTrackByIdInListSpySave.count(), 1);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newArtistInListSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(trackHasBeenAddedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByIdInListSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newArtistInListSpyRead.count(), 0);

    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
    myPlayListSave.enqueue(secondTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 2);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 2);
    QCOMPARE(trackHasBeenAddedSpySave.count(), 2);
    QCOMPARE(persistentStateChangedSpySave.count(), 2);
    QCOMPARE(dataChangedSpySave.count(), 1);
    QCOMPARE(newTrackByIdInListSpySave.count(), 2);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newArtistInListSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(trackHasBeenAddedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByIdInListSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newArtistInListSpyRead.count(), 0);

    QCOMPARE(dataChangedSpySave.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 2);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 2);
    QCOMPARE(trackHasBeenAddedSpySave.count(), 2);
    QCOMPARE(persistentStateChangedSpySave.count(), 2);
    QCOMPARE(dataChangedSpySave.count(), 2);
    QCOMPARE(newTrackByIdInListSpySave.count(), 2);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newArtistInListSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(trackHasBeenAddedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByIdInListSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newArtistInListSpyRead.count(), 0);

    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
    myPlayListSave.enqueue(thirdTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 3);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 3);
    QCOMPARE(trackHasBeenAddedSpySave.count(), 3);
    QCOMPARE(persistentStateChangedSpySave.count(), 3);
    QCOMPARE(dataChangedSpySave.count(), 2);
    QCOMPARE(newTrackByIdInListSpySave.count(), 3);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newArtistInListSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(trackHasBeenAddedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByIdInListSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newArtistInListSpyRead.count(), 0);

    QCOMPARE(dataChangedSpySave.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 3);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 3);
    QCOMPARE(trackHasBeenAddedSpySave.count(), 3);
    QCOMPARE(persistentStateChangedSpySave.count(), 3);
    QCOMPARE(dataChangedSpySave.count(), 3);
    QCOMPARE(newTrackByIdInListSpySave.count(), 3);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newArtistInListSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(trackHasBeenAddedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);
    QCOMPARE(newTrackByIdInListSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 0);
    QCOMPARE(newArtistInListSpyRead.count(), 0);

    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::ImageRole).toUrl(), QUrl());
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("file:$11")));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumHeader).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::DurationRole).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::ImageRole).toUrl(), QUrl());
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("file:$1")));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(1, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumHeader).toBool(), false);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::DurationRole).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::ImageRole).toUrl(), QUrl());
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("file:$12")));
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayListSave.data(myPlayListSave.index(2, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumHeader).toBool(), true);

    myPlayListRead.setPersistentState(myPlayListSave.persistentState());

    QCOMPARE(dataChangedSpyRead.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 3);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 3);
    QCOMPARE(trackHasBeenAddedSpySave.count(), 3);
    QCOMPARE(persistentStateChangedSpySave.count(), 3);
    QCOMPARE(dataChangedSpySave.count(), 3);
    QCOMPARE(newTrackByIdInListSpySave.count(), 3);
    QCOMPARE(newTrackByNameInListSpySave.count(), 0);
    QCOMPARE(newArtistInListSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 3);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 3);
    QCOMPARE(trackHasBeenAddedSpyRead.count(), 3);
    QCOMPARE(persistentStateChangedSpyRead.count(), 4);
    QCOMPARE(dataChangedSpyRead.count(), 6);
    QCOMPARE(newTrackByIdInListSpyRead.count(), 0);
    QCOMPARE(newTrackByNameInListSpyRead.count(), 3);
    QCOMPARE(newArtistInListSpyRead.count(), 0);

    QCOMPARE(myPlayListRead.tracksCount(), 3);

    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::ImageRole).toUrl(), QUrl());
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("file:$11")));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumHeader).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::DurationRole).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::ImageRole).toUrl(), QUrl());
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("file:$1")));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(1, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumHeader).toBool(), false);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::DurationRole).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::ImageRole).toUrl(), QUrl());
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("file:$12")));
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayListRead.data(myPlayListRead.index(2, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumHeader).toBool(), true);
}

void MediaPlayListTest::testClearAndEnqueueArtist()
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    myPlayList.clearAndEnqueue(QStringLiteral("artist3"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 1);

    QCOMPARE(myPlayList.tracksCount(), 1);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ImageRole).toUrl(), QUrl());
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("file:$3")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumHeader).toBool(), false);

    myPlayList.clearAndEnqueue(QStringLiteral("artist4"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 2);

    QCOMPARE(myPlayList.tracksCount(), 1);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ImageRole).toUrl(), QUrl());
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("file:$4")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumHeader).toBool(), false);
}

void MediaPlayListTest::testClearAndEnqueueTrackId()
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"),
                                                                                QStringLiteral("album1"), 3, 3);

    QCOMPARE(firstTrackId != 0, true);

    myPlayList.clearAndEnqueue(firstTrackId);

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

    QCOMPARE(myPlayList.tracksCount(), 1);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ImageRole).toUrl(), QUrl());
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("file:$3")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumHeader).toBool(), false);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist4"),
                                                                                QStringLiteral("album1"), 4, 4);

    QCOMPARE(secondTrackId != 0, true);

    myPlayList.clearAndEnqueue(secondTrackId);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.tracksCount(), 1);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ImageRole).toUrl(), QUrl());
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("file:$4")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumHeader).toBool(), false);
}

void MediaPlayListTest::testClearAndEnqueueTrackData()
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"),
                                                                                QStringLiteral("album1"), 3, 3);

    QCOMPARE(firstTrackId != 0, true);

    auto firstTrack = myDatabaseContent.trackFromDatabaseId(firstTrackId);

    QCOMPARE(firstTrack.isValid(), true);

    myPlayList.clearAndEnqueue(firstTrack);

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

    QCOMPARE(myPlayList.tracksCount(), 1);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ImageRole).toUrl(), QUrl());
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("file:$3")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumHeader).toBool(), false);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist4"),
                                                                                QStringLiteral("album1"), 4, 4);

    QCOMPARE(secondTrackId != 0, true);

    auto secondTrack = myDatabaseContent.trackFromDatabaseId(secondTrackId);

    QCOMPARE(secondTrack.isValid(), true);

    myPlayList.clearAndEnqueue(secondTrack);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.tracksCount(), 1);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ImageRole).toUrl(), QUrl());
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("file:$4")));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumHeader).toBool(), false);
}

void MediaPlayListTest::testSetData()
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
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
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
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
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
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
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayList.setData(myPlayList.index(2, 0), true, MediaPlayList::ColumnsRoles::IsPlayingRole), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::HasAlbumHeader).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), true);

    QCOMPARE(myPlayList.setData(myPlayList.index(2, 0), true, MediaPlayList::ColumnsRoles::TitleRole), false);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.setData(myPlayList.index(4, 0), true, MediaPlayList::ColumnsRoles::TitleRole), false);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);

    QCOMPARE(myPlayList.setData({}, true, MediaPlayList::ColumnsRoles::TitleRole), false);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(trackHasBeenAddedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByIdInListSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 0);
}

void MediaPlayListTest::testRemoveSelection()
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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    myPlayList.clearAndEnqueue(QStringLiteral("artist1"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 1);

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
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 1);

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
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

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

    myPlayList.clearAndEnqueue(QStringLiteral("artist1"));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 1);

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

    auto removedTrack = myDatabaseContent.trackFromDatabaseId(removedTrackId);

    QCOMPARE(removedTrack.isValid(), true);

    myPlayList.trackRemoved(removedTrack.databaseId());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(trackHasBeenAddedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByIdInListSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newArtistInListSpy.count(), 1);

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
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

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
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                             QStringLiteral("album2"), 1, 2);
    myPlayList.enqueue(newTrackID);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void MediaPlayListTest::testBringUpAndDownCase()
{
    MediaPlayList myPlayList;
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
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

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

    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2});

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

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

    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1});

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3});

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3});

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3});

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
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.setRepeatPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});
    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1});
    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2});

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3});

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.seedRandomGenerator(0);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});
    myPlayList.enqueue({QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

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

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(3, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void MediaPlayListTest::randomAndContinuePlayList()
{
    MediaPlayList myPlayList;
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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.seedRandomGenerator(0);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});
    myPlayList.enqueue({QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

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

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(3, 0)));

    myPlayList.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));
}

void MediaPlayListTest::continuePlayList()
{
    MediaPlayList myPlayList;
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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.seedRandomGenerator(0);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});
    myPlayList.enqueue({QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

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

    QVariantMap settings;
    settings[QStringLiteral("currentTrack")] = 2;
    settings[QStringLiteral("randomPlay")] = true;
    settings[QStringLiteral("repeatPlay")] = true;

    myPlayList.setPersistentState(settings);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.seedRandomGenerator(0);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});
    myPlayList.enqueue({QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));
}

void MediaPlayListTest::testSaveAndRestoreSettings()
{
    MediaPlayList myPlayListSave;
    DatabaseInterface myDatabaseContent;
    TracksListener myListenerSave(&myDatabaseContent);
    MediaPlayList myPlayListRestore;
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
    connect(&myListenerSave, &TracksListener::albumAdded,
            &myPlayListSave, &MediaPlayList::albumAdded,
            Qt::QueuedConnection);
    connect(&myPlayListSave, &MediaPlayList::newTrackByIdInList,
            &myListenerSave, &TracksListener::trackByIdInList,
            Qt::QueuedConnection);
    connect(&myPlayListSave, &MediaPlayList::newTrackByNameInList,
            &myListenerSave, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayListSave, &MediaPlayList::newArtistInList,
            &myListenerSave, &TracksListener::newArtistInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListenerSave, &TracksListener::tracksAdded);

    connect(&myListenerRestore, &TracksListener::trackHasChanged,
            &myPlayListRestore, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListenerRestore, &TracksListener::albumAdded,
            &myPlayListRestore, &MediaPlayList::albumAdded,
            Qt::QueuedConnection);
    connect(&myPlayListRestore, &MediaPlayList::newTrackByIdInList,
            &myListenerRestore, &TracksListener::trackByIdInList,
            Qt::QueuedConnection);
    connect(&myPlayListRestore, &MediaPlayList::newTrackByNameInList,
            &myListenerRestore, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayListRestore, &MediaPlayList::newArtistInList,
            &myListenerRestore, &TracksListener::newArtistInList,
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

    myPlayListSave.seedRandomGenerator(0);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 0);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 0);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

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

    myPlayListSave.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayListSave.enqueue({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});
    myPlayListSave.enqueue({QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1});
    myPlayListSave.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

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

    QCOMPARE(myPlayListSave.currentTrack(), QPersistentModelIndex(myPlayListSave.index(0, 0)));

    myPlayListSave.skipNextTrack();

    QCOMPARE(currentTrackChangedSaveSpy.count(), 3);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    QCOMPARE(myPlayListSave.currentTrack(), QPersistentModelIndex(myPlayListSave.index(3, 0)));

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
    QCOMPARE(myPlayListRestore.currentTrack(), QPersistentModelIndex(myPlayListRestore.index(3, 0)));
}

void MediaPlayListTest::removeBeforeCurrentTrack()
{
    MediaPlayList myPlayList;
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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});
    myPlayList.enqueue({QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

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

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3});
    myPlayList.enqueue({QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 4});
    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2});
    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

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

void MediaPlayListTest::singleTrack()
{
    MediaPlayList myPlayList;
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
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});

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

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myPlayList.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void MediaPlayListTest::testBringUpAndRemoveLastCase()
{
    MediaPlayList myPlayList;
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
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayList.enqueue({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1});
    myPlayList.enqueue({QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1});
    myPlayList.enqueue({QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1});

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

QTEST_GUILESS_MAIN(MediaPlayListTest)


#include "moc_mediaplaylisttest.cpp"
