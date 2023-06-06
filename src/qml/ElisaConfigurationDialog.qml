/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.11
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.4 as QQC2
import @QTQUICK_DIALOGS_IMPORT@ as Dialogs

import org.kde.kirigami 2.14 as Kirigami

import org.kde.elisa 1.0

Window {
    id: dialog

    title: i18nc("@title:window", "Configure")

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
                text: i18nc("@action:button", "OK")
                icon.name: 'dialog-ok-apply'
                QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.AcceptRole
                Accessible.onPressAction: onClicked
            }
            onAccepted: {
                ElisaConfigurationDialog.save()
                close()
            }

            QQC2.Button {
                text: i18nc("@action:button", "Apply")
                icon.name: 'dialog-ok-apply'
                QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.ApplyRole
                Accessible.onPressAction: onClicked

                enabled: ElisaConfigurationDialog.isDirty
            }
            onApplied: ElisaConfigurationDialog.save()

            QQC2.Button {
                text: i18nc("@action:button", "Cancel")
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

    // FIXME: the Dialogs.MessageDialog API changed completely and we can't do
    // simple line-by-line text replacement. Re-evaluate whether it even makes
    // sense to use this API at all as it's now very limited and can't even show
    // an icon!

    // Dialogs.MessageDialog {
    //     id: dirtyClosingDialog
    // 
    //     // standardButtons: Dialogs.StandardButton.Save | Dialogs.StandardButton.Discard | Dialogs.StandardButton.Cancel
    // 
    //     title: i18nc("@title:window", "Warning")
    // 
    //     // icon: Dialogs.StandardIcon.Warning
    //     text: i18nc("@info", 'You have unsaved changes. Do you want to apply the changes or discard them?')
    // 
    //     // onDiscard: {
    //     //     ElisaConfigurationDialog.cancel()
    //     //     dialog.close()
    //     // }
    //     // 
    //     // onAccepted: {
    //     //     ElisaConfigurationDialog.save()
    //     //     dialog.close()
    //     // }
    // }
    // 
    // onClosing: {
    //     if (ElisaConfigurationDialog.isDirty) {
    //         close.accepted = false
    //         dirtyClosingDialog.open()
    //     }
    // }
}
