/*
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import Qt.labs.platform
import org.kde.kirigami as Kirigami

MenuItem {
    property Kirigami.Action action

    text: action.text
    shortcut: action.shortcut
    icon.name: action.icon.name
    onTriggered: action.trigger()
    visible: action.visible
}
