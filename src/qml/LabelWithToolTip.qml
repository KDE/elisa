/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import org.kde.kirigami 2.5 as Kirigami

Kirigami.Heading {
    id: theLabel

    level: 5

    textFormat: Text.PlainText
    elide: Text.ElideRight

    Loader {
        id: mouseArea
        anchors.fill: parent
        active: theLabel.truncated
        sourceComponent: MouseArea {
            hoverEnabled: true
            acceptedButtons: Qt.NoButton
        }
    }
    ToolTip.visible: mouseArea.active ? mouseArea.item.containsMouse : false
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.text: theLabel.text
}
