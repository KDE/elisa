/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Nate Graham <nate@kde.org>
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.elisa

import ".."

// combine ApplicationMenu.qml and ElisaConfigurationDialog.qml into one page, and in a mobile friendly format

Kirigami.ScrollablePage {
    id: settingsPage

    title: KI18n.i18nc("@title:window", "Settings")

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.Titles

    onBackRequested: event => {
        if (ElisaConfigurationDialog.isDirty) {
            settingsForm.dirtyClosingDialog.open()
            event.accepted = true
        }
    }

    actions: [
        Kirigami.Action {
            icon.name: "dialog-ok-apply"
            text: KI18n.i18nc("@action:button", "Save")
            onTriggered: settingsForm.saveAndCloseForm()
            enabled: ElisaConfigurationDialog.isDirty
        },
        Kirigami.Action {
            icon.name: "dialog-cancel"
            text: KI18n.i18nc("@action:button", "Cancel")
            onTriggered: settingsForm.discardAndCloseForm()
        }
    ]

    SettingsForm {
        id: settingsForm
        onCloseForm: mainWindow.pageStack.layers.pop()
    }
}
