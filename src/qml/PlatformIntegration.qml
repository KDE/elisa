/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

import QtQuick 2.7
import org.kde.elisa 1.0

Item {
    id: rootItem

    property alias playListModel: mpris2Interface.playListModel
    property alias playListControler: mpris2Interface.playListControler
    property alias audioPlayerManager: mpris2Interface.audioPlayerManager
    property alias player: mpris2Interface.audioPlayer
    property alias headerBarManager: mpris2Interface.headerBarManager
    property alias manageMediaPlayerControl: mpris2Interface.manageMediaPlayerControl

    signal raisePlayer()

    Mpris2 {
        id: mpris2Interface

        playerName: 'elisa'

        onRaisePlayer:
        {
            rootItem.raisePlayer()
        }
    }
}
