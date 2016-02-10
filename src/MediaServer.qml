import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

ApplicationWindow {
    visible: true
    minimumWidth: 800
    minimumHeight: 400
    title: 'Music'
    id: mainWindow

    property UpnpDeviceDescription aDevice
    property UpnpControlConnectionManager connectionManager

    UpnpSsdpEngine {
        id: mySsdpEngine

        Component.onCompleted: {
            mySsdpEngine.initialize();
            mySsdpEngine.searchAllUpnpDevice();
        }

        onNewService: viewModeModel.newDevice(serviceDiscovery)

        onRemovedService: viewModeModel.removedDevice(serviceDiscovery)
    }

    ViewPagesModel {
        id: viewModeModel
        property var mediaServiceComponent
        property var mediaViewComponent

        onRowsInserted: {
            mainContentView.insertTab(first, '', Qt.createComponent(Qt.resolvedUrl('MediaContentDirectory.qml')))
            mainContentView.getTab(first).active = true
            mainContentView.getTab(first).item.pagesModel = viewModeModel
            mainContentView.getTab(first).item.playListModel = playListModelItem
            mainContentView.getTab(first).item.width = mainContentView.width
            mainContentView.getTab(first).item.height = mainContentView.height
            mainContentView.getTab(first).item.z = 0
            mainContentView.getTab(first).item.remoteMediaServer = viewModeModel.remoteServer(first)
        }
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

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        HeaderBar {
            id: headerBar

            Layout.preferredHeight: Screen.pixelDensity * 28.
            Layout.minimumHeight: Layout.preferredHeight
            Layout.maximumHeight: Layout.preferredHeight
            Layout.fillWidth: true

            tracksCount: playListControler.remainingTracks
            album: playListControler.album
            title: playListControler.title
            artist: playListControler.artist
            image: playListControler.image
            ratingVisible: false
        }

        MediaPlayerControl {
            id: playControlItem

            duration: audioPlayer.duration
            seekable: audioPlayer.seekable

            volume: 1.0
            position: playListControler.playControlPosition
            skipBackwardEnabled: playListControler.skipBackwardControlEnabled
            skipForwardEnabled: playListControler.skipForwardControlEnabled
            playEnabled: playListControler.playControlEnabled
            isPlaying: playListControler.musicPlaying

            Layout.preferredHeight: Screen.pixelDensity * 16.
            Layout.minimumHeight: Layout.preferredHeight
            Layout.maximumHeight: Layout.preferredHeight
            Layout.fillWidth: true

            onSeek: playListControler.playerSeek(position)
            onPlay: playListControler.playPause()
            onPause: playListControler.playPause()
            onPlayPrevious: playListControler.skipPreviousTrack()
            onPlayNext: playListControler.skipNextTrack()
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 0

            TableView {
                id: viewModeView

                Layout.fillHeight: true
                Layout.preferredWidth: Screen.pixelDensity * 50.
                Layout.minimumWidth: Screen.pixelDensity * 50.
                Layout.maximumWidth: Screen.pixelDensity * 50.

                headerVisible: false
                frameVisible: false
                focus: true
                backgroundVisible: false
                z: 2

                rowDelegate: Rectangle {
                    color: '#EFF0F1'

                    height: Screen.pixelDensity * 8.
                    width: viewModeView.width
                }

                model: viewModeModel

                itemDelegate: Rectangle {
                    height: Screen.pixelDensity * 8.
                    width: viewModeView.width
                    color: if (styleData.selected)
                               '#3DAEE9'
                           else
                               '#EFF0F1'
                    Label {
                        id: nameLabel
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: Screen.pixelDensity * 2.
                        anchors.rightMargin: Screen.pixelDensity * 2.
                        verticalAlignment: "AlignVCenter"

                        text: model.name

                        color: if (styleData.selected)
                                   'white'
                               else
                                   'black'

                    }
                }

                TableViewColumn {
                    role: 'name'
                    width: viewModeView.width
                }

                onCurrentRowChanged:
                {
                    mainContentView.currentIndex = currentRow
                }

                onRowCountChanged:
                {
                    viewModeView.selection.clear()
                    viewModeView.currentRow = 0
                    viewModeView.selection.select(0)
                }
            }

            TabView {
                id: mainContentView

                tabsVisible: false

                Layout.fillHeight: true
                Layout.fillWidth: true
                z: 1

                Rectangle {
                    anchors.fill: parent
                    color: "#FFFFFF"
                }

                Tab {
                    Rectangle {
                        anchors.fill: parent
                        color: "#FFFFFF"
                    }

                    MediaPlayListView {
                        id: playList

                        connectionManager: mainWindow.connectionManager
                        playControl: playControlItem
                        playListModel: playListModelItem

                        anchors.fill: parent
                        anchors.margins: 3

                        Component.onCompleted:
                        {
                            playListControler.randomPlay = Qt.binding(function() { return playList.randomPlayChecked })
                            playListControler.repeatPlay = Qt.binding(function() { return playList.repeatPlayChecked })
                        }
                    }
                }
            }
        }
    }
}

