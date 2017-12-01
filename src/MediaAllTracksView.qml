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
    property var playerControl
    property var playListModel
    property var tracksModel
    property var stackView
    property var musicListener

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
            labelText: i18nc("Title of the view of all tracks", "Tracks")

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

                focus: true

                ListView {
                    id: contentDirectoryView

                    focus: true

                    model: DelegateModel {
                        id: delegateContentModel

                        model: AlbumFilterProxyModel {
                            sourceModel: rootElement.tracksModel

                            filterText: filterBar.filterText

                            filterRating: filterBar.filterRating
                        }

                        delegate: MediaTracksDelegate {
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

                            trackData: if (model != undefined && model.trackData !== undefined)
                                           model.trackData
                                       else
                                           ''

                            coverImage: if (model != undefined && model.image !== undefined)
                                            model.image
                                        else
                                            ''

                            playList: rootElement.playListModel
                            playerControl: rootElement.playerControl

                            contextMenu: Menu {
                                MenuItem {
                                    action: entry.clearAndEnqueueAction
                                }
                                MenuItem {
                                    action: entry.enqueueAction
                                }
                            }

                            onClicked: contentDirectoryView.currentIndex = index

                            onRightClicked: entry.contextMenu.popup()
                        }
                    }
                }
            }
        }
    }
}
