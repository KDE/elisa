/*
 * Copyright 2016-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2018 Alexander Stippich <a.stippich@gmx.net>
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
//explore menu from Qt 5.10 once we can require it, but it is item-based
import Qt.labs.platform 1.0
import org.kde.elisa 1.0

Menu {
    id: applicationMenu
    title: i18nc("open application menu", "Application Menu")

    property var helpAction: elisa.action("help_contents")
    property var quitApplication: elisa.action("file_quit")
    property var reportBugAction: elisa.action("help_report_bug")
    property var aboutAppAction: elisa.action("help_about_app")
    property var configureShortcutsAction: elisa.action("options_configure_keybinding")
    property var configureAction: elisa.action("options_configure")
    property var togglePlaylistAction: elisa.action("toggle_playlist")

    MenuItem {
        text: i18nc("Refresh Music Collection application menu entry", "Refresh Music Collection")
        iconName: "collection-rescan-amarok"
        onTriggered: elisa.musicManager.resetMusicData()
    }

    MenuSeparator {
    }

    MenuItem {
        text: configureAction.text
        shortcut: configureAction.shortcut
        iconName: elisa.iconName(configureAction.icon)
        onTriggered: configureAction.trigger()
        visible: configureAction.text !== ""
    }

    MenuItem {
        text: configureShortcutsAction.text
        shortcut: configureShortcutsAction.shortcut
        iconName: elisa.iconName(configureShortcutsAction.icon)
        onTriggered: configureShortcutsAction.trigger()
        visible: configureShortcutsAction.text !== ""
    }

    MenuItem {
        shortcut: togglePlaylistAction.shortcut
        text: i18n("Show Playlist")
        iconName: "view-media-playlist"
        checkable: true
        checked: contentView.showPlaylist
        onTriggered: contentView.showPlaylist = !contentView.showPlaylist
        enabled: contentView.currentViewIndex != 0
    }

    MenuSeparator {
        visible: reportBugAction.text !== ""
    }

    MenuItem {
        text: reportBugAction.text
        shortcut: reportBugAction.shortcut
        iconName: elisa.iconName(reportBugAction.icon)
        onTriggered: reportBugAction.trigger()
        visible: reportBugAction.text !== ""
    }

    MenuSeparator {
        visible: helpAction.text !== ""
    }

    MenuItem {
        text: helpAction.text
        shortcut: helpAction.shortcut
        iconName: elisa.iconName(helpAction.icon)
        onTriggered: helpAction.trigger()
        visible: helpAction.text !== ""
    }

    MenuItem {
        text: aboutAppAction.text
        shortcut: aboutAppAction.shortcut
        iconName: elisa.iconName(aboutAppAction.icon)
        onTriggered: aboutAppAction.trigger()
        visible: aboutAppAction.text !== ""
    }

    MenuSeparator {
        visible: quitApplication.text !== ""
    }

    MenuItem {
        text: quitApplication.text
        shortcut: quitApplication.shortcut
        iconName: elisa.iconName(quitApplication.icon)
        onTriggered: quitApplication.trigger()
        visible: quitApplication.text !== ""
    }
}
