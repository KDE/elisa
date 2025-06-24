/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick 2.11
import QtQuick.Window 2.12
import QtQuick.Controls 2.4 as QQC2

import org.kde.kirigami 2.14 as Kirigami

import org.kde.elisa

Window {
    id: dialog

    title: i18nc("@title:window", "Configure")

    visible: true
    modality: Qt.ApplicationModal
    flags: Qt.Dialog

    minimumWidth: Kirigami.Units.gridUnit * 36
    minimumHeight: Kirigami.Units.gridUnit * 32

    // Close when pressing Esc key
    Shortcut {
        sequence: StandardKey.Cancel
        onActivated: close()
    }

    Kirigami.ScrollablePage {
        anchors.fill: parent

        SettingsForm {
            id: settingsForm
            onCloseForm: dialog.close()
        }

        footer: QQC2.DialogButtonBox {
            id: buttonBox
            standardButtons: QQC2.DialogButtonBox.Ok | QQC2.DialogButtonBox.Apply | QQC2.DialogButtonBox.Cancel

            onAccepted: settingsForm.saveAndCloseForm()
            onApplied: settingsForm.applyChanges()
            onRejected: settingsForm.discardAndCloseForm()

            Component.onCompleted: {
                const applyButton = standardButton(QQC2.DialogButtonBox.Apply);
                if (applyButton) {
                    applyButton.enabled = Qt.binding(() => ElisaConfigurationDialog.isDirty);
                }
            }
        }
    }

    onClosing: close => {
        if (ElisaConfigurationDialog.isDirty) {
            close.accepted = false
            settingsForm.dirtyClosingDialog.open()
        }
    }
}
