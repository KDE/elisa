/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2021 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

MouseArea {
    id: root

    property alias interactHeight: interact.implicitHeight
    property alias interactWidth: interact.implicitWidth
    property alias sliderValue: volumeSlider.value
    property alias slider: volumeSlider
    property bool muted

    // hardcode slider colours because theming doesn't really make sense on top of the blurred background
    property color sliderElapsedColor
    property color sliderRemainingColor
    property color sliderHandleColor
    property color sliderBorderActiveColor
    property color sliderBorderInactiveColor

    property int sliderHandleSize: Kirigami.Settings.isMobile ? Math.floor(Kirigami.Units.gridUnit * 0.75) : 18
    property int sliderBackgroundHeight: Kirigami.Settings.isMobile ? Math.floor(Kirigami.Units.smallSpacing / 2) : 6

    acceptedButtons: Qt.NoButton
    onWheel: {
        if (wheel.angleDelta.y > 0) {
            volumeSlider.increase()
        } else {
            volumeSlider.decrease()
        }
    }

    // Synthesized slider background that's not actually a part of the
    // slider. This is done so the slider's own background can be full
    // height yet transparent, for easier clicking
    Rectangle {
        anchors.left: volumeSlider.left
        anchors.verticalCenter: volumeSlider.verticalCenter
        implicitWidth: volumeSlider.width
        implicitHeight: sliderBackgroundHeight
        color: sliderRemainingColor
        radius: height / 2
        opacity: muted ? 0.5 : 1
    }

    Slider {
        id: volumeSlider
        anchors.fill: parent

        from: 0
        to: 100
        stepSize: 5

        enabled: !root.muted

        // This only provides a full-height area for clicking; see
        // https://bugs.kde.org/show_bug.cgi?id=408703. The actual visual
        // background is generated above ^^
        background: Rectangle {
            id: interact
            anchors.fill: parent
            color: "transparent"

            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                x: (LayoutMirroring.enabled ? volumeSlider.visualPosition * parent.width : 0)
                width: (LayoutMirroring.enabled ? parent.width - volumeSlider.visualPosition * parent.width : volumeSlider.visualPosition * parent.width)
                height: sliderBackgroundHeight
                color: sliderElapsedColor
                radius: height / 2
                opacity: root.muted ? 0.5 : 1
            }
        }

        handle: Rectangle {
            x: volumeSlider.leftPadding + volumeSlider.visualPosition * (volumeSlider.availableWidth - width)
            y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
            implicitWidth: sliderHandleSize
            implicitHeight: sliderHandleSize
            radius: width / 2
            color: sliderHandleColor
            border.width: elisaTheme.hairline
            border.color: volumeSlider.pressed ? sliderBorderActiveColor : (root.muted ? myPalette.mid : sliderBorderInactiveColor)
        }
    }
}
