import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Item {
    property UpnpControlContentDirectory contentDirectoryService
    property string rootId
    property StackView stackView
    property UpnpContentDirectoryModel contentModel
    property MediaPlayList playListModel

    Rectangle {
        color: 'white'
        width: parent.width
        height: parent.height

        Flickable {
            width: parent.width
            height: parent.height
            GridView {
                id: contentDirectoryView
                snapMode: GridView.SnapToRow
                width: parent.width
                height: parent.height

                model: DelegateModel {
                    id: delegateContentModel
                    model: contentModel
                    rootIndex: contentModel.indexFromId(rootId)

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
                                if (itemClass == UpnpContentDirectoryModel.AudioTrack)
                                {
                                    stackView.push({
                                                       item: Qt.resolvedUrl("mediaPlayer.qml"),
                                                       properties: {
                                                           'audioUrl': '',
                                                           'stackView': stackView,
                                                           'contentModel': contentModel,
                                                           'playListModel': playListModel
                                                       }
                                                   })
                                }
                                else if (itemClass == UpnpContentDirectoryModel.Album)
                                {
                                    stackView.push({
                                                       item: Qt.resolvedUrl("mediaAlbumViewMobile.qml"),
                                                       properties: {
                                                           'contentDirectoryService': contentDirectoryService,
                                                           'rootId': contentModel.objectIdByIndex(delegateContentModel.modelIndex(mediaServerEntry.DelegateModel.itemsIndex)),
                                                           'stackView': stackView,
                                                           'contentModel': contentModel,
                                                           'playListModel': playListModel
                                                       }
                                                   })
                                }
                                else
                                {
                                    stackView.push({
                                                       item: Qt.resolvedUrl("mediaServerListingMobile.qml"),
                                                       properties: {
                                                           'contentDirectoryService': contentDirectoryService,
                                                           'rootId': contentModel.objectIdByIndex(delegateContentModel.modelIndex(mediaServerEntry.DelegateModel.itemsIndex)),
                                                           'stackView': stackView,
                                                           'contentModel': contentModel,
                                                           'playListModel': playListModel
                                                       }
                                                   })
                                }
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
