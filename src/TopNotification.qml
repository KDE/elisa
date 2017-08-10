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
import org.mgallien.QmlExtension 1.0

Rectangle {
    id: topItem

    default property NotificationItem item

    Binding on Layout.preferredHeight {
        when: item && item.active
        value: elisaTheme.delegateHeight * 2
    }

    Binding on Layout.preferredHeight {
        when: !item || !item.active
        value: 0
    }

    visible: Layout.preferredHeight > 0

    color: myPalette.mid

    RowLayout {
        anchors.fill: parent

        Label {
            id: notificationText
            font.pixelSize: elisaTheme.defaultFontPixelSize * 1.5

            text: item ? item.message : ""

            Layout.leftMargin: elisaTheme.layoutHorizontalMargin
            Layout.alignment: Qt.AlignHCenter

            visible: topItem.height > height
            opacity: (topItem.height - height) / height
        }

        Button {
            id: mainButton

            text: item ? item.mainButtonText : ""
            iconName: item ? item.mainButtonIconName : ""

            Layout.leftMargin: elisaTheme.layoutHorizontalMargin
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumHeight: elisaTheme.delegateHeight

            visible: item && item.mainButtonText !== "" && topItem.height > height
            opacity: (topItem.height - height) / height

            onClicked: if (item) item.triggerMainButton()
        }

        Button {
            id: secondaryButton

            text: item ? item.secondaryButtonText : ""
            iconName: item ? item.secondaryButtonIconName: ""

            Layout.leftMargin: elisaTheme.layoutHorizontalMargin
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumHeight: elisaTheme.delegateHeight

            visible: item && item.secondaryButtonText !== "" && topItem.height > height
            opacity: (topItem.height - height) / height

            onClicked: if (item) item.triggerSecondaryButton()
        }

        Item {
            Layout.fillWidth: true
        }
    }

    ToolButton {
        anchors.top: parent.top
        anchors.right: parent.right

        visible: topItem.height > height
        opacity: (topItem.height - height) / height

        iconName: 'dialog-close'

        onClicked: if (item) item.active = false
    }

    Behavior on Layout.preferredHeight {
        NumberAnimation {
            duration: 300
        }
    }
}
