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
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

FocusScope {
    id: viewHeader

    property var viewType
    property var filterType
    property alias isSubPage: listView.isSubPage
    property alias mainTitle: listView.mainTitle
    property alias secondaryTitle: listView.secondaryTitle
    property int databaseId
    property alias showSection: listView.showSection
    property alias expandedFilterView: listView.expandedFilterView
    property alias image: listView.image
    property var modelType
    property alias sortRole: proxyModel.sortRole
    property var sortAscending
    property bool displaySingleAlbum: false
    property alias radioCase: listView.showCreateRadioButton

    function openMetaDataView(databaseId){
        metadataLoader.setSource("MediaTrackMetadataView.qml", {"databaseId": databaseId, "isRadio": viewHeader.radioCase});
        metadataLoader.active = true
    }

    DataModel {
        id: realModel
    }

    AllTracksProxyModel {
        id: proxyModel

        sourceModel: realModel

        onEntriesToEnqueue: elisa.mediaPlayList.enqueue(newEntries, databaseIdType, enqueueMode, triggerPlay)
    }

    Loader {
        id: metadataLoader
        active: false
        onLoaded: item.show()
    }

    Component {
        id: albumDelegate

        ListBrowserDelegate {
            id: entry

            width: listView.delegateWidth
            height: elisaTheme.delegateHeight

            focus: true

            databaseId: model.databaseId
            title: model.title ? model.title : ''
            artist: model.artist ? model.artist : ''
            album: model.album ? model.album : ''
            albumArtist: model.albumArtist ? model.albumArtist : ''
            duration: model.duration ? model.duration : ''
            imageUrl: model.imageUrl ? model.imageUrl : ''
            trackNumber: model.trackNumber ? model.trackNumber : -1
            discNumber: model.discNumber ? model.discNumber : -1
            rating: model.rating
            isSelected: listView.currentIndex === index
            isAlternateColor: (index % 2) === 1
            detailedView: false

            onEnqueue: elisa.mediaPlayList.enqueue(databaseId, name, ElisaUtils.Track,
                                                              ElisaUtils.AppendPlayList,
                                                              ElisaUtils.DoNotTriggerPlay)

            onReplaceAndPlay: elisa.mediaPlayList.enqueue(databaseId, name, ElisaUtils.Track,
                                                                     ElisaUtils.ReplacePlayList,
                                                                     ElisaUtils.TriggerPlay)


            onClicked: listView.currentIndex = index

            onActiveFocusChanged: {
                if (activeFocus && listView.currentIndex !== index) {
                    listView.currentIndex = index
                }
            }

            onCallOpenMetaDataView: {
                openMetaDataView(databaseId)
            }
        }
    }

    Component {
        id: detailedTrackDelegate

        ListBrowserDelegate {
            id: entry

            width: listView.delegateWidth
            height: elisaTheme.trackDelegateHeight

            focus: true

            databaseId: model.databaseId
            title: model.title ? model.title : ''
            artist: model.artist ? model.artist : ''
            album: model.album ? model.album : ''
            albumArtist: model.albumArtist ? model.albumArtist : ''
            duration: model.duration ? model.duration : ''
            imageUrl: model.imageUrl ? model.imageUrl : ''
            trackNumber: model.trackNumber ? model.trackNumber : -1
            discNumber: model.discNumber ? model.discNumber : -1
            rating: model.rating
            hideDiscNumber: model.isSingleDiscAlbum
            isSelected: listView.currentIndex === index
            isAlternateColor: (index % 2) === 1

            onEnqueue: elisa.mediaPlayList.enqueue(databaseId, name, modelType,
                                                   ElisaUtils.AppendPlayList,
                                                   ElisaUtils.DoNotTriggerPlay)

            onReplaceAndPlay: elisa.mediaPlayList.enqueue(databaseId, name, modelType,
                                                          ElisaUtils.ReplacePlayList,
                                                          ElisaUtils.TriggerPlay)

            onClicked: {
                listView.currentIndex = index
                entry.forceActiveFocus()
            }

            onCallOpenMetaDataView: {
                openMetaDataView(databaseId)
            }
        }
    }

    ListBrowserView {
        id: listView

        focus: true

        anchors.fill: parent

        contentModel: proxyModel

        delegate: (displaySingleAlbum ? albumDelegate : detailedTrackDelegate)

        enableSorting: !displaySingleAlbum

        allowArtistNavigation: isSubPage

        onShowArtist: {
            viewManager.openChildView(secondaryTitle, '', elisaTheme.artistIcon, 0, ElisaUtils.Artist, ViewManager.NoDiscHeaders)
        }

        onGoBack: viewManager.goBack()

        Loader {
            anchors.centerIn: parent
            height: Kirigami.Units.gridUnit * 5
            width: height

            visible: realModel.isBusy
            active: realModel.isBusy

            sourceComponent: BusyIndicator {
                anchors.centerIn: parent
            }
        }
    }

    Connections {
        target: elisa

        onMusicManagerChanged: realModel.initialize(elisa.musicManager,
                                                    elisa.musicManager.viewDatabase,
                                                    modelType)
    }

    Connections {
        target: listView.navigationBar

        onCreateRadio: {
            openMetaDataView(-1)
        }
    }

    Component.onCompleted: {
        if (elisa.musicManager) {
            realModel.initialize(elisa.musicManager, elisa.musicManager.viewDatabase, modelType, filterType, mainTitle, secondaryTitle, databaseId)
        }

        if (sortAscending === ViewManager.SortAscending) {
            proxyModel.sortModel(Qt.AscendingOrder)
        } else if (sortAscending === ViewManager.SortDescending) {
            proxyModel.sortModel(Qt.DescendingOrder)
        }
    }
}
