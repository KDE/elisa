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

import QtQuick 2.5
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2
import org.kde.kirigami 1.0


/**
 * RefreshableScrollView is a scroll view for any Flickable that supports the
 * "scroll down to refresh" behavior, and also allows the contents of the
 * flickable to have more top margins in order to make possible to scroll down the list
 * to reach it with the thumb while using the phone with a single hand.
 *
 * Example usage:
 *
 * @code
 * import org.kde.kirigami 1.0 as Kirigami
 * [...]
 * 
 * Kirigami.RefreshableScrollView {
 *     id: view
 *     supportsRefreshing: true
 *     onRefreshingChanged: {
 *         if (refreshing) {
 *             myModel.refresh();
 *         }
 *     }
 *     ListView {
 *         //NOTE: MyModel doesn't come from the components,
 *         //it's purely an example on how it can be used together
 *         //some application logic that can update the list model
 *         //and signals when it's done.
 *         model: MyModel {
 *             onRefreshDone: view.refreshing = false;
 *         }
 *         delegate: BasicListItem {}
 *     }
 * }
 * [...]
 *
 * @endcode
 * 
 * @inherit QtQuick.Controls.Scrollview
 */
ScrollView {
    id: root

    /**
     * type: bool
     * If true the list is asking for refresh and will show a loading spinner.
     * it will automatically be set to true when the user pulls down enough the list.
     * This signals the application logic to start its refresh procedure.
     * The application itself will have to set back this property to false when done.
     */
    property bool refreshing: false

    /**
     * type: bool
     * If true the list supports the "pull down to refresh" behavior.
     */
    property bool supportsRefreshing: false

    /**
     * leftPadding: int
     * default contents padding at left
     */
    property int leftPadding: Units.gridUnit

    /**
     * topPadding: int
     * default contents padding at top
     */
    property int topPadding: Units.gridUnit

    /**
     * rightPadding: int
     * default contents padding at right
     */
    property int rightPadding: Units.gridUnit

    /**
     * bottomPadding: int
     * default contents padding at bottom
     */
    property int bottomPadding: Units.gridUnit


    frameVisible: false

    children: [
        Item {
            z: 99
            y: -root.flickableItem.contentY-height
            width: root.flickableItem.width
            height: root.flickableItem.topMargin
            BusyIndicator {
                id: busyIndicator
                anchors.centerIn: parent
                running: root.refreshing
                visible: root.refreshing || parent.y < root.flickableItem.topMargin
                opacity: supportsRefreshing ? (root.refreshing ? 1 : (parent.y/(busyIndicator.height*2))) : 0
                rotation: root.refreshing ? 0 : 360 * opacity
            }
            Label {
                id: label
                anchors {
                    bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                    bottomMargin: Units.gridUnit * 2
                }
                //FIXME: how to translate at this tier?
                text: "Pull down to refresh"
                opacity: supportsRefreshing ? (root.refreshing ? 0 : Math.min(1, ((parent.height - Units.gridUnit * 8) + parent.y) / (Units.gridUnit * 9))) : 0
                Behavior on opacity {
                    OpacityAnimator {
                        duration: Units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                }
            }
            Rectangle {
                color: Theme.textColor
                opacity: 0.2
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                //only show in ListViews
                visible: root.flickableItem == root.contentItem
                height: Math.round(Units.smallSpacing / 3);
            }
            onYChanged: {
                if (!supportsRefreshing) {
                    return;
                }
                if (!root.refreshing && y > busyIndicator.height*2) {
                    root.refreshing = true;
                }
            }
            Binding {
                target: root.flickableItem
                property: "topMargin"
                value: height/2
            }


            Binding {
                target: root.flickableItem
                property: "bottomMargin"
                value: Math.max((root.height - root.flickableItem.contentHeight), Units.gridUnit * 5)
            }

            Binding {
                target: root.contentItem
                property: "width"
                value: root.flickableItem.width
            }

            //FIXME: this shouldn't exist
            Timer {
                id: resetTimer
                interval: 100
                onTriggered: {
                    if (applicationWindow() && applicationWindow().header) {
                        flickableItem.contentY = -applicationWindow().header.Layout.preferredHeight;
                        applicationWindow().header.y = -applicationWindow().header.height +applicationWindow().header.Layout.preferredHeight;
                    }

                    if (root.contentItem == root.flickableItem) {
                        flickableItem.anchors.leftMargin = 0;
                        flickableItem.anchors.topMargin = 0;
                        flickableItem.anchors.rightMargin = 0;
                        flickableItem.anchors.bottomMargin = 0;
                    } else {
                        flickableItem.anchors.leftMargin = leftPadding;
                        flickableItem.anchors.topMargin = topPadding;
                        flickableItem.anchors.rightMargin = rightPadding;
                        flickableItem.anchors.bottomMargin = bottomPadding;
                    }
                }
            }
        }
    ]

    onLeftPaddingChanged: {
        if (root.contentItem == root.flickableItem) {
            flickableItem.anchors.leftMargin = 0;
            flickableItem.anchors.topMargin = 0;
            flickableItem.anchors.rightMargin = 0;
            flickableItem.anchors.bottomMargin = 0;
        } else {
            flickableItem.anchors.leftMargin = leftPadding;
            flickableItem.anchors.topMargin = topPadding;
            flickableItem.anchors.rightMargin = rightPadding;
            flickableItem.anchors.bottomMargin = bottomPadding;
        }
    }

    onFlickableItemChanged: resetTimer.restart()
}
