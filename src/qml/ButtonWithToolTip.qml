/*
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Templates 2.15 as T
import org.kde.kirigami 2.15 as Kirigami
import org.kde.elisa 1.0

// Keep in sync with FlatButtonWithToolTip
Button {
    property T.Popup menu

    readonly property bool __exclusive: autoExclusive || (ButtonGroup.group?.exclusive ?? false)

    ToolTip.visible: hovered
                     && text.length > 0
                     && display === AbstractButton.IconOnly
                     && (menu === null || !menu.visible)
    ToolTip.delay: Kirigami.Units.toolTipDelay
    ToolTip.text: text

    Keys.onReturnPressed: __emulateClick()
    Keys.onEnterPressed: __emulateClick()
    Accessible.onPressAction: clicked()

    function __toggleChecked() {
        if (!checkable) {
            return
        }
        if (!__exclusive) {
            toggle()
            return
        }
        if (!checked) {
            toggle()
        }
    }

    function __trigger() {
        if (action) {
            action.trigger()
        } else {
            clicked()
        }
    }

    function __emulateClick() {
        __toggleChecked()
        __trigger()
    }
}
