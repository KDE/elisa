/*
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Templates 2.15 as T
import org.kde.kirigami 2.15 as Kirigami
import org.kde.elisa 1.0

ToolButton {
    property T.Popup menu

    display: AbstractButton.IconOnly

    ToolTip.visible: hovered
                     && text.length > 0
                     && display === AbstractButton.IconOnly
                     && (menu === null || !menu.visible)
    ToolTip.delay: Kirigami.Units.toolTipDelay
    ToolTip.text: text

    Keys.onReturnPressed: action ? action.trigger() : clicked()
    Keys.onEnterPressed: action ? action.trigger() : clicked()
    Accessible.onPressAction: clicked()
}
