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

Kirigami.Page {
    id: topItem

    property int databaseId: 0
    property var trackType
    property string songTitle: ''
    property string albumName: ''
    property string artistName: ''
    property url albumArtUrl: ''
    property url fileUrl: ''
    property int albumId
    property string albumArtist: ''

    signal openArtist()
    signal openAlbum()

    readonly property bool nothingPlaying: albumName.length === 0
                                        && artistName.length === 0
                                        && albumArtUrl.toString.length === 0
                                        && songTitle.length === 0
                                        && fileUrl.toString.length === 0

    title: i18nc("Title of the context view related to the currently playing track", "Now Playing")
    padding: 0

    property bool isWidescreen: mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold

    TrackContextMetaDataModel {
        id: metaDataModel

        manager: ElisaApplication.musicManager
    }

    // Header with title
    // TODO: Once we depend on Frameworks 5.80, change this to
    // "Kirigami.ApplicationHeaderStyle.None" and remove the custom header
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None
    header: ToolBar {
        implicitHeight: Math.round(Kirigami.Units.gridUnit * 2.5)
        Layout.fillWidth: true

        // Override color to use standard window colors, not header colors
        // TODO: remove this if the HeaderBar component is ever removed or moved
        // to the bottom of the window such that this toolbar touches the window
        // titlebar
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        RowLayout {
            anchors.fill: parent
            spacing: 0

            FlatButtonWithToolTip {
                id: showSidebarButton
                objectName: 'showSidebarButton'
                visible: Kirigami.Settings.isMobile
                text: i18nc("open the sidebar", "Open sidebar")
                icon.name: "application-menu"
                onClicked: mainWindow.globalDrawer.open()
            }

            Image {
                id: mainIcon
                source: elisaTheme.nowPlayingIcon
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                sourceSize.height: Layout.preferredHeight
                sourceSize.width: Layout.preferredWidth

                fillMode: Image.PreserveAspectFit
                asynchronous: true
                visible: !Kirigami.Settings.isMobile
            }
            Kirigami.Heading {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                text: topItem.title
            }
            // Invisible; this exists purely to make the toolbar height match that
            // of the adjacent one
            ToolButton {
                icon.name: "edit-paste"
                opacity: 0
            }

            ButtonGroup {
                id: nowPlayingButtons
                onCheckedButtonChanged: {
                    persistentSettings.nowPlayingPreferLyric = nowPlayingButtons.checkedButton === showLyricButton
                }
            }
            FlatButtonWithToolTip {
                id: showMetaDataButton
                ButtonGroup.group: nowPlayingButtons

                readonly property alias item: allMetaDataScroll

                checkable: true
                checked: !persistentSettings.nowPlayingPreferLyric
                display: topItem.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
                icon.name: "documentinfo"
                text: i18nc("One of the 'now playing' views", "Metadata")
                visible: !contentLayout.wideMode
            }
            FlatButtonWithToolTip {
                id: showLyricButton
                ButtonGroup.group: nowPlayingButtons

                readonly property alias item: lyricScroll

                checkable: true
                checked: persistentSettings.nowPlayingPreferLyric
                display: topItem.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
                icon.name: "view-media-lyrics"
                text: i18nc("One of the 'now playing' views", "Lyrics")
                visible: !contentLayout.wideMode
            }

            FlatButtonWithToolTip {
                id: showPlaylistButton
                visible: Kirigami.Settings.isMobile
                text: i18nc("show the playlist", "Show Playlist")
                icon.name: "view-media-playlist"
                display: topItem.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
                onClicked: {
                    if (topItem.isWidescreen) {
                        contentView.showPlaylist = !contentView.showPlaylist;
                    } else {
                        playlistDrawer.open();
                    }
                }
            }
        }
    }

    Item {
        anchors.fill: parent

        // Blurred album art background
        Loader {
            active: ElisaApplication.showNowPlayingBackground && !topItem.nothingPlaying
            anchors.fill: parent

            sourceComponent: Image {
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
                    // anchors.fill: parent
                    radius: 40
                }
            }
        }

        RowLayout {
            id: contentLayout

            property bool wideMode: allMetaDataLoader.width  <= width * 0.5
                                 && allMetaDataLoader.height <= height

            anchors.fill: parent
            visible: !topItem.nothingPlaying

            spacing:  0

            // Metadata
            ScrollView {
                id: allMetaDataScroll

                implicitWidth: {
                    if (contentLayout.wideMode) {
                        return contentLayout.width * 0.5
                    } else {
                        return showMetaDataButton.checked? contentLayout.width : 0
                    }
                }

                implicitHeight: Math.min(allMetaDataLoader.height, parent.height)

                contentWidth: implicitWidth
                contentHeight: allMetaDataLoader.height
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                Loader {
                    id: allMetaDataLoader

                    sourceComponent: Kirigami.FormLayout {
                        id: allMetaData
                        property real margins: Kirigami.Units.largeSpacing + allMetaDataScroll.ScrollBar.vertical.width
                        width: (implicitWidth + margins <= contentLayout.width * 0.5 ? contentLayout.width * 0.5 : contentLayout.width) - margins
                        x: wideMode? (allMetaDataScroll.width - width) * 0.5 : Kirigami.Units.largeSpacing

                        Repeater {
                            id: trackData
                            model: metaDataModel

                            delegate: Item {
                                Kirigami.FormData.label: "<b>" + model.name + ":</b>"
                                implicitWidth: childrenRect.width
                                implicitHeight: childrenRect.height

                                MediaTrackMetadataDelegate {
                                    maximumWidth: contentLayout.width - allMetaData.margins
                                    index: model.index
                                    name: model.name
                                    display: model.display
                                    type: model.type
                                    readOnly: true
                                }
                            }
                        }
                    }

                    // We need unload Kirigami.FormLayout and recreate it
                    // to avoid lots of warnings in the terminal
                    Timer {
                        id: resetTimer
                        interval: 0
                        onTriggered: {
                            allMetaDataLoader.active = true
                        }
                    }
                    Connections {
                        target: metaDataModel
                        function onModelAboutToBeReset() {
                            allMetaDataLoader.active = false
                        }
                        function onModelReset() {
                            resetTimer.restart()
                        }
                    }
                }
            }

            // Lyric
            ScrollView {
                id: lyricScroll

                implicitWidth: {
                    if (contentLayout.wideMode) {
                        return contentLayout.width * 0.5
                    } else {
                        return showLyricButton.checked? contentLayout.width : 0
                    }
                }

                implicitHeight: Math.min(lyricItem.height, parent.height)

                contentWidth: implicitWidth
                contentHeight: lyricItem.height
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                // Lyrics
                Item {
                    id: lyricItem
                    property real margins: Kirigami.Units.largeSpacing + lyricScroll.ScrollBar.vertical.width
                    width: lyricScroll.width - margins
                    height: lyricLabel.visible? lyricLabel.height : lyricPlaceholder.height
                    x: Kirigami.Units.largeSpacing

                    Label {
                        id: lyricLabel
                        text: metaDataModel.lyrics
                        wrapMode: Text.WordWrap
                        horizontalAlignment: contentLayout.wideMode? Text.AlignLeft : Text.AlignHCenter
                        visible: text !== ""
                        width: parent.width
                    }

                    Kirigami.PlaceholderMessage {
                        id: lyricPlaceholder
                        visible: !lyricLabel.visible
                        text: i18n("No lyrics found")
                        icon.name: "view-media-lyrics"
                        width: parent.width
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
            icon.name: "view-media-track"
        }
    }

    // Footer with file path label
    footer: ToolBar {
        implicitHeight: Math.round(Kirigami.Units.gridUnit * 2)
        visible: !topItem.nothingPlaying
        RowLayout {
            anchors.fill: parent

            LabelWithToolTip {
                id: fileUrlLabel
                text: metaDataModel.fileUrl
                elide: Text.ElideLeft
                Layout.fillWidth: true
            }

            Kirigami.ActionToolBar {
                // because fillWidth is true by default
                Layout.fillWidth: false

                // when there is not enough space, show the button in the compact mode
                // then the file url will be elided if needed
                Layout.preferredWidth: parent.width > fileUrlLabel.implicitWidth + spacing + maximumContentWidth ? maximumContentWidth : Kirigami.Units.gridUnit * 2

                Layout.fillHeight: true

                actions: [
                    Kirigami.Action {
                        text: i18n("Show In Folder")
                        icon.name: 'document-open-folder'
                        visible: metaDataModel.fileUrl.toString() !== "" && !metaDataModel.fileUrl.toString().startsWith("http") && !metaDataModel.fileUrl.toString().startsWith("rtsp")
                        onTriggered: {
                            ElisaApplication.showInFolder(metaDataModel.fileUrl)
                        }
                    }
                ]
            }
        }
    }

    onDatabaseIdChanged: {
        if (ElisaApplication.musicManager && trackType !== undefined && databaseId !== 0) {
            metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
        }
    }

    onTrackTypeChanged: {
        if (ElisaApplication.musicManager && trackType !== undefined && databaseId !== 0) {
            metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
        }
    }

    Connections {
        target: ElisaApplication

        function onMusicManagerChanged() {
            if (ElisaApplication.musicManager && trackType !== undefined && databaseId !== 0) {
                metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
            }
        }
    }

    Component.onCompleted: {
        if (ElisaApplication.musicManager && trackType !== undefined && databaseId !== 0) {
            metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
        }
    }
}
