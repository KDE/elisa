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
    property real labelWidth: trackData.width * 0.33
    property real metadataWidth: trackData.width * 0.66

    signal rejected()

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    title: i18nc("Window title for track metadata", "View Details")

    function updateData()
    {
        trackList.clear()
        if (trackDataHelper.hasValidTitle)
            trackList.append({"name": i18nc("Track title for track metadata view", "Title:"), "content": trackDataHelper.title})
        if (trackDataHelper.hasValidArtist)
            trackList.append({"name": i18nc("Track artist for track metadata view", "Artist:"), "content": trackDataHelper.artist})
        if (trackDataHelper.hasValidAlbumName)
            trackList.append({"name": i18nc("Album name for track metadata view", "Album:"), "content": trackDataHelper.albumName})
        if (trackDataHelper.hasValidTrackNumber)
            trackList.append({"name": i18nc("Track number for track metadata view", "Track Number:"), "content": trackDataHelper.trackNumber.toString()})
        if (trackDataHelper.hasValidDiscNumber)
            trackList.append({"name": i18nc("Disc number for track metadata view", "Disc Number:"), "content": trackDataHelper.discNumber.toString()})
        if (trackDataHelper.hasValidAlbumArtist)
            trackList.append({"name": i18nc("Album artist for track metadata view", "Album Artist:"), "content": trackDataHelper.albumArtist})
        trackList.append({"name": i18nc("Duration label for track metadata view", "Duration:"), "content": trackDataHelper.duration})
        if (trackDataHelper.hasValidYear)
            trackList.append({"name": i18nc("Year label for track metadata view", "Year:"), "content": trackDataHelper.year.toString()})
        if (trackDataHelper.hasValidGenre)
            trackList.append({"name": i18nc("Genre label for track metadata view", "Genre:"), "content": trackDataHelper.genre})
        if (trackDataHelper.hasValidComposer)
            trackList.append({"name": i18nc("Composer name for track metadata view", "Composer:"), "content": trackDataHelper.composer})
        if (trackDataHelper.hasValidLyricist)
            trackList.append({"name": i18nc("Lyricist label for track metadata view", "Lyricist:"), "content": trackDataHelper.lyricist})
        if (trackDataHelper.hasValidBitRate)
            trackList.append({"name": i18nc("Bit rate label for track metadata view", "Bit Rate:"), "content": (trackDataHelper.bitRate / 1000) + " " + i18nc("Unit of the bit rate in thousand", "kbit/s")})
        if (trackDataHelper.hasValidSampleRate)
            trackList.append({"name": i18nc("Sample Rate label for track metadata view", "Sample Rate:"), "content": trackDataHelper.sampleRate + " " + i18nc("Unit of the sample rate", "Hz")})
        if (trackDataHelper.hasValidChannels)
            trackList.append({"name": i18nc("Channels label for track metadata view", "Channels:"), "content": trackDataHelper.channels.toString()})
        if (trackDataHelper.hasValidComment)
            trackList.append({"name": i18nc("Comment label for track metadata view", "Comment:"), "content": trackDataHelper.comment})
    }

    function resize() {
        trackData.Layout.preferredHeight = textSize.height * trackData.count
        trackMetadata.height = textSize.height * (trackData.count + 1 + ( trackDataHelper.hasValidRating ? 1 : 0 )) + buttons.height + elisaTheme.layoutHorizontalMargin * 3
        trackMetadata.minimumHeight = trackMetadata.height
        trackMetadata.maximumHeight = trackMetadata.height
        trackMetadata.width = (trackDataHelper.hasValidAlbumCover ? elisaTheme.coverImageSize : 0) + labelWidth + metadataWidth + 2 * elisaTheme.layoutHorizontalMargin
        trackMetadata.minimumWidth = trackMetadata.width
    }

    Connections {
        target: trackDataHelper

        onTrackDataChanged: {
            updateData()
            resize()
        }
    }

    modality: Qt.NonModal
    flags: Qt.Dialog | Qt.WindowCloseButtonHint

    Component.onCompleted: {
        updateData()
        resize()
    }

    Rectangle {
        anchors.fill: parent

        color: myPalette.window

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: elisaTheme.layoutHorizontalMargin
            spacing: 0

            RowLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 0

                Image {
                    source: trackDataHelper.albumCover
                    visible: trackDataHelper.hasValidAlbumCover
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
                    Layout.fillWidth: true
                    spacing: 0

                    ListView {
                        id: trackData
                        interactive: false
                        Layout.fillWidth: true

                        model: ListModel {
                            id: trackList
                        }

                        onCountChanged: resize()

                        delegate: RowLayout {
                            id: delegateRow
                            spacing: 0

                            Label {
                                id: metaDataLabels
                                text: name
                                color: myPalette.text
                                horizontalAlignment: Text.AlignRight

                                Layout.preferredWidth: labelWidth
                                Layout.minimumHeight: textSize.height
                                Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                                Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                                Component.onCompleted: {
                                    if(labelWidth < metaDataLabels.implicitWidth) {
                                          labelWidth = metaDataLabels.implicitWidth
                                    }
                                }
                            }

                            Label {
                                id: metaData
                                text: content
                                color: myPalette.text

                                horizontalAlignment: Text.AlignLeft
                                elide: Text.ElideRight

                                Layout.preferredWidth: trackMetadata.width - labelWidth - 2 * elisaTheme.layoutHorizontalMargin - (trackDataHelper.hasValidAlbumCover ? elisaTheme.coverImageSize : 0)
                                Layout.fillWidth: true
                                Layout.minimumHeight: textSize.height
                                Component.onCompleted: {
                                    if(metadataWidth < metaData.implicitWidth) {
                                        if (metaData.implicitWidth > elisaTheme.maximumMetadataWidth) {
                                            metadataWidth = elisaTheme.maximumMetadataWidth
                                        } else {
                                            metadataWidth = metaData.implicitWidth + 1
                                        }
                                    }
                                }
                            }
                        }
                    }

                    RowLayout {
                        anchors.margins: 0
                        spacing: 0

                        visible: trackDataHelper.hasValidRating

                        Layout.minimumHeight: textSize.height

                        Label {
                            id: ratingText
                            text: i18nc("Rating label for information panel", "Rating:")
                            color: myPalette.text

                            horizontalAlignment: Text.AlignRight
                            Layout.preferredWidth: labelWidth
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
                            layer.effect: ColorOverlay {
                                source: ratingWidget
                                color: myPalette.text
                            }
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
                horizontalAlignment: implicitWidth > trackMetadata.width ? Text.AlignRight : Text.AlignLeft

                Layout.minimumHeight: textSize.height
                Layout.preferredWidth: trackMetadata.width * 0.98 - 2 * elisaTheme.layoutHorizontalMargin
                Layout.topMargin: elisaTheme.layoutVerticalMargin
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
