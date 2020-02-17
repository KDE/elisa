/*
 * Copyright 2020 Alexander Stippich <a.stippich@gmx.net>
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

Item {
    id: imageWithFallback
    property url source
    property url fallback
    property bool usingFallback: false
    property alias sourceSize: image.sourceSize
    property alias fillMode: image.fillMode
    property alias asynchronous: image.asynchronous
    property alias mipmap: image.mipmap

    implicitWidth: image.width
    implicitHeight: image.height

    onSourceChanged: {
        image.source = getImageUrl()
    }

    function getImageUrl() {
        if (source == '' || source === undefined) {
            usingFallback = true;
            return fallback;
        } else {
            usingFallback = false;
            return source;
        }
    }

    Image {
        id: image
        anchors.fill: parent
        source: imageWithFallback.getImageUrl()

        onStatusChanged: {
            if (status === Image.Error) {
                imageWithFallback.usingFallback = true;
                image.source = imageWithFallback.fallback;
            }
        }
    }
}
