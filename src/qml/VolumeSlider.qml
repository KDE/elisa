/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2021 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import org.kde.elisa

AccessibleSlider {
    id: volumeSlider

    property bool muted

    // Use its own visible height by default
    interactionHeight: height

    from: 0
    to: 100

    keyStepSize: 1
    wheelStepSize: 5

    enabled: !muted

    Accessible.name: i18nc("@label", "Volume")

    onPressedChanged: {
        tooltip.delay = pressed ? 0 : Kirigami.Units.toolTipDelay
    }

    MouseArea {
        id: sliderMouseArea
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
    }

    ToolTip {
        id: tooltip
        x: volumeSlider.visualPosition * volumeSlider.width - width / 2
        visible: volumeSlider.pressed || sliderMouseArea.containsMouse
        // delay is actually handled in volumeSlider.onPressedChanged, because property bindings aren't immediate
        delay: volumeSlider.pressed ? 0 : Kirigami.Units.toolTipDelay
        closePolicy: Popup.NoAutoClose
        timeout: -1
        text: i18nc("Volume as a percentage", "%1%", Math.round(volumeSlider.value))
    }
}
