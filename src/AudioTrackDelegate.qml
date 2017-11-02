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
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import org.mgallien.QmlExtension 1.0

FocusScope {
    property string title
    property string artist
    property string albumArtist
    property alias duration: durationLabel.text
    property int trackNumber
    property int discNumber
    property alias rating: ratingWidget.starRating
    property bool isFirstTrackOfDisc
    property bool isSingleDiscAlbum
    property var databaseId
    property var playList
    property var playerControl
    property bool isAlternateColor
    property var contextMenu
    property var trackData
    property alias clearAndEnqueueAction: clearAndEnqueue
    property alias enqueueAction: enqueue

    signal clicked()
    signal rightClicked()

    id: mediaServerEntry

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

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredHeight: elisaTheme.delegateHeight + elisaTheme.layoutVerticalMargin * 2
            Layout.minimumHeight: elisaTheme.delegateHeight + elisaTheme.layoutVerticalMargin * 2
            Layout.maximumHeight: elisaTheme.delegateHeight + elisaTheme.layoutVerticalMargin * 2
            Layout.fillWidth: true

            color: (mediaServerEntry.isAlternateColor ? myPalette.alternateBase : myPalette.base)

            visible: isFirstTrackOfDisc && !isSingleDiscAlbum

            focus: true

            LabelWithToolTip {
                id: discHeaderLabel

                text: 'CD ' + discNumber

                font.weight: Font.Bold
                font.italic: true
                color: myPalette.text

                anchors.fill: parent
                anchors.topMargin: elisaTheme.layoutVerticalMargin * 2
                anchors.leftMargin: elisaTheme.layoutHorizontalMargin

                elide: Text.ElideRight
            }
        }

        Rectangle {
            id: rowRoot

            Layout.fillHeight: true
            Layout.fillWidth: true

            color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)

            MouseArea {
                id: hoverHandle

                hoverEnabled: true
                acceptedButtons: Qt.LeftButton
                focus: true

                anchors.fill: parent

                onClicked:
                {
                    if (mouse.button == Qt.LeftButton) {
                        hoverHandle.forceActiveFocus()
                        mediaServerEntry.clicked()
                    } else if (mouse.button == Qt.RightButton) {
                        mediaServerEntry.rightClicked()
                    }
                }

                onDoubleClicked: playList.enqueue(trackData)

                RowLayout {
                    spacing: 0

                    anchors.fill: parent

                    LabelWithToolTip {
                        id: mainLabel

                        text: (artist !== albumArtist ?
                                   "<b>" + trackNumber + ' - ' + title + "</b>" + ' - <i>' + artist + '</i>' :
                                   "<b>" + trackNumber + ' - ' + title + "</b>")

                        horizontalAlignment: Text.AlignLeft

                        color: myPalette.text

                        Layout.leftMargin: {
                            if (!LayoutMirroring.enabled)
                                return (!isSingleDiscAlbum ? elisaTheme.layoutHorizontalMargin * 4 : elisaTheme.layoutHorizontalMargin)
                            else
                                return 0
                        }
                        Layout.rightMargin: {
                            if (LayoutMirroring.enabled)
                                return (!isSingleDiscAlbum ? elisaTheme.layoutHorizontalMargin * 4 : elisaTheme.layoutHorizontalMargin)
                            else
                                return 0
                        }
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.fillWidth: true

                        elide: Text.ElideRight
                    }

                    Loader {
                        id: hoverLoader
                        active: false

                        Layout.fillHeight: true
                        Layout.preferredWidth: elisaTheme.delegateHeight * 2
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                        sourceComponent: RowLayout {
                            id: highlightMarker

                            spacing: 0

                            anchors.fill: parent

                            ToolButton {
                                id: enqueueButton

                                opacity: 1
                                visible: opacity > 0.1
                                action: enqueue

                                Layout.preferredHeight: elisaTheme.delegateHeight * 0.75
                                Layout.preferredWidth: elisaTheme.delegateHeight * 0.75
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                            }

                            ToolButton {
                                id: clearAndEnqueueButton

                                opacity: 1
                                visible: opacity > 0.1
                                action: clearAndEnqueue

                                Layout.preferredHeight: elisaTheme.delegateHeight * 0.75
                                Layout.preferredWidth: elisaTheme.delegateHeight * 0.75
                                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                                Layout.leftMargin:   LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                            }
                        }
                    }

                    RatingStar {
                        id: ratingWidget

                        starSize: elisaTheme.ratingStarSize

                        Layout.leftMargin: elisaTheme.layoutHorizontalMargin
                        Layout.rightMargin: elisaTheme.layoutHorizontalMargin
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }

                    LabelWithToolTip {
                        id: durationLabel

                        text: duration

                        font.weight: Font.Light
                        color: myPalette.text

                        elide: "ElideRight"

                        Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.leftMargin:   LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }
                }
            }
        }
    }

    states: [
        State {
            name: 'default'

            PropertyChanges {
                target: hoverLoader
                active: false
            }
            PropertyChanges {
                target: rowRoot
                color: isAlternateColor ? myPalette.alternateBase : myPalette.base
            }
        },
        State {
            name: 'active'

            when: hoverHandle.containsMouse || mediaServerEntry.activeFocus

            PropertyChanges {
                target: hoverLoader
                active: true
            }
            PropertyChanges {
                target: rowRoot
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

