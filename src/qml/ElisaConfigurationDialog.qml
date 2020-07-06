/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import QtQml.Models 2.3
import org.kde.kirigami 2.5 as Kirigami

import org.kde.elisa 1.0

Window {
    id: dialog

    title: i18n("Configure")

    visible: true
    modality: Qt.ApplicationModal

    minimumWidth: 750
    minimumHeight: 500

    Shortcut {
        sequence: StandardKey.Cancel

        onActivated: close()
    }

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Component {
        id: highlightBar

        Rectangle {
            width: 200; height: 50
            color: myPalette.highlight
        }
    }

    Rectangle {
        anchors.fill: parent

        color: myPalette.window

        ColumnLayout {
            spacing: 0

            anchors.fill: parent

            Kirigami.Heading {
                text: i18n("General")

                Layout.leftMargin: 5
                Layout.rightMargin: 5
            }

            GeneralConfiguration {
                Layout.fillWidth: true

                Layout.topMargin: 10
                Layout.leftMargin: 20
                Layout.rightMargin: 10
            }

            Kirigami.Heading {
                text: i18n("Music Search Configuration")

                Layout.topMargin: 15
                Layout.leftMargin: 5
                Layout.rightMargin: 5
            }

            FileScanningConfiguration {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Layout.topMargin: 10
                Layout.leftMargin: 20
                Layout.rightMargin: 10
            }

            DialogButtonBox {
                Layout.fillWidth: true

                Button {
                    text: i18n("OK")
                    icon.name: 'dialog-ok-apply'
                    DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                }

                Button {
                    text: i18n("Apply")
                    icon.name: 'dialog-ok-apply'
                    DialogButtonBox.buttonRole: DialogButtonBox.ApplyRole

                    enabled: ElisaConfigurationDialog.isDirty
                }

                Button {
                    text: i18n("Cancel")
                    icon.name: 'dialog-cancel'
                    DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                }

                onAccepted: {
                    ElisaConfigurationDialog.save()
                    close()
                }

                onApplied: ElisaConfigurationDialog.save()

                onRejected: close()
            }
        }
    }
}
