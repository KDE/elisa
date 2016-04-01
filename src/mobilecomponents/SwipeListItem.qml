/*
 *   Copyright 2010 Marco Martin <notmart@gmail.com>
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

import QtQuick 2.5
import QtQuick.Layouts 1.2
import org.kde.kirigami 1.0
import QtGraphicalEffects 1.0

/**
 * An item delegate Intended to support extra actions obtainable
 * by uncovering them by dragging away the item with the handle
 * This acts as a container for normal list items.
 * Any subclass of AbstractListItem can be assigned as the contentItem property.
 * @code
 * ListView {
 *     model: myModel
 *     delegate: ActionsForListItem {
 *         BasicListItem {
 *             label: model.text
 *         }
 *         actions: [
 *              Action {
 *                  iconName: "document-decrypt"
 *                  onTriggered: print("Action 1 clicked")
 *              },
 *              Action {
 *                  iconName: model.action2Icon
 *                  onTriggered: //do something
 *              }
 *         ]
 *     }
 * 
 * }
 * @endcode
 *
 * @inherit QtQuick.Item
 */
Item {
    id: listItem

//BEGIN properties
    /**
     * type: Item
     * This property holds the visual content item.
     *
     * Note: The content item is automatically resized inside the
     * padding of the control.
     */
     default property Item contentItem

    /**
     * type: bool
     * Holds if the item emits signals related to mouse interaction.
     *TODO: remove
     * The default value is false.
     */
    property alias supportsMouseEvents: itemMouse.enabled

    /**
     * type: signal
     * This signal is emitted when there is a click.
     *
     * This is disabled by default, set enabled to true to use it.
     * @see enabled
     */
    signal clicked


    /**
     * type: signal
     * The user pressed the item with the mouse and didn't release it for a
     * certain amount of time.
     *
     * This is disabled by default, set enabled to true to use it.
     * @see enabled
     */
    signal pressAndHold

    /**
     * type: bool
     * If true makes the list item look as checked or pressed. It has to be set
     * from the code, it won't change by itself.
     */
    property bool checked: false

    /**
     * type: bool
     * If true the item will be a delegate for a section, so will look like a
     * "title" for the items under it.
     */
    property bool sectionDelegate: false

    /**
     * type: bool
     * True if the separator between items is visible
     * default: true
     */
    property bool separatorVisible: true

    /**
     * type: list<Action>
     * Defines the actions for the list item: at most 4 buttons will
     * contain the actions for the item, that can be revealed by
     * sliding away the list item.
     */
    property list<Action> actions


    /**
     * position: real
     * This property holds the position of the dragged list item relative to its
     * final destination (just like the Drawer). That is, the position
     * will be 0 when the list item is fully closed, and 1 when fully open.
     */
    property real position: 0

    /**
     * type: Item
     * This property holds the background item.
     *
     * Note: If the background item has no explicit size specified,
     * it automatically follows the control's size.
     * In most cases, there is no need to specify width or
     * height for a background item.
     */
    property Item background: Rectangle {
        color: listItem.checked || itemMouse.pressed ? Theme.highlightColor : Theme.viewBackgroundColor

        parent: itemMouse
        anchors.fill: parent
        visible: listItem.ListView.view ? listItem.ListView.view.highlight === null : true
        opacity: itemMouse.containsMouse && !itemMouse.pressed ? 0.5 : 1
        Behavior on color {
            ColorAnimation { duration: Units.longDuration }
        }
        Behavior on opacity { NumberAnimation { duration: Units.longDuration } }

        Rectangle {
            id: separator
            color: Theme.textColor
            opacity: 0.2
            visible: listItem.separatorVisible
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: Math.round(Units.smallSpacing / 3);
        }
    }

    Item {
        id: behindItem
        parent: listItem
        anchors {
            fill: parent
            leftMargin: height
        }
        Rectangle {
            id: shadowHolder
            color: Theme.backgroundColor
            anchors.fill: parent
        }
        LinearGradient {
            height: Units.gridUnit/2
            anchors {
                right: parent.right
                left: parent.left
                top: parent.top
            }

            start: Qt.point(0, 0)
            end: Qt.point(0, Units.gridUnit/2)
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: Qt.rgba(0, 0, 0, 0.2)
                }
                GradientStop {
                    position: 0.3
                    color: Qt.rgba(0, 0, 0, 0.1)
                }
                GradientStop {
                    position: 1.0
                    color:  "transparent"
                }
            }
        }
        LinearGradient {
            width: Units.gridUnit/2
            x: behindItem.width - (behindItem.width * listItem.position)
            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            start: Qt.point(0, 0)
            end: Qt.point(Units.gridUnit/2, 0)
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: Qt.rgba(0, 0, 0, 0.2)
                }
                GradientStop {
                    position: 0.3
                    color: Qt.rgba(0, 0, 0, 0.1)
                }
                GradientStop {
                    position: 1.0
                    color:  "transparent"
                }
            }
        }
    }

    implicitWidth: parent ? parent.width : contentItem.width + paddingItem.anchors.margins * 2
    implicitHeight: contentItem.implicitHeight + paddingItem.anchors.margins * 2
    height: visible ? implicitHeight : 0
//END properties

//BEGIN signal handlers
    onBackgroundChanged: {
        background.parent = itemMouse;
        background.anchors.fill = itemMouse;
        background.z = 0;
    }

    onContentItemChanged: {
        contentItem.parent = paddingItem
        contentItem.anchors.fill = paddingItem;
        //contentItem.anchors.leftMargin = background.height;
        contentItem.z = 0;
    }

    Component.onCompleted: {
        background.parent = itemMouse;
        background.z = 0;
        contentItem.parent = itemMouse
        contentItem.z = 1;
    }

    onPositionChanged: {
        if (!handleMouse.pressed && !mainFlickable.flicking &&
            !mainFlickable.dragging && !positionAnimation.running) {
            mainFlickable.contentX = (listItem.width-listItem.height) * mainFlickable.internalPosition;
        }
    }
//END signal handlers

//BEGIN UI implementation
    RowLayout {
        id: actionsLayout
        z: 1
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
            rightMargin: y
        }
        height: Math.min( parent.height / 1.5, Units.iconSizes.medium)
        property bool exclusive: false
        property Item checkedButton
        spacing: Units.largeSpacing
        Repeater {
            model: {
                if (listItem.actions.length == 0) {
                    return null;
                } else {
                    return listItem.actions[0].text !== undefined &&
                        listItem.actions[0].trigger !== undefined ?
                            listItem.actions :
                            listItem.actions[0];
                }
            }
            delegate: Icon {
                Layout.fillHeight: true
                Layout.minimumWidth: height
                source: modelData.iconName
                MouseArea {
                    anchors {
                        fill: parent
                        margins: -Units.smallSpacing
                    }
                    onClicked: {
                        if (modelData && modelData.trigger !== undefined) {
                            modelData.trigger();
                        // assume the model is a list of QAction or Action
                        } else if (toolbar.model.length > index) {
                            toolbar.model[index].trigger();
                        } else {
                            console.log("Don't know how to trigger the action")
                        }
                        positionAnimation.to = 0;
                        positionAnimation.running = true;
                    }
                }
            }
        }
    }

    PropertyAnimation {
        id: positionAnimation
        target: mainFlickable
        properties: "contentX"
        duration: Units.longDuration
        easing.type: Easing.InOutQuad
    }

    Flickable {
        id: mainFlickable
        z: 2
        interactive: false
        boundsBehavior: Flickable.StopAtBounds
        anchors.fill: parent
        contentWidth: mainItem.width
        contentHeight: height
        onFlickEnded: {
            if (contentX > width / 2) {
                positionAnimation.to = width - height;
            } else {
                positionAnimation.to = 0;
            }
            positionAnimation.running = true;
        }
        property real internalPosition:  (mainFlickable.contentX/(listItem.width-listItem.height));
        onInternalPositionChanged: {
            listItem.position = internalPosition;
        }

        Item {
            id: mainItem
            width: (mainFlickable.width * 2) - height 
            height: mainFlickable.height
            MouseArea {
                id: itemMouse
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                width: mainFlickable.width
                onClicked: listItem.clicked()
                onPressAndHold: listItem.pressAndHold()

                Item {
                    id: paddingItem
                    anchors {
                        fill: parent
                        margins: Units.smallSpacing
                    }
                }
            }

            MouseArea {
                id: handleMouse
                anchors {
                    left: itemMouse.right
                    top: parent.top
                    bottom: parent.bottom
                    leftMargin:  -height
                }
                preventStealing: true
                width: mainFlickable.width - actionsLayout.width
                property var downTimestamp;
                property int startX
                property int startMouseX

                onClicked: {
                    if (Math.abs(startX - mainFlickable.contentX) > Units.gridUnit ||
                        Math.abs(startMouseX - mouse.x) > Units.gridUnit) {
                        return;
                    }
                    if (mainFlickable.contentX > mainFlickable.width / 2) {
                        positionAnimation.to = 0;
                    } else {
                        positionAnimation.to = mainFlickable.width - mainFlickable.height;
                    }
                    positionAnimation.running = true;
                }
                onPressed: {
                    downTimestamp = (new Date()).getTime();
                    startX = mainFlickable.contentX;
                    startMouseX = mouse.x;
                }
                onPositionChanged: {
                    mainFlickable.contentX = Math.max(0, Math.min(mainFlickable.width - height, mainFlickable.contentX + (startMouseX - mouse.x)))
                }
                onReleased: {
                    var speed = ((startX - mainFlickable.contentX) / ((new Date()).getTime() - downTimestamp) * 1000);
                    mainFlickable.flick(speed, 0);
                }
                Icon {
                    id: handleIcon
                    anchors.verticalCenter: parent.verticalCenter
                    width: Units.iconSizes.smallMedium
                    height: width
                    x: y
                    source: "application-menu"
                }
            }
        }
    }
//END UI implementation

    Accessible.role: Accessible.ListItem
}
