/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 1.0

Item {
    id: button

    //either Action or QAction should work here
    property QtObject action: pageStack.currentItem ? pageStack.currentItem.mainAction : null

    implicitWidth: implicitHeight
    implicitHeight: Units.iconSizes.large
    visible: action != null


    onXChanged: {
        if (mouseArea.pressed) {
            if (globalDrawer && globalDrawer.enabled) {
                globalDrawer.position = Math.min(1, Math.max(0, (x - button.parent.width/2 + button.width/2)/globalDrawer.contentItem.width));
            }
            if (contextDrawer && contextDrawer.enabled) {
                contextDrawer.position = Math.min(1, Math.max(0, (button.parent.width/2 - button.width/2 - x)/contextDrawer.contentItem.width));
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        property bool internalVisibility: (applicationWindow === undefined || applicationWindow().controlsVisible) && (button.action === null || button.action.visible === undefined || button.action.visible)
        onInternalVisibilityChanged: {
            showAnimation.running = false;
            if (internalVisibility) {
                showAnimation.to = 0;
            } else {
                showAnimation.to = button.height;
            }
            showAnimation.running = true;
        }

        drag {
            target: button
            axis: Drag.XAxis
            minimumX: contextDrawer.enabled ? 0 : button.parent.width/2 - button.width/2
            maximumX: globalDrawer.enabled ? button.parent.width : button.parent.width/2 - button.width/2
        }

        transform: Translate {
            id: translateTransform
        }
        property var downTimestamp;
        property int startX
        property bool buttonPressedUnderMouse: false

        onPressed: {
            downTimestamp = (new Date()).getTime();
            startX = button.x + button.width/2;
            buttonPressedUnderMouse = mouse.x > buttonGraphics.x - buttonGraphics.width / 2 && mouse.x < buttonGraphics.x + buttonGraphics.width / 2;
        }
        onReleased: {
            //pixel/second
            var x = button.x + button.width/2;
            var speed = ((x - startX) / ((new Date()).getTime() - downTimestamp) * 1000);

            //project where it would be a full second in the future
            if (globalDrawer && x + speed > Math.min(button.parent.width/4*3, button.parent.width/2 + globalDrawer.contentItem.width/2)) {
                globalDrawer.open();
                contextDrawer.close();
            } else if (contextDrawer && x + speed < Math.max(button.parent.width/4, button.parent.width/2 - contextDrawer.contentItem.width/2)) {print("222")
                if (contextDrawer) {
                    contextDrawer.open();
                }
                if (globalDrawer) {
                    globalDrawer.close();
                }
            } else {
                if (globalDrawer) {
                    globalDrawer.close();
                }
                if (contextDrawer) {
                    contextDrawer.close();
                }
            }
        }
        onClicked: {
            if (!buttonPressedUnderMouse || !button.action) {
                return;
            }
            if (button.action.checkable) {
                checked = !checked;
            }

            //if an action has been assigned, trigger it
            if (button.action && button.action.trigger) {
                button.action.trigger();
            }
        }
        Connections {
            target: globalDrawer
            onPositionChanged: {
                if (!mouseArea.pressed) {
                    button.x = globalDrawer.contentItem.width * globalDrawer.position + button.parent.width/2 - button.width/2;
                }
            }
        }
        Connections {
            target: contextDrawer
            onPositionChanged: {
                if (!mouseArea.pressed) {
                    button.x = button.parent.width/2 - button.width/2 - contextDrawer.contentItem.width * contextDrawer.position;
                }
            }
        }
        Connections {
            target: button.parent
            onWidthChanged: button.x = button.parent.width/2 - button.width/2
        }

        NumberAnimation {
            id: showAnimation
            target: translateTransform
            properties: "y"
            duration: Units.longDuration
            easing.type: Easing.InOutQuad
        }
        Item {
            id: background
            anchors {
                fill: parent
                leftMargin: -Units.gridUnit
                rightMargin: -Units.gridUnit
            }

            Rectangle {
                id: buttonGraphics
                radius: width/2
                anchors.centerIn: parent
                height: parent.height - Units.smallSpacing*2
                width: height
                color: (button.action && mouseArea.buttonPressedUnderMouse && mouseArea.pressed) || button.checked ? Theme.highlightColor : Theme.backgroundColor
                Icon {
                    id: icon
                    source: button.action && button.action.iconName ? button.action.iconName : ""
                    anchors {
                        fill: parent
                        margins: Units.smallSpacing
                    }
                }
                Behavior on color {
                    ColorAnimation {
                        duration: Units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                }
                Behavior on x {
                    NumberAnimation {
                        duration: Units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }
        DropShadow {
            anchors.fill: background
            horizontalOffset: 0
            verticalOffset: Units.smallSpacing/3
            radius: Units.gridUnit / 3.5
            samples: 16
            color: mouseArea.pressed && button.action && mouseArea.buttonPressedUnderMouse ? "transparent" : Qt.rgba(0, 0, 0, 0.5)
            source: background
        }
    }
}

