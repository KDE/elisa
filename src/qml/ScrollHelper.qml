/*
   SPDX-FileCopyrightText: 2016 (c) Michael Bohlender <michael.bohlender@kdemail.net>
   SPDX-FileCopyrightText: 2017 (c) Christian Mollekopf <mollekopf@kolabsystems.com>

   SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.2

/*
* The MouseArea + interactive: false + maximumFlickVelocity are required
* to fix scrolling for desktop systems where we don't want flicking behaviour.
*
* See also:
* ScrollView.qml in qtquickcontrols
* qquickwheelarea.cpp in qtquickcontrols
*/
MouseArea {
    id: root
    propagateComposedEvents: true

    property Flickable flickable

    //Place the mouse area under the flickable
    z: -1
    onFlickableChanged: {
        flickable.interactive = false
        flickable.maximumFlickVelocity = 100000
        flickable.boundsBehavior = Flickable.StopAtBounds
        root.parent = flickable
    }

    function scrollByPixelDelta(flickableItem, pixelDelta) {
        if (!pixelDelta) {
            return flickableItem.contentY;
        }

        var minYExtent = flickableItem.originY + flickableItem.topMargin;
        var maxYExtent = (flickableItem.contentHeight + flickableItem.bottomMargin + flickableItem.originY) - flickableItem.height;

        if (typeof(flickableItem.headerItem) !== "undefined" && flickableItem.headerItem) {
            minYExtent += flickableItem.headerItem.height
        }

        //Avoid overscrolling
        return Math.max(minYExtent, Math.min(maxYExtent, flickableItem.contentY - pixelDelta));
    }

    function calculateNewPosition(flickableItem, wheel) {
        //Nothing to scroll
        if (flickableItem.contentHeight < flickableItem.height) {
            return flickableItem.contentY;
        }
        //Ignore 0 events (happens at least with Christians trackpad)
        if (wheel.pixelDelta.y === 0 && wheel.angleDelta.y === 0) {
            return flickableItem.contentY;
        }
        //pixelDelta seems to be the same as angleDelta/8
        var pixelDelta = 0
        //The pixelDelta is a smaller number if both are provided, so pixelDelta can be 0 while angleDelta is still something. So we check the angleDelta
        if (wheel.angleDelta.y) {
            var wheelScrollLines = 3 //Default value of QApplication wheelScrollLines property
            var pixelPerLine = 20 //Default value in Qt, originally comes from QTextEdit
            var ticks = (wheel.angleDelta.y / 8) / 15.0 //Divide by 8 gives us pixels typically come in 15pixel steps.
            pixelDelta =  ticks * pixelPerLine * wheelScrollLines
        } else {
            pixelDelta = wheel.pixelDelta.y
        }

        return scrollByPixelDelta(flickableItem, pixelDelta);
    }

    function scrollDown() {
        flickable.flick(0, 0);
        flickable.contentY = scrollByPixelDelta(flickable, -60); //3 lines * 20 pixels
    }

    function scrollUp() {
        flickable.flick(0, 0);
        flickable.contentY = scrollByPixelDelta(flickable, 60); //3 lines * 20 pixels
    }

    onWheel: {
        var newPos = calculateNewPosition(flickable, wheel);
        // console.warn("Delta: ", wheel.pixelDelta.y);
        // console.warn("Old position: ", flickable.contentY);
        // console.warn("New position: ", newPos);

        // Show the scrollbars
        flickable.flick(0, 0);
        flickable.contentY = newPos;
        cancelFlickStateTimer.start()
    }


    Timer {
        id: cancelFlickStateTimer
        //How long the scrollbar will remain visible
        interval: 500
        // Hide the scrollbars
        onTriggered: flickable.cancelFlick();
    }
}

