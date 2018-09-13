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

import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtQml.Models 2.3
import org.kde.kcm 1.0

Item {
    //implicitWidth and implicitHeight will be used as initial size
    //when loaded in kcmshell5
    implicitWidth: 400
    implicitHeight: 200

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    ConfigModule.buttons: ConfigModule.Help|ConfigModule.Apply

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

    Component {
        id: pathDelegate

        Item {
            id: delegateItem

            height: 3 * 30

            width: pathList.width

            Rectangle {
                anchors.fill: parent
                anchors.margins: 0.1 * 30

                color: myPalette.base

                MouseArea {
                    anchors.fill: parent

                    hoverEnabled: true

                    onEntered: pathList.currentIndex = delegateItem.DelegateModel.itemsIndex

                    Label {
                        text: modelData

                        anchors.centerIn: parent
                    }

                    ToolButton {
                        iconName: 'list-remove'

                        anchors.top: parent.top
                        anchors.right: parent.right

                        onClicked:
                        {
                            var oldPaths = kcm.rootPath
                            oldPaths.splice(delegateItem.DelegateModel.itemsIndex, 1)
                            kcm.rootPath = oldPaths
                        }
                    }
                }
            }
        }
    }

    RowLayout {
        spacing: 0

        anchors.fill: parent

        ScrollView {
            flickableItem.boundsBehavior: Flickable.StopAtBounds

            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id:pathList

                anchors.fill: parent

                model: DelegateModel {
                    model: kcm.rootPath

                    delegate: pathDelegate
                }

                highlight: highlightBar
            }
        }

        ColumnLayout {
            Layout.fillHeight: true
            Layout.leftMargin: !LayoutMirroring.enabled ? (0.3 * 30) : 0
            Layout.rightMargin: LayoutMirroring.enabled ? (0.3 * 30) : 0

            Button {
                text: i18n("Add new path")
                onClicked: fileDialog.open()

                Layout.alignment: Qt.AlignTop | Qt.AlignLeft

                FileDialog {
                    id: fileDialog
                    title: i18n("Choose a Folder")
                    folder: shortcuts.home
                    selectFolder: true

                    visible: false

                    onAccepted: {
                        var oldPaths = kcm.rootPath
                        oldPaths.push(fileDialog.fileUrls)
                        kcm.rootPath = oldPaths
                    }
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
