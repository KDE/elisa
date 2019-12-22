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
        onTriggered: elisa.mediaPlayList.undoClearPlayList()
    }

    Kirigami.Action {
        id: retryLoadAction
        text: i18nc("Retry", "Retry")
        icon.name: "edit-redo"
        onTriggered: loadPlaylist.trigger()
    }

    Kirigami.Action {
        id: retrySaveAction
        text: i18nc("Retry", "Retry")
        icon.name: "edit-redo"
        onTriggered: savePlaylist.trigger()
    }

    Connections {
        target: elisa.mediaPlayList
        onPlayListLoadFailed: {
            showPlayListNotification(i18nc("Message when playlist load failed", "Loading failed"), Kirigami.MessageType.Error, retryLoadAction)
        }
    }

    Connections {
         target: elisa.mediaPlayList
         onDisplayUndoInline: {
             showPlayListNotification(i18nc("Playlist cleared", "Playlist cleared"), Kirigami.MessageType.Information, undoAction)
         }
    }

    Connections {
         target: elisa.mediaPlayList
         onHideUndoInline: hideNotification()
    }

    id: topItem

    Accessible.role: Accessible.Pane
    Accessible.name: viewTitle.text

    Action {
        id: clearPlayList
        text: i18nc("Remove all tracks from play list", "Clear Playlist")
        icon.name: 'edit-clear-all'
        enabled: elisa.mediaPlayList ? elisa.mediaPlayList.tracksCount > 0 : false
        onTriggered: elisa.mediaPlayList.clearPlayList()
    }

    Action {
        id: showCurrentTrack
        text: i18nc("Show currently played track inside playlist", "Show Current Track")
        icon.name: 'media-show-active-track-amarok'
        enabled: elisa.mediaPlayList ? elisa.mediaPlayList.tracksCount > 0 : false
        onTriggered: {
            playListView.positionViewAtIndex(elisa.mediaPlayList.currentTrackRow, ListView.Contain)
            playListView.currentIndex = elisa.mediaPlayList.currentTrackRow
            playListView.currentItem.forceActiveFocus()
        }
    }

    Action {
        id: loadPlaylist
        text: i18nc("Load a playlist file", "Load Playlist...")
        icon.name: 'document-open'
        onTriggered:
        {
            fileDialog.fileMode = PlatformDialog.FileDialog.OpenFile
            fileDialog.file = ''
            fileDialog.open()
        }
    }

    Action {
        id: savePlaylist
        text: i18nc("Save a playlist file", "Save Playlist...")
        icon.name: 'document-save'
        enabled: elisa.mediaPlayList ? elisa.mediaPlayList.tracksCount > 0 : false
        onTriggered:
        {
            fileDialog.fileMode = PlatformDialog.FileDialog.SaveFile
            fileDialog.file = ''
            fileDialog.open()
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
                if (!elisa.mediaPlayList.savePlaylist(fileDialog.file)) {
                    showPlayListNotification(i18nc("Message when saving a playlist failed", "Saving failed"), Kirigami.MessageType.Error, retrySaveAction)
                }
            } else {
                elisa.mediaPlayList.loadPlaylist(fileDialog.file)
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
                    action: showCurrentTrack

                    icon.height: elisaTheme.smallControlButtonSize
                    icon.width: elisaTheme.smallControlButtonSize
                },
                FlatButtonWithToolTip {
                    action: savePlaylist

                    icon.height: elisaTheme.smallControlButtonSize
                    icon.width: elisaTheme.smallControlButtonSize
                },
                FlatButtonWithToolTip {
                    action: loadPlaylist

                    icon.height: elisaTheme.smallControlButtonSize
                    icon.width: elisaTheme.smallControlButtonSize
                },
                FlatButtonWithToolTip {
                    action: clearPlayList

                    icon.height: elisaTheme.smallControlButtonSize
                    icon.width: elisaTheme.smallControlButtonSize
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
                visible: elisa.mediaPlayList ? elisa.mediaPlayList.tracksCount === 0 : true
                Layout.preferredHeight: (emptyPlaylistText.height-emptyImage.height-emptyLabel0.height-emptyLabel1.height)/2
            }

            Image {
                id: emptyImage
                visible: emptyVisible.visible
                Layout.alignment: Qt.AlignHCenter

                width: elisaTheme.gridDelegateWidth * 5
                height: elisaTheme.gridDelegateWidth * 5

                source: elisaTheme.playlistIcon
                opacity: 0.25

                sourceSize {
                    width: elisaTheme.viewSelectorDelegateHeight * 5
                    height: elisaTheme.viewSelectorDelegateHeight * 5
                }
            }

            LabelWithToolTip {
                id: emptyLabel0
                visible: emptyVisible.visible
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.rightMargin: elisaTheme.layoutHorizontalMargin
                Layout.leftMargin: elisaTheme.layoutHorizontalMargin

                level: 1
                wrapMode: Text.WordWrap

                horizontalAlignment: Text.AlignHCenter
                text: i18nc("Your playlist is empty", "Your playlist is empty")
            }

            Label {
                id: emptyLabel1
                visible: emptyVisible.visible
                Layout.topMargin: 5
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.rightMargin: elisaTheme.layoutHorizontalMargin
                Layout.leftMargin: elisaTheme.layoutHorizontalMargin

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
                playListModel: elisa.mediaPlayList

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
                Layout.topMargin: 5
                Layout.fillWidth: true
                Layout.rightMargin: elisaTheme.layoutHorizontalMargin
                Layout.leftMargin: elisaTheme.layoutHorizontalMargin

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

                    text: i18np("1 track", "%1 tracks", (elisa.mediaPlayList ? elisa.mediaPlayList.tracksCount : 0))
                    elide: Text.ElideLeft
                }
            ]
        }
    }
}


