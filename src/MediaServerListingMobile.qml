/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import org.kde.kirigami 1.0 as MobileComponents
import QtGraphicalEffects 1.0
import QtMultimedia 5.5

MobileComponents.Page {
    property var contentModel
    property MediaPlayList playListModel
    property MusicStatistics musicDatabase
    property string serverName

    id: rootElement
    title: serverName

    ColumnLayout {
        anchors.fill: parent

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

                                MouseArea {
                                    id: clickHandle

                                    anchors.fill: parent
                                    propagateComposedEvents: true

                                    onClicked: {
                                        stackView.push(Qt.resolvedUrl("MediaAlbumViewMobile.qml"),
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
