/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml.Models 2.1
import Qt.labs.platform 1.0 as PlatformDialog
import org.kde.kirigami 2.15 as Kirigami
import org.kde.elisa 1.0

import "mobile"

// Not using ScrollablePage because we don't need any of the refresh features
// that it provides
Kirigami.Page {
    id: topItem

    // set by the respective mobile/desktop view
    property var playListNotification
    property var playListView

    title: i18nc("@info Title of the view of the playlist; keep this string as short as possible because horizontal space is quite scarce", "Playlist")
    padding: 0

    // Use view colors so the background is white
    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    Accessible.role: Accessible.Pane
    Accessible.name: topItem.title

    // TODO: Once we depend on Frameworks 5.80, change this to
    // "Kirigami.ApplicationHeaderStyle.None" and remove the custom header
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None
    header: ToolBar {
        // Override color to use standard window colors, not header colors
        // TODO: remove this if the HeaderBar component is ever removed or moved
        // to the bottom of the window such that this toolbar touches the window
        // titlebar
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        implicitHeight: Math.round(Kirigami.Units.gridUnit * 2.5)
        leftPadding: Kirigami.Units.largeSpacing
        rightPadding: 0

        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing * 3

            Kirigami.Heading {
                text: topItem.title
            }
            Kirigami.ActionToolBar {
                Layout.fillWidth: true
                alignment: Qt.AlignRight

                actions: [
                    Kirigami.Action {
                        text: i18nc("Remove all tracks from play list", "Clear")
                        icon.name: 'edit-clear-all'
                        enabled: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount > 0 : false
                        onTriggered: ElisaApplication.mediaPlayListProxyModel.clearPlayList()
                    },
                    Kirigami.Action {
                        id: loadPlaylistButton
                        text: i18nc("Load a playlist file", "Load…")
                        icon.name: 'document-open'
                        onTriggered: {
                            fileDialog.fileMode = PlatformDialog.FileDialog.OpenFile
                            fileDialog.file = ''
                            fileDialog.open()
                        }
                    },
                    Kirigami.Action {
                        id: savePlaylistButton
                        text: i18nc("Save a playlist file", "Save…")
                        icon.name: 'document-save'
                        enabled: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount > 0 : false
                        onTriggered: {
                            fileDialog.fileMode = PlatformDialog.FileDialog.SaveFile
                            fileDialog.file = ''
                            fileDialog.open()
                        }
                    }
                ]
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

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

                    focus: true
                    clip: true
                    keyNavigationEnabled: true
                    activeFocusOnTab: true

                    currentIndex: -1

                    Accessible.role: Accessible.List
                    Accessible.name: topItem.title

                    section.property: 'albumSection'
                    section.criteria: ViewSection.FullString
                    section.labelPositioning: ViewSection.InlineLabels
                    section.delegate: BasicPlayListAlbumHeader {
                        headerData: JSON.parse(section)
                        width: playListView.width
                    }

                    model: ElisaApplication.mediaPlayListProxyModel

                    onCurrentIndexChanged: {
                        if (currentItem) {
                            currentItem.entry.forceActiveFocus()
                        }
                    }

                    delegate: Item {
                        property alias entry: entry

                        width: entry.width
                        height: entry.height

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
                            showDragHandle: playListView.count > 1
                        }
                    }

                    onCountChanged: if (count === 0) {
                        currentIndex = -1;
                    }

                    Kirigami.PlaceholderMessage {
                        anchors.centerIn: parent
                        width: parent.width - (Kirigami.Units.largeSpacing * 4)
                        text: i18n("Playlist is empty")
                        explanation: i18n("Add some songs to get started. You can browse your music using the views on the left.")
                        visible: playListView.count === 0
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

                    moveDisplaced: Transition {
                        YAnimator {
                            duration: Kirigami.Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }

                    Kirigami.PlaceholderMessage {
                        anchors.centerIn: parent
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: Kirigami.Units.largeSpacing
                        width: parent.width - (Kirigami.Units.largeSpacing * 4)
                        visible: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount === 0 : true

                        icon.name: "view-media-playlist"
                        text: xi18nc("@info", "Your playlist is empty.")
                    }

                    delegate: Item {
                        width: entry.width
                        height: entry.height

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
            text: i18nc("Playlist cleared", "Playlist cleared")

            actions: [
                Kirigami.Action {
                    text: i18n("Undo")
                    icon.name: "edit-undo-symbolic"
                    onTriggered: ElisaApplication.mediaPlayListProxyModel.undoClearPlayList()
                }
            ]
        }
    }

    footer: ToolBar {
        implicitHeight: Math.round(Kirigami.Units.gridUnit * 2)
        leftPadding: Kirigami.Units.largeSpacing
        rightPadding: Kirigami.Units.largeSpacing

        RowLayout {
            anchors.fill: parent

            LabelWithToolTip {
                text: {
                    if (ElisaApplication.mediaPlayListProxyModel.remainingTracks === -1) {
                        return i18np("%1 track", "%1 tracks", ElisaApplication.mediaPlayListProxyModel.tracksCount);
                    } else {
                        if (ElisaApplication.mediaPlayListProxyModel.tracksCount == 1) {
                            return i18n("1 track");
                        } else {
                            var nTracks = i18np("%1 track", "%1 tracks", ElisaApplication.mediaPlayListProxyModel.tracksCount);
                            var nRemaining = i18ncp("number of tracks remaining", "%1 remaining", "%1 remaining", ElisaApplication.mediaPlayListProxyModel.remainingTracks);
                            return i18nc("%1 is the translation of track[s], %2 is the translation of remaining", "%1 (%2)", nTracks, nRemaining);
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
                        text: i18nc("Show currently played track inside playlist", "Show Current")
                        icon.name: 'media-track-show-active'
                        displayHint: Kirigami.DisplayHint.KeepVisible
                        enabled: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount > 0 : false
                        onTriggered: {
                            playListView.positionViewAtIndex(ElisaApplication.mediaPlayListProxyModel.currentTrackRow, ListView.Contain)
                            playListView.currentIndex = ElisaApplication.mediaPlayListProxyModel.currentTrackRow
                            playListView.currentItem.forceActiveFocus()
                        }
                    }
                ]
            }
        }
    }

    PlatformDialog.FileDialog {
        id: fileDialog

        defaultSuffix: 'm3u8'
        folder: PlatformDialog.StandardPaths.writableLocation(PlatformDialog.StandardPaths.MusicLocation)
        nameFilters: [i18nc("file type (mime type) for m3u and m3u8 playlist file formats", "Playlist (*.m3u*)")]

        onAccepted:
        {
            if (fileMode === PlatformDialog.FileDialog.SaveFile) {
                if (!ElisaApplication.mediaPlayListProxyModel.savePlayList(fileDialog.file)) {
                    showPassiveNotification(i18n("Saving failed"), 7000, i18n("Retry"), function() { savePlaylistButton.clicked(); })
                }
            } else {
                ElisaApplication.mediaPlayListProxyModel.loadPlayList(fileDialog.file)
            }
        }
    }
}

