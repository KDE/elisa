/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
import org.kde.elisa 1.0

FocusScope {
    id: viewTrackDelegate

    property string title
    property var coverImage
    property string artist
    property string albumName
    property alias duration: durationLabel.text
    property int trackNumber
    property int discNumber
    property alias rating: ratingWidget.starRating
    property var databaseId
    property var playList
    property var playerControl
    property bool isAlternateColor
    property var trackData

    signal clicked()

    Action {
        id: clearAndEnqueue
        text: i18nc("Clear play list and enqueue current track", "Play Now and Replace Play List")
        iconName: "media-playback-start"
        onTriggered: {
            playList.clearAndEnqueue(trackData)
            playerControl.ensurePlay()
        }
    }

    Action {
        id: enqueue
        text: i18nc("Enqueue current track", "Enqueue")
        iconName: "media-track-add-amarok"
        onTriggered: {
            playList.enqueue(trackData)
        }
    }

    Rectangle {
        id: highlightMarker

        anchors.fill: parent

        color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)

        MouseArea {
            id: hoverArea

            anchors.fill: parent

            hoverEnabled: true

            acceptedButtons: Qt.LeftButton

            onClicked: {
                hoverArea.forceActiveFocus()
                viewTrackDelegate.clicked()
            }

            onDoubleClicked: playList.enqueue(trackData)

            RowLayout {
                anchors.fill: parent
                spacing: 0

                Item {
                    Layout.preferredHeight: viewTrackDelegate.height * 0.9
                    Layout.preferredWidth: viewTrackDelegate.height * 0.9

                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                    Image {
                        id: coverImageElement

                        anchors.fill: parent

                        sourceSize.width: viewTrackDelegate.height * 0.9
                        sourceSize.height: viewTrackDelegate.height * 0.9
                        fillMode: Image.PreserveAspectFit
                        smooth: true

                        source: (coverImage ? coverImage : Qt.resolvedUrl(elisaTheme.albumCover))

                        asynchronous: true

                        layer.enabled: true
                        layer.effect: DropShadow {
                            horizontalOffset: viewTrackDelegate.height * 0.02
                            verticalOffset: viewTrackDelegate.height * 0.02

                            source: coverImageElement

                            radius: 5.0
                            samples: 11

                            color: myPalette.shadow
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignLeft

                    spacing: 0

                    LabelWithToolTip {
                        id: mainLabel

                        text: trackNumber + ' - ' + title
                        horizontalAlignment: Text.AlignLeft

                        font.weight: Font.Bold
                        color: myPalette.text

                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                        Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.fillWidth: true
                        Layout.topMargin: elisaTheme.layoutVerticalMargin / 2

                        elide: "ElideRight"
                    }

                    Item {
                        Layout.fillHeight: true
                    }

                    LabelWithToolTip {
                        id: artistLabel

                        text: artist + ' - ' + albumName
                        horizontalAlignment: Text.AlignLeft

                        font.weight: Font.Light
                        font.italic: true
                        color: myPalette.text

                        Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                        Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.fillWidth: true
                        Layout.bottomMargin: elisaTheme.layoutVerticalMargin / 2

                        elide: "ElideRight"
                    }
                }

                ToolButton {
                    id: enqueueButton

                    Layout.preferredHeight: elisaTheme.trackDelegateHeight * 0.75
                    Layout.preferredWidth: elisaTheme.trackDelegateHeight * 0.75

                    opacity: 0
                    visible: opacity > 0.1
                    action: enqueue
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                }

                ToolButton {
                    id: clearAndEnqueueButton

                    Layout.preferredHeight: elisaTheme.trackDelegateHeight * 0.75
                    Layout.preferredWidth: elisaTheme.trackDelegateHeight * 0.75

                    opacity: 0
                    visible: opacity > 0.1
                    action: clearAndEnqueue

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                    Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                }

                RatingStar {
                    id: ratingWidget

                    starSize: elisaTheme.ratingStarSize

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    Layout.leftMargin: elisaTheme.layoutHorizontalMargin
                    Layout.rightMargin: elisaTheme.layoutHorizontalMargin
                }

                LabelWithToolTip {
                    id: durationLabel

                    text: duration

                    font.weight: Font.Light
                    color: myPalette.text

                    elide: "ElideRight"

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                    Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                }
            }
        }
    }

    states: [
        State {
            name: 'notSelected'
            when: !hoverArea.containsMouse && !viewTrackDelegate.activeFocus
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
                color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)
            }
        },
        State {
            name: 'hoveredAndNotSelected'
            when: hoverArea.containsMouse || viewTrackDelegate.activeFocus
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
                color: myPalette.mid
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
