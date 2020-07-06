/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.5
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import Qt.labs.platform 1.0 as PlatformDialog
import org.kde.kirigami 2.12 as Kirigami
import org.kde.elisa 1.0

FocusScope {
    property StackView parentStackView

    property int placeholderHeight: elisaTheme.dragDropPlaceholderHeight

    signal startPlayback()
    signal pausePlayback()

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

    function hideNotification() {
        playListNotification.visible = false;
    }

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
        onPlayListLoadFailed: {
            showPlayListNotification(i18nc("Message when playlist load failed", "Loading failed"), Kirigami.MessageType.Error, retryLoadAction)
        }
    }

    Connections {
         target: ElisaApplication.mediaPlayListProxyModel
         onDisplayUndoNotification: {
             showPlayListNotification(i18nc("Playlist cleared", "Playlist cleared"), Kirigami.MessageType.Information, undoAction)
         }
    }

    Connections {
         target: ElisaApplication.mediaPlayListProxyModel
         onHideUndoNotification: hideNotification()
    }

    id: topItem

    Accessible.role: Accessible.Pane
    Accessible.name: viewTitle.text

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

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Header with title and toolbar buttons
        HeaderFooterToolbar {
            type: "header"
            contentItems: [

                // Header title
                LabelWithToolTip {
                    id: viewTitle

                    Layout.fillWidth: true

                    text: i18nc("Title of the view of the playlist", "Playlist")

                    level: 1
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                },

                // Toolbar buttons
                FlatButtonWithToolTip {
                    text: i18nc("Show currently played track inside playlist", "Show Current Track")
                    icon.name: 'media-track-show-active'
                    enabled: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount > 0 : false
                    onClicked: {
                        playListView.positionViewAtIndex(ElisaApplication.mediaPlayListProxyModel.currentTrackRow, ListView.Contain)
                        playListView.currentIndex = ElisaApplication.mediaPlayListProxyModel.currentTrackRow
                        playListView.currentItem.forceActiveFocus()
                    }
                },
                FlatButtonWithToolTip {
                    id: savePlaylistButton
                    text: i18nc("Save a playlist file", "Save Playlist...")
                    icon.name: 'document-save'
                    enabled: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount > 0 : false
                    onClicked: {
                        fileDialog.fileMode = PlatformDialog.FileDialog.SaveFile
                        fileDialog.file = ''
                        fileDialog.open()
                    }
                },
                FlatButtonWithToolTip {
                    id: loadPlaylistButton
                    text: i18nc("Load a playlist file", "Load Playlist...")
                    icon.name: 'document-open'
                    onClicked: {
                        fileDialog.fileMode = PlatformDialog.FileDialog.OpenFile
                        fileDialog.file = ''
                        fileDialog.open()
                    }
                },
                FlatButtonWithToolTip {
                    text: i18nc("Remove all tracks from play list", "Clear Playlist")
                    icon.name: 'edit-clear-all'
                    enabled: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount > 0 : false
                    onClicked: ElisaApplication.mediaPlayListProxyModel.clearPlayList()
                }
            ]
        }

        Item {
            id: emptyPlaylistMessage

            visible: ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount === 0 : true

            Layout.fillHeight: true
            Layout.fillWidth: true

            Kirigami.PlaceholderMessage {
                anchors.centerIn: parent
                width: parent.width - (Kirigami.Units.largeSpacing * 4)

                text: xi18nc("@info", "Your playlist is empty.<nl/><nl/>Add some songs to get started. You can browse your music using the views on the left.")
            }
        }

        PlayListBasicView {
            id: playListView

            visible: !emptyPlaylistMessage.visible

            Layout.fillWidth: true
            Layout.fillHeight: true

            title: viewTitle.text
            playListModel: ElisaApplication.mediaPlayListProxyModel

            focus: true

            onStartPlayback: topItem.startPlayback()

            onPausePlayback: topItem.pausePlayback()

            onDisplayError: showPlayListNotification(errorText, Kirigami.MessageType.Error)

        }

        Kirigami.InlineMessage {
            id: playListNotification

            Timer {
                id: autoHideNotificationTimer

                interval: 7000

                onTriggered: playListNotification.visible = false
            }

            type: Kirigami.MessageType.Information
            showCloseButton: true
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.largeSpacing

            onVisibleChanged:
            {
                if (visible) {
                    autoHideNotificationTimer.start()
                } else {
                    autoHideNotificationTimer.stop()
                }
            }
        }

        // Footer with number of tracks label
        HeaderFooterToolbar {
            type: "footer"
            contentItems: [
                LabelWithToolTip {
                    id: trackCountLabel

                    Layout.fillWidth: true

                    text: i18np("1 track", "%1 tracks", (ElisaApplication.mediaPlayListProxyModel ? ElisaApplication.mediaPlayListProxyModel.tracksCount : 0))
                    elide: Text.ElideLeft
                }
            ]
        }
    }
}


