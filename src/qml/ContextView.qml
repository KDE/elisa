/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
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
                                           && albumArtUrl.toString().length === 0
                                           && songTitle.length === 0
                                           && fileUrl.toString().length === 0

    title: i18nc("Title of the context view related to the currently playing track", "Now Playing")
    padding: 0

    property bool isWidescreen: mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold

    TrackContextMetaDataModel {
        id: metaDataModel
        onLyricsChanged: lyricsModel.setLyric(lyrics)
        manager: ElisaApplication.musicManager
    }

    // Header with title and actions
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None
    header: ToolBar {
        implicitHeight: Math.round(Kirigami.Units.gridUnit * 2.5)

        // Override color to use standard window colors, not header colors
        // TODO: remove this if the HeaderBar component is ever removed or moved
        // to the bottom of the window such that this toolbar touches the window
        // titlebar
        Kirigami.Theme.colorSet: Kirigami.Theme.Window

        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.smallSpacing

            FlatButtonWithToolTip {
                id: showSidebarButton
                objectName: 'showSidebarButton'
                visible: Kirigami.Settings.isMobile
                text: i18nc("open the sidebar", "Open sidebar")
                icon.name: "open-menu-symbolic"
                onClicked: mainWindow.globalDrawer.open()
            }

            Kirigami.Heading {
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
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

                asynchronous: true

                fillMode: Image.PreserveAspectCrop

                layer.enabled: true
                opacity: 0.2
                layer.effect: FastBlur {
                    source: albumArtBackground
                    // anchors.fill: parent
                    radius: 40
                }

                // HACK: set sourceSize to a fixed value to prevent background flickering (BUG431607)
                onStatusChanged: if (status === Image.Ready && (sourceSize.width > Kirigami.Units.gridUnit * 50 || sourceSize.height > Kirigami.Units.gridUnit * 50)) sourceSize = Qt.size(Kirigami.Units.gridUnit * 50, Kirigami.Units.gridUnit * 50)
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

                contentWidth: availableWidth
                contentHeight: allMetaDataLoader.height

                // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
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
                                    url: topItem.fileUrl
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

            // Lyrics
            ScrollView {
                id: lyricScroll
                implicitWidth: {
                    if (contentLayout.wideMode) {
                        return contentLayout.width * 0.5
                    } else {
                        return showLyricButton.checked ? contentLayout.width : 0
                    }
                }
                implicitHeight: Math.min(lyricItem.height, parent.height)

                contentWidth: availableWidth
                contentHeight: lyricItem.height

                // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                PropertyAnimation {
                    id: lyricScrollAnimation

                    // the target is a flickable
                    target: lyricScroll.contentItem
                    property: "contentY"
                    onToChanged: restart()
                }

                Item {
                    id: lyricItem
                    property real margins: Kirigami.Units.largeSpacing + lyricScroll.ScrollBar.vertical.width
                    width: lyricScroll.width - margins
                    height: lyricsView.count == 0 ? lyricPlaceholder.height : lyricsView.height
                    x: Kirigami.Units.largeSpacing

                    ListView {
                        id: lyricsView
                        height: contentHeight
                        width: parent.width
                        model: lyricsModel
                        delegate: Label {
                            text: lyric
                            width: lyricItem.width
                            wrapMode: Text.WordWrap
                            font.bold: ListView.isCurrentItem
                            horizontalAlignment: contentLayout.wideMode? Text.AlignLeft : Text.AlignHCenter
                            MouseArea {
                                height: parent.height
                                width: Math.min(parent.width, parent.contentWidth)
                                x: contentLayout.wideMode? 0 : (parent.width - width) / 2
                                enabled: lyricsModel.isLRC
                                cursorShape: enabled ? Qt.PointingHandCursor : undefined
                                onClicked: {
                                    ElisaApplication.audioPlayer.position = timestamp;
                                }
                            }
                        }
                        currentIndex: lyricsModel.highlightedIndex
                        onCurrentIndexChanged: {
                            if (currentIndex === -1)
                                return

                            // center aligned
                            var toPos = Math.round(currentItem.y + currentItem.height * 0.5 - lyricScroll.height * 0.5)
                            // make sure the first and the last lines are always
                            // positioned at the beginning and the end of the view

                            toPos = Math.max(toPos, 0)
                            toPos = Math.min(toPos, contentHeight - lyricScroll.height)
                            lyricScrollAnimation.to = toPos

                        }
                    }

                    LyricsModel {
                        id: lyricsModel
                    }
                    Connections {
                        target: ElisaApplication.audioPlayer
                        function onPositionChanged(position) {
                            lyricsModel.setPosition(position)
                        }
                    }

                    Loader {
                        id: lyricPlaceholder
                        anchors.centerIn: parent
                        width: parent.width

                        active: lyricsView.count === 0
                        visible: active && status === Loader.Ready

                        sourceComponent: Kirigami.PlaceholderMessage {
                            text: i18n("No lyrics found")
                            icon.name: "view-media-lyrics"
                        }
                    }
                }
            }
        }

        // "Nothing Playing" message
        Loader {
            anchors.centerIn: parent
            width: parent.width - (Kirigami.Units.largeSpacing * 4)

            active: topItem.nothingPlaying
            visible: active && status === Loader.Ready

            sourceComponent: Kirigami.PlaceholderMessage {
                anchors.centerIn: parent
                text: i18n("Nothing playing")
                icon.name: "view-media-track"
            }
        }
    }

    // Footer with file path label
    footer: ToolBar {
        implicitHeight: Math.round(Kirigami.Units.gridUnit * 2)
        visible: !topItem.nothingPlaying

        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.smallSpacing

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

    onFileUrlChanged: {
        if (ElisaApplication.musicManager && trackType !== undefined && fileUrl.toString().length !== 0) {
            if (databaseId !== 0) {
                metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
            } else {
                metaDataModel.initializeByUrl(trackType, fileUrl)
            }
        }
    }

    onTrackTypeChanged: {
        if (ElisaApplication.musicManager && trackType !== undefined && fileUrl.toString().length !== 0) {
            if (databaseId !== 0) {
                metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
            } else {
                metaDataModel.initializeByUrl(trackType, fileUrl)
            }
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
        if (ElisaApplication.musicManager && trackType !== undefined) {
            if (databaseId !== 0) {
                metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl)
            } else {
                metaDataModel.initializeByUrl(trackType, fileUrl)
            }
        }
    }
}
