import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import org.kde.plasma.mobilecomponents 0.2 as MobileComponents
import QtMultimedia 5.4

MobileComponents.Page {
    property UpnpControlContentDirectory contentDirectoryService
    property var rootIndex
    property UpnpContentDirectoryModel contentModel
    property MediaPlayList playListModel

    id: rootElement

    color: MobileComponents.Theme.viewBackgroundColor

    ColumnLayout {
        anchors.fill: parent

        MobileComponents.Heading {
            Layout.bottomMargin: MobileComponents.Units.largeSpacing
            text: "UPnP Content"
            Layout.fillWidth: true
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            GridView {
                id: contentDirectoryView

                snapMode: GridView.SnapToRow
                boundsBehavior: Flickable.StopAtBounds
                focus: true
                clip: true

                cellWidth: Screen.pixelDensity * 30.
                cellHeight: Screen.pixelDensity * 20.

                Layout.fillHeight: true
                Layout.fillWidth: true

                model: DelegateModel {
                    id: delegateContentModel
                    model: contentModel
                    rootIndex: rootElement.rootIndex

                    delegate: MobileComponents.ListItem {
                        id: mediaServerEntry

                        width: contentDirectoryView.cellWidth
                        height: contentDirectoryView.cellHeight

                        MouseArea {
                            id: clickHandle

                            anchors.fill: parent

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
                                    stackView.push(Qt.resolvedUrl("MediaAlbumView.qml"),
                                                   {
                                                       'contentDirectoryService': contentDirectoryService,
                                                       'rootId': contentModel.objectIdByIndex(delegateContentModel.modelIndex(mediaServerEntry.DelegateModel.itemsIndex)),
                                                       'stackView': stackView,
                                                       'contentModel': contentModel,
                                                       'playListModel': playListModel
                                                   })
                                }
                                else
                                {
                                    stackView.push(Qt.resolvedUrl("MediaServerListing.qml"),
                                                   {
                                                       'contentDirectoryService': contentDirectoryService,
                                                       'rootIndex': delegateContentModel.modelIndex(mediaServerEntry.DelegateModel.itemsIndex),
                                                       'stackView': stackView,
                                                       'contentModel': contentModel,
                                                       'playListModel': playListModel
                                                   })
                                }
                            }
                        }

                        ColumnLayout {
                            anchors.fill: parent

                            Image {
                                id: playIcon
                                source: image
                                sourceSize.width: Layout.preferredWidth
                                sourceSize.height: Layout.preferredWidth
                                fillMode: Image.PreserveAspectFit

                                Layout.fillHeight: true
                                Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                            }

                            Label {
                                id: mainLabel
                                text: if (title != undefined)
                                          title
                                      else
                                          ''
                                elide: "ElideRight"
                                horizontalAlignment: Text.AlignHCenter

                                Layout.fillWidth: true
                                Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                            }
                        }
                    }
                }
            }
        }
    }
}
