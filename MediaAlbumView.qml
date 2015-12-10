import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Item {
    property UpnpControlContentDirectory contentDirectoryService
    property string rootId
    property StackView stackView
    property UpnpContentDirectoryModel contentModel
    property MediaPlayList playListModel

    width: stackView.width
    height: stackView.height

    Component {
        id: rowDelegate

        Item {
            id: rowDelegateContent
            height: Screen.pixelDensity * 15.
            Rectangle {
                color: "#FFFFFF"
                anchors.fill: parent
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TableView {
            id: contentDirectoryView

            model: DelegateModel {
                model: contentModel
                rootIndex: contentModel.indexFromId(rootId)

                delegate: AudioTrackDelegate {
                    height: Screen.pixelDensity * 15.
                    width: contentDirectoryView.width
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
                    isPlaying: if (model != undefined && model.isPlaying !== undefined)
                                   model.isPlaying
                               else
                                   false
                }
            }

            backgroundVisible: false
            headerVisible: false
            frameVisible: false
            focus: true
            rowDelegate: rowDelegate

            onClicked: {
                playListModel.enqueue(model.modelIndex(row))
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
