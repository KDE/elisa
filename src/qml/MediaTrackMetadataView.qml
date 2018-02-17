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
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

Dialog {
    id: trackMetadata

    property var trackDataHelper

    modal: false
    closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
    padding: elisaTheme.layoutVerticalMargin
    standardButtons: Dialog.Close

    parent: ApplicationWindow.overlay
    width: elisaTheme.trackMetadataWidth

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

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
        trackMetadata.height = textSize.height * (trackData.count + 1 + (rating > -1 ? 1 : 0)) + 3 * elisaTheme.layoutVerticalMargin + footer.height
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        // This is needed since Dialog doesn't inherit from Item
        LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
        LayoutMirroring.childrenInherit: true

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 0

            Image {
                source: trackDataHelper.albumCover
                visible: trackDataHelper.hasValidAlbumCover()
                width: elisaTheme.coverImageSize
                height: elisaTheme.coverImageSize
                fillMode: Image.PreserveAspectFit
                Layout.preferredHeight: height
                Layout.preferredWidth: width
                Layout.minimumHeight: height
                Layout.minimumWidth: width
                Layout.maximumHeight: height
                Layout.maximumWidth: width
            }

            ColumnLayout {
                Layout.leftMargin: elisaTheme.layoutHorizontalMargin
                Layout.fillHeight: true
                Layout.preferredWidth: elisaTheme.trackMetadataWidth
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

                        LabelWithToolTip {
                            text: name
                            color: myPalette.text
                            horizontalAlignment: Text.AlignRight

                            Layout.preferredWidth: trackData.width * 0.3
                            Layout.minimumHeight: textSize.height
                            Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                            Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        }

                        LabelWithToolTip {
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

                    visible: trackDataHelper.hasValidRating()

                    Layout.minimumHeight: textSize.height

                    LabelWithToolTip {
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
                        starRating: trackDataHelper.rating
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
            }
        }

        LabelWithToolTip {
            id: trackResource
            text: trackDataHelper.resourceURI
            color: myPalette.text
            font.italic: true

            elide: Text.ElideLeft
            horizontalAlignment: Text.AlignRight

            Layout.minimumHeight: textSize.height
            Layout.preferredWidth: trackDataHelper.hasValidAlbumCover() ? elisaTheme.coverImageSize + elisaTheme.trackMetadataWidth : elisaTheme.trackMetadataWidth + 2 * elisaTheme.layoutHorizontalMargin
            Layout.fillWidth: true
            Layout.topMargin: elisaTheme.layoutVerticalMargin
        }

    }
}
