/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2019 Nate Graham <nate@kde.org>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.5
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import Qt.labs.platform 1.0 as PlatformDialog
import org.kde.kirigami 2.5 as Kirigami
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
        onTriggered: elisa.mediaPlayListProxyModel.undoClearPlayList()
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
        target: elisa.mediaPlayListProxyModel
        onPlayListLoadFailed: {
            showPlayListNotification(i18nc("Message when playlist load failed", "Loading failed"), Kirigami.MessageType.Error, retryLoadAction)
        }
    }

    Connections {
         target: elisa.mediaPlayListProxyModel
         onDisplayUndoNotification: {
             showPlayListNotification(i18nc("Playlist cleared", "Playlist cleared"), Kirigami.MessageType.Information, undoAction)
         }
    }

    Connections {
         target: elisa.mediaPlayListProxyModel
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
                if (!elisa.mediaPlayListProxyModel.savePlayList(fileDialog.file)) {
                    showPlayListNotification(i18nc("Message when saving a playlist failed", "Saving failed"), Kirigami.MessageType.Error, retrySaveAction)
                }
            } else {
                elisa.mediaPlayListProxyModel.loadPlayList(fileDialog.file)
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
                    enabled: elisa.mediaPlayListProxyModel ? elisa.mediaPlayListProxyModel.tracksCount > 0 : false
                    onClicked: {
                        playListView.positionViewAtIndex(elisa.mediaPlayListProxyModel.currentTrackRow, ListView.Contain)
                        playListView.currentIndex = elisa.mediaPlayListProxyModel.currentTrackRow
                        playListView.currentItem.forceActiveFocus()
                    }
                },
                FlatButtonWithToolTip {
                    id: savePlaylistButton
                    text: i18nc("Save a playlist file", "Save Playlist...")
                    icon.name: 'document-save'
                    enabled: elisa.mediaPlayListProxyModel ? elisa.mediaPlayListProxyModel.tracksCount > 0 : false
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
                    enabled: elisa.mediaPlayListProxyModel ? elisa.mediaPlayListProxyModel.tracksCount > 0 : false
                    onClicked: elisa.mediaPlayListProxyModel.clearPlayList()
                }
            ]
        }

        ColumnLayout {
            id: emptyPlaylistText
            spacing: 0
            visible: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillHeight: true
            Layout.fillWidth: true

            Item {
                id: emptyVisible
                visible: elisa.mediaPlayListProxyModel ? elisa.mediaPlayListProxyModel.tracksCount === 0 : true
                Layout.fillHeight: true
            }

            Image {
                id: emptyImage
                visible: emptyVisible.visible
                Layout.alignment: Qt.AlignHCenter

                width: elisaTheme.gridDelegateSize
                height: elisaTheme.gridDelegateSize

                source: elisaTheme.playlistIcon
                opacity: 0.25

                sourceSize {
                    width: elisaTheme.gridDelegateSize
                    height: elisaTheme.gridDelegateSize
                }
            }

            LabelWithToolTip {
                id: emptyLabel0
                visible: emptyVisible.visible
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.leftMargin: Kirigami.Units.largeSpacing

                level: 1
                wrapMode: Text.WordWrap

                horizontalAlignment: Text.AlignHCenter
                text: i18nc("Your playlist is empty", "Your playlist is empty")
            }

            Label {
                id: emptyLabel1
                visible: emptyVisible.visible
                Layout.topMargin: Kirigami.Units.largeSpacing
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.leftMargin: Kirigami.Units.largeSpacing

                wrapMode: Text.WordWrap

                horizontalAlignment: Text.AlignHCenter
                text: i18nc("Text shown when play list is empty", "Add some songs to get started. You can browse your music using the views on the left.")
            }

            Item {
                visible: emptyVisible.visible
                Layout.fillHeight: true
            }

            PlayListBasicView {
                id: playListView

                visible: !emptyVisible.visible

                Layout.fillWidth: true
                Layout.fillHeight: true

                title: viewTitle.text
                playListModel: elisa.mediaPlayListProxyModel

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
                Layout.topMargin: Kirigami.Units.largeSpacing
                Layout.fillWidth: true
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.leftMargin: Kirigami.Units.largeSpacing

                onVisibleChanged:
                {
                    if (visible) {
                        autoHideNotificationTimer.start()
                    } else {
                        autoHideNotificationTimer.stop()
                    }
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

                    text: i18np("1 track", "%1 tracks", (elisa.mediaPlayListProxyModel ? elisa.mediaPlayListProxyModel.tracksCount : 0))
                    elide: Text.ElideLeft
                }
            ]
        }
    }
}


