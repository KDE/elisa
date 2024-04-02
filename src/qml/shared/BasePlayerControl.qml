/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import org.kde.elisa

FocusScope {
    readonly property int position: ElisaApplication.audioControl.playerPosition
    readonly property int duration: ElisaApplication.audioControl.audioDuration
    readonly property bool isPlaying: ElisaApplication.playerControl.musicPlaying
    readonly property bool seekable: ElisaApplication.audioPlayer.seekable
    readonly property bool playEnabled: ElisaApplication.playerControl.playControlEnabled
    readonly property bool skipForwardEnabled: ElisaApplication.playerControl.skipForwardControlEnabled
    readonly property bool skipBackwardEnabled: ElisaApplication.playerControl.skipBackwardControlEnabled

    property bool muted

    signal play()
    signal pause()
    signal playPrevious()
    signal playNext()
    signal seek(int position)

    onSeek: position => ElisaApplication.audioControl.playerSeek(position)
    onPlay: ElisaApplication.audioControl.playPause()
    onPause: ElisaApplication.audioControl.playPause()
    onPlayPrevious: ElisaApplication.mediaPlayListProxyModel.skipPreviousTrack(ElisaApplication.audioPlayer.position)
    onPlayNext: ElisaApplication.mediaPlayListProxyModel.skipNextTrack(ElisaUtils.Manual)
}
