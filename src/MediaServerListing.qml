import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Item {
    property UpnpControlContentDirectory contentDirectoryService
    property var rootIndex
    property StackView stackView
    property UpnpAlbumModel contentModel
    property MediaPlayList playListModel

    id: rootElement

    Rectangle {
        color: 'white'
        width: parent.width
        height: parent.height

        ScrollView {
            anchors.fill: parent
            GridView {
                id: contentDirectoryView
                snapMode: GridView.SnapToRow

                model: DelegateModel {
                    id: delegateContentModel
                    model: contentModel
                    rootIndex: rootElement.rootIndex

                    delegate: Rectangle {
                        id: mediaServerEntry

                        color: 'white'
                        width: contentDirectoryView.cellWidth
                        height: contentDirectoryView.cellHeight

                        MouseArea {
                            id: clickHandle

                            width: parent.width
                            height: parent.height

                            onClicked: {
                                stackView.push(Qt.resolvedUrl("MediaAlbumView.qml"),
                                               {
                                                   'contentDirectoryService': contentDirectoryService,
                                                   'rootIndex': delegateContentModel.modelIndex(mediaServerEntry.DelegateModel.itemsIndex),
                                                   'stackView': stackView,
                                                   'contentModel': contentModel,
                                                   'playListModel': playListModel
                                               })
                            }
                        }

                        Column {
                            width: parent.width
                            height: parent.height

                            Image {
                                id: playIcon
                                source: image
                                width: parent.height * 0.8
                                height: parent.height * 0.8
                                sourceSize.width: width
                                sourceSize.height: width
                                fillMode: Image.PreserveAspectFit
                            }

                            Label {
                                id: mainLabel
                                text: if (title != undefined)
                                          title
                                      else
                                          ''
                                width: parent.width
                                elide: "ElideRight"
                            }
                        }
                    }
                }

                focus: true
            }
        }
    }
}
