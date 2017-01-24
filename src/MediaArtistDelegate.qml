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

Rectangle {
    property StackView stackView
    property MediaPlayList playListModel
    property var playerControl
    property var contentDirectoryModel
    property alias image: playIcon.source
    property alias name: nameLabel.text

    id: mediaServerEntry

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    color: myPalette.base

    ColumnLayout {
        id: mainData

        anchors.fill: parent

        Item {
            Layout.preferredHeight: Screen.pixelDensity * 0.5
            Layout.minimumHeight: Screen.pixelDensity * 0.5
            Layout.maximumHeight: Screen.pixelDensity * 0.5
        }

        Image {
            id: artistDecoration

            source: 'image://icon/view-media-artist'

            Layout.preferredWidth: mediaServerEntry.width * 0.9
            Layout.preferredHeight: mediaServerEntry.width * 0.9

            width: mediaServerEntry.width * 0.9
            height: mediaServerEntry.width * 0.9

            sourceSize.width: mediaServerEntry.width * 0.9
            sourceSize.height: mediaServerEntry.width * 0.9

            fillMode: Image.PreserveAspectFit

            Layout.alignment: Qt.AlignHCenter

            Loader {
                anchors.fill: parent

                active: false

                Image {
                    id: playIcon

                    visible: false

                    anchors.fill: parent

                    sourceSize.width: mediaServerEntry.width * 0.9
                    sourceSize.height: mediaServerEntry.width * 0.9
                    fillMode: Image.PreserveAspectFit
                }
            }

            Loader {
                id: hoverLoader
                active: false

                anchors.fill: artistDecoration

                sourceComponent: Rectangle {
                    id: hoverLayer

                    anchors.fill: parent

                    color: myPalette.dark
                    opacity: 0.85

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
                                stackView.push({
                                                   item: Qt.resolvedUrl("MediaArtistAlbumView.qml"),
                                                   properties :
                                                   {
                                                       playListModel: mediaServerEntry.playListModel,
                                                       contentDirectoryModel: mediaServerEntry.contentDirectoryModel,
                                                       playerControl: mediaServerEntry.playerControl,
                                                       stackView: mediaServerEntry.stackView,
                                                       artistName: name,
                                                   }})
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

                anchors.fill: artistDecoration
                propagateComposedEvents: true

                onEntered: hoverLoader.active = true
                onExited: hoverLoader.active = false
            }
        }

        Text {
            id: nameLabel

            font.weight: Font.Bold
            horizontalAlignment: Text.AlignLeft

            Layout.preferredWidth: mediaServerEntry.width * 0.9
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
