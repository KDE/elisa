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

    property alias sliderValue: volumeSlider.value
    property alias slider: volumeSlider
    property bool muted

    acceptedButtons: Qt.NoButton
    onWheel: {
        if (wheel.angleDelta.y > 0) {
            volumeSlider.increase()
        } else {
            volumeSlider.decrease()
        }
    }

    Slider {
        id: volumeSlider
        anchors.fill: parent

        from: 0
        to: 100

        enabled: !root.muted
    }
}
