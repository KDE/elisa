import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQml.Models 2.1
import QtMultimedia 5.5
import org.kde.kirigami 1.0 as MobileComponents
import org.mgallien.QmlExtension 1.0

MobileComponents.Page {
    property var targetPlayListControler
    property var targetAudioPlayer
    property var playListModel

    id: rootItem
    title: "Local Playlist"

    objectName: "ServerList"

    ColumnLayout {
        anchors.fill: parent

        HeaderBarMobile {
            id: headerBar

            Layout.preferredHeight: Screen.pixelDensity * 28.
            Layout.minimumHeight: Layout.preferredHeight
            Layout.maximumHeight: Layout.preferredHeight
            Layout.fillWidth: true

            tracksCount: targetPlayListControler.remainingTracks
            album: targetPlayListControler.album
            title: targetPlayListControler.title
            artist: targetPlayListControler.artist
            image: targetPlayListControler.image
            ratingVisible: false
        }

        MediaPlayerControlMobile {
            id: playControlItem

            duration: targetAudioPlayer.duration
            seekable: targetAudioPlayer.seekable

            volume: 1.0
            position: targetPlayListControler.playControlPosition
            skipBackwardEnabled: targetPlayListControler.skipBackwardControlEnabled
            skipForwardEnabled: targetPlayListControler.skipForwardControlEnabled
            playEnabled: targetPlayListControler.playControlEnabled
            isPlaying: targetPlayListControler.musicPlaying

            Layout.preferredHeight: Screen.pixelDensity * 16.
            Layout.minimumHeight: Layout.preferredHeight
            Layout.maximumHeight: Layout.preferredHeight
            Layout.fillWidth: true

            onSeek: targetPlayListControler.playerSeek(position)
            onPlay: targetPlayListControler.playPause()
            onPause: targetPlayListControler.playPause()
            onPlayPrevious: targetPlayListControler.skipPreviousTrack()
            onPlayNext: targetPlayListControler.skipNextTrack()
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ListView {
                id: playListView

                model: DelegateModel {
                    model: playListModel

                    delegate: AudioTrackDelegateMobile {
                        height: Screen.pixelDensity * 12.
                        width: playListView.width

                        title: (model != undefined && model.title !== undefined) ? model.title : ''
                        artist: (model != undefined && model.artist !== undefined) ? model.artist : ''
                        album: (model != undefined && model.album !== undefined) ? model.album : ''
                        itemDecoration: (model != undefined && model.image !== undefined) ? model.image : ''
                        duration: (model != undefined && model.duration !== undefined) ? model.duration : ''

                        actions: [
                            MobileComponents.Action {
                                text: "Remove"
                                iconName: "list-remove"
                                onTriggered: playListModel.removeRows(index, 1, playListView.model.rootIndex)
                            },
                            MobileComponents.Action {
                                text: "Play Now"
                                iconName: "media-playback-start"
                                onTriggered: targetPlayListControler.skipToTrack(index)
                            }
                        ]
                    }
                }

                focus: true

                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}
