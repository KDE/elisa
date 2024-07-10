/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MEDIAPLAYLISTPROXYMODELTEST_H
#define MEDIAPLAYLISTPROXYMODELTEST_H

#include "databasetestdata.h"

#include <QObject>

class DatabaseInterface;
class MediaPlayList;
class MediaPlayListProxyModel;
class QAbstractItemModelTester;
class QSignalSpy;
class TracksListener;

class MediaPlayListProxyModelTest : public QObject, public DatabaseTestData
{

    Q_OBJECT

public:

    explicit MediaPlayListProxyModelTest(QObject *parent = nullptr);

Q_SIGNALS:

private Q_SLOTS:

    void initTestCase();

    void init();

    void cleanup();

    void m3uPlaylistParser_SimpleCase();

    void m3uPlaylistParser_CommentCase();

    void m3uPlaylistParser_WindowsLineTerminator();

    void plsPlaylistParserCase();

    void plsPlaylistParser_WindowsLineTerminator();

    void m3uPlaylistParser_ToPlaylist();

    void plsPlaylistParser_ToPlaylist();

    void simpleInitialCase();

    void enqueueAlbumCase();

    void enqueueArtistCase();

    void enqueueMultipleAlbumsCase();

    void enqueueTrackByUrl();

    void enqueueTracksByUrl();

    void enqueueReplaceAndPlay();

    void enqueueFiles();

    void enqueueSampleFiles();

    void enqueueEmpty();

    void enqueueNext_data();

    void enqueueNext();

    void removeFirstTrackOfAlbum();

    void testSaveLoadPlayList();

    void testSavePersistentState();

    void testRestoreSettings();

    void testSaveAndRestoreSettings();

    void testBringUpAndSkipPreviousAndContinueCase();

    void testBringUpAndRemoveMultipleNotBeginCase();

    void testBringUpAndPlayCase();

    void testBringUpAndSkipNextCase();

    void testBringUpAndSkipPreviousCase();

    void testBringUpAndSkipPreviousWithSeekCase();

    void testBringUpAndRemoveCase();

    void testBringUpAndRemoveLastCase();

    void testBringUpAndRemoveMultipleCase();

    void testBringUpAndDownCase();

    void testBringUpCase();

    void testBringUpCaseFromNewAlbum();

    void testSetData();

    void testRemoveSelection();

    void testReplaceAndPlayArtist();

    void testReplaceAndPlayTrackId();

    void testTrackBeenRemoved();

    void shufflePlayList();

    void testTrackShuffleMode();

    void testAlbumShuffleMode();

    void testEnqueueShuffleMode();

    void testSwitchShuffleMode();

    void randomAndContinuePlayList();

    void continuePlayList();

    void previousAndNextTracksTest();

    void remainingTracksTest();

    void clearPlayListCase();

    void undoClearPlayListCase();

    void undoReplacePlayListCase();

    void finishPlayList();

    void removeBeforeCurrentTrack();

    void switchToTrackTest();

    void singleTrack();

    void testHasHeader();

    void testHasHeaderWithRemove();

    void testHasHeaderMoveFirst();

    void testHasHeaderMoveAnother();

    void testHasHeaderAlbumWithSameTitle();

    void testHasHeaderMoveFirstLikeQml();

    void testHasHeaderMoveAnotherLikeQml();

    void testHasHeaderYetAnotherMoveLikeQml();

    void testMoveAndShuffle();

    void testMoveCurrentTrack();

private:

    MediaPlayList *mPlayList = nullptr;
    MediaPlayListProxyModel *mPlayListProxyModel = nullptr;
    QAbstractItemModelTester *mModelTester = nullptr;
    QAbstractItemModelTester *mProxyModelTester = nullptr;
    DatabaseInterface *mDatabaseContent = nullptr;
    TracksListener *mListener = nullptr;

    QSignalSpy *mRowsAboutToBeMovedSpy = nullptr;
    QSignalSpy *mRowsAboutToBeRemovedSpy = nullptr;
    QSignalSpy *mRowsAboutToBeInsertedSpy = nullptr;
    QSignalSpy *mRowsMovedSpy = nullptr;
    QSignalSpy *mRowsRemovedSpy = nullptr;
    QSignalSpy *mRowsInsertedSpy = nullptr;
    QSignalSpy *mDataChangedSpy = nullptr;

    QSignalSpy *mCurrentTrackChangedSpy = nullptr;
    QSignalSpy *mDisplayUndoNotificationSpy = nullptr;
    QSignalSpy *mNextTrackChangedSpy = nullptr;
    QSignalSpy *mPersistentStateChangedSpy = nullptr;
    QSignalSpy *mPlayListFinishedSpy = nullptr;
    QSignalSpy *mPlayListLoadedSpy = nullptr;
    QSignalSpy *mPlayListLoadFailedSpy = nullptr;
    QSignalSpy *mPreviousTrackChangedSpy = nullptr;
    QSignalSpy *mRemainingTracksChangedSpy = nullptr;
    QSignalSpy *mRepeatModeChangedSpy = nullptr;
    QSignalSpy *mShuffleModeChangedSpy = nullptr;
    QSignalSpy *mTracksCountChangedSpy = nullptr;
    QSignalSpy *mTrackSeekedSpy = nullptr;

    QSignalSpy *mNewTrackByNameInListSpy = nullptr;
    QSignalSpy *mNewEntryInListSpy = nullptr;
    QSignalSpy *mNewUrlInListSpy = nullptr;
};

#endif // MEDIAPLAYLISTPROXYMODELTEST_H
