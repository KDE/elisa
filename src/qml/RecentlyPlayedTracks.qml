/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import org.kde.elisa 1.0

FocusScope {
    id: localTracks

    property alias mainTitle: listView.mainTitle
    property alias image: listView.image
    property var modelType

    focus: true

    DataModel {
        id: realModel
    }

    AllTracksProxyModel {
        id: proxyModel

        sortRole: DatabaseInterface.LastPlayDate
        sourceModel: realModel

        onTrackToEnqueue: elisa.mediaPlayList.enqueue(newEntries, databaseIdType, enqueueMode, triggerPlay)
    }

    ListBrowserView {
        id: listView

        focus: true

        anchors.fill: parent

        contentModel: proxyModel

        delegate: MediaTrackDelegate {
            id: entry

            width: listView.delegateWidth
            height: elisaTheme.trackDelegateHeight

            focus: true

            databaseId: model.databaseId
            title: model.title
            artist: model.artist
            album: (model.album !== undefined && model.album !== '' ? model.album : '')
            albumArtist: model.albumArtist
            duration: model.duration
            imageUrl: (model.imageUrl !== undefined && model.imageUrl !== '' ? model.imageUrl : '')
            trackNumber: model.trackNumber
            discNumber: model.discNumber
            rating: model.rating
            isFirstTrackOfDisc: false
            isSingleDiscAlbum: model.isSingleDiscAlbum

            onEnqueue: elisa.mediaPlayList.enqueue(databaseId, name, modelType,
                                                   ElisaUtils.AppendPlayList,
                                                   ElisaUtils.DoNotTriggerPlay)

            onReplaceAndPlay: elisa.mediaPlayList.enqueue(databaseId, name, modelType,
                                                          ElisaUtils.ReplacePlayList,
                                                          ElisaUtils.TriggerPlay)

            onClicked: contentDirectoryView.currentIndex = index
        }
    }

    Connections {
        target: elisa

        onMusicManagerChanged: realModel.initializeRecentlyPlayed(elisa.musicManager, modelType)
    }

    Component.onCompleted: {
        if (elisa.musicManager) {
            realModel.initializeRecentlyPlayed(elisa.musicManager, modelType)
        }

        proxyModel.sortModel(Qt.DescendingOrder)
    }
}
