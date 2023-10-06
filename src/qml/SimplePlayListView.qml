/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.10
import QtQuick.Controls 2.2
import QtQml.Models 2.1

import org.kde.kirigami 2.5 as Kirigami

import org.kde.elisa 1.0

ScrollView {
    id: topItem

    property alias model: playListView.model

    // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    contentItem: ListView {
        id: playListView

        readonly property int nextIndex: {
            var playingIndex = ElisaApplication.mediaPlayListProxyModel.currentTrackRow
            if (playingIndex > 0 && playingIndex < playListView.count - 1) {
                return playingIndex + 1
            } else {
                return playingIndex
            }
        }

        property bool draggingEntry: false

        clip: true
        focus: true
        activeFocusOnTab: true
        keyNavigationEnabled: true

        // position the view at the playing index
        Component.onCompleted: currentIndex = nextIndex
        Connections {
            target: ElisaApplication.mediaPlayListProxyModel
            function onCurrentTrackRowChanged() {
                playListView.currentIndex = playListView.nextIndex
            }
        }

        highlightMoveDuration: Kirigami.Units.veryLongDuration
        highlightMoveVelocity: -1
        highlight: Item {}

        section.property: 'albumSection'
        section.criteria: ViewSection.FullString

        delegate: Column {
            id: playListDelegate
            width: playListView.width

            // album seperator
            Item {
                width: playListView.width
                height: Kirigami.Units.smallSpacing
                visible: entry.previousAlbum && entry.previousAlbum !== entry.currentAlbum
            }

            Loader {
                id: albumSection
                active: entry.sectionVisible
                visible: active
                sourceComponent: BasicPlayListAlbumHeader {
                    headerData: JSON.parse(playListDelegate.ListView.section)
                    width: playListView.width
                    simpleMode: true
                    Kirigami.Theme.textColor: "#eff0f1"
                }
            }

            PlayListEntry {
                id: entry

                focus: true
                width: playListView.width

                index: model.index
                simpleMode: true
                listView: playListView
                listDelegate: playListDelegate

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
                metadataModifiableRole: model.metadataModifiableRole ? model.metadataModifiableRole : false
            }
        }

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
                easing.type: Easing.InOutQuad}
        }
    }
}
