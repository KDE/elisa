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

RowLayout {
    id: headerRow

    property var headerData
    property string album: headerData[0]
    property string albumArtist: headerData[1]
    property url imageUrl: headerData[2]
    property alias textColor: mainLabel.color

    TextMetrics {
        id: trackNumberSize

        text: (99).toLocaleString(Qt.locale(), 'f', 0)
    }

    TextMetrics {
        id: fakeDiscNumberSize

        text: '/9'
    }

    spacing: elisaTheme.layoutHorizontalMargin

    Item {
        property int widthToTrackNumber: elisaTheme.playListDelegateHeight +
                                         elisaTheme.layoutHorizontalMargin +
                                         (trackNumberSize.boundingRect.width - trackNumberSize.boundingRect.x) +
                                         (fakeDiscNumberSize.boundingRect.width - fakeDiscNumberSize.boundingRect.x)

        Layout.minimumWidth: widthToTrackNumber
        Layout.maximumWidth: widthToTrackNumber
        Layout.preferredWidth: widthToTrackNumber
        Layout.fillHeight: true

        Image {
            id: mainIcon

            source: (imageUrl != '' ? imageUrl : Qt.resolvedUrl(elisaTheme.defaultAlbumImage))

            anchors.right: parent.right
            width:  headerRow.height
            height: headerRow.height
            sourceSize.width: headerRow.height
            sourceSize.height: headerRow.height

            fillMode: Image.PreserveAspectFit
            asynchronous: true

            opacity: 1
        }
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
            font.pointSize: Math.round(elisaTheme.defaultFontPointSize * 1.4)

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
            color: mainLabel.color

            horizontalAlignment: Text.AlignLeft

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

            elide: Text.ElideRight
        }
    }
}
