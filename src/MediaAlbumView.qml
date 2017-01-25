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

import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4
import QtQuick.Layouts 1.2

Item {
    id: topListing

    property StackView stackView
    property MediaPlayList playListModel
    property var playerControl
    property var albumName
    property var artistName
    property var tracksCount
    property var albumArtUrl
    property bool isSingleDiscAlbum
    property var albumData

    width: stackView.width
    height: stackView.height

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Component {
        id: rowDelegate

        Item {
            id: rowDelegateContent
            height: Screen.pixelDensity * 15.
            Rectangle {
                color: myPalette.base
                anchors.fill: parent
            }
        }
    }

    AlbumModel {
        id: contentModel

        albumData: topListing.albumData
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        NavigationActionBar {
            id: navBar

            height: Screen.pixelDensity * 25.

            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height
            Layout.fillWidth: true

            parentStackView: topListing.stackView
            playList: topListing.playListModel
            playerControl: topListing.playerControl
            artist: topListing.artistName
            album: topListing.albumName
            image: topListing.albumArtUrl
            tracksCount: topListing.tracksCount

            enqueueAction: Action {
                text: i18nc("Add whole album to play list", "Enqueue")
                iconName: "media-track-add-amarok"
                onTriggered: topListing.playListModel.enqueue(topListing.albumData)
            }

            clearAndEnqueueAction: Action {
                text: i18nc("Clear play list and add whole album to play list", "Play Now and Replace Play List")
                iconName: "media-playback-start"
                onTriggered: {
                    topListing.playListModel.clearAndEnqueue(topListing.albumData)
                    topListing.playerControl.playPause()
                }
            }
        }

        Rectangle {
            border.width: 1
            border.color: myPalette.mid
            color: myPalette.mid

            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

            Layout.fillWidth: parent

            Layout.leftMargin: Screen.pixelDensity * 2.5
            Layout.rightMargin: Screen.pixelDensity * 2.5

            Layout.preferredHeight: 1
            Layout.minimumHeight: 1
            Layout.maximumHeight: 1
        }

        TableView {
            id: contentDirectoryView

            Layout.fillHeight: true
            Layout.fillWidth: true

            flickableItem.boundsBehavior: Flickable.StopAtBounds

            model: DelegateModel {
                model: contentModel

                groups: [
                    DelegateModelGroup { name: "selected" }
                ]

                delegate: AudioTrackDelegate {
                    id: entry

                    isAlternateColor: DelegateModel.itemsIndex % 2
                    height: ((model.isFirstTrackOfDisc && !isSingleDiscAlbum) ? Screen.pixelDensity * 22. : Screen.pixelDensity * 15.)
                    width: contentDirectoryView.width

                    databaseId: model.databaseId
                    playList: topListing.playListModel
                    playerControl: topListing.playerControl

                    title: if (model != undefined && model.title !== undefined)
                               model.title
                           else
                               ''
                    artist: if (model != undefined && model.artist !== undefined)
                                model.artist
                            else
                                ''
                    albumArtist: if (model != undefined && model.albumArtist !== undefined)
                                     model.albumArtist
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
                    discNumber: if (model != undefined && model.discNumber !== undefined)
                                     model.discNumber
                                 else
                                     ''
                    isFirstTrackOfDisc: model.isFirstTrackOfDisc
                    isSingleDiscAlbum: topListing.isSingleDiscAlbum
                    isSelected: DelegateModel.inSelected

                    contextMenu: Menu {
                        MenuItem {
                            action: entry.clearAndEnqueueAction
                        }
                        MenuItem {
                            action: entry.enqueueAction
                        }
                    }

                    onClicked:
                    {
                        var myGroup = contentDirectoryView.model.groups[2]
                        if (myGroup.count > 0 && !DelegateModel.inSelected) {
                            myGroup.remove(0, myGroup.count)
                        }

                        DelegateModel.inSelected = !DelegateModel.inSelected
                    }

                    onRightClicked: contextMenu.popup()
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
        }
    }
}
