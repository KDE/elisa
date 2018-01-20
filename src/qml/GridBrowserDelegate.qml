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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import org.kde.elisa 1.0

Item {
    id: gridEntry

    property var imageUrl
    property bool shadowForImage
    property alias mainText: mainLabel.text
    property alias secondaryText: secondaryLabel.text
    property var containerData
    property bool delegateDisplaySecondaryText: true

    signal enqueue(var data)
    signal replaceAndPlay(var data)
    signal open()
    signal selected()

    Action {
        id: enqueueAction

        text: i18nc("Add whole container to play list", "Enqueue")
        iconName: 'media-track-add-amarok'
        onTriggered: enqueue(containerData)
    }

    Action {
        id: openAction

        text: i18nc("Open view of the container", "Open")
        iconName: 'document-open-folder'
        onTriggered: open()
    }

    Action {
        id: replaceAndPlayAction

        text: i18nc("Clear play list and add whole container to play list", "Play Now and Replace Play List")
        iconName: 'media-playback-start'
        onTriggered: replaceAndPlay(containerData)
    }

    Keys.onReturnPressed: open()
    Keys.onEnterPressed: open()

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        MouseArea {
            id: hoverHandle

            hoverEnabled: true
            acceptedButtons: Qt.LeftButton
            focus: true

            Layout.preferredHeight: gridEntry.width * 0.85 + elisaTheme.layoutVerticalMargin * 0.5 + mainLabelSize.height + secondaryLabelSize.height
            Layout.fillWidth: true

            onClicked: {
                hoverHandle.forceActiveFocus()
                gridEntry.selected()
            }

            onDoubleClicked: open()

            TextMetrics {
                id: mainLabelSize
                font: mainLabel.font
                text: mainLabel.text
            }

            TextMetrics {
                id: secondaryLabelSize
                font: secondaryLabel.font
                text: secondaryLabel.text
            }

            ColumnLayout {
                id: mainData

                spacing: 0
                anchors.fill: parent

                Item {
                    Layout.preferredHeight: gridEntry.width * 0.85
                    Layout.preferredWidth: gridEntry.width * 0.85

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
                                id: replaceAndPlayButton

                                action: replaceAndPlayAction

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

                        source: gridEntry.imageUrl

                        asynchronous: true

                        layer.enabled: shadowForImage
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
                    id: mainLabel

                    font.weight: Font.Bold
                    color: myPalette.text

                    horizontalAlignment: Text.AlignLeft

                    Layout.topMargin: elisaTheme.layoutVerticalMargin * 0.5
                    Layout.preferredWidth: gridEntry.width * 0.85
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                    elide: Text.ElideRight
                }

                LabelWithToolTip {
                    id: secondaryLabel

                    font.weight: Font.Light
                    color: myPalette.text

                    horizontalAlignment: Text.AlignLeft

                    Layout.preferredWidth: gridEntry.width * 0.85
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                    visible: delegateDisplaySecondaryText

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
            when: !gridEntry.activeFocus && !hoverHandle.containsMouse
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
            when: gridEntry.activeFocus || hoverHandle.containsMouse
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
                PropertyAction {
                    properties: "active"
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
                PropertyAction {
                    properties: "active"
                }
            }
        }
    ]
}
