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
import org.kde.kirigami 2.12 as Kirigami
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

    readonly property bool nothingPlaying: albumName.length === 0
                                        && artistName.length === 0
                                        && albumArtUrl.toString.length === 0
                                        && fileUrl.toString.length === 0

    TrackContextMetaDataModel {
        id: metaDataModel

        manager: ElisaApplication.musicManager
    }

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        // Header with title
        HeaderFooterToolbar {
            Layout.fillWidth: true
            toolbarType: HeaderFooterToolbar.ToolbarType.Header
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

                visible: !topItem.nothingPlaying

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

                visible: !topItem.nothingPlaying

                contentWidth: content.width
                contentHeight: content.height

                // This Item holds only the content layout so we can center the
                // ColumnLayout within it to provide appropriate margins. We
                // can't do this if the ColumnLayout is directly inside the
                // ScrollView
                Item {
                    width: scrollView.width - scrollView.ScrollBar.vertical.width

                    // Layout holding the Title + metadata + lyrics labels
                    ColumnLayout {
                        id: content

                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.leftMargin: Kirigami.Units.largeSpacing * 2
                        anchors.right: parent.right
                        anchors.rightMargin: Kirigami.Units.largeSpacing * 2
                        // No bottom anchors so it can grow

                        // Song title
                        LabelWithToolTip {
                            id: titleLabel

                            level: 1

                            horizontalAlignment: Label.AlignHCenter

                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignTop

                            Layout.topMargin: elisaTheme.layoutVerticalMargin

                            wrapMode: Text.Wrap
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

                            wrapMode: Text.Wrap
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
                            //Layout.leftMargin:Kirigami.Units.largeSpacing
                            //Layout.rightMargin:Kirigami.Units.largeSpacing

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

            // "Nothing Playing" message
            Kirigami.PlaceholderMessage {
                anchors.centerIn: parent
                width: parent.width - (Kirigami.Units.largeSpacing * 4)
                visible: topItem.nothingPlaying
                text: i18n("Nothing playing")
            }
        }

        // Footer with file path label
        HeaderFooterToolbar {
            visible: !topItem.nothingPlaying
            toolbarType: HeaderFooterToolbar.ToolbarType.Footer
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 2
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

        function onMusicManagerChanged() {
            metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
        }
    }

    Component.onCompleted: {
        if (ElisaApplication.musicManager) {
            metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
        }
    }
}
