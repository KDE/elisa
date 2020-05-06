/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.4
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

/*
 * This visually mimics the Kirigami.InlineMessage due to the
 * BusyIndicator, which is not supported by the InlineMessage.
 * Consider implementing support for the BusyIndicator within
 * the InlineMessage in the future.
 */
Rectangle {
    id: rootComponent

    property bool indexingRunning
    property int importedTracksCount

    color: Kirigami.Theme.activeTextColor

    width: (labelWidth.boundingRect.width - labelWidth.boundingRect.x) + 3 * Kirigami.Units.largeSpacing +
           indicator.width
    height: indicator.height

    visible: opacity > 0
    opacity: 0

    radius: Kirigami.Units.smallSpacing / 2

    Rectangle {
        id: bgFillRect

        anchors.fill: parent
        anchors.margins: Kirigami.Units.devicePixelRatio

        color: Kirigami.Theme.backgroundColor

        radius: rootComponent.radius * 0.60
    }

    Rectangle {
        anchors.fill: bgFillRect

        color: rootComponent.color

        opacity: 0.20

        radius: bgFillRect.radius
    }

    RowLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing

        BusyIndicator{
            id: indicator
            Layout.alignment: Qt.AlignVCenter
        }

        Label {
            id: importedTracksCountLabel
            text: (importedTracksCount ?
                       i18ncp("number of imported tracks",
                              "Imported one track",
                              "Imported %1 tracks",
                              importedTracksCount) :
                       i18nc("message to show when Elisa is scanning music files", "Scanning music"))
            color: Kirigami.Theme.textColor

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter
        }
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
            duration: Kirigami.Units.longDuration
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
