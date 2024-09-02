/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml.Models 2.1
import org.kde.kirigami 2.15 as Kirigami
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
                implicitHeight: elisaTheme.toolBarHeaderMinimumHeight
            }

            Kirigami.ActionToolBar {
                Layout.fillWidth: true
                alignment: Qt.AlignRight

                actions: [
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
                        text: i18nc("@action:button Load a playlist file", "Load…")
                        icon.name: 'document-open'
                        displayHint: Kirigami.DisplayHint.KeepVisible
                        onTriggered: {
                            mainWindow.fileDialog.loadPlaylist()
                        }
                    }
                ]
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        Kirigami.InlineMessage {
            id: partiallyLoadedMessage
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.smallSpacing

            visible: false
            showCloseButton: true

            type: Kirigami.MessageType.Warning
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
                                    playListView.currentIndex = model.index
                                    entry.forceActiveFocus()
                                }
                            }

                            // ListItemDragHandle requires listItem
                            // to be a child of delegate
                            PlayListEntry {
                                id: entry

                                width: playListView.width

                                index: model.index
                                isSelected: playListView.currentIndex === index

                                databaseId: model.databaseId ? model.databaseId : 0
                                entryType: model.entryType ? model.entryType : ElisaUtils.Unknown
                                title: model.title ? model.title : ''
                                artist: model.artist ? model.artist : ''
                                album: model.album ? model.album : ''
                                albumArtist: model.albumArtist ? model.albumArtist : ''
                                duration: model.duration ? model.duration : ''
                                fileName: model.trackResource ? model.trackResource : ''
                                imageUrl: model.imageUrl ? model.imageUrl : ''
                                trackNumber: model.trackNumber ? model.trackNumber : -1
                                discNumber: model.discNumber ? model.discNumber : -1
                                rating: model.rating ? model.rating : 0
                                isSingleDiscAlbum: model.isSingleDiscAlbum !== undefined ? model.isSingleDiscAlbum : true
                                isValid: model.isValid
                                isPlaying: model.isPlaying
                                metadataModifiableRole: model && model.metadataModifiableRole ? model.metadataModifiableRole : false
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

                            index: model ? model.index : 0
                            isSelected: playListView.currentIndex === index

                            databaseId: model && model.databaseId ? model.databaseId : 0
                            entryType: model && model.entryType ? model.entryType : ElisaUtils.Unknown
                            title: model ? model.title || '' : ''
                            artist: model ? model.artist || '' : ''
                            album: model ? model.album || '' : ''
                            albumArtist: model ? model.albumArtist || '' : ''
                            duration: model ? model.duration || '' : ''
                            fileName: model ? model.trackResource || '' : ''
                            imageUrl: model ? model.imageUrl || '' : ''
                            trackNumber: model ? model.trackNumber || -1 : -1
                            discNumber: model ? model.discNumber || -1 : -1
                            rating: model ? model.rating || 0 : 0
                            isSingleDiscAlbum: model && model.isSingleDiscAlbum !== undefined ? model.isSingleDiscAlbum : true
                            isValid: model && model.isValid
                            isPlaying: model ? model.isPlaying : false
                            metadataModifiableRole: model && model.metadataModifiableRole ? model.metadataModifiableRole : false
                            hideDiscNumber: model && model.isSingleDiscAlbum
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

        Kirigami.InlineMessage {
            id: mobileClearedMessage
            Layout.fillWidth: true
            visible: false
            showCloseButton: true
            text: i18nc("@label", "Playlist cleared")

            actions: [
                Kirigami.Action {
                    text: i18nc("@action:button", "Undo")
                    icon.name: "edit-undo-symbolic"
                    onTriggered: ElisaApplication.mediaPlayListProxyModel.undoClearPlayList()
                }
            ]
        }
    }

    footer: ToolBar {
        implicitHeight: Math.round(Kirigami.Units.gridUnit * 2)

        leftPadding: mirrored ? undefined : Kirigami.Units.largeSpacing
        rightPadding: mirrored ? Kirigami.Units.largeSpacing : undefined

        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.smallSpacing

            LabelWithToolTip {
                text: {
                    if (ElisaApplication.mediaPlayListProxyModel.remainingTracks === -1) {
                        return i18ncp("@info:status", "%1 track", "%1 tracks", ElisaApplication.mediaPlayListProxyModel.tracksCount);
                    } else {
                        if (ElisaApplication.mediaPlayListProxyModel.tracksCount == 1) {
                            return i18nc("@info:status", "1 track");
                        } else {
                            var nTracks = i18ncp("@info:status", "%1 track", "%1 tracks", ElisaApplication.mediaPlayListProxyModel.tracksCount);
                            var nRemaining = i18ncp("@info:status number of tracks remaining", "%1 remaining", "%1 remaining", ElisaApplication.mediaPlayListProxyModel.remainingTracks);
                            return i18nc("@info:status %1 is the translation of track[s], %2 is the translation of remaining", "%1 (%2)", nTracks, nRemaining);
                        }
                    }
                }
                elide: Text.ElideLeft
            }

            Kirigami.ActionToolBar {
                Layout.fillWidth: true
                Layout.fillHeight: true
                alignment: Qt.AlignRight

                actions: [
                    Kirigami.Action {
                        text: i18nc("@action:button", "Show Current")
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
                        icon.name: 'edit-clear-all'
                        displayHint: Kirigami.DisplayHint.KeepVisible
                        enabled: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount > 0 : false
                        onTriggered: ElisaApplication.mediaPlayListProxyModel.clearPlayList()
                    }
                ]
            }
        }
    }
}

