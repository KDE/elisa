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
import QtQuick.Window 2.2
import QtQml.Models 2.2
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

Window {
    id: trackMetadata

    property var trackDataHelper

    signal rejected()

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    title: i18nc("Window title for track metadata", "View Details")
    width: elisaTheme.trackMetadataWidth
    maximumWidth: width
    minimumWidth: width
    modality: Qt.NonModal
    flags: Qt.Dialog | Qt.WindowCloseButtonHint

    Component.onCompleted: {
        if (trackDataHelper.hasValidTitle())
            trackList.append({"name": i18nc("Track title for track metadata view", "Title:"), "content": trackDataHelper.title})
        if (trackDataHelper.hasValidArtist())
            trackList.append({"name": i18nc("Track artist for track metadata view", "Artist:"), "content": trackDataHelper.artist})
        if (trackDataHelper.hasValidAlbumName())
            trackList.append({"name": i18nc("Album name for track metadata view", "Album:"), "content": trackDataHelper.albumName})
        if (trackDataHelper.hasValidTrackNumber())
            trackList.append({"name": i18nc("Track number for track metadata view", "Track Number:"), "content": trackDataHelper.trackNumber})
        if (trackDataHelper.hasValidDiscNumber())
            trackList.append({"name": i18nc("Disc number for track metadata view", "Disc Number:"), "content": trackDataHelper.discNumber})
        if (trackDataHelper.hasValidAlbumArtist())
            trackList.append({"name": i18nc("Album artist for track metadata view", "Album Artist:"), "content": trackDataHelper.albumArtist})
        trackList.append({"name": i18nc("Duration label for track metadata view", "Duration:"), "content": trackDataHelper.duration})
        if (trackDataHelper.hasValidYear())
            trackList.append({"name": i18nc("Year label for track metadata view", "Year:"), "content": trackDataHelper.year})
        if (trackDataHelper.hasValidGenre())
            trackList.append({"name": i18nc("Genre label for track metadata view", "Genre:"), "content": trackDataHelper.genre})
        if (trackDataHelper.hasValidComposer())
            trackList.append({"name": i18nc("Composer name for track metadata view", "Composer:"), "content": trackDataHelper.composer})
        if (trackDataHelper.hasValidLyricist())
            trackList.append({"name": i18nc("Lyricist label for track metadata view", "Lyricist:"), "content": trackDataHelper.lyricist})
        if (trackDataHelper.hasValidBitRate())
            trackList.append({"name": i18nc("Bit rate label for track metadata view", "Bit Rate:"), "content": trackDataHelper.bitRate + " " + i18nc("Unit of the bit rate in thousand", "kbit/s")})
        if (trackDataHelper.hasValidSampleRate())
            trackList.append({"name": i18nc("Sample Rate label for track metadata view", "Sample Rate:"), "content": trackDataHelper.sampleRate + " " + i18nc("Unit of the sample rate", "Hz")})
        if (trackDataHelper.hasValidChannels())
            trackList.append({"name": i18nc("Channels label for track metadata view", "Channels:"), "content": trackDataHelper.channels})
        if (trackDataHelper.hasValidComment())
            trackList.append({"name": i18nc("Comment label for track metadata view", "Comment:"), "content": trackDataHelper.comment})
        trackData.Layout.preferredHeight = textSize.height * trackData.count
        trackMetadata.height = textSize.height * (trackData.count + 1 + ( rating > -1 ? 1 : 0 )) + buttons.height + elisaTheme.layoutHorizontalMargin
        trackMetadata.maximumHeight = trackMetadata.height
        trackMetadata.minimumHeight = trackMetadata.height
    }

    Rectangle {
        anchors.fill: parent

        color: myPalette.window

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
                        Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
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

            Text {
                id: trackResource
                text: trackMetadata.resource
                color: myPalette.text
                font.italic: true

                elide: Text.ElideLeft

                Layout.minimumHeight:  textSize.height
                Layout.preferredWidth: elisaTheme.trackMetadataWidth
                Layout.fillWidth: true
                Layout.topMargin: elisaTheme.layoutHorizontalMargin
            }

            DialogButtonBox {
                id: buttons

                standardButtons: DialogButtonBox.Close
                alignment: Qt.AlignRight
                onRejected: trackMetadata.rejected()

                Layout.fillWidth: true
            }
        }
    }
}
