/*
   SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
   SPDX-FileCopyrightText: 2024 Bart De Vries <bart@mogwai.be>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import Qt.labs.platform 1.1
import org.kde.elisa

MenuItem {
    property int mode: -1
    checkable: true
    checked: ElisaApplication.mediaPlayListProxyModel.shuffleMode == mode
    onTriggered: ElisaApplication.mediaPlayListProxyModel.shuffleMode = mode
}
