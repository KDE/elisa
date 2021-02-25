/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml.Models 2.1
import Qt.labs.platform 1.0 as PlatformDialog
import org.kde.kirigami 2.15 as Kirigami
import org.kde.elisa 1.0

// Not using ScrollablePage because we don't need any of the refresh features
// that it provides
Kirigami.Page {
    id: topItem

    signal startPlayback()
    signal pausePlayback()

    function hideNotification() {
        playListNotification.visible = false;
    }

    function showPlayListNotification(message, type, action) {
        if (!message) {
            return;
        }

        if (type) {
            playListNotification.type = type;
        } else {
            playListNotification.type = Kirigami.MessageType.Information;
        }

        if (action) {
            playListNotification.actions = action;
        } else {
            playListNotification.actions = [];
        }

        playListNotification.text = message ? message : "";
        playListNotification.visible = true;
    }


    title: i18nc("@info Title of the view of the playlist; keep this string as short as possible because horizontal space is quite scarce", "Playlist")
    padding: 0

    // Use view colors so the background is white
    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    Accessible.role: Accessible.Pane
    Accessible.name: topItem.title

    Kirigami.Action {
        id: undoAction
        text: i18nc("Undo", "Undo")
        icon.name: "dialog-cancel"
        onTriggered: ElisaApplication.mediaPlayListProxyModel.undoClearPlayList()
    }

    Kirigami.Action {
        id: retryLoadAction
        text: i18nc("Retry", "Retry")
        icon.name: "edit-redo"
        onTriggered: loadPlaylistButton.clicked()
    }

    Kirigami.Action {
        id: retrySaveAction
        text: i18nc("Retry", "Retry")
        icon.name: "edit-redo"
        onTriggered: savePlaylistButton.clicked()
    }

    Connections {
        target: ElisaApplication.mediaPlayListProxyModel
        function onPlayListLoadFailed() {
            showPlayListNotification(i18nc("Message when playlist load failed", "Loading failed"), Kirigami.MessageType.Error, retryLoadAction)
        }
    }

    Connections {
         target: ElisaApplication.mediaPlayListProxyModel
         function onDisplayUndoNotification() {
             showPlayListNotification(i18nc("Playlist cleared", "Playlist cleared"), Kirigami.MessageType.Information, undoAction)
         }
    }

    Connections {
         target: ElisaApplication.mediaPlayListProxyModel
         function onHideUndoNotification() {
            hideNotification()
         }
    }

    // FIXME: Apparently the page needs to have a row or a stack to show the
    // header toolbar automatically, so we have to make our own See
    // https://bugs.kde.org/show_bug.cgi?id=432541
    // globalToolBarStyle: Kirigami.ApplicationHeaderStyle.ToolBar

    header: ToolBar {
        // Override color to use standard window colors, not header colors
        // TODO: remove this if the HeaderBar component is ever removed or moved
        // to the bottom of the window such that this toolbar touches the window
        // titlebar
        Kirigami.Theme.colorSet: Kirigami.Theme.Window

        RowLayout {
            anchors.fill: parent

            Kirigami.Heading {
                text: topItem.title
            }
            Kirigami.ActionToolBar {
                Layout.fillWidth: true
                alignment: Qt.AlignRight

                actions: [
                    Kirigami.Action {
                        text: i18nc("Show currently played track inside playlist", "Show Current Track")
                        icon.name: 'media-track-show-active'
                        displayHint: Kirigami.DisplayHint.KeepVisible | Kirigami.DisplayHint.IconOnly
                        enabled: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount > 0 : false
                        onTriggered: {
                            playListView.positionViewAtIndex(ElisaApplication.mediaPlayListProxyModel.currentTrackRow, ListView.Contain)
                            playListView.currentIndex = ElisaApplication.mediaPlayListProxyModel.currentTrackRow
                            playListView.currentItem.forceActiveFocus()
                        }
                    },
                    Kirigami.Action {
                        id: savePlaylistButton
                        text: i18nc("Save a playlist file", "Save Playlist...")
                        icon.name: 'document-save'
                        displayHint: Kirigami.DisplayHint.KeepVisible | Kirigami.DisplayHint.IconOnly
                        enabled: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount > 0 : false
                        onTriggered: {
                            fileDialog.fileMode = PlatformDialog.FileDialog.SaveFile
                            fileDialog.file = ''
                            fileDialog.open()
                        }
                    },
                    Kirigami.Action {
                        id: loadPlaylistButton
                        text: i18nc("Load a playlist file", "Load Playlist...")
                        icon.name: 'document-open'
                        displayHint: Kirigami.DisplayHint.KeepVisible | Kirigami.DisplayHint.IconOnly
                        onTriggered: {
                            fileDialog.fileMode = PlatformDialog.FileDialog.OpenFile
                            fileDialog.file = ''
                            fileDialog.open()
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("Remove all tracks from play list", "Clear Playlist")
                        icon.name: 'edit-clear-all'
                        displayHint: Kirigami.DisplayHint.KeepVisible | Kirigami.DisplayHint.IconOnly
                        enabled: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount > 0 : false
                        onTriggered: ElisaApplication.mediaPlayListProxyModel.clearPlayList()
                    }
                ]
            }
        }
    }

    ScrollView {
        anchors.fill: parent

        ListView {
            id: playListView

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

            model: DelegateModel {
                model: ElisaApplication.mediaPlayListProxyModel

                groups: [
                    DelegateModelGroup { name: "selected" }
                ]

                delegate: DraggableItem {
                    id: item
                    width: playListView.width
                    placeholderHeight: elisaTheme.dragDropPlaceholderHeight

                    focus: true

                    PlayListEntry {
                        id: entry

                        focus: true

                        width: parent.width

                        index: model.index
                        isAlternateColor: item.DelegateModel.itemsIndex % 2
                        isSelected: playListView.currentIndex === index
                        containsMouse: item.containsMouse

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
                        metadataModifiableRole: model.metadataModifiableRole

                        onStartPlayback: topItem.startPlayback()
                        onPausePlayback: topItem.pausePlayback()
                        onRemoveFromPlaylist: ElisaApplication.mediaPlayListProxyModel.removeRow(trackIndex)
                        onSwitchToTrack: ElisaApplication.mediaPlayListProxyModel.switchTo(trackIndex)

                        onActiveFocusChanged: {
                            if (activeFocus && playListView.currentIndex !== index) {
                                playListView.currentIndex = index
                            }
                        }
                    }

                    draggedItemParent: playListView

                    onClicked: {
                        playListView.currentIndex = index
                        entry.forceActiveFocus()
                    }

                    onDoubleClicked: {
                        if (model.isValid) {
                            ElisaApplication.mediaPlayListProxyModel.switchTo(model.index)
                            topItem.startPlayback()
                        }
                    }

                    onMoveItemRequested: {
                        ElisaApplication.mediaPlayListProxyModel.moveRow(from, to);
                    }
                }
            }

            onCountChanged: if (count === 0) {
                currentIndex = -1;
            }

            Kirigami.PlaceholderMessage {
                anchors.centerIn: parent
                width: parent.width - (Kirigami.Units.largeSpacing * 4)
                text: xi18nc("@info", "Your playlist is empty.<nl/><nl/>Add some songs to get started. You can browse your music using the views on the left.")
                visible: playListView.count === 0
            }

            Kirigami.InlineMessage {
                id: playListNotification

                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    margins: Kirigami.Units.largeSpacing
                }

                type: Kirigami.MessageType.Information
                showCloseButton: true

                onVisibleChanged: {
                    if (visible) {
                        autoHideNotificationTimer.start()
                    } else {
                        autoHideNotificationTimer.stop()
                    }
                }

                Timer {
                    id: autoHideNotificationTimer
                    interval: 7000
                    onTriggered: playListNotification.visible = false
                }
            }
        }
    }

    footer: ToolBar {
        implicitHeight: Math.round(Kirigami.Units.gridUnit * 2)

        RowLayout {
            anchors.fill: parent

            LabelWithToolTip {
                text: i18np("%1 track", "%1 tracks", (ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount : 0))
                elide: Text.ElideLeft
            }
            Item {
                Layout.fillWidth: true
            }
            LabelWithToolTip {
                visible: ElisaApplication.mediaPlayListProxyModel.remainingTracks != -1

                text: ElisaApplication.mediaPlayListProxyModel.remainingTracks == 0 ? i18n("Last track") : i18ncp("Number of remaining tracks in a playlist of songs", "%1 remaining", "%1 remaining", ElisaApplication.mediaPlayListProxyModel.remainingTracks)
                elide: Text.ElideRight
            }
        }
    }

    PlatformDialog.FileDialog {
        id: fileDialog

        defaultSuffix: 'm3u'
        folder: PlatformDialog.StandardPaths.writableLocation(PlatformDialog.StandardPaths.MusicLocation)
        nameFilters: [i18nc("file type (mime type) for m3u playlist", "Playlist (*.m3u)")]

        onAccepted:
        {
            if (fileMode === PlatformDialog.FileDialog.SaveFile) {
                if (!ElisaApplication.mediaPlayListProxyModel.savePlayList(fileDialog.file)) {
                    showPlayListNotification(i18nc("Message when saving a playlist failed", "Saving failed"), Kirigami.MessageType.Error, retrySaveAction)
                }
            } else {
                ElisaApplication.mediaPlayListProxyModel.loadPlayList(fileDialog.file)
            }
        }
    }
}


