/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

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
    ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
    ToolTip.text: theLabel.text
}
