/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

    signal raisePlayer()

    Connections {
        target: elisaMainWindow

        onClosing: {
            if (systemTrayIcon.available && showSystemTrayIcon) {
                close.accepted = false
                elisaMainWindow.hide()
            }
        }
    }

    NativeMenu.MenuBar {
        NativeApplicationMenu {
            id: globalMenu
        }
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

        menu: globalMenu

        onActivated: {
            if (reason === NativeMenu.SystemTrayIcon.Trigger && !elisaMainWindow.visible) {
                elisaMainWindow.visible = true
            } else if (reason === NativeMenu.SystemTrayIcon.Trigger && elisaMainWindow.visible) {
                raisePlayer()
            }
        }
    }
}
