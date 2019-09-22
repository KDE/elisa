/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami

FocusScope {
    id: rootItem

    property bool isSelected
    property alias colorizeIcon: viewIcon.colorizeIcon
    property string title
    property alias image: viewIcon.source
    property int databaseId
    property string secondTitle
    property bool useSecondTitle

    signal clicked()

    Rectangle {
        id: backgroundHighlight

        anchors.fill: parent

        z: 0

        color: "transparent"
    }

    Accessible.role: Accessible.ListItem
    Accessible.description: title
    Accessible.name: title

    MouseArea {
        id: hoverArea

        anchors.fill: parent

        z: 1

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
                text: title
                visible: hoverArea && hoverArea.containsMouse && !nameLabel.visible

                contentItem: Label {
                    text: title
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

            property bool colorizeIcon

            z: 1

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

            layer.enabled: viewIcon.colorizeIcon
            layer.effect:  ColorOverlay {
                color: nameLabel.color
            }
        }

        LabelWithToolTip {
            id: nameLabel

            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: Kirigami.Units.largeSpacing
            anchors.left: viewIcon.right
            anchors.right: parent.right
            anchors.rightMargin: Kirigami.Units.largeSpacing
            verticalAlignment: "AlignVCenter"

            text: title
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
