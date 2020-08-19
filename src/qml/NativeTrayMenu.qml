/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import Qt.labs.platform 1.1
import org.kde.elisa 1.0

Menu {
    id: applicationMenu
    title: i18nc("open application menu", "Application Menu")

    property var helpAction: ElisaApplication.action("help_contents")
    property var quitApplication: ElisaApplication.action("file_quit")
    property var reportBugAction: ElisaApplication.action("help_report_bug")
    property var aboutAppAction: ElisaApplication.action("help_about_app")
    property var configureShortcutsAction: ElisaApplication.action("options_configure_keybinding")
    property var configureAction: ElisaApplication.action("options_configure")
    property var togglePlaylistAction: ElisaApplication.action("toggle_playlist")

    MenuItem {
        text: i18nc("Refresh Music Collection application menu entry", "Refresh Music Collection")
        iconName: "view-refresh"
        onTriggered: ElisaApplication.musicManager.resetMusicData()
    }

    MenuSeparator {
    }

    MenuItem {
        text: configureAction.text
        shortcut: configureAction.shortcut
        iconName: ElisaApplication.iconName(configureAction.icon)
        onTriggered: configureAction.trigger()
        visible: configureAction.text !== ""
    }

    MenuItem {
        text: configureShortcutsAction.text
        shortcut: configureShortcutsAction.shortcut
        iconName: ElisaApplication.iconName(configureShortcutsAction.icon)
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
        iconName: ElisaApplication.iconName(reportBugAction.icon)
        onTriggered: reportBugAction.trigger()
        visible: reportBugAction.text !== ""
    }

    MenuSeparator {
        visible: helpAction.text !== ""
    }

    MenuItem {
        text: helpAction.text
        shortcut: helpAction.shortcut
        iconName: ElisaApplication.iconName(helpAction.icon)
        onTriggered: helpAction.trigger()
        visible: helpAction.text !== ""
    }

    MenuItem {
        text: aboutAppAction.text
        shortcut: aboutAppAction.shortcut
        iconName: ElisaApplication.iconName(aboutAppAction.icon)
        onTriggered: aboutAppAction.trigger()
        visible: aboutAppAction.text !== ""
    }

    MenuSeparator {
        visible: quitApplication.text !== ""
    }

    MenuItem {
        text: quitApplication.text
        shortcut: quitApplication.shortcut
        iconName: ElisaApplication.iconName(quitApplication.icon)
        onTriggered: quitApplication.trigger()
        visible: quitApplication.text !== ""
    }
}
