/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects as FX
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.elisa

Kirigami.Page {
    id: topItem

    property int databaseId: 0
    property var trackType
    property string songTitle: ""
    property string albumName: ""
    property string artistName: ""
    property url albumArtUrl: ""
    property url fileUrl: ""
    property int albumId
    property string albumArtist: ""

    signal openArtist()
    signal openAlbum()

    readonly property bool nothingPlaying: albumName.length === 0
                                           && artistName.length === 0
                                           && albumArtUrl.toString().length === 0
                                           && songTitle.length === 0
                                           && fileUrl.toString().length === 0

    title: KI18n.i18nc("@title:window Title of the context view related to the currently playing track", "Now Playing")
    padding: 0

    property bool isWidescreen: mainWindow.width >= Theme.viewSelectorSmallSizeThreshold

    onAlbumArtUrlChanged: {
        background.loadImage();
    }

    TrackContextMetaDataModel {
        id: metaDataModel
        onLyricsChanged: lyricsView.model.setLyric(lyrics)
        manager: ElisaApplication.musicManager
    }

    TrackMetadataProxyModel {
        id: metadataProxyModel
        sourceModel: metaDataModel
    }

    // Header with title and actions
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None
    header: ToolBar {
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
                text: KI18n.i18nc("@action:button", "Open sidebar")
                icon.name: "open-menu-symbolic"
                onClicked: mainWindow.globalDrawer.open()
            }

            Kirigami.Heading {
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.alignment: Qt.AlignVCenter
                elide: Text.ElideRight

                text: topItem.title
            }

            // Invisible; this exists purely to make the toolbar height match that
            // of the adjacent one
            Item {
                implicitHeight: Theme.toolBarHeaderMinimumHeight
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
                text: KI18n.i18nc("@option:radio One of the 'now playing' views", "Metadata")
                visible: !contentLayout.wideMode
            }
            FlatButtonWithToolTip {
                id: showLyricButton
                ButtonGroup.group: nowPlayingButtons

                checkable: true
                checked: persistentSettings.nowPlayingPreferLyric
                display: topItem.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
                icon.name: "view-media-lyrics"
                text: KI18n.i18nc("@option:radio One of the 'now playing' views", "Lyrics")
                visible: !contentLayout.wideMode
            }

            FlatButtonWithToolTip {
                id: showPlaylistButton
                visible: Kirigami.Settings.isMobile
                text: KI18n.i18nc("@action:button", "Show Playlist")
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
        StackView {
            id: background
            anchors.fill: parent

            readonly property bool active: ElisaApplication.showNowPlayingBackground && !topItem.nothingPlaying
            property Item pendingImage
            property bool doesSkipAnimation: true

            layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
            opacity: GraphicsInfo.api === GraphicsInfo.Software ? 0.1 : 0.2
            layer.effect: FX.MultiEffect {
                autoPaddingEnabled: false
                blurEnabled: true
                blurMax: 40
                blur: 1
            }

            replaceEnter: Transition {
                OpacityAnimator {
                    id: replaceEnterOpacityAnimator
                    from: 0
                    to: 1
                    // 1 is HACK for https://bugreports.qt.io/browse/QTBUG-106797 to avoid flickering
                    duration: background.doesSkipAnimation ? 1 : Kirigami.Units.longDuration
                }
            }
            // Keep the old image around till the new one is fully faded in
            // If we fade both at the same time you can see the background behind glimpse through
            replaceExit: Transition {
                PauseAnimation {
                    duration: replaceEnterOpacityAnimator.duration
                }
            }

            onActiveChanged: loadImage()

            function loadImage() {
                if (pendingImage) {
                    pendingImage.statusChanged.disconnect(replaceWhenLoaded);
                    pendingImage.destroy();
                    pendingImage = null;
                }

                if (!active) {
                    clear();
                    return;
                }

                doesSkipAnimation = currentItem == undefined;
                pendingImage = backgroundComponent.createObject(background, {
                    "source": topItem.albumArtUrl.toString() === "" ? Qt.resolvedUrl(Theme.defaultAlbumImage) : topItem.albumArtUrl,
                    "opacity": 0,
                });

                if (pendingImage.status === Image.Loading) {
                    pendingImage.statusChanged.connect(background.replaceWhenLoaded);
                } else {
                    background.replaceWhenLoaded();
                }
            }

            function replaceWhenLoaded() {
                pendingImage.statusChanged.disconnect(replaceWhenLoaded);
                replace(pendingImage, {}, StackView.Transition);
                pendingImage = null;
            }

            Component.onCompleted: {
                loadImage();
            }
        }

        Component {
            id: backgroundComponent

            Image {
                asynchronous: true
                fillMode: Image.PreserveAspectCrop

                // HACK: set sourceSize to a fixed value to prevent background flickering (BUG431607)
                onStatusChanged: {
                    if (status === Image.Ready && (sourceSize.width > Kirigami.Units.gridUnit * 50 || sourceSize.height > Kirigami.Units.gridUnit * 50)) {
                        sourceSize = Qt.size(Kirigami.Units.gridUnit * 50, Kirigami.Units.gridUnit * 50);
                    }
                }

                StackView.onRemoved: {
                    destroy();
                }
            }
        }

        RowLayout {
            id: contentLayout

            property bool wideMode: allMetaDataLoader.width <= width * 0.5
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
                        return showMetaDataButton.checked ? contentLayout.width : 0
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
                            model: metadataProxyModel

                            delegate: Item {
                                id: metadataDelegate

                                required property int index
                                required property var display
                                required property string name
                                required property int type
                                required property bool hasData

                                Kirigami.FormData.label: "<b>" + name + ":</b>"
                                implicitWidth: childrenRect.width
                                implicitHeight: childrenRect.height
                                visible: hasData

                                MediaTrackMetadataDelegate {
                                    maximumWidth: contentLayout.width - allMetaData.margins
                                    index: metadataDelegate.index
                                    name: metadataDelegate.name
                                    display: metadataDelegate.display
                                    type: metadataDelegate.type
                                    readOnly: true
                                    url: topItem.fileUrl
                                    hasData: metadataDelegate.hasData
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

            //Lyrics
            LyricsView {
                id: lyricsView
                visible: contentLayout.wideMode || showLyricButton.checked
                alignLeft: contentLayout.wideMode

                Layout.fillHeight: true

                implicitWidth: {
                    if (contentLayout.wideMode) {
                        return contentLayout.width * 0.5;
                    } else {
                        return showLyricButton.checked ? contentLayout.width : 0;
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
                text: KI18n.i18nc("@info:placeholder", "Nothing playing")
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
                        text: KI18n.i18nc("@action:button", "Show In Folder")
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
            if (ElisaApplication.musicManager && topItem.trackType !== undefined && topItem.databaseId !== 0) {
                metaDataModel.initializeByIdAndUrl(topItem.trackType, topItem.databaseId, topItem.fileUrl)
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
