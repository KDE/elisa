/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.elisa

Item {
    id: rootItem

    property alias audioPlayerManager: androidPlayerInterface.audioPlayerManager
    property var playListModel
    property var player
    property var headerBarManager
    property var manageMediaPlayerControl
    property var showProgressOnTaskBar
    property bool showSystemTrayIcon
    property var elisaMainWindow
    property bool forceCloseWindow: false

    signal raisePlayer()

    Connections {
        target: elisaMainWindow

        function onClosing() {
            close.accepted = false
            elisaMainWindow.hide()
        }
    }

    Connections {
        target: ElisaApplication

        function onCommitDataRequest() {
            forceCloseWindow = true
        }
    }

    AndroidPlayer {
        id: androidPlayerInterface

        Component.onCompleted: {
            initialize()
        }
    }
}
