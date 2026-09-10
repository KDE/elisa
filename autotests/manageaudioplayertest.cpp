/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "manageaudioplayertest.h"

#include "manageaudioplayer.h"

#include "elisa_settings.h"

#include <QSignalSpy>
#include <QTest>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QUrl>

ManageAudioPlayerTest::ManageAudioPlayerTest(QObject *parent) : QObject(parent)
{
}

void ManageAudioPlayerTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
}

void ManageAudioPlayerTest::simpleInitialCase()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    myPlayer.setPlayerStatus(QMediaPlayer::NoMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::NoMedia);

    QCOMPARE(skipNextTrackSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
}

void ManageAudioPlayerTest::noPlayCase()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(100), false);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
}

void ManageAudioPlayerTest::skipNextTrack()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    QCOMPARE(playerStopSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    QCOMPARE(skipNextTrackSpy.wait(100), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
}

void ManageAudioPlayerTest::skipNextTrackWithRandomPlay()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    QCOMPARE(playerStopSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    QCOMPARE(skipNextTrackSpy.wait(100), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
}

void ManageAudioPlayerTest::skipPreviousTrack()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///2.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    QCOMPARE(playerStopSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    QCOMPARE(skipNextTrackSpy.wait(100), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
}

void ManageAudioPlayerTest::playTrackAndskipNextTrack()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    myPlayer.setPlayerStatus(QMediaPlayer::NoMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::NoMedia);

    QCOMPARE(skipNextTrackSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///2.mp3")));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 7);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 7);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 7);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setCurrentTrack(myPlayList.index(2, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));

    QCOMPARE(playerStopSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///3.mp3")));
    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    QCOMPARE(skipNextTrackSpy.wait(100), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 9);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 10);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 10);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 3);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 10);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 5);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 3);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));
}

void ManageAudioPlayerTest::playTrackAndskipPreviousTrack()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///2.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    myPlayer.setPlayerStatus(QMediaPlayer::NoMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::NoMedia);

    QCOMPARE(skipNextTrackSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setCurrentTrack(myPlayList.index(2, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///3.mp3")));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 7);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 7);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 7);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    QCOMPARE(playerStopSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///2.mp3")));
    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    QCOMPARE(skipNextTrackSpy.wait(100), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 9);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 10);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 10);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 3);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 10);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 5);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 3);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));
}

void ManageAudioPlayerTest::skipNextTrackAndPlayTrack()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    QCOMPARE(playerStopSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///2.mp3")));
    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    QCOMPARE(skipNextTrackSpy.wait(100), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 7);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 7);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    myPlayer.setPlayerStatus(QMediaPlayer::NoMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::NoMedia);

    QCOMPARE(skipNextTrackSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setCurrentTrack(myPlayList.index(2, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///3.mp3")));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 9);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 10);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 10);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 3);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
}

void ManageAudioPlayerTest::skipPreviousTrackAndPlayTrack()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///2.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    QCOMPARE(playerStopSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));
    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    QCOMPARE(skipNextTrackSpy.wait(100), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 7);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 7);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    myPlayer.setPlayerStatus(QMediaPlayer::NoMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::NoMedia);

    QCOMPARE(skipNextTrackSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 8);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///2.mp3")));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 9);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 10);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 10);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 3);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
}

void ManageAudioPlayerTest::playLastCase()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setCurrentTrack(myPlayList.index(2, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(2, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///3.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    myPlayer.setPlayerStatus(QMediaPlayer::NoMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::NoMedia);

    QCOMPARE(skipNextTrackSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.playListFinished();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    QCOMPARE(playerPlaySpy.wait(100), false);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
}

void ManageAudioPlayerTest::playSingleAndClearPlayListTrack()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setCurrentTrack({});

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex{});

    QCOMPARE(playerStopSpy.wait(300), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    QCOMPARE(playerPlaySpy.wait(300), false);

    QCOMPARE(currentTrackChangedSpy.count(), 3);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 4);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
}

void ManageAudioPlayerTest::playSingleTrack()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    myPlayer.setPlayerStatus(QMediaPlayer::NoMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::NoMedia);

    QCOMPARE(skipNextTrackSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.playListFinished();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    QCOMPARE(playerPlaySpy.wait(100), false);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
}

void ManageAudioPlayerTest::playRestoredTrack()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
}

void ManageAudioPlayerTest::testRestoreSettingsAutomaticPlay()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    Elisa::ElisaConfiguration::self()->setPlayAtStartup(true);
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy titleRoleChangedSpy(&myPlayer, &ManageAudioPlayer::titleRoleChanged);
    QSignalSpy artistNameRoleChangedSpy(&myPlayer, &ManageAudioPlayer::artistNameRoleChanged);
    QSignalSpy albumNameRoleChangedSpy(&myPlayer, &ManageAudioPlayer::albumNameRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("title"), ManageAudioPlayerTest::TitleRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("artist"), ManageAudioPlayerTest::ArtistRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("album"), ManageAudioPlayerTest::AlbumRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setTitleRole(ManageAudioPlayerTest::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.titleRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::TitleRole));

    myPlayer.setArtistNameRole(ManageAudioPlayerTest::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.artistNameRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::ArtistRole));

    myPlayer.setAlbumNameRole(ManageAudioPlayerTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.albumNameRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::AlbumRole));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QVariantMap settings;
    settings[QStringLiteral("isPlaying")] = true;
    settings[QStringLiteral("audioPlayerCurrentTitle")] = QStringLiteral("title");
    settings[QStringLiteral("audioPlayerCurrentArtistName")] = QStringLiteral("artist");
    settings[QStringLiteral("audioPlayerCurrentAlbumName")] = QStringLiteral("album");

    myPlayer.setPersistentState(settings);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    QCOMPARE(playerPlaySpy.wait(500), false);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    QCOMPARE(playerPlaySpy.wait(), true);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    QCOMPARE(skipNextTrackSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::InvalidMedia);

    QCOMPARE(skipNextTrackSpy.wait(150), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 2);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::InvalidMedia);
}

void ManageAudioPlayerTest::testRestoreSettingsNoAutomaticPlay()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy titleRoleChangedSpy(&myPlayer, &ManageAudioPlayer::titleRoleChanged);
    QSignalSpy artistNameRoleChangedSpy(&myPlayer, &ManageAudioPlayer::artistNameRoleChanged);
    QSignalSpy albumNameRoleChangedSpy(&myPlayer, &ManageAudioPlayer::albumNameRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("title"), ManageAudioPlayerTest::TitleRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("artist"), ManageAudioPlayerTest::ArtistRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("album"), ManageAudioPlayerTest::AlbumRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QVariantMap settings;
    settings[QStringLiteral("isPlaying")] = false;
    settings[QStringLiteral("audioPlayerCurrentTitle")] = QStringLiteral("title");
    settings[QStringLiteral("audioPlayerCurrentArtistName")] = QStringLiteral("artist");
    settings[QStringLiteral("audioPlayerCurrentAlbumName")] = QStringLiteral("album");

    myPlayer.setPersistentState(settings);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setTitleRole(ManageAudioPlayerTest::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.titleRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::TitleRole));

    myPlayer.setArtistNameRole(ManageAudioPlayerTest::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.artistNameRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::ArtistRole));

    myPlayer.setAlbumNameRole(ManageAudioPlayerTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.albumNameRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::AlbumRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(1000), false);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
}

void ManageAudioPlayerTest::testRestoreSettingsAutomaticPlayAndPosition()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    Elisa::ElisaConfiguration::self()->setPlayAtStartup(true);
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy titleRoleChangedSpy(&myPlayer, &ManageAudioPlayer::titleRoleChanged);
    QSignalSpy artistNameRoleChangedSpy(&myPlayer, &ManageAudioPlayer::artistNameRoleChanged);
    QSignalSpy albumNameRoleChangedSpy(&myPlayer, &ManageAudioPlayer::albumNameRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("title"), ManageAudioPlayerTest::TitleRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("artist"), ManageAudioPlayerTest::ArtistRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("album"), ManageAudioPlayerTest::AlbumRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QVariantMap settings;
    settings[QStringLiteral("isPlaying")] = true;
    settings[QStringLiteral("audioPlayerCurrentTitle")] = QStringLiteral("title");
    settings[QStringLiteral("audioPlayerCurrentArtistName")] = QStringLiteral("artist");
    settings[QStringLiteral("audioPlayerCurrentAlbumName")] = QStringLiteral("album");
    settings[QStringLiteral("playerPosition")] = 10000;

    myPlayer.setPersistentState(settings);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setTitleRole(ManageAudioPlayerTest::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.titleRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::TitleRole));

    myPlayer.setArtistNameRole(ManageAudioPlayerTest::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.artistNameRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::ArtistRole));

    myPlayer.setAlbumNameRole(ManageAudioPlayerTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(seekSpy.at(0).at(0).toInt(), 10000);
    QCOMPARE(myPlayer.albumNameRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::AlbumRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    myPlayer.setPlayerStatus(QMediaPlayer::NoMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::NoMedia);

    QCOMPARE(skipNextTrackSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 1);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
}

void ManageAudioPlayerTest::playTrackPauseAndSkipNextTrack()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(playerPauseSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 1);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PausedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 1);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PausedState));

    myPlayer.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 1);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    QCOMPARE(playerStopSpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 1);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 1);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///2.mp3")));
    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    QCOMPARE(skipNextTrackSpy.wait(100), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 1);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 1);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    myPlayer.playPause();

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 1);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(playerPlaySpy.wait(100), true);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 3);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 1);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 6);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 1);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 3);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 7);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 4);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 2);
    QCOMPARE(playerPauseSpy.count(), 1);
    QCOMPARE(playerStopSpy.count(), 1);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);
}

void ManageAudioPlayerTest::testRestoreSettingsNoPlayWrongTrack()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy titleRoleChangedSpy(&myPlayer, &ManageAudioPlayer::titleRoleChanged);
    QSignalSpy artistNameRoleChangedSpy(&myPlayer, &ManageAudioPlayer::artistNameRoleChanged);
    QSignalSpy albumNameRoleChangedSpy(&myPlayer, &ManageAudioPlayer::albumNameRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("title"), ManageAudioPlayerTest::TitleRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("artist"), ManageAudioPlayerTest::ArtistRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("album"), ManageAudioPlayerTest::AlbumRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QStringLiteral("title2"), ManageAudioPlayerTest::TitleRole);
    myPlayList.item(1, 0)->setData(QStringLiteral("artist2"), ManageAudioPlayerTest::ArtistRole);
    myPlayList.item(1, 0)->setData(QStringLiteral("album2"), ManageAudioPlayerTest::AlbumRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setTitleRole(ManageAudioPlayerTest::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.titleRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::TitleRole));

    myPlayer.setArtistNameRole(ManageAudioPlayerTest::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.artistNameRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::ArtistRole));

    myPlayer.setAlbumNameRole(ManageAudioPlayerTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.albumNameRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::AlbumRole));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QVariantMap settings;
    settings[QStringLiteral("isPlaying")] = true;
    settings[QStringLiteral("audioPlayerCurrentTitle")] = QStringLiteral("title");
    settings[QStringLiteral("audioPlayerCurrentArtistName")] = QStringLiteral("artist");
    settings[QStringLiteral("audioPlayerCurrentAlbumName")] = QStringLiteral("album");

    myPlayer.setPersistentState(settings);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setCurrentTrack(myPlayList.index(1, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));
    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///2.mp3")));

    QCOMPARE(playerPlaySpy.wait(500), false);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(500), false);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
}

void ManageAudioPlayerTest::testRestorePlayListAndSettingsAutomaticPlay()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    Elisa::ElisaConfiguration::self()->setPlayAtStartup(true);
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playListModelChangedSpy(&myPlayer, &ManageAudioPlayer::playListModelChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy urlRoleChangedSpy(&myPlayer, &ManageAudioPlayer::urlRoleChanged);
    QSignalSpy isPlayingRoleChangedSpy(&myPlayer, &ManageAudioPlayer::isPlayingRoleChanged);
    QSignalSpy titleRoleChangedSpy(&myPlayer, &ManageAudioPlayer::titleRoleChanged);
    QSignalSpy artistNameRoleChangedSpy(&myPlayer, &ManageAudioPlayer::artistNameRoleChanged);
    QSignalSpy albumNameRoleChangedSpy(&myPlayer, &ManageAudioPlayer::albumNameRoleChanged);
    QSignalSpy playerStatusChangedSpy(&myPlayer, &ManageAudioPlayer::playerStatusChanged);
    QSignalSpy playerPlaybackStateChangedSpy(&myPlayer, &ManageAudioPlayer::playerPlaybackStateChanged);
    QSignalSpy playerErrorChangedSpy(&myPlayer, &ManageAudioPlayer::playerErrorChanged);
    QSignalSpy persistentStateChangedSpy(&myPlayer, &ManageAudioPlayer::persistentStateChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy seekSpy(&myPlayer, &ManageAudioPlayer::seek);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QStringLiteral("title"), ManageAudioPlayerTest::TitleRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("artist"), ManageAudioPlayerTest::ArtistRole);
    myPlayList.item(0, 0)->setData(QStringLiteral("album"), ManageAudioPlayerTest::AlbumRole);
    myPlayList.item(1, 0)->setData(QStringLiteral("title2"), ManageAudioPlayerTest::TitleRole);
    myPlayList.item(1, 0)->setData(QStringLiteral("artist2"), ManageAudioPlayerTest::ArtistRole);
    myPlayList.item(1, 0)->setData(QStringLiteral("album2"), ManageAudioPlayerTest::AlbumRole);
    myPlayList.item(2, 0)->setData(QStringLiteral("title3"), ManageAudioPlayerTest::TitleRole);
    myPlayList.item(2, 0)->setData(QStringLiteral("artist3"), ManageAudioPlayerTest::ArtistRole);
    myPlayList.item(2, 0)->setData(QStringLiteral("album3"), ManageAudioPlayerTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 0);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setTitleRole(ManageAudioPlayerTest::TitleRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 0);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.titleRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::TitleRole));

    myPlayer.setArtistNameRole(ManageAudioPlayerTest::ArtistRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 0);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.artistNameRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::ArtistRole));

    myPlayer.setAlbumNameRole(ManageAudioPlayerTest::AlbumRole);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.albumNameRole(), static_cast<int>(ManageAudioPlayerTest::ManageAudioPlayerTest::AlbumRole));

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayListModel(&myPlayList);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 0);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QVariantMap settings;
    settings[QStringLiteral("isPlaying")] = true;
    settings[QStringLiteral("audioPlayerCurrentTitle")] = QStringLiteral("title");
    settings[QStringLiteral("audioPlayerCurrentArtistName")] = QStringLiteral("artist");
    settings[QStringLiteral("audioPlayerCurrentAlbumName")] = QStringLiteral("album");

    myPlayer.setPersistentState(settings);

    QCOMPARE(currentTrackChangedSpy.count(), 0);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 0);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 0);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.urlRole(), static_cast<int>(ManageAudioPlayerTest::ResourceRole));

    QCOMPARE(playerPlaySpy.wait(500), false);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 0);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.isPlayingRole(), static_cast<int>(ManageAudioPlayerTest::IsPlayingRole));

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(2, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///3.mp3")), ManageAudioPlayerTest::ResourceRole);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 0);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerSource(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadingMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerError(QMediaPlayer::NoError);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 1);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 2);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::LoadedMedia);

    myPlayer.setPlayerStatus(QMediaPlayer::BufferingMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::BufferingMedia);

    QCOMPARE(playerPlaySpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 0);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 3);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::PlayingState));

    myPlayer.setPlayerStatus(QMediaPlayer::EndOfMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 1);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::EndOfMedia);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerPlaybackState(), static_cast<int>(QMediaPlayer::StoppedState));

    QCOMPARE(skipNextTrackSpy.wait(), true);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 4);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    myPlayer.setPlayerStatus(QMediaPlayer::InvalidMedia);

    QCOMPARE(currentTrackChangedSpy.count(), 1);
    QCOMPARE(playListModelChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(urlRoleChangedSpy.count(), 1);
    QCOMPARE(isPlayingRoleChangedSpy.count(), 1);
    QCOMPARE(titleRoleChangedSpy.count(), 1);
    QCOMPARE(artistNameRoleChangedSpy.count(), 1);
    QCOMPARE(albumNameRoleChangedSpy.count(), 1);
    QCOMPARE(playerStatusChangedSpy.count(), 5);
    QCOMPARE(playerPlaybackStateChangedSpy.count(), 2);
    QCOMPARE(playerErrorChangedSpy.count(), 0);
    QCOMPARE(persistentStateChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerPauseSpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(seekSpy.count(), 0);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(1, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);
    QCOMPARE(myPlayList.data(myPlayList.index(2, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), false);

    QCOMPARE(myPlayer.playerStatus(), QMediaPlayer::InvalidMedia);

    QCOMPARE(skipNextTrackSpy.wait(300), true);
}


void ManageAudioPlayerTest::switchTrackWhilePaused()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerPauseSpy(&myPlayer, &ManageAudioPlayer::playerPause);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.appendRow(new QStandardItem);

    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);
    myPlayList.item(1, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///2.mp3")), ManageAudioPlayerTest::ResourceRole);

    myPlayer.setPlayListModel(&myPlayList);
    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);
    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    // play the first track
    myPlayer.setCurrentTrack(myPlayList.index(0, 0));
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.last().at(0).toUrl(), QUrl::fromUserInput(QStringLiteral("file:///1.mp3")));
    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);
    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);
    myPlayer.ensurePlay();
    QTRY_COMPARE(playerPlaySpy.count(), 1);
    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);
    myPlayer.setPlayerStatus(QMediaPlayer::BufferedMedia);

    // pause it
    myPlayer.playPause();
    QTRY_COMPARE(playerPauseSpy.count(), 1);
    myPlayer.setPlayerPlaybackState(QMediaPlayer::PausedState);

    // select the second track in the playlist, like a click on a playlist entry does
    myPlayer.setCurrentTrack(myPlayList.index(1, 0));
    myPlayer.ensurePlay();

    QTRY_COMPARE(playerStopSpy.count(), 1);
    QTest::qWait(50);

    // the player still has the first track loaded: asking it to play now would restart the first track
    QCOMPARE(playerPlaySpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);

    // the player reports it has stopped, the second track gets loaded
    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);
    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(playerSourceChangedSpy.last().at(0).toUrl(), QUrl::fromUserInput(QStringLiteral("file:///2.mp3")));

    // and is played once it is loaded
    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);
    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);
    QTRY_VERIFY(playerPlaySpy.count() > 1);
}

void ManageAudioPlayerTest::doubleClickReselectOfCurrentTrack()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    QStandardItemModel myPlayList;

    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy playerPlaySpy(&myPlayer, &ManageAudioPlayer::playerPlay);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);

    myPlayList.appendRow(new QStandardItem);
    myPlayList.item(0, 0)->setData(QUrl::fromUserInput(QStringLiteral("file:///1.mp3")), ManageAudioPlayerTest::ResourceRole);

    myPlayer.setPlayListModel(&myPlayList);
    myPlayer.setUrlRole(ManageAudioPlayerTest::ResourceRole);
    myPlayer.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);

    QCOMPARE(playerSourceChangedSpy.count(), 0);

    // First click of a double-click: select the track and start playback.
    myPlayer.setCurrentTrack(myPlayList.index(0, 0));

    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(playerPlaySpy.count(), 0);
    QCOMPARE(playerStopSpy.count(), 0);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);
    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    myPlayer.ensurePlay();
    QCOMPARE(playerPlaySpy.wait(300), true);
    QCOMPARE(playerPlaySpy.count(), 1);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);

    // Second click
    myPlayer.setCurrentTrack(myPlayList.index(0, 0));
    QCOMPARE(playerStopSpy.wait(300), true);
    QCOMPARE(playerStopSpy.count(), 1);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::StoppedState);

    QCOMPARE(playerSourceChangedSpy.count(), 2);

    myPlayer.setPlayerStatus(QMediaPlayer::LoadingMedia);
    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);

    QCOMPARE(playerPlaySpy.wait(300), true);
    QCOMPARE(playerPlaySpy.count(), 2);

    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);
    QCOMPARE(myPlayList.data(myPlayList.index(0, 0), ManageAudioPlayerTest::IsPlayingRole).toBool(), true);
}

namespace
{

class GaplessPlayListModel : public QStandardItemModel
{
public:
    void refreshTrack(int row)
    {
        Q_EMIT dataChanged(index(row, 0), index(row, 0), {});
    }
};

QUrl gaplessTrackUrl(int trackNumber)
{
    return QUrl::fromUserInput(QStringLiteral("file:///%1.mp3").arg(trackNumber));
}

bool isMarkedPlaying(const QStandardItemModel &playList, int row)
{
    return playList.data(playList.index(row, 0), ManageAudioPlayerTest::IsPlayingRole).toBool();
}

void setupGaplessFixture(ManageAudioPlayer &player, QStandardItemModel &playList, int trackCount)
{
    for (int row = 0; row < trackCount; ++row) {
        auto *item = new QStandardItem;
        item->setData(gaplessTrackUrl(row + 1), ManageAudioPlayerTest::ResourceRole);
        playList.appendRow(item);
    }

    QObject::connect(&player, &ManageAudioPlayer::prepareNextSource, &player, [&player, &playList]() {
        const auto nextRow = player.currentTrack().row() + 1;
        if (nextRow < playList.rowCount()) {
            player.setPendingNextSource(playList.data(playList.index(nextRow, 0), ManageAudioPlayerTest::ResourceRole).toUrl());
        }
    });
    QObject::connect(&player, &ManageAudioPlayer::skipNextTrack, &player, [&player, &playList]() {
        if (!player.currentTrack().isValid()) {
            return;
        }
        const auto nextRow = player.currentTrack().row() + 1;
        if (nextRow < playList.rowCount()) {
            player.setCurrentTrack(playList.index(nextRow, 0));
        }
    });

    player.setUrlRole(ManageAudioPlayerTest::ResourceRole);
    player.setIsPlayingRole(ManageAudioPlayerTest::IsPlayingRole);
    player.setPlayListModel(&playList);
    player.setCurrentTrack(playList.index(0, 0));
}

void startGaplessPlayback(ManageAudioPlayer &player)
{
    player.requestPlay();
    player.setPlayerStatus(QMediaPlayer::LoadedMedia);
    player.setPlayerPlaybackState(QMediaPlayer::PlayingState);
}

void endGaplessPlayback(ManageAudioPlayer &player)
{
    player.setPlayerStatus(QMediaPlayer::EndOfMedia);
    player.setPlayerPlaybackState(QMediaPlayer::StoppedState);
}
}

void ManageAudioPlayerTest::gaplessHandoffCase()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    GaplessPlayListModel myPlayList;

    QSignalSpy currentTrackChangedSpy(&myPlayer, &ManageAudioPlayer::currentTrackChanged);
    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy prepareNextSourceSpy(&myPlayer, &ManageAudioPlayer::prepareNextSource);
    QSignalSpy setNextSourceSpy(&myPlayer, &ManageAudioPlayer::setNextSourceInAudioWrapper);
    QSignalSpy startedPlayingTrackSpy(&myPlayer, &ManageAudioPlayer::startedPlayingTrack);
    QSignalSpy finishedPlayingTrackSpy(&myPlayer, &ManageAudioPlayer::finishedPlayingTrack);

    setupGaplessFixture(myPlayer, myPlayList, 2);

    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(playerSourceChangedSpy.at(0).at(0).toUrl(), gaplessTrackUrl(1));

    // A gapless handoff is not armed while the player is not playing.
    myPlayer.gaplessAboutToFinish();
    QCOMPARE(prepareNextSourceSpy.count(), 0);
    QCOMPARE(setNextSourceSpy.count(), 0);

    startGaplessPlayback(myPlayer);

    QCOMPARE(isMarkedPlaying(myPlayList, 0), true);
    QCOMPARE(isMarkedPlaying(myPlayList, 1), false);
    QCOMPARE(startedPlayingTrackSpy.count(), 1);

    // The backend reports that the current track is about to finish: Elisa
    // resolves the next source and arms the backend with it.
    myPlayer.gaplessAboutToFinish();

    QCOMPARE(prepareNextSourceSpy.count(), 1);
    QCOMPARE(setNextSourceSpy.count(), 1);
    QCOMPARE(setNextSourceSpy.takeFirst().at(0).toUrl(), gaplessTrackUrl(2));

    // The backend seamlessly switched to the next source: the play list must
    // advance exactly one track without stopping or reloading the source.
    myPlayer.gaplessTrackChanged(gaplessTrackUrl(2));

    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(currentTrackChangedSpy.count(), 2);
    QCOMPARE(playerStopSpy.count(), 0);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
    QCOMPARE(isMarkedPlaying(myPlayList, 0), false);
    QCOMPARE(isMarkedPlaying(myPlayList, 1), true);
    QCOMPARE(finishedPlayingTrackSpy.count(), 1);
    QCOMPARE(finishedPlayingTrackSpy.takeFirst().at(0).toUrl(), gaplessTrackUrl(1));
    QCOMPARE(startedPlayingTrackSpy.count(), 2);
    QCOMPARE(startedPlayingTrackSpy.takeLast().at(0).toUrl(), gaplessTrackUrl(2));

    // A track change that was not armed by Elisa must be ignored.
    myPlayer.gaplessTrackChanged(gaplessTrackUrl(3));
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));

    // Refreshing the data of the handed-off track must not reload it: the
    // player source is already the one the backend is playing.
    myPlayList.refreshTrack(1);
    QCOMPARE(playerSourceChangedSpy.count(), 1);

    // Reaching the end of the handed-off track falls back to the regular
    // EndOfMedia advance: exactly one further skip, no double advance.
    endGaplessPlayback(myPlayer);

    QCOMPARE(finishedPlayingTrackSpy.count(), 1);
    QCOMPARE(finishedPlayingTrackSpy.takeFirst().at(0).toUrl(), gaplessTrackUrl(2));
    QCOMPARE(skipNextTrackSpy.wait(300), true);
    QCOMPARE(skipNextTrackSpy.count(), 2);
}

void ManageAudioPlayerTest::gaplessHandoffWithoutNextTrackCase()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    GaplessPlayListModel myPlayList;

    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy prepareNextSourceSpy(&myPlayer, &ManageAudioPlayer::prepareNextSource);
    QSignalSpy setNextSourceSpy(&myPlayer, &ManageAudioPlayer::setNextSourceInAudioWrapper);

    setupGaplessFixture(myPlayer, myPlayList, 1);
    startGaplessPlayback(myPlayer);

    // No next track in the play list: nothing is armed and the EndOfMedia
    // path handles the end of the track.
    myPlayer.gaplessAboutToFinish();

    QCOMPARE(prepareNextSourceSpy.count(), 1);
    QCOMPARE(setNextSourceSpy.count(), 0);

    myPlayer.gaplessTrackChanged(gaplessTrackUrl(1));

    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    endGaplessPlayback(myPlayer);

    QCOMPARE(skipNextTrackSpy.wait(300), true);
    QCOMPARE(skipNextTrackSpy.count(), 1);
}

void ManageAudioPlayerTest::gaplessDisarmWhenNextTrackUrlChangedCase()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    GaplessPlayListModel myPlayList;

    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy setNextSourceSpy(&myPlayer, &ManageAudioPlayer::setNextSourceInAudioWrapper);

    setupGaplessFixture(myPlayer, myPlayList, 2);
    startGaplessPlayback(myPlayer);

    myPlayer.gaplessAboutToFinish();
    QCOMPARE(setNextSourceSpy.count(), 1);

    // The URL of the armed next track changes mid-window: the arm must be
    // dropped instead of handing off to the removed URL.
    myPlayList.setData(myPlayList.index(1, 0), gaplessTrackUrl(3), ManageAudioPlayerTest::ResourceRole);
    QCOMPARE(playerSourceChangedSpy.count(), 1);
    QCOMPARE(setNextSourceSpy.count(), 2);
    QCOMPARE(setNextSourceSpy.at(1).at(0).toUrl(), QUrl());

    // A transition to the stale armed source is ignored: the disarm cleared
    // the pending source.
    myPlayer.gaplessTrackChanged(gaplessTrackUrl(2));
    QCOMPARE(skipNextTrackSpy.count(), 0);
    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(0, 0)));

    // The track ends via the regular EndOfMedia advance.
    endGaplessPlayback(myPlayer);

    QCOMPARE(skipNextTrackSpy.wait(300), true);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(playerSourceChangedSpy.at(1).at(0).toUrl(), gaplessTrackUrl(3));

    // Also verify wrap-around (Repeat::Playlist) where the next track is at row 0
    // rather than currentTrack.row() + 1.
    myPlayer.setPlayerStatus(QMediaPlayer::LoadedMedia);
    myPlayer.setPlayerPlaybackState(QMediaPlayer::PlayingState);
    myPlayer.gaplessNextTrackChanged(myPlayList.index(0, 0));
    myPlayer.setPendingNextSource(gaplessTrackUrl(1));
    QCOMPARE(setNextSourceSpy.count(), 3);
    QCOMPARE(setNextSourceSpy.at(2).at(0).toUrl(), gaplessTrackUrl(1));

    myPlayList.setData(myPlayList.index(0, 0), gaplessTrackUrl(4), ManageAudioPlayerTest::ResourceRole);
    QCOMPARE(setNextSourceSpy.count(), 4);
    QCOMPARE(setNextSourceSpy.at(3).at(0).toUrl(), QUrl());
}

void ManageAudioPlayerTest::gaplessDisarmWhenNextTrackRemovedCase()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    GaplessPlayListModel myPlayList;

    QSignalSpy playerSourceChangedSpy(&myPlayer, &ManageAudioPlayer::playerSourceChanged);
    QSignalSpy playerStopSpy(&myPlayer, &ManageAudioPlayer::playerStop);
    QSignalSpy skipNextTrackSpy(&myPlayer, &ManageAudioPlayer::skipNextTrack);
    QSignalSpy setNextSourceSpy(&myPlayer, &ManageAudioPlayer::setNextSourceInAudioWrapper);

    setupGaplessFixture(myPlayer, myPlayList, 3);
    startGaplessPlayback(myPlayer);

    myPlayer.gaplessAboutToFinish();
    QCOMPARE(setNextSourceSpy.count(), 1);

    // The user removes the track the backend was armed with, the following
    // track shifts up into its place. The proxy model notifies that the next
    // track changed (its persistent index became invalid).
    myPlayList.removeRow(1);
    myPlayer.gaplessNextTrackChanged();

    QCOMPARE(setNextSourceSpy.count(), 2);
    QCOMPARE(setNextSourceSpy.at(1).at(0).toUrl(), QUrl());

    // The track ends via the regular EndOfMedia advance onto the track that
    // shifted up.
    endGaplessPlayback(myPlayer);

    QCOMPARE(skipNextTrackSpy.wait(300), true);
    QCOMPARE(skipNextTrackSpy.count(), 1);
    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex(myPlayList.index(1, 0)));
    QCOMPARE(playerSourceChangedSpy.count(), 2);
    QCOMPARE(playerSourceChangedSpy.at(1).at(0).toUrl(), gaplessTrackUrl(3));

    // A data refresh of the newly current track must not reload it.
    myPlayList.refreshTrack(1);
    QCOMPARE(playerSourceChangedSpy.count(), 2);
}

void ManageAudioPlayerTest::gaplessDisarmOnClearPlayListCase()
{
    Elisa::ElisaConfiguration::self()->setDefaults();
    ManageAudioPlayer myPlayer;
    GaplessPlayListModel myPlayList;

    QSignalSpy setNextSourceSpy(&myPlayer, &ManageAudioPlayer::setNextSourceInAudioWrapper);

    setupGaplessFixture(myPlayer, myPlayList, 2);
    startGaplessPlayback(myPlayer);

    myPlayer.gaplessAboutToFinish();
    QCOMPARE(setNextSourceSpy.count(), 1);
    QCOMPARE(setNextSourceSpy.at(0).at(0).toUrl(), gaplessTrackUrl(2));

    // The user clears the play list while the backend is armed with the next
    // source: playListFinished() must drop the arm before the backend can
    // hand off to the removed track.
    myPlayList.clear();
    myPlayer.playListFinished();

    QCOMPARE(setNextSourceSpy.count(), 2);
    QCOMPARE(setNextSourceSpy.at(1).at(0).toUrl(), QUrl());

    // Invalidate the current track
    myPlayer.setCurrentTrack(QPersistentModelIndex());
    QCOMPARE(myPlayer.currentTrack(), QPersistentModelIndex());
}


QTEST_GUILESS_MAIN(ManageAudioPlayerTest)


#include "moc_manageaudioplayertest.cpp"
