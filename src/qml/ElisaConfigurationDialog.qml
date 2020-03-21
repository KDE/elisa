/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import QtQml.Models 2.3
import org.kde.kirigami 2.5 as Kirigami

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

                    enabled: config.isDirty
                }

                Button {
                    text: i18n("Cancel")
                    icon.name: 'dialog-cancel'
                    DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                }

                onAccepted: {
                    config.save()
                    close()
                }

                onApplied: config.save()

                onRejected: close()
            }
        }
    }
}
