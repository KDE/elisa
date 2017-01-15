/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

import QtQuick 2.5
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Item {
    property StackView parentStackView
    property MediaPlayList playListModel
    property var playListControler

    property alias randomPlayChecked: shuffleOption.checked
    property alias repeatPlayChecked: repeatOption.checked

    id: topItem

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Component {
        id: rowDelegate

        Rectangle {
            height: Screen.pixelDensity * 15.
            anchors.fill: parent
        }
    }

    Action {
        id: clearPlayList
        text: i18nc("Remove all tracks from play list", "Clear Play List")
        iconName: "list-remove"
        enabled: playListModelDelegate.items.count > 0
        onTriggered: {
            var selectedItems = []
            var myGroup = playListModelDelegate.items
            for (var i = 0; i < myGroup.count; ++i) {
                var myItem = myGroup.get(i)
                selectedItems.push(myItem.itemsIndex)
            }

            playListModel.removeSelection(selectedItems)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Text {
            text: i18nc("text shown at the top of the play list", "Playlist")

            Layout.topMargin: Screen.pixelDensity * 1.5
            Layout.leftMargin: Screen.pixelDensity * 1.5
            Layout.rightMargin: Screen.pixelDensity * 1.5
            Layout.bottomMargin: Screen.pixelDensity * 3.5
        }

        RowLayout {
            Layout.preferredHeight: Screen.pixelDensity * 5.5
            Layout.minimumHeight: Screen.pixelDensity * 5.5
            Layout.maximumHeight: Screen.pixelDensity * 5.5
            Layout.alignment: Qt.AlignVCenter

            Layout.fillWidth: true

            Layout.leftMargin: Screen.pixelDensity * 1.
            Layout.rightMargin: Screen.pixelDensity * 1.

            CheckBox {
                id: shuffleOption

                text: i18n("Shuffle")

                Layout.alignment: Qt.AlignVCenter
            }

            Item {
                Layout.preferredWidth: Screen.pixelDensity * 1.5
                Layout.minimumWidth: Screen.pixelDensity * 1.5
                Layout.maximumWidth: Screen.pixelDensity * 1.5
            }

            CheckBox {
                id: repeatOption

                text: i18n("Repeat")

                Layout.alignment: Qt.AlignVCenter
            }

            Item {
                Layout.fillWidth: true
            }

            Text {
                id: playListInfo
                text: i18np("1 track", "%1 tracks", playListView.rowCount)

                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }
        }

        Rectangle {
            border.width: 1
            border.color: myPalette.mid
            color: myPalette.mid

            Layout.fillWidth: true
            Layout.preferredHeight: 1
            Layout.minimumHeight: 1
            Layout.maximumHeight: 1

            Layout.leftMargin: Screen.pixelDensity * 2.5
            Layout.rightMargin: Screen.pixelDensity * 2.5
        }

        TableView {
            id: playListView

            Layout.fillWidth: true
            Layout.fillHeight: true

            TextEdit {
                readOnly: true
                visible: playListModelDelegate.count === 0
                wrapMode: TextEdit.Wrap
                renderType: TextEdit.NativeRendering

                font.weight: Font.ExtraLight
                font.pointSize: 12

                text: i18nc("Text shown when play list is empty", "Your play list is empty.\nIn order to start, you can explore your music library with the left menu.\nUse the available buttons to add your selection.")
                anchors.fill: parent
                anchors.margins: Screen.pixelDensity * 2.
            }

            model: DelegateModel {
                id: playListModelDelegate
                model: playListModel

                groups: [
                    DelegateModelGroup { name: "selected" }
                ]

                delegate: DraggableItem {
                    id: item

                    PlayListEntry {
                        id: entry
                        width: playListView.width
                        index: model.index
                        hasAlbumHeader: if (model != undefined && model.hasAlbumHeader !== undefined)
                                            model.hasAlbumHeader
                                        else
                                            true
                        title: if (model != undefined && model.title !== undefined)
                                   model.title
                               else
                                   ''
                        artist: if (model != undefined && model.artist !== undefined)
                                    model.artist
                                else
                                    ''
                        itemDecoration: if (model != undefined && model.image !== undefined)
                                            model.image
                                        else
                                            ''
                        duration: if (model != undefined && model.duration !== undefined)
                                      model.duration
                                  else
                                      ''
                        trackNumber: if (model != undefined && model.trackNumber !== undefined)
                                         model.trackNumber
                                     else
                                         ''
                        discNumber: if (model != undefined && model.discNumber !== undefined)
                                         model.discNumber
                                     else
                                         ''
                        album: if (model != undefined && model.album !== undefined)
                                   model.album
                               else
                                   ''
                        isPlaying: model.isPlaying
                        isSelected: item.DelegateModel.inSelected
                        containsMouse: item.containsMouse

                        playListModel: topItem.playListModel
                        playListControler: topItem.playListControler

                        contextMenu: Menu {
                            MenuItem {
                                action: entry.clearPlayListAction
                            }
                            MenuItem {
                                action: entry.playNowAction
                            }
                        }
                    }

                    draggedItemParent: topItem

                    onClicked:
                    {
                        var myGroup = playListModelDelegate.groups[2]
                        if (myGroup.count > 0 && !DelegateModel.inSelected) {
                            myGroup.remove(0, myGroup.count)
                        }

                        DelegateModel.inSelected = !DelegateModel.inSelected
                    }

                    onRightClicked: contentItem.contextMenu.popup()

                    onMoveItemRequested: {
                        playListModel.move(from, to, 1);
                    }
                }
            }

            backgroundVisible: false
            headerVisible: false
            frameVisible: false
            focus: true
            rowDelegate: rowDelegate

            TableViewColumn {
                role: "title"
                title: "Title"
            }
        }

        ToolBar {
            id: actionBar

            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                ToolButton {
                    action: clearPlayList
                }

                Item { Layout.fillWidth: true }
            }
        }
    }
}

