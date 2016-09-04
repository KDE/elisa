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

import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Item {
    property StackView parentStackView
    property MediaPlayerControl playControl
    property MediaPlayList playListModel

    property alias randomPlayChecked: shuffleOption.checked
    property alias repeatPlayChecked: repeatOption.checked

    property alias albumName: albumContext.albumName
    property alias artistName: albumContext.artistName
    property alias albumArtUrl: albumContext.albumArtUrl
    property alias tracksCount: albumContext.tracksCount

    id: topItem

    Component {
        id: rowDelegate

        Rectangle {
            height: Screen.pixelDensity * 15.
            anchors.fill: parent
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.preferredHeight: Screen.pixelDensity * 5.5
            Layout.minimumHeight: Screen.pixelDensity * 5.5
            Layout.maximumHeight: Screen.pixelDensity * 5.5
            Layout.alignment: Qt.AlignVCenter

            Layout.fillWidth: true

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

            Label {
                id: playListInfo
                text: i18np("1 track", "%1 tracks", playListView.rowCount)

                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }
        }

        Rectangle {
            border.width: 1
            border.color: "#DDDDDD"
            color: "#DDDDDD"

            Layout.fillWidth: true
            Layout.preferredHeight: 1
            Layout.minimumHeight: 1
            Layout.maximumHeight: 1
        }

        RowLayout {
            id: contentLayout
            spacing: 0

            Layout.fillHeight: true
            Layout.fillWidth: true

            TableView {
                id: playListView

                Layout.minimumWidth: topItem.width / 2
                Layout.maximumWidth: topItem.width / 2
                Layout.preferredWidth: topItem.width / 2

                Layout.fillWidth: true
                Layout.fillHeight: true

                model: DelegateModel {
                    model: playListModel

                    delegate: DraggableItem {
                        AudioTrackDelegate {
                            height: Screen.pixelDensity * 15.
                            width: playListView.width
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
                            isPlaying: model.isPlaying
                            showHoverButtons: false

                            hoverAction: Action {
                                id: playListRemove

                                iconSource: 'image://icon/list-remove'

                                onTriggered: playListModel.removeRows(index, 1, playListView.model.rootIndex)
                            }
                        }

                        draggedItemParent: topItem

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

            Rectangle {
                border.width: 1
                border.color: "#DDDDDD"
                color: "#DDDDDD"

                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                Layout.preferredHeight: parent.height - Screen.pixelDensity * 5.
                Layout.preferredWidth: 1
                Layout.minimumWidth: 1
                Layout.maximumWidth: 1
            }

            ContextView {
                id: albumContext

                Layout.minimumWidth: topItem.width / 2
                Layout.maximumWidth: topItem.width / 2
                Layout.preferredWidth: topItem.width / 2

                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}

