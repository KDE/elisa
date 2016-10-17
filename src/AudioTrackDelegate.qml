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

import QtQuick 2.4
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import org.mgallien.QmlExtension 1.0

Item {
    id: viewAlbumDelegate

    property string title
    property string artist
    property string album
    property alias duration : durationLabel.text
    property int trackNumber
    property var databaseId
    property var playList

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 2
            Layout.minimumWidth: Screen.pixelDensity * 2
            Layout.maximumWidth: Screen.pixelDensity * 2
            width: Screen.pixelDensity * 2
        }

        ColumnLayout {
            Layout.preferredHeight: viewAlbumDelegate.height

            Layout.fillWidth: true

            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            spacing: 0

            Item {
                Layout.preferredHeight: Screen.pixelDensity * 3.
                Layout.minimumHeight: Screen.pixelDensity * 3.
                Layout.maximumHeight: Screen.pixelDensity * 3.
            }

            Label {
                id: mainLabel
                text: trackNumber + ' - ' + title
                font.weight: Font.Bold
                Layout.preferredWidth: Screen.pixelDensity * 9
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                elide: "ElideRight"
            }

            Item {
                Layout.fillHeight: true
            }

            Label {
                id: durationLabel
                text: duration
                font.weight: Font.Light
                Layout.preferredWidth: Screen.pixelDensity * 3
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                elide: "ElideRight"
            }

            Item {
                Layout.preferredHeight: Screen.pixelDensity * 3.
                Layout.minimumHeight: Screen.pixelDensity * 3.
                Layout.maximumHeight: Screen.pixelDensity * 3.
            }
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
        }

        Image {
            id: addAlbum
            MouseArea {
                anchors.fill: parent
                onClicked: playList.enqueue(databaseId)
            }
            source: 'image://icon/media-track-add-amarok'
            Layout.preferredWidth: Screen.pixelDensity * 8.
            Layout.preferredHeight: Screen.pixelDensity * 8.
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: Screen.pixelDensity * 8.
            Layout.maximumHeight: Screen.pixelDensity * 8.
            Layout.minimumWidth: Screen.pixelDensity * 8.
            Layout.minimumHeight: Screen.pixelDensity * 8.
            sourceSize.width: Screen.pixelDensity * 8.
            sourceSize.height: Screen.pixelDensity * 8.
            height: Screen.pixelDensity * 8.
            width: Screen.pixelDensity * 8.
            fillMode: Image.PreserveAspectFit
            enabled: true
            opacity: enabled ? 1.0 : 0.6
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }

        Image {
            id: clearAddAlbum
            MouseArea {
                anchors.fill: parent
                onClicked: playList.clearAndEnqueue(databaseId)
            }
            source: 'image://icon/media-playback-start'
            Layout.preferredWidth: Screen.pixelDensity * 8.
            Layout.preferredHeight: Screen.pixelDensity * 8.
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: Screen.pixelDensity * 8.
            Layout.maximumHeight: Screen.pixelDensity * 8.
            Layout.minimumWidth: Screen.pixelDensity * 8.
            Layout.minimumHeight: Screen.pixelDensity * 8.
            sourceSize.width: Screen.pixelDensity * 8.
            sourceSize.height: Screen.pixelDensity * 8.
            height: Screen.pixelDensity * 8.
            width: Screen.pixelDensity * 8.
            fillMode: Image.PreserveAspectFit
            enabled: true
            opacity: enabled ? 1.0 : 0.6
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 6.
            Layout.minimumWidth: Screen.pixelDensity * 6.
            Layout.maximumWidth: Screen.pixelDensity * 6.
        }
    }
}

