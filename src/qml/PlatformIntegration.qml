/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import Qt.labs.platform as NativeMenu
import org.kde.elisa

Item {
    id: root

    property alias playListModel: mpris2Interface.playListModel
    property alias audioPlayerManager: mpris2Interface.audioPlayerManager
    property alias player: mpris2Interface.audioPlayer
    property alias headerBarManager: mpris2Interface.headerBarManager
    property alias manageMediaPlayerControl: mpris2Interface.manageMediaPlayerControl
    property alias showProgressOnTaskBar: mpris2Interface.showProgressOnTaskBar
    property bool showSystemTrayIcon
    property var elisaMainWindow
    property bool forceCloseWindow: false

    signal raisePlayer()

    Connections {
        target: root.elisaMainWindow

        function onClosing(close) {
            if (systemTrayIcon.available && root.showSystemTrayIcon && !root.forceCloseWindow) {
                close.accepted = false
                root.elisaMainWindow.hide()
            }
        }
    }

    Connections {
        target: ElisaApplication

        function onCommitDataRequest() {
            root.forceCloseWindow = true;
        }

        function onRaisePlayer() {
            root.raisePlayer();
        }
    }

    NativeGlobalMenu {
        id: globalMenu
    }

    Mpris2 {
        id: mpris2Interface

        playerName: 'elisa'

        onRaisePlayer: root.raisePlayer()
    }

    NativeMenu.SystemTrayIcon {
        id: systemTrayIcon

        icon.name: "elisa-symbolic"
        tooltip: root.elisaMainWindow.title
        visible: available && root.showSystemTrayIcon && !root.elisaMainWindow.visible

        menu: NativeTrayMenu {
            id: exportedMenu
        }

        onActivated: reason => {
            if (reason === NativeMenu.SystemTrayIcon.Trigger) {
                root.raisePlayer();
            }
        }

        Component.onCompleted: {
            exportedMenu.visible = false
            exportedMenu.enabled = false
        }
    }
}
