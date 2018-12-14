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

MediaBrowser {
    id: allAlbums

    focus: true

    anchors {
        fill: parent

        leftMargin: elisaTheme.layoutHorizontalMargin
        rightMargin: elisaTheme.layoutHorizontalMargin
    }

    Loader {
        id: modelLoaderIndicator

        anchors.fill: parent

        anchors.leftMargin: parent.width / 3
        anchors.rightMargin: parent.width / 3
        anchors.topMargin: parent.height / 3
        anchors.bottomMargin: parent.height / 3

        z: 2

        sourceComponent: BusyIndicator {
            id: busyScanningMusic
            hoverEnabled: false

            anchors.fill: parent

            opacity: 0.8

            visible: true
            running: true

            z: 2
        }

        active: false
    }

    AllAlbumsModel {
        id: realModel
    }

    AllAlbumsProxyModel {
        id: proxyModel

        sourceModel: realModel

        onAlbumToEnqueue: elisa.mediaPlayList.enqueue(newEntries, databaseIdType, enqueueMode, triggerPlay)
    }

    firstPage: GridBrowserView {
        id: allAlbumsView

        focus: true

        defaultIcon: elisaTheme.albumCoverIcon

        contentModel: proxyModel

        image: elisaTheme.albumIcon
        mainTitle: i18nc("Title of the view of all albums", "Albums")

        onEnqueue: elisa.mediaPlayList.enqueue(databaseId, name, ElisaUtils.Album,
                                               ElisaUtils.AppendPlayList,
                                               ElisaUtils.DoNotTriggerPlay)

        onReplaceAndPlay: elisa.mediaPlayList.enqueue(databaseId, name, ElisaUtils.Album,
                                                      ElisaUtils.ReplacePlayList,
                                                      ElisaUtils.TriggerPlay)

        onOpen: viewManager.openOneAlbum(allAlbums.stackView, innerMainTitle,
                                         innerSecondaryTitle, innerImage, databaseId)

        onGoBack: viewManager.goBack()
    }

    Connections {
        target: elisa

        onMusicManagerChanged: realModel.initialize(elisa.musicManager)
    }

    Component.onCompleted: {
        if (elisa.musicManager) {
            realModel.initialize(elisa.musicManager)
        }
    }
}
