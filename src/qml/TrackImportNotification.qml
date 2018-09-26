/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
import QtQuick.Controls 2.2
import org.kde.elisa 1.0

Rectangle {
    id: rootComponent

    property bool indexingRunning
    property int importedTracksCount
    property MusicListenersManager musicManager

    color: myPalette.highlight

    width: Math.max(elisaTheme.gridDelegateWidth * 1.5, labelWidth.width + 2 * elisaTheme.layoutHorizontalMargin)
    height: elisaTheme.smallControlButtonSize * 1.5

    visible: opacity > 0
    opacity: (indexingRunning ? 1 : 0)

    Label {
        id: importedTracksCountLabel
        anchors.centerIn: parent
        text: i18ncp("number of imported tracks", "Imported one track", "Imported %1 tracks", importedTracksCount)
        color: myPalette.highlightedText
    }

    TextMetrics {
        id: labelWidth

        text: i18ncp("number of imported tracks", "Imported one track", "Imported %1 tracks", 999999)
    }

    Timer {
        id: hideTimer

        interval: 6000
        repeat: false

        onTriggered:
        {
            rootComponent.opacity = 0
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
