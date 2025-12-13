/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import org.kde.elisa
import org.kde.ki18n
import org.kde.kirigami as Kirigami

/**
 * Aligned into a 5x2 grid
 *
 * Labels inline (bottom row takes up 0 space):
 *  --------------------------------------------
 * | Label  | Slider | Slider | Slider | Label  |
 *  --------------------------------------------
 * |        |        |        |        |        |
 *  --------------------------------------------
 *
 * Labels below (outer columns take up 0 space):
 *  --------------------------------------------
 * |        | Slider | Slider | Slider |        |
 *  --------------------------------------------
 * |        | Label  |        | Label  |        |
 *  --------------------------------------------
 */
GridLayout {
    id: root

    property int position
    property int duration
    property bool seekable
    property bool playEnabled

    property color labelColor
    property bool labelsInline: true

    rows: 2
    columns: 5
    rowSpacing: 0
    columnSpacing: Kirigami.Units.largeSpacing

    signal seek(int position)

    onPositionChanged: {
        if (!slider.pressed) {
            slider.value = position / 1000
        }
    }
    onDurationChanged: {
        slider.to = duration / 1000
    }

    Connections {
        target: ElisaApplication.mediaPlayListProxyModel
        function onClearPlayListPlayer() {
            slider.value = 0;
        }
    }

    TextMetrics {
        id: durationTextMetrics
        text: KI18n.i18nc("@info:placeholder This is used to preserve a fixed width for the duration text.", "00:00:00")
    }

    LabelWithToolTip {
        id: positionLabel

        text: timeIndicator.progressDuration
        color: root.labelColor
        font.features: { "tnum": 1 }

        Layout.row: root.labelsInline ? 0 : 1
        Layout.column: root.labelsInline ? 0 : 1
        Layout.alignment: Qt.AlignVCenter
        Layout.fillHeight: true
        Layout.rightMargin: !root.labelsInline ? 0 : !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : Kirigami.Units.largeSpacing * 2
        Layout.preferredWidth: (durationTextMetrics.boundingRect.width - durationTextMetrics.boundingRect.x) + Kirigami.Units.smallSpacing

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: root.labelsInline ? Text.AlignRight : Text.AlignLeft

        ProgressIndicator {
            id: timeIndicator
            position: root.position
        }
    }

    AccessibleSlider {
        id: slider

        Layout.row: 0
        Layout.column: 1
        Layout.columnSpan: 3
        Layout.alignment: Qt.AlignVCenter
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.rightMargin: !root.labelsInline ? 0 : !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
        Layout.leftMargin: !root.labelsInline ? 0 : LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0

        Accessible.name: KI18n.i18nc("@label", "Duration")

        // from, to and value of Slider are rescaled to seconds to avoid integer overflow issues
        from: 0
        to: root.duration / 1000

        keyStepSize: 5
        wheelStepSize: 10

        enabled: root.seekable && root.playEnabled
        live: true
        onMoved: root.seek(value * 1000)
    }

    LabelWithToolTip {
        id: durationLabel

        text: durationIndicator.progressDuration

        color: root.labelColor
        font.features: { "tnum": 1 }

        Layout.row: root.labelsInline ? 0 : 1
        Layout.column: root.labelsInline ? 4 : 3
        Layout.alignment: Qt.AlignVCenter
        Layout.fillHeight: true
        Layout.leftMargin: !root.labelsInline ? 0 : LayoutMirroring.enabled ? (Kirigami.Units.largeSpacing * 2) : 0
        Layout.preferredWidth: (durationTextMetrics.boundingRect.width - durationTextMetrics.boundingRect.x)

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: root.labelsInline ? Text.AlignLeft : Text.AlignRight

        ProgressIndicator {
            id: durationIndicator
            position: root.duration
        }
    }
}
