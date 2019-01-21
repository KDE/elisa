/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQml.Models 2.2
import QtGraphicalEffects 1.0

import org.kde.elisa 1.0

FocusScope {
    id: rootFocusScope

    readonly property alias currentIndex: viewModeView.currentIndex
    property double textOpacity
    property double maximumSize
    property alias model: pageDelegateModel.model

    signal switchView(var viewType)

    function setCurrentIndex(index)
    {
        viewModeView.ignoreCurrentItemChanges = true
        viewModeView.currentIndex = index
        viewModeView.ignoreCurrentItemChanges = false
    }

    implicitWidth: elisaTheme.dp(500)

    Rectangle {
        anchors.fill: parent

        color: myPalette.base
        border {
            color: (rootFocusScope.activeFocus ? myPalette.highlight : "transparent")
            width: 1
        }

        ScrollView {
            focus: true

            anchors.fill: parent

            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff


            ListView {
                id: viewModeView

                focus: true
                activeFocusOnTab: true
                keyNavigationEnabled: true

                property bool ignoreCurrentItemChanges: false

                z: 2

                anchors.topMargin: elisaTheme.layoutHorizontalMargin * 2

                model: DelegateModel {
                    id: pageDelegateModel

                    delegate: MouseArea {
                        id: itemMouseArea

                        property var viewType: model.type

                        height: elisaTheme.viewSelectorDelegateHeight * 1.4
                        width: viewModeView.width

                        hoverEnabled: true
                        acceptedButtons: Qt.LeftButton

                        Loader {
                            anchors.fill: parent
                            active: itemMouseArea && itemMouseArea.containsMouse && !nameLabel.visible

                            sourceComponent: ToolTip {
                                delay: Qt.styleHints.mousePressAndHoldInterval
                                text: model.display
                                visible: itemMouseArea && itemMouseArea.containsMouse && !nameLabel.visible

                                contentItem: Label {
                                    text: model.display
                                    color: myPalette.highlightedText
                                }

                                enter: Transition { NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; from: 0.0; to: 1.0; duration: 300; } }
                                exit: Transition { NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; from: 1.0; to: 0.0; duration: 300; } }

                                background: Rectangle {
                                    color: myPalette.shadow
                                    radius: elisaTheme.tooltipRadius

                                    layer.enabled: true
                                    layer.effect: DropShadow {
                                        horizontalOffset: elisaTheme.shadowOffset
                                        verticalOffset: elisaTheme.shadowOffset
                                        radius: 8
                                        samples: 17
                                        color: myPalette.shadow
                                    }
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

                            source: model.image

                            layer.effect:  ColorOverlay {

                                color: (index === viewModeView.currentIndex || itemMouseArea.containsMouse ? myPalette.highlight : "transparent")

                                Behavior on color {
                                    ColorAnimation {
                                        duration: 300
                                    }
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

                            text: model.display
                            elide: Text.ElideRight

                            opacity: textOpacity
                            visible: opacity > 0

                            Behavior on opacity {
                                NumberAnimation {
                                    duration: 150
                                }
                            }

                            color: (viewModeView.currentIndex === index || itemMouseArea.containsMouse ? myPalette.highlight : myPalette.text)

                            Behavior on color {
                                ColorAnimation {
                                    duration: 300
                                }
                            }
                        }

                        onClicked: viewModeView.currentIndex = index
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

                onCurrentItemChanged: if (!ignoreCurrentItemChanges) switchView(currentItem.viewType)
            }
        }

        Behavior on border.color {
            ColorAnimation {
                duration: 300
            }
        }
    }

    Connections {
        target: elisa

        onInitializationDone: {
            viewModeView.currentIndex = 3
        }
    }

    Behavior on implicitWidth {
        NumberAnimation {
            duration: 150
        }
    }

    Behavior on width {
        NumberAnimation {
            duration: 150
        }
    }

    states: [
        State {
            name: 'iconsAndText'
            when: maximumSize >= elisaTheme.viewSelectorSmallSizeThreshold
            PropertyChanges {
                target: rootFocusScope
                textOpacity: 1
                implicitWidth: elisaTheme.dp(500)
            }
        },
        State {
            name: 'iconsOnly'
            when: maximumSize < elisaTheme.viewSelectorSmallSizeThreshold
            PropertyChanges {
                target: rootFocusScope
                textOpacity: 0
                implicitWidth: elisaTheme.viewSelectorDelegateHeight + 2 * elisaTheme.layoutHorizontalMargin
            }
        }
    ]
}
