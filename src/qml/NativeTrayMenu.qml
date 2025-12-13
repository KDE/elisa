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
        action: Kirigami.Action {
            fromQAction: ElisaApplication.action("options_configure")
        }
    }

    NativeMenuItemFromAction {
        action: Kirigami.Action {
            fromQAction: ElisaApplication.action("options_configure_keybinding")
        }
    }

    NativeMenuItemFromAction {
        action: Kirigami.Action {
            fromQAction: ElisaApplication.action("help_report_bug")
        }
    }

    NativeMenuItemFromAction {
        action: Kirigami.Action {
            fromQAction: ElisaApplication.action("help_about_app")
        }
    }

    NativeMenuItemFromAction {
        action: Kirigami.Action {
            fromQAction: ElisaApplication.action("help_about_kde")
        }
    }

    MenuSeparator {
    }

    NativeMenuItemFromAction {
        action: Kirigami.Action {
            fromQAction: ElisaApplication.action("file_quit")
        }
    }
}
