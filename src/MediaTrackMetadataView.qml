/*
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
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

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQml.Models 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0

Dialog {
    id: trackMetadata

    property string trackTitle
    property string artist
    property string albumArtist
    property string albumName
    property string composer
    property string duration
    property string genre
    property string resource
    property int year
    property int rating: -1
    property int trackNumber
    property int discNumber
    property bool isSingleDiscAlbum

    title: i18nc("Window title for track metadata", "View Details")
    modality: Qt.NonModal

    standardButtons: StandardButton.Close

    Component.onCompleted: {
        if (trackTitle.length !== 0)
            trackList.append({"name": i18nc("Track title for track metadata view", "Title:"), "content": trackTitle})
        if (artist.length !== 0)
            trackList.append({"name": i18nc("Track artist for track metadata view", "Artist:"), "content": artist})
        if (albumName.length !== 0)
            trackList.append({"name": i18nc("Album name for track metadata view", "Album:"), "content": albumName})
        if (composer.length !== 0)
            trackList.append({"name": i18nc("Composer name for track metadata view", "Composer:"), "content": composer})
        if (trackNumber !== 0)
            trackList.append({"name": i18nc("Track number for track metadata view", "Track Number:"), "content": trackNumber + ""})
        if (discNumber !== 0)
            trackList.append({"name": i18nc("Disc number for track metadata view", "Disc Number:"), "content": discNumber + ""})
        if (albumArtist.length !== 0)
            trackList.append({"name": i18nc("Album artist for track metadata view", "Album Artist:"), "content": albumArtist})
        if (duration.length !== 0)
            trackList.append({"name": i18nc("Duration label for track metadata view", "Duration:"), "content": duration})
        if (year !== 0)
            trackList.append({"name": i18nc("Year label for track metadata view", "Year:"), "content": year})
        if (genre.length !== 0)
            trackList.append({"name": i18nc("Genre label for track metadata view", "Genre:"), "content": genre})
        trackData.Layout.preferredHeight = textSize.height * trackData.count
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        ListView {
            id: trackData

            interactive: false
            Layout.fillWidth: true

            model: ListModel {
                id: trackList
            }

            delegate: RowLayout {
                id: delegateRow
                spacing: 0

                Text {
                    text: name
                    color: myPalette.text
                    horizontalAlignment: Text.AlignRight

                    Layout.preferredWidth: trackData.width * 0.3
                    Layout.minimumHeight: textSize.height
                    Layout.rightMargin: elisaTheme.layoutHorizontalMargin
                }

                Text {
                    text: content
                    color: myPalette.text

                    horizontalAlignment: Text.AlignLeft
                    elide: Text.ElideRight

                    Layout.preferredWidth: trackData.width * 0.66
                    Layout.fillWidth: true
                    Layout.minimumHeight: textSize.height
                }
            }
        }

        RowLayout {
            anchors.margins: 0
            spacing: 0

            visible: rating > -1

            Layout.minimumHeight: textSize.height

            Text {
                id: ratingText
                text: i18nc("Rating label for information panel", "Rating:")
                color: myPalette.text

                horizontalAlignment: Text.AlignRight
                Layout.preferredWidth: trackData.width * 0.3
                Layout.rightMargin: elisaTheme.layoutHorizontalMargin
            }

            TextMetrics {
                id: textSize
                font: ratingText.font
                text: ratingText.text
            }

            RatingStar {
                id: ratingWidget
                starRating: rating
                readOnly: true

                starSize: elisaTheme.ratingStarSize

                Layout.fillWidth: true
            }

           ColorOverlay {
                source: ratingWidget

                z: 2
                anchors.fill: ratingWidget

                color: myPalette.text
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        Text {
            id: trackResource
            text: trackMetadata.resource
            color: myPalette.text
            font.italic: true

            elide: Text.ElideLeft

            Layout.minimumHeight:  textSize.height
            Layout.preferredWidth: elisaTheme.trackMetadataWidth
            Layout.fillWidth: true
        }
    }
}
