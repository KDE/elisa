/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window
import QtQuick.Controls as QQC2

import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.elisa

Window {
    id: dialog

    title: KI18n.i18nc("@title:window", "Configure")

    visible: true
    modality: Qt.ApplicationModal
    flags: Qt.Dialog

    minimumWidth: Kirigami.Units.gridUnit * 36
    minimumHeight: Kirigami.Units.gridUnit * 32

    // Close when pressing Esc key
    Shortcut {
        sequence: StandardKey.Cancel
        onActivated: dialog.close()
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
