/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "mediaplaylisttest.h"
#include "mediaplaylisttestconfig.h"

#include "mediaplaylist.h"
#include "databaseinterface.h"
#include "trackslistener.h"

#include "config-upnp-qt.h"

#include <QtTest>
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
    auto newTrackData = myDatabaseContent.trackDataFromDatabaseId(newTrackID);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, newTrackID}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                        QStringLiteral("album2"), {}});

    QVERIFY(rowsAboutToBeRemovedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    myPlayList.enqueueOneEntry({{{DataTypes::ElementTypeRole, ElisaUtils::Artist}}, QStringLiteral("artist1"), {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 1);

    QCOMPARE(rowsAboutToBeInsertedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 6);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumIdRole).toULongLong(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
}

void MediaPlayListTest::enqueueTrackByUrl()
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 0);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 0);

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
    auto trackData = myDatabaseContent.trackDataFromDatabaseId(newTrackID);
    myPlayList.enqueueFilesList({{{}, {}, trackData.resourceURI()}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 1);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 1);
}

void MediaPlayListTest::enqueueTracksByUrl()
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
   QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 0);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 0);

    auto firstNewTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
    auto firstTrackData = myDatabaseContent.trackDataFromDatabaseId(firstNewTrackID);
    auto secondNewTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    auto secondTrackData = myDatabaseContent.trackDataFromDatabaseId(secondNewTrackID);
    myPlayList.enqueueFilesList({{{}, {}, firstTrackData.resourceURI()}, {{}, {}, secondTrackData.resourceURI()}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 2);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 2);
}

void MediaPlayListTest::enqueueFiles()
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 0);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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
    QCOMPARE(newUrlInListSpy.count(), 0);

    myPlayList.enqueueFilesList({{{}, {}, QUrl::fromLocalFile(QStringLiteral("/$1"))}, {{}, {}, QUrl::fromLocalFile(QStringLiteral("/$2"))}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 2);

    QCOMPARE(myPlayList.rowCount(), 2);

    QCOMPARE(dataChangedSpy.wait(300), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 2);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 2);
}

void MediaPlayListTest::enqueueSampleFiles()
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 0);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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
    QCOMPARE(newUrlInListSpy.count(), 0);

    myPlayList.enqueueFilesList({{{}, {}, QUrl::fromLocalFile(QStringLiteral(MEDIAPLAYLIST_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/test.ogg"))},
                        {{}, {}, QUrl::fromLocalFile(QStringLiteral(MEDIAPLAYLIST_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/test2.ogg"))}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 2);

    QCOMPARE(myPlayList.rowCount(), 2);

    while (dataChangedSpy.count() < 2) {
        QCOMPARE(dataChangedSpy.wait(), true);
    }

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 2);

#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("Title"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("Test"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("Artist"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1000);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("Title2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("Test2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("Artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1000);
#else
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("test.ogg"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral(""));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral(""));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), -1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), -1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("test2.ogg"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral(""));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral(""));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), -1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), -1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 0);
#endif
}

void MediaPlayListTest::enqueueEmpty()
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    myPlayList.enqueueOneEntry(DataTypes::EntryData{});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueueOneEntry(DataTypes::EntryData{});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueueOneEntry(DataTypes::EntryData{});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueueOneEntry(DataTypes::EntryData{});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueueOneEntry(DataTypes::EntryData{});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                        QStringLiteral("album2"), {}});

    QVERIFY(rowsAboutToBeRemovedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    myPlayList.removeRows(0, 1);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 2);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    myPlayList.enqueueOneEntry(DataTypes::EntryData{{{DataTypes::ElementTypeRole, ElisaUtils::Artist}}, QStringLiteral("artist1"), {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(rowsAboutToBeInsertedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 6);

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

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.rowCount(), 6);

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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    myPlayList.enqueueOneEntry(DataTypes::EntryData{{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
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
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
    myPlayList.enqueueOneEntry(DataTypes::EntryData{{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
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
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
    myPlayList.enqueueOneEntry(DataTypes::EntryData{{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
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
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
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
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
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
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
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
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
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
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
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
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
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
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
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
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myPlayList.enqueueRestoredEntries(
                QVariantList({QStringList({{},QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("1"), QStringLiteral("1"),{}}),
                              QStringList({{},QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("1"), QStringLiteral("1"),{}}),
                              QStringList({{},QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("2"), QStringLiteral("1"),{}}),
                              QStringList({{},QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("3"), QStringLiteral("1"),{}})
                             }));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    while (dataChangedSpy.count() < 4) {
        QCOMPARE(dataChangedSpy.wait(), true);
    }

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newEntryInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
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
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
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
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
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
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
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
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    myPlayList.moveRows({}, 0, 1, {}, 3);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
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
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
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
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
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
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
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
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    myPlayList.moveRows({}, 3, 1, {}, 0);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myPlayList.enqueueRestoredEntries(
                QVariantList({QStringList({{},QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("3"), QStringLiteral("3"),{}}),
                              QStringList({{},QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("3"), QStringLiteral("3"),{}}),
                              QStringList({{},QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("3"), QStringLiteral("3"),{}}),
                              QStringList({{},QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("3"), QStringLiteral("3"),{}})
                             }));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 4);
    QCOMPARE(newEntryInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 3);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    auto newTrack = DataTypes::TrackDataType{};

    newTrack[DataTypes::IdRole] = QStringLiteral("$29");
    newTrack[DataTypes::ParentIdRole] = QStringLiteral("0");
    newTrack[DataTypes::TitleRole] = QStringLiteral("track19");
    newTrack[DataTypes::ArtistRole] = QStringLiteral("artist2");
    newTrack[DataTypes::DurationRole] = QTime::fromMSecsSinceStartOfDay(29);
    newTrack[DataTypes::ResourceRole] = QUrl::fromLocalFile(QStringLiteral("/$29"));
    newTrack[DataTypes::FileModificationTime] = QDateTime::fromMSecsSinceEpoch(29);
    newTrack[DataTypes::ImageUrlRole] = QUrl::fromLocalFile(QStringLiteral("withoutAlbum"));
    newTrack[DataTypes::RatingRole] = 9;
    newTrack[DataTypes::IsSingleDiscAlbumRole] = true;
    newTrack[DataTypes::GenreRole] = QStringLiteral("genre1");
    newTrack[DataTypes::ComposerRole] = QStringLiteral("composer1");
    newTrack[DataTypes::LyricistRole] = QStringLiteral("lyricist1");
    newTrack[DataTypes::HasEmbeddedCover] = false;
    newTrack[DataTypes::ElementTypeRole] = ElisaUtils::Track;

    myDatabaseContent.insertTracksList({newTrack}, mNewCovers);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 0);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayList.enqueueRestoredEntries(QVariantList({QStringList({QStringLiteral("0"), QStringLiteral("track19"), QStringLiteral("artist2"), {}, {}, {}, QString::number(ElisaUtils::PlayListEntryType::Unknown)})}));

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 1);
    QCOMPARE(newEntryInListSpy.count(), 0);

    QCOMPARE(dataChangedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 1);
    QCOMPARE(newEntryInListSpy.count(), 0);

    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track19"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::AlbumRole).toString(), QString());
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 29);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
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
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
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
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
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
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"),
                                                                                QStringLiteral("album3"), 3, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
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
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    auto fithTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track9"), QStringLiteral("artist2"),
                                                                              QStringLiteral("album3"), 9, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, fithTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 5);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 5);
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
    QCOMPARE(dataChangedSpy.count(), 5);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 5);

    auto sixthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist7"),
                                                                               QStringLiteral("album3"), 1, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, sixthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 6);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 6);
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
    QCOMPARE(dataChangedSpy.count(), 6);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 6);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
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
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
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
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
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
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
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
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    myPlayList.moveRows({}, 0, 1, {}, 3);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"),
                                                                             QStringLiteral("album2"), 6, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, newTrackID}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
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
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
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
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
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
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
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
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
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
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    myPlayList.moveRows({}, 3, 1, {}, 0);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
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
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    connect(&myPlayList, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
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

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
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
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
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
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
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
    QCOMPARE(dataChangedSpy.count(), 3);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    auto fourthTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    myPlayList.enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
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
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    myPlayList.moveRows({}, 0, 1, {}, 2);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 1);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);

    myPlayList.moveRows({}, 2, 1, {}, 0);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 2);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 4);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 4);
}

CrashEnqueuePlayList::CrashEnqueuePlayList(MediaPlayList *list, QObject *parent) : QObject(parent), mList(list)
{
}

void CrashEnqueuePlayList::crashMediaPlayList()
{
    mList->data(mList->index(0, 0), MediaPlayList::ResourceRole);
}

QTEST_GUILESS_MAIN(MediaPlayListTest)

#include "moc_mediaplaylisttest.cpp"
