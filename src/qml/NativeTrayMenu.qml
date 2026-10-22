/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import Qt.labs.platform
import org.kde.elisa
import org.kde.ki18n
import org.kde.kirigami as Kirigami

Menu {
    id: applicationMenu
    title: KI18n.i18nc("@title:menu", "Application Menu")

    MenuItem  {
        text: KI18n.i18nc("@action:inmenu", "Scan for New Music")
        icon.name: "view-refresh"
        onTriggered: ElisaApplication.musicManager.scanCollection(MusicListenersManager.Soft)
    }

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Reset Database and Re-Scan Everything")
        icon.name: "edit-clear-all"
        onTriggered: ElisaApplication.musicManager.scanCollection(MusicListenersManager.Hard)
    }

    MenuSeparator {
    }

   NativeMenuItemFromAction {
        actionName: "options_configure"
        icon.name: "configure"
    }

    NativeMenuItemFromAction {
        actionName: "options_configure_keybinding"
        icon.name: "configure-shortcuts"
    }

    NativeMenuItemFromAction {
        actionName: "help_report_bug"
        icon.name: "tools-report-bug"
    }

    NativeMenuItemFromAction {
        actionName: "help_about_app"
        icon.name: "help-about"
    }

    NativeMenuItemFromAction {
        actionName: "help_about_kde"
        icon.name: "kde"
    }

    MenuSeparator {
    }

    NativeMenuItemFromAction {
        actionName: "file_quit"
        icon.name: "application-exit"
    } 
}
