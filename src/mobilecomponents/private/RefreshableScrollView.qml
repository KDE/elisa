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
import QtGraphicalEffects 1.0
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
            id: busyIndicatorFrame
            z: 99
            y: -root.flickableItem.contentY-height
            width: root.flickableItem.width
            height: busyIndicator.height + Units.gridUnit * 2
            BusyIndicator {
                id: busyIndicator
                anchors.centerIn: parent
                running: root.refreshing
                visible: root.refreshing
                //Android busywidget QQC seems to be broken at custom sizes
            }
            Rectangle {
                id: spinnerProgress
                anchors {
                    fill: busyIndicator
                    margins: Math.ceil(Units.smallSpacing/2)
                }
                radius: width
                visible: supportsRefreshing && !refreshing && progress > 0
                color: "transparent"
                opacity: 0.8
                border.color: Theme.viewBackgroundColor
                border.width: Math.ceil(Units.smallSpacing/4)
                property real progress: supportsRefreshing && !refreshing ? (parent.y/busyIndicatorFrame.height) : 0
                
            }
            ConicalGradient {
                source: spinnerProgress
                visible: spinnerProgress.visible
                anchors.fill: spinnerProgress
                gradient: Gradient {
                    GradientStop { position: 0.00; color: Theme.highlightColor }
                    GradientStop { position: spinnerProgress.progress; color: Theme.highlightColor }
                    GradientStop { position: spinnerProgress.progress + 0.01; color: "transparent" }
                    GradientStop { position: 1.00; color: "transparent" }
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
                height: Math.ceil(Units.smallSpacing / 5);
            }
            onYChanged: {
                if (y > busyIndicatorFrame.height*1.5 + topPadding && applicationWindow() && root.flickableItem.atYBeginning && applicationWindow().pageStack.anchors.bottomMargin == 0 && root.width < root.height) {
                    //here assume applicationWindow().pageStack has a translate as transform
                    applicationWindow().pageStack.transform[0].y = root.height/2;
                    overshootResetTimer.restart();
                    canOvershootBackTimer.restart();
                }

                if (!supportsRefreshing) {
                    return;
                }
                if (!root.refreshing && y > busyIndicatorFrame.height/2 + topPadding) {
                    root.refreshing = true;
                }
            }
            Timer {
                id: overshootResetTimer
                interval: 8000
                onTriggered: {
                    applicationWindow().pageStack.transform[0].y = 0;
                }
            }
            //HACK?
            Timer {
                id: canOvershootBackTimer
                interval: 800
            }
            Connections {
                target: root.flickableItem
                onMovementEnded: {
                    if (!canOvershootBackTimer.running &&
                        applicationWindow().pageStack.transform[0].y > 0) {
                        applicationWindow().pageStack.transform[0].y = 0;
                    }
                }
            }
            Binding {
                target: root.flickableItem
                property: "topMargin"
                value: root.topPadding + (root.refreshing ? busyIndicatorFrame.height : 0)
            }


            Binding {
                target: root.flickableItem
                property: "bottomMargin"
                value: Units.gridUnit * 5
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
                        flickableItem.contentY = -applicationWindow().header.preferredHeight;
                        applicationWindow().header.y = -applicationWindow().header.maximumHeight +applicationWindow().header.preferredHeight;
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
