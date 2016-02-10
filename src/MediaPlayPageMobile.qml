import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQml.Models 2.1
import QtMultimedia 5.4
import org.kde.plasma.mobilecomponents 0.2 as MobileComponents
import org.mgallien.QmlExtension 1.0

MobileComponents.Page {
    property var targetPlayListControler
    property var targetAudioPlayer
    property var playListModel

    id: rootItem

    objectName: "ServerList"
    color: MobileComponents.Theme.viewBackgroundColor

    ColumnLayout {
        anchors.fill: parent

        MobileComponents.Heading {
            Layout.bottomMargin: MobileComponents.Units.largeSpacing
            text: "Local Playlist"
            Layout.fillWidth: true
        }

        HeaderBar {
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

        MediaPlayerControl {
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

                    delegate: AudioTrackDelegate {
                        height: Screen.pixelDensity * 15.
                        width: playListView.width
                        title: if (model != undefined && model.title !== undefined)
                                   model.title
                               else
                                   ''
                        artist: if (model != undefined && model.artist !== undefined)
                                    model.artist
                                else
                                    ''
                        itemDecoration: if (model != undefined && model.image !== undefined)
                                            model.image
                                        else
                                            ''
                        duration: if (model != undefined && model.duration !== undefined)
                                      model.duration
                                  else
                                      ''
                        trackRating: if (model != undefined && model.rating !== undefined)
                                         model.rating
                                     else
                                         ''

                        isPlaying: model.isPlaying
                        showHoverButtons: false
                        itemIndex: index

                        onRemove: playListModel.removeRows(indexToRemove, 1, playListView.model.rootIndex)

                        MouseArea {
                            anchors.fill: parent
                            propagateComposedEvents: true
                            hoverEnabled: true
                            acceptedButtons: Qt.NoButton

                            onEntered: parent.showHoverButtons = true
                            onExited: parent.showHoverButtons = false
                        }
                    }
                }

                focus: true

                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}
