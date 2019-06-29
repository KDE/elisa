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
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import org.kde.elisa 1.0

FocusScope {
    id: topItem

    signal close()
    signal mainButtonClicked()
    signal secondaryButtonClicked()
    signal entered()

    property alias itemMessage: notificationText.text
    property alias itemMainButtonText: mainButton.text
    property alias itemMainButtonIconName: mainButton.icon.name
    property alias itemSecondaryButtonText: secondaryButton.text
    property alias itemSecondaryButtonIconName: secondaryButton.icon.name
    property var parentList

    MouseArea {
        anchors.fill: parent

        hoverEnabled: true
        acceptedButtons: Qt.NoButton

        onEntered: topItem.entered()
    }

    RowLayout {
        id: content
        anchors.fill: parent

        Label {
            id: notificationText
            font.pointSize: Math.round(elisaTheme.defaultFontPoinPoint * 1.5)

            Layout.leftMargin: elisaTheme.layoutHorizontalMargin
            Layout.alignment: Qt.AlignHCenter

            visible: topItem.height > height
            opacity: (topItem.height - height) / height
        }

        Button {
            id: mainButton

            Layout.leftMargin: elisaTheme.layoutHorizontalMargin
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumHeight: elisaTheme.delegateHeight

            visible: text !== "" && topItem.height > height
            opacity: (topItem.height - height) / height

            onClicked: {
                mainButton.enabled = false
                enableAgainMainButtonTimer.start()
                mainButtonClicked()
            }
            Accessible.onPressAction: onClicked

            Timer {
                id: enableAgainMainButtonTimer
                interval: 500
                onTriggered: mainButton.enabled = true
            }
        }

        Button {
            id: secondaryButton

            Layout.leftMargin: elisaTheme.layoutHorizontalMargin
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumHeight: elisaTheme.delegateHeight

            visible: text !== "" && topItem.height > height
            opacity: (topItem.height - height) / height

            onClicked: {
                secondaryButton.enabled = false
                enableAgainSecondaryButtonTimer.start()
                secondaryButtonClicked()
            }
            Accessible.onPressAction: onClicked

            Timer {
                id: enableAgainSecondaryButtonTimer
                interval: 500
                onTriggered: secondaryButton.enabled = true
            }
        }

        Item {
            Layout.fillWidth: true
        }
    }

    FlatButtonWithToolTip {
        anchors.top: parent.top
        anchors.right: parent.right
        icon.name: 'dialog-close'

        visible: topItem.height > height
        opacity: (topItem.height - height) / height

        onClicked: close()
        Accessible.onPressAction: onClicked
    }
}
