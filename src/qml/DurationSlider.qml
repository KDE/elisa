/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import org.kde.elisa 1.0
import org.kde.kirigami 2.5 as Kirigami
import ".."

RowLayout {
    id: durationSlider
    property int position
    property int duration
    property bool seekable
    property bool playEnabled

    property color labelColor: "white"
    property color sliderElapsedColor: "white"
    property color sliderRemainingColor: "grey"
    property color sliderHandleColor: "white"
    property color sliderBorderInactiveColor: "white"
    property color sliderBorderActiveColor: "grey"

    property int sliderBackgroundHeight: Kirigami.Settings.isMobile ? Math.floor(Kirigami.Units.smallSpacing / 2) : 6
    property int sliderHandleSize: Kirigami.Settings.isMobile ? Math.floor(Kirigami.Units.gridUnit * 0.75) : 18

    signal seek(int position)

    onPositionChanged: {
        if (!musicProgress.seekStarted) {
            musicProgress.value = position
        }
    }
    onDurationChanged: {
        musicProgress.to = durationSlider.duration
        musicProgress.value = Qt.binding(function() { return durationSlider.position })
    }

    spacing: 0

    TextMetrics {
        id: durationTextMetrics
        text: i18nc("This is used to preserve a fixed width for the duration text.", "00:00:00")
    }

    LabelWithToolTip {
        id: positionLabel

        text: timeIndicator.progressDuration

        color: durationSlider.labelColor

        Layout.alignment: Qt.AlignVCenter
        Layout.fillHeight: true
        Layout.rightMargin: !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : Kirigami.Units.largeSpacing * 2
        Layout.preferredWidth: (durationTextMetrics.boundingRect.width - durationTextMetrics.boundingRect.x) + Kirigami.Units.smallSpacing

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight

        ProgressIndicator {
            id: timeIndicator
            position: durationSlider.position
        }
    }

    MouseArea {
        id: seekWheelHandler
        Layout.alignment: Qt.AlignVCenter
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.rightMargin: !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
        Layout.leftMargin: LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0

        acceptedButtons: Qt.NoButton
        onWheel: {
            if (wheel.angleDelta.y > 0) {
                durationSlider.seek(position + 10000)
            } else {
                durationSlider.seek(position - 10000)
            }
        }

        // Synthesized slider background that's not actually a part of the
        // slider. This is done so the slider's own background can be full
        // height yet transparent, for easier clicking
        Rectangle {
            anchors.left: musicProgress.left
            anchors.verticalCenter: musicProgress.verticalCenter
            implicitWidth: seekWheelHandler.width
            implicitHeight: sliderBackgroundHeight
            color: sliderRemainingColor
            radius: height / 2
        }

        Slider {
            property bool seekStarted: false
            property int seekValue

            id: musicProgress
            anchors.fill: parent

            from: 0
            to: durationSlider.duration

            enabled: durationSlider.seekable && durationSlider.playEnabled
            live: true

            onValueChanged: {
                if (seekStarted) {
                    seekValue = value
                }
            }
            onPressedChanged: {
                if (pressed) {
                    seekStarted = true;
                    seekValue = value
                } else {
                    durationSlider.seek(seekValue)
                    seekStarted = false;
                }
            }

            // This only provides a full-height area for clicking; see
            // https://bugs.kde.org/show_bug.cgi?id=408703. The actual visual
            // background is generated above ^^
            background: Rectangle {
                anchors.fill: parent
                implicitWidth: seekWheelHandler.width
                implicitHeight: seekWheelHandler.height
                color: "transparent"

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    x: (LayoutMirroring.enabled ? musicProgress.visualPosition * parent.width : 0)
                    width: LayoutMirroring.enabled ? parent.width - musicProgress.visualPosition * parent.width: musicProgress.handle.x + radius
                    height: sliderBackgroundHeight
                    color: sliderElapsedColor
                    radius: height / 2
                }
            }

            handle: Rectangle {
                x: musicProgress.leftPadding + musicProgress.visualPosition * (musicProgress.availableWidth - width)
                y: musicProgress.topPadding + musicProgress.availableHeight / 2 - height / 2
                implicitWidth: sliderHandleSize
                implicitHeight: sliderHandleSize
                radius: width / 2
                color: sliderHandleColor
                border.width: elisaTheme.hairline
                border.color: musicProgress.pressed ? sliderBorderActiveColor : sliderBorderInactiveColor

            }
        }
    }

    LabelWithToolTip {
        id: durationLabel

        text: durationIndicator.progressDuration

        color: labelColor

        Layout.alignment: Qt.AlignVCenter
        Layout.fillHeight: true
        Layout.leftMargin: LayoutMirroring.enabled ? (Kirigami.Units.largeSpacing * 2) : 0
        Layout.preferredWidth: (durationTextMetrics.boundingRect.width - durationTextMetrics.boundingRect.x)

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft

        ProgressIndicator {
            id: durationIndicator
            position: durationSlider.duration
        }
    }
}
