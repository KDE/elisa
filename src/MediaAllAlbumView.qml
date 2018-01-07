/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import org.kde.elisa 1.0

FocusScope {
    property var rootIndex
    property StackView stackView
    property MediaPlayList playListModel
    property var musicListener
    property var playerControl
    property var contentDirectoryModel

    signal showArtist(var name)

    id: rootElement

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        FilterBar {
            id: filterBar
            labelText: i18nc("Title of the view of all albums", "Albums")

            height: elisaTheme.navigationBarHeight
            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height
            Layout.fillWidth: true
        }

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollView {
                anchors.fill: parent
                flickableItem.boundsBehavior: Flickable.StopAtBounds
                flickableItem.interactive: true

                GridView {
                    id: contentDirectoryView

                    focus: true

                    TextMetrics {
                        id: textLineHeight
                        text: 'Album'
                    }

                    cellWidth: elisaTheme.gridDelegateWidth
                    cellHeight: elisaTheme.gridDelegateWidth + elisaTheme.layoutVerticalMargin + textLineHeight.height * 2

                    model: DelegateModel {
                        id: delegateContentModel

                        model: AlbumFilterProxyModel {
                            sourceModel: rootElement.contentDirectoryModel

                            filterText: filterBar.filterText

                            filterRating: filterBar.filterRating
                        }

                        delegate: MediaAlbumDelegate {
                            width: contentDirectoryView.cellWidth
                            height: contentDirectoryView.cellHeight

                            focus: true

                            musicListener: rootElement.musicListener
                            image: if (model.image)
                                       model.image
                                   else
                                       ""
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
                            albumId: model.albumId

                            stackView: rootElement.stackView

                            playListModel: rootElement.playListModel
                            playerControl: rootElement.playerControl

                            onAlbumClicked: contentDirectoryView.currentIndex = index

                            onShowArtist: rootElement.showArtist(name)
                        }
                    }
                }
            }
        }
    }
}
