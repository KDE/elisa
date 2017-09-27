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

import QtQuick 2.7
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQml.Models 2.1

FocusScope {
    id: rootFocusScope
    property alias currentIndex: viewModeView.currentIndex

    Rectangle {
        anchors.fill: parent

        color: myPalette.window
        border {
            color: (rootFocusScope.activeFocus ? myPalette.highlight : myPalette.window)
            width: 1
        }

        ScrollView {
            flickableItem.boundsBehavior: Flickable.StopAtBounds

            focus: true

            anchors.fill: parent

            ListView {
                id: viewModeView

                focus: true
                z: 2

                highlight: Rectangle {
                    id: item

                    height: elisaTheme.viewSelectorDelegateHeight * 1.4
                    width: viewModeView.width
                    color: myPalette.highlight
                }

                model: DelegateModel {
                    id: pageDelegateModel

                    model: ListModel {
                        id: pageModel
                    }

                    delegate: MouseArea {
                        id: item

                        height: elisaTheme.viewSelectorDelegateHeight * 1.4
                        width: viewModeView.width


                        acceptedButtons: Qt.LeftButton

                        Image {
                            id: viewIcon

                            anchors {
                                verticalCenter: parent.verticalCenter
                                leftMargin: elisaTheme.layoutHorizontalMargin
                                left: parent.left
                            }

                            height: elisaTheme.viewSelectorDelegateHeight
                            width: elisaTheme.viewSelectorDelegateHeight

                            sourceSize {
                                width: elisaTheme.viewSelectorDelegateHeight
                                height: elisaTheme.viewSelectorDelegateHeight
                            }

                            source: iconName
                        }

                        LabelWithToolTip {
                            id: nameLabel
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.leftMargin: elisaTheme.layoutHorizontalMargin
                            anchors.left: viewIcon.right
                            anchors.right: parent.right
                            anchors.rightMargin: elisaTheme.layoutHorizontalMargin
                            verticalAlignment: "AlignVCenter"

                            font.pixelSize: elisaTheme.defaultFontPixelSize * 1.4

                            text: model.name

                            color: (viewModeView.currentIndex === index ? myPalette.highlightedText : myPalette.text)

                            Behavior on color {
                                ColorAnimation {
                                    duration: 300
                                }
                            }
                        }

                        onClicked:
                        {
                            viewModeView.currentIndex = index
                            rootFocusScope.focus = true
                        }
                    }

                    Component.onCompleted:
                    {
                        pageModel.insert(0, {"name": i18nc("Title of the view of the playlist", "Now Playing"), "iconName": "image://icon/amarok_playlist"})
                        pageModel.insert(1, {"name": i18nc("Title of the view of all albums", "Albums"), "iconName": "image://icon/media-album-cover"})
                        pageModel.insert(2, {"name": i18nc("Title of the view of all artists", "Artists"), "iconName": "image://icon/view-media-artist"})
                        pageModel.insert(3, {"name": i18nc("Title of the view of all tracks", "Tracks"), "iconName": "image://icon/media-album-track"})

                        viewModeView.currentIndex = 1
                    }
                }

                footer: MouseArea {
                    width: viewModeView.width
                    height: viewModeView.height - y

                    acceptedButtons: Qt.LeftButton

                    onClicked:
                    {
                        rootFocusScope.focus = true
                    }
                }
            }
        }

        Behavior on border.color {
            ColorAnimation {
                duration: 300
            }
        }
    }
}
