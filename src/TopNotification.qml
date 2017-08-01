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
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4

Rectangle {
    id: topItem

    property bool notificationActive : false

    Layout.preferredHeight: noTrackNotification.notificationActive ? elisaTheme.delegateHeight * 2 : 0
    visible: Layout.preferredHeight > 0

    color: myPalette.mid

    RowLayout {
        anchors.fill: parent

        Label {
            font.pixelSize: elisaTheme.defaultFontPixelSize * 1.5
            text: i18nc("No track found message", "No track have been found")

            Layout.leftMargin: elisaTheme.layoutHorizontalMargin
            Layout.alignment: Qt.AlignHCenter

            visible: topItem.height > height
            opacity: (topItem.height - height) / height
        }

        Button {
            id: configureListenerButton

            text: i18nc("general configuration menu entry", "Configure Elisa...")
            iconName: 'configure'

            Layout.leftMargin: elisaTheme.layoutHorizontalMargin
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumHeight: elisaTheme.delegateHeight

            visible: topItem.height > height
            opacity: (topItem.height - height) / height

            onClicked: configureAction.trigger()
        }
        Item {
            Layout.fillWidth: true
        }
    }

    ToolButton {
        anchors.top: parent.top
        anchors.right: parent.right

        iconName: 'dialog-close'

        onClicked: notificationActive = false
    }

    Behavior on Layout.preferredHeight {
        NumberAnimation {
            duration: 300
        }
    }
}
