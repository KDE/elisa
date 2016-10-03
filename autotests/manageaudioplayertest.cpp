/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "manageaudioplayertest.h"

#include "manageaudioplayer.h"

#include <QtTest/QtTest>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QUrl>

ManageAudioPlayerTest::ManageAudioPlayerTest(QObject *parent) : QObject(parent)
{
}

void ManageAudioPlayerTest::initTestCase()
{
}

void ManageAudioPlayerTest::simpleInitialCase()
{
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));
}

QTEST_MAIN(ManageAudioPlayerTest)


#include "moc_manageaudioplayertest.cpp"
