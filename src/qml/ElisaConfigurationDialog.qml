/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.11
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.4 as QQC2
import QtQuick.Dialogs 1.3 as Dialogs

import org.kde.kirigami 2.14 as Kirigami

import org.kde.elisa 1.0

Window {
    id: dialog

    title: i18n("Configure")

    visible: true
    modality: Qt.ApplicationModal

    minimumWidth: Kirigami.Units.gridUnit * 36
    minimumHeight: Kirigami.Units.gridUnit * 32

    // Close when pressing Esc key
    Shortcut {
        sequence: StandardKey.Cancel
        onActivated: close()
    }

    Kirigami.ScrollablePage {
        anchors.fill: parent

        SettingsForm {}

        footer: QQC2.DialogButtonBox {

            QQC2.Button {
                text: i18n("OK")
                icon.name: 'dialog-ok-apply'
                QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.AcceptRole
                Accessible.onPressAction: onClicked
            }
            onAccepted: {
                ElisaConfigurationDialog.save()
                close()
            }

            QQC2.Button {
                text: i18n("Apply")
                icon.name: 'dialog-ok-apply'
                QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.ApplyRole
                Accessible.onPressAction: onClicked

                enabled: ElisaConfigurationDialog.isDirty
            }
            onApplied: ElisaConfigurationDialog.save()

            QQC2.Button {
                text: i18n("Cancel")
                icon.name: 'dialog-cancel'
                QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.RejectRole
                Accessible.onPressAction: onClicked
            }
            onRejected: {
                ElisaConfigurationDialog.cancel()
                close()
            }
        }
    }

    Dialogs.MessageDialog {
        id: dirtyClosingDialog

        standardButtons: Dialogs.StandardButton.Save | Dialogs.StandardButton.Discard | Dialogs.StandardButton.Cancel

        title: i18n("Warning")

        icon: Dialogs.StandardIcon.Warning
        text: i18n('You have unsaved changes. Do you want to apply the changes or discard them?')

        onDiscard: {
            ElisaConfigurationDialog.cancel()
            dialog.close()
        }

        onAccepted: {
            ElisaConfigurationDialog.save()
            dialog.close()
        }
    }

    onClosing: {
        if (ElisaConfigurationDialog.isDirty) {
            close.accepted = false
            dirtyClosingDialog.open()
        }
    }
}
