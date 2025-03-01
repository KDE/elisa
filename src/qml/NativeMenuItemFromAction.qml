/*
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import Qt.labs.platform 1.1
import org.kde.elisa
import org.kde.kirigami as Kirigami

MenuItem {
    property Kirigami.Action action

    text: action.text
    shortcut: action.shortcut
    icon.name: action.icon.name
    onTriggered: action.trigger()
    visible: action.visible
}
