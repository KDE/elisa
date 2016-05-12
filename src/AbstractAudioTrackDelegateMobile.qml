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

import QtQuick 2.4
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.2
import org.kde.kirigami 1.0 as MobileComponents

Row {
    id: rowItem

    property alias title: mainLabel.text
    property string artist
    property string album
    property alias itemDecoration: mainIcon.source
    property alias duration: durationLabel.text

    spacing: Screen.pixelDensity * 0.25
    height: Screen.pixelDensity * 12.

    Image {
        id: mainIcon

        source: itemDecoration

        height: parent.height
        width: parent.height
        sourceSize.width: height
        sourceSize.height: height
        fillMode: Image.PreserveAspectFit

        verticalAlignment: Image.AlignVCenter
        horizontalAlignment: Image.AlignHCenter
    }

    Column {
        Label {
            id: mainLabel
            text: title
            elide: "ElideRight"
            width: viewAlbumDelegate.width - mainIcon.width - durationLabel.width - Screen.pixelDensity * 0.5
        }

        Label {
            id: authorLabel
            text: artist + ' - ' + album
            elide: "ElideRight"
            verticalAlignment: Text.AlignTop
            width: viewAlbumDelegate.width - mainIcon.width - durationLabel.width - Screen.pixelDensity * 0.5

            anchors.verticalCenter: parent.center
        }
    }

    Label {
        id: durationLabel
        text: duration
        elide: "ElideRight"
        width: Screen.pixelDensity * 9.
    }
}
