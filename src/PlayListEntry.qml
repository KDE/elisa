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
    property alias itemDecoration : mainIcon.source
    property alias duration : durationLabel.text
    property int trackNumber
    property alias isPlaying : playIcon.visible
    property bool isSelected
    property bool hasAlbumHeader

    signal clicked()

    Rectangle {
        id: contentLayout

        anchors.top: viewAlbumDelegate.top
        anchors.left: viewAlbumDelegate.left
        anchors.right: viewAlbumDelegate.right
        anchors.bottom: viewAlbumDelegate.bottom

        SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

        color: (isSelected ? myPalette.highlight : myPalette.base)

        ColumnLayout {
            spacing: 0

            anchors.fill: parent
            anchors.leftMargin: Screen.pixelDensity * 1.5
            anchors.rightMargin: Screen.pixelDensity * 5.5
            anchors.topMargin: 0
            anchors.bottomMargin: 1

            RowLayout {
                id: headerRow

                spacing: Screen.pixelDensity * 1.5

                Layout.fillWidth: true
                Layout.preferredHeight: Screen.pixelDensity * 15.
                Layout.minimumHeight: Screen.pixelDensity * 15.
                Layout.maximumHeight: Screen.pixelDensity * 15.

                visible: hasAlbumHeader

                Image {
                    id: mainIcon
                    source: itemDecoration
                    Layout.preferredWidth: parent.height - 2
                    Layout.preferredHeight: parent.height - 2
                    width: parent.height - 2
                    height: parent.height - 2
                    sourceSize.width: parent.height - 2
                    sourceSize.height: parent.height - 2
                    fillMode: Image.PreserveAspectFit
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    spacing: 0

                    Item {
                        height: Screen.pixelDensity * 1.5
                    }

                    Label {
                        id: mainLabel
                        text: album
                        font.weight: Font.Bold
                        horizontalAlignment: "AlignHCenter"
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        elide: "ElideRight"
                    }

                    Item {
                        Layout.fillHeight: true
                    }

                    Label {
                        id: authorLabel
                        text: artist
                        font.weight: Font.Light
                        horizontalAlignment: "AlignHCenter"
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        elide: "ElideRight"
                    }

                    Item {
                        height: Screen.pixelDensity * 1.5
                    }
                }
            }

            Item {
                Layout.preferredHeight: (hasAlbumHeader ? Screen.pixelDensity * 1.5 : 0)
                Layout.minimumHeight: (hasAlbumHeader ? Screen.pixelDensity * 1.5 : 0)
                Layout.maximumHeight: (hasAlbumHeader ? Screen.pixelDensity * 1.5 : 0)

                visible: hasAlbumHeader
            }


            RowLayout {
                id: trackRow

                Layout.fillWidth: true
                Layout.preferredHeight: Screen.pixelDensity * 5.
                Layout.minimumHeight: Screen.pixelDensity * 5.
                Layout.maximumHeight: Screen.pixelDensity * 5.

                spacing: Screen.pixelDensity * 1.

                Label {
                    id: mainCompactLabel

                    text: trackNumber + ' - ' + title
                    font.weight: Font.Bold

                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                    elide: "ElideRight"
                }

                Image {
                    id: playIcon

                    Layout.preferredWidth: parent.height * 1
                    Layout.preferredHeight: parent.height * 1
                    Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                    Layout.maximumWidth: parent.height * 1
                    Layout.maximumHeight: parent.height * 1

                    source: 'image://icon/media-playback-start'
                    width: parent.height * 1.
                    height: parent.height * 1.
                    sourceSize.width: parent.height * 1.
                    sourceSize.height: parent.height * 1.
                    fillMode: Image.PreserveAspectFit
                    visible: isPlaying
                }

                Label {
                    id: durationLabel
                    text: duration
                    font.weight: Font.Bold
                    elide: "ElideRight"
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                }
            }
        }
    }

    Rectangle {
        id: entrySeparatorItem

        border.width: 0.5
        border.color: "#DDDDDD"
        color: "#DDDDDD"
        visible: true

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        anchors.leftMargin: Screen.pixelDensity * 0.5
        anchors.rightMargin: Screen.pixelDensity * 0.5

        height: 1
    }

    MouseArea {
        id: hoverHandle

        acceptedButtons: Qt.LeftButton

        propagateComposedEvents: true

        anchors.fill: parent

        onClicked: viewAlbumDelegate.clicked()
    }
}

