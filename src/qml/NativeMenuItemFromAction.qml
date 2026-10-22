/*
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import Qt.labs.platform
import org.kde.kirigami as Kirigami
import org.kde.elisa

MenuItem {
    property string actionName

    text: ElisaApplication.action(actionName).text
    onTriggered: ElisaApplication.action(actionName).trigger()
}
