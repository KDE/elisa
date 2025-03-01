/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import Qt.labs.platform 1.1
import org.kde.elisa

Menu {
    id: applicationMenu
    title: i18nc("@title:menu", "Application Menu")

    property var quitApplication: ElisaApplication.action("file_quit")
    property var reportBugAction: ElisaApplication.action("help_report_bug")
    property var aboutAppAction: ElisaApplication.action("help_about_app")
    property var aboutKdeAction: ElisaApplication.action("help_about_kde")
    property var configureShortcutsAction: ElisaApplication.action("options_configure_keybinding")
    property var configureAction: ElisaApplication.action("options_configure")
    property var togglePlaylistAction: ElisaApplication.action("toggle_playlist")

    MenuItem  {
        text: i18nc("@action:inmenu", "Scan for New Music")
        icon.name: "view-refresh"
        onTriggered: ElisaApplication.musicManager.scanCollection(MusicListenersManager.Soft)
    }

    MenuItem {
        text: i18nc("@action:inmenu", "Reset Database and Re-Scan Everything")
        icon.name: "edit-clear-all"
        onTriggered: ElisaApplication.musicManager.scanCollection(MusicListenersManager.Hard)
    }

    MenuSeparator {
    }

    MenuItem {
        text: configureAction.text
        shortcut: ElisaApplication.actionShortcut(configureAction)
        icon.name: ElisaApplication.iconName(configureAction.icon)
        onTriggered: configureAction.trigger()
        visible: configureAction.text !== ""
    }

    MenuItem {
        text: configureShortcutsAction.text
        shortcut: ElisaApplication.actionShortcut(configureShortcutsAction)
        icon.name: ElisaApplication.iconName(configureShortcutsAction.icon)
        onTriggered: configureShortcutsAction.trigger()
        visible: configureShortcutsAction.text !== ""
    }

    MenuSeparator {
        visible: reportBugAction.text !== ""
    }

    MenuItem {
        text: reportBugAction.text
        shortcut: ElisaApplication.actionShortcut(reportBugAction)
        icon.name: ElisaApplication.iconName(reportBugAction.icon)
        onTriggered: reportBugAction.trigger()
        visible: reportBugAction.text !== ""
    }

    MenuItem {
        text: aboutAppAction.text
        shortcut: ElisaApplication.actionShortcut(aboutAppAction)
        icon.name: ElisaApplication.iconName(aboutAppAction.icon)
        onTriggered: aboutAppAction.trigger()
        visible: aboutAppAction.text !== ""
    }

    MenuItem {
        text: aboutKdeAction.text
        shortcut: ElisaApplication.actionShortcut(aboutKdeAction)
        icon.name: ElisaApplication.iconName(aboutKdeAction.icon)
        onTriggered: aboutKdeAction.trigger()
        visible: aboutKdeAction.text !== ""
    }

    MenuSeparator {
        visible: quitApplication.text !== ""
    }

    MenuItem {
        text: quitApplication.text
        shortcut: ElisaApplication.actionShortcut(quitApplication)
        icon.name: ElisaApplication.iconName(quitApplication.icon)
        onTriggered: quitApplication.trigger()
        visible: quitApplication.text !== ""
    }
}
