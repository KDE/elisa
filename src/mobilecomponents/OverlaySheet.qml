/*
*   Copyright (C) 2016 by Marco Martin <mart@kde.org>
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
import QtQuick.Controls 1.3
import org.kde.kirigami 1.0
import QtGraphicalEffects 1.0

/**
 * An overlay sheet that covers the current Page content.
 * Its contents can be scrolled up or down, scrolling all the way up or
 * all the way down, dismisses it.
 * Use this for big, modal dialogs or information display, that can't be
 * logically done as a new separate Page, even if potentially
 * are taller than the screen space.
 */
Item {
    id: root

    anchors.fill: parent
    visible: false

    /**
     * opened: bool
     * If true the sheet is open showing the contents of the OverlaySheet
     * component.
     */
    property bool opened

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

    function open() {
        root.visible = true;
        openAnimation.running = true;
        root.opened = true;
    }

    function close() {
        closeAnimation.to = -height;
        closeAnimation.running = true;
    }

    property Item background: Item {
        anchors.fill: parent
        Rectangle {
            anchors.fill: parent
            color: Theme.textColor
            opacity: 0.6 * Math.min(
                (Math.min(mainFlickable.contentY + mainFlickable.height, mainFlickable.height) / mainFlickable.height),
                (2 + (mainFlickable.contentHeight - mainFlickable.contentY - mainFlickable.topMargin - mainFlickable.bottomMargin)/mainFlickable.height))
        }
        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
            }
            color: Theme.viewBackgroundColor
            y: -mainFlickable.contentY
            height: mainFlickable.contentHeight

            LinearGradient {
                height: Units.gridUnit/2
                anchors {
                    right: parent.right
                    left: parent.left
                    bottom: parent.top
                }

                start: Qt.point(0, Units.gridUnit/2)
                end: Qt.point(0, 0)
                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: Qt.rgba(0, 0, 0, 0.4)
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
                Behavior on opacity {
                    OpacityAnimator {
                        duration: Units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                }
            }

            LinearGradient {
                height: Units.gridUnit/2
                anchors {
                    right: parent.right
                    left: parent.left
                    top: parent.bottom
                }

                start: Qt.point(0, 0)
                end: Qt.point(0, Units.gridUnit/2)
                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: Qt.rgba(0, 0, 0, 0.4)
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
                Behavior on opacity {
                    OpacityAnimator {
                        duration: Units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }
    }

    default property Item contentItem

    Component.onCompleted: {
        backgroundChanged();
        contentItemChanged();
    }
    onBackgroundChanged: background.parent = root;
    onContentItemChanged: {
        contentItem.parent = contentItemParent;
        contentItem.anchors.left = contentItemParent.left;
        contentItem.anchors.right = contentItemParent.right;
    }
    onOpenedChanged: {
        if (opened) {
            open();
        } else {
            close();
        }
    }


    NumberAnimation {
        id: openAnimation
        target: mainFlickable
        properties: "contentY"
        from: -root.height
        to: Math.min(-root.height*0.15, flickableContents.height - root.height)
        duration: Units.longDuration
        easing.type: Easing.InOutQuad
    }

    SequentialAnimation {
        id: closeAnimation
        property int to: -root.height
        NumberAnimation {
            target: mainFlickable
            properties: "contentY"
            to: closeAnimation.to
            duration: Units.longDuration
            easing.type: Easing.InOutQuad
        }
        ScriptAction {
            script: root.visible = root.opened = false;
        }
    }

    ScrollView {
        id: scrollView
        z: 2
        anchors.fill: parent
        Flickable {
            id: mainFlickable
            topMargin: height
            contentWidth: width
            contentHeight: flickableContents.height;
            Item {
                id: flickableContents
                width: root.width
                height: contentItem.height + topPadding + bottomPadding + Units.iconSizes.medium + Units.gridUnit
                Item {
                    id: contentItemParent
                    anchors {
                        fill: parent
                        leftMargin: leftPadding
                        topMargin: topPadding
                        rightMargin: rightPadding
                        bottomMargin: bottomPadding
                    }
                }
            }
            bottomMargin: height
            onMovementEnded: {
                if ((root.height + mainFlickable.contentY) < root.height/2) {
                    closeAnimation.to = -root.height;
                    closeAnimation.running = true;
                } else if ((root.height*0.6 + mainFlickable.contentY) > flickableContents.height) {
                    closeAnimation.to = flickableContents.height
                    closeAnimation.running = true;
                }
            }
            onFlickEnded: movementEnded();
        }
    }
}
