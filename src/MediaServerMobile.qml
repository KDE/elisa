import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
import QtMultimedia 5.4
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

        /*actions: [
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
        ]*/
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

        muted: false
        volume: 1.0

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

        deviceId: 'urn:schemas-upnp-org:device:MediaServer:1'

        browseFlag: globalBrowseFlag
        filter: globalFilter
        sortCriteria: globalSortCriteria

        withPlaylist: false
        useLocalIcons: true

        property var mediaServiceComponent
        property var mediaViewComponent
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
    pageStack.initialPage: listServerPage
}
