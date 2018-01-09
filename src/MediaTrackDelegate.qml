/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
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

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import org.kde.elisa 1.0

FocusScope {
    id: mediaTrack

    property string title
    property var coverImage
    property string artist
    property string albumName
    property string albumArtist
    property string trackResource
    property string duration
    property int trackNumber
    property int discNumber
    property int rating
    property bool isFirstTrackOfDisc
    property bool isSingleDiscAlbum
    property var trackData
    property bool isAlternateColor
    property bool detailedView: true

    signal clicked()
    signal clearPlaylist()
    signal enqueueToPlaylist(var track)
    signal ensurePlay()

    Action {
        id: clearAndEnqueue
        text: i18nc("Clear play list and enqueue current track", "Play Now and Replace Play List")
        iconName: "media-playback-start"
        onTriggered: {
            clearPlaylist()
            enqueueToPlaylist(trackData)
            ensurePlay()
        }
    }

    Action {
        id: enqueue
        text: i18nc("Enqueue current track", "Enqueue")
        iconName: "media-track-add-amarok"
        onTriggered: enqueueToPlaylist(trackData)
    }

    Keys.onReturnPressed: enqueueToPlaylist(trackData)
    Keys.onEnterPressed: enqueueToPlaylist(trackData)

    Rectangle {
        id: rowRoot

        anchors.fill: parent

        color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)

        MouseArea {
            id: hoverArea

            anchors.fill: parent

            hoverEnabled: true
            focus: true
            acceptedButtons: Qt.LeftButton

            onClicked: {
                hoverArea.forceActiveFocus()
                mediaTrack.clicked()
            }

            onDoubleClicked: enqueueToPlaylist(trackData)

            RowLayout {
                anchors.fill: parent
                spacing: 0

                LabelWithToolTip {
                    id: mainLabel

                    visible: !detailedView

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

                Item {
                    Layout.preferredHeight: mediaTrack.height * 0.9
                    Layout.preferredWidth: mediaTrack.height * 0.9

                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                    visible: detailedView

                    Image {
                        id: coverImageElement

                        anchors.fill: parent

                        sourceSize.width: mediaTrack.height * 0.9
                        sourceSize.height: mediaTrack.height * 0.9
                        fillMode: Image.PreserveAspectFit
                        smooth: true

                        source: (coverImage ? coverImage : Qt.resolvedUrl(elisaTheme.defaultAlbumImage))

                        asynchronous: true

                        layer.enabled: coverImage === '' ? false : true

                        layer.effect: DropShadow {
                            source: coverImageElement

                            radius: 10
                            spread: 0.1
                            samples: 21

                            color: myPalette.shadow
                        }
                    }
                }

                ColumnLayout {
                    visible: detailedView

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignLeft

                    spacing: 0

                    LabelWithToolTip {
                        id: mainLabelDetailed

                        text: trackNumber + ' - ' + title
                        horizontalAlignment: Text.AlignLeft

                        font.weight: Font.Bold
                        color: myPalette.text

                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                        Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.fillWidth: true
                        Layout.topMargin: elisaTheme.layoutVerticalMargin / 2

                        elide: Text.ElideRight
                    }

                    Item {
                        Layout.fillHeight: true
                    }

                    LabelWithToolTip {
                        id: artistLabel

                        text: (isSingleDiscAlbum ? artist + ' - ' + albumName : artist + ' - ' + albumName + ' - CD ' + discNumber)
                        horizontalAlignment: Text.AlignLeft

                        font.weight: Font.Light
                        font.italic: true
                        color: myPalette.text

                        Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                        Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.fillWidth: true
                        Layout.bottomMargin: elisaTheme.layoutVerticalMargin / 2

                        elide: Text.ElideRight
                    }
                }

                ToolButton {
                    id: enqueueButton

                    Layout.preferredHeight: elisaTheme.delegateHeight
                    Layout.preferredWidth: elisaTheme.delegateHeight

                    opacity: 0
                    visible: opacity > 0.1
                    action: enqueue
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                }

                ToolButton {
                    id: clearAndEnqueueButton

                    Layout.preferredHeight: elisaTheme.delegateHeight
                    Layout.preferredWidth: elisaTheme.delegateHeight

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

                    elide: Text.ElideRight

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
            when: !hoverArea.containsMouse && !mediaTrack.activeFocus
            PropertyChanges {
                target: clearAndEnqueueButton
                opacity: 0
            }
            PropertyChanges {
                target: enqueueButton
                opacity: 0
            }
            PropertyChanges {
                target: rowRoot
                color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)
            }
        },
        State {
            name: 'hoveredOrSelected'
            when: hoverArea.containsMouse || mediaTrack.activeFocus
            PropertyChanges {
                target: clearAndEnqueueButton
                opacity: 1
            }
            PropertyChanges {
                target: enqueueButton
                opacity: 1
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
                properties: "opacity"
                easing.type: Easing.InOutQuad
                duration: 250
            }
            ColorAnimation {
                properties: "color"
                duration: 250
            }
        }
    }
}
