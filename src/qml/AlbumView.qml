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
    id: albumView

    property alias mainTitle: albumGridView.mainTitle
    property alias secondaryTitle: albumGridView.secondaryTitle
    property alias image: albumGridView.image

    DataModel {
        id: realModel
    }

    SingleAlbumProxyModel {
        id: proxyModel

        sourceModel: realModel

        onTrackToEnqueue: elisa.mediaPlayList.enqueue(newEntries, databaseIdType, enqueueMode, triggerPlay)
    }

    ListBrowserView {
        id: albumGridView

        anchors.fill: parent

        contentModel: proxyModel

        isSubPage: true

        enableSorting: false

        delegate: MediaAlbumTrackDelegate {
            id: entry

            width: albumGridView.delegateWidth
            height: ((true && !true) ? elisaTheme.delegateHeight*2 : elisaTheme.delegateHeight)

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
            isFirstTrackOfDisc: true
            isSingleDiscAlbum: true
            isAlternateColor: (index % 2) === 1

            mediaTrack.onEnqueue: elisa.mediaPlayList.enqueue(databaseId, name, ElisaUtils.Track,
                                                              ElisaUtils.AppendPlayList,
                                                              ElisaUtils.DoNotTriggerPlay)

            mediaTrack.onReplaceAndPlay: elisa.mediaPlayList.enqueue(databaseId, name, ElisaUtils.Track,
                                                                     ElisaUtils.ReplacePlayList,
                                                                     ElisaUtils.TriggerPlay)


            mediaTrack.onClicked: albumGridView.currentIndex = index
        }

        allowArtistNavigation: true

        onShowArtist: {
            viewManager.openOneArtist(name, elisaTheme.artistIcon, 0)
        }

        onGoBack: viewManager.goBack()
    }

    Connections {
        target: elisa

        onMusicManagerChanged: realModel.initializeByAlbumTitleAndArtist(elisa.musicManager, ElisaUtils.Track, mainTitle, secondaryTitle)
    }

    Component.onCompleted: {
        if (elisa.musicManager) {
            realModel.initializeByAlbumTitleAndArtist(elisa.musicManager, ElisaUtils.Track, mainTitle, secondaryTitle)
        }
    }
}
