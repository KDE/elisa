/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "manageheaderbartest.h"

#include "manageheaderbar.h"
#include "mediaplaylist.h"
#include "databaseinterface.h"
#include "trackslistener.h"

#include <QHash>
#include <QString>

#include <QStringListModel>

#include <QtTest>

ManageHeaderBarTest::ManageHeaderBarTest(QObject *parent) : QObject(parent)
{

}

void ManageHeaderBarTest::initTestCase()
{

}

void ManageHeaderBarTest::simpleInitialCase()
{
    ManageHeaderBar myControl;

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.artistRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.titleRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.albumRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.imageRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.albumIdRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.isValidRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.artist().toString().isEmpty(), true);
    QCOMPARE(myControl.title().toString().isEmpty(), true);
    QCOMPARE(myControl.album().toString().isEmpty(), true);
    QCOMPARE(myControl.image().toString().isEmpty(), true);
    QCOMPARE(myControl.isValid(), false);
}

void ManageHeaderBarTest::setCurrentTrackWithInvalidData()
{
    ManageHeaderBar myControl;

    QSignalSpy currentTrackChangedSpy(&myControl, &ManageHeaderBar::currentTrackChanged);
    QSignalSpy artistRoleChangedSpy(&myControl, &ManageHeaderBar::artistRoleChanged);
    QSignalSpy titleRoleChangedSpy(&myControl, &ManageHeaderBar::titleRoleChanged);
    QSignalSpy albumRoleChangedSpy(&myControl, &ManageHeaderBar::albumRoleChanged);
    QSignalSpy imageRoleChangedSpy(&myControl, &ManageHeaderBar::imageRoleChanged);
    QSignalSpy albumIdRoleChangedSpy(&myControl, &ManageHeaderBar::albumIdRoleChanged);
    QSignalSpy isValidRoleChangedSpy(&myControl, &ManageHeaderBar::isValidRoleChanged);
    QSignalSpy artistChangedSpy(&myControl, &ManageHeaderBar::artistChanged);
    QSignalSpy titleChangedSpy(&myControl, &ManageHeaderBar::titleChanged);
    QSignalSpy albumChangedSpy(&myControl, &ManageHeaderBar::albumChanged);
    QSignalSpy imageChangedSpy(&myControl, &ManageHeaderBar::imageChanged);
    QSignalSpy albumIdChangedSpy(&myControl, &ManageHeaderBar::albumIdChanged);
    QSignalSpy isValidChangedSpy(&myControl, &ManageHeaderBar::isValidChanged);

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.artistRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.titleRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.albumRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.imageRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.albumIdRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.isValidRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.artist().toString().isEmpty(), true);
    QCOMPARE(myControl.title().toString().isEmpty(), true);
    QCOMPARE(myControl.album().toString().isEmpty(), true);
    QCOMPARE(myControl.image().toString().isEmpty(), true);
    QCOMPARE(myControl.isValid(), false);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(albumRoleChangedSpy.count(), 0);
    QCOMPARE(imageRoleChangedSpy.count(), 0);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);

    myControl.setCurrentTrack(QPersistentModelIndex());

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(albumRoleChangedSpy.count(), 0);
    QCOMPARE(imageRoleChangedSpy.count(), 0);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
}

void ManageHeaderBarTest::setCurrentTrackWithData()
{
    QStringListModel myPlayList;

    ManageHeaderBar myControl;

    QSignalSpy currentTrackChangedSpy(&myControl, &ManageHeaderBar::currentTrackChanged);
    QSignalSpy artistRoleChangedSpy(&myControl, &ManageHeaderBar::artistRoleChanged);
    QSignalSpy titleRoleChangedSpy(&myControl, &ManageHeaderBar::titleRoleChanged);
    QSignalSpy albumRoleChangedSpy(&myControl, &ManageHeaderBar::albumRoleChanged);
    QSignalSpy imageRoleChangedSpy(&myControl, &ManageHeaderBar::imageRoleChanged);
    QSignalSpy albumIdRoleChangedSpy(&myControl, &ManageHeaderBar::albumIdRoleChanged);
    QSignalSpy isValidRoleChangedSpy(&myControl, &ManageHeaderBar::isValidRoleChanged);
    QSignalSpy artistChangedSpy(&myControl, &ManageHeaderBar::artistChanged);
    QSignalSpy titleChangedSpy(&myControl, &ManageHeaderBar::titleChanged);
    QSignalSpy albumChangedSpy(&myControl, &ManageHeaderBar::albumChanged);
    QSignalSpy imageChangedSpy(&myControl, &ManageHeaderBar::imageChanged);
    QSignalSpy albumIdChangedSpy(&myControl, &ManageHeaderBar::albumIdChanged);
    QSignalSpy isValidChangedSpy(&myControl, &ManageHeaderBar::isValidChanged);

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.artistRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.titleRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.albumRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.imageRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.albumIdRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.isValidRole(), static_cast<int>(Qt::DisplayRole));
    QCOMPARE(myControl.artist().toString().isEmpty(), true);
    QCOMPARE(myControl.title().toString().isEmpty(), true);
    QCOMPARE(myControl.album().toString().isEmpty(), true);
    QCOMPARE(myControl.image().toString().isEmpty(), true);
    QCOMPARE(myControl.isValid(), false);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(albumRoleChangedSpy.count(), 0);
    QCOMPARE(imageRoleChangedSpy.count(), 0);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);

    QCOMPARE(myPlayList.insertRows(0, 1), true);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(albumRoleChangedSpy.count(), 0);
    QCOMPARE(imageRoleChangedSpy.count(), 0);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(albumRoleChangedSpy.count(), 0);
    QCOMPARE(imageRoleChangedSpy.count(), 0);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);;

    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

QTEST_GUILESS_MAIN(ManageHeaderBarTest)


#include "moc_manageheaderbartest.cpp"
