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

import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import org.kde.elisa 1.0

FocusScope {
    id: rootElement

    property var stackView
    property alias contentModel: contentDirectoryView.model

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

        NavigationActionBar {
            id: navigationBar

            mainTitle: i18nc("Title of the view of all tracks", "Tracks")
            secondaryTitle: ""
            image: elisaTheme.tracksIcon
            enableGoBack: false

            height: elisaTheme.navigationBarHeight
            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height
            Layout.fillWidth: true

            Binding {
                target: contentModel
                property: 'filterText'
                value: navigationBar.filterText
            }

            Binding {
                target: contentModel
                property: 'filterRating'
                value: navigationBar.filterRating
            }

            onEnqueue: contentModel.enqueueToPlayList()

            onReplaceAndPlay: contentModel.replaceAndPlayOfPlayList()
        }

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollView {
                anchors.fill: parent
                flickableItem.boundsBehavior: Flickable.StopAtBounds
                flickableItem.interactive: true

                ListView {
                    id: contentDirectoryView

                    focus: true

                    delegate: MediaTrackDelegate {
                        id: entry

                        width: contentDirectoryView.width
                        height: elisaTheme.trackDelegateHeight

                        focus: true

                        isAlternateColor: (index % 2) === 1

                        title: if (model != undefined && model.title !== undefined)
                                   model.title
                               else
                                   ''
                        artist: if (model != undefined && model.artist !== undefined)
                                    model.artist
                                else
                                    ''
                        albumName: if (model != undefined && model.album !== undefined)
                                       model.album
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
                        rating: if (model != undefined && model.rating !== undefined)
                                    model.rating
                                else
                                    0
                        trackData: if (model != undefined && model.containerData !== undefined)
                                       model.containerData
                                   else
                                       ''
                        coverImage: if (model != undefined && model.image !== undefined)
                                        model.image
                                    else
                                        ''
                        trackResource: if (model != undefined && model.trackResource !== undefined)
                                           model.trackResource
                                       else
                                           ''
                        isFirstTrackOfDisc: if (model != undefined && model.isFirstTrackOfDisc !== undefined)
                                                model.isFirstTrackOfDisc
                                            else
                                                false
                        isSingleDiscAlbum: if (model != undefined && model.isSingleDiscAlbum !== undefined)
                                               model.isSingleDiscAlbum
                                           else
                                               true

                        onEnqueue: elisa.mediaPlayList.enqueue(data)
                        onReplaceAndPlay: elisa.mediaPlayList.replaceAndPlay(data)

                        onClicked: contentDirectoryView.currentIndex = index
                    }
                }
            }
        }
    }
}
