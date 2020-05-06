/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.0
import org.kde.elisa 1.0

Item {
    id: rootItem

    property var playListModel
    property var playListControler
    property var audioPlayerManager
    property var player
    property var headerBarManager
    property var manageMediaPlayerControl

    signal raisePlayer()
}
