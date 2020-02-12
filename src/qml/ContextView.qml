/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2019 Nate Graham <nate@kde.org>
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

import QtQuick 2.10
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQml.Models 2.2
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import org.kde.elisa 1.0

FocusScope {
    id: topItem

    property var viewType
    property int databaseId: 0
    property var trackType
    property alias title: titleLabel.text
    property string albumName: ''
    property string artistName: ''
    property url albumArtUrl: ''
    property url fileUrl: ''

    TrackContextMetaDataModel {
        id: metaDataModel

        manager: elisa.musicManager
    }

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        TextMetrics {
            id: titleHeight
            text: viewTitle.text
            font: viewTitle.font
        }

        // Header with title
        HeaderFooterToolbar {
            type: "header"
            contentItems: [
                Image {
                    id: mainIcon
                    source: elisaTheme.nowPlayingIcon

                    height: viewTitle.height
                    width: height
                    sourceSize.height: height
                    sourceSize.width: width

                    fillMode: Image.PreserveAspectFit
                    asynchronous: true

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                },
                Item {
                    id: spacer
                    width: elisaTheme.layoutHorizontalMargin
                },
                LabelWithToolTip {
                    id: viewTitle
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                    text: i18nc("Title of the context view related to the currently playing track", "Now Playing")

                    level: 1
                }
            ]
        }

        // Container to hold both the blurred background and the scrollview
        // We can't make the scrollview a child of the background item since then
        // everything in the scrollview will be blurred and transparent too!
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Blurred album art background
            Image {
                id: albumArtBackground
                anchors.fill: parent

                source: albumArtUrl.toString() === '' ? Qt.resolvedUrl(elisaTheme.defaultAlbumImage) : albumArtUrl

                sourceSize.width: topItem.width
                sourceSize.height: topItem.height

                asynchronous: true

                fillMode: Image.PreserveAspectCrop

                layer.enabled: true
                opacity: 0.2
                layer.effect: FastBlur {
                    source: albumArtBackground
//                     anchors.fill: parent
                    radius: 40
                }
            }

            // Scrollview to hold all the content
            ScrollView {
                anchors.fill: parent
                clip: true

                contentWidth: content.width
                contentHeight: content.height

                // Title + metadata + lyrics
                ColumnLayout {
                    id: content

                    width: topItem.width
                    anchors.leftMargin: elisaTheme.layoutHorizontalMargin
                    anchors.rightMargin: elisaTheme.layoutHorizontalMargin

                    // Song title
                    LabelWithToolTip {
                        id: titleLabel

                        level: 1

                        horizontalAlignment: Label.AlignHCenter

                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop

                        Layout.topMargin: elisaTheme.layoutVerticalMargin
                    }

                    LabelWithToolTip {
                        id: subtitleLabel

                        text: {
                            if (artistName !== '' && albumName !== '') {
                                return i18nc("display of artist and album in context view", "<i>by</i> <b>%1</b> <i>from</i> <b>%2</b>", artistName, albumName)
                            } else if (artistName === '' && albumName !== '') {
                                return i18nc("display of album in context view", "<i>from</i> <b>%1</b>", albumName)
                            } else if (artistName !== '' && albumName === '') {
                                i18nc("display of artist in context view", "<i>by</i> <b>%1</b>", artistName)
                            }
                        }

                        level: 3
                        opacity: 0.6

                        horizontalAlignment: Label.AlignHCenter

                        visible: artistName !== '' && albumName !== ''

                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop
                    }

                    // Horizontal line separating title and subtitle from metadata
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.leftMargin: elisaTheme.layoutHorizontalMargin * 5
                        Layout.rightMargin: elisaTheme.layoutHorizontalMargin * 5
                        Layout.topMargin: elisaTheme.layoutVerticalMargin * 2
                        Layout.bottomMargin: elisaTheme.layoutVerticalMargin * 2

                        height: 1

                        color: myPalette.mid
                    }

                    // Metadata
                    ColumnLayout {
                        id: allMetaData

                        spacing: 0
                        Layout.fillWidth: true

                        Repeater {
                            id: trackData

                            model: metaDataModel

                            delegate: MetaDataDelegate {
                                Layout.fillWidth: true
                            }
                        }
                    }

                    // Horizontal line separating metadata from lyrics
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.leftMargin: elisaTheme.layoutHorizontalMargin * 5
                        Layout.rightMargin: elisaTheme.layoutHorizontalMargin * 5
                        Layout.topMargin: elisaTheme.layoutVerticalMargin * 2
                        Layout.bottomMargin: elisaTheme.layoutVerticalMargin * 2

                        height: 1

                        color: myPalette.mid

                        visible: metaDataModel.lyrics !== ""
                    }

                    // Lyrics
                    Label {
                        text: metaDataModel.lyrics

                        wrapMode: Text.WordWrap

                        horizontalAlignment: Label.AlignHCenter

                        Layout.fillWidth: true
                        Layout.bottomMargin: elisaTheme.layoutVerticalMargin

                        visible: metaDataModel.lyrics !== ""

                    }
                }
            }
        }

        // Footer with file path label
        HeaderFooterToolbar {
            type: "footer"
            contentLayoutSpacing: elisaTheme.layoutHorizontalMargin
            contentItems: [
                Image {
                    sourceSize.width: fileNameLabel.height
                    sourceSize.height: fileNameLabel.height

                    source: elisaTheme.folderIcon
                },
                LabelWithToolTip {
                    id: fileNameLabel

                    Layout.fillWidth: true

                    text: metaDataModel.fileUrl
                    elide: Text.ElideLeft
                }
            ]
        }
    }

    onDatabaseIdChanged: {
        metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
    }

    Connections {
        target: elisa

        onMusicManagerChanged: {
            metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
        }
    }

    Component.onCompleted: {
        if (elisa.musicManager) {
            metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
        }
    }
}
