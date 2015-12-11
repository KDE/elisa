import QtQuick 2.3
import QtQuick.Controls 1.2
import QtMultimedia 5.4
import org.mgallien.QmlExtension 1.0
import org.kde.plasma.mobilecomponents 0.2 as MobileComponents

MobileComponents.ApplicationWindow {
    visible: true
    title: qsTr("UPnP Player")
    id: rootItem

    globalDrawer: MobileComponents.GlobalDrawer {
        title: "UPnP Player"
    }

    contextDrawer: MobileComponents.ContextDrawer {
        id: contextDrawer
        actions: rootItem.pageStack.currentPage ? rootItem.pageStack.currentPage.contextualActions : null
        title: "Actions"
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

        //muted: playControlItem.muted

        //volume: playControlItem.volume
        source: playListControler.playerSource

        onPlaying: playListControler.playerPlaying()
        onPaused: playListControler.playerPaused()
        onStopped: playListControler.playerStopped()
        onPositionChanged: playListControler.audioPlayerPositionChanged(position)
        onStatusChanged: playListControler.audioPlayerFinished(status == Audio.EndOfMedia)
        onErrorChanged: console.log(errorString)
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

    property Item stackView: pageStack
    initialPage: ListView {
        id: serverView
        model: serverListModel

        delegate: MobileComponents.ListItem {
            height: 120
            enabled: true

            MobileComponents.Label {
                id: nameLabel
                anchors.fill: parent
                width: serverView.width
                text: model.name
            }


            onClicked: {
                console.log("have been clicked " + index)
                contentDirectory.remoteMediaServer = serverListModel.remoteServer(index)
                rootItem.stackView.push(contentDirectory)
            }
        }
    }

    MediaContentDirectory {
        id: contentDirectory
        parentStackView: rootItem.stackView
        playListModel: playListModelItem
        visible: false
    }
}
