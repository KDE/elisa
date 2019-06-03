/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
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

FocusScope {
    id: albumTrack

    property alias mediaTrack: mediaTrack
    property alias databaseId: mediaTrack.databaseId
    property alias title: mediaTrack.title
    property alias artist: mediaTrack.artist
    property alias album: mediaTrack.album
    property alias albumArtist: mediaTrack.albumArtist
    property alias duration: mediaTrack.duration
    property alias imageUrl: mediaTrack.imageUrl
    property alias trackNumber: mediaTrack.trackNumber
    property alias discNumber: mediaTrack.discNumber
    property alias rating: mediaTrack.rating
    property alias isFirstTrackOfDisc: mediaTrack.isFirstTrackOfDisc
    property alias isSingleDiscAlbum: mediaTrack.isSingleDiscAlbum
    property alias isSelected: mediaTrack.isSelected
    property alias isAlternateColor: mediaTrack.isAlternateColor

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        Rectangle {
            Layout.preferredHeight: elisaTheme.delegateHeight
            Layout.minimumHeight: elisaTheme.delegateHeight
            Layout.maximumHeight: elisaTheme.delegateHeight
            Layout.fillWidth: true
            color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)

            visible: isFirstTrackOfDisc && !isSingleDiscAlbum

            LabelWithToolTip {
                id: discHeaderLabel

                text: 'CD ' + discNumber

                font.weight: Font.Bold
                font.italic: true
                color: myPalette.text

                anchors.fill: parent
                anchors.topMargin: elisaTheme.layoutVerticalMargin
                anchors.leftMargin: elisaTheme.layoutHorizontalMargin

                elide: Text.ElideRight
            }
        }

        MediaTrackDelegate {
            id: mediaTrack

            Layout.preferredHeight: elisaTheme.delegateHeight
            Layout.minimumHeight: elisaTheme.delegateHeight
            Layout.maximumHeight: elisaTheme.delegateHeight
            Layout.fillWidth: true

            focus: true

            detailedView: false
        }
    }
}
