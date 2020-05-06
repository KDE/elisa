/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import QtQuick.Dialogs 1.2 as Dialogs

ColumnLayout {
    spacing: 0

    CheckBox {
        id: forceFileIndexerUsageCheckBox

        text: i18n("Force filesystem indexing")

        checked: !config.forceUsageOfFastFileSearch

        onCheckedChanged: config.forceUsageOfFastFileSearch = !checked
    }

    Label {
        text: i18n("This is much slower than usage of fast search!\nPlease activate it only if Elisa cannot find your music and searching for the file also does not work.\nPlease report this as a bug.")
    }

    RowLayout {
        Layout.fillHeight: true
        Layout.fillWidth: true

        Layout.topMargin: 10

        spacing: 0

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

                width: scrollBar.visible ? pathList.width - scrollBar.width : pathList.width

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
                            icon.name: 'list-remove'

                            Accessible.onPressAction: onClicked

                            anchors.top: parent.top
                            anchors.right: parent.right

                            onClicked:
                            {
                                var oldPaths = config.rootPath
                                oldPaths.splice(delegateItem.DelegateModel.itemsIndex, 1)
                                config.rootPath = oldPaths
                            }
                        }
                    }
                }
            }
        }

        ListView {
            id:pathList

            Layout.fillWidth: true
            Layout.fillHeight: true
            boundsBehavior: Flickable.StopAtBounds

            clip: true

            model: DelegateModel {
                model: config.rootPath

                delegate: pathDelegate
            }

            ScrollBar.vertical: ScrollBar {
                id: scrollBar
            }

            highlight: highlightBar
        }

        ColumnLayout {
            Layout.fillHeight: true
            Layout.leftMargin: !LayoutMirroring.enabled ? (0.3 * 30) : 0
            Layout.rightMargin: LayoutMirroring.enabled ? (0.3 * 30) : 0

            Button {
                text: i18n("Add new path")
                onClicked: fileDialog.open()

                Accessible.onPressAction: onClicked

                Layout.alignment: Qt.AlignTop | Qt.AlignLeft

                Dialogs.FileDialog {
                    id: fileDialog
                    title: i18n("Choose a Folder")
                    folder: shortcuts.home
                    selectFolder: true

                    visible: false

                    onAccepted: {
                        var oldPaths = config.rootPath
                        oldPaths.push(fileDialog.fileUrls)
                        config.rootPath = oldPaths
                    }
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
