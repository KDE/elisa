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

    void randomPlayList();

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

};

#endif // MEDIAPLAYLISTPROXYMODELTEST_H
