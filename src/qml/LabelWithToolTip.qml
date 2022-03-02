/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.2
import org.kde.kirigami 2.5 as Kirigami

Kirigami.Heading {
    id: theLabel

    level: 5

    textFormat: Text.PlainText
    elide: Text.ElideRight

    HoverHandler {
        id: hoverHandler
        enabled: theLabel.truncated
    }
    ToolTip.visible: hoverHandler.enabled ? hoverHandler.hovered : false
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.text: theLabel.text
}
