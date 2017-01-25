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
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.2
import QtQml.Models 2.1
import QtGraphicalEffects 1.0
import QtMultimedia 5.4

import org.mgallien.QmlExtension 1.0

Item {
    property var rootIndex
    property StackView stackView
    property MediaPlayList playListModel
    property var playerControl
    property var contentDirectoryModel

    property alias artistName: navBar.artist

    id: rootElement

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
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

            parentStackView: rootElement.stackView
            playList: rootElement.playListModel
            playerControl: rootElement.playerControl

            enqueueAction: Action {
                text: i18nc("Add all tracks from artist to play list", "Enqueue")
                iconName: "media-track-add-amarok"
                onTriggered: rootElement.playListModel.enqueue(rootElement.artistName)
            }

            clearAndEnqueueAction: Action {
                text: i18nc("Clear play list and add all tracks from artist to play list", "Play Now and Replace Play List")
                iconName: "media-playback-start"
                onTriggered: {
                    rootElement.playListModel.clearAndEnqueue(rootElement.artistName)
                    rootElement.playerControl.playPause()
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

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollView {
                anchors.fill: parent
                flickableItem.boundsBehavior: Flickable.StopAtBounds

                GridView {
                    id: contentDirectoryView

                    cellWidth: Screen.pixelDensity * 40.
                    cellHeight: Screen.pixelDensity * 60.

                    model: DelegateModel {
                        id: delegateContentModel

                        model: SortFilterProxyModel {
                            sourceModel: contentDirectoryModel
                            filterRole: AllAlbumsModel.AllArtistsRole

                            filterRegExp: new RegExp('.*' + artistName + '.*', 'i')
                        }

                        delegate: MediaAlbumDelegate {
                            width: contentDirectoryView.cellWidth
                            height: contentDirectoryView.cellHeight

                            image: model.image
                            title: if (model.title)
                                       model.title
                                   else
                                       ""
                            artist: if (model.artist)
                                        model.artist
                                    else
                                        ""
                            trackNumber: if (model.count !== undefined)
                                             i18np("1 track", "%1 tracks", model.count)
                                         else
                                             i18nc("Number of tracks for an album", "0 track")

                            isSingleDiscAlbum: model.isSingleDiscAlbum

                            albumData: model.albumData

                            stackView: rootElement.stackView

                            playListModel: rootElement.playListModel
                            playerControl: rootElement.playerControl
                        }
                    }

                    focus: true
                }
            }
        }
    }
}
