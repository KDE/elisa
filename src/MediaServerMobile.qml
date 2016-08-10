/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
import QtMultimedia 5.5
import org.mgallien.QmlExtension 1.0
import org.kde.kirigami 1.0 as MobileComponents

MobileComponents.ApplicationWindow {
    visible: true
    title: qsTr("Elisa")
    id: rootItem

    property string globalBrowseFlag: 'BrowseDirectChildren'
    property string globalFilter: '*'
    property string globalSortCriteria: ''

    globalDrawer: MobileComponents.GlobalDrawer {
        title: "UPnP Player"
        titleIcon: Qt.resolvedUrl("new-audio-alarm.svg")
        bannerImageSource: "background.jpg"

        actions: [
            MobileComponents.Action {
                text: "Browse Media Servers"
                onTriggered: {
                    stackView.clear()
                    stackView.push(listServerPage)
                }
            },
            MobileComponents.Action {
                text: "Play List"
                onTriggered: {
                    stackView.push(playListPage)
                }
            }
        ]
    }

    property UpnpDeviceDescription aDevice
    property UpnpControlConnectionManager connectionManager

    UpnpSsdpEngine {
        id: mySsdpEngine

        Component.onCompleted: {
            mySsdpEngine.initialize();
            mySsdpEngine.searchAllUpnpDevice();
        }
    }

    DatabaseInterface {
        id: localAlbumDatabase
    }

    Audio {
        id: audioPlayer

        muted: playControlItem.muted

        volume: playControlItem.volume
        source: playListControler.playerSource

        onPlaying: playListControler.playerPlaying()
        onPaused: playListControler.playerPaused()
        onStopped: playListControler.playerStopped()
        onPositionChanged: playListControler.audioPlayerPositionChanged(position)
        onStatusChanged: playListControler.audioPlayerFinished(status == Audio.EndOfMedia)
    }

    MediaPlayList {
        id: playListModelItem

        onTrackHasBeenAdded: rootItem.showPassiveNotification('Track ' + title + ' has been added', 'short', "", {})
    }

    PlayListControler {
        id: playListControler

        playListModel: playListModelItem

        urlRole: MediaPlayList.ResourceRole
        isPlayingRole: MediaPlayList.IsPlayingRole
        artistRole: MediaPlayList.ArtistRole
        titleRole: MediaPlayList.TitleRole
        albumRole: MediaPlayList.AlbumRole
        imageRole: MediaPlayList.ImageRole

        audioDuration: audioPlayer.duration

        onPlayMusic: audioPlayer.play()
        onPauseMusic: audioPlayer.pause()
        onStopMusic: audioPlayer.stop()
        onAudioPositionChanged: audioPlayer.seek(audioPosition)
    }

    ViewPagesModel {
        id: serverListModel

        deviceId: 'urn:schemas-upnp-org:device:MediaServer:1'

        browseFlag: globalBrowseFlag
        filter: globalFilter
        sortCriteria: globalSortCriteria

        withPlaylist: false
        useLocalIcons: true

        property var mediaServiceComponent
        property var mediaViewComponent
    }

    MediaPlayPageMobile {
        id: playListPage
        targetPlayListControler: playListControler
        targetAudioPlayer: audioPlayer
        playListModel: playListModelItem

        visible: false
    }

    MediaServerPageMobile {
        id: listServerPage
        serverModel: serverListModel
        globalPlayListModel: playListModelItem

        visible: false
    }

    property Item stackView: pageStack
    pageStack.initialPage: listServerPage
}
