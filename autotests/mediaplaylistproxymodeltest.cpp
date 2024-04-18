/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "mediaplaylistproxymodeltest.h"
#include "mediaplaylisttestconfig.h"

#include "datatypes.h"
#include "mediaplaylist.h"
#include "mediaplaylistproxymodel.h"
#include "databaseinterface.h"
#include "trackslistener.h"

#include "elisa_settings.h"

#include "config-upnp-qt.h"

#include <QSignalSpy>
#include <QTest>
#include <QUrl>
#include <QTemporaryFile>
#include <QAbstractItemModelTester>

MediaPlayListProxyModelTest::MediaPlayListProxyModelTest(QObject *parent) : QObject(parent)
{
}

void MediaPlayListProxyModelTest::initTestCase()
{
    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QList<qlonglong>>("QList<qlonglong>");
    qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
    qRegisterMetaType<ElisaUtils::PlayListEntryType>("PlayListEntryType");
    Elisa::ElisaConfiguration::instance(QStringLiteral("testfoo"));
}

void MediaPlayListProxyModelTest::init()
{
    mPlayList = new MediaPlayList;
    mPlayListProxyModel = new MediaPlayListProxyModel;
    mPlayListProxyModel->setPlayListModel(mPlayList);
    mModelTester = new QAbstractItemModelTester(mPlayList);
    mProxyModelTester = new QAbstractItemModelTester(mPlayListProxyModel);
    mDatabaseContent = new DatabaseInterface;
    mListener = new TracksListener(mDatabaseContent);

    mRowsAboutToBeMovedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    mRowsAboutToBeRemovedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    mRowsAboutToBeInsertedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    mRowsMovedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::rowsMoved);
    mRowsRemovedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    mRowsInsertedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::rowsInserted);
    mDataChangedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::dataChanged);

    mCurrentTrackChangedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    mDisplayUndoNotificationSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::displayUndoNotification);
    mNextTrackChangedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::nextTrackChanged);
    mPersistentStateChangedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    mPlayListFinishedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::playListFinished);
    mPlayListLoadedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::playListLoaded);
    mPlayListLoadFailedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::playListLoadFailed);
    mPreviousTrackChangedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::previousTrackChanged);
    mRemainingTracksChangedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::remainingTracksChanged);
    mRepeatModeChangedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::repeatModeChanged);
    mShuffleModeChangedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::shuffleModeChanged);
    mTracksCountChangedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::tracksCountChanged);
    mTrackSeekedSpy = new QSignalSpy(mPlayListProxyModel, &MediaPlayListProxyModel::seek);

    mNewTrackByNameInListSpy = new QSignalSpy(mPlayList, &MediaPlayList::newTrackByNameInList);
    mNewEntryInListSpy = new QSignalSpy(mPlayList, &MediaPlayList::newEntryInList);
    mNewUrlInListSpy = new QSignalSpy(mPlayList, &MediaPlayList::newUrlInList);

    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 0);
    QCOMPARE(mDisplayUndoNotificationSpy->count(), 0);
    QCOMPARE(mNextTrackChangedSpy->count(), 0);
    QCOMPARE(mPersistentStateChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListLoadedSpy->count(), 0);
    QCOMPARE(mPlayListLoadFailedSpy->count(), 0);
    QCOMPARE(mPreviousTrackChangedSpy->count(), 0);
    QCOMPARE(mRemainingTracksChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mTracksCountChangedSpy->count(), 0);
    QCOMPARE(mTrackSeekedSpy->count(), 0);

    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
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

    mDatabaseContent->insertTracksList(mNewTracks, mNewCovers);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 0);
    QCOMPARE(mDisplayUndoNotificationSpy->count(), 0);
    QCOMPARE(mNextTrackChangedSpy->count(), 0);
    QCOMPARE(mPersistentStateChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListLoadedSpy->count(), 0);
    QCOMPARE(mPlayListLoadFailedSpy->count(), 0);
    QCOMPARE(mPreviousTrackChangedSpy->count(), 0);
    QCOMPARE(mRemainingTracksChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mTracksCountChangedSpy->count(), 0);
    QCOMPARE(mTrackSeekedSpy->count(), 0);

    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
}

void MediaPlayListProxyModelTest::cleanup()
{
    delete mPlayList;
    delete mPlayListProxyModel;
    delete mModelTester;
    delete mProxyModelTester;
    delete mDatabaseContent;
    delete mListener;

    delete mRowsAboutToBeMovedSpy;
    delete mRowsAboutToBeRemovedSpy;
    delete mRowsAboutToBeInsertedSpy;
    delete mRowsMovedSpy;
    delete mRowsRemovedSpy;
    delete mRowsInsertedSpy;
    delete mDataChangedSpy;

    delete mCurrentTrackChangedSpy;
    delete mDisplayUndoNotificationSpy;
    delete mNextTrackChangedSpy;
    delete mPersistentStateChangedSpy;
    delete mPlayListFinishedSpy;
    delete mPlayListLoadedSpy;
    delete mPlayListLoadFailedSpy;
    delete mPreviousTrackChangedSpy;
    delete mRemainingTracksChangedSpy;
    delete mRepeatModeChangedSpy;
    delete mShuffleModeChangedSpy;
    delete mTracksCountChangedSpy;
    delete mTrackSeekedSpy;

    delete mNewTrackByNameInListSpy;
    delete mNewEntryInListSpy;
    delete mNewUrlInListSpy;
}

void MediaPlayListProxyModelTest::m3uPlaylistParser_SimpleCase()
{
    PlaylistParser playlistParser;
    QList<QUrl> listOfUrls = playlistParser.fromPlaylist(QUrl(QStringLiteral("file:///home/n/a.m3u")), QStringLiteral("/home/n/Music/1.mp3\n/home/n/Music/2.mp3\n").toUtf8());

    QCOMPARE(listOfUrls.count(), 2);
}

void MediaPlayListProxyModelTest::m3uPlaylistParser_CommentCase()
{
    const char * asString = R"--(#EXTM3U
#EXTINF:-1 tvg-id="ArianaAfghanistanInternationalTV.us" status="online",Ariana Afghanistan International TV (720p) [Not 24/7]
http://iptv.arianaafgtv.com/ariana/playlist.m3u8
#EXTINF:-1 tvg-id="ArianaTVNational.af" status="online",Ariana TV National (720p) [Not 24/7]
https://d10rltuy0iweup.cloudfront.net/ATNNAT/myStream/playlist.m3u8
)--";
    PlaylistParser playlistParser;
    QList<QUrl> listOfUrls = playlistParser.fromPlaylist(QUrl(QStringLiteral("file:///home/n/a.m3u")), QString::fromUtf8(asString).toUtf8());

    QCOMPARE(listOfUrls.count(), 2);
}

void MediaPlayListProxyModelTest::m3uPlaylistParser_WindowsLineTerminator()
{
    const char * asString = R"--(#EXTM3U\r
#EXTINF:-1 tvg-id="ArianaAfghanistanInternationalTV.us" status="online",Ariana Afghanistan International TV (720p) [Not 24/7]\r
http://iptv.arianaafgtv.com/ariana/playlist.m3u8\r
#EXTINF:-1 tvg-id="ArianaTVNational.af" status="online",Ariana TV National (720p) [Not 24/7]\r
https://d10rltuy0iweup.cloudfront.net/ATNNAT/myStream/playlist.m3u8\r
)--";
    PlaylistParser playlistParser;
    QList<QUrl> listOfUrls = playlistParser.fromPlaylist(QUrl(QStringLiteral("file:///home/n/a.m3u")), QString::fromUtf8(asString).toUtf8());

    QCOMPARE(listOfUrls.count(), 2);
}

void MediaPlayListProxyModelTest::plsPlaylistParserCase()
{
    const char * asString = R"--([playlist]

File1=https://test.test-dom:8068
Length1=-1

File2=example2.mp3
Title2=Relative path
Length2=120

File3=/home/n/Music/1.mp3
Title3=Absolute path

NumberOfEntries=3
Version=2
)--";
    PlaylistParser playlistParser;
    QList<QUrl> listOfUrls = playlistParser.fromPlaylist(QUrl(QStringLiteral("file:///home/n/a.pls")), QString::fromUtf8(asString).toUtf8());

    QCOMPARE(listOfUrls.count(), 3);
}

void MediaPlayListProxyModelTest::plsPlaylistParser_WindowsLineTerminator()
{
    const char * asString = R"--([playlist]\r
\r
File1=https://test.test-dom:8068\r
Length1=-1\r

File2=example2.mp3\r
Title2=Relative path\r
Length2=120\r
\r
File3=/home/n/Music/1.mp3\r
Title3=Absolute path\r
\r
NumberOfEntries=3\r
Version=2\r
)--";
    PlaylistParser playlistParser;
    QList<QUrl> listOfUrls = playlistParser.fromPlaylist(QUrl(QStringLiteral("file:///home/n/a.pls")), QString::fromUtf8(asString).toUtf8());

    QCOMPARE(listOfUrls.count(), 3);
}

void MediaPlayListProxyModelTest::m3uPlaylistParser_ToPlaylist()
{
    PlaylistParser playlistParser;
    QList<QString> listOfUrls {QStringLiteral("/home/n/Music/1.mp3"), QStringLiteral("/home/n/Music/2.mp3")};
    QString fileContent = playlistParser.toPlaylist(QUrl(QStringLiteral("file:///home/n/a.m3u")), listOfUrls);

    QCOMPARE(fileContent.count(QLatin1Char('\n')), 2);
}

void MediaPlayListProxyModelTest::plsPlaylistParser_ToPlaylist()
{
    PlaylistParser playlistParser;
    QList<QString> listOfUrls {QStringLiteral("/home/n/Music/1.mp3"), QStringLiteral("/home/n/Music/2.mp3")};
    QString fileContent = playlistParser.toPlaylist(QUrl(QStringLiteral("file:///home/n/a.pls")), listOfUrls);

    QCOMPARE(fileContent.count(QStringLiteral("\nFile")), 2);
}

void MediaPlayListProxyModelTest::simpleInitialCase()
{
    auto newTrackID = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
    auto newTrackData = mDatabaseContent->trackDataFromDatabaseId(newTrackID);
    mPlayListProxyModel->enqueue({{{{DataTypes::DatabaseIdRole, newTrackID}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);
}

void MediaPlayListProxyModelTest::enqueueAlbumCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Album}, {DataTypes::DatabaseIdRole, mDatabaseContent->albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                                   QStringLiteral("album2"), {}}}, {}, {});

    QVERIFY(mRowsAboutToBeInsertedSpy->wait());

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->rowCount(), 6);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
}

void MediaPlayListProxyModelTest::enqueueArtistCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Artist}}, QStringLiteral("artist1"), {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->rowCount(), 1);

    QCOMPARE(mRowsAboutToBeInsertedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->rowCount(), 6);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumIdRole).toULongLong(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
}

void MediaPlayListProxyModelTest::enqueueMultipleAlbumsCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Album},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                                   QStringLiteral("album2"), {}},
                                  {{{DataTypes::ElementTypeRole, ElisaUtils::Album},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->albumIdFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist2"), QStringLiteral("/"))}},
                                   QStringLiteral("album3"), {}}}, {}, {});

    QVERIFY(mRowsAboutToBeInsertedSpy->wait());

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 2);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 2);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 5);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mPlayListProxyModel->rowCount(), 9);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::AlbumIdRole).toULongLong(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(6, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(6, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(6, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(6, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(6, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(6, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 11);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(6, 0), MediaPlayList::AlbumIdRole).toULongLong(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(7, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(7, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(7, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(7, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(7, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(7, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 12);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(7, 0), MediaPlayList::AlbumIdRole).toULongLong(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(8, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(8, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(8, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(8, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(8, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(8, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 13);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(8, 0), MediaPlayList::AlbumIdRole).toULongLong(), 3);
}

void MediaPlayListProxyModelTest::enqueueTrackByUrl()
{
    auto newTrackID = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
    auto trackData = mDatabaseContent->trackDataFromDatabaseId(newTrackID);
    mPlayListProxyModel->enqueue({{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                  {DataTypes::ResourceRole, trackData.resourceURI()}}, {}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 1);
}

void MediaPlayListProxyModelTest::enqueueTracksByUrl()
{
    auto firstNewTrackID = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);
    auto firstTrackData = mDatabaseContent->trackDataFromDatabaseId(firstNewTrackID);
    auto secondNewTrackID = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    auto secondTrackData = mDatabaseContent->trackDataFromDatabaseId(secondNewTrackID);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::ResourceRole, firstTrackData.resourceURI()}}, {}, {}},
                                  {{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::ResourceRole, secondTrackData.resourceURI()}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 2);
}

void MediaPlayListProxyModelTest::enqueueReplaceAndPlay()
{
    auto firstTrackID = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"),
                                                                               QStringLiteral("album2"), 6, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::DatabaseIdRole, firstTrackID},{DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::DatabaseIdRole, secondTrackId}, {DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Album},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/"))}},
                                   QStringLiteral("album1"), {}}},
                                 ElisaUtils::ReplacePlayList,
                                 ElisaUtils::TriggerPlay);

    QVERIFY(mRowsAboutToBeInsertedSpy->wait());

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 2);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 2);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 6);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 4);
}

void MediaPlayListProxyModelTest::removeFirstTrackOfAlbum()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Album},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                                   QStringLiteral("album2"), {}}}, {}, {});

    QVERIFY(mRowsAboutToBeInsertedSpy->wait());

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    mPlayListProxyModel->removeRow(0);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 2);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 2);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);
}

void MediaPlayListProxyModelTest::testSaveLoadPlayList()
{
    MediaPlayList myPlayListRestore;
    MediaPlayListProxyModel myPlayListProxyModelRestore;
    myPlayListProxyModelRestore.setPlayListModel(&myPlayListRestore);
    QAbstractItemModelTester testModelRestore(&myPlayListProxyModelRestore);
    TracksListener myListenerRestore(mDatabaseContent);

    QSignalSpy currentTrackChangedRestoreSpy(&myPlayListProxyModelRestore, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy shuffleModeChangedRestoreSpy(&myPlayListProxyModelRestore, &MediaPlayListProxyModel::shuffleModeChanged);
    QSignalSpy repeatModeChangedRestoreSpy(&myPlayListProxyModelRestore, &MediaPlayListProxyModel::repeatModeChanged);
    QSignalSpy playListFinishedRestoreSpy(&myPlayListProxyModelRestore, &MediaPlayListProxyModel::playListFinished);
    QSignalSpy playListLoadedRestoreSpy(&myPlayListProxyModelRestore, &MediaPlayListProxyModel::playListLoaded);
    QSignalSpy playListLoadFailedRestoreSpy(&myPlayListProxyModelRestore, &MediaPlayListProxyModel::playListLoadFailed);

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
    connect(mDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListenerRestore, &TracksListener::tracksAdded);

    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(shuffleModeChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatModeChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadFailedRestoreSpy.count(), 0);

    mPlayListProxyModel->enqueue(
        {{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
           {DataTypes::ResourceRole, QUrl::fromLocalFile(QStringLiteral(MEDIAPLAYLIST_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/test.ogg"))}},
          {},
          {}},
         {{{DataTypes::ElementTypeRole, ElisaUtils::Track},
           {DataTypes::ResourceRole, QUrl::fromLocalFile(QStringLiteral(MEDIAPLAYLIST_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/test2.ogg"))}},
          {},
          {}}},
        {},
        {});

    QVERIFY(mDataChangedSpy->wait());

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListLoadedSpy->count(), 0);
    QCOMPARE(mPlayListLoadFailedSpy->count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(shuffleModeChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatModeChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadFailedRestoreSpy.count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    QTemporaryFile playlistFile(QStringLiteral("./myPlaylistXXXXXX.m3u8"));
    playlistFile.open();

    QCOMPARE(mPlayListProxyModel->savePlayList(QUrl::fromLocalFile(playlistFile.fileName())), true);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListLoadedSpy->count(), 0);
    QCOMPARE(mPlayListLoadFailedSpy->count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(shuffleModeChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatModeChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadFailedRestoreSpy.count(), 0);

    myPlayListProxyModelRestore.loadPlayList(QUrl::fromLocalFile(playlistFile.fileName()));

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListLoadedSpy->count(), 0);
    QCOMPARE(mPlayListLoadFailedSpy->count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(shuffleModeChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatModeChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadedRestoreSpy.count(), 1);
    QCOMPARE(playListLoadFailedRestoreSpy.count(), 0);

    QCOMPARE(currentTrackChangedRestoreSpy.wait(), true);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListLoadedSpy->count(), 0);
    QCOMPARE(mPlayListLoadFailedSpy->count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 1);
    QCOMPARE(shuffleModeChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatModeChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);
    QCOMPARE(playListLoadedRestoreSpy.count(), 1);
    QCOMPARE(playListLoadFailedRestoreSpy.count(), 0);

    QCOMPARE(myPlayListProxyModelRestore.currentTrack(), QPersistentModelIndex(myPlayListProxyModelRestore.index(0, 0)));
}

void MediaPlayListProxyModelTest::testSavePersistentState()
{
    MediaPlayList myPlayListRead;
    QAbstractItemModelTester testModelRead(&myPlayListRead);
    MediaPlayListProxyModel myPlayListReadProxyModel;
    myPlayListReadProxyModel.setPlayListModel(&myPlayListRead);
    QAbstractItemModelTester testProxyModelRead(&myPlayListReadProxyModel);
    TracksListener myListenerRead(mDatabaseContent);

    QSignalSpy rowsAboutToBeMovedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::rowsAboutToBeMoved);
    QSignalSpy rowsAboutToBeRemovedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::rowsAboutToBeRemoved);
    QSignalSpy rowsAboutToBeInsertedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::rowsAboutToBeInserted);
    QSignalSpy rowsMovedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::rowsMoved);
    QSignalSpy rowsRemovedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::rowsRemoved);
    QSignalSpy rowsInsertedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::rowsInserted);
    QSignalSpy persistentStateChangedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::persistentStateChanged);
    QSignalSpy mDataChangedSpyRead(&myPlayListReadProxyModel, &MediaPlayListProxyModel::dataChanged);

    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(mDataChangedSpyRead.count(), 0);

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
    connect(mDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListenerRead, &TracksListener::tracksAdded);

    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(mDataChangedSpyRead.count(), 0);

    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                        {DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(mDataChangedSpyRead.count(), 0);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(mDataChangedSpyRead.count(), 0);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                        {DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(mDataChangedSpyRead.count(), 0);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(mDataChangedSpyRead.count(), 0);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                        {DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(mDataChangedSpyRead.count(), 0);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 0);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 0);
    QCOMPARE(persistentStateChangedSpyRead.count(), 0);
    QCOMPARE(mDataChangedSpyRead.count(), 0);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 11);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album3")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$11")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$1")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 12);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album3")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$12")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), true);

    myPlayListReadProxyModel.setPersistentState(mPlayListProxyModel->persistentState());

    QCOMPARE(mDataChangedSpyRead.wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(rowsAboutToBeRemovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeMovedSpyRead.count(), 0);
    QCOMPARE(rowsAboutToBeInsertedSpyRead.count(), 1);
    QCOMPARE(rowsRemovedSpyRead.count(), 0);
    QCOMPARE(rowsMovedSpyRead.count(), 0);
    QCOMPARE(rowsInsertedSpyRead.count(), 1);
    QCOMPARE(persistentStateChangedSpyRead.count(), 2);
    QCOMPARE(mDataChangedSpyRead.count(), 3);

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
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                          {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                         QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                          {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                         QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                          {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                         QStringLiteral("track5"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                          {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                         QStringLiteral("track1"), {}}}, {}, {});

    QVERIFY(mDataChangedSpy->wait());

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QVariantMap settings;
    settings[QStringLiteral("currentTrack")] = 2;
    settings[QStringLiteral("shuffleMode")] = MediaPlayListProxyModel::Shuffle::Track;
    settings[QStringLiteral("randomMapping")] = QVariantList({QVariant(2), QVariant(0), QVariant(3), QVariant(1)});
    settings[QStringLiteral("repeatMode")] = true;

    mPlayListProxyModel->setPersistentState(settings);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
}

void MediaPlayListProxyModelTest::testSaveAndRestoreSettings()
{
    MediaPlayList myPlayListRestore;
    QAbstractItemModelTester testModelRestore(&myPlayListRestore);
    MediaPlayListProxyModel myPlayListRestoreProxyModel;
    myPlayListRestoreProxyModel.setPlayListModel(&myPlayListRestore);
    QAbstractItemModelTester testProxyModelRestore(&myPlayListRestoreProxyModel);
    TracksListener myListenerRestore(mDatabaseContent);

    QSignalSpy currentTrackChangedRestoreSpy(&myPlayListRestoreProxyModel, &MediaPlayListProxyModel::currentTrackChanged);
    QSignalSpy shuffleModeChangedRestoreSpy(&myPlayListRestoreProxyModel, &MediaPlayListProxyModel::shuffleModeChanged);
    QSignalSpy repeatModeChangedRestoreSpy(&myPlayListRestoreProxyModel, &MediaPlayListProxyModel::repeatModeChanged);
    QSignalSpy playListFinishedRestoreSpy(&myPlayListRestoreProxyModel, &MediaPlayListProxyModel::playListFinished);

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
    connect(mDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListenerRestore, &TracksListener::tracksAdded);

    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(shuffleModeChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatModeChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::Playlist);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 0);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(shuffleModeChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatModeChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                        {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                       QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                        {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                                       QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                        {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                                       QStringLiteral("track5"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                        {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1)}},
                                       QStringLiteral("track1"), {}}}, {}, {});

    QVERIFY(mDataChangedSpy->wait());

    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Track);

    QVERIFY(mCurrentTrackChangedSpy->count() >= 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(shuffleModeChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatModeChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    const auto oldCurrentTrackChangedCount = mCurrentTrackChangedSpy->count();

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(shuffleModeChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatModeChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 0);
    QCOMPARE(shuffleModeChangedRestoreSpy.count(), 0);
    QCOMPARE(repeatModeChangedRestoreSpy.count(), 0);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    myPlayListRestoreProxyModel.setPersistentState(mPlayListProxyModel->persistentState());

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(currentTrackChangedRestoreSpy.count(), 1);
    QCOMPARE(shuffleModeChangedRestoreSpy.count(), 1);
    QCOMPARE(repeatModeChangedRestoreSpy.count(), 1);
    QCOMPARE(playListFinishedRestoreSpy.count(), 0);

    QCOMPARE(myPlayListRestoreProxyModel.repeatMode(), MediaPlayListProxyModel::Repeat::Playlist);
    QCOMPARE(myPlayListRestoreProxyModel.shuffleMode(), MediaPlayListProxyModel::Shuffle::Track);
}

void MediaPlayListProxyModelTest::shufflePlayList()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                                   QStringLiteral("track5"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::Playlist);
    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Track);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->shuffleMode(), MediaPlayListProxyModel::Shuffle::Track);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 4);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 5);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
}

void MediaPlayListProxyModelTest::testTrackShuffleMode()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                                   QStringLiteral("track5"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);

    mPlayListProxyModel->switchTo(3);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::Playlist);
    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Track);

    QVERIFY(mCurrentTrackChangedSpy->count() >= 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->shuffleMode(), MediaPlayListProxyModel::Shuffle::Track);
    QCOMPARE(mPlayListProxyModel->repeatMode(), MediaPlayListProxyModel::Repeat::Playlist);

    // The current track should now be the first in the randomized list
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    const auto oldCurrentTrackChangedCount = mCurrentTrackChangedSpy->count();

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 3);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->skipPreviousTrack(0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 4);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->removeRow(1);

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 4);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist7"), QStringLiteral("album3"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mRowsInsertedSpy->count(), 5);
    QVERIFY(mRowsInsertedSpy->constLast().at(1).toInt() > 1);
    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 4);
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"), QStringLiteral("album3"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mRowsInsertedSpy->count(), 6);
    QVERIFY(mRowsInsertedSpy->constLast().at(1).toInt() > 1);
    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 4);
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListProxyModel->currentTrackRow(), 1);
}

void MediaPlayListProxyModelTest::testAlbumShuffleMode()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                                   QStringLiteral("track5"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);

    mPlayListProxyModel->switchTo(2);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::Playlist);
    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Album);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->shuffleMode(), MediaPlayListProxyModel::Shuffle::Album);
    QCOMPARE(mPlayListProxyModel->repeatMode(), MediaPlayListProxyModel::Repeat::Playlist);

    // The current track should now be the second in the playlist (i.e. the
    // second track of the currently playing album, which should be the first
    // album in the randomized list)
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    const auto oldCurrentTrackChangedCount = mCurrentTrackChangedSpy->count();

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 3);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->skipPreviousTrack(0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 4);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->removeRow(1);

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 4);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(2, 0)));

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist7"), QStringLiteral("album3"), 1, 1)},
                                    {DataTypes::AlbumIdRole, 6}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsInsertedSpy->count(), 5);
    QVERIFY(mRowsInsertedSpy->constLast().at(1).toInt() > 2);
    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 4);
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(2, 0)));

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"), QStringLiteral("album3"), 1, 1)},
                                    {DataTypes::AlbumIdRole, 6}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsInsertedSpy->count(), 6);
    QVERIFY(mRowsInsertedSpy->constLast().at(1).toInt() > 2);
    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 4);
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(2, 0)));

    QCOMPARE(mCurrentTrackChangedSpy->count(), oldCurrentTrackChangedCount + 4);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListProxyModel->currentTrackRow(), 2);
}

void MediaPlayListProxyModelTest::testEnqueueShuffleMode()
{
    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::Playlist);
    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Track);

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"), QStringLiteral("album3"), 1, 1)},
                                    {DataTypes::AlbumIdRole, 4}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)},
                                    {DataTypes::AlbumIdRole, 3}},
                                   QStringLiteral("track5"), {}},
                                  {{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)},
                                    {DataTypes::AlbumIdRole, 3}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->removeRow(0);
    mPlayListProxyModel->removeRow(0);
    mPlayListProxyModel->removeRow(0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
    QCOMPARE(mPlayListProxyModel->tracksCount(), 0);
    QVERIFY(mCurrentTrackChangedSpy->count() >= 1);

    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Album);

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)},
                                    {DataTypes::AlbumIdRole, 3}},
                                   QStringLiteral("track5"), {}},
                                  {{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)},
                                    {DataTypes::AlbumIdRole, 3}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
    QVERIFY(mCurrentTrackChangedSpy->count() >= 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 2);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QVERIFY(mPlayListFinishedSpy->count() >= 1);

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)},
                                    {DataTypes::AlbumIdRole, 2}},
                                   QStringLiteral("track3"), {}}}, {}, {});

    QCOMPARE(mDataChangedSpy->wait(), true);

    QVERIFY(mPlayListProxyModel->currentTrackRow() <= 1);
    QVERIFY(mCurrentTrackChangedSpy->count() >= 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 2);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QVERIFY(mPlayListFinishedSpy->count() >= 1);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->currentTrack(), MediaPlayList::AlbumRole), QStringLiteral("album2"));
    QVERIFY(mPlayListProxyModel->currentTrackRow() >= 1);
    QVERIFY(mPlayListProxyModel->currentTrackRow() <= 2);
}

void MediaPlayListProxyModelTest::testSwitchShuffleMode()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                                   QStringLiteral("track5"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"), QStringLiteral("album3"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"), QStringLiteral("album3"), 1, 1)}},
                                   QStringLiteral("track2"), {}}}, {}, {});

    QCOMPARE(mDataChangedSpy->wait(), true);

    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::Playlist);
    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Track);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListProxyModel->shuffleMode(), MediaPlayListProxyModel::Shuffle::Track);
    QCOMPARE(mPlayListProxyModel->repeatMode(), MediaPlayListProxyModel::Repeat::Playlist);

    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::NoShuffle);
    mPlayListProxyModel->switchTo(5);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(5, 0)));
    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 2);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListProxyModel->shuffleMode(), MediaPlayListProxyModel::Shuffle::NoShuffle);

    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::Playlist);
    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Track);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 3);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListProxyModel->shuffleMode(), MediaPlayListProxyModel::Shuffle::Track);

    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Album);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));
    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 4);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListProxyModel->shuffleMode(), MediaPlayListProxyModel::Shuffle::Album);

    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Track);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 5);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListProxyModel->shuffleMode(), MediaPlayListProxyModel::Shuffle::Track);

    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::NoShuffle);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(5, 0)));
    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 6);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListProxyModel->shuffleMode(), MediaPlayListProxyModel::Shuffle::NoShuffle);

    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Album);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));
    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 7);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListProxyModel->shuffleMode(), MediaPlayListProxyModel::Shuffle::Album);

    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::NoShuffle);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(5, 0)));
    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 8);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mPlayListProxyModel->shuffleMode(), MediaPlayListProxyModel::Shuffle::NoShuffle);
}

void MediaPlayListProxyModelTest::randomAndContinuePlayList()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                                   QStringLiteral("track5"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Track);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->shuffleMode(), MediaPlayListProxyModel::Shuffle::Track);

    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::Playlist);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->repeatMode(), MediaPlayListProxyModel::Repeat::Playlist);

    mPlayListProxyModel->switchTo(3);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(3, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 4);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 5);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
}

void MediaPlayListProxyModelTest::continuePlayList()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                                   QStringLiteral("track5"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::Playlist);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->repeatMode(), MediaPlayListProxyModel::Repeat::Playlist);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(2, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 4);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(3, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 5);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
}

void MediaPlayListProxyModelTest::previousAndNextTracksTest()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 4)}},
                                   QStringLiteral("track4"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2)}},
                                   QStringLiteral("track2"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mPreviousTrackChangedSpy->count(), 0);
    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mNextTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->previousTrack(), QPersistentModelIndex());
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
    QCOMPARE(mPlayListProxyModel->nextTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mPreviousTrackChangedSpy->count(), 1);
    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mNextTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->previousTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));
    QCOMPARE(mPlayListProxyModel->nextTrack(), QPersistentModelIndex(mPlayListProxyModel->index(2, 0)));

    mPlayListProxyModel->switchTo(4);

    QCOMPARE(mPreviousTrackChangedSpy->count(), 2);
    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);
    QCOMPARE(mNextTrackChangedSpy->count(), 3);

    QCOMPARE(mPlayListProxyModel->previousTrack(), QPersistentModelIndex(mPlayListProxyModel->index(3, 0)));
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));
    QCOMPARE(mPlayListProxyModel->nextTrack(), QPersistentModelIndex());

    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::Playlist);

    QCOMPARE(mPreviousTrackChangedSpy->count(), 2);
    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);
    QCOMPARE(mNextTrackChangedSpy->count(), 4);

    QCOMPARE(mPlayListProxyModel->previousTrack(), QPersistentModelIndex(mPlayListProxyModel->index(3, 0)));
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));
    QCOMPARE(mPlayListProxyModel->nextTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mPreviousTrackChangedSpy->count(), 3);
    QCOMPARE(mCurrentTrackChangedSpy->count(), 4);
    QCOMPARE(mNextTrackChangedSpy->count(), 5);

    QCOMPARE(mPlayListProxyModel->previousTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
    QCOMPARE(mPlayListProxyModel->nextTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Track);

    QVERIFY(mPlayListProxyModel->previousTrack() != QPersistentModelIndex());
    QVERIFY(mPlayListProxyModel->currentTrack() != QPersistentModelIndex());
    QVERIFY(mPlayListProxyModel->nextTrack() != QPersistentModelIndex());
}

void MediaPlayListProxyModelTest::remainingTracksTest()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 4)}},
                                   QStringLiteral("track4"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2)}},
                                   QStringLiteral("track2"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mRemainingTracksChangedSpy->count(), 6);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
    QCOMPARE(mPlayListProxyModel->remainingTracks(), 4);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mRemainingTracksChangedSpy->count(), 7);
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));
    QCOMPARE(mPlayListProxyModel->remainingTracks(), 3);

    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Track);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 1);
    QCOMPARE(mRepeatModeChangedSpy->count(), 0);
    QCOMPARE(mRemainingTracksChangedSpy->count(), 8);

    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::NoShuffle);
    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::Playlist);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 2);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mRemainingTracksChangedSpy->count(), 10);

    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::One);
    QCOMPARE(mPlayListProxyModel->remainingTracks(), -1);
}

void MediaPlayListProxyModelTest::clearPlayListCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Album},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                                   QStringLiteral("album2"), {}}}, {}, {});

    QVERIFY(mRowsAboutToBeInsertedSpy->wait());

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mTracksCountChangedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);
    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->rowCount(), 6);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->clearPlayList();

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 2);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 2);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mTracksCountChangedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);
    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);

    QCOMPARE(mPlayListProxyModel->rowCount(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack().isValid(), false);
}

void MediaPlayListProxyModelTest::undoClearPlayListCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Album},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                                   QStringLiteral("album2"), {}}}, {}, {});

    QVERIFY(mRowsAboutToBeInsertedSpy->wait());

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mTracksCountChangedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);
    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->rowCount(), 6);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->clearPlayList();

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 2);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 2);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mTracksCountChangedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);
    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);

    QCOMPARE(mPlayListProxyModel->rowCount(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack().isValid(), false);

    mPlayListProxyModel->undoClearPlayList();

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 2);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 2);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mTracksCountChangedSpy->count(), 5);
    QCOMPARE(mPersistentStateChangedSpy->count(), 6);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 6);
    QCOMPARE(mNewEntryInListSpy->count(), 1);
    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);
    QCOMPARE(mDisplayUndoNotificationSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->rowCount(), 6);

    QVERIFY(mDataChangedSpy->wait());

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
}

void MediaPlayListProxyModelTest::undoReplacePlayListCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Album},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/"))}},
                                   QStringLiteral("album2"), {}}}, {}, {});

    QVERIFY(mRowsAboutToBeInsertedSpy->wait());

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mTracksCountChangedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);
    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->rowCount(), 6);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Album},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/"))}},
                                   QStringLiteral("album1"), {}}},
                                 ElisaUtils::ReplacePlayList,
                                 ElisaUtils::TriggerPlay);

    QVERIFY(mRowsAboutToBeInsertedSpy->wait());

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 3);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 3);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 7);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);
    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 4);

    mPlayListProxyModel->undoClearPlayList();

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 4);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 5);
    QCOMPARE(mRowsRemovedSpy->count(), 4);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 5);
    QCOMPARE(mTracksCountChangedSpy->count(), 9);
    QCOMPARE(mPersistentStateChangedSpy->count(), 10);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 6);
    QCOMPARE(mNewEntryInListSpy->count(), 2);
    QCOMPARE(mCurrentTrackChangedSpy->count(), 5);
    QCOMPARE(mDisplayUndoNotificationSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->rowCount(), 6);

    QVERIFY(mDataChangedSpy->wait());

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 7);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 8);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 9);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track6"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1 and artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::TrackNumberRole).toInt(), 6);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 10);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndSkipPreviousAndContinueCase()
{
    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::Playlist);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 0);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1)}},
                                   QStringLiteral("track2"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1)}},
                                   QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2)}},
                                   QStringLiteral("track2"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(2, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 4);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(3, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 5);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 6);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(5, 0)));

    mPlayListProxyModel->skipPreviousTrack(0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 7);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));

    mPlayListProxyModel->skipPreviousTrack(0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 8);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(3, 0)));

    mPlayListProxyModel->skipPreviousTrack(0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 9);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(2, 0)));

    mPlayListProxyModel->skipPreviousTrack(0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 10);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    mPlayListProxyModel->skipPreviousTrack(0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 11);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->skipPreviousTrack(0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 12);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(5, 0)));

    mPlayListProxyModel->setRepeatMode(MediaPlayListProxyModel::Repeat::One);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 12);
    QCOMPARE(mShuffleModeChangedSpy->count(), 0);
    QCOMPARE(mRepeatModeChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->nextTrack(), QPersistentModelIndex(mPlayListProxyModel->index(5, 0)));
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(5, 0)));
    QCOMPARE(mPlayListProxyModel->previousTrack(), QPersistentModelIndex(mPlayListProxyModel->index(5, 0)));

    mPlayListProxyModel->skipPreviousTrack(0);

    QCOMPARE(mPlayListProxyModel->nextTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));
    QCOMPARE(mPlayListProxyModel->previousTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));

    mPlayListProxyModel->skipPreviousTrack(2001);

    QCOMPARE(mPlayListProxyModel->nextTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));
    QCOMPARE(mPlayListProxyModel->previousTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mPlayListProxyModel->nextTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));
    QCOMPARE(mPlayListProxyModel->previousTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));

    mPlayListProxyModel->skipNextTrack(ElisaUtils::SkipReason::Manual);

    QCOMPARE(mPlayListProxyModel->nextTrack(), QPersistentModelIndex(mPlayListProxyModel->index(5, 0)));
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(5, 0)));
    QCOMPARE(mPlayListProxyModel->previousTrack(), QPersistentModelIndex(mPlayListProxyModel->index(5, 0)));

    mPlayListProxyModel->skipNextTrack(ElisaUtils::SkipReason::Manual);

    QCOMPARE(mPlayListProxyModel->nextTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
    QCOMPARE(mPlayListProxyModel->previousTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndRemoveMultipleNotBeginCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2)}},
                                   QStringLiteral("track2"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 1)}},
                                   QStringLiteral("track4"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    qDebug() << mPlayListProxyModel->currentTrack() << QPersistentModelIndex(mPlayListProxyModel->index(1, 0));
    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    mPlayListProxyModel->removeRow(1);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndPlayCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndSkipNextCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndSkipPreviousCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    mPlayListProxyModel->skipPreviousTrack(0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndSkipPreviousWithSeekCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mTrackSeekedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mTrackSeekedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    mPlayListProxyModel->skipPreviousTrack(10000);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mTrackSeekedSpy->count(), 1);

    mPlayListProxyModel->skipPreviousTrack(0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
    QCOMPARE(mTrackSeekedSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
}

void MediaPlayListProxyModelTest::testRemoveSelection()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Artist}}, QStringLiteral("artist1"), {}}},
                                 ElisaUtils::PlayListEnqueueMode::ReplacePlayList, ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mRowsAboutToBeInsertedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(),1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->tracksCount(), 6);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);

    mPlayListProxyModel->removeSelection({2, 4, 5});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 4);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 4);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 6);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->tracksCount(), 3);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
}

void MediaPlayListProxyModelTest::testReplaceAndPlayArtist()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Artist}}, QStringLiteral("artist3"), {}}},
                                 ElisaUtils::PlayListEnqueueMode::ReplacePlayList, ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->tracksCount(), 1);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$3")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Artist}}, QStringLiteral("artist4"), {}}},
                                 ElisaUtils::PlayListEnqueueMode::ReplacePlayList, ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 2);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 2);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 5);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mRowsAboutToBeInsertedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 3);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 3);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 7);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mPlayListProxyModel->tracksCount(), 1);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$4")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);
}

void MediaPlayListProxyModelTest::testReplaceAndPlayTrackId()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"),
                                                                               QStringLiteral("album1"), 3, 3);

    QCOMPARE(firstTrackId != 0, true);

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track}, {DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}},
                                 ElisaUtils::PlayListEnqueueMode::ReplacePlayList, ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->tracksCount(), 1);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$3")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist4"),
                                                                                QStringLiteral("album1"), 4, 4);

    QCOMPARE(secondTrackId != 0, true);

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track}, {DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}},
                                 ElisaUtils::PlayListEnqueueMode::ReplacePlayList, ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mDataChangedSpy->wait(), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mPlayListProxyModel->tracksCount(), 1);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DurationRole).toTime().msecsSinceStartOfDay(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ImageUrlRole).toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ResourceRole).toUrl(), QUrl::fromUserInput(QStringLiteral("/$4")));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsSingleDiscAlbumRole).toBool(), false);
}

void MediaPlayListProxyModelTest::testTrackBeenRemoved()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Artist}}, QStringLiteral("artist1"), {}}},
                                 ElisaUtils::PlayListEnqueueMode::ReplacePlayList, ElisaUtils::PlayListEnqueueTriggerPlay::TriggerPlay);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->tracksCount(), 6);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);

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
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->tracksCount(), 6);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("Various Artists"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), -1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 0);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(3, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(4, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::AlbumArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 5);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(5, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
}

void MediaPlayListProxyModelTest::finishPlayList()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);
    QCOMPARE(mPlayListFinishedSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
}

void MediaPlayListProxyModelTest::removeBeforeCurrentTrack()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"), QStringLiteral("album2"), 5, 1)}},
                                   QStringLiteral("track5"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(2, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 4);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(3, 0)));

    mPlayListProxyModel->removeRow(1);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 4);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);
}

void MediaPlayListProxyModelTest::switchToTrackTest()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"), QStringLiteral("album1"), 3, 3)}},
                                   QStringLiteral("track3"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album1"), 4, 4)}},
                                   QStringLiteral("track4"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2)}},
                                   QStringLiteral("track2"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->switchTo(4);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(4, 0)));
}

void MediaPlayListProxyModelTest::singleTrack()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndRemoveCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->removeRow(0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex());
}

void MediaPlayListProxyModelTest::testBringUpAndRemoveLastCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1)}},
                                   QStringLiteral("track2"), {}}}, {}, {});
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1)}},
                                   QStringLiteral("track3"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(1, 0)));

    mPlayListProxyModel->skipNextTrack();

    QCOMPARE(mCurrentTrackChangedSpy->count(), 3);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(2, 0)));

    mPlayListProxyModel->removeRow(2);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 4);
    QCOMPARE(mPlayListFinishedSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndRemoveMultipleCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2)}},
                                   QStringLiteral("track2"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    mPlayListProxyModel->removeRow(0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
}

void MediaPlayListProxyModelTest::testBringUpAndDownCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));

    mPlayListProxyModel->removeRow(0);

    QCOMPARE(mCurrentTrackChangedSpy->count(), 2);
    QCOMPARE(mPlayListFinishedSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex());
}

void MediaPlayListProxyModelTest::testBringUpCase()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1)}},
                                   QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
}

void MediaPlayListProxyModelTest::testBringUpCaseFromNewAlbum()
{
    auto newTrackID = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                             QStringLiteral("album2"), 1, 1);

    QVERIFY(newTrackID != 0);

    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, newTrackID}}, QStringLiteral("track1"), {}}}, {}, {});

    QCOMPARE(mCurrentTrackChangedSpy->count(), 1);
    QCOMPARE(mPlayListFinishedSpy->count(), 0);

    QCOMPARE(mPlayListProxyModel->currentTrack(), QPersistentModelIndex(mPlayListProxyModel->index(0, 0)));
}

void MediaPlayListProxyModelTest::testSetData()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);

    QCOMPARE(mPlayListProxyModel->setData(mPlayListProxyModel->index(2, 0), true, MediaPlayList::ColumnsRoles::IsPlayingRole), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), false);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(2, 0), MediaPlayList::ColumnsRoles::IsPlayingRole).toBool(), true);

    QCOMPARE(mPlayListProxyModel->setData(mPlayListProxyModel->index(2, 0), true, MediaPlayList::ColumnsRoles::SecondaryTextRole), false);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mPlayListProxyModel->setData(mPlayListProxyModel->index(4, 0), true, MediaPlayList::ColumnsRoles::TitleRole), false);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    QCOMPARE(mPlayListProxyModel->setData({}, true, MediaPlayList::ColumnsRoles::TitleRole), false);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);
}

void MediaPlayListProxyModelTest::testHasHeader()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayListProxyModel->removeRow(2);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 5);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
}

void MediaPlayListProxyModelTest::testHasHeaderWithRemove()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayListProxyModel->removeRow(2);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 5);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayListProxyModel->removeRow(0);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 2);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 2);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 6);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
}

void MediaPlayListProxyModelTest::testHasHeaderMoveFirst()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayListProxyModel->moveRow(0, 3);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 1);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 5);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
}

void MediaPlayListProxyModelTest::testHasHeaderMoveAnother()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayListProxyModel->moveRow(3, 0);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 1);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 5);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
}

void MediaPlayListProxyModelTest::testHasHeaderAlbumWithSameTitle()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                                QStringLiteral("album1"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                               QStringLiteral("album3"), 2, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"),
                                                                                QStringLiteral("album3"), 3, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    auto fithTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track9"), QStringLiteral("artist2"),
                                                                              QStringLiteral("album3"), 9, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, fithTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 5);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 5);
    QCOMPARE(mPersistentStateChangedSpy->count(), 5);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 5);
    QCOMPARE(mDataChangedSpy->count(), 5);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 5);

    auto sixthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist7"),
                                                                               QStringLiteral("album3"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, sixthTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 6);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 6);
    QCOMPARE(mPersistentStateChangedSpy->count(), 6);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 6);
    QCOMPARE(mDataChangedSpy->count(), 6);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 6);
}

void MediaPlayListProxyModelTest::testHasHeaderMoveFirstLikeQml()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayListProxyModel->moveRow(0, 3);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 1);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 5);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
}

void MediaPlayListProxyModelTest::testHasHeaderMoveAnotherLikeQml()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayListProxyModel->moveRow(3, 0);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 1);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 5);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
}

void MediaPlayListProxyModelTest::testHasHeaderYetAnotherMoveLikeQml()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 4, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayListProxyModel->moveRow(0, 2);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 1);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 5);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);

    mPlayListProxyModel->moveRow(2, 0);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 2);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 4);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 2);
    QCOMPARE(mRowsInsertedSpy->count(), 4);
    QCOMPARE(mPersistentStateChangedSpy->count(), 6);
    QCOMPARE(mDataChangedSpy->count(), 4);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 4);
}

void MediaPlayListProxyModelTest::enqueueFiles()
{
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::ResourceRole, QUrl::fromLocalFile(QStringLiteral("/$1"))}}, {}, {}},
                                  {{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::ResourceRole, QUrl::fromLocalFile(QStringLiteral("/$2"))}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 2);

    QCOMPARE(mPlayListProxyModel->rowCount(), 2);

    QCOMPARE(mDataChangedSpy->wait(300), true);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 2);

    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("track2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("album1"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 2);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 2);
}

void MediaPlayListProxyModelTest::enqueueSampleFiles()
{
    mPlayListProxyModel->enqueue({
                                     {{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                       {DataTypes::ResourceRole, QUrl::fromLocalFile(QStringLiteral(MEDIAPLAYLIST_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/test.ogg"))}}, {}, {}},
                                     {{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                       {DataTypes::ResourceRole, QUrl::fromLocalFile(QStringLiteral(MEDIAPLAYLIST_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/test2.ogg"))}}, {}, {}}},
                                 {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 2);

    QCOMPARE(mPlayListProxyModel->rowCount(), 2);

    while (mDataChangedSpy->count() < 2) {
        QCOMPARE(mDataChangedSpy->wait(), true);
    }

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
    QCOMPARE(mNewUrlInListSpy->count(), 2);

#if KFFileMetaData_FOUND
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("Title"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("Test"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("Artist"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1000);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("Title2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral("Test2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral("Artist2"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), 1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 1000);
#else
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("test.ogg"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral(""));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral(""));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::TrackNumberRole).toInt(), -1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DiscNumberRole).toInt(), -1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(0, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 0);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TitleRole).toString(), QStringLiteral("test2.ogg"));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::AlbumRole).toString(), QStringLiteral(""));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::ArtistRole).toString(), QStringLiteral(""));
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::TrackNumberRole).toInt(), -1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DiscNumberRole).toInt(), -1);
    QCOMPARE(mPlayListProxyModel->data(mPlayListProxyModel->index(1, 0), MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay(), 0);
#endif
}

void MediaPlayListProxyModelTest::enqueueEmpty()
{
    mPlayListProxyModel->enqueue(DataTypes::EntryDataList{}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mPersistentStateChangedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    mPlayListProxyModel->enqueue(DataTypes::EntryDataList{}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mPersistentStateChangedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    mPlayListProxyModel->enqueue(DataTypes::EntryDataList{}, ElisaUtils::AppendPlayList, ElisaUtils::DoNotTriggerPlay);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mPersistentStateChangedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    mPlayListProxyModel->enqueue(DataTypes::EntryDataList{}, ElisaUtils::AppendPlayList, ElisaUtils::DoNotTriggerPlay);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mPersistentStateChangedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);

    mPlayListProxyModel->enqueue(DataTypes::EntryDataList{},
                                 ElisaUtils::AppendPlayList,
                                 ElisaUtils::DoNotTriggerPlay);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 0);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 0);
    QCOMPARE(mPersistentStateChangedSpy->count(), 0);
    QCOMPARE(mDataChangedSpy->count(), 0);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 0);
}

void MediaPlayListProxyModelTest::testMoveAndShuffle()
{
    auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 1);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 1);
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 1);
    QCOMPARE(mDataChangedSpy->count(), 1);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 1);

    auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 2);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 2);
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 2);
    QCOMPARE(mDataChangedSpy->count(), 2);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 2);

    auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
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
    QCOMPARE(mPersistentStateChangedSpy->count(), 3);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    mPlayListProxyModel->setShuffleMode(MediaPlayListProxyModel::Shuffle::Track);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);

    mPlayListProxyModel->moveRow(2, 0);

    QCOMPARE(mRowsAboutToBeRemovedSpy->count(), 0);
    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 1);
    QCOMPARE(mRowsAboutToBeInsertedSpy->count(), 3);
    QCOMPARE(mRowsRemovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 1);
    QCOMPARE(mRowsInsertedSpy->count(), 3);
    QCOMPARE(mPersistentStateChangedSpy->count(), 4);
    QCOMPARE(mDataChangedSpy->count(), 3);
    QCOMPARE(mNewTrackByNameInListSpy->count(), 0);
    QCOMPARE(mNewEntryInListSpy->count(), 3);
}

void MediaPlayListProxyModelTest::testMoveCurrentTrack()
{
    const auto firstTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, firstTrackId}}, {}, {}}}, {}, {});

    const auto secondTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album2"), 2, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, secondTrackId}}, {}, {}}}, {}, {});

    const auto thirdTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, thirdTrackId}}, {}, {}}}, {}, {});

    const auto fourthTrackId = mDatabaseContent->trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"), QStringLiteral("album2"), 3, 1);
    mPlayListProxyModel->enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                    {DataTypes::DatabaseIdRole, fourthTrackId}}, {}, {}}}, {}, {});

    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);

    // Set current track index
    mPlayListProxyModel->switchTo(1);

    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 0);
    QCOMPARE(mRowsMovedSpy->count(), 0);
    QCOMPARE(mPlayListProxyModel->previousTrack().row(), 0);
    QCOMPARE(mPlayListProxyModel->currentTrack().row(), 1);
    QCOMPARE(mPlayListProxyModel->nextTrack().row(), 2);

    // Move current track
    mPlayListProxyModel->moveRow(1, 2);

    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 1);
    QCOMPARE(mRowsMovedSpy->count(), 1);
    QCOMPARE(mPlayListProxyModel->previousTrack().row(), 1);
    QCOMPARE(mPlayListProxyModel->currentTrack().row(), 2);
    QCOMPARE(mPlayListProxyModel->nextTrack().row(), 3);

    // Move previous track to after current track
    mPlayListProxyModel->moveRow(1, 3);

    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 2);
    QCOMPARE(mRowsMovedSpy->count(), 2);
    QCOMPARE(mPlayListProxyModel->previousTrack().row(), 0);
    QCOMPARE(mPlayListProxyModel->currentTrack().row(), 1);
    QCOMPARE(mPlayListProxyModel->nextTrack().row(), 2);

    // Move next track to before current track
    mPlayListProxyModel->moveRow(2, 0);

    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 3);
    QCOMPARE(mRowsMovedSpy->count(), 3);
    QCOMPARE(mPlayListProxyModel->previousTrack().row(), 1);
    QCOMPARE(mPlayListProxyModel->currentTrack().row(), 2);
    QCOMPARE(mPlayListProxyModel->nextTrack().row(), 3);

    // Move into position of current track (from below)
    mPlayListProxyModel->moveRow(0, 2);

    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 4);
    QCOMPARE(mRowsMovedSpy->count(), 4);
    QCOMPARE(mPlayListProxyModel->previousTrack().row(), 0);
    QCOMPARE(mPlayListProxyModel->currentTrack().row(), 1);
    QCOMPARE(mPlayListProxyModel->nextTrack().row(), 2);

    // Move into position of current track (from above)
    mPlayListProxyModel->moveRow(3, 1);

    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 5);
    QCOMPARE(mRowsMovedSpy->count(), 5);
    QCOMPARE(mPlayListProxyModel->previousTrack().row(), 1);
    QCOMPARE(mPlayListProxyModel->currentTrack().row(), 2);
    QCOMPARE(mPlayListProxyModel->nextTrack().row(), 3);

    // Move current track to start
    mPlayListProxyModel->moveRow(2, 0);

    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 6);
    QCOMPARE(mRowsMovedSpy->count(), 6);
    QCOMPARE(mPlayListProxyModel->previousTrack().isValid(), false);
    QCOMPARE(mPlayListProxyModel->currentTrack().row(), 0);
    QCOMPARE(mPlayListProxyModel->nextTrack().row(), 1);

    // Move current track to end
    mPlayListProxyModel->moveRow(0, 3);

    QCOMPARE(mRowsAboutToBeMovedSpy->count(), 7);
    QCOMPARE(mRowsMovedSpy->count(), 7);
    QCOMPARE(mPlayListProxyModel->previousTrack().row(), 2);
    QCOMPARE(mPlayListProxyModel->currentTrack().row(), 3);
    QCOMPARE(mPlayListProxyModel->nextTrack().isValid(), false);
}

QTEST_GUILESS_MAIN(MediaPlayListProxyModelTest)


#include "moc_mediaplaylistproxymodeltest.cpp"
