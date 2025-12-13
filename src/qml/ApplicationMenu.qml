/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import org.kde.elisa
import org.kde.ki18n
import org.kde.kirigami as Kirigami

Menu {
    id: applicationMenu
    title: KI18n.i18nc("@title:menu", "Application Menu")

    Action {
        text: mainWindow.visibility == Window.FullScreen ? KI18n.i18nc("@action:inmenu", "Exit Full Screen") : KI18n.i18nc("@action:inmenu", "Enter Full Screen")
        icon.name: mainWindow.visibility == Window.FullScreen ? "view-restore" : "view-fullscreen"
        onTriggered: mainWindow.visibility == Window.FullScreen ? mainWindow.restorePreviousStateBeforeFullScreen() : mainWindow.goFullScreen()
    }

    MenuSeparator {}

    Action {
        text: KI18n.i18nc("@action:inmenu", "Scan for New Music")
        icon.name: "view-refresh"
        onTriggered: {
            applicationMenu.close()
            ElisaApplication.musicManager.scanCollection(MusicListenersManager.Soft)
        }
    }

    Action {
        text: KI18n.i18nc("@action:inmenu", "Reset Database and Re-Scan Everything")
        icon.name: "edit-clear-all"
        onTriggered: {
            applicationMenu.close()
            ElisaApplication.musicManager.scanCollection(MusicListenersManager.Hard)
        }
    }

    MenuSeparator {}

    Menu {
        title: KI18n.i18nc("@action:inmenu this has child menu items", "Color Scheme")
        Repeater {
            model: ElisaApplication.colorSchemesModel
            delegate: MenuItem {
                required property var model

                icon.source: "image://colorScheme/" + model.display
                icon.color: "transparent"
                text: model.display
                checkable: true
                checked: {
                    if (model.index === 0) {
                        return ElisaApplication.activeColorSchemeName === "";
                    }

                    return model.display === ElisaApplication.activeColorSchemeName;
                }
                autoExclusive: true
                onTriggered: {
                    ElisaApplication.activeColorSchemeName = model.display
                    applicationMenu.close()
                }
            }
        }
    }

    Kirigami.Action {
        fromQAction: ElisaApplication.action("options_configure")
    }

    Kirigami.Action {
        fromQAction: ElisaApplication.action("options_configure_keybinding")
    }

    MenuSeparator {}

    Kirigami.Action {
        fromQAction: ElisaApplication.action("help_about_app")
    }

    Kirigami.Action {
        fromQAction: ElisaApplication.action("help_about_kde")
    }

    Kirigami.Action {
        fromQAction: ElisaApplication.action("help_report_bug")
    }

    MenuSeparator {
        visible: ElisaApplication.showSystemTrayIcon
    }

    MenuItem {
        action: Kirigami.Action {
            fromQAction: ElisaApplication.action("file_quit")
        }
        visible: ElisaApplication.showSystemTrayIcon
    }
}
