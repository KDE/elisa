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
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import org.mgallien.QmlExtension 1.0

Item {
    id: viewAlbumDelegate

    property string title
    property string artist
    property alias itemDecoration : mainIcon.source
    property alias duration : durationLabel.text
    property int trackNumber
    property alias isPlaying : playIcon.visible
    property bool showHoverButtons
    property Action hoverAction

    RowLayout {
        width: parent.width
        height: parent.height
        spacing: 0

        ColumnLayout {
            Layout.preferredWidth: parent.height
            Layout.preferredHeight: parent.height
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            spacing: 0

            Item {
                Layout.preferredHeight: 1
                Layout.minimumHeight: 1
                Layout.maximumHeight: 1
            }

            Image {
                id: mainIcon
                source: itemDecoration
                Layout.preferredWidth: parent.height - 2
                Layout.preferredHeight: parent.height - 2
                width: parent.height - 2
                sourceSize.width: width
                sourceSize.height: width
                fillMode: Image.PreserveAspectFit
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

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

                                onClicked: hoverAction.trigger(clickHandle)
                            }
                        }

                        Image {
                            id: playAction
                            source: hoverAction.iconSource

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
            }

            Item {
                Layout.preferredHeight: 1
                Layout.minimumHeight: 1
                Layout.maximumHeight: 1
            }
        }

        ColumnLayout {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.preferredHeight: viewAlbumDelegate.height
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            spacing: 0

            Item {
                Layout.preferredHeight: Screen.pixelDensity * 4.
                Layout.minimumHeight: Screen.pixelDensity * 4.
                Layout.maximumHeight: Screen.pixelDensity * 4.
            }

            Label {
                id: mainLabel
                text: trackNumber + ' - ' + title
                Layout.preferredWidth: Screen.pixelDensity * 12
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                elide: "ElideRight"
            }

            Item {
                Layout.fillHeight: true
            }

            Label {
                id: authorLabel
                text: artist + ' - ' + album
                Layout.preferredWidth: Screen.pixelDensity * 3
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                elide: "ElideRight"
            }

            Item {
                Layout.preferredHeight: Screen.pixelDensity * 4.
                Layout.minimumHeight: Screen.pixelDensity * 4.
                Layout.maximumHeight: Screen.pixelDensity * 4.
            }
        }

        Item {
            Layout.preferredWidth: width
            Layout.minimumWidth: width
            Layout.maximumWidth: width
            width: Screen.pixelDensity * 2
        }

        Button {
            id: playButton
            iconName: 'media-playback-start'

            visible: showHoverButtons && !isPlaying

            Layout.preferredWidth: parent.height * 0.5
            Layout.preferredHeight: parent.height * 0.5
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.maximumWidth: parent.height * 0.7
            Layout.maximumHeight: parent.height * 0.7
            width: parent.height * 0.7
            height: parent.height * 0.7
        }

        Image {
            id: playIcon
            source: 'image://icon/media-playback-start'
            Layout.preferredWidth: parent.height * 0.5
            Layout.preferredHeight: parent.height * 0.5
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.maximumWidth: parent.height * 0.7
            Layout.maximumHeight: parent.height * 0.7
            width: parent.height * 0.7
            height: parent.height * 0.7
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit
            visible: isPlaying
        }

        Item {
            Layout.preferredWidth: width
            Layout.minimumWidth: width
            Layout.maximumWidth: width
            width: Screen.pixelDensity * 12
        }

        ColumnLayout {
            Layout.preferredHeight: viewAlbumDelegate.height
            Layout.minimumHeight: viewAlbumDelegate.height
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            spacing: 0

            Item {
                Layout.preferredHeight: Screen.pixelDensity * 3.
                Layout.minimumHeight: Screen.pixelDensity * 3.
                Layout.maximumHeight: Screen.pixelDensity * 3.
            }

            Label {
                id: durationLabel
                text: duration
                elide: "ElideRight"
                Layout.alignment: Qt.AlignRight
            }

            Item {
                Layout.fillHeight: true
            }
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 6.
            Layout.minimumWidth: Screen.pixelDensity * 6.
            Layout.maximumWidth: Screen.pixelDensity * 6.
        }
    }

    MouseArea {
        id: hoverHandle

        acceptedButtons: Qt.NoButton

        hoverEnabled: true

        anchors.fill: parent
        propagateComposedEvents: true

        onEntered: hoverLoader.active = true
        onExited: hoverLoader.active = false
    }
}

