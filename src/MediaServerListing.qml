import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import QtMultimedia 5.4

import org.mgallien.QmlExtension 1.0

Item {
    property var rootIndex
    property StackView stackView
    property var contentModel
    property MediaPlayList playListModel

    id: rootElement

    AlbumFilterProxyModel {
        id: filterProxyModel

        sourceModel: contentModel

        filterText: filterTextInput.text
    }

    DelegateModel {
        id: delegateContentModel
        model: filterProxyModel

        delegate: Rectangle {
            id: mediaServerEntry

            color: 'white'

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
                                                       'stackView': stackView,
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

                Label {
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

                Label {
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

                Label {
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

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: 'Filter: '
            }

            TextField {
                id: filterTextInput

                placeholderText: 'Filter'

                Layout.fillWidth: true
            }
        }

        Rectangle {
            color: 'white'

            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollView {
                anchors.fill: parent
                GridView {
                    id: contentDirectoryView
                    snapMode: GridView.SnapToRow

                    cellWidth: Screen.pixelDensity * 40.
                    cellHeight: Screen.pixelDensity * 60.

                    model: delegateContentModel

                    focus: true
                }
            }
        }
    }
}
