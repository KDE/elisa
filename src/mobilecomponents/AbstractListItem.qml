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

import QtQuick 2.1
import org.kde.kirigami 1.0

/**
 * An item delegate for the primitive ListView component.
 *
 * It's intended to make all listviews look coherent.
 *
 * @inherit QtQuick.Item
 */
Rectangle {
    id: listItem
    
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
     * type: Item
     * This property holds the background item.
     *
     * Note: If the background item has no explicit size specified,
     * it automatically follows the control's size.
     * In most cases, there is no need to specify width or
     * height for a background item.
     */
    property Item background: Rectangle {
        color: listItem.checked || (itemMouse.pressed && itemMouse.changeBackgroundOnPress) ? Theme.highlightColor : Theme.viewBackgroundColor

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

    implicitWidth: parent ? parent.width : childrenRect.width

    implicitHeight: paddingItem.childrenRect.height + Units.smallSpacing*2


    opacity: enabled ? 1 : 0.6

    height: visible ? implicitHeight : 0

    onContentItemChanged: {
        contentItem.parent = paddingItem;
        contentItem.anchors.fill = parent;
    }

    MouseArea {
        id: itemMouse
        property bool changeBackgroundOnPress: !listItem.checked && !listItem.sectionDelegate
        anchors.fill: parent
        enabled: false
        hoverEnabled: true

        onClicked: listItem.clicked()
        onPressAndHold: listItem.pressAndHold()

        Item {
            id: paddingItem
            z: 2
            anchors {
                fill: parent
                margins: Units.smallSpacing
            }
        }
    }

    onBackgroundChanged: {
        background.parent = itemMouse
    }

    Accessible.role: Accessible.ListItem
}
