/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2020 Alexander Stippich <a.stippich@gmx.net>
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

#include "mediaplaylistproxymodeltest.h"
#include "mediaplaylisttestconfig.h"

#include "mediaplaylist.h"
#include "mediaplaylistproxymodel.h"
#include "databaseinterface.h"
#include "trackslistener.h"

#include "config-upnp-qt.h"

#include <QtTest>
#include <QUrl>
#include <QTime>
#include <QTemporaryFile>
#include <QAbstractItemModelTester>

MediaPlayListProxyModelTest::MediaPlayListProxyModelTest(QObject *parent) : QObject(parent)
{
}

void MediaPlayListProxyModelTest::initTestCase()
{
    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
    qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
    qRegisterMetaType<ElisaUtils::PlayListEntryType>("PlayListEntryType");
}

void MediaPlayListProxyModelTest::simpleInitialCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayList::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
    auto newTrackData = myDatabaseContent.trackDataFromDatabaseId(newTrackID);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, newTrackID}}, {}, {}}, ElisaUtils::Track);

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

void MediaPlayListProxyModelTest::enqueueAlbumCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
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

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                        QStringLiteral("album2"), {}},
                       ElisaUtils::Album);

    QVERIFY(rowsAboutToBeInsertedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.rowCount(), 6);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
}

void MediaPlayListProxyModelTest::enqueueArtistCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayListProxyModel.enqueue({{}, QStringLiteral("artist1"), {}}, ElisaUtils::Artist);

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

    QCOMPARE(myPlayListProxyModel.rowCount(), 1);

    QCOMPARE(rowsAboutToBeInsertedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.rowCount(), 6);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumIdRole).toULongLong(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
}

void MediaPlayListProxyModelTest::enqueueMultipleAlbumsCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
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

    myPlayListProxyModel.enqueue({{{{DataTypes::DatabaseIdRole, myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                         QStringLiteral("album2"), {}},
                        {{{DataTypes::DatabaseIdRole, myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist2"), QStringLiteral("/"))}},
                         QStringLiteral("album3"), {}}},
                       ElisaUtils::Album);

    QVERIFY(rowsAboutToBeInsertedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 2);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(myPlayListProxyModel.rowCount(), 9);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(6, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(6, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(6, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(6, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(6, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(6, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 11);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(6, 0), MediaPlayList::AlbumIdRole).toULongLong(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(7, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(7, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(7, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(7, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(7, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(7, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 12);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(7, 0), MediaPlayList::AlbumIdRole).toULongLong(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(8, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(8, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(8, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(8, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(8, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(8, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 13);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(8, 0), MediaPlayList::AlbumIdRole).toULongLong(), 3);
}

void MediaPlayListProxyModelTest::enqueueTrackByUrl()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 0);

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
    auto trackData = myDatabaseContent.trackDataFromDatabaseId(newTrackID);
    myPlayListProxyModel.enqueue({{}, {}, trackData.resourceURI()}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
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
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 1);
}

void MediaPlayListProxyModelTest::enqueueTracksByUrl()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 0);

    auto firstNewTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
    auto firstTrackData = myDatabaseContent.trackDataFromDatabaseId(firstNewTrackID);
    auto secondNewTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    auto secondTrackData = myDatabaseContent.trackDataFromDatabaseId(secondNewTrackID);
    myPlayListProxyModel.enqueue({{{}, {}, firstTrackData.resourceURI()}, {{}, {}, secondTrackData.resourceURI()}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
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
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 2);
}

void MediaPlayListProxyModelTest::enqueueReplaceAndPlay()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"),
                                                                               QStringLiteral("album2"), 6, 1);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, firstTrackID}}, {}, {}}, ElisaUtils::Track);

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

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}, ElisaUtils::Track);

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

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/"))}},
                        QStringLiteral("album1"), {}},
                       ElisaUtils::Album,
                       ElisaUtils::ReplacePlayList,
                       ElisaUtils::TriggerPlay);

    QVERIFY(rowsAboutToBeInsertedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 2);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 2);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 3);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 4);
}

void MediaPlayListProxyModelTest::removeFirstTrackOfAlbum()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                        QStringLiteral("album2"), {}},
                       ElisaUtils::Album);

    QVERIFY(rowsAboutToBeInsertedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    myPlayListProxyModel.removeRow(0);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 2);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
}

void MediaPlayListProxyModelTest::testSaveLoadPlayList()
{
    MediaPlayList myPlayListSave;
    MediaPlayListProxyModel myPlayListProxyModelSave;
    myPlayListProxyModelSave.setPlayListModel(&myPlayListSave);
    QAbstractItemModelTester testModelSave(&myPlayListProxyModelSave);
    DatabaseInterface myDatabaseContent;
    TracksListener myListenerSave(&myDatabaseContent);
    MediaPlayList myPlayListRestore;
    MediaPlayListProxyModel myPlayListProxyModelRestore;
    myPlayListProxyModelRestore.setPlayListModel(&myPlayListRestore);
    QAbstractItemModelTester testModelRestore(&myPlayListProxyModelRestore);
    TracksListener myListenerRestore(&myDatabaseContent);

    QSignalSpy currentTrackChangedSaveSpy(&myPlayListProxyModelSave, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy randomPlayChangedSaveSpy(&myPlayListProxyModelSave, &MediaPlayListProxyModel::randomPlayChanged);
    QSignalSpy repeatPlayChangedSaveSpy(&myPlayListProxyModelSave, &MediaPlayListProxyModel::repeatPlayChanged);
    QSignalSpy playListFinishedSaveSpy(&myPlayListProxyModelSave, &MediaPlayListProxyModel::playListFinished);
    QSignalSpy playListLoadedSaveSpy(&myPlayListProxyModelSave, &MediaPlayListProxyModel::playListLoaded);
    QSignalSpy playListLoadFailedSaveSpy(&myPlayListProxyModelSave, &MediaPlayListProxyModel::playListLoadFailed);
    QSignalSpy dataChangedSaveSpy(&myPlayListProxyModelSave, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy currentTrackChangedRestoreSpy(&myPlayListProxyModelRestore, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy randomPlayChangedRestoreSpy(&myPlayListProxyModelRestore, &MediaPlayListProxyModel::randomPlayChanged);
    QSignalSpy repeatPlayChangedRestoreSpy(&myPlayListProxyModelRestore, &MediaPlayListProxyModel::repeatPlayChanged);
    QSignalSpy playListFinishedRestoreSpy(&myPlayListProxyModelRestore, &MediaPlayListProxyModel::playListFinished);
    QSignalSpy playListLoadedRestoreSpy(&myPlayListProxyModelRestore, &MediaPlayListProxyModel::playListLoaded);
    QSignalSpy playListLoadFailedRestoreSpy(&myPlayListProxyModelRestore, &MediaPlayListProxyModel::playListLoadFailed);

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
    connect(&myPlayListSave, &MediaPlayList::newUrlInList,
            &myListenerSave, &TracksListener::newUrlInList,
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
    connect(&myPlayListRestore, &MediaPlayList::newUrlInList,
            &myListenerRestore, &TracksListener::newUrlInList,
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

    myPlayListProxyModelSave.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModelSave.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModelSave.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                        QStringLiteral("track5"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModelSave.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QVERIFY(dataChangedSaveSpy.wait());

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

    QCOMPARE(myPlayListProxyModelSave.currentTrack(), QPersistentModelIndex(myPlayListProxyModelSave.index(0, 0)));

    QTemporaryFile playlistFile(QStringLiteral("./myPlaylistXXXXXX.m3u"));
    playlistFile.open();

    QCOMPARE(myPlayListProxyModelSave.savePlayList(QUrl::fromLocalFile(playlistFile.fileName())), true);

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

    myPlayListProxyModelRestore.loadPlayList(QUrl::fromLocalFile(playlistFile.fileName()));

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

    QCOMPARE(currentTrackChangedRestoreSpy.wait(), true);

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

    QCOMPARE(myPlayListProxyModelRestore.currentTrack(), QPersistentModelIndex(myPlayListProxyModelRestore.index(0, 0)));
}

void MediaPlayListProxyModelTest::testSavePersistentState()
{
    MediaPlayList myPlayListSave;
    QAbstractItemModelTester testModelSave(&myPlayListSave);
    MediaPlayListProxyModel myPlayListSaveProxyModel;
    myPlayListSaveProxyModel.setPlayListModel(&myPlayListSave);
    QAbstractItemModelTester testProxyModelSave(&myPlayListSaveProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListenerSave(&myDatabaseContent);
    MediaPlayList myPlayListRead;
    QAbstractItemModelTester testModelRead(&myPlayListRead);
    MediaPlayListProxyModel myPlayListReadProxyModel;
    myPlayListReadProxyModel.setPlayListModel(&myPlayListRead);
    QAbstractItemModelTester testProxyModelRead(&myPlayListReadProxyModel);
    TracksListener myListenerRead(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpySave(&myPlayListSaveProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpySave(&myPlayListSaveProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpySave(&myPlayListSaveProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpySave(&myPlayListSaveProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpySave(&myPlayListSaveProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpySave(&myPlayListSaveProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpySave(&myPlayListSaveProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpySave(&myPlayListSaveProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy rowsAboutToBeMovedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::dataChanged);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 0);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 0);
    QCOMPARE(persistentStateChangedSpySave.count(), 0);
    QCOMPARE(dataChangedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);

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
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    myPlayListSaveProxyModel.enqueue({{{DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 1);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 1);
    QCOMPARE(persistentStateChangedSpySave.count(), 1);
    QCOMPARE(dataChangedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);

    QCOMPARE(dataChangedSpySave.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 1);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 1);
    QCOMPARE(persistentStateChangedSpySave.count(), 1);
    QCOMPARE(dataChangedSpySave.count(), 1);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
    myPlayListSaveProxyModel.enqueue({{{DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 2);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 2);
    QCOMPARE(persistentStateChangedSpySave.count(), 2);
    QCOMPARE(dataChangedSpySave.count(), 1);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);

    QCOMPARE(dataChangedSpySave.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 2);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 2);
    QCOMPARE(persistentStateChangedSpySave.count(), 2);
    QCOMPARE(dataChangedSpySave.count(), 2);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
    myPlayListSaveProxyModel.enqueue({{{DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}, ElisaUtils::Track);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 3);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 3);
    QCOMPARE(persistentStateChangedSpySave.count(), 3);
    QCOMPARE(dataChangedSpySave.count(), 2);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);

    QCOMPARE(dataChangedSpySave.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 3);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 3);
    QCOMPARE(persistentStateChangedSpySave.count(), 3);
    QCOMPARE(dataChangedSpySave.count(), 3);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(dataChangedSpyRead.count(), 0);

    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 11);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album3")));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$11")));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), true);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$1")));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 12);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album3")));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$12")));
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListSaveProxyModel.data(myPlayListSaveProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), true);

    myPlayListReadProxyModel.setPersistentState(myPlayListSaveProxyModel.persistentState());

    QCOMPARE(dataChangedSpyRead.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpySave.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpySave.count(), 3);
    QCOMPARE(rowsRemovedSpySave.count(), 0);
    QCOMPARE(rowsMovedSpySave.count(), 0);
    QCOMPARE(rowsInsertedSpySave.count(), 3);
    QCOMPARE(persistentStateChangedSpySave.count(), 3);
    QCOMPARE(dataChangedSpySave.count(), 3);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 1);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 1);
    QCOMPARE(persistentStateChangedSpyRead.count(), 2);
    QCOMPARE(dataChangedSpyRead.count(), 6);

    QCOMPARE(myPlayListReadProxyModel.tracksCount(), 3);

    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 11);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album3")));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$11")));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), true);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$1")));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 12);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album3")));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$12")));
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListReadProxyModel.data(myPlayListReadProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), true);
}

void MediaPlayListProxyModelTest::testRestoreSettings()
{
    MediaPlayList myPlayListModel;
    MediaPlayListProxyModel myPlayList;
    myPlayList.setPlayListModel(&myPlayListModel);
    QAbstractItemModelTester testModel(&myPlayList);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayList, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayList, &MediaPlayListProxyModel::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayList, &MediaPlayListProxyModel::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayList, &MediaPlayListProxyModel::playListFinished);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayListModel, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(&myListener, &TracksListener::tracksListAdded,
            &myPlayListModel, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(&myPlayListModel, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(&myPlayListModel, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(&myPlayListModel, &MediaPlayList::newUrlInList,
            &myListener, &TracksListener::newUrlInList,
            Qt::QueuedConnection);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    myPlayList.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayList.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);
    myPlayList.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                        QStringLiteral("track5"), {}},
                       ElisaUtils::Track);
    myPlayList.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QVariantMap settings;
    settings[QStringLiteral("currentTrack")] = 2;
    settings[QStringLiteral("randomPlay")] = true;
    settings[QStringLiteral("repeatPlay")] = true;

    myPlayList.setPersistentState(settings);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);
}

void MediaPlayListProxyModelTest::testSaveAndRestoreSettings()
{
    MediaPlayList myPlayListSave;
    QAbstractItemModelTester testModelSave(&myPlayListSave);
    MediaPlayListProxyModel myPlayListSaveProxyModel;
    myPlayListSaveProxyModel.setPlayListModel(&myPlayListSave);
    QAbstractItemModelTester testProxyModelSave(&myPlayListSaveProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListenerSave(&myDatabaseContent);
    MediaPlayList myPlayListRestore;
    QAbstractItemModelTester testModelRestore(&myPlayListRestore);
    MediaPlayListProxyModel myPlayListRestoreProxyModel;
    myPlayListRestoreProxyModel.setPlayListModel(&myPlayListRestore);
    QAbstractItemModelTester testProxyModelRestore(&myPlayListRestoreProxyModel);
    TracksListener myListenerRestore(&myDatabaseContent);

    QSignalSpy currentTrackChangedSaveSpy(&myPlayListSaveProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy randomPlayChangedSaveSpy(&myPlayListSaveProxyModel, &MediaPlayListProxyModel::randomPlayChanged);
    QSignalSpy repeatPlayChangedSaveSpy(&myPlayListSaveProxyModel, &MediaPlayListProxyModel::repeatPlayChanged);
    QSignalSpy playListFinishedSaveSpy(&myPlayListSaveProxyModel, &MediaPlayListProxyModel::playListFinished);
    QSignalSpy currentTrackChangedRestoreSpy(&myPlayListRestoreProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy randomPlayChangedRestoreSpy(&myPlayListRestoreProxyModel, &MediaPlayListProxyModel::randomPlayChanged);
    QSignalSpy repeatPlayChangedRestoreSpy(&myPlayListRestoreProxyModel, &MediaPlayListProxyModel::repeatPlayChanged);
    QSignalSpy playListFinishedRestoreSpy(&myPlayListRestoreProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    myPlayListSaveProxyModel.setRepeatPlay(true);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 0);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    myPlayListSaveProxyModel.setRandomPlay(true);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 0);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    myPlayListSaveProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListSaveProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);
    myPlayListSaveProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                        QStringLiteral("track5"), {}},
                       ElisaUtils::Track);
    myPlayListSaveProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSaveSpy.count(), 1);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    QCOMPARE(myPlayListSaveProxyModel.currentTrack(), QPersistentModelIndex(myPlayListSaveProxyModel.index(0, 0)));

    myPlayListSaveProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSaveSpy.count(), 2);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    myPlayListSaveProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSaveSpy.count(), 3);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    myPlayListRestoreProxyModel.setPersistentState(myPlayListSaveProxyModel.persistentState());

    QCOMPARE(currentTrackChangedSaveSpy.count(), 3);
    QCOMPARE(randomPlayChangedSaveSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSaveSpy.count(), 1);
    QCOMPARE(playListFinishedSaveSpy.count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 1);
    QCOMPARE(randomPlayChangedRestoreSpy.count(), 1);
    QCOMPARE(repeatPlayChangedRestoreSpy.count(), 1);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    QCOMPARE(myPlayListRestoreProxyModel.repeatPlay(), true);
    QCOMPARE(myPlayListRestoreProxyModel.randomPlay(), true);
}

void MediaPlayListProxyModelTest::randomPlayList()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                        QStringLiteral("track5"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.setRepeatPlay(true);
    myPlayListProxyModel.setRandomPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.randomPlay(), true);

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);
}

void MediaPlayListProxyModelTest::testShuffleMode()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.setRepeatPlay(true);
    myPlayListProxyModel.setRandomPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.randomPlay(), true);
    QCOMPARE(myPlayListProxyModel.repeatPlay(), true);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                        QStringLiteral("track5"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.removeRow(1);
}

void MediaPlayListProxyModelTest::randomAndContinuePlayList()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                        QStringLiteral("track5"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.setRandomPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.randomPlay(), true);

    myPlayListProxyModel.setRepeatPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.repeatPlay(), true);

    myPlayListProxyModel.switchTo(3);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(3, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);
}

void MediaPlayListProxyModelTest::continuePlayList()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                        QStringLiteral("track5"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.setRepeatPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.repeatPlay(), true);

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(2, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(3, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
}

void MediaPlayListProxyModelTest::previousAndNextTracksTest()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy previousTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::previousTrackChanged);
    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy nextTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::nextTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(previousTrackChangedSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(nextTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 4)}},
                        QStringLiteral("track4"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2)}},
                        QStringLiteral("track2"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(previousTrackChangedSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(nextTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.previousTrack(), QPersistentModelIndex());
    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
    QCOMPARE(myPlayListProxyModel.nextTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(previousTrackChangedSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(nextTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.previousTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));
    QCOMPARE(myPlayListProxyModel.nextTrack(), QPersistentModelIndex(myPlayListProxyModel.index(2, 0)));

    myPlayListProxyModel.switchTo(4);

    QCOMPARE(previousTrackChangedSpy.count(), 2);
    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(nextTrackChangedSpy.count(), 3);

    QCOMPARE(myPlayListProxyModel.previousTrack(), QPersistentModelIndex(myPlayListProxyModel.index(3, 0)));
    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(4, 0)));
    QCOMPARE(myPlayListProxyModel.nextTrack(), QPersistentModelIndex());

    myPlayListProxyModel.setRepeatPlay(true);

    QCOMPARE(previousTrackChangedSpy.count(), 2);
    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(nextTrackChangedSpy.count(), 4);

    QCOMPARE(myPlayListProxyModel.previousTrack(), QPersistentModelIndex(myPlayListProxyModel.index(3, 0)));
    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(4, 0)));
    QCOMPARE(myPlayListProxyModel.nextTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(previousTrackChangedSpy.count(), 3);
    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(nextTrackChangedSpy.count(), 5);

    QCOMPARE(myPlayListProxyModel.previousTrack(), QPersistentModelIndex(myPlayListProxyModel.index(4, 0)));
    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
    QCOMPARE(myPlayListProxyModel.nextTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));

    myPlayListProxyModel.setRandomPlay(true);

    QVERIFY(myPlayListProxyModel.previousTrack() != QPersistentModelIndex());
    QVERIFY(myPlayListProxyModel.currentTrack() != QPersistentModelIndex());
    QVERIFY(myPlayListProxyModel.nextTrack() != QPersistentModelIndex());
}

void MediaPlayListProxyModelTest::remainingTracksTest()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::repeatPlayChanged);
    QSignalSpy remainingTracksChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::remainingTracksChanged);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 4)}},
                        QStringLiteral("track4"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2)}},
                        QStringLiteral("track2"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 6);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
    QCOMPARE(myPlayListProxyModel.remainingTracks(), 4);

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(remainingTracksChangedSpy.count(), 7);
    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));
    QCOMPARE(myPlayListProxyModel.remainingTracks(), 3);

    myPlayListProxyModel.setRandomPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 1);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 8);

    myPlayListProxyModel.setRandomPlay(false);
    myPlayListProxyModel.setRepeatPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 2);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 10);
}

void MediaPlayListProxyModelTest::clearPlayListCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy tracksCountChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::tracksCountChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(tracksCountChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                        QStringLiteral("album2"), {}},
                       ElisaUtils::Album);

    QVERIFY(rowsAboutToBeInsertedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(tracksCountChangedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);

    QCOMPARE(myPlayListProxyModel.rowCount(), 6);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.clearPlayList();

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 2);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(tracksCountChangedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 3);

    QCOMPARE(myPlayListProxyModel.rowCount(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack().isValid(), false);
}

void MediaPlayListProxyModelTest::undoClearPlayListCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy tracksCountChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::tracksCountChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy displayUndoNotificationSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::displayUndoNotification);
    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(tracksCountChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                        QStringLiteral("album2"), {}},
                       ElisaUtils::Album);

    QVERIFY(rowsAboutToBeInsertedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(tracksCountChangedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);

    QCOMPARE(myPlayListProxyModel.rowCount(), 6);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.clearPlayList();

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 2);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(tracksCountChangedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 4);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 3);

    QCOMPARE(myPlayListProxyModel.rowCount(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack().isValid(), false);

    myPlayListProxyModel.undoClearPlayList();

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 2);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(tracksCountChangedSpy.count(), 5);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 6);
    QCOMPARE(newTrackByNameInListSpy.count(), 6);
    QCOMPARE(newEntryInListSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(displayUndoNotificationSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.rowCount(), 6);

    QVERIFY(dataChangedSpy.wait());

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
}

void MediaPlayListProxyModelTest::undoReplacePlayListCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy tracksCountChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::tracksCountChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy displayUndoNotificationSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::displayUndoNotification);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(tracksCountChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(currentTrackChangedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                        QStringLiteral("album2"), {}},
                       ElisaUtils::Album);

    QVERIFY(rowsAboutToBeInsertedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(tracksCountChangedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);

    QCOMPARE(myPlayListProxyModel.rowCount(), 6);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/"))}},
                        QStringLiteral("album1"), {}},
                       ElisaUtils::Album,
                       ElisaUtils::ReplacePlayList,
                       ElisaUtils::TriggerPlay);

    QVERIFY(rowsAboutToBeInsertedSpy.wait());

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 3);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 3);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 7);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);
    QCOMPARE(currentTrackChangedSpy.count(), 5);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 4);

    myPlayListProxyModel.undoClearPlayList();

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 4);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 5);
    QCOMPARE(rowsRemovedSpy.count(), 4);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 5);
    QCOMPARE(tracksCountChangedSpy.count(), 9);
    QCOMPARE(persistentStateChangedSpy.count(), 10);
    QCOMPARE(dataChangedSpy.count(), 6);
    QCOMPARE(newTrackByNameInListSpy.count(), 6);
    QCOMPARE(newEntryInListSpy.count(), 2);
    QCOMPARE(currentTrackChangedSpy.count(), 7);
    QCOMPARE(displayUndoNotificationSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.rowCount(), 6);

    QVERIFY(dataChangedSpy.wait());

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndSkipPreviousAndContinueCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy randomPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::randomPlayChanged);
    QSignalSpy repeatPlayChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::repeatPlayChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.setRepeatPlay(true);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1)}},
                        QStringLiteral("track2"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2)}},
                        QStringLiteral("track2"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(2, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(3, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 5);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(4, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 6);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(5, 0)));

    myPlayListProxyModel.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 7);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(4, 0)));

    myPlayListProxyModel.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 8);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(3, 0)));

    myPlayListProxyModel.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 9);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(2, 0)));

    myPlayListProxyModel.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 10);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));

    myPlayListProxyModel.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 11);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 12);
    QCOMPARE(randomPlayChangedSpy.count(), 0);
    QCOMPARE(repeatPlayChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(5, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndRemoveMultipleNotBeginCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2)}},
                        QStringLiteral("track2"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 1)}},
                        QStringLiteral("track4"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));

    myPlayListProxyModel.removeRow(1);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndPlayCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndSkipNextCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndSkipPreviousCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));

    myPlayListProxyModel.skipPreviousTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
}

void MediaPlayListProxyModelTest::testRemoveSelection()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayListProxyModel.enqueue(ElisaUtils::EntryData{{}, QStringLiteral("artist1"), {}}, ElisaUtils::Artist, ElisaUtils::PlayListEnqueueMode::ReplacePlayList, ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);

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

    QCOMPARE(rowsAboutToBeInsertedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(),1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.tracksCount(), 6);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);

    myPlayListProxyModel.removeSelection({2, 4, 5});

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 4);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 4);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 6);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.tracksCount(), 3);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
}

void MediaPlayListProxyModelTest::testReplaceAndPlayArtist()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayListProxyModel.enqueue(ElisaUtils::EntryData{{}, QStringLiteral("artist3"), {}}, ElisaUtils::Artist, ElisaUtils::PlayListEnqueueMode::ReplacePlayList, ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);

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

    QCOMPARE(rowsAboutToBeInsertedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.tracksCount(), 1);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$3")));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);

    myPlayListProxyModel.enqueue(ElisaUtils::EntryData{{}, QStringLiteral("artist4"), {}}, ElisaUtils::Artist, ElisaUtils::PlayListEnqueueMode::ReplacePlayList, ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 2);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 3);
    QCOMPARE(rowsRemovedSpy.count(), 2);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 3);
    QCOMPARE(persistentStateChangedSpy.count(), 5);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(rowsAboutToBeInsertedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 3);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 4);
    QCOMPARE(rowsRemovedSpy.count(), 3);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 4);
    QCOMPARE(persistentStateChangedSpy.count(), 7);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 2);

    QCOMPARE(myPlayListProxyModel.tracksCount(), 1);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$4")));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);
}

void MediaPlayListProxyModelTest::testReplaceAndPlayTrackId()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"),
                                                                               QStringLiteral("album1"), 3, 3);

    QCOMPARE(firstTrackId != 0, true);

    myPlayListProxyModel.enqueue(ElisaUtils::EntryData{{{DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}, ElisaUtils::Track, ElisaUtils::PlayListEnqueueMode::ReplacePlayList, ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);

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

    QCOMPARE(myPlayListProxyModel.tracksCount(), 1);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$3")));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist4"),
                                                                                QStringLiteral("album1"), 4, 4);

    QCOMPARE(secondTrackId != 0, true);

    myPlayListProxyModel.enqueue(ElisaUtils::EntryData{{{DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}, ElisaUtils::Track, ElisaUtils::PlayListEnqueueMode::ReplacePlayList, ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);

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

    QCOMPARE(myPlayListProxyModel.tracksCount(), 1);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$4")));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);
}

void MediaPlayListProxyModelTest::testTrackBeenRemoved()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayListProxyModel.enqueue(ElisaUtils::EntryData{{}, QStringLiteral("artist1"), {}}, ElisaUtils::Artist, ElisaUtils::PlayListEnqueueMode::ReplacePlayList, ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);

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

    QCOMPARE(rowsAboutToBeInsertedSpy.wait(), true);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 1);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 2);
    QCOMPARE(rowsRemovedSpy.count(), 1);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 2);
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.tracksCount(), 6);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);

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
    QCOMPARE(persistentStateChangedSpy.count(), 3);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.tracksCount(), 6);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), false);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), -1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 0);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(3, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(4, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 5);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(5, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
}

void MediaPlayListProxyModelTest::finishPlayList()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
}

void MediaPlayListProxyModelTest::removeBeforeCurrentTrack()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                        QStringLiteral("track5"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(2, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(3, 0)));

    myPlayListProxyModel.removeRow(1);

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(playListFinishedSpy.count(), 0);
}

void MediaPlayListProxyModelTest::switchToTrackTest()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 4)}},
                        QStringLiteral("track4"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2)}},
                        QStringLiteral("track2"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.switchTo(4);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(4, 0)));
}

void MediaPlayListProxyModelTest::singleTrack()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndRemoveCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.removeRow(0);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex());
}

void MediaPlayListProxyModelTest::testBringUpAndRemoveLastCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1)}},
                        QStringLiteral("track2"), {}},
                       ElisaUtils::Track);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1)}},
                        QStringLiteral("track3"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(1, 0)));

    myPlayListProxyModel.skipNextTrack();

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(2, 0)));

    myPlayListProxyModel.removeRow(2);

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndRemoveMultipleCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2)}},
                        QStringLiteral("track2"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.removeRow(0);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndDownCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));

    myPlayListProxyModel.removeRow(0);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListFinishedSpy.count(), 1);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex());
}

void MediaPlayListProxyModelTest::testBringUpCase()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                        QStringLiteral("track1"), {}},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
}

void MediaPlayListProxyModelTest::testBringUpCaseFromNewAlbum()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy currentTrackChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy playListFinishedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);

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

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListFinishedSpy.count(), 0);

    auto newTrackID = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                             QStringLiteral("album2"), 1, 1);

    QVERIFY(newTrackID != 0);

    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, newTrackID}}, QStringLiteral("track1"), {}}, ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListFinishedSpy.count(), 0);

    QCOMPARE(myPlayListProxyModel.currentTrack(), QPersistentModelIndex(myPlayListProxyModel.index(0, 0)));
}

void MediaPlayListProxyModelTest::testSetData()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    myPlayListProxyModel.enqueue(ElisaUtils::EntryData{{{DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}, ElisaUtils::Track);

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

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    auto secondTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
    myPlayListProxyModel.enqueue(ElisaUtils::EntryData{{{DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}, ElisaUtils::Track);

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

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    auto thirdTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
    myPlayListProxyModel.enqueue(ElisaUtils::EntryData{{{DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}, ElisaUtils::Track);

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

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayListProxyModel.setData(myPlayListProxyModel.index(2, 0), true, MediaPlayList::ColumnsRoles::IsPlayingRole), true);

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

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), true);

    QCOMPARE(myPlayListProxyModel.setData(myPlayListProxyModel.index(2, 0), true, MediaPlayList::ColumnsRoles::SecondaryTextRole), false);

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

    QCOMPARE(myPlayListProxyModel.setData(myPlayListProxyModel.index(4, 0), true, MediaPlayList::ColumnsRoles::TitleRole), false);

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

    QCOMPARE(myPlayListProxyModel.setData({}, true, MediaPlayList::ColumnsRoles::TitleRole), false);

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

void MediaPlayListProxyModelTest::testHasHeader()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}, ElisaUtils::Track);

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

    myPlayListProxyModel.removeRow(2);

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

void MediaPlayListProxyModelTest::testHasHeaderWithRemove()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}, ElisaUtils::Track);

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

    myPlayListProxyModel.removeRow(2);

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

    myPlayListProxyModel.removeRow(0);

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

void MediaPlayListProxyModelTest::testHasHeaderMoveFirst()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}, ElisaUtils::Track);

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

    myPlayListProxyModel.moveRow(0, 3);

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

void MediaPlayListProxyModelTest::testHasHeaderMoveAnother()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}, ElisaUtils::Track);

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

    myPlayListProxyModel.moveRow(3, 0);

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

void MediaPlayListProxyModelTest::testHasHeaderAlbumWithSameTitle()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, fithTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, sixthTrackId}}, {}, {}}, ElisaUtils::Track);

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

void MediaPlayListProxyModelTest::testHasHeaderMoveFirstLikeQml()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}, ElisaUtils::Track);

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

    myPlayListProxyModel.moveRow(0, 3);

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

void MediaPlayListProxyModelTest::testHasHeaderMoveAnotherLikeQml()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}, ElisaUtils::Track);

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

    myPlayListProxyModel.moveRow(3, 0);

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

void MediaPlayListProxyModelTest::testHasHeaderYetAnotherMoveLikeQml()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayList::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    auto firstTrackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}, ElisaUtils::Track);

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
    myPlayListProxyModel.enqueue({{{DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}, ElisaUtils::Track);

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

    myPlayListProxyModel.moveRow(0, 2);

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

    myPlayListProxyModel.moveRow(2, 0);

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

void MediaPlayListProxyModelTest::enqueueFiles()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{}, {}, QUrl::fromLocalFile(QStringLiteral("/$1"))}, {{}, {}, QUrl::fromLocalFile(QStringLiteral("/$2"))}}, ElisaUtils::FileName);

    QCOMPARE(rowsAboutToBeRemovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpy.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpy.count(), 1);
    QCOMPARE(rowsRemovedSpy.count(), 0);
    QCOMPARE(rowsMovedSpy.count(), 0);
    QCOMPARE(rowsInsertedSpy.count(), 1);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(dataChangedSpy.count(), 1);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 2);

    QCOMPARE(myPlayListProxyModel.rowCount(), 2);

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
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 2);

    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 2);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 2);
}

void MediaPlayListProxyModelTest::enqueueSampleFiles()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 0);

    myPlayListProxyModel.enqueue({{{}, {}, QUrl::fromLocalFile(QStringLiteral(MEDIAPLAYLIST_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/test.ogg"))},
                        {{}, {}, QUrl::fromLocalFile(QStringLiteral(MEDIAPLAYLIST_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/test2.ogg"))}},
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
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 2);

    QCOMPARE(myPlayListProxyModel.rowCount(), 2);

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
    QCOMPARE(newEntryInListSpy.count(), 0);
    QCOMPARE(newUrlInListSpy.count(), 2);

#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("Title"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("Test"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("Artist"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1000);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("Title2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("Test2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("Artist2"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1000);
#else
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("test.ogg"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral(""));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral(""));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::TrackNumberRole).toInt(), -1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DiscNumberRole).toInt(), -1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 0);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("test2.ogg"));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral(""));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral(""));
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::TrackNumberRole).toInt(), -1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DiscNumberRole).toInt(), -1);
    QCOMPARE(myPlayListProxyModel.data(myPlayListProxyModel.index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 0);
#endif
}

void MediaPlayListProxyModelTest::enqueueEmpty()
{
    MediaPlayList myPlayList;
    QAbstractItemModelTester testModel(&myPlayList);
    MediaPlayListProxyModel myPlayListProxyModel;
    myPlayListProxyModel.setPlayListModel(&myPlayList);
    QAbstractItemModelTester testProxyModel(&myPlayListProxyModel);
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpy(&myPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy dataChangedSpy(&myPlayList, &MediaPlayListProxyModel::dataChanged);
    QSignalSpy newTrackByNameInListSpy(&myPlayList, &MediaPlayList::newTrackByNameInList);
    QSignalSpy newEntryInListSpy(&myPlayList, &MediaPlayList::newEntryInList);
    QSignalSpy newUrlInListSpy(&myPlayList, &MediaPlayList::newUrlInList);

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
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(dataChangedSpy.count(), 0);
    QCOMPARE(newTrackByNameInListSpy.count(), 0);
    QCOMPARE(newEntryInListSpy.count(), 0);

    myPlayListProxyModel.enqueue(ElisaUtils::EntryDataList{}, ElisaUtils::Track);

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

    myPlayListProxyModel.enqueue(ElisaUtils::EntryDataList{}, ElisaUtils::FileName);

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

    myPlayListProxyModel.enqueue(ElisaUtils::EntryDataList{}, ElisaUtils::Track, ElisaUtils::AppendPlayList, ElisaUtils::DoNotTriggerPlay);

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

    myPlayListProxyModel.enqueue(ElisaUtils::EntryDataList{}, ElisaUtils::Album, ElisaUtils::AppendPlayList, ElisaUtils::DoNotTriggerPlay);

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

    myPlayListProxyModel.enqueue(ElisaUtils::EntryDataList{},
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

QTEST_GUILESS_MAIN(MediaPlayListProxyModelTest)


#include "moc_mediaplaylistproxymodeltest.cpp"
