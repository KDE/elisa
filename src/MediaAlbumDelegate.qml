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

Rectangle {
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

    id: mediaServerEntry

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
        onTriggered: {
            stackView.push(Qt.resolvedUrl("MediaAlbumView.qml"),
                           {
                               'stackView': stackView,
                               'playListModel': playListModel,
                               'musicListener': musicListener,
                               'playerControl': playerControl,
                               'albumArtUrl': image,
                               'albumName': title,
                               'artistName': artist,
                               'tracksCount': count,
                               'isSingleDiscAlbum': mediaServerEntry.isSingleDiscAlbum,
                               'albumData': mediaServerEntry.albumData,
                               'albumId': mediaServerEntry.albumId
                           })
        }
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

    color: myPalette.base

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        MouseArea {
            id: hoverHandle

            hoverEnabled: true

            Layout.preferredHeight: mediaServerEntry.width * 0.9 + elisaTheme.layoutVerticalMargin * 0.5 + titleSize.height + artistSize.height
            Layout.fillWidth: true

            onEntered: hoverLoader.active = true
            onExited: hoverLoader.active = false

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

            Loader {
                id: hoverLoader
                active: false

                z: 2

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: mediaServerEntry.width * 0.9 + elisaTheme.layoutVerticalMargin

                sourceComponent: Item {
                    GaussianBlur {
                        id: hoverLayer

                        radius: 4
                        samples: 16
                        deviation: 5

                        anchors.fill: parent

                        source: ShaderEffectSource {
                            sourceItem: mainData
                            sourceRect: Qt.rect(0, 0, hoverLayer.width, hoverLayer.height)
                        }

                        Rectangle {
                            color: myPalette.light
                            opacity: 0.5
                            anchors.fill: parent
                        }
                    }

                    Row {
                        anchors.centerIn: parent

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
            }

            ColumnLayout {
                id: mainData

                spacing: 0
                anchors.fill: parent

                z: 1

                Item {
                    Layout.preferredHeight: mediaServerEntry.width * 0.9
                    Layout.preferredWidth: mediaServerEntry.width * 0.9

                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    Image {
                        id: coverImage

                        anchors.fill: parent

                        sourceSize.width: mediaServerEntry.width * 0.9
                        sourceSize.height: mediaServerEntry.width * 0.9
                        fillMode: Image.PreserveAspectFit
                        smooth: true

                        source: (mediaServerEntry.image ? mediaServerEntry.image : Qt.resolvedUrl(elisaTheme.albumCover))

                        asynchronous: true

                        layer.enabled: true
                        layer.effect: DropShadow {
                            horizontalOffset: mediaServerEntry.width * 0.02
                            verticalOffset: mediaServerEntry.width * 0.02

                            source: coverImage

                            radius: 5.0
                            samples: 11

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
                    Layout.preferredWidth: mediaServerEntry.width * 0.9
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                    elide: Text.ElideRight
                }

                LabelWithToolTip {
                    id: artistLabel

                    font.weight: Font.Normal
                    color: myPalette.text

                    horizontalAlignment: Text.AlignLeft

                    Layout.preferredWidth: mediaServerEntry.width * 0.9
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                    elide: Text.ElideRight
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
