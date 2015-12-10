import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Item {
    property UpnpControlContentDirectory contentDirectoryService
    property string rootId
    property StackView stackView

    width: stackView.width
    height: stackView.height

    Component {
        id: rowDelegate

        Item {
            id: rowDelegateContent
            height: Screen.pixelDensity * 14
            Rectangle {
                color: "#fff"
                anchors.fill: parent
            }
        }
    }


    UpnpContentDirectoryModel {
        id: contentDirectoryModel
        browseFlag: 'BrowseDirectChildren'
        filter: '*'
        sortCriteria: ''
        contentDirectory: contentDirectoryService

        Component.onCompleted: {
            contentDirectoryModel.forceRefresh(rootId)
        }
    }

    Audio {
        id: player
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        MediaPlayerControlMobile {
            id: playControl

            volume: player.volume
            position: player.position
            duration: player.duration
            muted: player.muted
            isPlaying: (player.playbackState == Audio.PlayingState)
            seekable: player.seekable

            Layout.preferredHeight: Screen.pixelDensity * 10
            Layout.fillWidth: true

            onPlay: player.play()
            onPause: player.pause()
            onSeek: player.seek(position)
            onVolumeChanged: player.volume = volume
            onMutedChanged: player.muted = muted
        }

        TableView {
            id: contentDirectoryView

            model: contentDirectoryModel
            headerVisible: false
            frameVisible: false
            focus: true
            rowDelegate: rowDelegate

            itemDelegate: AudioTrackDelegateMobile {
                height: Screen.pixelDensity * 20
                title: if (model != undefined)
                           model.title
                       else
                           ''
                artist: if (model != undefined)
                            model.artist
                        else
                            ''
                itemDecoration: if (model != undefined)
                                    model.image
                                else
                                    ''
                duration: if (model != undefined)
                              model.duration
                          else
                              ''
                trackRating: if (model != undefined)
                                 model.rating
                             else
                                 ''
                isPlaying: true
            }

            TableViewColumn {
                role: "title"
                title: "Title"
            }

            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
