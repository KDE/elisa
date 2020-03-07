/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
