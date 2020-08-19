/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import Qt.labs.platform 1.1 as NativeMenu
import org.kde.elisa 1.0

Item {
    id: rootItem

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
        target: elisaMainWindow

        function onClosing() {
            if (systemTrayIcon.available && showSystemTrayIcon && !forceCloseWindow) {
                close.accepted = false
                elisaMainWindow.hide()
            }
        }
    }

    Connections {
        target: ElisaApplication

        function onCommitDataRequest() {
            forceCloseWindow = true
        }
    }

    NativeGlobalMenu {
        id: globalMenu
    }

    Mpris2 {
        id: mpris2Interface

        playerName: 'elisa'

        onRaisePlayer:
        {
            rootItem.raisePlayer()
        }
    }

    NativeMenu.SystemTrayIcon {
        id: systemTrayIcon

        icon.name: 'elisa'
        tooltip: mainWindow.title
        visible: available && showSystemTrayIcon && !mainWindow.visible

        menu: NativeTrayMenu {
            id: exportedMenu
        }

        onActivated: {
            if (reason === NativeMenu.SystemTrayIcon.Trigger && !elisaMainWindow.visible) {
                elisaMainWindow.visible = true
            } else if (reason === NativeMenu.SystemTrayIcon.Trigger && elisaMainWindow.visible) {
                raisePlayer()
            }
        }

        Component.onCompleted: {
            exportedMenu.visible = false
            exportedMenu.enabled = false
        }
    }
}
