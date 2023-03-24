/*
   SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import org.kde.elisa 1.0
import QtQuick.Controls 2.3

MenuItem {
    required property int mode
    checkable: true
    checked: ElisaApplication.mediaPlayListProxyModel.repeatMode === mode
    onTriggered: ElisaApplication.mediaPlayListProxyModel.repeatMode = mode
}
