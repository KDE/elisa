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
import QtQuick.Controls 2.2
import QtQml.Models 2.2
import QtGraphicalEffects 1.0

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

                        hoverEnabled: true
                        acceptedButtons: Qt.LeftButton

                        Rectangle {
                            anchors.fill: parent

                            z: 1

                            color: ((item.containsMouse && index !== viewModeView.currentIndex) ? myPalette.mid : "transparent")

                            Behavior on color {
                                ColorAnimation {
                                    duration: 200
                                }
                            }
                        }

                        Image {
                            id: viewIcon

                            z: 2

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

                            visible: false
                        }

                        ColorOverlay {
                            source: viewIcon

                            z: 2

                            anchors {
                                verticalCenter: parent.verticalCenter
                                leftMargin: elisaTheme.layoutHorizontalMargin
                                left: parent.left
                            }

                            height: elisaTheme.viewSelectorDelegateHeight
                            width: elisaTheme.viewSelectorDelegateHeight

                            color: (index === viewModeView.currentIndex ? myPalette.highlightedText : "transparent")

                            Behavior on color {
                                ColorAnimation {
                                    duration: 300
                                }
                            }
                        }

                        LabelWithToolTip {
                            id: nameLabel

                            z: 2

                            anchors.verticalCenter: parent.verticalCenter
                            anchors.leftMargin: elisaTheme.layoutHorizontalMargin
                            anchors.left: viewIcon.right
                            anchors.right: parent.right
                            anchors.rightMargin: elisaTheme.layoutHorizontalMargin
                            verticalAlignment: "AlignVCenter"

                            font.pointSize: elisaTheme.defaultFontPointSize * 1.4

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
                        pageModel.insert(0, {"name": i18nc("Title of the view of the playlist", "Now Playing"), "iconName": elisaTheme.playlistIcon})
                        pageModel.insert(1, {"name": i18nc("Title of the view of all albums", "Albums"), "iconName": elisaTheme.albumIcon})
                        pageModel.insert(2, {"name": i18nc("Title of the view of all artists", "Artists"), "iconName": elisaTheme.artistIcon})
                        pageModel.insert(3, {"name": i18nc("Title of the view of all tracks", "Tracks"), "iconName": elisaTheme.tracksIcon})

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
