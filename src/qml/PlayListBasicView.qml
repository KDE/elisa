/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
    section.delegate: BasicPlayListAlbumHeader {
        headerData: JSON.parse(section)
        width: scrollBar.visible ? (!LayoutMirroring.enabled ? playListView.width - scrollBar.width : playListView.width) : playListView.width
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

                onStartPlayback: playListView.startPlayback()
                onPausePlayback: playListView.pausePlayback()
                onRemoveFromPlaylist: playListView.playListModel.removeRow(trackIndex)
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
                playListModel.moveRow(from, to);
            }
        }
    }

    onCountChanged: if (count === 0) {
                        currentIndex = -1;
                    }
}
