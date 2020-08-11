/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.0
import org.kde.elisa 1.0
import QtWinExtras 1.0

Item {
    id: rootItem

    property var playListModel
    property var audioPlayerManager
    property var player
    property var headerBarManager
    property var manageMediaPlayerControl
    property alias showProgressOnTaskBar: progressBar.active
    property var elisaMainWindow
    property bool showSystemTrayIcon
    property bool forceCloseWindow: false

    signal raisePlayer()

    Theme {
        id: elisaTheme
    }

    Loader {
        id: progressBar

        sourceComponent: taskBarComponent
    }

    Component {
        id:taskBarComponent

        TaskbarButton {
            progress.minimum: 0
            progress.maximum: player.duration
            progress.value: player.position
            progress.visible: manageMediaPlayerControl.musicPlaying

            overlay.iconSource: (manageMediaPlayerControl.musicPlaying ?
                                     Qt.resolvedUrl(elisaTheme.playingIndicatorIcon) : Qt.resolvedUrl(elisaTheme.pausedIndicatorIcon))
        }
    }

    ThumbnailToolBar {
        iconicThumbnailSource: (headerBarManager.image.toString() !== '' ? headerBarManager.image : Qt.resolvedUrl(elisaTheme.albumCover))

        ThumbnailToolButton {
            iconSource: Qt.resolvedUrl(LayoutMirroring.enabled ? elisaTheme.skipForwardIcon : elisaTheme.skipBackwardIcon)
            onClicked: playListModel.skipPreviousTrack(player.position)
            enabled: manageMediaPlayerControl.skipBackwardControlEnabled
        }

        ThumbnailToolButton {
            iconSource: (manageMediaPlayerControl.musicPlaying ? Qt.resolvedUrl(elisaTheme.pauseIcon) : Qt.resolvedUrl(elisaTheme.playIcon))
            onClicked: audioPlayerManager.playPause()
            enabled: manageMediaPlayerControl.playControlEnabled
        }

        ThumbnailToolButton {
            iconSource: Qt.resolvedUrl(LayoutMirroring.enabled ? elisaTheme.skipBackwardIcon : elisaTheme.skipForwardIcon)
            onClicked: playListModel.skipNextTrack()
            enabled: manageMediaPlayerControl.skipForwardControlEnabled
        }
    }
}
