/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

import QtQuick 2.5
import QtQuick.Controls 2.2
import QtQuick.Controls 1.3 as Controls1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml.Models 2.1
import Qt.labs.platform 1.0 as PlatformDialog
import org.kde.elisa 1.0

FocusScope {
    property StackView parentStackView
    property MediaPlayList playListModel

    property int placeholderHeight: elisaTheme.dragDropPlaceholderHeight

    signal startPlayback()
    signal pausePlayback()
    signal displayError(var errorText)

    id: topItem

    Controls1.Action {
        id: clearPlayList
        text: i18nc("Remove all tracks from play list", "Clear Playlist")
        iconName: "list-remove"
        enabled: playListModelDelegate.items.count > 0
        onTriggered: playListModel.clearPlayList()
    }

    Controls1.Action {
        id: showCurrentTrack
        text: i18nc("Show currently played track inside playlist", "Show Current Track")
        iconName: 'media-show-active-track-amarok'
        enabled: playListModelDelegate.items.count > 0
        onTriggered: {
            playListView.positionViewAtIndex(playListModel.currentTrackRow, ListView.Contain)
            playListView.currentIndex = playListModel.currentTrackRow
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
        enabled: playListModelDelegate.items.count > 0
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
                if (!playListModel.savePlaylist(fileDialog.file)) {
                    displayError(i18nc("message of passive notification when playlist load failed", "Save of playlist failed"))
                }
            } else {
                playListModel.loadPlaylist(fileDialog.file)
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        ColumnLayout {
            height: elisaTheme.navigationBarHeight
            Layout.preferredHeight: elisaTheme.navigationBarHeight
            Layout.minimumHeight: elisaTheme.navigationBarHeight
            Layout.maximumHeight: elisaTheme.navigationBarHeight
            spacing: 0

            Layout.leftMargin:  elisaTheme.layoutHorizontalMargin
            Layout.rightMargin:  elisaTheme.layoutHorizontalMargin

            TextMetrics {
                id: titleHeight
                text: viewTitleHeight.text
                font
                {
                    pointSize: viewTitleHeight.font.pointSize
                    bold: viewTitleHeight.font.bold
                }
            }

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

                    text: i18np("1 track", "%1 tracks", playListModel.tracksCount)
                    color: myPalette.text
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                }

                Item { Layout.fillWidth: true }

                Controls1.ToolButton { action: showCurrentTrack }
                Controls1.ToolButton { action: savePlaylist }
                Controls1.ToolButton { action: loadPlaylist }
                Controls1.ToolButton { action: clearPlayList }
            }

            Item { Layout.fillHeight: true }

        }


        ListView {
            id: playListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: elisaTheme.layoutVerticalMargin

            focus: true

            ScrollBar.vertical: ScrollBar {
                id: scrollBar
            }
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            TextEdit {
                readOnly: true
                visible: playListModelDelegate.count === 0
                wrapMode: TextEdit.Wrap

                color: myPalette.text

                font.weight: Font.ExtraLight
                font.pixelSize: elisaTheme.defaultFontPixelSize * 1.5

                text: i18nc("Text shown when play list is empty", "Your play list is empty.\nIn order to start, you can explore your music library with the views on the left.\nUse the available buttons to add your selection.")
                anchors.fill: parent
                anchors.margins: elisaTheme.layoutHorizontalMargin
            }

            add: Transition {
                NumberAnimation {
                    property: "opacity";
                    from: 0;
                    to: 1;
                    duration: 100 }
            }

            populate: Transition {
                NumberAnimation {
                    property: "opacity";
                    from: 0;
                    to: 1;
                    duration: 100 }
            }

            remove: Transition {
                NumberAnimation {
                    property: "opacity";
                    from: 1.0;
                    to: 0;
                    duration: 100 }
            }

            displaced: Transition {
                NumberAnimation {
                    properties: "x,y";
                    duration: 100;
                    easing.type: Easing.InOutQuad}
            }

            model: DelegateModel {
                id: playListModelDelegate
                model: playListModel

                groups: [
                    DelegateModelGroup { name: "selected" }
                ]

                delegate: DraggableItem {
                    id: item
                    placeholderHeight: topItem.placeholderHeight

                    focus: true

                    PlayListEntry {
                        id: entry

                        focus: true

                        width: scrollBar.visible ? (!LayoutMirroring.enabled ? playListView.width - scrollBar.width : playListView.width) : playListView.width

                        scrollBarWidth: scrollBar.visible ? scrollBar.width : 0

                        index: model.index

                        isAlternateColor: item.DelegateModel.itemsIndex % 2

                        hasAlbumHeader: model.hasAlbumHeader

                        isSingleDiscAlbum: model.isSingleDiscAlbum

                        trackData: model.trackData

                        titleDisplay: model.title

                        albumDisplay: model.album

                        artistDisplay: model.artist

                        albumArtistDisplay: model.albumArtist

                        isValid: model.isValid

                        isPlaying: model.isPlaying

                        isSelected: playListView.currentIndex === index

                        containsMouse: item.containsMouse

                        onStartPlayback: topItem.startPlayback()

                        onPausePlayback: topItem.pausePlayback()

                        onRemoveFromPlaylist: topItem.playListModel.removeRows(trackIndex, 1)

                        onSwitchToTrack: topItem.playListModel.switchTo(trackIndex)
                    }

                    draggedItemParent: topItem

                    onClicked: {
                        playListView.currentIndex = index
                        entry.forceActiveFocus()
                    }

                    onDoubleClicked: {
                        if (model.isValid) {
                            topItem.playListModel.switchTo(model.index)
                            topItem.startPlayback()
                        }
                    }

                    onMoveItemRequested: {
                        playListModel.move(from, to, 1);
                    }
                }
            }
        }


    }
}


