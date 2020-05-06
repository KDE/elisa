/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
import org.kde.kirigami 2.5 as Kirigami

Kirigami.Heading {
    id: theLabel

    level: 5

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
