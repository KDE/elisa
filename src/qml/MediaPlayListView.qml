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
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml.Models 2.1
import Qt.labs.platform 1.0 as PlatformDialog
import org.kde.elisa 1.0

FocusScope {
    property StackView parentStackView
    property MediaPlayList playListModel
    property var playListControler

    property alias randomPlayChecked: shuffleOption.checked
    property alias repeatPlayChecked: repeatOption.checked

    property int placeholderHeight: elisaTheme.dragDropPlaceholderHeight

    signal startPlayback()
    signal pausePlayback()
    signal displayError(var errorText)

    id: topItem

    Action {
        id: clearPlayList
        text: i18nc("Remove all tracks from play list", "Clear Play List")
        iconName: "list-remove"
        enabled: playListModelDelegate.items.count > 0
        onTriggered: {
            var selectedItems = []
            var myGroup = playListModelDelegate.items
            for (var i = 0; i < myGroup.count; ++i) {
                var myItem = myGroup.get(i)
                selectedItems.push(myItem.itemsIndex)
            }

            playListModel.removeSelection(selectedItems)
        }
    }

    Action {
        id: showCurrentTrack
        text: i18nc("Show currently played track inside playlist", "Show Current Track")
        iconName: 'media-show-active-track-amarok'
        enabled: playListModelDelegate.items.count > 0
        onTriggered: {
            playListView.positionViewAtIndex(playListControler.currentTrackRow, ListView.Contain)
            playListView.currentIndex = playListControler.currentTrackRow
            playListView.currentItem.forceActiveFocus()
        }
    }

    Action {
        id: loadPlaylist
        text: i18nc("Load a playlist file", "Load a Playlist")
        iconName: 'document-open'
        onTriggered:
        {
            fileDialog.fileMode = PlatformDialog.FileDialog.OpenFile
            fileDialog.file = ''
            fileDialog.open()
        }
    }

    Action {
        id: savePlaylist
        text: i18nc("Save a playlist file", "Save a Playlist")
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

        RowLayout {
            height: elisaTheme.navigationBarHeight
            Layout.preferredHeight: elisaTheme.navigationBarHeight
            Layout.minimumHeight: elisaTheme.navigationBarHeight
            Layout.maximumHeight: elisaTheme.navigationBarHeight

            Image {
                id: mainIcon
                source: elisaTheme.playlistIcon

                asynchronous: true

                sourceSize.height: elisaTheme.coverImageSize / 2
                sourceSize.width: elisaTheme.coverImageSize / 2

                fillMode: Image.PreserveAspectFit

                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                Layout.preferredHeight: elisaTheme.coverImageSize / 2
                Layout.minimumHeight: elisaTheme.coverImageSize / 2
                Layout.maximumHeight: elisaTheme.coverImageSize / 2
                Layout.preferredWidth: elisaTheme.coverImageSize / 2
                Layout.minimumWidth: elisaTheme.coverImageSize / 2
                Layout.maximumWidth: elisaTheme.coverImageSize / 2
                Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            }

            ColumnLayout {
                spacing: 0

                Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0

                TextMetrics {
                    id: titleHeight
                    text: viewTitleHeight.text
                    font
                    {
                        pixelSize: viewTitleHeight.font.pixelSize
                        bold: viewTitleHeight.font.bold
                    }
                }

                LabelWithToolTip {
                    id: viewTitleHeight
                    text: i18nc("Title of the view of the playlist", "Now Playing")

                    color: myPalette.text
                    font.pixelSize: elisaTheme.defaultFontPixelSize * 2
                    Layout.topMargin: elisaTheme.layoutVerticalMargin
                  }

                LabelWithToolTip {
                    id: playListInfo

                    text: i18np("1 track", "%1 tracks", playListModel.tracksCount)

                    color: myPalette.text

                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                }

                Item {
                    Layout.fillHeight: true
                }

                RowLayout {
                    Layout.bottomMargin: elisaTheme.layoutVerticalMargin

                    CheckBox {
                        id: shuffleOption

                        text: i18n("Shuffle")
                    }

                    CheckBox {
                        id: repeatOption

                        text: i18n("Repeat")
                    }
                }
            }
        }

        ScrollView {
            flickableItem.boundsBehavior: Flickable.StopAtBounds

            Layout.fillWidth: true
            Layout.fillHeight: true

            focus: true

            ListView {
                id: playListView

                focus: true

                TextEdit {
                    readOnly: true
                    visible: playListModelDelegate.count === 0
                    wrapMode: TextEdit.Wrap
                    renderType: TextEdit.NativeRendering

                    color: myPalette.text

                    font.weight: Font.ExtraLight
                    font.pointSize: 12

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

                            width: playListView.width
                            index: model.index
                            isAlternateColor: item.DelegateModel.itemsIndex % 2
                            hasAlbumHeader: if (model != undefined && model.hasAlbumHeader !== undefined)
                                                model.hasAlbumHeader
                                            else
                                                true
                            title: if (model != undefined && model.title !== undefined)
                                       model.title
                                   else
                                       ''
                            artist: if (model != undefined && model.artist !== undefined)
                                        model.artist
                                    else
                                        ''
                            albumArtist: if (model != undefined && model.albumArtist !== undefined)
                                             model.albumArtist
                                         else
                                             ''
                            itemDecoration: if (model != undefined && model.image !== undefined)
                                                model.image
                                            else
                                                ''
                            duration: if (model != undefined && model.duration !== undefined)
                                          model.duration
                                      else
                                          ''
                            trackNumber: if (model != undefined && model.trackNumber !== undefined)
                                             model.trackNumber
                                         else
                                             ''
                            discNumber: if (model != undefined && model.discNumber !== undefined)
                                            model.discNumber
                                        else
                                            ''
                            isSingleDiscAlbum: if (model != undefined && model.isSingleDiscAlbum !== undefined)
                                                   model.isSingleDiscAlbum
                                               else
                                                   false
                            album: if (model != undefined && model.album !== undefined)
                                       model.album
                                   else
                                       ''
                            rating: if (model != undefined && model.rating !== undefined)
                                        model.rating
                                    else
                                        0
                            trackResource: if (model != undefined && model.trackResource !== undefined)
                                               model.trackResource
                                           else
                                               ''
                            isValid: model.isValid
                            isPlaying: model.isPlaying
                            isSelected: playListView.currentIndex === index
                            containsMouse: item.containsMouse

                            onStartPlayback: topItem.startPlayback()

                            onPausePlayback: topItem.pausePlayback()

                            onRemoveFromPlaylist: topItem.playListModel.removeRows(trackIndex, 1)

                            onSwitchToTrack: topItem.playListControler.switchTo(trackIndex)
                        }

                        draggedItemParent: topItem

                        onClicked: {
                            playListView.currentIndex = index
                            entry.forceActiveFocus()
                        }

                        onDoubleClicked: {
                            if (model.isValid) {
                                topItem.playListControler.switchTo(model.index)
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

        Rectangle {
            id: actionBar

            Layout.fillWidth: true
            Layout.topMargin: elisaTheme.layoutVerticalMargin
            Layout.preferredHeight: elisaTheme.delegateToolButtonSize

            color: myPalette.mid

            RowLayout {
                id: actionBarLayout

                anchors.fill: parent

                ToolButton {
                    action: clearPlayList
                    Layout.bottomMargin: elisaTheme.layoutVerticalMargin
                }
                ToolButton {
                    action: showCurrentTrack
                    Layout.bottomMargin: elisaTheme.layoutVerticalMargin
                }
                ToolButton {
                    action: loadPlaylist
                    Layout.bottomMargin: elisaTheme.layoutVerticalMargin
                }
                ToolButton {
                    action: savePlaylist
                    Layout.bottomMargin: elisaTheme.layoutVerticalMargin
                }
                Item {
                    Layout.fillWidth: true
                }
            }
        }
    }
}

