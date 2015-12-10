import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Item {
    property UpnpControlMediaServer mediaServerDevice
    property StackView parentStackView
    property UpnpControlConnectionManager connectionManager
    property MediaPlayerControl playControl
    property MediaPlayList playListModel
    property alias randomPlayChecked: shuffleOption.checked
    property alias repeatPlayChecked: repeatOption.checked

    Rectangle {
        color: "#FFFFFF"
        anchors.fill: parent
    }

    Component {
        id: rowDelegate

        Rectangle {
            height: Screen.pixelDensity * 15.
            color: "#FFFFFF"
            anchors.fill: parent
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.preferredHeight: Screen.pixelDensity * 5.5
            Layout.minimumHeight: Screen.pixelDensity * 5.5
            Layout.maximumHeight: Screen.pixelDensity * 5.5
            Layout.alignment: Qt.AlignVCenter

            Layout.fillWidth: true

            CheckBox {
                id: shuffleOption

                text: 'Shuffle'

                Layout.alignment: Qt.AlignVCenter
            }

            Item {
                Layout.preferredWidth: Screen.pixelDensity * 1.5
                Layout.minimumWidth: Screen.pixelDensity * 1.5
                Layout.maximumWidth: Screen.pixelDensity * 1.5
            }

            CheckBox {
                id: repeatOption

                text: 'Repeat'

                Layout.alignment: Qt.AlignVCenter
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                id: playListInfo
                text: playListView.rowCount + ' tracks'

                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }
        }

        Rectangle {
            border.width: 1
            border.color: "#DDDDDD"
            color: "#DDDDDD"

            Layout.fillWidth: true
            Layout.preferredHeight: 1
            Layout.minimumHeight: 1
            Layout.maximumHeight: 1
        }

        TableView {
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

            backgroundVisible: false
            headerVisible: false
            frameVisible: false
            focus: true
            rowDelegate: rowDelegate

            TableViewColumn {
                role: "title"
                title: "Title"
            }

            Layout.fillHeight: true
            Layout.fillWidth: true

            Rectangle {
                color: "#FFFFFF"
                anchors.fill: parent
                z: parent.z - 1
            }
        }
    }
}

