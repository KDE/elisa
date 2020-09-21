/*
   SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import Qt.labs.platform 1.1
import org.kde.elisa 1.0

MenuItem {
    property int mode: -1
    checkable: true
    checked: ElisaApplication.mediaPlayListProxyModel.repeatMode == mode
    onTriggered: ElisaApplication.mediaPlayListProxyModel.repeatMode = mode
}
