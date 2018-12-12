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

#include "manageheaderbartest.h"

#include "manageheaderbar.h"
#include "mediaplaylist.h"
#include "databaseinterface.h"
#include "trackslistener.h"

#include <QHash>
#include <QString>
#include <QUrl>
#include <QVector>
#include <QVariant>
#include <QList>

#include <QStringListModel>
#include <QStandardItemModel>
#include <QStandardItem>

#include <QtTest>

ManageHeaderBarTest::ManageHeaderBarTest(QObject *parent) : QObject(parent)
{

}

void ManageHeaderBarTest::initTestCase()
{
    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
    qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
}

void ManageHeaderBarTest::simpleInitialCase()
{
    ManageHeaderBar myControl;

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
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
    QCOMPARE(myControl.remainingTracks(), 0);
}

void ManageHeaderBarTest::setCurrentTrackCase()
{
    QStringListModel myPlayList;

    ManageHeaderBar myControl;

    QSignalSpy currentTrackChangedSpy(&myControl, &ManageHeaderBar::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageHeaderBar::playListModelChanged);
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
    QSignalSpy remainingTracksChangedSpy(&myControl, &ManageHeaderBar::remainingTracksChanged);

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
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
    QCOMPARE(myControl.remainingTracks(), 0);

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    QCOMPARE(myPlayList.insertRows(0, 3), true);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
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
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
}

void ManageHeaderBarTest::setCurrentTrackCaseFromNewAlbum()
{
    QStringListModel myPlayList;

    ManageHeaderBar myControl;

    QSignalSpy currentTrackChangedSpy(&myControl, &ManageHeaderBar::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageHeaderBar::playListModelChanged);
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
    QSignalSpy remainingTracksChangedSpy(&myControl, &ManageHeaderBar::remainingTracksChanged);

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
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
    QCOMPARE(myControl.remainingTracks(), 0);

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    QCOMPARE(myPlayList.insertRows(0, 1), true);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
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
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
}

void ManageHeaderBarTest::completeCase()
{
    QStandardItemModel myPlayList;

    ManageHeaderBar myControl;

    QSignalSpy currentTrackChangedSpy(&myControl, &ManageHeaderBar::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageHeaderBar::playListModelChanged);
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
    QSignalSpy remainingTracksChangedSpy(&myControl, &ManageHeaderBar::remainingTracksChanged);

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
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
    QCOMPARE(myControl.remainingTracks(), 0);

    myControl.setArtistRole(ManageHeaderBarTest::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.artistRole(), static_cast<int>(ManageHeaderBarTest::ArtistRole));

    myControl.setTitleRole(ManageHeaderBarTest::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.titleRole(), static_cast<int>(ManageHeaderBarTest::TitleRole));

    myControl.setAlbumRole(ManageHeaderBarTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 0);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumRole(), static_cast<int>(ManageHeaderBarTest::AlbumRole));

    myControl.setImageRole(ManageHeaderBarTest::ImageRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.imageRole(), static_cast<int>(ManageHeaderBarTest::ImageRole));

    myControl.setAlbumIdRole(ManageHeaderBarTest::AlbumIdRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumIdRole(), static_cast<int>(ManageHeaderBarTest::AlbumIdRole));

    myControl.setIsValidRole(ManageHeaderBarTest::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.isValidRole(), static_cast<int>(ManageHeaderBarTest::IsValidRole));

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
    QCOMPARE(myControl.remainingTracks(), 1);

    auto firstItem = myPlayList.item(0, 0);
    firstItem->setData(QUrl::fromUserInput(QStringLiteral("file://0.mp3")), ManageHeaderBarTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    auto currentItem = myPlayList.item(1, 0);
    currentItem->setData(QStringLiteral("artist1"), ManageHeaderBarTest::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.artist(), QVariant(QStringLiteral("artist1")));

    currentItem->setData(QStringLiteral("song1"), ManageHeaderBarTest::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.title(), QVariant(QStringLiteral("song1")));

    currentItem->setData(QStringLiteral("album1"), ManageHeaderBarTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.album(), QVariant(QStringLiteral("album1")));

    currentItem->setData(QUrl::fromUserInput(QStringLiteral("file://image.png")), ManageHeaderBarTest::ImageRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.image(), QUrl::fromUserInput(QStringLiteral("file://image.png")));

    currentItem->setData(qulonglong(12), ManageHeaderBarTest::AlbumIdRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 1);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.albumId(), 12);

    currentItem->setData(true, ManageHeaderBarTest::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 1);
    QCOMPARE(isValidChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.isValid(), true);

    QCOMPARE(myPlayList.removeRow(2), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 1);
    QCOMPARE(isValidChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 2);

    QCOMPARE(myControl.remainingTracks(), 0);

    myControl.setCurrentTrack({});

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 2);
    QCOMPARE(titleChangedSpy.count(), 2);
    QCOMPARE(albumChangedSpy.count(), 2);
    QCOMPARE(imageChangedSpy.count(), 2);
    QCOMPARE(albumIdChangedSpy.count(), 2);
    QCOMPARE(isValidChangedSpy.count(), 2);
    QCOMPARE(remainingTracksChangedSpy.count(), 2);

    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex());
    QCOMPARE(myControl.artist().toString().isEmpty(), true);
    QCOMPARE(myControl.title().toString().isEmpty(), true);
    QCOMPARE(myControl.album().toString().isEmpty(), true);
    QCOMPARE(myControl.image().toString().isEmpty(), true);
    QCOMPARE(myControl.albumId(), 0);
    QCOMPARE(myControl.isValid(), false);
    QCOMPARE(myControl.remainingTracks(), 0);

    myControl.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 3);
    QCOMPARE(titleChangedSpy.count(), 3);
    QCOMPARE(albumChangedSpy.count(), 3);
    QCOMPARE(imageChangedSpy.count(), 3);
    QCOMPARE(albumIdChangedSpy.count(), 3);
    QCOMPARE(isValidChangedSpy.count(), 3);
    QCOMPARE(remainingTracksChangedSpy.count(), 2);

    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
    QCOMPARE(myControl.artist().toString(), QStringLiteral("artist1"));
    QCOMPARE(myControl.title().toString(), QStringLiteral("song1"));
    QCOMPARE(myControl.album().toString(), QStringLiteral("album1"));
    QCOMPARE(myControl.image().toString(), QStringLiteral("file://image.png"));
    QCOMPARE(myControl.isValid(), true);
    QCOMPARE(myControl.remainingTracks(), 0);

    myPlayList.appendRow(new QStandardItem);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 3);
    QCOMPARE(titleChangedSpy.count(), 3);
    QCOMPARE(albumChangedSpy.count(), 3);
    QCOMPARE(imageChangedSpy.count(), 3);
    QCOMPARE(albumIdChangedSpy.count(), 3);
    QCOMPARE(isValidChangedSpy.count(), 3);
    QCOMPARE(remainingTracksChangedSpy.count(), 3);

    QCOMPARE(myControl.remainingTracks(), 1);

    auto lastItem = myPlayList.item(2, 0);
    lastItem->setData(QUrl::fromUserInput(QStringLiteral("file://2.mp3")), ManageHeaderBarTest::ResourceRole);
    lastItem->setData(QStringLiteral("artist1"), ManageHeaderBarTest::ArtistRole);
    lastItem->setData(QStringLiteral("song2"), ManageHeaderBarTest::TitleRole);
    lastItem->setData(QStringLiteral("album1"), ManageHeaderBarTest::AlbumRole);
    lastItem->setData(QUrl::fromUserInput(QStringLiteral("file://image.png")), ManageHeaderBarTest::ImageRole);
    lastItem->setData(qulonglong(12), ManageHeaderBarTest::AlbumIdRole);
    lastItem->setData(true, ManageHeaderBarTest::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 3);
    QCOMPARE(titleChangedSpy.count(), 3);
    QCOMPARE(albumChangedSpy.count(), 3);
    QCOMPARE(imageChangedSpy.count(), 3);
    QCOMPARE(albumIdChangedSpy.count(), 3);
    QCOMPARE(isValidChangedSpy.count(), 3);
    QCOMPARE(remainingTracksChangedSpy.count(), 3);

    myControl.setCurrentTrack(myPlayList.index(2, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 4);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 3);
    QCOMPARE(titleChangedSpy.count(), 4);
    QCOMPARE(albumChangedSpy.count(), 3);
    QCOMPARE(imageChangedSpy.count(), 3);
    QCOMPARE(albumIdChangedSpy.count(), 3);
    QCOMPARE(isValidChangedSpy.count(), 3);
    QCOMPARE(remainingTracksChangedSpy.count(), 4);

    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));
    QCOMPARE(myControl.title().toString(), QStringLiteral("song2"));
    QCOMPARE(myControl.albumId(), 12);
    QCOMPARE(myControl.remainingTracks(), 0);
}

void ManageHeaderBarTest::setCurrentTrackAndInvalidCase()
{
    QStandardItemModel myPlayList;

    ManageHeaderBar myControl;

    QSignalSpy currentTrackChangedSpy(&myControl, &ManageHeaderBar::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageHeaderBar::playListModelChanged);
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
    QSignalSpy remainingTracksChangedSpy(&myControl, &ManageHeaderBar::remainingTracksChanged);

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
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
    QCOMPARE(myControl.remainingTracks(), 0);

    myControl.setArtistRole(ManageHeaderBarTest::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.artistRole(), static_cast<int>(ManageHeaderBarTest::ArtistRole));

    myControl.setTitleRole(ManageHeaderBarTest::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.titleRole(), static_cast<int>(ManageHeaderBarTest::TitleRole));

    myControl.setAlbumRole(ManageHeaderBarTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 0);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumRole(), static_cast<int>(ManageHeaderBarTest::AlbumRole));

    myControl.setImageRole(ManageHeaderBarTest::ImageRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.imageRole(), static_cast<int>(ManageHeaderBarTest::ImageRole));

    myControl.setAlbumIdRole(ManageHeaderBarTest::AlbumIdRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumIdRole(), static_cast<int>(ManageHeaderBarTest::AlbumIdRole));

    myControl.setIsValidRole(ManageHeaderBarTest::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.isValidRole(), static_cast<int>(ManageHeaderBarTest::IsValidRole));

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
    QCOMPARE(myControl.remainingTracks(), 1);

    auto firstItem = myPlayList.item(0, 0);
    firstItem->setData(QUrl::fromUserInput(QStringLiteral("file://0.mp3")), ManageHeaderBarTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    auto currentItem = myPlayList.item(1, 0);
    currentItem->setData(QStringLiteral("artist1"), ManageHeaderBarTest::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.artist(), QVariant(QStringLiteral("artist1")));

    currentItem->setData(QStringLiteral("song1"), ManageHeaderBarTest::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.title(), QVariant(QStringLiteral("song1")));

    currentItem->setData(QStringLiteral("album1"), ManageHeaderBarTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.album(), QVariant(QStringLiteral("album1")));

    currentItem->setData(QUrl::fromUserInput(QStringLiteral("file://image.png")), ManageHeaderBarTest::ImageRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.image(), QUrl::fromUserInput(QStringLiteral("file://image.png")));

    currentItem->setData(qulonglong(12), ManageHeaderBarTest::AlbumIdRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 1);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.albumId(), qulonglong(12));

    currentItem->setData(true, ManageHeaderBarTest::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 1);
    QCOMPARE(isValidChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.isValid(), true);

    QCOMPARE(myPlayList.removeRow(2), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 1);
    QCOMPARE(isValidChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 2);

    QCOMPARE(myControl.remainingTracks(), 0);

    QCOMPARE(myPlayList.removeRows(0, 2), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 2);
    QCOMPARE(titleChangedSpy.count(), 2);
    QCOMPARE(albumChangedSpy.count(), 2);
    QCOMPARE(imageChangedSpy.count(), 2);
    QCOMPARE(albumIdChangedSpy.count(), 2);
    QCOMPARE(isValidChangedSpy.count(), 2);
    QCOMPARE(remainingTracksChangedSpy.count(), 2);

    QCOMPARE(myControl.artist().toString().isEmpty(), true);
    QCOMPARE(myControl.title().toString().isEmpty(), true);
    QCOMPARE(myControl.album().toString().isEmpty(), true);
    QCOMPARE(myControl.image().toString().isEmpty(), true);
    QCOMPARE(myControl.isValid(), false);
    QCOMPARE(myControl.remainingTracks(), 0);
}

void ManageHeaderBarTest::setCurrentTrackAndRemoveItCase()
{
    QStandardItemModel myPlayList;

    ManageHeaderBar myControl;

    QSignalSpy currentTrackChangedSpy(&myControl, &ManageHeaderBar::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageHeaderBar::playListModelChanged);
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
    QSignalSpy remainingTracksChangedSpy(&myControl, &ManageHeaderBar::remainingTracksChanged);

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
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
    QCOMPARE(myControl.remainingTracks(), 0);

    myControl.setArtistRole(ManageHeaderBarTest::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.artistRole(), static_cast<int>(ManageHeaderBarTest::ArtistRole));

    myControl.setTitleRole(ManageHeaderBarTest::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.titleRole(), static_cast<int>(ManageHeaderBarTest::TitleRole));

    myControl.setAlbumRole(ManageHeaderBarTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 0);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumRole(), static_cast<int>(ManageHeaderBarTest::AlbumRole));

    myControl.setImageRole(ManageHeaderBarTest::ImageRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.imageRole(), static_cast<int>(ManageHeaderBarTest::ImageRole));

    myControl.setAlbumIdRole(ManageHeaderBarTest::AlbumIdRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumIdRole(), static_cast<int>(ManageHeaderBarTest::AlbumIdRole));

    myControl.setIsValidRole(ManageHeaderBarTest::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.isValidRole(), static_cast<int>(ManageHeaderBarTest::IsValidRole));

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    auto firstItem = myPlayList.item(0, 0);
    firstItem->setData(QUrl::fromUserInput(QStringLiteral("file://0.mp3")), ManageHeaderBarTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    firstItem->setData(QStringLiteral("artist1"), ManageHeaderBarTest::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    firstItem->setData(QStringLiteral("song1"), ManageHeaderBarTest::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    firstItem->setData(QStringLiteral("album1"), ManageHeaderBarTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    firstItem->setData(QUrl::fromUserInput(QStringLiteral("file://image.png")), ManageHeaderBarTest::ImageRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    firstItem->setData(true, ManageHeaderBarTest::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
    QCOMPARE(myControl.remainingTracks(), 1);
    QCOMPARE(myControl.isValid(), true);
    QCOMPARE(myControl.image(), QUrl::fromUserInput(QStringLiteral("file://image.png")));
    QCOMPARE(myControl.album(), QVariant(QStringLiteral("album1")));
    QCOMPARE(myControl.artist(), QVariant(QStringLiteral("artist1")));
    QCOMPARE(myControl.title(), QVariant(QStringLiteral("song1")));

    QCOMPARE(myPlayList.removeRow(0), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 2);
    QCOMPARE(titleChangedSpy.count(), 2);
    QCOMPARE(albumChangedSpy.count(), 2);
    QCOMPARE(imageChangedSpy.count(), 2);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 2);
    QCOMPARE(remainingTracksChangedSpy.count(), 2);

    QCOMPARE(myControl.artist().toString().isEmpty(), true);
    QCOMPARE(myControl.title().toString().isEmpty(), true);
    QCOMPARE(myControl.album().toString().isEmpty(), true);
    QCOMPARE(myControl.image().toString().isEmpty(), true);
    QCOMPARE(myControl.isValid(), false);
    QCOMPARE(myControl.remainingTracks(), 0);
}

void ManageHeaderBarTest::insertBeforeCurrentTrack()
{
    QStandardItemModel myPlayList;

    ManageHeaderBar myControl;

    QSignalSpy currentTrackChangedSpy(&myControl, &ManageHeaderBar::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageHeaderBar::playListModelChanged);
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
    QSignalSpy remainingTracksChangedSpy(&myControl, &ManageHeaderBar::remainingTracksChanged);

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
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
    QCOMPARE(myControl.remainingTracks(), 0);

    myControl.setArtistRole(ManageHeaderBarTest::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.artistRole(), static_cast<int>(ManageHeaderBarTest::ArtistRole));

    myControl.setTitleRole(ManageHeaderBarTest::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.titleRole(), static_cast<int>(ManageHeaderBarTest::TitleRole));

    myControl.setAlbumRole(ManageHeaderBarTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 0);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumRole(), static_cast<int>(ManageHeaderBarTest::AlbumRole));

    myControl.setImageRole(ManageHeaderBarTest::ImageRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.imageRole(), static_cast<int>(ManageHeaderBarTest::ImageRole));

    myControl.setAlbumIdRole(ManageHeaderBarTest::AlbumIdRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumIdRole(), static_cast<int>(ManageHeaderBarTest::AlbumIdRole));

    myControl.setIsValidRole(ManageHeaderBarTest::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.isValidRole(), static_cast<int>(ManageHeaderBarTest::IsValidRole));

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    auto firstItem = myPlayList.item(0, 0);
    firstItem->setData(QUrl::fromUserInput(QStringLiteral("file://0.mp3")), ManageHeaderBarTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    firstItem->setData(QStringLiteral("artist1"), ManageHeaderBarTest::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    firstItem->setData(QStringLiteral("song1"), ManageHeaderBarTest::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    firstItem->setData(QStringLiteral("album1"), ManageHeaderBarTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    firstItem->setData(QUrl::fromUserInput(QStringLiteral("file://image.png")), ManageHeaderBarTest::ImageRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    firstItem->setData(true, ManageHeaderBarTest::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
    QCOMPARE(myControl.remainingTracks(), 1);
    QCOMPARE(myControl.isValid(), true);
    QCOMPARE(myControl.image(), QUrl::fromUserInput(QStringLiteral("file://image.png")));
    QCOMPARE(myControl.album(), QVariant(QStringLiteral("album1")));
    QCOMPARE(myControl.artist(), QVariant(QStringLiteral("artist1")));
    QCOMPARE(myControl.title(), QVariant(QStringLiteral("song1")));

    myPlayList.insertRow(0, new QStandardItem);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);
}

void ManageHeaderBarTest::moveCurrentTrack()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    ManageHeaderBar myControl;

    QSignalSpy currentTrackChangedSpy(&myControl, &ManageHeaderBar::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageHeaderBar::playListModelChanged);
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
    QSignalSpy remainingTracksChangedSpy(&myControl, &ManageHeaderBar::remainingTracksChanged);

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
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
    QCOMPARE(myControl.remainingTracks(), 0);

    myControl.setArtistRole(MediaPlayList::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.artistRole(), static_cast<int>(MediaPlayList::ArtistRole));

    myControl.setTitleRole(MediaPlayList::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.titleRole(), static_cast<int>(MediaPlayList::TitleRole));

    myControl.setAlbumRole(MediaPlayList::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 0);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumRole(), static_cast<int>(MediaPlayList::AlbumRole));

    myControl.setImageRole(MediaPlayList::ImageUrlRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.imageRole(), static_cast<int>(MediaPlayList::ImageUrlRole));

    myControl.setAlbumIdRole(MediaPlayList::AlbumIdRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumIdRole(), static_cast<int>(MediaPlayList::AlbumIdRole));

    myControl.setIsValidRole(MediaPlayList::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.isValidRole(), static_cast<int>(MediaPlayList::IsValidRole));

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2),
                        QStringLiteral("track2")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 1);
    QCOMPARE(isValidChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
    QCOMPARE(myControl.remainingTracks(), 1);
    QCOMPARE(myControl.isValid(), true);
    QCOMPARE(myControl.image(), QUrl::fromUserInput(QStringLiteral("file:album2")));
    QCOMPARE(myControl.album(), QVariant(QStringLiteral("album2")));
    QCOMPARE(myControl.artist(), QVariant(QStringLiteral("artist1")));
    QCOMPARE(myControl.title(), QVariant(QStringLiteral("track1")));
    QCOMPARE(myControl.albumId(), qulonglong(2));

    QCOMPARE(myPlayList.moveRow(myControl.currentTrack().parent(), 0, myControl.currentTrack().parent(), 2), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 1);
    QCOMPARE(isValidChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 2);

    QCOMPARE(myControl.remainingTracks(), 0);
}

void ManageHeaderBarTest::moveAnotherTrack()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    ManageHeaderBar myControl;

    QSignalSpy currentTrackChangedSpy(&myControl, &ManageHeaderBar::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageHeaderBar::playListModelChanged);
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
    QSignalSpy remainingTracksChangedSpy(&myControl, &ManageHeaderBar::remainingTracksChanged);

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
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
    QCOMPARE(myControl.remainingTracks(), 0);

    myControl.setArtistRole(MediaPlayList::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.artistRole(), static_cast<int>(MediaPlayList::ArtistRole));

    myControl.setTitleRole(MediaPlayList::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.titleRole(), static_cast<int>(MediaPlayList::TitleRole));

    myControl.setAlbumRole(MediaPlayList::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 0);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumRole(), static_cast<int>(MediaPlayList::AlbumRole));

    myControl.setImageRole(MediaPlayList::ImageUrlRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.imageRole(), static_cast<int>(MediaPlayList::ImageUrlRole));

    myControl.setAlbumIdRole(MediaPlayList::AlbumIdRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumIdRole(), static_cast<int>(MediaPlayList::AlbumIdRole));

    myControl.setIsValidRole(MediaPlayList::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.isValidRole(), static_cast<int>(MediaPlayList::IsValidRole));

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    auto newFiles = QList<QUrl>();
    const auto &constNewTracks = mNewTracks;
    for (const auto &oneTrack : constNewTracks) {
        newFiles.push_back(oneTrack.resourceURI());
    }

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2),
                        QStringLiteral("track2")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 1);
    QCOMPARE(isValidChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
    QCOMPARE(myControl.remainingTracks(), 1);
    QCOMPARE(myControl.isValid(), true);
    QCOMPARE(myControl.image(), QUrl::fromUserInput(QStringLiteral("file:album2")));
    QCOMPARE(myControl.album(), QVariant(QStringLiteral("album2")));
    QCOMPARE(myControl.artist(), QVariant(QStringLiteral("artist1")));
    QCOMPARE(myControl.title(), QVariant(QStringLiteral("track1")));
    QCOMPARE(myControl.albumId(), qulonglong(2));

    QCOMPARE(myPlayList.moveRow(myControl.currentTrack().parent(), 1, myControl.currentTrack().parent(), 0), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 1);
    QCOMPARE(isValidChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 2);

    QCOMPARE(myControl.remainingTracks(), 0);
}

void ManageHeaderBarTest::setCurrentTrackTest()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    ManageHeaderBar myControl;

    QSignalSpy currentTrackChangedSpy(&myControl, &ManageHeaderBar::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageHeaderBar::playListModelChanged);
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
    QSignalSpy remainingTracksChangedSpy(&myControl, &ManageHeaderBar::remainingTracksChanged);

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
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
    QCOMPARE(myControl.remainingTracks(), 0);

    myControl.setArtistRole(MediaPlayList::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.artistRole(), static_cast<int>(MediaPlayList::ArtistRole));

    myControl.setTitleRole(MediaPlayList::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.titleRole(), static_cast<int>(MediaPlayList::TitleRole));

    myControl.setAlbumRole(MediaPlayList::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 0);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumRole(), static_cast<int>(MediaPlayList::AlbumRole));

    myControl.setImageRole(MediaPlayList::ImageUrlRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 0);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.imageRole(), static_cast<int>(MediaPlayList::ImageUrlRole));

    myControl.setAlbumIdRole(MediaPlayList::AlbumIdRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 0);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.albumIdRole(), static_cast<int>(MediaPlayList::AlbumIdRole));

    myControl.setIsValidRole(MediaPlayList::IsValidRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.isValidRole(), static_cast<int>(MediaPlayList::IsValidRole));

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    auto newFiles = QList<QUrl>();
    const auto &constNewTracks = mNewTracks;
    for (const auto &oneTrack : constNewTracks) {
        newFiles.push_back(oneTrack.resourceURI());
    }

    myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album2"), 1, 1),
                        QStringLiteral("track1")},
                       ElisaUtils::Track);
    myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 2),
                        QStringLiteral("track2")},
                       ElisaUtils::Track);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 0);
    QCOMPARE(titleChangedSpy.count(), 0);
    QCOMPARE(albumChangedSpy.count(), 0);
    QCOMPARE(imageChangedSpy.count(), 0);
    QCOMPARE(albumIdChangedSpy.count(), 0);
    QCOMPARE(isValidChangedSpy.count(), 0);
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 1);
    QCOMPARE(isValidChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);

    QCOMPARE(myControl.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));
    QCOMPARE(myControl.remainingTracks(), 1);
    QCOMPARE(myControl.isValid(), true);
    QCOMPARE(myControl.image(), QUrl::fromUserInput(QStringLiteral("file:album2")));
    QCOMPARE(myControl.album(), QVariant(QStringLiteral("album2")));
    QCOMPARE(myControl.artist(), QVariant(QStringLiteral("artist1")));
    QCOMPARE(myControl.title(), QVariant(QStringLiteral("track1")));
    QCOMPARE(myControl.albumId(), qulonglong(2));

    myControl.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(artistRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(albumRoleChangedSpy.count(), 1);
    QCOMPARE(imageRoleChangedSpy.count(), 1);
    QCOMPARE(albumIdRoleChangedSpy.count(), 1);
    QCOMPARE(isValidRoleChangedSpy.count(), 1);
    QCOMPARE(artistChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(albumChangedSpy.count(), 1);
    QCOMPARE(imageChangedSpy.count(), 1);
    QCOMPARE(albumIdChangedSpy.count(), 1);
    QCOMPARE(isValidChangedSpy.count(), 1);
    QCOMPARE(remainingTracksChangedSpy.count(), 1);
}

void ManageHeaderBarTest::setPlayListModelTest()
{
    MediaPlayList myPlayList;
    DatabaseInterface myDatabaseContent;
    TracksListener myListener(&myDatabaseContent);

    myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

    connect(&myListener, &TracksListener::trackHasChanged,
            &myPlayList, &MediaPlayList::trackChanged);
    connect(&myPlayList, &MediaPlayList::newEntryInList,
            &myListener, &TracksListener::newEntryInList);
    connect(&myPlayList, &MediaPlayList::newTrackByNameInList,
            &myListener, &TracksListener::trackByNameInList);
    connect(&myDatabaseContent, &DatabaseInterface::tracksAdded,
            &myListener, &TracksListener::tracksAdded);

    ManageHeaderBar myControl;

    QSignalSpy currentTrackChangedSpy(&myControl, &ManageHeaderBar::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myControl, &ManageHeaderBar::playListModelChanged);
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
    QSignalSpy remainingTracksChangedSpy(&myControl, &ManageHeaderBar::remainingTracksChanged);

    QCOMPARE(myControl.currentTrack().isValid(), false);
    QCOMPARE(myControl.playListModel(), static_cast<void*>(nullptr));
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
    QCOMPARE(myControl.remainingTracks(), 0);

    myControl.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), &myPlayList);

    myControl.setPlayListModel(nullptr);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 2);
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
    QCOMPARE(remainingTracksChangedSpy.count(), 0);

    QCOMPARE(myControl.playListModel(), static_cast<QAbstractItemModel*>(nullptr));
}

QTEST_GUILESS_MAIN(ManageHeaderBarTest)


#include "moc_manageheaderbartest.cpp"
