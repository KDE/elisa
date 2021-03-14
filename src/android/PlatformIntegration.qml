/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import Qt.labs.platform 1.1 as NativeMenu
import org.kde.elisa 1.0

Item {
    id: rootItem

    property var playListModel
    property var audioPlayerManager
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
}
