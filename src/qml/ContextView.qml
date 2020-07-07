/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.10
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQml.Models 2.2
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

FocusScope {
    id: topItem

    property int databaseId: 0
    property var trackType
    property alias title: titleLabel.text
    property string albumName: ''
    property string artistName: ''
    property url albumArtUrl: ''
    property url fileUrl: ''

    TrackContextMetaDataModel {
        id: metaDataModel

        manager: ElisaApplication.musicManager
    }

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

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
                    width: Kirigami.Units.largeSpacing
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
                id: scrollView
                anchors.fill: parent
                clip: true

                contentWidth: content.width
                contentHeight: content.height

                // Title + metadata + lyrics
                ColumnLayout {
                    id: content

                    width: scrollView.width - scrollView.ScrollBar.vertical.width

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
                    Kirigami.Separator {
                        Layout.fillWidth: true
                        Layout.leftMargin: Kirigami.Units.largeSpacing* 5
                        Layout.rightMargin: Kirigami.Units.largeSpacing * 5
                        Layout.topMargin: Kirigami.Units.largeSpacing
                        Layout.bottomMargin: Kirigami.Units.largeSpacing
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
                    Kirigami.Separator {
                        Layout.fillWidth: true
                        Layout.leftMargin: Kirigami.Units.largeSpacing * 5
                        Layout.rightMargin: Kirigami.Units.largeSpacing * 5
                        Layout.topMargin: Kirigami.Units.largeSpacing
                        Layout.bottomMargin: Kirigami.Units.largeSpacing

                        visible: metaDataModel.lyrics !== ""
                    }

                    // Lyrics
                    Label {
                        text: metaDataModel.lyrics

                        wrapMode: Text.WordWrap

                        horizontalAlignment: Label.AlignHCenter

                        Layout.fillWidth: true
                        Layout.bottomMargin: Kirigami.Units.smallSpacing
                        visible: metaDataModel.lyrics !== ""

                    }
                }
            }
        }

        // Footer with file path label
        HeaderFooterToolbar {
            type: "footer"
            contentLayoutSpacing: Kirigami.Units.largeSpacing
            contentItems: [
                Image {
                    Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
                    Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
                    sourceSize.width: Kirigami.Units.iconSizes.smallMedium
                    sourceSize.height: Kirigami.Units.iconSizes.smallMedium
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
        target: ElisaApplication

        onMusicManagerChanged: {
            metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
        }
    }

    Component.onCompleted: {
        if (ElisaApplication.musicManager) {
            metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
        }
    }
}
