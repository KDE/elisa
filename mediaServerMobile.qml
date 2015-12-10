import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Item {
    property UpnpDeviceDescription aDevice
    property StackView parentStackView
    property UpnpControlConnectionManager connectionManager

    id: rootElement

    width: parent.width
    height: parent.height

    UpnpControlMediaServer {
        id: upnpDevice

        description: aDevice
    }

    Audio {
        id: audioPlayer

        muted: playControlItem.muted
        volume: if (muted)
                    0
                else
                    playControlItem.volume
    }

    MediaPlayList {
        id: playListModelItem
    }

    PlayListControler {
        id: playListControler

        playListModel: DelegateModel {
            model: playListModelItem
        }
        modelData: playListModelItem

        player: audioPlayer
        playControl: playControlItem
    }

    ListModel {
        id: viewModeModel

        ListElement {
            name: "Content"
        }

        ListElement {
            name: "PlayList"
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        MediaPlayerControlMobile {
            id: playControlItem

            volume: 100
            position: audioPlayer.position
            duration: audioPlayer.duration
            muted: false
            seekable: audioPlayer.seekable
            isPlaying: false

            Layout.preferredHeight: 60
            Layout.minimumHeight: Layout.preferredHeight
            Layout.maximumHeight: Layout.preferredHeight
            Layout.fillWidth: true

            onSeek: audioPlayer.seek(position)
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 0

            TableView {
                id: viewModeView

                Layout.fillHeight: true
                Layout.preferredWidth: 150

                headerVisible: false
                frameVisible: false
                focus: true
                backgroundVisible: false

                rowDelegate: Rectangle {
                    color: '#EFF0F1'

                    height: 50
                    width: viewModeView.width
                }

                model: viewModeModel

                itemDelegate: Rectangle {
                    height: 50
                    width: viewModeView.width
                    color: if (styleData.selected)
                               '#3DAEE9'
                           else
                               '#EFF0F1'
                    Label {
                        anchors.fill: parent
                        width: viewModeView.width
                        text: model.name
                    }
                }

                TableViewColumn {
                    role: 'name'
                    width: viewModeView.width
                }

                Component.onCompleted: {
                    currentRow = 0
                    selection.select(0, 0)
                }
            }

            TabView {
                id: mainContentView

                tabsVisible: false

                currentIndex: viewModeView.currentRow

                Layout.fillHeight: true
                Layout.fillWidth: true

                Tab {
                    MediaContentDirectoryMobile {
                        id: contentBrowser

                        mediaServerDevice: upnpDevice
                        parentStackView: rootElement.parentStackView
                        connectionManager: rootElement.connectionManager
                        playListModel: playListModelItem

                        width: mainContentView.width
                        height: mainContentView.height
                    }
                }

                /*Tab {
                    MediaPlayListView {
                        id: playList

                        mediaServerDevice: upnpDevice
                        parentStackView: rootElement.parentStackView
                        connectionManager: rootElement.connectionManager
                        player: audioPlayer
                        playControl: playControlItem
                        playListModel: playListModelItem

                        width: mainContentView.width
                        height: mainContentView.height
                    }
                }*/
            }
        }
    }
}

