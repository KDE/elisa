/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MEDIAPLAYLISTPROXYMODELTEST_H
#define MEDIAPLAYLISTPROXYMODELTEST_H

#include "databasetestdata.h"

#include <QObject>
#include <QHash>

class MediaPlayListProxyModelTest : public QObject, public DatabaseTestData
{

    Q_OBJECT

public:

    explicit MediaPlayListProxyModelTest(QObject *parent = nullptr);

Q_SIGNALS:

private Q_SLOTS:

    void initTestCase();

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

    void testShuffleMode();

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

};

#endif // MEDIAPLAYLISTPROXYMODELTEST_H
