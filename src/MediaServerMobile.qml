import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
import QtMultimedia 5.4
import org.mgallien.QmlExtension 1.0
import org.kde.plasma.mobilecomponents 0.2 as MobileComponents

MobileComponents.ApplicationWindow {
    visible: true
    title: qsTr("UPnP Player")
    id: rootItem

    globalDrawer: MobileComponents.GlobalDrawer {
        title: "UPnP Player"
        titleIcon: "applications-graphics"
        bannerImageSource: "banner.jpg"

        actions: [
            Action {
                text: "Browse Media Servers"
                onTriggered: {
                    stackView.push(listServerPage)
                }
            },
            Action {
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

        onNewService: serverListModel.newDevice(serviceDiscovery)

        onRemovedService: serverListModel.removedDevice(serviceDiscovery)
    }

    Audio {
        id: audioPlayer

        muted: playControlItem.muted
        volume: playControlItem.volume

        source: playListControler.playerSource

        onPlaying: playListControler.playerPlaying()
        onPaused: playListControler.playerPaused()
        onStopped: playListControler.playerStopped()
        onPositionChanged: {
            console.log('Audio: ' + status)
            playListControler.audioPlayerPositionChanged(position)
        }
        onStatusChanged: playListControler.audioPlayerFinished(status == Audio.EndOfMedia)
        onErrorChanged: console.log(errorString)
        onError: console.log('audio player ' + error + ' ' + errorString)
    }

    MediaPlayList {
        id: playListModelItem
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
        property var mediaServiceComponent
        property var mediaViewComponent

        withPlaylist: false
    }

    MediaPlayPage {
        id: playListPage
        targetPlayListControler: playListControler
        targetAudioPlayer: audioPlayer
        playListModel: playListModelItem

        visible: false
    }

    MediaServerPage {
        id: listServerPage
        serverModel: serverListModel
        globalPlayListModel: playListModelItem

        visible: false
    }

    property Item stackView: pageStack
    initialPage: listServerPage
}
