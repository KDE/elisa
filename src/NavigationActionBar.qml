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

import QtQml 2.2
import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Window 2.0

Item {
    id: navigationBar

    property var parentStackView
    property var playList
    property var playerControl
    property string artist
    property string album
    property string image
    property string tracksCount
    property var enqueueAction
    property var clearAndEnqueueAction

    Action {
        id: goPreviousAction
        text: i18nc("navigate back in the views stack", "Back")
        iconName: "go-previous"
        onTriggered:
        {
            if (listingView.depth > 1) {
                listingView.pop()
            } else {
                parentStackView.pop()
            }
        }
    }

    RowLayout {
        anchors.fill: parent

        spacing: 0

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }

        ToolButton {
            action: goPreviousAction
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }

        Image {
            id: mainIcon
            source: image
            fillMode: Image.PreserveAspectFit

            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

            Layout.preferredHeight: Screen.pixelDensity * 20.
            Layout.minimumHeight: Screen.pixelDensity * 20.
            Layout.maximumHeight: Screen.pixelDensity * 20.
            Layout.preferredWidth: Screen.pixelDensity * 20.
            Layout.minimumWidth: Screen.pixelDensity * 20.
            Layout.maximumWidth: Screen.pixelDensity * 20.
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }

        ColumnLayout {
            Layout.fillHeight: true

            spacing: 0

            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.preferredWidth: Screen.pixelDensity * 40.
            Layout.minimumWidth: Screen.pixelDensity * 40.
            Layout.maximumWidth: Screen.pixelDensity * 40.

            Item {
                Layout.fillHeight: true
            }

            Text {
                id: authorLabel

                text: artist

                font.weight: Font.Bold
                color: myPalette.text

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft

                elide: "ElideRight"
            }

            Item {
                Layout.fillHeight: true
            }

            Text {
                id: albumLabel

                text: album

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft

                elide: "ElideRight"

                color: myPalette.text
            }

            Item {
                Layout.fillHeight: true
            }
        }

        Item {
            Layout.fillWidth: true
        }

        ToolButton {
            action: enqueueAction
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }

        ToolButton {
            action: clearAndEnqueueAction
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }
    }
}
