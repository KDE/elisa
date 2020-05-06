/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.3
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

    Action {
        text: i18nc("Refresh Music Collection application menu entry", "Refresh Music Collection")
        icon.name: "view-refresh"
        onTriggered: {
            applicationMenu.close()
            elisa.musicManager.resetMusicData()
        }
    }

    MenuSeparator {
    }

    Action {
        text: configureAction.text
        shortcut: configureAction.shortcut
        icon.name: elisa.iconName(configureAction.icon)
        onTriggered: {
            applicationMenu.close()
            configureAction.trigger()
        }
    }

    Action {
        text: configureShortcutsAction.text
        shortcut: configureShortcutsAction.shortcut
        icon.name: elisa.iconName(configureShortcutsAction.icon)
        onTriggered: {
            applicationMenu.close()
            configureShortcutsAction.trigger()
        }
    }

    MenuSeparator {
        visible: reportBugAction.text !== ""
    }

    Action {
        text: reportBugAction.text
        shortcut: reportBugAction.shortcut
        icon.name: elisa.iconName(reportBugAction.icon)
        onTriggered: {
            applicationMenu.close()
            reportBugAction.trigger()
        }
    }

    MenuSeparator {
        visible: helpAction.text !== ""
    }

    Action {
        text: helpAction.text
        shortcut: helpAction.shortcut
        icon.name: elisa.iconName(helpAction.icon)
        onTriggered: {
            applicationMenu.close()
            helpAction.trigger()
        }
    }

    Action {
        text: aboutAppAction.text
        shortcut: aboutAppAction.shortcut
        icon.name: elisa.iconName(aboutAppAction.icon)
        onTriggered: {
            applicationMenu.close()
            aboutAppAction.trigger()
        }
    }

    MenuSeparator {
        visible: quitApplication.text !== ""
    }

    Action {
        text: quitApplication.text
        shortcut: quitApplication.shortcut
        icon.name: elisa.iconName(quitApplication.icon)
        onTriggered: quitApplication.trigger()
    }
}
