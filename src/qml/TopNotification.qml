/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
import QtQuick.Controls 2.2
import QtQml.Models 2.2
import org.kde.elisa 1.0

FocusScope {
    id: topItem

    property var musicManager
    property bool isViewExpanded: false

    property int rowHeight: elisaTheme.delegateHeight * 2

    visible: Layout.preferredHeight > 0

    Rectangle {
        anchors.fill: parent

        color: myPalette.mid
    }

    Component {
        id: highlightBar

        Rectangle {
            width: notificationColumn.width
            height: rowHeight
            color: (topItem.state === 'expanded' ? myPalette.highlight : myPalette.mid)

            Behavior on color {
                ColorAnimation {
                    duration: 300
                }
            }
        }
    }

    Rectangle {
        id: notificationCounter

        anchors {
            top: parent.top
            left: parent.left
            margins: elisaTheme.layoutVerticalMargin
        }

        width: elisaTheme.delegateToolButtonSize
        height: elisaTheme.delegateToolButtonSize

        radius: width / 2

        color: myPalette.window

        opacity: 0.0
        visible: opacity > 0

        z: 3

        ToolButton {
            anchors.centerIn: parent
            text: manager.countNotifications

            onClicked: topItem.isViewExpanded = !topItem.isViewExpanded
        }
    }

    ScrollView {
        id: expandedView

        ScrollBar.vertical.policy: ScrollBar.AlwaysOff

        anchors {
            top: parent.top
            right: parent.right
            left: notificationCounter.right
            bottom: parent.bottom
        }

        opacity: 0
        visible: opacity > 0

        focus: true

        z: 2

        ListView {
            id: notificationColumn

            focus: true

            populate: Transition {
                NumberAnimation { properties: "opacity"; from: 0; to: 1.0; duration: 300 }
            }

            add: Transition {
                NumberAnimation { properties: "opacity"; from: 0; to: 1.0; duration: 300 }
            }

            displaced: Transition {
                NumberAnimation { properties: "x,y"; duration: 300 }
            }

            highlight: highlightBar

            model: DelegateModel {
                model: manager

                delegate: TopNotificationItem {
                    id: currentDelegate

                    focus: true

                    width: ListView.view.width
                    height: rowHeight

                    onEntered: notificationColumn.currentIndex = index

                    onClose: manager.closeNotification(index)

                    onMainButtonClicked: manager.triggerMainButton(index)

                    onSecondaryButtonClicked: manager.triggerSecondaryButton(index)

                    itemMessage: message
                    itemMainButtonText: mainButtonText
                    itemMainButtonIconName: mainButtonIconName
                    itemSecondaryButtonText: secondaryButtonText
                    itemSecondaryButtonIconName: secondaryButtonIconName

                    ListView.onRemove: SequentialAnimation {
                        PropertyAction { target: currentDelegate; property: "ListView.delayRemove"; value: true }
                        ParallelAnimation {
                            NumberAnimation { target: currentDelegate; properties: "height"; from: elisaTheme.delegateHeight * 2; to: 0; duration: 300 }
                            NumberAnimation { target: currentDelegate; properties: "opacity"; from: 1.0; to: 0; duration: 300 }
                        }
                        PropertyAction { target: currentDelegate; property: "ListView.delayRemove"; value: false }
                    }
                }
            }

            function gotoBeginning() {
                anim.from = notificationColumn.contentY;
                anim.to = 0;
                anim.running = true;
            }

            NumberAnimation { id: anim; target: notificationColumn; property: "contentY"; duration: 300 }
        }
    }

    TopNotificationManager {
        id: manager
    }

    Connections {
        target: elisa.musicManager

        onNewNotification:
            manager.addNotification(notification)

        onCloseNotification:
            manager.closeNotificationById(notificationId)
    }

    states: [
        State {
            name: "empty"
            when: manager.countNotifications === 0

            PropertyChanges {
                target: topItem
                Layout.preferredHeight: 0
            }
            PropertyChanges {
                target: notificationCounter
                opacity: 0.0
            }
            PropertyChanges {
                target: notificationCounter
                width: 0
            }
            PropertyChanges {
                target: notificationColumn
                interactive: false
            }
            PropertyChanges {
                target: expandedView
                opacity: 0.0
                ScrollBar.vertical.policy: ScrollBar.AlwaysOff
            }
            StateChangeScript {
                script: notificationColumn.gotoBeginning()
            }
        },
        State {
            name: "oneNotification"
            when: manager.countNotifications === 1

            PropertyChanges {
                target: topItem
                Layout.preferredHeight: elisaTheme.delegateHeight * 2
            }
            PropertyChanges {
                target: notificationCounter
                opacity: 0.0
            }
            PropertyChanges {
                target: notificationCounter
                width: 0
            }
            PropertyChanges {
                target: notificationColumn
                interactive: false
            }
            PropertyChanges {
                target: expandedView
                opacity: 1.0
                ScrollBar.vertical.policy: ScrollBar.AlwaysOff
            }
            StateChangeScript {
                script: notificationColumn.gotoBeginning()
            }
        },
        State {
            name: "multipleNotifications"
            when: manager.countNotifications > 1 && !isViewExpanded

            PropertyChanges {
                target: topItem
                Layout.preferredHeight: elisaTheme.delegateHeight * 2
            }
            PropertyChanges {
                target: notificationCounter
                opacity: 1.0
            }
            PropertyChanges {
                target: notificationCounter
                width: elisaTheme.delegateToolButtonSize
            }
            PropertyChanges {
                target: notificationColumn
                interactive: false
            }
            PropertyChanges {
                target: expandedView
                opacity: 1.0
                ScrollBar.vertical.policy: ScrollBar.AsNeeded
            }
            StateChangeScript {
                script: notificationColumn.gotoBeginning()
            }
        },
        State {
            name: "expanded"
            when: manager.countNotifications > 1 && isViewExpanded

            PropertyChanges {
                target: topItem
                Layout.preferredHeight: elisaTheme.delegateHeight * 4
            }
            PropertyChanges {
                target: notificationCounter
                opacity: 1.0
            }
            PropertyChanges {
                target: notificationCounter
                width: elisaTheme.delegateToolButtonSize
            }
            PropertyChanges {
                target: notificationColumn
                interactive: true
            }
            PropertyChanges {
                target: expandedView
                opacity: 1.0
                ScrollBar.vertical.policy: ScrollBar.AsNeeded
            }
        }
    ]

    transitions: [
        Transition {
            SequentialAnimation {
                PropertyAction {
                    target: expandedView
                    property: "ScrollBar.vertical.policy"
                    value: ScrollBar.AlwaysOff
                }
                ParallelAnimation {
                    NumberAnimation {
                        target: topItem

                        duration: 300

                        property: "Layout.preferredHeight"
                    }
                    NumberAnimation {
                        target: notificationCounter

                        duration: 300

                        property: "opacity"
                    }
                    NumberAnimation {
                        target: notificationCounter

                        duration: 300

                        property: "width"
                    }
                    NumberAnimation {
                        target: expandedView

                        duration: 300

                        property: "opacity"
                    }
                }
            }
        }
    ]
}
