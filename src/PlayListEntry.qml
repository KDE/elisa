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

import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import org.mgallien.QmlExtension 1.0

FocusScope {
    id: viewAlbumDelegate

    property string title
    property string artist
    property string album
    property string albumArtist
    property var index
    property var itemDecoration
    property alias duration : durationLabel.text
    property int trackNumber
    property int discNumber
    property bool isSingleDiscAlbum
    property alias rating: ratingWidget.starRating
    property int isPlaying
    property bool isSelected
    property bool isValid
    property bool isAlternateColor
    property bool containsMouse
    property bool hasAlbumHeader
    property var playListModel
    property var playListControler
    property var contextMenu
    property alias clearPlayListAction: removeFromPlayList
    property alias playNowAction: playNow

    signal startPlayback()

    Action {
        id: removeFromPlayList
        text: i18nc("Remove current track from play list", "Remove")
        iconName: "list-remove"
        onTriggered: {
            playListModel.removeRows(viewAlbumDelegate.index, 1)
        }
    }

    Action {
        id: playNow
        text: i18nc("Play now current track from play list", "Play Now")
        iconName: "media-playback-start"
        enabled: !isPlaying && isValid
        onTriggered: {
            playListControler.switchTo(viewAlbumDelegate.index)
            viewAlbumDelegate.startPlayback()
        }
    }

    Rectangle {
        id: entryBackground

        anchors.fill: parent

        color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)

        height: (hasAlbumHeader ? elisaTheme.delegateWithHeaderHeight : elisaTheme.delegateHeight)

        focus: true

        ColumnLayout {
            spacing: 0

            anchors.fill: parent
            anchors.leftMargin: elisaTheme.layoutHorizontalMargin
            anchors.rightMargin: elisaTheme.layoutHorizontalMargin
            anchors.topMargin: 0
            anchors.bottomMargin: 1

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: elisaTheme.delegateWithHeaderHeight - elisaTheme.delegateHeight
                Layout.minimumHeight: elisaTheme.delegateWithHeaderHeight - elisaTheme.delegateHeight
                Layout.maximumHeight: elisaTheme.delegateWithHeaderHeight - elisaTheme.delegateHeight

                visible: hasAlbumHeader

                RowLayout {
                    id: headerRow

                    spacing: elisaTheme.layoutHorizontalMargin

                    anchors.fill: parent

                    Image {
                        id: mainIcon

                        source: (isValid ? (viewAlbumDelegate.itemDecoration ? viewAlbumDelegate.itemDecoration : Qt.resolvedUrl(elisaTheme.albumCover)) : Qt.resolvedUrl(elisaTheme.errorIcon))

                        Layout.minimumWidth: headerRow.height - 4
                        Layout.maximumWidth: headerRow.height - 4
                        Layout.preferredWidth: headerRow.height - 4
                        Layout.minimumHeight: headerRow.height - 4
                        Layout.maximumHeight: headerRow.height - 4
                        Layout.preferredHeight: headerRow.height - 4
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                        sourceSize.width: headerRow.height - 4
                        sourceSize.height: parent.height - 4

                        fillMode: Image.PreserveAspectFit
                        asynchronous: true

                        visible: isValid
                    }

                    BrightnessContrast {
                        source: mainIcon

                        cached: true

                        visible: !isValid

                        contrast: -0.9

                        Layout.minimumWidth: headerRow.height - 4
                        Layout.maximumWidth: headerRow.height - 4
                        Layout.preferredWidth: headerRow.height - 4
                        Layout.minimumHeight: headerRow.height - 4
                        Layout.maximumHeight: headerRow.height - 4
                        Layout.preferredHeight: headerRow.height - 4
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        spacing: 0

                        LabelWithToolTip {
                            id: mainLabel
                            text: album

                            font.weight: Font.Bold
                            color: myPalette.text

                            horizontalAlignment: "AlignHCenter"

                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignCenter
                            Layout.topMargin: elisaTheme.layoutVerticalMargin

                            elide: Text.ElideRight
                        }

                        Item {
                            Layout.fillHeight: true
                        }

                        LabelWithToolTip {
                            id: authorLabel

                            text: albumArtist

                            font.weight: Font.Light
                            color: myPalette.text

                            horizontalAlignment: "AlignHCenter"

                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignCenter
                            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

                            elide: Text.ElideRight
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                RowLayout {
                    id: trackRow

                    anchors.fill: parent

                    spacing: elisaTheme.layoutHorizontalMargin

                    LabelWithToolTip {
                        id: mainCompactLabel

                        text: ((discNumber && !isSingleDiscAlbum) ? discNumber + ' - ' + trackNumber : trackNumber) + ' - ' + title

                        font.weight: (isPlaying ? Font.Bold : Font.Normal)
                        color: myPalette.text

                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                        visible: isValid

                        elide: Text.ElideRight
                    }

                    LabelWithToolTip {
                        id: mainInvalidCompactLabel

                        text: title

                        font.weight: Font.Normal
                        color: myPalette.text

                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                        visible: !isValid

                        elide: Text.ElideRight
                    }

                    Item { Layout.fillWidth: true }

                    ToolButton {
                        id: playNowButton

                        implicitHeight: elisaTheme.smallDelegateToolButtonSize
                        implicitWidth: elisaTheme.smallDelegateToolButtonSize

                        opacity: 0

                        visible: opacity > 0.1
                        action: playNow
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }

                    ToolButton {
                        id: removeButton

                        implicitHeight: elisaTheme.smallDelegateToolButtonSize
                        implicitWidth: elisaTheme.smallDelegateToolButtonSize

                        opacity: 0

                        visible: opacity > 0.1
                        action: removeFromPlayList
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }

                    Image {
                        id: playIcon

                        Layout.preferredWidth: parent.height * 1
                        Layout.preferredHeight: parent.height * 1
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                        Layout.maximumWidth: elisaTheme.smallDelegateToolButtonSize
                        Layout.maximumHeight: elisaTheme.smallDelegateToolButtonSize

                        opacity: 0

                        source: (isPlaying == MediaPlayList.IsPlaying ? Qt.resolvedUrl(elisaTheme.playIcon) : Qt.resolvedUrl(elisaTheme.pauseIcon))
                        width: parent.height * 1.
                        height: parent.height * 1.
                        sourceSize.width: parent.height * 1.
                        sourceSize.height: parent.height * 1.
                        fillMode: Image.PreserveAspectFit
                        mirror: LayoutMirroring.enabled
                        visible: isPlaying == MediaPlayList.IsPlaying || isPlaying == MediaPlayList.IsPaused

                        SequentialAnimation on opacity {
                            running: isPlaying == MediaPlayList.IsPlaying
                            loops: Animation.Infinite

                            NumberAnimation {
                                from: 0
                                to: 1.
                                duration: 1000
                                easing.type: Easing.InOutCubic
                            }
                            NumberAnimation {
                                from: 1
                                to: 0
                                duration: 1000
                                easing.type: Easing.InOutCubic
                            }
                        }

                        SequentialAnimation on opacity {
                            running: isPlaying == MediaPlayList.IsPaused

                            NumberAnimation {
                                from: playIcon.opacity
                                to: 1.
                                duration: 1000
                                easing.type: Easing.InOutCubic
                            }
                        }
                    }

                    RatingStar {
                        id: ratingWidget

                        starSize: elisaTheme.ratingStarSize
                    }

                    LabelWithToolTip {
                        id: durationLabel
                        text: duration

                        color: myPalette.text

                        elide: Text.ElideRight
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }
                }
            }
        }
    }

    states: [
        State {
            name: 'notSelected'
            when: !containsMouse && (!viewAlbumDelegate.activeFocus || !isSelected)
            PropertyChanges {
                target: viewAlbumDelegate
                height: (hasAlbumHeader ? elisaTheme.delegateWithHeaderHeight : elisaTheme.delegateHeight)
            }
            PropertyChanges {
                target: removeButton
                opacity: 0
            }
            PropertyChanges {
                target: playNowButton
                opacity: 0
            }
            PropertyChanges {
                target: entryBackground
                color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)
            }
        },
        State {
            name: 'hoveredOrSelected'
            when: containsMouse || (viewAlbumDelegate.activeFocus && isSelected)
            PropertyChanges {
                target: viewAlbumDelegate
                height: (hasAlbumHeader ? elisaTheme.delegateWithHeaderHeight : elisaTheme.delegateHeight)
            }
            PropertyChanges {
                target: removeButton
                opacity: 1
            }
            PropertyChanges {
                target: playNowButton
                opacity: 1
            }
            PropertyChanges {
                target: entryBackground
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

