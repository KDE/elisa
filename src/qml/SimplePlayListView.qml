/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls 2.2

import org.kde.kirigami 2.5 as Kirigami

import org.kde.elisa

ScrollView {
    id: topItem

    property alias model: playListView.model

    // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    contentItem: ListView {
        id: playListView

        readonly property int nextIndex: {
            const playingIndex = ElisaApplication.mediaPlayListProxyModel.currentTrackRow
            if (playingIndex > 0 && playingIndex < playListView.count - 1) {
                return playingIndex + 1
            } else {
                return playingIndex
            }
        }

        property bool draggingEntry: false

        reuseItems: true
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

            required property var model

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
                }
            }

            PlayListEntry {
                id: entry

                focus: true
                width: playListView.width

                index: playListDelegate.model.index
                simpleMode: true
                listView: playListView
                listDelegate: playListDelegate

                databaseId: playListDelegate.model.databaseId ? playListDelegate.model.databaseId : 0
                entryType: playListDelegate.model.entryType ? playListDelegate.model.entryType : ElisaUtils.Unknown
                title: playListDelegate.model.title ? playListDelegate.model.title : ''
                artist: playListDelegate.model.artist ? playListDelegate.model.artist : ''
                album: playListDelegate.model.album ? playListDelegate.model.album : ''
                albumArtist: playListDelegate.model.albumArtist ? playListDelegate.model.albumArtist : ''
                duration: playListDelegate.model.duration ? playListDelegate.model.duration : ''
                fileName: playListDelegate.model.trackResource ? playListDelegate.model.trackResource : ''
                imageUrl: playListDelegate.model.imageUrl ? playListDelegate.model.imageUrl : ''
                trackNumber: playListDelegate.model.trackNumber ? playListDelegate.model.trackNumber : -1
                discNumber: playListDelegate.model.discNumber ? playListDelegate.model.discNumber : -1
                rating: playListDelegate.model.rating ? playListDelegate.model.rating : 0
                isSingleDiscAlbum: playListDelegate.model.isSingleDiscAlbum !== undefined ? playListDelegate.model.isSingleDiscAlbum : true
                isValid: playListDelegate.model.isValid
                isPlaying: playListDelegate.model.isPlaying
                metadataModifiableRole: playListDelegate.model.metadataModifiableRole ? playListDelegate.model.metadataModifiableRole : false
            }
        }
        /* Disable animations until https://bugreports.qt.io/browse/QTBUG-49868 is fixed
         * See also https://invent.kde.org/multimedia/elisa/-/merge_requests/68 and MediaPlayListView.qml
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
        }*/
    }
}
