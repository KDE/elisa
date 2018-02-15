/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

/*
 * listviewdragitem
 *
 * An example of reordering items in a ListView via drag'n'drop.
 *
 * Author: Aurélien Gâteau
 * License: BSD
 */
import QtQuick 2.7

FocusScope {
    id: root

    default property alias contentItem: dragArea.contentItem

    // This item will become the parent of the dragged item during the drag operation
    property Item draggedItemParent

    signal moveItemRequested(int from, int to)

    // Size of the area at the top and bottom of the list where drag-scrolling happens
    property int scrollEdgeSize: 6

    // Internal: set to -1 when drag-scrolling up and 1 when drag-scrolling down
    property int _scrollingDirection: 0

    // Internal: shortcut to access the attached ListView from everywhere. Shorter than root.ListView.view
    property ListView _listView: ListView.view

    property alias containsMouse: dragArea.containsMouse

    signal clicked()
    signal doubleClicked()

    width: contentItem.width
    height: topPlaceholder.height + wrapperParent.height + bottomPlaceholder.height

    property int placeholderHeight

    // Make contentItem a child of contentItemWrapper
    onContentItemChanged: {
        contentItem.parent = dragArea;
    }

    Rectangle {
        id: topPlaceholder
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: 0
        color: myPalette.mid
    }

    Item {
        id: wrapperParent
        anchors {
            left: parent.left
            right: parent.right
            top: topPlaceholder.bottom
        }
        height: contentItem.height

        Rectangle {
            id: contentItemWrapper
            anchors.fill: parent
            Drag.active: dragArea.drag.active
            Drag.hotSpot {
                x: contentItem.width / 2
                y: contentItem.height / 2
            }

            MouseArea {
                id: dragArea
                anchors.fill: parent
                drag.target: parent
                // Disable smoothed so that the Item pixel from where we started the drag remains under the mouse cursor
                drag.smoothed: false

                property Item contentItem

                hoverEnabled: true
                preventStealing: true

                acceptedButtons: Qt.LeftButton

                onReleased: {
                    if (drag.active) {
                        emitMoveItemRequested();
                    }
                }

                onClicked: root.clicked()

                onDoubleClicked: root.doubleClicked()
            }
        }
    }

    Rectangle {
        id: bottomPlaceholder
        anchors {
            left: parent.left
            right: parent.right
            top: wrapperParent.bottom
        }
        height: 0
        color: myPalette.mid
    }

    SmoothedAnimation {
        id: upAnimation
        target: _listView
        property: "contentY"
        to: 0
        running: _scrollingDirection == -1
    }

    SmoothedAnimation {
        id: downAnimation
        target: _listView
        property: "contentY"
        to: _listView.contentHeight - _listView.height
        running: _scrollingDirection == 1
    }

    Loader {
        id: topDropAreaLoader
        active: model.index === 0
        anchors {
            left: parent.left
            right: parent.right
            bottom: wrapperParent.verticalCenter
        }
        height: placeholderHeight
        sourceComponent: Component {
            DropArea {
                property int dropIndex: 0
            }
        }
    }

    DropArea {
        id: bottomDropArea
        anchors {
            left: parent.left
            right: parent.right
            top: wrapperParent.verticalCenter
        }
        property bool isLast: model.index === _listView.count - 1
        height: isLast ? _listView.contentHeight - y : placeholderHeight

        property int dropIndex: model.index + 1
    }

    states: [
        State {
            when: dragArea.drag.active
            name: "dragging"

            ParentChange {
                target: contentItemWrapper
                parent: draggedItemParent
            }
            PropertyChanges {
                target: contentItemWrapper
                opacity: 0.9
                anchors.fill: undefined
                width: contentItem.width
                height: contentItem.height
            }
            PropertyChanges {
                target: wrapperParent
                height: 0
            }
            PropertyChanges {
                target: root
                _scrollingDirection: {
                    var yCoord = _listView.mapFromItem(dragArea, 0, dragArea.mouseY).y;
                    if (yCoord < scrollEdgeSize) {
                        -1;
                    } else if (yCoord > _listView.height - scrollEdgeSize) {
                        1;
                    } else {
                        0;
                    }
                }
            }
        },
        State {
            when: bottomDropArea.containsDrag
            name: "droppingBelow"
            PropertyChanges {
                target: bottomPlaceholder
                height: placeholderHeight
            }
            PropertyChanges {
                target: bottomDropArea
                height: contentItem.height
            }
        },
        State {
            when: topDropAreaLoader.item.containsDrag
            name: "droppingAbove"
            PropertyChanges {
                target: topPlaceholder
                height: placeholderHeight
            }
            PropertyChanges {
                target: topDropAreaLoader
                height: contentItem.height
            }
        }
    ]

    function emitMoveItemRequested() {
        var dropArea = contentItemWrapper.Drag.target;
        if (!dropArea) {
            return;
        }
        var dropIndex = dropArea.dropIndex;

        // If the target item is below us, then decrement dropIndex because the target item is going to move up when
        // our item leaves its place
        if (model.index < dropIndex) {
            dropIndex--;
        }
        if (model.index === dropIndex) {
            return;
        }
        root.moveItemRequested(model.index, dropIndex);

        // Scroll the ListView to ensure the dropped item is visible. This is required when dropping an item after the
        // last item of the view. Delay the scroll using a Timer because we have to wait until the view has moved the
        // item before we can scroll to it.
        makeDroppedItemVisibleTimer.start();
    }

    Timer {
        id: makeDroppedItemVisibleTimer
        interval: 0
        onTriggered: {
            _listView.positionViewAtIndex(model.index, ListView.Contain);
        }
    }
}
