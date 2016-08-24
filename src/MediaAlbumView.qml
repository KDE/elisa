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
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4
import QtQuick.Layouts 1.2

Item {
    id: topListing

    property StackView stackView
    property var musicDatabase
    property MediaPlayList playListModel
    property var albumName
    property var artistName
    property var tracksCount
    property var albumArtUrl

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

    AlbumModel {
        id: contentModel

        databaseInterface: musicDatabase
        title: albumName
        author: artistName
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Button {
            id: backButton

            height: Screen.pixelDensity * 8.
            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height
            Layout.fillWidth: true

            onClicked: if (listingView.depth > 1) {
                           listingView.pop()
                       } else {
                           parentStackView.pop()
                       }
            text: 'Back'
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            spacing: 0

            ColumnLayout {
                Layout.preferredWidth: topListing.width / 2
                Layout.fillHeight: true

                spacing: 0

                TableView {
                    id: contentDirectoryView

                    model: DelegateModel {
                        model: contentModel

                        delegate: AudioTrackDelegate {
                            height: Screen.pixelDensity * 15.
                            width: contentDirectoryView.width

                            hoverAction: Action {
                                id: queueTrack

                                iconSource: 'image://icon/media-playback-start'

                                onTriggered: topListing.playListModel.enqueue(contentDirectoryView.model.modelIndex(index))
                            }

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
                            trackNumber: if (model != undefined && model.trackNumber !== undefined)
                                             model.trackNumber
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

                    TableViewColumn {
                        role: "title"
                        title: "Title"
                    }

                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
            }

            Rectangle {
                border.width: 1
                border.color: "#DDDDDD"
                color: "#DDDDDD"

                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                Layout.preferredHeight: parent.height - Screen.pixelDensity * 5.
                Layout.preferredWidth: 1
                Layout.minimumWidth: 1
                Layout.maximumWidth: 1
            }

            ContextView {
                id: albumContext

                Layout.preferredWidth: topListing.width / 2
                Layout.fillHeight: true

                albumArtUrl: topListing.albumArtUrl
                albumName: topListing.albumName
                tracksCount: topListing.tracksCount
                artistName: topListing.artistName
            }
        }
    }
}
