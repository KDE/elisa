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

#include <QSignalSpy>
#include <QTest>
#include <QUrl>
#include <QTime>
#include <QTemporaryFile>
#include <QAbstractItemModelTester>

using namespace Qt::Literals::StringLiterals;

using TestTrackData = QMap<MediaPlayList::ColumnsRoles, QVariant>;
using ListTestTrackData = QList<TestTrackData>;

static void validateTracks(const QAbstractItemModel *const playListModel, const ListTestTrackData &expectedTrackData)
{
    QVERIFY(playListModel);
    QCOMPARE(playListModel->rowCount(), expectedTrackData.size());
    for (auto i = 0; i < expectedTrackData.size(); ++ i) {
        const auto index = playListModel->index(i, 0);
        const auto &oneTrack = expectedTrackData.at(i);
        for (const auto &[columnRole, expectedData] : oneTrack.asKeyValueRange()) {
            QCOMPARE(playListModel->data(index, columnRole), expectedData);
        }
    }
};

MediaPlayListTest::MediaPlayListTest(QObject *parent) : QObject(parent)
{
}

void MediaPlayListTest::initTestCase()
{
    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QList<qlonglong>>("QList<qlonglong>");
    qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
    qRegisterMetaType<ElisaUtils::PlayListEntryType>("PlayListEntryType");
}

void MediaPlayListTest::init()
{
    mPlayList = new MediaPlayList;
    mModelTester = new QAbstractItemModelTester(mPlayList);
    mDatabaseContent = new DatabaseInterface;
    mListener = new TracksListener(mDatabaseContent);

    mRowsAboutToBeMovedSpy = new QSignalSpy(mPlayList, &MediaPlayList::rowsAboutToBeMoved);
    mRowsAboutToBeRemovedSpy = new QSignalSpy(mPlayList, &MediaPlayList::rowsAboutToBeRemoved);
    mRowsAboutToBeInsertedSpy = new QSignalSpy(mPlayList, &MediaPlayList::rowsAboutToBeInserted);
    mRowsMovedSpy = new QSignalSpy(mPlayList, &MediaPlayList::rowsMoved);
    mRowsRemovedSpy = new QSignalSpy(mPlayList, &MediaPlayList::rowsRemoved);
    mRowsInsertedSpy = new QSignalSpy(mPlayList, &MediaPlayList::rowsInserted);
    mDataChangedSpy = new QSignalSpy(mPlayList, &MediaPlayList::dataChanged);

    mNewEntryInListSpy = new QSignalSpy(mPlayList, &MediaPlayList::newEntryInList);
    mNewTrackByNameInListSpy = new QSignalSpy(mPlayList, &MediaPlayList::newTrackByNameInList);
    mNewUrlInListSpy = new QSignalSpy(mPlayList, &MediaPlayList::newUrlInList);

    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);

    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 0);

    mDatabaseContent->init(QStringLiteral("testDbDirectContent"));

    connect(mListener, &TracksListener::trackHasChanged,
            mPlayList, &MediaPlayList::trackChanged,
            Qt::QueuedConnection);
    connect(mListener, &TracksListener::tracksListAdded,
            mPlayList, &MediaPlayList::tracksListAdded,
            Qt::QueuedConnection);
    connect(mPlayList, &MediaPlayList::newEntryInList,
            mListener, &TracksListener::newEntryInList,
            Qt::QueuedConnection);
    connect(mPlayList, &MediaPlayList::newUrlInList,
            mListener, &TracksListener::newUrlInList,
            Qt::QueuedConnection);
    connect(mPlayList, &MediaPlayList::newTrackByNameInList,
            mListener, &TracksListener::trackByNameInList,
            Qt::QueuedConnection);
    connect(mDatabaseContent, &DatabaseInterface::tracksAdded,
            mListener, &TracksListener::tracksAdded);

    mDatabaseContent->insertTracksList(mNewTracks);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);

    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 0);
}

void MediaPlayListTest::cleanup()
{
    delete mPlayList;
    delete mModelTester;
    delete mDatabaseContent;
    delete mListener;

    delete mRowsAboutToBeMovedSpy;
    delete mRowsAboutToBeRemovedSpy;
    delete mRowsAboutToBeInsertedSpy;
    delete mRowsMovedSpy;
    delete mRowsRemovedSpy;
    delete mRowsInsertedSpy;
    delete mDataChangedSpy;

    delete mNewTrackByNameInListSpy;
    delete mNewEntryInListSpy;
    delete mNewUrlInListSpy;
}

void MediaPlayListTest::simpleInitialCase()
{
    auto newTrackID = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
    auto newTrackData = mDatabaseContent->trackDataFromDatabaseId(newTrackID);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, newTrackID}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);
}

void MediaPlayListTest::enqueueAlbumCase()
{
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, mDatabaseContent->albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                        QStringLiteral("album2"), {}});

    QVERIFY(mRowsAboutToBeRemovedSpy->wait());

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayList->rowCount(), 6);

    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
}

void MediaPlayListTest::enqueueArtistCase()
{
    mPlayList->enqueueOneEntry({{{DataTypes::ElementTypeRole, ElisaUtils::Artist}}, QStringLiteral("artist1"), {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayList->rowCount(), 1);

    QCOMPARE(mRowsAboutToBeInsertedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayList->rowCount(), 7);

    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::AlbumIdRole).toULongLong(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
}

void MediaPlayListTest::enqueueTrackByUrl()
{
    auto newTrackID = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
    auto trackData = mDatabaseContent->trackDataFromDatabaseId(newTrackID);
    mPlayList->enqueueMultipleEntries({{{}, {}, trackData.resourceURI()}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 1);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 1);
}

void MediaPlayListTest::enqueueTracksByUrl()
{
    auto firstNewTrackID = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
    auto firstTrackData = mDatabaseContent->trackDataFromDatabaseId(firstNewTrackID);
    auto secondNewTrackID = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    auto secondTrackData = mDatabaseContent->trackDataFromDatabaseId(secondNewTrackID);
    mPlayList->enqueueMultipleEntries({
        {{}, {}, firstTrackData.resourceURI()},
        {{}, {}, secondTrackData.resourceURI()}
    });

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 2);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 2);
}

void MediaPlayListTest::enqueueFiles()
{
    mPlayList->enqueueMultipleEntries({
        {{}, {}, QUrl::fromLocalFile(QStringLiteral("/$1"))},
        {{}, {}, QUrl::fromLocalFile(QStringLiteral("/$2"))}
    });

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 2);

    QCOMPARE(mPlayList->rowCount(), 2);

    QCOMPARE(mDataChangedSpy->wait(300), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 2);

    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 2);
}

void MediaPlayListTest::enqueueSampleFiles()
{
    mPlayList->enqueueMultipleEntries({
        {{}, {}, QUrl::fromLocalFile(QStringLiteral(MEDIAPLAYLIST_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/test.ogg"))},
        {{}, {}, QUrl::fromLocalFile(QStringLiteral(MEDIAPLAYLIST_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/test2.ogg"))}
    });

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 2);

    QCOMPARE(mPlayList->rowCount(), 2);

    while (mDataChangedSpy->count() < 2) {
        QCOMPARE(mDataChangedSpy->wait(), true);
    }

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 2);

#if KFFileMetaData_FOUND
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("Title"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("Test"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("Artist"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1000);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("Title2"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("Test2"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("Artist2"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1000);
#else
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("test.ogg"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral(""));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral(""));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), -1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), -1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 0);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("test2.ogg"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral(""));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral(""));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), -1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), -1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 0);
#endif
}

void MediaPlayListTest::enqueueEmpty()
{
    mPlayList->enqueueOneEntry(DataTypes::EntryData{});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    mPlayList->enqueueOneEntry(DataTypes::EntryData{});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    mPlayList->enqueueOneEntry(DataTypes::EntryData{});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    mPlayList->enqueueOneEntry(DataTypes::EntryData{});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    mPlayList->enqueueOneEntry(DataTypes::EntryData{});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
}

void MediaPlayListTest::enqueueAtIndex()
{
    const QList<DataTypes::EntryData> trackEntries = {
            {{{DataTypes::ResourceRole, QUrl::fromLocalFile(u"/$1"_s)}}, {}, {}},
            {{{DataTypes::ResourceRole, QUrl::fromLocalFile(u"/$2"_s)}}, {}, {}},
            {{{DataTypes::ResourceRole, QUrl::fromLocalFile(u"/$3"_s)}}, {}, {}},
    };
    mPlayList->enqueueMultipleEntries(trackEntries);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 3);
    QCOMPARE(mPlayList->rowCount(), 3);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 3);
    QCOMPARE(mPlayList->rowCount(), 3);

    validateTracks(mPlayList, {
        {{MediaPlayList::TitleRole, u"track1"_s}},
        {{MediaPlayList::TitleRole, u"track2"_s}},
        {{MediaPlayList::TitleRole, u"track3"_s}},
    });

    const QList<DataTypes::EntryData> trackEntries2 = {
            {{{DataTypes::ResourceRole, QUrl::fromLocalFile(u"/$4"_s)}}, {}, {}},
            {{{DataTypes::ResourceRole, QUrl::fromLocalFile(u"/$9"_s)}}, {}, {}},
    };
    mPlayList->enqueueMultipleEntries(trackEntries2, 2);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 5);
    QCOMPARE(mPlayList->rowCount(), 5);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 5);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 5);
    QCOMPARE(mPlayList->rowCount(), 5);

    validateTracks(mPlayList, {
        {{MediaPlayList::TitleRole, u"track1"_s}},
        {{MediaPlayList::TitleRole, u"track2"_s}},
        {{MediaPlayList::TitleRole, u"track4"_s}},
        {{MediaPlayList::TitleRole, u"track5"_s}},
        {{MediaPlayList::TitleRole, u"track3"_s}},
    });

    const QList<DataTypes::EntryData> trackEntries3 = {
            {{{DataTypes::ResourceRole, QUrl::fromLocalFile(u"/$10"_s)}}, {}, {}},
            {{{DataTypes::ResourceRole, QUrl::fromLocalFile(u"/$22"_s)}}, {}, {}},
    };
    mPlayList->enqueueMultipleEntries(trackEntries3, mPlayList->rowCount() + 100);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 5);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 7);
    QCOMPARE(mPlayList->rowCount(), 7);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 7);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 7);
    QCOMPARE(mPlayList->rowCount(), 7);

    validateTracks(mPlayList, {
        {{MediaPlayList::TitleRole, u"track1"_s}},
        {{MediaPlayList::TitleRole, u"track2"_s}},
        {{MediaPlayList::TitleRole, u"track4"_s}},
        {{MediaPlayList::TitleRole, u"track5"_s}},
        {{MediaPlayList::TitleRole, u"track3"_s}},
        {{MediaPlayList::TitleRole, u"track6"_s}},
        {{MediaPlayList::TitleRole, u"track9"_s}},
    });
}

void MediaPlayListTest::removeFirstTrackOfAlbum()
{
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, mDatabaseContent->albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                        QStringLiteral("album2"), {}});

    QVERIFY(mRowsAboutToBeRemovedSpy->wait());

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    mPlayList->removeRows(0, 1);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 2);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 2);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);
}

void MediaPlayListTest::testTrackBeenRemoved()
{
    mPlayList->enqueueOneEntry(DataTypes::EntryData{{{DataTypes::ElementTypeRole, ElisaUtils::Artist}}, QStringLiteral("artist1"), {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mRowsAboutToBeInsertedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayList->rowCount(), 7);

    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 6);
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);

    auto removedTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"),
                                                                                 QStringLiteral("album2"), 2, 1);

    QCOMPARE(removedTrackId != 0, true);

    auto removedTrack = mDatabaseContent->trackDataFromDatabaseId(removedTrackId);

    QVERIFY(!removedTrack.isEmpty());

    mPlayList->trackRemoved(removedTrack[DataTypes::DatabaseIdRole].toULongLong());

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayList->rowCount(), 7);

    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), false);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), -1);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 0);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayList->data(mPlayList->index(4, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayList->data(mPlayList->index(5, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 6);
    QCOMPARE(mPlayList->data(mPlayList->index(6, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
}

void MediaPlayListTest::testSetData()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    mPlayList->enqueueOneEntry(DataTypes::EntryData{{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
    mPlayList->enqueueOneEntry(DataTypes::EntryData{{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
    mPlayList->enqueueOneEntry(DataTypes::EntryData{{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    QCOMPARE(mPlayList->setData(mPlayList->index(2, 0), true, MediaPlayList::ColumnsRoles::IsPlayingRole), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), true);

    QCOMPARE(mPlayList->setData(mPlayList->index(2, 0), true, MediaPlayList::ColumnsRoles::SecondaryTextRole), false);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mPlayList->setData(mPlayList->index(4, 0), true, MediaPlayList::ColumnsRoles::TitleRole), false);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mPlayList->setData({}, true, MediaPlayList::ColumnsRoles::TitleRole), false);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);
}

void MediaPlayListTest::testHasHeader()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayList->removeRows(2, 1);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
}

void MediaPlayListTest::testHasHeaderWithRemove()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayList->removeRows(2, 1);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayList->removeRows(0, 1);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 2);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 2);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
}

void MediaPlayListTest::testHasHeaderWithRestore()
{
    mDatabaseContent->clearData();

    mPlayList->enqueueRestoredEntries(
                QVariantList({QStringList({{},QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("1"), QStringLiteral("1"),{}}),
                              QStringList({{},QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("1"), QStringLiteral("1"),{}}),
                              QStringList({{},QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("2"), QStringLiteral("1"),{}}),
                              QStringList({{},QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("3"), QStringLiteral("1"),{}})
                             }));

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 4);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    mDatabaseContent->insertTracksList(mNewTracks);

    while (mDataChangedSpy->count() < 4) {
        QCOMPARE(mDataChangedSpy->wait(), true);
    }

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 4);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
}

void MediaPlayListTest::testHasHeaderMoveFirst()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayList->moveRows({}, 0, 1, {}, 3);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 1);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
}

void MediaPlayListTest::testHasHeaderMoveAnother()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayList->moveRows({}, 3, 1, {}, 0);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 1);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
}

void MediaPlayListTest::restoreMultipleIdenticalTracks()
{
    mDatabaseContent->clearData();

    mPlayList->enqueueRestoredEntries(
                QVariantList({QStringList({{},QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("3"), QStringLiteral("3"),{}}),
                              QStringList({{},QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("3"), QStringLiteral("3"),{}}),
                              QStringList({{},QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("3"), QStringLiteral("3"),{}}),
                              QStringList({{},QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("3"), QStringLiteral("3"),{}})
                             }));

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 4);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    mDatabaseContent->insertTracksList(mNewTracks);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 4);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(mPlayList->data(mPlayList->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 3);
}

void MediaPlayListTest::restoreTrackWithoutAlbum()
{
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

    mDatabaseContent->insertTracksList({newTrack});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    mPlayList->enqueueRestoredEntries(QVariantList({QStringList({QStringLiteral("0"), QStringLiteral("track19"), QStringLiteral("artist2"), {}, {}, {}, QString::number(ElisaUtils::PlayListEntryType::Unknown)})}));

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 1);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 1);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track19"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::AlbumRole).toString(), QString());
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 0);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 0);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 29);
}

void MediaPlayListTest::restoreLocalTrack()
{
    mPlayList->enqueueRestoredEntries(QVariantList({QStringList({{},
                                                                 QStringLiteral("Title"),
                                                                 QStringLiteral("Artist"),
                                                                 QStringLiteral("Test"),
                                                                 {},
                                                                 {},
                                                                 QString::number(ElisaUtils::PlayListEntryType::Unknown),
                                                                 QUrl::fromLocalFile(QStringLiteral(MEDIAPLAYLIST_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/test.ogg")).toString()})}));

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("Title"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("Test"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("Artist"));
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1000);
    QCOMPARE(mPlayList->data(mPlayList->index(0, 0), MediaPlayList::IsValidRole).toBool(), true);
}

void MediaPlayListTest::testHasHeaderAlbumWithSameTitle()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"),
                                                                                QStringLiteral("album3"), 3, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    auto fithTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track9"), QStringLiteral("artist2"),
                                                                              QStringLiteral("album3"), 9, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, fithTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 5);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 5);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 5);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 5);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 5);
    QCOMPARE(mDataChangedSpy->count(), 5);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 5);

    auto sixthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist7"),
                                                                               QStringLiteral("album3"), 1, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, sixthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 6);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 6);
    QCOMPARE(mDataChangedSpy->count(), 5);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 6);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 6);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 6);
    QCOMPARE(mDataChangedSpy->count(), 6);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 6);
}

void MediaPlayListTest::testHasHeaderMoveFirstLikeQml()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayList->moveRows({}, 0, 1, {}, 3);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 1);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
}

void MediaPlayListTest::crashOnEnqueue()
{
    auto newTrackID = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"),
                                                                             QStringLiteral("album2"), 6, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, newTrackID}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);
}

void MediaPlayListTest::testHasHeaderMoveAnotherLikeQml()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayList->moveRows({}, 3, 1, {}, 0);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 1);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
}

void MediaPlayListTest::testHasHeaderYetAnotherMoveLikeQml()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, firstTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, thirdTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    mPlayList->enqueueOneEntry({{{DataTypes::DatabaseIdRole, fourthTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayList->moveRows({}, 0, 1, {}, 2);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 1);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayList->moveRows({}, 2, 1, {}, 0);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 2);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 2);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
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
