/*
 * Copyright 2018 Alexander Stippich <a.stippich@gmx.net>
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
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import org.kde.elisa 1.0

Button {
    id: flatButtonWithToolTip

    activeFocusOnTab: true

    Keys.onReturnPressed: action.trigger()

    contentItem: Image {
        anchors.fill: parent

        source: flatButtonWithToolTip.action.icon.name != "" ? ('image://icon/' + flatButtonWithToolTip.action.icon.name) : Qt.resolvedUrl(flatButtonWithToolTip.action.icon.source)

        sourceSize.width: flatButtonWithToolTip.width
        sourceSize.height: flatButtonWithToolTip.height

        fillMode: Image.PreserveAspectFit
        opacity: flatButtonWithToolTip.action.enabled ? 1 : 0.6
    }

    background: Rectangle {
        color: parent.pressed ? myPalette.highlight : "transparent"
        border.color: (parent.hovered || parent.activeFocus) ? myPalette.highlight : "transparent"
        border.width: 1
    }

    ToolTip.visible: hovered
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.text: flatButtonWithToolTip.action.text
}
