/****************************************************************************
**
** Copyright (C) 2012 Marco Martin  <mart@kde.org>
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.2
import org.kde.kirigami 1.0

import "PageStack.js" as Engine

Item {
    id: actualRoot

    anchors.fill: parent

//BEGIN PROPERTIES
    /**
     * This property holds the number of items currently pushed onto the view
     */
    property int depth: Engine.getDepth()

    /**
     * The last Page in the Row
     */
    property Item lastItem: null

    /**
     * The currently visible Item
     */
    property Item currentItem: currentItem

    /**
     * the index of the currently visible Item
     */
    property int currentIndex: 0

    /**
     * This property holds the list of content children.
     */
    property var contentChildren: Engine.actualPages

    /**
     * The initial item when this PageRow is created
     */
    property variant initialPage

    /**
     * The main flickable of this Row
     */
    property alias contentItem: mainFlickable

    /**
     * The default width for a column
     * default is wide enough for 30 characters.
     * Pages can override it with their Layout.fillWidth,
     * implicitWidth Layout.minimumWidth etc.
     */
    property int defaultColumnWidth: Math.floor(parent.width/(Units.gridUnit*30)) > 0 ? parent.width/Math.floor(parent.width/(Units.gridUnit*30)) : width

//END PROPERTIES

//BEGIN FUNCTIONS
    /**
     * Pushes a page on the stack.
     * The page can be defined as a component, item or string.
     * If an item is used then the page will get re-parented.
     * If a string is used then it is interpreted as a url that is used to load a page 
     * component.
     *
     * @param page The page can also be given as an array of pages.
     *     In this case all those pages will
     *     be pushed onto the stack. The items in the stack can be components, items or
     *     strings just like for single pages.
     *     Additionally an object can be used, which specifies a page and an optional
     *     properties property.
     *     This can be used to push multiple pages while still giving each of
     *     them properties.
     *     When an array is used the transition animation will only be to the last page.
     *
     * @param properties The properties argument is optional and allows defining a
     * map of properties to set on the page.
     * @return The new created page
     */
    function push(page, properties) {
        scrollAnimation.running = false;

        Engine.pop(currentItem, true);
        var item = Engine.push(page, properties, false, false);

        actualRoot.currentIndex = depth-1;
        return item
    }

    /**
     * Pops a page off the stack.
     * @param page If page is specified then the stack is unwound to that page,
     * to unwind to the first page specify
     * page as null.
     * @return The page instance that was popped off the stack.
     */
    function pop(page) {
        var page = Engine.pop(page, true);
        actualRoot.currentIndex = depth-1;
        return page;
    }

    /**
     * Replaces a page on the stack.
     * @param page The page can also be given as an array of pages.
     *     In this case all those pages will
     *     be pushed onto the stack. The items in the stack can be components, items or
     *     strings just like for single pages.
     *     Additionally an object can be used, which specifies a page and an optional
     *     properties property.
     *     This can be used to push multiple pages while still giving each of
     *     them properties.
     *     When an array is used the transition animation will only be to the last page.
     * @param properties The properties argument is optional and allows defining a
     * map of properties to set on the page.
     * @see push() for details.
     */
    function replace(page, properties) {
        pop(currentItem, true);
        scrollAnimation.running = false;
        var item = Engine.push(page, properties, true, false);
        actualRoot.currentIndex = depth-1;
        return item
    }

    /**
     * Clears the page stack.
     * Destroy (or reparent) all the pages contained.
     */
    function clear() {
        return Engine.clear();
    }

//END FUNCTIONS
    onCurrentIndexChanged: {
        internal.syncWithCurrentIndex();

        actualRoot.currentItem = Engine.pageStack[actualRoot.currentIndex].page;
        if (!actualRoot.currentItem) {
            actualRoot.currentItem = actualRoot.lastItem;
        }
    }

    property alias clip: scrollArea.clip

    SequentialAnimation {
        id: scrollAnimation
        property alias to: actualScrollAnimation.to
        NumberAnimation {
            id: actualScrollAnimation
            target: mainFlickable
            properties: "contentX"
            duration: internal.transitionDuration
            easing.type: Easing.InOutQuad
        }
        ScriptAction {
            script: {
                //At startup sometimes the contentX is NaN for an instant
                if (isNaN(mainFlickable.contentX)) {
                    return;
                }

                mainFlickable.returnToBounds();
                //to not break syncIndexWithPosition
                scrollAnimation.running = false;
                internal.syncIndexWithPosition();
            }
        }
    }

    // Called when the page stack visibility changes.
    onVisibleChanged: {
        if (lastItem) {
            if (visible)
                lastItem.visible = lastItem.parent.visible = true;
        }
    }

    onInitialPageChanged: {
        if (!internal.completed) {
            return
        }

        if (initialPage) {
            if (depth == 0) {
                push(initialPage, null, true)
            } else if (depth == 1) {
                replace(initialPage, null, true)
            } else {
                console.log("Cannot update PageStack.initialPage")
            }
        }
    }

    Component.onCompleted: {
        internal.completed = true
        if (initialPage && depth == 0) {
            scrollAnimation.running = false;
            //Disable animation: push() doesn't expose this anymore
            Engine.push(initialPage, null, false, true)
            actualRoot.currentIndex = depth-1;
        }
        actualRoot.currentItem = actualRoot.lastItem;
    }

    QtObject {
        id: internal

        // The number of ongoing transitions.
        property int ongoingTransitionCount: 0

        //FIXME: there should be a way to access to theh without storing it in an ugly way
        property bool completed: false

        // Duration of transition animation (in ms)
        property int transitionDuration: Units.longDuration

        function syncIndexWithPosition() {
            if (scrollAnimation.running) {
                return;
            }

            //search the last page to kinda fit
            for (var i = Engine.pageStack.length - 1; i >= 0; --i) {
                var item = Engine.pageStack[i];

                var mapped = item.mapToItem(mainFlickable, 0, 0, item.width, item.height);

                if (mapped.x < item.width /2 && (mapped.x+mapped.width) <= actualRoot.width) {
                    actualRoot.currentIndex = i;
                    break;
                }
            }
        }

        function syncWithCurrentIndex() {
            if (currentIndex < 0 || currentIndex > depth || root.width < width) {
                return;
            }

            var itemAtIndex = Engine.pageStack[Math.max(0, Math.min(currentIndex, depth-1))];
            //Why itemAtPrevIndex? sometimes itemAtIndex is not positioned yet
            var itemAtPrevIndex = Engine.pageStack[Math.max(0, currentIndex-1)];

            scrollAnimation.running = false;
            if (currentIndex > 0) {
                var itemAtPrevIndex = Engine.pageStack[Math.max(0, currentIndex-1)];
                scrollAnimation.to = Math.min(itemAtPrevIndex.x + itemAtPrevIndex.page.width, mainFlickable.contentWidth - mainFlickable.width);
            } else {
                scrollAnimation.to = 0;
            }

            //don't bother if we don't actually move
            if (mainFlickable.contentX != scrollAnimation.to &&
                //If current page is completely contained, don't scroll
                !(mainFlickable.contentX < scrollAnimation.to &&
                 mainFlickable.contentX + actualRoot.width > scrollAnimation.to + itemAtIndex.width)) {
                scrollAnimation.running = true;
            }
        }
    }

    ScrollView {
        id: scrollArea
        anchors.fill: parent
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        Flickable {
            id: mainFlickable
            anchors.fill: parent
            interactive: true//root.width > width
            boundsBehavior: Flickable.StopAtBounds
            contentWidth: root.width
            contentHeight: height
            onContentWidthChanged: internal.syncWithCurrentIndex();
            Row {
                id: root
                spacing: 0
                width: Math.max((depth-1+children[children.length-1].takenColumns) * defaultColumnWidth, childrenRect.width)

                height: parent.height
            }

            onContentXChanged: {
                if (!dragging) {
                    scrollEventCompresstimer.restart();
                }
            }
            onDraggingChanged: {
                if (!dragging) {
                    scrollEventCompresstimer.restart();
                }
            }

            Timer {
                id: scrollEventCompresstimer
                interval: 150
                onTriggered: {
                    //get correct index
                    internal.syncIndexWithPosition();
                    //snap
                    internal.syncWithCurrentIndex();
                }
            }
            onWidthChanged: {
                resizeEventCompressTimer.restart();
            }
            Timer {
                id: resizeEventCompressTimer
                interval: 150
                onTriggered: {
                    if (actualRoot &&
                        actualRoot.lastVisiblePage &&
                        actualRoot.lastVisiblePage.parent &&
                        actualRoot.lastVisiblePage.parent.parent &&
                        actualRoot.lastVisiblePage.parent.parent.pageLevel) {
                        scrollToLevel(actualRoot.lastVisiblePage.parent.parent.pageLevel);
                    }
                }
            }
        }
    }

    // Component for page containers.
    Component {
        id: containerComponent

        Item {
            id: container
            implicitWidth: actualContainer.width //+ Units.gridUnit * 8
            width: implicitWidth
            height: parent ? parent.height : 0
            property int pageLevel: 0

            x: 0

            // The actual parent of page: page will anchor to that
            property Item pageParent: actualContainer

            property int pageDepth: 0
            Component.onCompleted: {
                pageDepth = Engine.getDepth() + 1
                container.z = -Engine.getDepth()
            }

            // The states correspond to the different possible positions of the container.
            state: "Hidden"

            // The page held by this container.
            property Item page: null

            // The owner of the page.
            property Item owner: null

            // The width of the longer stack dimension
            property int stackWidth: Math.max(actualRoot.width, actualRoot.height)


            // Flag that indicates the container should be cleaned up after the transition has ended.
            property bool cleanupAfterTransition: false

            // Flag that indicates if page transition animation is running
            property bool transitionAnimationRunning: false

            // State to be set after previous state change animation has finished
            property string pendingState: "none"

            //how many columns take the page?
            property alias takenColumns: actualContainer.takenColumns

            // Ensures that transition finish actions are executed
            // in case the object is destroyed before reaching the
            // end state of an ongoing transition
            Component.onDestruction: {
                if (transitionAnimationRunning)
                    transitionEnded();
            }

            Item {
                id: actualContainer

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                }

                property int takenColumns: {
                    if (container.page && container.page.Layout && container.page.Layout.fillWidth) {
                        return Math.max(Math.max(1, Math.round(actualRoot.width/defaultColumnWidth)-(container.x > 0 ? 1: 0)), Math.round(container.page ? container.page.implicitWidth/defaultColumnWidth : 1));
                    } else {
                        return Math.max(1, Math.round(container.page ? container.page.implicitWidth/defaultColumnWidth : 1));
                    }
                }

                width: (container.pageDepth >= actualRoot.depth ? Math.min(actualRoot.width, takenColumns*defaultColumnWidth) : defaultColumnWidth)
            }

            Rectangle {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: actualContainer.right
                }
                width: 1
                color: Theme.textColor
                opacity: 0.3
                visible: container.pageDepth < actualRoot.depth
            }

            // Sets pending state as current if state change is delayed
            onTransitionAnimationRunningChanged: {
                if (!transitionAnimationRunning && pendingState != "none") {
                    state = pendingState;
                    pendingState = "none";
                }
            }

            // Handles state change depening on transition animation status
            function setState(newState)
            {
                if (transitionAnimationRunning)
                    pendingState = newState;
                else
                    state = newState;
            }

            // Performs a push enter transition.
            function pushEnter(immediate, orientationChanges)
            {
                if (!immediate) {
                    setState("Right");
                }
                setState("");
                page.visible = true;
            }

            // Performs a push exit transition.
            function pushExit(replace, immediate, orientationChanges)
            {
                if (replace) {
                    setState(immediate ? "Hidden" : "Left");
                }

                if (replace) {
                    if (immediate)
                        cleanup();
                    else
                        cleanupAfterTransition = true;
                }
            }

            // Performs a pop enter transition.
            function popEnter(immediate, orientationChanges)
            {
                setState("");
                page.visible = true;
            }

            // Performs a pop exit transition.
            function popExit(immediate, orientationChanges)
            {
                setState(immediate ? "Hidden" : "Left");

                if (immediate)
                    cleanup();
                else
                    cleanupAfterTransition = true;
            }

            // Called when a transition has started.
            function transitionStarted()
            {
                container.clip = true
                transitionAnimationRunning = true;
                internal.ongoingTransitionCount++;
            }

            // Called when a transition has ended.
            function transitionEnded()
            {
                container.clip = false
                if (state != "")
                    state = "Hidden";

                internal.ongoingTransitionCount--;
                transitionAnimationRunning = false;

                if (cleanupAfterTransition) {
                    cleanup();
                }
                internal.syncWithCurrentIndex();
            }

            states: [
                // Explicit properties for default state.
                State {
                    name: ""
                    PropertyChanges { target: container; visible: true; opacity: 1 }
                    PropertyChanges { target: container; width: container.implicitWidth}
                },
                // Start state for pop entry, end state for push exit.
                State {
                    name: "Left"
                    PropertyChanges { target: container; opacity: 0 }
                    PropertyChanges { target: container; width: Units.gridUnit * 8}
                },
                // Start state for push entry, end state for pop exit.
                State {
                    name: "Right"
                    PropertyChanges { target: container; opacity: 0 }
                    PropertyChanges { target: container; width: Units.gridUnit * 8}
                },
                // Inactive state.
                State {
                    name: "Hidden"
                    PropertyChanges { target: container; visible: false }
                    PropertyChanges { target: container; width: container.implicitWidth}
                }
            ]

            transitions: [
                // Push exit transition
                Transition {
                    from: ""; to: "Left"
                    SequentialAnimation {
                        ScriptAction { script: transitionStarted() }
                        ParallelAnimation {
                            PropertyAnimation { properties: "width"; easing.type: Easing.InQuad; duration: internal.transitionDuration }
                            PropertyAnimation { properties: "opacity"; easing.type: Easing.InQuad; duration: internal.transitionDuration }
                        }
                        ScriptAction { script: transitionEnded() }
                    }
                },
                // Pop entry transition
                Transition {
                    from: "Left"; to: ""
                    SequentialAnimation {
                        ScriptAction { script: transitionStarted() }
                        ParallelAnimation {
                            PropertyAnimation { properties: "width"; easing.type: Easing.OutQuad; duration: internal.transitionDuration }
                            PropertyAnimation { properties: "opacity"; easing.type: Easing.InQuad; duration: internal.transitionDuration }
                        }
                        ScriptAction { script: transitionEnded() }
                    }
                },
                // Pop exit transition
                Transition {
                    from: ""; to: "Right"
                    SequentialAnimation {
                        ScriptAction { script: transitionStarted() }
                        ParallelAnimation {
                            PropertyAnimation { properties: "width"; easing.type: Easing.InQuad; duration: internal.transitionDuration }
                            PropertyAnimation { properties: "opacity"; easing.type: Easing.InQuad; duration: internal.transitionDuration }
                        }
                        // Workaround for transition animation bug causing ghost view with page pop transition animation
                        // TODO: Root cause still unknown
                        PropertyAnimation {}
                        ScriptAction { script: transitionEnded() }
                    }
                },
                // Push entry transition
                Transition {
                    from: "Right"; to: ""
                    SequentialAnimation {
                        ScriptAction { script: transitionStarted() }
                        ParallelAnimation {
                            PropertyAnimation { properties: "width"; easing.type: Easing.OutQuad; duration: internal.transitionDuration }
                            PropertyAnimation { properties: "opacity"; easing.type: Easing.InQuad; duration: internal.transitionDuration }
                        }
                        ScriptAction { script: transitionEnded() }
                    }
                }
            ]

            // Cleans up the container and then destroys it.
            function cleanup()
            {
                if (page != null) {
                    if (owner != container) {
                        // container is not the owner of the page - re-parent back to original owner
                        page.visible = false;
                        page.anchors.fill = undefined
                        page.parent = owner;
                    }
                }
                container.parent = null;
                container.visible = false;
                destroy();
            }
        }
    }
}
