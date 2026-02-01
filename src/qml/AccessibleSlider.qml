/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2021 (c) Devin Lin <espidev@gmail.com>
   SPDX-FileCopyrightText: 2024 (c) Jack Hill <jackhill3103@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2

/**
 * A slider that allows setting different step sizes for arrow keys and mouse wheels.
 *
 * Setting `stepSize` gives the slider tickmarks, which look unappealing for lots of steps.
 * If `stepSize` is not set then the Slider's built-in increase() and decrease() functions
 * use increments of 0.1, which is much too small for our usage. Hence we have our own
 * implementation here. This also means we can define separate step sizes for the arrow keys
 * and mouse wheel.
 *
 * NOTE: The Accessible attached property reads `stepSize` to the user, which may not be used
 * when incrementing/decrementing.
 */
QQC2.Slider {
    id: root

    /**
     * The step size for arrow keys
     */
    property real keyStepSize: stepSize


    /**
     * The height of the space the user can interact with
     */
    property real interactionHeight: root.height

    /**
     * The step size for the mouse wheel
     */
    property real wheelStepSize: stepSize

    readonly property real __keyStepSize: keyStepSize === null ? 0 : keyStepSize
    readonly property real __wheelStepSize: wheelStepSize === null ? 0 : wheelStepSize

    function __move(step: real) {
        value = Math.max(from, Math.min(value + step, to))
        moved()
    }

    // containmentMask for layout
    containmentMask: interactionRect

    Accessible.onDecreaseAction: __move(-__keyStepSize)
    Accessible.onIncreaseAction: __move(__keyStepSize)
    Keys.onLeftPressed: __move(-__keyStepSize)
    Keys.onRightPressed: __move(__keyStepSize)

    Item {
        id: interactionRect
        anchors.verticalCenter: root.verticalCenter
        visible: false 
        height: root.interactionHeight
        width: root.width
    }

    MouseArea {
        anchors.fill: interactionRect
        acceptedButtons: Qt.NoButton
        onWheel: wheel => root.__move(root.__wheelStepSize * (wheel.angleDelta.y > 0 ? 1 : -1))
    }
}
