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

    MenuItem {
        text: mainWindow.showProgressOnTaskBar ? i18nc("Do not show progress on task bar menu entry in the elisa menu", "Do Not Show Progress on Task Bar Entries") :
                                                 i18nc("Show Progress on Task Bar Entries entry in the elisa menu", "Show Progress on Task Bar Entries")
        onTriggered: mainWindow.showProgressOnTaskBar = !mainWindow.showProgressOnTaskBar
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
