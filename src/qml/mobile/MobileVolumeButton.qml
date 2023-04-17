/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>
   SPDX-FileCopyrightText: 2023 (c) ivan tkachenko <me@ratijas.tk>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

import ".."

FlatButtonWithToolTip {
    id: volumeButton

    property alias sliderValue: slider.value
    property alias slider: slider
    property bool muted

    Layout.maximumHeight: parent.height
    Layout.preferredHeight: Math.floor(Kirigami.Units.gridUnit * 2.5)
    Layout.maximumWidth: height
    Layout.preferredWidth: height

    text: i18nc("@action:button", "Change Volume")
    icon.name: volumeButton.muted || volumeButton.slider.visualPosition == 0 ? "player-volume-muted" : "player-volume"
    icon.color: "white"
    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
    Kirigami.Theme.inherit: false

    menuPolicy: FlatButtonWithToolTip.MenuPolicy.OnClickOrPressOrEnter
    menu: Popup {
        height: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing * 2
        width: Kirigami.Units.gridUnit * 10
        focus: true

        background: Rectangle {
            radius: Kirigami.Units.smallSpacing
            color: "#616161" // hardcode colour, since background is darkened blur (theming doesn't make sense)
        }

        contentItem: VolumeSlider {
            id: slider

            muted: volumeButton.muted
        }
    }
}
