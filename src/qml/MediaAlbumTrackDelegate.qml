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

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        Rectangle {
            Layout.preferredHeight: elisaTheme.delegateHeight
            Layout.minimumHeight: elisaTheme.delegateHeight
            Layout.maximumHeight: elisaTheme.delegateHeight
            Layout.fillWidth: true
            color: (mediaTrack.isAlternateColor ? myPalette.alternateBase : myPalette.base)

            visible: mediaTrack.isFirstTrackOfDisc && !mediaTrack.isSingleDiscAlbum

            LabelWithToolTip {
                id: discHeaderLabel

                text: 'CD ' + mediaTrack.discNumber

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
