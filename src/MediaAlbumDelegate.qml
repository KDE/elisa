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

import org.kde.elisa 1.0

FocusScope {
    id: mediaServerEntry

    property StackView stackView
    property MediaPlayList playListModel
    property var musicListener
    property var playerControl
    property var image
    property alias title: titleLabel.text
    property alias artist: artistLabel.text
    property string trackNumber
    property bool isSingleDiscAlbum
    property var albumData
    property var albumId

    signal showArtist(var name)
    signal albumClicked()

    Action {
        id: enqueueAction

        text: i18nc("Add whole album to play list", "Enqueue")
        iconName: 'media-track-add-amarok'
        onTriggered: mediaServerEntry.playListModel.enqueue(mediaServerEntry.albumData)
    }

    Action {
        id: openAction

        text: i18nc("Open album view", "Open Album")
        iconName: 'document-open-folder'
        onTriggered: showAlbumTracks()
    }

    Action {
        id: enqueueAndPlayAction

        text: i18nc("Clear play list and add whole album to play list", "Play Now and Replace Play List")
        iconName: 'media-playback-start'
        onTriggered: {
            mediaServerEntry.playListModel.clearAndEnqueue(mediaServerEntry.albumData)
            mediaServerEntry.playerControl.ensurePlay()
        }
    }

    Keys.onReturnPressed: showAlbumTracks()
    Keys.onEnterPressed: showAlbumTracks()

    Component {
        id: albumViewComponent

        MediaAlbumView {
            stackView: mediaServerEntry.stackView
            playListModel: mediaServerEntry.playListModel
            musicListener: mediaServerEntry.musicListener
            playerControl: mediaServerEntry.playerControl
            albumArtUrl: image
            albumName: title
            artistName: artist
            tracksCount: count
            isSingleDiscAlbum: mediaServerEntry.isSingleDiscAlbum
            albumData: mediaServerEntry.albumData
            albumId: mediaServerEntry.albumId

            onShowArtist: mediaServerEntry.showArtist(name)
        }
    }

    function showAlbumTracks() {
        stackView.push(albumViewComponent)
    }

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        MouseArea {
            id: hoverHandle

            hoverEnabled: true
            acceptedButtons: Qt.LeftButton
            focus: true

            Layout.preferredHeight: mediaServerEntry.width * 0.85 + elisaTheme.layoutVerticalMargin * 0.5 + titleSize.height + artistSize.height
            Layout.fillWidth: true

            onClicked: {
                hoverHandle.forceActiveFocus()
                albumClicked()
            }

            onDoubleClicked: showAlbumTracks()

            TextMetrics {
                id: titleSize
                font: titleLabel.font
                text: titleLabel.text
            }

            TextMetrics {
                id: artistSize
                font: artistLabel.font
                text: artistLabel.text
            }

            ColumnLayout {
                id: mainData

                spacing: 0
                anchors.fill: parent

                Item {
                    Layout.preferredHeight: mediaServerEntry.width * 0.85
                    Layout.preferredWidth: mediaServerEntry.width * 0.85

                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    focus: true

                    Loader {
                        id: hoverLoader
                        active: false

                        anchors.centerIn: parent
                        z: 1

                        opacity: 0

                        sourceComponent: Row {

                            ToolButton {
                                id: enqueueButton

                                action: enqueueAction

                                width: elisaTheme.delegateToolButtonSize
                                height: elisaTheme.delegateToolButtonSize
                            }

                            ToolButton {
                                id: openButton

                                action: openAction

                                width: elisaTheme.delegateToolButtonSize
                                height: elisaTheme.delegateToolButtonSize
                            }

                            ToolButton {
                                id: enqueueAndPlayButton

                                action: enqueueAndPlayAction

                                width: elisaTheme.delegateToolButtonSize
                                height: elisaTheme.delegateToolButtonSize
                            }
                        }
                    }

                    Image {
                        id: coverImage

                        anchors.fill: parent

                        sourceSize.width: parent.width
                        sourceSize.height: parent.height
                        fillMode: Image.PreserveAspectFit
                        smooth: true

                        source: (mediaServerEntry.image ? mediaServerEntry.image : Qt.resolvedUrl(elisaTheme.albumCover))

                        asynchronous: true

                        layer.enabled: image === '' ? false : true
                        layer.effect: DropShadow {
                            source: coverImage

                            radius: 10
                            spread: 0.1
                            samples: 21

                            color: myPalette.shadow
                        }
                    }
                }

                LabelWithToolTip {
                    id: titleLabel

                    font.weight: Font.Bold
                    color: myPalette.text

                    horizontalAlignment: Text.AlignLeft

                    Layout.topMargin: elisaTheme.layoutVerticalMargin * 0.5
                    Layout.preferredWidth: mediaServerEntry.width * 0.85
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                    elide: Text.ElideRight
                }

                LabelWithToolTip {
                    id: artistLabel

                    font.weight: Font.Light
                    color: myPalette.text

                    horizontalAlignment: Text.AlignLeft

                    Layout.preferredWidth: mediaServerEntry.width * 0.85
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                    elide: Text.ElideRight
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }

    states: [
        State {
            name: 'notSelected'
            when: !mediaServerEntry.activeFocus && !hoverHandle.containsMouse
            PropertyChanges {
                target: hoverLoader
                active: false
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 0.0
            }
            PropertyChanges {
                target: coverImage
                opacity: 1
            }
        },
        State {
            name: 'hoveredOrSelected'
            when: mediaServerEntry.activeFocus || hoverHandle.containsMouse
            PropertyChanges {
                target: hoverLoader
                active: true
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 1.0
            }
            PropertyChanges {
                target: coverImage
                opacity: 0.2
            }
        }
    ]

    transitions: [
        Transition {
            to: 'hoveredOrSelected'
            SequentialAnimation {
                NumberAnimation {
                    properties: "active"
                    duration: 0
                }
                NumberAnimation {
                    properties: "opacity"
                    easing.type: Easing.InOutQuad
                    duration: 100
                }
            }
        },
        Transition {
            to: 'notSelected'
            SequentialAnimation {
                NumberAnimation {
                    properties: "opacity"
                    easing.type: Easing.InOutQuad
                    duration: 100
                }
                NumberAnimation {
                    properties: "active"
                    duration: 0
                }
            }
        }
    ]
}
