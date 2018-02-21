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
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import org.kde.elisa 1.0

Rectangle {
    id: rootComponent

    property bool indexingRunning
    property int importedTracksCount
    property MusicListenersManager musicManager

    color: myPalette.highlight

    width: elisaTheme.gridDelegateWidth * 1.5

    visible: opacity > 0
    opacity: (indexingRunning ? 1 : 0)

    Label {
        anchors.centerIn: parent
        text: i18ncp("number of imported tracks", "Imported one track", "Imported %1 tracks", importedTracksCount)
        color: myPalette.highlightedText
    }

    Timer {
        id: hideTimer

        interval: 6000
        repeat: false

        onTriggered:
        {
            rootComponent.opacity = 0
            musicManager.resetImportedTracksCounter()
        }
    }

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.InOutQuad
            duration: 300
        }
    }

    onIndexingRunningChanged:
        if (indexingRunning) {
            hideTimer.stop()
            opacity = 1
        } else {
            hideTimer.start()
        }
}
