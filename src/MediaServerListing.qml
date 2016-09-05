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
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import QtMultimedia 5.4

import org.mgallien.QmlExtension 1.0

Item {
    property var rootIndex
    property StackView stackView
    property var contentModel
    property MediaPlayList playListModel
    property var musicDatabase

    id: rootElement

    AlbumFilterProxyModel {
        id: filterProxyModel

        sourceModel: contentModel

        filterText: filterTextInput.text
    }

    DelegateModel {
        id: delegateContentModel
        model: filterProxyModel

        delegate: Rectangle {
            id: mediaServerEntry

            color: 'white'

            width: contentDirectoryView.cellWidth
            height: contentDirectoryView.cellHeight

            ColumnLayout {
                id: mainData

                anchors.fill: parent

                Item {
                    Layout.preferredHeight: Screen.pixelDensity * 0.5
                    Layout.minimumHeight: Screen.pixelDensity * 0.5
                    Layout.maximumHeight: Screen.pixelDensity * 0.5
                }

                Image {
                    id: playIcon
                    source: image
                    width: parent.height * 0.7
                    height: parent.height * 0.7
                    sourceSize.width: parent.height * 0.7
                    sourceSize.height: parent.height * 0.7
                    fillMode: Image.PreserveAspectFit

                    Layout.preferredWidth: contentDirectoryView.cellWidth * 0.9
                    Layout.preferredHeight: contentDirectoryView.cellWidth * 0.9

                    Layout.alignment: Qt.AlignHCenter

                    Loader {
                        id: hoverLoader
                        active: false

                        anchors.fill: parent

                        sourceComponent: Rectangle {
                            id: hoverLayer

                            anchors.fill: parent

                            color: 'black'
                            opacity: 0.7

                            BrightnessContrast {
                                anchors.fill: playAction
                                source: playAction
                                brightness: 1.0
                                contrast: 1.0

                                MouseArea {
                                    id: clickHandle

                                    anchors.fill: parent
                                    acceptedButtons: Qt.LeftButton

                                    onClicked: {
                                        stackView.push(Qt.resolvedUrl("MediaAlbumView.qml"),
                                                       {
                                                           'stackView': stackView,
                                                           'musicDatabase': musicDatabase,
                                                           'playListModel': playListModel,
                                                           'albumArtUrl': image,
                                                           'albumName': title,
                                                           'artistName': artist,
                                                           'tracksCount': count
                                                       })
                                    }
                                }
                            }

                            Image {
                                id: playAction
                                source: 'image://icon/document-open-folder'

                                anchors.centerIn: parent

                                opacity: 1
                                visible: false

                                width: Screen.pixelDensity * 10
                                height: Screen.pixelDensity * 10
                                sourceSize.width: width
                                sourceSize.height: width
                                fillMode: Image.PreserveAspectFit
                            }
                        }
                    }

                    MouseArea {
                        id: hoverHandle

                        hoverEnabled: true

                        anchors.fill: parent
                        propagateComposedEvents: true

                        onEntered: hoverLoader.active = true
                        onExited: hoverLoader.active = false
                    }
                }

                Label {
                    id: titleLabel
                    text: if (title != undefined)
                              title
                          else
                              ''
                    font.weight: Font.Bold
                    horizontalAlignment: Text.AlignLeft

                    Layout.preferredWidth: contentDirectoryView.cellWidth * 0.9
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                    elide: "ElideRight"
                }

                Label {
                    id: artistLabel
                    text: if (artist != undefined)
                              artist
                          else
                              ''
                    font.weight: Font.Normal
                    horizontalAlignment: Text.AlignLeft

                    Layout.preferredWidth: contentDirectoryView.cellWidth * 0.9
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                    elide: "ElideRight"
                }

                Label {
                    id: numberLabel
                    text: if (count != undefined)
                              i18np("1 Song", "%1 Songs", playListView.rowCount)
                          else
                              i18nc("Number of songs for an albu", '0 Songs')

                    font.weight: Font.Light
                    horizontalAlignment: Text.AlignLeft

                    Layout.preferredWidth: contentDirectoryView.cellWidth * 0.9
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                    elide: "ElideRight"
                }

                Item {
                    Layout.preferredHeight: Screen.pixelDensity * 0.5
                    Layout.minimumHeight: Screen.pixelDensity * 0.5
                    Layout.maximumHeight: Screen.pixelDensity * 0.5
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: i18n("Filter: ")
            }

            TextField {
                id: filterTextInput

                placeholderText: i18nc("Placeholder text in the filter text box", "Filter")

                Layout.fillWidth: true
            }
        }

        Rectangle {
            color: 'white'

            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollView {
                anchors.fill: parent
                GridView {
                    id: contentDirectoryView

                    cellWidth: Screen.pixelDensity * 40.
                    cellHeight: Screen.pixelDensity * 60.

                    model: delegateContentModel

                    focus: true
                }
            }
        }
    }
}
