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
    id: viewHeader

    property var viewType
    property alias mainTitle: gridView.mainTitle
    property alias secondaryTitle: gridView.secondaryTitle
    property alias image: gridView.image
    property var modelType
    property alias defaultIcon: gridView.defaultIcon
    property alias showRating: gridView.showRating
    property alias delegateDisplaySecondaryText: gridView.delegateDisplaySecondaryText
    property alias isSubPage: gridView.isSubPage
    property string genreFilterText
    property string artistFilter

    focus: true

    DataModel {
        id: realModel
    }

    GridViewProxyModel {
        id: proxyModel

        sourceModel: realModel
        dataType: modelType

        onEntriesToEnqueue: elisa.mediaPlayList.enqueue(newEntries, databaseIdType, enqueueMode, triggerPlay)
    }

    GridBrowserView {
        id: gridView

        focus: true

        anchors.fill: parent

        contentModel: proxyModel

        onEnqueue: elisa.mediaPlayList.enqueue(databaseId, name, modelType,
                                               ElisaUtils.AppendPlayList,
                                               ElisaUtils.DoNotTriggerPlay)

        onReplaceAndPlay: elisa.mediaPlayList.enqueue(databaseId, name, modelType,
                                                      ElisaUtils.ReplacePlayList,
                                                      ElisaUtils.TriggerPlay)

        onOpen: viewManager.openChildView(innerMainTitle, innerSecondaryTitle, innerImage, databaseId, dataType)

        onGoBack: viewManager.goBack()

        Loader {
            anchors.fill: parent

            visible: realModel.isBusy
            active: realModel.isBusy

            sourceComponent: BusyIndicator {
                anchors.fill: parent
            }
        }
    }

    Connections {
        target: elisa

        onMusicManagerChanged: {
            if (genreFilterText && artistFilter) {
                realModel.initializeByGenreAndArtist(elisa.musicManager, modelType, genreFilterText, artistFilter)
            } else if (genreFilterText) {
                realModel.initializeByGenre(elisa.musicManager, modelType, genreFilterText)
            } else if (artistFilter) {
                realModel.initializeByArtist(elisa.musicManager, modelType, artistFilter)
            } else {
                realModel.initialize(elisa.musicManager, modelType)
            }
        }
    }

    Component.onCompleted: {
        if (elisa.musicManager) {
            if (genreFilterText && artistFilter) {
                realModel.initializeByGenreAndArtist(elisa.musicManager, modelType, genreFilterText, artistFilter)
            } else if (genreFilterText) {
                realModel.initializeByGenre(elisa.musicManager, modelType, genreFilterText)
            } else if (artistFilter) {
                realModel.initializeByArtist(elisa.musicManager, modelType, artistFilter)
            } else {
                realModel.initialize(elisa.musicManager, modelType)
            }
        }
    }
}
