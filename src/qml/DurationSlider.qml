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

    property color labelColor

    signal seek(int position)

    onPositionChanged: {
        if (!musicProgress.seekStarted) {
            musicProgress.value = position / 1000
        }
    }
    onDurationChanged: {
        musicProgress.to = durationSlider.duration / 1000
        musicProgress.value = Qt.binding(() => durationSlider.position / 1000)
    }

    Connections {
        target: ElisaApplication.mediaPlayListProxyModel
        function onClearPlayListPlayer() {
            musicProgress.value = 0;
        }
    }

    spacing: 0

    TextMetrics {
        id: durationTextMetrics
        text: i18nc("@info:placeholder This is used to preserve a fixed width for the duration text.", "00:00:00")
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

    Slider {
        property bool seekStarted: false

        id: musicProgress
        Layout.alignment: Qt.AlignVCenter
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.rightMargin: !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
        Layout.leftMargin: LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0

        // from, to and value of Slider are rescaled to seconds to avoid integer overflow issues
        from: 0
        to: durationSlider.duration / 1000

        enabled: durationSlider.seekable && durationSlider.playEnabled
        live: true

        onMoved: {
            durationSlider.seek(value * 1000)
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            onWheel: {
                if (wheel.angleDelta.y > 0) {
                    durationSlider.seek((musicProgress.value + 10) * 1000)
                } else {
                    durationSlider.seek((musicProgress.value - 10) * 1000)
                }
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
