/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import org.kde.elisa 1.0

FocusScope {
    property int position: ElisaApplication.audioControl.playerPosition
    property int duration: ElisaApplication.audioControl.audioDuration
    property bool muted
    property bool isPlaying: ElisaApplication.playerControl.musicPlaying
    property bool seekable: ElisaApplication.audioPlayer.seekable
    property bool playEnabled: ElisaApplication.playerControl.playControlEnabled
    property bool skipForwardEnabled: ElisaApplication.playerControl.skipForwardControlEnabled
    property bool skipBackwardEnabled: ElisaApplication.playerControl.skipBackwardControlEnabled

    property bool shuffle: ElisaApplication.mediaPlayListProxyModel.shufflePlayList
    property int repeat: ElisaApplication.mediaPlayListProxyModel.repeatMode

    signal play()
    signal pause()
    signal playPrevious()
    signal playNext()
    signal seek(int position)

    onSeek: ElisaApplication.audioControl.playerSeek(position)
    onPlay: ElisaApplication.audioControl.playPause()
    onPause: ElisaApplication.audioControl.playPause()
    onPlayPrevious: ElisaApplication.mediaPlayListProxyModel.skipPreviousTrack(ElisaApplication.audioPlayer.position)
    onPlayNext: ElisaApplication.mediaPlayListProxyModel.skipNextTrack()
}
