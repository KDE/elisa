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

    minimumWidth: 600
    minimumHeight: 400

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

    RowLayout {
        spacing: 0

        anchors.fill: parent
        anchors.margins: 0

        ListView {
            id: pagesList

            Layout.fillHeight: true
            Layout.preferredWidth: 150

            boundsBehavior: Flickable.StopAtBounds

            delegate: ItemDelegate {
                text: model.pageName
                icon.name: model.iconName

                width: pagesList.width

                onClicked: pagesList.currentIndex = index
            }

            model: ListModel {
                id: settingsPagesModel
            }

            ScrollBar.vertical: ScrollBar {
                id: scrollBar
            }

            highlight: highlightBar
        }

        Kirigami.Separator {
            Layout.fillHeight: true
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true

            color: myPalette.window

            ColumnLayout {
                spacing: 0

                anchors.fill: parent

                StackLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Layout.leftMargin: 10
                    Layout.rightMargin: 10
                    Layout.topMargin: 10
                    Layout.bottomMargin: 10

                    currentIndex: pagesList.currentIndex

                    Item {
                    }

                    FileScanningConfiguration {
                    }
                }

                DialogButtonBox {
                    Layout.fillWidth: true

                    standardButtons: DialogButtonBox.Ok | DialogButtonBox.Apply | DialogButtonBox.Cancel

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

    Component.onCompleted: {
        settingsPagesModel.insert(0, { "pageName": i18nc("Settings page about main preference", "General"), "iconName": 'settings-configure' })
        settingsPagesModel.insert(1, { "pageName": i18nc("Settings page about music search folders configuration", "Music Search Folders"), "iconName": 'folder-music' })
    }
}
