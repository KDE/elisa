/*
   SPDX-FileCopyrightText: 2020 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
