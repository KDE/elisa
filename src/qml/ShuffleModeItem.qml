/*
   SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
   SPDX-FileCopyrightText: 2024 Bart De Vries <bart@mogwai.be>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import org.kde.elisa
import QtQuick.Controls 2.3

MenuItem {
    required property int mode
    autoExclusive: true
    checkable: true
    checked: ElisaApplication.mediaPlayListProxyModel.shuffleMode === mode
    onTriggered: ElisaApplication.mediaPlayListProxyModel.shuffleMode = mode
}
