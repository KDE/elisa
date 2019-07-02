/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2

Label {
    id: theLabel

    horizontalAlignment: Text.AlignLeft

    elide: Text.ElideRight

    Loader {
        anchors.fill: parent

        active: theLabel.truncated
        visible: theLabel.truncated

        sourceComponent: MouseArea {
            id: hoverArea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.NoButton

            Loader {
                anchors.fill: parent
                active: hoverArea && hoverArea.containsMouse

                sourceComponent: ToolTip {
                    delay: Qt.styleHints.mousePressAndHoldInterval
                    visible: hoverArea && hoverArea.containsMouse && theLabel.truncated
                    text: theLabel.text
                }
            }
        }
    }
}
