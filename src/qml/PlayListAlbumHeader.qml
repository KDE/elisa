/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3

import org.kde.elisa 1.0

import QtQuick 2.0

Rectangle {
    property alias headerData: albumHeader.headerData

    color: myPalette.midlight

    BasicPlayListAlbumHeader {
        id: albumHeader

        anchors {
            fill: parent
            topMargin: elisaTheme.layoutVerticalMargin * 1.5
            bottomMargin: elisaTheme.layoutVerticalMargin * 1.5
            leftMargin: elisaTheme.layoutHorizontalMargin
            rightMargin: elisaTheme.layoutHorizontalMargin
        }
    }
}
