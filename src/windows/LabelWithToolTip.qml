/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
import org.kde.kirigami 2.5 as Kirigami

Label {
    id: theLabel

    Loader {
        anchors.fill: parent

        active: theLabel.truncated
        visible: theLabel.truncated

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true

            ToolTip {
                delay: Qt.styleHints.mousePressAndHoldInterval
                visible: parent.containsMouse && theLabel.truncated
                text: theLabel.text

                enter: Transition { NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; from: 0.0; to: 1.0; duration: Kirigami.Units.longDuration; } }
                exit: Transition { NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; from: 1.0; to: 0.0; duration: Kirigami.Units.longDuration; } }
            }
        }
    }
}
