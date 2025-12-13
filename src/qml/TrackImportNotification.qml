/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import org.kde.ki18n
import org.kde.kirigami as Kirigami

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

    readonly property int margins: Kirigami.Units.smallSpacing

    implicitWidth: layout.implicitWidth + (margins * 2)
    implicitHeight: layout.implicitHeight + (margins * 2)

    radius: Kirigami.Units.cornerRadius

    color: Qt.alpha(Kirigami.Theme.activeTextColor, 0.2)
    border.color: Kirigami.Theme.activeTextColor
    border.width: 2

    visible: opacity > 0
    opacity: 0

    RowLayout {
        id: layout

        anchors.centerIn: parent
        spacing: Kirigami.Units.smallSpacing

        BusyIndicator {
            Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
            Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
            Layout.alignment: Qt.AlignVCenter
        }

        Label {
            id: importedTracksCountLabel
            text: {
                if (rootComponent.importedTracksCount > 1) {
                    return KI18n.i18ncp("number of imported tracks",
                                  "Imported one track",
                                  "Imported %1 tracks",
                                  rootComponent.importedTracksCount);
                } else {
                    return KI18n.i18nc("@info:status", "Scanning music");
                }
            }
            color: Kirigami.Theme.textColor

            Layout.fillWidth: true
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
        }
    }

    TextMetrics {
        id: labelWidth

        text: KI18n.i18ncp("number of imported tracks", "Imported one track", "Imported %1 tracks", 999999)
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
