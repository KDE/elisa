/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Nate Graham <nate@kde.org>
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.11
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.4 as QQC2
import QtQuick.Dialogs 1.3 as Dialogs

import org.kde.kirigami 2.14 as Kirigami

import org.kde.elisa 1.0

import ".."

// combine ApplicationMenu.qml and ElisaConfigurationDialog.qml into one page, and in a mobile friendly format

Kirigami.ScrollablePage {
    id: settingsPage

    title: i18n("Settings")

    actions.main: Kirigami.Action {
        iconName: "dialog-ok-apply"
        text: i18n("Save")
        onTriggered: {
            ElisaConfigurationDialog.save()
            mainWindow.pageStack.layers.pop();
        }
    }
    actions.left: Kirigami.Action {
        iconName: "dialog-cancel"
        text: i18n("Close")
        onTriggered: {
            ElisaConfigurationDialog.cancel()
            mainWindow.pageStack.layers.pop()
        }
    }

    SettingsForm {}
}
