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

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        Item {
            Layout.fillHeight: true
        }

        Image {
            id: albumIcon
            source: albumArtUrl

            Layout.preferredWidth: elisaTheme.coverImageSize
            Layout.preferredHeight: elisaTheme.coverImageSize
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.maximumWidth: elisaTheme.coverImageSize
            Layout.maximumHeight: elisaTheme.coverImageSize
            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

            width: elisaTheme.coverImageSize
            height: elisaTheme.coverImageSize

            sourceSize.width: elisaTheme.coverImageSize
            sourceSize.height: elisaTheme.coverImageSize

            fillMode: Image.PreserveAspectFit
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
            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

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
            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

            elide: "ElideRight"
        }

        Item {
            Layout.fillHeight: true
        }

        RowLayout {
            Layout.fillWidth: true

            spacing: 0

            Image {
                id: artistJumpIcon

                source: Qt.resolvedUrl(elisaTheme.artistImage)

                Layout.preferredWidth: elisaTheme.smallImageSize
                Layout.preferredHeight: elisaTheme.smallImageSize
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                Layout.maximumWidth: elisaTheme.smallImageSize
                Layout.maximumHeight: elisaTheme.smallImageSize
                Layout.leftMargin: elisaTheme.layoutHorizontalMargin

                visible: artistName !== undefined
                width: elisaTheme.smallImageSize
                height: elisaTheme.smallImageSize

                sourceSize.width: elisaTheme.smallImageSize
                sourceSize.height: elisaTheme.smallImageSize

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
