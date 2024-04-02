/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Nate Graham <nate@kde.org>
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.11
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

import org.kde.kirigami 2.14 as Kirigami

import org.kde.elisa

import ".."

// combine ApplicationMenu.qml and ElisaConfigurationDialog.qml into one page, and in a mobile friendly format

Kirigami.ScrollablePage {
    id: settingsPage

    title: i18nc("@title:window", "Settings")

    onBackRequested: event => {
        if (ElisaConfigurationDialog.isDirty) {
            settingsForm.dirtyClosingDialog.open()
            event.accepted = true
        }
    }

    actions: [
        Kirigami.Action {
            icon.name: "dialog-ok-apply"
            text: i18nc("@action:button", "Save")
            onTriggered: settingsForm.saveAndCloseForm()
            enabled: ElisaConfigurationDialog.isDirty
        },
        Kirigami.Action {
            icon.name: "dialog-cancel"
            text: i18nc("@action:button", "Cancel")
            onTriggered: settingsForm.discardAndCloseForm()
        }
    ]

    SettingsForm {
        id: settingsForm
        onCloseForm: mainWindow.pageStack.layers.pop()
    }
}
