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
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtQuick.Layouts 1.2

Item {
    id: topItem

    property var albumName
    property var artistName
    property var tracksCount
    property var albumArtUrl

    Theme {
        id: elisaTheme
    }

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        Image {
            id: albumIcon
            source: albumArtUrl
            Layout.preferredWidth: width
            Layout.preferredHeight: height
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.maximumWidth: width
            Layout.maximumHeight: height
            width: Screen.pixelDensity * 45.
            height: Screen.pixelDensity * 45.
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit
        }

        Item {
            Layout.preferredHeight: Screen.pixelDensity * 0.5
            Layout.minimumHeight: Screen.pixelDensity * 0.5
            Layout.maximumHeight: Screen.pixelDensity * 0.5
        }

        LabelWithToolTip {
            id: titleLabel

            text: if (albumName !== undefined)
                      albumName
                  else
                      ''

            font.weight: Font.Bold
            color: myPalette.text

            horizontalAlignment: Text.AlignHCenter

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

            elide: "ElideRight"
        }

        LabelWithToolTip {
            id: artistLabel

            text: if (artistName !== undefined)
                      artistName
                  else
                      ''

            font.weight: Font.Normal
            color: myPalette.text

            horizontalAlignment: Text.AlignHCenter

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

            elide: "ElideRight"
        }

        LabelWithToolTip {
            id: numberLabel

            text: i18np("1 track", "%1 track", tracksCount)

            visible: tracksCount !== undefined

            font.weight: Font.Light
            color: myPalette.text

            horizontalAlignment: Text.AlignHCenter

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

            elide: "ElideRight"
        }

        Item {
            Layout.preferredHeight: Screen.pixelDensity * 2.
            Layout.minimumHeight: Screen.pixelDensity * 2.
            Layout.maximumHeight: Screen.pixelDensity * 2.
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: Screen.pixelDensity * 2.

            spacing: 0

            Item {
                Layout.preferredWidth: Screen.pixelDensity * 2.
                Layout.minimumWidth: Screen.pixelDensity * 2.
                Layout.maximumWidth: Screen.pixelDensity * 2.
            }

            Image {
                id: artistJumpIcon

                source: Qt.resolvedUrl(elisaTheme.artistImage)

                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                Layout.maximumWidth: width
                Layout.maximumHeight: height

                visible: artistName !== undefined
                width: Screen.pixelDensity * 8.
                height: Screen.pixelDensity * 8.

                sourceSize.width: width
                sourceSize.height: width

                fillMode: Image.PreserveAspectFit
            }

            LabelWithToolTip {
                text: if (artistName !== undefined)
                          artistName
                      else
                          ''

                font.weight: Font.Normal
                color: myPalette.text

                horizontalAlignment: Text.AlignLeft
            }
        }
    }
}
