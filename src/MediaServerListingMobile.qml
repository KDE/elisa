import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import org.kde.kirigami 1.0 as MobileComponents
import QtGraphicalEffects 1.0
import QtMultimedia 5.4

MobileComponents.Page {
    property var contentModel
    property MediaPlayList playListModel
    property MusicStatistics musicDatabase
    property string serverName

    id: rootElement

    ColumnLayout {
        anchors.fill: parent

        MobileComponents.Heading {
            Layout.bottomMargin: MobileComponents.Units.largeSpacing
            text: serverName
            Layout.fillWidth: true
        }

        AlbumFilterProxyModel {
            id: filterProxyModel

            sourceModel: rootElement.contentModel

            filterText: filterTextInput.text
        }

        RowLayout {
            Layout.fillWidth: true

            MobileComponents.Label {
                text: 'Filter: '
            }

            TextField {
                id: filterTextInput

                placeholderText: 'Filter'

                Layout.fillWidth: true
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            GridView {
                anchors.fill: parent
                id: contentDirectoryView
                snapMode: GridView.SnapToRow

                cellWidth: width / 3
                cellHeight: cellWidth * 1.9

                model:
                    DelegateModel {
                    id: delegateContentModel
                    model: filterProxyModel

                    delegate: Item {
                        id: mediaServerEntry

                        width: contentDirectoryView.cellWidth
                        height: contentDirectoryView.cellHeight

                        ColumnLayout {
                            id: mainData

                            anchors.fill: parent

                            Item {
                                Layout.preferredHeight: Screen.pixelDensity * 0.5
                                Layout.minimumHeight: Screen.pixelDensity * 0.5
                                Layout.maximumHeight: Screen.pixelDensity * 0.5
                            }

                            Image {
                                id: playIcon
                                source: image
                                width: parent.height * 0.7
                                height: parent.height * 0.7
                                sourceSize.width: width
                                sourceSize.height: width
                                fillMode: Image.PreserveAspectFit

                                Layout.preferredWidth: contentDirectoryView.cellWidth * 0.9
                                Layout.preferredHeight: contentDirectoryView.cellWidth * 0.9

                                Layout.alignment: Qt.AlignHCenter

                                Rectangle {
                                    id: hoverLayer

                                    anchors.fill: parent

                                    color: 'black'
                                    opacity: 0.7
                                    visible: false

                                    BrightnessContrast {
                                        anchors.fill: playAction
                                        source: playAction
                                        brightness: 1.0
                                        contrast: 1.0

                                        MouseArea {
                                            id: clickHandle

                                            anchors.fill: parent
                                            acceptedButtons: Qt.LeftButton

                                            onClicked: {
                                                stackView.push(Qt.resolvedUrl("MediaAlbumView.qml"),
                                                               {
                                                                   'rootIndex': filterProxyModel.mapToSource(delegateContentModel.modelIndex(mediaServerEntry.DelegateModel.itemsIndex)),
                                                                   'contentModel': contentModel,
                                                                   'playListModel': playListModel,
                                                                   'albumArtUrl': image,
                                                                   'albumName': title,
                                                                   'artistName': artist,
                                                                   'tracksCount': count
                                                               })
                                            }
                                        }
                                    }

                                    Image {
                                        id: playAction
                                        source: 'image://icon/document-open-folder'

                                        anchors.centerIn: parent

                                        opacity: 1
                                        visible: false

                                        width: Screen.pixelDensity * 10
                                        height: Screen.pixelDensity * 10
                                        sourceSize.width: width
                                        sourceSize.height: width
                                        fillMode: Image.PreserveAspectFit
                                    }
                                }

                                MouseArea {
                                    id: hoverHandle

                                    hoverEnabled: true

                                    anchors.fill: parent
                                    propagateComposedEvents: true

                                    onEntered: hoverLayer.visible = true
                                    onExited: hoverLayer.visible = false
                                }
                            }

                            MobileComponents.Label {
                                id: titleLabel
                                text: if (title != undefined)
                                          title
                                      else
                                          ''
                                font.weight: Font.Bold
                                horizontalAlignment: Text.AlignLeft

                                Layout.preferredWidth: contentDirectoryView.cellWidth * 0.9
                                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                                elide: "ElideRight"
                            }

                            MobileComponents.Label {
                                id: artistLabel
                                text: if (artist != undefined)
                                          artist
                                      else
                                          ''
                                font.weight: Font.Normal
                                horizontalAlignment: Text.AlignLeft

                                Layout.preferredWidth: contentDirectoryView.cellWidth * 0.9
                                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                                elide: "ElideRight"
                            }

                            MobileComponents.Label {
                                id: numberLabel
                                text: if (count != undefined)
                                          if (count == 1)
                                              count + ' Song'
                                          else
                                              count + ' Songs'
                                      else
                                          '0 Songs'

                                font.weight: Font.Light
                                horizontalAlignment: Text.AlignLeft

                                Layout.preferredWidth: contentDirectoryView.cellWidth * 0.9
                                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                                elide: "ElideRight"
                            }

                            Item {
                                Layout.preferredHeight: Screen.pixelDensity * 0.5
                                Layout.minimumHeight: Screen.pixelDensity * 0.5
                                Layout.maximumHeight: Screen.pixelDensity * 0.5
                            }
                        }
                    }
                }

                focus: true
            }
        }
    }
}
