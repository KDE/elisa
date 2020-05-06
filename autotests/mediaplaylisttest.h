/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MEDIAPLAYLISTTEST_H
#define MEDIAPLAYLISTTEST_H

#include "databasetestdata.h"

#include <QObject>
#include <QHash>

class MediaPlayListTest : public QObject, public DatabaseTestData
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

    void enqueueTrackByUrl();

    void enqueueTracksByUrl();

    void enqueueFiles();

    void enqueueSampleFiles();

    void enqueueEmpty();

    void removeFirstTrackOfAlbum();

    void testTrackBeenRemoved();

    void testSetData();

    void testHasHeader();

    void testHasHeaderWithRemove();

    void testHasHeaderWithRestore();

    void testHasHeaderMoveFirst();

    void testHasHeaderMoveAnother();

    void restoreMultipleIdenticalTracks();

    void restoreTrackWithoutAlbum();

    void testHasHeaderAlbumWithSameTitle();

    void testHasHeaderMoveFirstLikeQml();

    void testHasHeaderMoveAnotherLikeQml();

    void testHasHeaderYetAnotherMoveLikeQml();

    void crashOnEnqueue();

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
