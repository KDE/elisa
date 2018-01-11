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
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.2
import QtQml.Models 2.1
import org.kde.elisa 1.0
import QtQuick.Layouts 1.2

FocusScope {
    id: topListing

    property var musicListener
    property var albumName
    property var artistName
    property var albumArtUrl
    property bool isSingleDiscAlbum
    property var albumId
    property alias albumModel: contentDirectoryView.model

    signal showArtist(var name)
    signal enqueueAlbum(var album)
    signal clearPlayList()
    signal enqueueTrack(var track)
    signal ensurePlay()
    signal goBack();

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    Connections {
        target: musicListener

        onAlbumRemoved:
        {
            if (albumId === removedAlbumId) {
                contentModel.albumRemoved(removedAlbum)
            }
        }
    }

    Connections {
        target: musicListener

        onAlbumModified:
        {
            if (albumId === modifiedAlbumId) {
                albumModel.albumData = modifiedAlbum
                contentModel.albumModified(modifiedAlbum)
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        NavigationActionBar {
            id: navBar

            height: elisaTheme.navigationBarHeight

            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height
            Layout.fillWidth: true

            mainTitle: (topListing.albumModel ? topListing.albumModel.author : '')
            secondaryTitle: topListing.albumName
            image: (topListing.albumArtUrl ? topListing.albumArtUrl : elisaTheme.defaultAlbumImage)
            allowArtistNavigation: true

            onEnqueue: topListing.enqueueAlbum(albumModel.albumData)

            onEnqueueAndPlay: {
                topListing.clearPlayList()
                topListing.enqueueAlbum(albumModel.albumData)
                topListing.ensurePlay()
            }

            onGoBack: topListing.goBack()

            onShowArtist: topListing.showArtist((topListing.albumModel ? topListing.albumModel.author : ''))
        }

        ScrollView {
            flickableItem.boundsBehavior: Flickable.StopAtBounds
            flickableItem.interactive: true

            Layout.fillHeight: true
            Layout.fillWidth: true

            ListView {
                id: contentDirectoryView

                focus: true

                delegate: MediaAlbumTrackDelegate {
                    id: entry

                    height: ((model.isFirstTrackOfDisc && !isSingleDiscAlbum) ? elisaTheme.delegateHeight*2 : elisaTheme.delegateHeight)
                    width: contentDirectoryView.width

                    focus: true

                    mediaTrack.isAlternateColor: (index % 2) === 1

                    mediaTrack.title: if (model != undefined && model.title !== undefined)
                               model.title
                           else
                               ''
                    mediaTrack.artist: if (model != undefined && model.artist !== undefined)
                                model.artist
                            else
                                ''
                    mediaTrack.albumArtist: if (model != undefined && model.albumArtist !== undefined)
                                     model.albumArtist
                                 else
                                     ''
                    mediaTrack.duration: if (model != undefined && model.duration !== undefined)
                                  model.duration
                              else
                                  ''
                    mediaTrack.trackNumber: if (model != undefined && model.trackNumber !== undefined)
                                     model.trackNumber
                                 else
                                     ''
                    mediaTrack.discNumber: if (model != undefined && model.discNumber !== undefined)
                                    model.discNumber
                                else
                                    ''
                    mediaTrack.rating: if (model != undefined && model.rating !== undefined)
                                model.rating
                            else
                                0
                    mediaTrack.trackData: if (model != undefined && model.trackData !== undefined)
                                   model.trackData
                               else
                                   ''
                    mediaTrack.isFirstTrackOfDisc: if (model != undefined && model.isFirstTrackOfDisc !== undefined)
                                            model.isFirstTrackOfDisc
                                        else
                                            false
                    mediaTrack.isSingleDiscAlbum: if (model != undefined && model.isSingleDiscAlbum !== undefined)
                                           model.isSingleDiscAlbum
                                       else
                                           true


                    mediaTrack.onClearPlaylist: topListing.clearPlayList()

                    mediaTrack.onEnqueueToPlaylist: topListing.enqueueTrack(track)

                    mediaTrack.onEnsurePlay: topListing.ensurePlay()

                    mediaTrack.onClicked: contentDirectoryView.currentIndex = index
                }
            }
        }
    }
}
