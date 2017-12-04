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

#ifndef MEDIAPLAYLISTTEST_H
#define MEDIAPLAYLISTTEST_H

#include "musicaudiotrack.h"

#include <QObject>
#include <QHash>
#include <QVector>
#include <QUrl>
#include <QString>

class MediaPlayListTest : public QObject
{

    Q_OBJECT

public:

    explicit MediaPlayListTest(QObject *parent = nullptr);

Q_SIGNALS:

private Q_SLOTS:

    void initTestCase();

    void simpleInitialCase();

    void enqueueAlbumCase();

    void enqueueArtistCase();

    void removeFirstTrackOfAlbum();

    void testHasHeader();

    void testHasHeaderWithRestore();

    void testHasHeaderWithRemove();

    void testHasHeaderMoveFirst();

    void testHasHeaderMoveAnother();

    void testHasHeaderMoveFirstLikeQml();

    void testHasHeaderMoveAnotherLikeQml();

    void testHasHeaderYetAnotherMoveLikeQml();

    void enqueueClearAndEnqueue();

    void crashOnEnqueue();

    void restoreMultipleIdenticalTracks();

    void testHasHeaderAlbumWithSameTitle();

    void testSavePersistentState();

    void testClearAndEnqueueArtist();

    void testClearAndEnqueueTrackId();

    void testClearAndEnqueueTrackData();

    void testSetData();

    void testRemoveSelection();

    void testTrackBeenRemoved();

    void testBringUpCase();

    void testBringUpCaseFromNewAlbum();

    void testBringUpAndDownCase();

    void testBringUpAndRemoveCase();

    void testBringUpAndRemoveMultipleCase();

    void testBringUpAndRemoveMultipleNotBeginCase();

    void testBringUpAndPlayCase();

    void testBringUpAndSkipNextCase();

    void testBringUpAndSkipPreviousCase();

    void testBringUpAndSkipPreviousAndContinueCase();

    void finishPlayList();

    void randomPlayList();

    void randomAndContinuePlayList();

    void continuePlayList();

    void testRestoreSettings();

    void removeBeforeCurrentTrack();

    void switchToTrackTest();

    void singleTrack();

    void testBringUpAndRemoveLastCase();

    void testSaveAndRestoreSettings();

private:

    QList<MusicAudioTrack> mNewTracks;
    QHash<QString, QUrl> mNewCovers;

};

class MediaPlayList;

class CrashEnqueuePlayList : public QObject
{
    Q_OBJECT

public:

    explicit CrashEnqueuePlayList(MediaPlayList *list, QObject *parent = nullptr);

public Q_SLOTS:

    void crashMediaPlayList();

private:

    MediaPlayList *mList = nullptr;
};

#endif // MEDIAPLAYLISTTEST_H
