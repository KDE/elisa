/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
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
import "private"
import org.kde.kirigami 1.0
import QtGraphicalEffects 1.0


/**
 * An item that can be used as a title for the application.
 * Scrolling the main page will make it taller or shorter (trough the point of going away)
 * It's a behavior similar to the typical mobile web browser adressbar
 * the minimum, preferred and maximum heights of the item can be controlled with
 * * Layout.minimumHeight: default is 0, i.e. hidden
 * * Layout.preferredHeight: default is Units.gridUnit * 1.6
 * * Layout.maximumHeight: default is Units.gridUnit * 3
 *
 * To achieve a titlebar that stays completely fixed just set the 3 sizes as the same
 */
Rectangle {
    id: headerItem
    z: 2
    anchors {
        left: parent.left
        right: parent.right
    }
    color: Theme.highlightColor
    Layout.minimumHeight: 0
    Layout.preferredHeight: Units.gridUnit * 1.6
    Layout.maximumHeight: Units.gridUnit * 3

    height: Layout.maximumHeight

    y: -height + Layout.preferredHeight

    property QtObject __appWindow: applicationWindow();
    parent: __appWindow.contentItem;

    transform: Translate {
        id: translateTransform
        y: __appWindow.controlsVisible ? 0 : -headerItem.height - shadow.height
        Behavior on y {
            NumberAnimation {
                duration: Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
    }

    Connections {
        id: headerSlideConnection
        target: __appWindow.pageStack.currentItem ? __appWindow.pageStack.currentItem.flickable : null
        property int oldContentY
        onContentYChanged: {
            if (!__appWindow.pageStack.currentItem) {
                return;
            }
            if (__appWindow.pageStack.currentItem.flickable.atYBeginning ||
                __appWindow.pageStack.currentItem.flickable.atYEnd) {
                return;
            }
            headerItem.y = Math.min(0, Math.max(-headerItem.height + headerItem.Layout.minimumHeight, headerItem.y + oldContentY - __appWindow.pageStack.currentItem.flickable.contentY))
            oldContentY = __appWindow.pageStack.currentItem.flickable.contentY
        }
    }
    Connections {
        target: __appWindow.pageStack
        onCurrentItemChanged: {
            if (!__appWindow.pageStack.currentItem) {
                return;
            }
            if (__appWindow.pageStack.currentItem.flickable) {
                headerSlideConnection.oldContentY = __appWindow.pageStack.currentItem.flickable.contentY;
            } else {
                headerSlideConnection.oldContentY = 0;
            }
            headerItem.y = -headerItem.height + headerItem.Layout.preferredHeight;
        }

        onContentChildrenChanged: {
            var i = 0;
            for (; i < __appWindow.pageStack.contentChildren.length; ++i) {
                if (i >= model.count || __appWindow.pageStack.contentChildren[i].title 
                    != model.get(i).title) {
                    break;
                }
            }

            while (model.count > i && model.count > 0) {
                model.remove(model.count - 1);
            }

            for (var j = i; j < __appWindow.pageStack.contentChildren.length; ++j) {
                model.append({"title": __appWindow.pageStack.contentChildren[j].title});
            }
        }
    }

    Behavior on y {
        enabled: __appWindow.pageStack.currentItem && __appWindow.pageStack.currentItem.flickable && !__appWindow.pageStack.currentItem.flickable.moving
        NumberAnimation {
            duration: Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    ListView {
        id: titleList
        anchors {
            fill: parent
            topMargin: Math.min(headerItem.height - headerItem.Layout.preferredHeight, -headerItem.y)
        }
        property bool wideScreen: __appWindow.pageStack.currentItem && __appWindow.pageStack.currentItem.width > 0 && __appWindow.pageStack.width > __appWindow.pageStack.currentItem.width
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        //FIXME: proper implmentation needs Qt 5.6 for new ObjectModel api
        model: ListModel {
            id: model
        }
        //__appWindow.pageStack.depth
        spacing: wideScreen ? 0 : Units.gridUnit
        currentIndex: __appWindow.pageStack.currentIndex
        snapMode: ListView.SnapToItem

        onCurrentIndexChanged: {
            positionViewAtIndex(currentIndex, ListView.Contain);
        }

        onContentXChanged: {
            if (wideScreen && !__appWindow.pageStack.contentItem.moving) {
                __appWindow.pageStack.contentItem.contentX = titleList.contentX
            }
        }
        onHeightChanged: {
            titleList.returnToBounds()
        }
        onMovementEnded: {
            if (wideScreen) {
                __appWindow.pageStack.contentItem.movementEnded();
            }
        }
        delegate: MouseArea {
            width: {
                //more columns shown?
                if (titleList.wideScreen) {
                    return __appWindow.pageStack.defaultColumnWidth;
                } else {
                    return Math.min(titleList.width, delegateRoot.implicitWidth);
                }
            }
            height: titleList.height
            onClicked: __appWindow.pageStack.currentIndex = model.index
            Row {
                id: delegateRoot

                spacing: Units.gridUnit
                Rectangle {
                    opacity: model.index > 0 ? 0.4 : 0
                    visible: !titleList.wideScreen
                    color: Theme.viewBackgroundColor
                    anchors.verticalCenter: parent.verticalCenter
                    width: height
                    height: Math.min(Units.gridUnit, title.height / 2)
                    radius: width
                }
                Heading {
                    id: title
                    width:Math.min(titleList.width, implicitWidth)
                    anchors.verticalCenter: parent.verticalCenter
                    opacity: titleList.currentIndex == index ? 1 : 0.4
                    //Scaling animate NativeRendering is too slow
                    renderType: Text.QtRendering
                    color: Theme.viewBackgroundColor
                    elide: Text.ElideRight
                    text: model.title
                    font.pixelSize: titleList.height / 1.6
                }
            }
        }
        Connections {
            target: titleList.wideScreen ? __appWindow.pageStack.contentItem : null
            onContentXChanged: {
                if (!titleList.contentItem.moving) {
                    titleList.contentX = Math.max(0, __appWindow.pageStack.contentItem.contentX)
                }
            }
        }
    }
    LinearGradient {
        id: shadow
        height: Units.gridUnit/2
        opacity: headerItem.y > -headerItem.height ? 1 : 0
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
        Behavior on opacity {
            OpacityAnimator {
                duration: Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
    }
}
