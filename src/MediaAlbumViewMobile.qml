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
import org.kde.kirigami 1.0 as MobileComponents
import QtMultimedia 5.5
import QtQuick.Layouts 1.2

MobileComponents.Page {
    id: topListing

    title: albumName

    property var rootIndex
    property var contentModel
    property MediaPlayList playListModel
    property var albumName
    property var artistName
    property var tracksCount
    property var albumArtUrl

    ColumnLayout {
        anchors.fill: parent

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ListView {
                id: contentDirectoryView

                boundsBehavior: Flickable.StopAtBounds
                focus: true
                clip: true

                model: DelegateModel {
                    model: contentModel
                    rootIndex: topListing.rootIndex

                    delegate: AlbumTrackDelegateMobile {
                        title: (model != undefined && model.title !== undefined) ? model.title : ''
                        artist: (model != undefined && model.artist !== undefined) ? model.artist : ''
                        album: (model != undefined && model.album !== undefined) ? model.album : ''
                        itemDecoration: (model != undefined && model.image !== undefined) ? model.image : ''
                        duration: (model != undefined && model.duration !== undefined) ? model.duration : ''

                        onClicked: playListModel.enqueue(contentDirectoryView.model.modelIndex(index))
                    }
                }

                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}
