/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
import QtQuick.Controls 2.2
import QtQuick.Controls 1.3 as Controls1
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
    signal displayError(var errorText)

    id: topItem

    Controls1.Action {
        id: clearPlayList
        text: i18nc("Remove all tracks from play list", "Clear Playlist")
        iconName: 'edit-clear-all'
        enabled: elisa.mediaPlayList ? elisa.mediaPlayList.tracksCount > 0 : false
        onTriggered: elisa.mediaPlayList.clearPlayList()
    }

    Controls1.Action {
        id: showCurrentTrack
        text: i18nc("Show currently played track inside playlist", "Show Current Track")
        iconName: 'media-show-active-track-amarok'
        enabled: elisa.mediaPlayList ? elisa.mediaPlayList.tracksCount > 0 : false
        onTriggered: {
            playListView.positionViewAtIndex(elisa.mediaPlayList.currentTrackRow, ListView.Contain)
            playListView.currentIndex = elisa.mediaPlayList.currentTrackRow
            playListView.currentItem.forceActiveFocus()
        }
    }

    Controls1.Action {
        id: loadPlaylist
        text: i18nc("Load a playlist file", "Load Playlist...")
        iconName: 'document-open'
        onTriggered:
        {
            fileDialog.fileMode = PlatformDialog.FileDialog.OpenFile
            fileDialog.file = ''
            fileDialog.open()
        }
    }

    Controls1.Action {
        id: savePlaylist
        text: i18nc("Save a playlist file", "Save Playlist...")
        iconName: 'document-save'
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
                    displayError(i18nc("message of passive notification when playlist load failed", "Save of playlist failed"))
                }
            } else {
                elisa.mediaPlayList.loadPlaylist(fileDialog.file)
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        LabelWithToolTip {
            id: viewTitleHeight
            text: i18nc("Title of the view of the playlist", "Playlist")

            color: myPalette.text
            font.pointSize: elisaTheme.defaultFontPointSize * 2
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.topMargin: elisaTheme.layoutVerticalMargin * 3
        }


        RowLayout {
            Layout.fillWidth: true

            LabelWithToolTip {
                id: playListInfo

                text: i18np("1 track", "%1 tracks", (elisa.mediaPlayList ? elisa.mediaPlayList.tracksCount : 0))
                visible: elisa.mediaPlayList ? elisa.mediaPlayList.tracksCount > 0 : false
                color: myPalette.text
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            Item { Layout.fillWidth: true }

            Controls1.ToolButton {
                action: showCurrentTrack
                Keys.onReturnPressed: action.trigger()
            }

            Controls1.ToolButton {
                action: savePlaylist
                Keys.onReturnPressed: action.trigger()
            }

            Controls1.ToolButton {
                action: loadPlaylist
                Keys.onReturnPressed: action.trigger()
            }

            Controls1.ToolButton {
                action: clearPlayList
                Keys.onReturnPressed: action.trigger()
            }
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

            Label {
                id: emptyLabel0
                visible: emptyVisible.visible
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.rightMargin: elisaTheme.layoutHorizontalMargin
                Layout.leftMargin: elisaTheme.layoutHorizontalMargin

                font.pointSize: elisaTheme.defaultFontPointSize * 2
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

                playListModel: elisa.mediaPlayList

                focus: true

                onStartPlayback: topItem.startPlayback()

                onPausePlayback: topItem.pausePlayback()

                onDisplayError: topItem.displayError(errorText)

            }

            Kirigami.InlineMessage {

                Connections {
                     target: elisa.mediaPlayList
                     onDisplayUndoInline: undoClear.visible = true
                }

                Connections {
                     target: elisa.mediaPlayList
                     onHideUndoInline: undoClear.visible = false
                }

                Timer {
                    id: autoHideUndoTimer

                    interval: 7000

                    onTriggered: undoClear.visible = false
                }

                id: undoClear

                text: i18nc("Playlist cleared", "Playlist cleared")
                type: Kirigami.MessageType.Information
                showCloseButton: true
                Layout.topMargin: 5
                Layout.fillWidth: true
                Layout.rightMargin: elisaTheme.layoutHorizontalMargin
                Layout.leftMargin: elisaTheme.layoutHorizontalMargin

                onVisibleChanged:
                {
                    if (visible) {
                        autoHideUndoTimer.start()
                    } else {
                        autoHideUndoTimer.stop()
                    }
                }

                actions: [
                    Kirigami.Action {
                        text: i18nc("Undo", "Undo")
                        icon.name: "dialog-cancel"
                        onTriggered: elisa.mediaPlayList.undoClearPlayList()
                    }
                ]
            }
        }
    }
}


