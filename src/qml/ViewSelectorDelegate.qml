/*
 * Copyright 2016-2019 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami

FocusScope {
    id: rootItem

    property var viewType: model.type
    property bool isSelected

    signal clicked()

    Rectangle {
        id: backgroundHighlight

        anchors.fill: parent
        z: 1

        color: "transparent"
    }

    Accessible.role: Accessible.ListItem
    Accessible.description: model.display
    Accessible.name: model.display

    MouseArea {
        id: hoverArea

        anchors.fill: parent
        z: 2

        hoverEnabled: true
        acceptedButtons: Qt.LeftButton

        onClicked: {
            rootItem.clicked()
        }

        Loader {
            id: hoverLoader

            anchors.fill: parent
            active: false

            sourceComponent: ToolTip {
                delay: Qt.styleHints.mousePressAndHoldInterval
                text: model.display
                visible: hoverArea && hoverArea.containsMouse && !nameLabel.visible

                contentItem: Label {
                    text: model.display
                    color: myPalette.highlightedText
                }

                enter: Transition { NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; from: 0.0; to: 1.0; duration: Kirigami.Units.longDuration; } }
                exit: Transition { NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; from: 1.0; to: 0.0; duration: Kirigami.Units.longDuration; } }

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
                leftMargin: Kirigami.Units.largeSpacing
                left: parent.left
            }

            height: Kirigami.Units.iconSizes.smallMedium
            width: Kirigami.Units.iconSizes.smallMedium

            sourceSize {
                width: Kirigami.Units.iconSizes.smallMedium
                height: Kirigami.Units.iconSizes.smallMedium
            }

            source: model.image

            layer.enabled: true
            layer.effect:  ColorOverlay {
                color: nameLabel.color
            }
        }

        LabelWithToolTip {
            id: nameLabel

            z: 2

            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: Kirigami.Units.largeSpacing
            anchors.left: viewIcon.right
            anchors.right: parent.right
            anchors.rightMargin: Kirigami.Units.largeSpacing
            verticalAlignment: "AlignVCenter"

            text: model.display
            elide: Text.ElideRight

            opacity: textOpacity
            visible: opacity > 0

            color: (viewModeView.currentIndex === index || hoverArea.containsMouse ? myPalette.highlight : myPalette.text)
        }
    }

    states: [
        State {
            name: 'notSelected'
            when: !rootItem.activeFocus && !hoverArea.containsMouse && !rootItem.isSelected
            PropertyChanges {
                target: hoverLoader
                active: false
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 0.0
            }
            PropertyChanges {
                target: viewIcon
                opacity: 1
            }
            PropertyChanges {
                target: nameLabel
                color: myPalette.buttonText
            }
            PropertyChanges {
                target: backgroundHighlight
                color: 'transparent'
            }
        },
        State {
            name: 'hovered'
            when: !rootItem.activeFocus && hoverArea.containsMouse
            PropertyChanges {
                target: hoverLoader
                active: true
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 1.0
            }
            PropertyChanges {
                target: viewIcon
                opacity: 0.4
            }
            PropertyChanges {
                target: nameLabel
                color: myPalette.buttonText
            }
            PropertyChanges {
                target: backgroundHighlight
                color: Qt.rgba(myPalette.highlight.r, myPalette.highlight.g, myPalette.highlight.b, 0.2)
            }
        },
        State {
            name: 'selected'
            when: !rootItem.activeFocus && rootItem.isSelected
            PropertyChanges {
                target: hoverLoader
                active: false
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 0.0
            }
            PropertyChanges {
                target: viewIcon
                opacity: 1
            }
            PropertyChanges {
                target: nameLabel
                color: myPalette.buttonText
            }
            PropertyChanges {
                target: backgroundHighlight
                color: myPalette.mid
            }
        },
        State {
            name: 'focused'
            when: rootItem.activeFocus
            PropertyChanges {
                target: hoverLoader
                active: false
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 0.0
            }
            PropertyChanges {
                target: viewIcon
                opacity: 1.
            }
            PropertyChanges {
                target: nameLabel
                color: myPalette.highlightedText
            }
            PropertyChanges {
                target: backgroundHighlight
                color: myPalette.highlight
            }
        }
    ]
}
