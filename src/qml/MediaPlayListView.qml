/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import org.kde.kirigami as Kirigami
import org.kde.elisa

import "mobile"

// Not using ScrollablePage because we don't need any of the refresh features
// that it provides
Kirigami.Page {
    id: topItem

    // set by the respective mobile/desktop view
    property var playListNotification
    property var playListView

    title: i18nc("@title:window Title of the view of the playlist; keep this string as short as possible because horizontal space is quite scarce", "Playlist")
    padding: 0

    // Use view colors so the background is white
    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    Accessible.role: Accessible.Pane
    Accessible.name: topItem.title

    // Header with title and actions
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None
    header: ToolBar {
        leftPadding: mirrored ? undefined : Kirigami.Units.largeSpacing
        rightPadding: mirrored ? Kirigami.Units.largeSpacing : undefined

        // Override color to use standard window colors, not header colors
        // TODO: remove this if the HeaderBar component is ever removed or moved
        // to the bottom of the window such that this toolbar touches the window
        // titlebar
        Kirigami.Theme.colorSet: Kirigami.Theme.Window

        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.smallSpacing

            Kirigami.Heading {
                text: topItem.title
            }

            // Invisible; this exists purely to make the toolbar height match that
            // of the adjacent one
            Item {
                implicitHeight: Theme.toolBarHeaderMinimumHeight
            }

            Kirigami.ActionToolBar {
                Layout.fillWidth: true
                alignment: Qt.AlignRight

                actions: [
                    Kirigami.Action {
                        text: i18nc("@action:button Show current track in playlist", "Show Current")
                        icon.name: 'media-track-show-active'
                        displayHint: Kirigami.DisplayHint.KeepVisible
                        enabled: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount > 0 : false
                        onTriggered: {
                            playListView.positionViewAtIndex(ElisaApplication.mediaPlayListProxyModel.currentTrackRow, ListView.Contain)
                            playListView.currentIndex = ElisaApplication.mediaPlayListProxyModel.currentTrackRow
                            playListView.currentItem.forceActiveFocus()
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action:button Remove all tracks from play list", "Clear All")
                        // TODO uncomment once we can depend on Qt 6.8
                        // Accessible.name: i18nc("@action:button Remove all tracks from playlist", "Clear playlist")
                        icon.name: 'edit-clear-all'
                        displayHint: Kirigami.DisplayHint.KeepVisible
                        enabled: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount > 0 : false
                        onTriggered: ElisaApplication.mediaPlayListProxyModel.clearPlayList()
                    },
                    Kirigami.Action {
                        id: savePlaylistButton
                        text: i18nc("@action:button Save a playlist file", "Save…")
                        icon.name: 'document-save'
                        displayHint: Kirigami.DisplayHint.KeepVisible
                        enabled: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount > 0 : false
                        onTriggered: {
                            mainWindow.fileDialog.savePlaylist()
                        }
                    },
                    Kirigami.Action {
                        id: loadPlaylistButton
                        text: i18nc("@action:button Load a playlist file", "Open…")
                        tooltip: i18nc("@info:tooltip", "Open music files or saved playlists")
                        icon.name: 'document-open'
                        displayHint: Kirigami.DisplayHint.KeepVisible
                        onTriggered: {
                            mainWindow.fileDialog.loadPlaylistOrMusicFile()
                        }
                    }
                ]
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Kirigami.InlineMessage {
            id: partiallyLoadedMessage
            Layout.fillWidth: true

            visible: false
            showCloseButton: true

            type: Kirigami.MessageType.Warning
            position: Kirigami.InlineMessage.Header
            text: i18nc("@info", "Failed to load some tracks. Make sure that they have not been removed or renamed.")

            actions: [
                Kirigami.Action {
                    id: actionButton
                    visible: false
                    text: i18nc("@action:button", "Edit Playlist File")
                    icon.name: "document-edit"
                    onTriggered: {
                        ElisaApplication.mediaPlayListProxyModel.openLoadedPlayList()
                        partiallyLoadedMessage.visible = false
                    }
                }
            ]

            onVisibleChanged: {
                if (!visible) {
                    ElisaApplication.mediaPlayListProxyModel.resetPartiallyLoaded()
                }
            }

            Connections {
                target: ElisaApplication.mediaPlayListProxyModel
                function onPartiallyLoadedChanged() {
                    partiallyLoadedMessage.visible = ElisaApplication.mediaPlayListProxyModel.partiallyLoaded
                }
                function onCanOpenLoadedPlaylistChanged() {
                    actionButton.visible = ElisaApplication.mediaPlayListProxyModel.canOpenLoadedPlaylist
                }
            }
        }

        // ========== desktop listview ==========
        Component {
            id: desktopListView

            ScrollView {
                property alias list: playListView

                // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                contentItem: ListView {
                    id: playListView

                    signal moveItemRequested(int oldIndex, int newIndex)
                    property bool draggingEntry: false

                    reuseItems: true
                    clip: true
                    keyNavigationEnabled: true
                    activeFocusOnTab: count > 0

                    currentIndex: -1

                    onActiveFocusChanged: {
                        if (activeFocus) {
                            // callLater to make sure this is a Tab focusing
                            Qt.callLater(function() {
                                // if the currentItem is not visible, focus on the top fully visible item
                                if (!itemIsFullyVisible(currentIndex)) {
                                    const topIndex = indexAt(0, contentY)
                                    if (topIndex > -1) {
                                        playListView.currentIndex = Math.min(itemIsFullyVisible(topIndex) ? topIndex : topIndex + 1, playListView.count - 1)
                                        playListView.currentItem.forceActiveFocus()
                                    }
                                }
                            })
                        }
                    }

                    function itemIsFullyVisible(index) {
                        if (index < 0 || index > count - 1) {
                            return false
                        }
                        const item = itemAtIndex(index)
                        // `item === null` can be true if the delegate is not loaded, hence check `item`
                        return item && contentY <= item.y && item.y <= (contentY + height)
                    }

                    Accessible.role: Accessible.List
                    Accessible.name: topItem.title

                    section.property: 'albumSection'
                    section.criteria: ViewSection.FullString

                    model: ElisaApplication.mediaPlayListProxyModel

                    delegate: ColumnLayout {
                        id: playListDelegate

                        required property var model
                        property bool nextDelegateHasSection

                        width: playListView.width
                        spacing: 0

                        onFocusChanged: {
                            if (focus) {
                                entry.forceActiveFocus()
                            }
                        }

                        Loader {
                            id: albumSection
                            active: entry.sectionVisible
                            visible: active
                            Layout.fillWidth: true
                            sourceComponent: BasicPlayListAlbumHeader {
                                headerData: JSON.parse(playListDelegate.ListView.section)
                            }
                        }

                        // entry's placeholder
                        // otherwise the layout is broken while dragging
                        FocusScope {
                            implicitWidth: entry.width
                            // the height must be set because entry's parent
                            // will be changed while dragging
                            implicitHeight: entry.height

                            // because this is a FocusScope, clicking on any button inside PlayListEntry
                            // will change this activeFocus
                            onActiveFocusChanged: {
                                if (activeFocus) {
                                    playListView.currentIndex = playListDelegate.model.index
                                    entry.forceActiveFocus()
                                }
                            }

                            // ListItemDragHandle requires listItem
                            // to be a child of delegate
                            PlayListEntry {
                                id: entry

                                width: playListView.width

                                index: playListDelegate.model.index
                                isSelected: playListView.currentIndex === index

                                databaseId: playListDelegate.model.databaseId ? playListDelegate.model.databaseId : 0
                                entryType: playListDelegate.model.entryType ? playListDelegate.model.entryType : ElisaUtils.Unknown
                                title: playListDelegate.model.title ? playListDelegate.model.title : ''
                                artist: playListDelegate.model.artist ? playListDelegate.model.artist : ''
                                album: playListDelegate.model.album ? playListDelegate.model.album : ''
                                albumArtist: playListDelegate.model.albumArtist ? playListDelegate.model.albumArtist : ''
                                duration: playListDelegate.model.duration ? playListDelegate.model.duration : ''
                                fileName: playListDelegate.model.trackResource ? playListDelegate.model.trackResource : ''
                                imageUrl: playListDelegate.model.imageUrl ? playListDelegate.model.imageUrl : ''
                                trackNumber: playListDelegate.model.trackNumber ? playListDelegate.model.trackNumber : -1
                                discNumber: playListDelegate.model.discNumber ? playListDelegate.model.discNumber : -1
                                rating: playListDelegate.model.rating ? playListDelegate.model.rating : 0
                                isSingleDiscAlbum: playListDelegate.model.isSingleDiscAlbum !== undefined ? playListDelegate.model.isSingleDiscAlbum : true
                                isValid: playListDelegate.model.isValid
                                isPlaying: playListDelegate.model.isPlaying
                                metadataModifiableRole: playListDelegate.model.metadataModifiableRole ? playListDelegate.model.metadataModifiableRole : false
                                listView: playListView
                                listDelegate: playListDelegate
                                showDragHandle: playListView.count > 1
                            }
                        }
                    }

                    onCountChanged: if (count === 0) {
                        currentIndex = -1;
                    }

                    Loader {
                        anchors.centerIn: parent
                        width: parent.width - (Kirigami.Units.largeSpacing * 4)

                        active: playListView.count === 0
                        visible: active && status === Loader.Ready

                        sourceComponent: Kirigami.PlaceholderMessage {
                            icon.name: "view-media-playlist"
                            text: i18nc("@info:placeholder", "Playlist is empty")
                            explanation: i18nc("@info:usagetip", "Add some songs to get started. You can browse your music using the views on the left.")
                        }
                    }

                    /* currently disabled animations due to display corruption
                    because of https://bugreports.qt.io/browse/QTBUG-49868
                    causing https://bugs.kde.org/show_bug.cgi?id=406524
                    and https://bugs.kde.org/show_bug.cgi?id=398093
                    add: Transition {
                        NumberAnimation {
                            property: "opacity";
                            from: 0;
                            to: 1;
                            duration: Kirigami.Units.shortDuration }
                    }

                    populate: Transition {
                        NumberAnimation {
                            property: "opacity";
                            from: 0;
                            to: 1;
                            duration: Kirigami.Units.shortDuration }
                    }

                    remove: Transition {
                        NumberAnimation {
                            property: "opacity";
                            from: 1.0;
                            to: 0;
                            duration: Kirigami.Units.shortDuration }
                    }

                    displaced: Transition {
                        NumberAnimation {
                            properties: "x,y";
                            duration: Kirigami.Units.shortDuration
                            easing.type: Easing.InOutQuad }
                    }
                    */
                }
            }
        }

        // ========== mobile listview ==========
        Component {
            id: mobileListView

            ScrollView {
                property alias list: playListView

                contentItem: ListView {
                    id: playListView

                    reuseItems: true
                    model: ElisaApplication.mediaPlayListProxyModel
                    activeFocusOnTab: count > 0

                    moveDisplaced: Transition {
                        YAnimator {
                            duration: Kirigami.Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }

                    Loader {
                        anchors.centerIn: parent
                        width: parent.width - (Kirigami.Units.largeSpacing * 4)

                        active: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount === 0 : true
                        visible: active && status === Loader.Ready

                        sourceComponent: Kirigami.PlaceholderMessage {
                            anchors.centerIn: parent

                            icon.name: "view-media-playlist"
                            text: i18nc("@info:placeholder", "Your playlist is empty.")
                        }
                    }

                    delegate: Item {
                        id: mobileDelegate
                        required property var model

                        width: entry.width
                        height: entry.height

                        onFocusChanged: {
                            if (focus) {
                                entry.forceActiveFocus()
                            }
                        }

                        // ListItemDragHandle requires listItem
                        // to be a child of delegate
                        MobilePlayListDelegate {
                            id: entry
                            width: playListView.width

                            index: mobileDelegate.model.index
                            isSelected: playListView.currentIndex === index

                            databaseId: mobileDelegate.model.databaseId ?? 0
                            entryType: mobileDelegate.model.entryType ?? ElisaUtils.Unknown
                            title: mobileDelegate.model.title ?? ''
                            artist: mobileDelegate.model.artist ?? ''
                            album: mobileDelegate.model.album ?? ''
                            albumArtist: mobileDelegate.model.albumArtist ?? ''
                            duration: mobileDelegate.model.duration ?? ''
                            fileName: mobileDelegate.model.trackResource ?? ''
                            imageUrl: mobileDelegate.model.imageUrl ?? ''
                            trackNumber: mobileDelegate.model.trackNumber ?? -1
                            discNumber: mobileDelegate.model.discNumber ?? -1
                            rating: mobileDelegate.model.rating ?? 0
                            isSingleDiscAlbum: mobileDelegate.model.isSingleDiscAlbum ?? true
                            isValid: mobileDelegate.model.isValid
                            isPlaying: mobileDelegate.model.isPlaying ?? false
                            metadataModifiableRole: mobileDelegate.model.metadataModifiableRole ?? false
                            hideDiscNumber: isSingleDiscAlbum
                            showDragHandle: playListView.count > 1

                            listView: playListView

                            onActiveFocusChanged: {
                                if (activeFocus && playListView.currentIndex !== index) {
                                    playListView.currentIndex = index
                                }
                            }
                        }
                    }
                }
            }
        }

        Loader {
            id: playListLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            sourceComponent: Kirigami.Settings.isMobile ? mobileListView : desktopListView
            onLoaded: playListView = item.list
        }
    }

    footer: ToolBar {
        implicitHeight: Math.round(Kirigami.Units.gridUnit * 2)

        leftPadding: mirrored ? undefined : Kirigami.Units.largeSpacing
        rightPadding: mirrored ? Kirigami.Units.largeSpacing : undefined

        LabelWithToolTip {
            id: durationLabel
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            readonly property int currentSongPosition: ElisaApplication.audioControl.playerPosition
            readonly property int remainingTracksDuration: ElisaApplication.mediaPlayListProxyModel.remainingTracksDuration
            readonly property int remainingDuration: remainingTracksDuration - currentSongPosition
            readonly property int totalDuration: ElisaApplication.mediaPlayListProxyModel.totalTracksDuration

            ProgressIndicator {
                id: durationLeftIndicator
                position: durationLabel.remainingDuration
            }
            ProgressIndicator {
                id: totalDurationIndicator
                position: durationLabel.totalDuration
            }

            TextMetrics {
                id: longText
                font.family: durationLabel.font.family
                text: {
                    const nTracks = i18ncp("@info:status %2 is always bigger than 1", "%1/%2 tracks remaining", "%1/%2 tracks remaining", ElisaApplication.mediaPlayListProxyModel.remainingTracks, ElisaApplication.mediaPlayListProxyModel.tracksCount)
                    const nDuration = i18nc("@info:status %1 and %2 are formatted like hh:mm:ss or mm:ss", "%1 remaining, %2 total", durationLeftIndicator.progressDuration, totalDurationIndicator.progressDuration);
                    return i18nc("@info:status %1 is total tracks and tracks remaining. %2 is total time and time remaining", "%1, %2", nTracks, nDuration);
                }
            }
            TextMetrics {
                id: middleText
                font.family: durationLabel.font.family
                text: {
                    const nTracks = i18ncp("@info:status %2 is always bigger than 1", "%1/%2 tracks remaining", "%1/%2 tracks remaining", ElisaApplication.mediaPlayListProxyModel.remainingTracks, ElisaApplication.mediaPlayListProxyModel.tracksCount)
                    const nDuration = i18nc("@info:status %1 and %2 are formatted like hh:mm:ss or mm:ss", "%1/%2 remaining", durationLeftIndicator.progressDuration, totalDurationIndicator.progressDuration);
                    return i18nc("@info:status %1 is total tracks and tracks remaining. %2 is total time and time remaining", "%1, %2", nTracks, nDuration);
                }
            }
            TextMetrics {
                id: shortText
                font.family: durationLabel.font.family
                text: {
                    const nTracks = i18ncp("@info:status %2 is always bigger than 1", "%1/%2 tracks", "%1/%2 tracks", ElisaApplication.mediaPlayListProxyModel.remainingTracks, ElisaApplication.mediaPlayListProxyModel.tracksCount)
                    const nDuration = i18nc("@info:status %1 and %2 are formatted like hh:mm:ss or mm:ss", "%1/%2 remaining", durationLeftIndicator.progressDuration, totalDurationIndicator.progressDuration);
                    return i18nc("@info:status %1 is total tracks and tracks remaining. %2 is total time and time remaining", "%1, %2", nTracks, nDuration);
                }
            }

            text: {
                if (ElisaApplication.mediaPlayListProxyModel.radioCount !== 0) {
                    // the current track is a radio -> duration remaining is therefore infinite -> it isn't displayed
                    const nTracks = i18ncp("@info:status %1 is the total number of tracks of the playlist", "%1 track", "%1 tracks", ElisaApplication.mediaPlayListProxyModel.tracksCount)
                    const nRadios = i18ncp("@info:status %1 is the total number of radios of the playlist", "%1 radio", "%1 radios", ElisaApplication.mediaPlayListProxyModel.radioCount)
                    return i18nc("@info:status %1 is the translation of tracks, %2 is the translation of total radios", "%1, %2 total", nTracks, nRadios);
                } else if (ElisaApplication.mediaPlayListProxyModel.remainingTracks === -1) {
                    // track or playlist is on repeat: only show total tracks and duration, since remaining is infinite
                    return i18ncp("@info:status %1 is the total tracks of the playlist. %2 is the total duration formatted like hh:mm:ss or mm:ss", "%1 track, %2 total","%1 tracks, %2 total", ElisaApplication.mediaPlayListProxyModel.tracksCount, totalDurationIndicator.progressDuration);
                } else if (ElisaApplication.mediaPlayListProxyModel.tracksCount === 1) {
                    return i18nc("@info:status %1 is the remaining duration of the playlist. It is formatted like hh:mm:ss or mm:ss", "1 track, %1min remaining", durationLeftIndicator.progressDuration);
                } else if (durationLabel.width >= longText.boundingRect.width) {
                    return longText.text;
                } else if (durationLabel.width >= middleText.boundingRect.width) {
                    return middleText.text;
                } else {
                    return shortText.text;
                }
            }
            elide: Text.ElideRight
            font.features: { "tnum": 1 }
        }
    }
}
