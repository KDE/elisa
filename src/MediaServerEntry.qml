/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.0
import QtQuick.Controls 1.2

Item {
    property string imageName
    property string entryTitle

    signal clicked()

    MouseArea {
        id: clickHandle

        width: parent.width
        height: parent.height

        onClicked: {
            parent.clicked()
        }
    }

    Column {
        Image {
            id: mainIcon
            source: imageName
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit

            width: parent.width * 0.9
            height: parent.width * 0.9
        }
        Label {
            id: mainLabel
            text: entryTitle
            width: parent.width - 5
            elide: "ElideRight"
        }
    }
}

