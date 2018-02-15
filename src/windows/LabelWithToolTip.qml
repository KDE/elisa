/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2

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

                enter: Transition { NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; from: 0.0; to: 1.0; duration: 300; } }
                exit: Transition { NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; from: 1.0; to: 0.0; duration: 300; } }
            }
        }
    }
}
