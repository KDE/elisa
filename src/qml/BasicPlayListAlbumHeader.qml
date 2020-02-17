/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.0

Rectangle {
    id: background

    property var headerData
    property string album: headerData[0]
    property string albumArtist: headerData[1]
    property url imageUrl: headerData[2]
    property alias textColor: mainLabel.color
    property alias backgroundColor: background.color

    implicitHeight: contentLayout.implicitHeight

    color: myPalette.midlight

    RowLayout {
        id: contentLayout
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        // No bottom anchor so it can grow vertically

        spacing: elisaTheme.layoutHorizontalMargin / 4

        ImageWithFallback {
            Layout.preferredWidth: elisaTheme.playListAlbumArtSize
            Layout.preferredHeight: elisaTheme.playListAlbumArtSize
            Layout.margins: elisaTheme.layoutHorizontalMargin

            source: imageUrl
            fallback: elisaTheme.defaultAlbumImage

            sourceSize.width: elisaTheme.playListAlbumArtSize
            sourceSize.height: elisaTheme.playListAlbumArtSize

            fillMode: Image.PreserveAspectFit
            asynchronous: true
        }

        ColumnLayout {
            id: albumHeaderTextColumn

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: !LayoutMirroring.enabled ? - elisaTheme.layoutHorizontalMargin / 4 : 0
            Layout.rightMargin: LayoutMirroring.enabled ? - elisaTheme.layoutHorizontalMargin / 4 : 0
            Layout.topMargin: elisaTheme.layoutVerticalMargin
            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

            spacing: elisaTheme.layoutVerticalMargin

            LabelWithToolTip {
                id: mainLabel

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBottom | Qt.AlignLeft

                text: album
                level: 2
                font.weight: Font.Bold

                elide: Text.ElideRight
                wrapMode: Text.WordWrap
                maximumLineCount: 2
            }

            LabelWithToolTip {
                id: authorLabel

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop | Qt.AlignLeft

                text: albumArtist
                color: mainLabel.color

                elide: Text.ElideRight
                wrapMode: Text.WordWrap
                maximumLineCount: 2
            }
        }
    }
}
