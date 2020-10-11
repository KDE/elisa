/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.3
import org.kde.elisa 1.0
import org.kde.kirigami 2.5 as Kirigami

Menu {
    id: applicationMenu
    title: i18nc("open application menu", "Application Menu")

    property var helpAction: ElisaApplication.action("help_contents")
    property var quitApplication: ElisaApplication.action("file_quit")
    property var reportBugAction: ElisaApplication.action("help_report_bug")
    property var aboutAppAction: ElisaApplication.action("help_about_app")
    property var configureShortcutsAction: ElisaApplication.action("options_configure_keybinding")
    property var configureAction: ElisaApplication.action("options_configure")

    Action {
        text: i18nc("Refresh Music Collection application menu entry", "Refresh Music Collection")
        icon.name: "view-refresh"
        onTriggered: {
            applicationMenu.close()
            ElisaApplication.musicManager.resetMusicData()
        }
    }

    Menu {
        title: i18n("Color Scheme")
        Repeater {
            model: ElisaApplication.colorSchemesModel
            delegate: Kirigami.BasicListItem {
                icon: model.decoration
                text: model.display
                highlighted: model.display === ElisaConfigurationDialog.colorScheme
                onClicked: {
                    ElisaApplication.activateColorScheme(model.display)
                    ElisaConfigurationDialog.setColorScheme(model.display)
                    ElisaConfigurationDialog.save()
                    applicationMenu.close()
                }
            }
        }
    }

    MenuSeparator {
    }

    Action {
        text: configureAction.text
        shortcut: configureAction.shortcut
        icon.name: ElisaApplication.iconName(configureAction.icon)
        onTriggered: {
            applicationMenu.close()
            configureAction.trigger()
        }
    }

    Action {
        text: configureShortcutsAction.text
        shortcut: configureShortcutsAction.shortcut
        icon.name: ElisaApplication.iconName(configureShortcutsAction.icon)
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
        icon.name: ElisaApplication.iconName(reportBugAction.icon)
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
        icon.name: ElisaApplication.iconName(helpAction.icon)
        onTriggered: {
            applicationMenu.close()
            helpAction.trigger()
        }
    }

    Action {
        text: aboutAppAction.text
        shortcut: aboutAppAction.shortcut
        icon.name: ElisaApplication.iconName(aboutAppAction.icon)
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
        icon.name: ElisaApplication.iconName(quitApplication.icon)
        onTriggered: quitApplication.trigger()
    }
}
