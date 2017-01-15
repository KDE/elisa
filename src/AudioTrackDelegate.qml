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
    property alias artist: artistLabel.text
    property string albumArtist
    property alias duration: durationLabel.text
    property int trackNumber
    property int discNumber
    property bool isFirstTrackOfDisc
    property bool isSingleDiscAlbum
    property var databaseId
    property var playList
    property var playerControl
    property bool isSelected
    property var contextMenu
    property alias clearAndEnqueueAction: clearAndEnqueue
    property alias enqueueAction: enqueue

    signal clicked()
    signal rightClicked()

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Action {
        id: clearAndEnqueue
        text: i18nc("Clear play list and enqueue current track", "Play Now and Replace Play List")
        iconName: "media-playback-start"
        onTriggered: {
            playList.clearAndEnqueue(databaseId)
            playerControl.playPause()
        }
    }

    Action {
        id: enqueue
        text: i18nc("Enqueue current track", "Enqueue")
        iconName: "media-track-add-amarok"
        onTriggered: {
            playList.enqueue(databaseId)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.preferredHeight: Screen.pixelDensity * 3.
            Layout.minimumHeight: Screen.pixelDensity * 3.
            Layout.maximumHeight: Screen.pixelDensity * 3.

            visible: isFirstTrackOfDisc && !isSingleDiscAlbum
        }

        Text {
            id: discHeaderLabel
            text: 'CD ' + discNumber
            font.weight: Font.Bold
            font.italic: true
            Layout.preferredWidth: Screen.pixelDensity * 9
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft
            elide: "ElideRight"
            visible: isFirstTrackOfDisc && !isSingleDiscAlbum
        }

        Item {
            Layout.preferredHeight: (isFirstTrackOfDisc && !isSingleDiscAlbum ? Screen.pixelDensity * 1.5 : 0)
            Layout.minimumHeight: (isFirstTrackOfDisc && !isSingleDiscAlbum ? Screen.pixelDensity * 1.5 : 0)
            Layout.maximumHeight: (isFirstTrackOfDisc && !isSingleDiscAlbum ? Screen.pixelDensity * 1.5 : 0)

            visible: isFirstTrackOfDisc && !isSingleDiscAlbum
        }

        Rectangle {
            id: highlightMarker

            Layout.fillHeight: true
            Layout.fillWidth: true

            color: myPalette.base

            MouseArea {
                id: hoverArea

                anchors.fill: parent

                hoverEnabled: true

                acceptedButtons: Qt.RightButton | Qt.LeftButton

                onClicked:
                {
                    if (mouse.button == Qt.LeftButton)
                        viewAlbumDelegate.clicked()
                    if (mouse.button == Qt.RightButton)
                        viewAlbumDelegate.rightClicked()
                }

                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    Item {
                        Layout.preferredWidth: Screen.pixelDensity * 2
                        Layout.minimumWidth: Screen.pixelDensity * 2
                        Layout.maximumWidth: Screen.pixelDensity * 2
                        width: Screen.pixelDensity * 2
                    }

                    ColumnLayout {
                        Layout.preferredHeight: viewAlbumDelegate.height

                        Layout.fillWidth: true

                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                        spacing: 0

                        Item {
                            Layout.preferredHeight: (isFirstTrackOfDisc && !isSingleDiscAlbum ? Screen.pixelDensity * 1.5 : Screen.pixelDensity * 3.)
                            Layout.minimumHeight: (isFirstTrackOfDisc && !isSingleDiscAlbum ? Screen.pixelDensity * 1.5 : Screen.pixelDensity * 3.)
                            Layout.maximumHeight: (isFirstTrackOfDisc && !isSingleDiscAlbum ? Screen.pixelDensity * 1.5 : Screen.pixelDensity * 3.)
                        }

                        Text {
                            id: mainLabel
                            text: trackNumber + ' - ' + title
                            font.weight: Font.Bold
                            Layout.preferredWidth: Screen.pixelDensity * 9
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            Layout.leftMargin: (!isSingleDiscAlbum ? Screen.pixelDensity * 4 : 0)
                            elide: "ElideRight"
                        }

                        Item {
                            Layout.fillHeight: true
                        }

                        Row {
                            Text {
                                id: artistLabel
                                visible: artist !== albumArtist
                                text: artist
                                font.weight: Font.Light
                                font.italic: true
                                elide: "ElideRight"
                            }
                            Text {
                                id: artistSeparatorLabel
                                visible: artist !== albumArtist
                                text: ' - '
                                font.weight: Font.Light
                                elide: "ElideRight"
                            }
                            Text {
                                id: durationLabel
                                text: duration
                                font.weight: Font.Light
                                elide: "ElideRight"
                            }

                            Layout.preferredWidth: Screen.pixelDensity * 3
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            Layout.leftMargin: (!isSingleDiscAlbum ? Screen.pixelDensity * 4 : 0)
                        }

                        Item {
                            Layout.preferredHeight: Screen.pixelDensity * 3.
                            Layout.minimumHeight: Screen.pixelDensity * 3.
                            Layout.maximumHeight: Screen.pixelDensity * 3.
                        }
                    }

                    Item {
                        Layout.preferredWidth: Screen.pixelDensity * 3.
                        Layout.minimumWidth: Screen.pixelDensity * 3.
                        Layout.maximumWidth: Screen.pixelDensity * 3.
                    }

                    ToolButton {
                        id: enqueueButton

                        visible: opacity > 0.1
                        action: enqueue
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }

                    ToolButton {
                        id: clearAndEnqueueButton

                        visible: opacity > 0.1
                        action: clearAndEnqueue
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }

                    Item {
                        Layout.preferredWidth: Screen.pixelDensity * 6.
                        Layout.minimumWidth: Screen.pixelDensity * 6.
                        Layout.maximumWidth: Screen.pixelDensity * 6.
                    }
                }
            }
        }
    }

    states: [
        State {
            name: 'notSelected'
            when: !isSelected && !hoverArea.containsMouse
            PropertyChanges {
                target: clearAndEnqueueButton
                opacity: 0
            }
            PropertyChanges {
                target: enqueueButton
                opacity: 0
            }
            PropertyChanges {
                target: highlightMarker
                color: myPalette.base
            }
        },
        State {
            name: 'selected'
            when: isSelected
            PropertyChanges {
                target: clearAndEnqueueButton
                opacity: 1
            }
            PropertyChanges {
                target: enqueueButton
                opacity: 1
            }
            PropertyChanges {
                target: highlightMarker
                color: myPalette.highlight
            }
        },
        State {
            name: 'hoveredAndNotSelected'
            when: !isSelected && hoverArea.containsMouse
            PropertyChanges {
                target: clearAndEnqueueButton
                opacity: 1
            }
            PropertyChanges {
                target: enqueueButton
                opacity: 1
            }
            PropertyChanges {
                target: highlightMarker
                color: myPalette.alternateBase
            }
        }

    ]
    transitions: Transition {
        ParallelAnimation {
            NumberAnimation {
                properties: "height, opacity"
                easing.type: Easing.InOutQuad
                duration: 50
            }
            ColorAnimation {
                properties: "color"
                duration: 200
            }
        }
    }
}

