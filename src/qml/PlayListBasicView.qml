/*
 * Copyright 2016-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.10
import QtQuick.Controls 2.2
import QtQml.Models 2.1

import org.kde.elisa 1.0

ListView {
    id: playListView

    property alias playListModel: playListModelDelegate.model
    property string title

    signal startPlayback()
    signal pausePlayback()
    signal displayError(var errorText)

    focus: true
    keyNavigationEnabled: true
    activeFocusOnTab: true

    currentIndex: -1

    Accessible.role: Accessible.List
    Accessible.name: title

    section.property: 'albumSection'
    section.criteria: ViewSection.FullString
    section.labelPositioning: ViewSection.InlineLabels
    section.delegate: PlayListAlbumHeader {
        headerData: JSON.parse(section)
        width: scrollBar.visible ? (!LayoutMirroring.enabled ? playListView.width - scrollBar.width : playListView.width) : playListView.width
        height: elisaTheme.playListHeaderHeight
    }

    ScrollBar.vertical: ScrollBar {
        id: scrollBar
    }
    boundsBehavior: Flickable.StopAtBounds
    clip: true

    ScrollHelper {
        id: scrollHelper
        flickable: playListView
        anchors.fill: playListView
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
                isSelected: playListView.currentIndex === index
                containsMouse: item.containsMouse

                databaseId: model.databaseId ? model.databaseId : 0
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

                onStartPlayback: playListView.startPlayback()
                onPausePlayback: playListView.pausePlayback()
                onRemoveFromPlaylist: playListView.playListModel.removeRows(trackIndex, 1)
                onSwitchToTrack: playListView.playListModel.switchTo(trackIndex)

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
                    playListView.playListModel.switchTo(model.index)
                    playListView.startPlayback()
                }
            }

            onMoveItemRequested: {
                playListModel.move(from, to, 1);
            }
        }
    }

    onCountChanged: if (count === 0) {
                        currentIndex = -1;
                    }
}
