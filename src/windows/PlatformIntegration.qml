/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.elisa

Item {
    id: rootItem

    property var playListModel
    property var audioPlayerManager
    property var player
    property var headerBarManager
    property var manageMediaPlayerControl
    property bool showProgressOnTaskBar
    property var elisaMainWindow
    property bool showSystemTrayIcon
    property bool forceCloseWindow: false

    signal raisePlayer()

    TaskBarManager {
        playbackState: rootItem.player.playbackState

        showProgress: rootItem.showProgressOnTaskBar
        progressMaximum: rootItem.player.duration
        progressValue: rootItem.player.position

        canSkipBackward: rootItem.manageMediaPlayerControl.skipBackwardControlEnabled
        canSkipForward: rootItem.manageMediaPlayerControl.skipForwardControlEnabled
        canTogglePlayback: true

        onSkipBackward: rootItem.playListModel.skipPreviousTrack(rootItem.player.position)
        onSkipForward: rootItem.playListModel.skipNextTrack(ElisaUtils.Manual)
        onTogglePlayback: rootItem.audioPlayerManager.playPause()
    }
}
