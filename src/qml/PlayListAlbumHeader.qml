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
    property var headerData
    property string album: headerData[0]
    property string albumArtist: headerData[1]
    property url imageUrl: headerData[2]

    color: myPalette.midlight

    TextMetrics {
        id: trackNumberSize

        text: (99).toLocaleString(Qt.locale(), 'f', 0)
    }

    TextMetrics {
        id: fakeDiscNumberSize

        text: '/9'
    }

    RowLayout {
        id: headerRow

        spacing: elisaTheme.layoutHorizontalMargin

        anchors.fill: parent
        anchors.topMargin: elisaTheme.layoutVerticalMargin * 1.5
        anchors.bottomMargin: elisaTheme.layoutVerticalMargin * 1.5

        Image {
            id: mainIcon

            source: (imageUrl != '' ? imageUrl : Qt.resolvedUrl(elisaTheme.defaultAlbumImage))

            Layout.minimumWidth: headerRow.height
            Layout.maximumWidth: headerRow.height
            Layout.preferredWidth: headerRow.height
            Layout.minimumHeight: headerRow.height
            Layout.maximumHeight: headerRow.height
            Layout.preferredHeight: headerRow.height
            Layout.leftMargin: !LayoutMirroring.enabled ?
                                   (elisaTheme.smallDelegateToolButtonSize +
                                    trackNumberSize.width +
                                    fakeDiscNumberSize.width +
                                    (elisaTheme.layoutHorizontalMargin * 5 / 4) -
                                    headerRow.height) :
                                   0
            Layout.rightMargin: LayoutMirroring.enabled ?
                                    (elisaTheme.smallDelegateToolButtonSize +
                                     trackNumberSize.width +
                                     fakeDiscNumberSize.width +
                                     (elisaTheme.layoutHorizontalMargin * 5 / 4) -
                                     headerRow.height) :
                                    0
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

            sourceSize.width: headerRow.height
            sourceSize.height: headerRow.height

            fillMode: Image.PreserveAspectFit
            asynchronous: true

            opacity: 1
        }

        ColumnLayout {
            id: albumHeaderTextColumn

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: !LayoutMirroring.enabled ? - elisaTheme.layoutHorizontalMargin / 4 : 0
            Layout.rightMargin: LayoutMirroring.enabled ? - elisaTheme.layoutHorizontalMargin / 4 : 0

            spacing: 0

            LabelWithToolTip {
                id: mainLabel

                text: album

                font.weight: Font.Bold
                font.pointSize: elisaTheme.defaultFontPointSize * 1.4
                color: myPalette.text

                horizontalAlignment: Text.AlignLeft

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                Layout.topMargin: elisaTheme.layoutVerticalMargin

                elide: Text.ElideRight
            }

            Item {
                Layout.fillHeight: true
            }

            LabelWithToolTip {
                id: authorLabel

                text: albumArtist

                font.weight: Font.Light
                color: myPalette.text

                horizontalAlignment: Text.AlignLeft

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                Layout.bottomMargin: elisaTheme.layoutVerticalMargin

                elide: Text.ElideRight
            }
        }
    }
}
