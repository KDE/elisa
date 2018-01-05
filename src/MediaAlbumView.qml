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

    property StackView stackView
    property MediaPlayList playListModel
    property var musicListener
    property var playerControl
    property var albumName
    property var artistName
    property var tracksCount
    property var albumArtUrl
    property bool isSingleDiscAlbum
    property var albumData
    property var albumId

    signal showArtist(var name)

    width: stackView.width
    height: stackView.height

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    AlbumModel {
        id: contentModel

        albumData: topListing.albumData
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
                albumData = modifiedAlbum
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

            parentStackView: topListing.stackView
            playList: topListing.playListModel
            playerControl: topListing.playerControl
            artist: topListing.artistName
            album: topListing.albumName
            image: (topListing.albumArtUrl ? topListing.albumArtUrl : elisaTheme.albumCover)
            tracksCount: topListing.tracksCount

            enqueueAction: Action {
                text: i18nc("Add whole album to play list", "Enqueue")
                iconName: "media-track-add-amarok"
                onTriggered: topListing.playListModel.enqueue(topListing.albumData)
            }

            clearAndEnqueueAction: Action {
                text: i18nc("Clear play list and play", "Replace and Play")
                tooltip: i18nc("Clear play list and add whole album to play list", "Replace Play List and Play Now")
                iconName: "media-playback-start"
                onTriggered: {
                    topListing.playListModel.clearAndEnqueue(topListing.albumData)
                    topListing.playerControl.ensurePlay()
                }
            }

            navigateToArtistAction: Action {
                text: i18nc("Button to navigate to the artist of the album", "Display Artist")
                iconName: "view-media-artist"
                onTriggered: {
                    showArtist(topListing.artistName)
                }
            }
        }

        ScrollView {
            flickableItem.boundsBehavior: Flickable.StopAtBounds
            flickableItem.interactive: true

            Layout.fillHeight: true
            Layout.fillWidth: true

            ListView {
                id: contentDirectoryView

                focus: true

                model:  contentModel

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


                    mediaTrack.onClearPlaylist: topListing.playListModel.clearPlayList()

                    mediaTrack.onEnqueueToPlaylist: topListing.playListModel.enqueue(track)

                    mediaTrack.onEnsurePlay: topListing.playerControl.ensurePlay()

                    mediaTrack.onClicked: contentDirectoryView.currentIndex = index
                }
            }
        }
    }
}
