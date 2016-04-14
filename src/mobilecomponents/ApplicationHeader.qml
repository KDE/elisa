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


/**
 * An item that can be used as a title for the application.
 * Scrolling the main page will make it taller or shorter (trough the point of going away)
 * It's a behavior similar to the typical mobile web browser adressbar
 * the minimum, preferred and maximum heights of the item can be controlled with
 * * minimumHeight: default is 0, i.e. hidden
 * * preferredHeight: default is Units.gridUnit * 1.6
 * * maximumHeight: default is Units.gridUnit * 3
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
    property int minimumHeight: 0
    property int preferredHeight: Units.gridUnit * 1.6
    property int maximumHeight: Units.gridUnit * 3
    property alias contentItem: titleList

    height: maximumHeight

    y: -maximumHeight + preferredHeight

    property QtObject __appWindow: applicationWindow();
    parent: __appWindow.pageStack;

    transform: Translate {
        id: translateTransform
        y: __appWindow.controlsVisible ? 0 : -headerItem.height - shadow.height
        Behavior on y {
            NumberAnimation {
                duration: Units.longDuration
                easing.type: translateTransform.y < 0 ? Easing.OutQuad : Easing.InQuad
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

            if (titleList.wideScreen) {
                headerItem.y = -headerItem.maximumHeight + headerItem.preferredHeight;
            } else {
                headerItem.y = Math.min(0, Math.max(-headerItem.height + headerItem.minimumHeight, headerItem.y + oldContentY - __appWindow.pageStack.currentItem.flickable.contentY));
                oldContentY = __appWindow.pageStack.currentItem.flickable.contentY;
            }
        }
        onMovementEnded: {
            if (headerItem.y > -headerItem.maximumHeight + headerItem.preferredHeight) {
                //if don't change the position if more then preferredSize is shown
            } else if (headerItem.y > -headerItem.maximumHeight + headerItem.preferredHeight - headerItem.preferredHeight/2 ) {
                headerItem.y = -headerItem.maximumHeight + headerItem.preferredHeight;
            } else {
                headerItem.y = -headerItem.maximumHeight;
            }
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
            headerItem.y = -headerItem.maximumHeight + headerItem.preferredHeight;
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
        property Translate overshootTransform
        Component.onCompleted: {
            if (applicationWindow() && applicationWindow().pageStack.transform[0]) {
                overshootTransform = applicationWindow().pageStack.transform[0]
            }
            //only on iOs put the back button on top left corner
            if (Qt.platform.os == "ios") {
                var component = Qt.createComponent(Qt.resolvedUrl("private/BackButton.qml"));
                print(component.error);
                titleList.backButton = component.createObject(headerItem);
            }
        }
        property Item backButton
        clip: true
        anchors {
            fill: parent
            leftMargin: backButton ? backButton.width : 0
            topMargin: overshootTransform && overshootTransform.y > 0 ? 0 : Math.min(headerItem.height - headerItem.preferredHeight, -headerItem.y)
        }
        cacheBuffer: __appWindow.pageStack.width
        property bool wideScreen: __appWindow.pageStack.width > __appWindow.pageStack.height
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        //FIXME: proper implmentation needs Qt 5.6 for new ObjectModel api
        model: ListModel {
            id: model
        }
        //__appWindow.pageStack.depth
        spacing: 0
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

        NumberAnimation {
            id: scrollTopAnimation
            target: __appWindow.pageStack.currentItem.flickable || null
            properties: "contentY"
            to: 0
            duration: Units.longDuration
            easing.type: Easing.InOutQuad
        }

        delegate: MouseArea {
            width: {
                //more columns shown?
                if (titleList.wideScreen) {
                    return __appWindow.pageStack.defaultColumnWidth;
                } else {
                    return Math.min(titleList.width, delegateRoot.implicitWidth + Units.gridUnit + Units.smallSpacing);
                }
            }
            height: titleList.height
            onClicked: {
                //scroll up if current otherwise make current
                if (__appWindow.pageStack.currentIndex == model.index) {
                    scrollTopAnimation.running = true;
                } else {
                    __appWindow.pageStack.currentIndex = model.index;
                }
            }
            Row {
                id: delegateRoot
                x: Units.smallSpacing

                spacing: Units.gridUnit
                Rectangle {
                    opacity: model.index > 0 ? 0.4 : 0
                    visible: !titleList.wideScreen && opacity > 0
                    color: Theme.viewBackgroundColor
                    anchors.verticalCenter: parent.verticalCenter
                    width: height
                    height: Math.min(Units.gridUnit/2, title.height / 2)
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
    EdgeShadow {
        id: shadow
        edge: Qt.TopEdge
        opacity: headerItem.y > -headerItem.height ? 1 : 0
        anchors {
            right: parent.right
            left: parent.left
            top: parent.bottom
        }
        Behavior on opacity {
            OpacityAnimator {
                duration: Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
    }
}
