/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.elisa

import ".."

FlatButtonWithToolTip {
    id: volumeButton

    property alias sliderValue: slider.value
    property alias slider: slider
    property bool muted

    Layout.maximumHeight: parent.height
    Layout.preferredHeight: Math.floor(Kirigami.Units.gridUnit * 3)
    Layout.maximumWidth: height
    Layout.preferredWidth: height

    text: KI18n.i18nc("@action:button", "Change Volume")
    icon.name: volumeButton.muted || volumeButton.slider.value == 0
        ? "audio-volume-muted" : "audio-volume-high"
    icon.color: "white"
    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
    Kirigami.Theme.inherit: false

    onClicked: {
        if (volumeSliderPopup.visible) {
            volumeSliderPopup.close();
        } else {
            volumeSliderPopup.open();
        }
    }

    Popup {
        id: volumeSliderPopup
        x: mirrored ? parent.width - width : 0
        y: -volumeSliderPopup.height
        width: Kirigami.Units.gridUnit * 10
        margins: Kirigami.Units.smallSpacing
        focus: true

        background: Rectangle {
            radius: Kirigami.Units.cornerRadius
            color: "#616161" // hardcode colour, since background is darkened blur (theming doesn't make sense)
        }

        contentItem: VolumeSlider {
            id: slider

            muted: volumeButton.muted
        }
    }
}
