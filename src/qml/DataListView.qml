/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

FocusScope {
    id: viewHeader

    property var filterType
    property alias isSubPage: listView.isSubPage
    property alias mainTitle: listView.mainTitle
    property alias secondaryTitle: listView.secondaryTitle
    property url pathFilter
    property int databaseId
    property alias showSection: listView.showSection
    property alias expandedFilterView: listView.expandedFilterView
    property alias haveTreeModel: listView.haveTreeModel
    property alias image: listView.image
    property var modelType
    property AbstractItemModel realModel
    property AbstractProxyModel proxyModel
    property int sortRole
    property var sortAscending
    property bool displaySingleAlbum: false
    property alias radioCase: listView.showCreateRadioButton
    property bool modelIsInitialized: false

    function openMetaDataView(databaseId, url, entryType) {
        metadataLoader.setSource("MediaTrackMetadataView.qml",
                                 {
                                     "fileName": url,
                                     "modelType": entryType,
                                     "showImage": entryType !== ElisaUtils.Radio,
                                     "showTrackFileName": entryType !== ElisaUtils.Radio,
                                     "showDeleteButton": entryType === ElisaUtils.Radio,
                                     "showApplyButton": entryType === ElisaUtils.Radio,
                                     "editableMetadata": entryType === ElisaUtils.Radio,
                                     "widthIndex": (entryType === ElisaUtils.Radio ? 4.5 : 2.8),
                                 });
        metadataLoader.active = true
    }
    function openCreateRadioView()
    {
        metadataLoader.setSource("MediaTrackMetadataView.qml",
                                 {
                                     "modelType": ElisaUtils.Radio,
                                     "isCreation": true,
                                     "showImage": false,
                                     "showTrackFileName": false,
                                     "showDeleteButton": false,
                                     "showApplyButton": true,
                                     "editableMetadata": true,
                                     "widthIndex": 4.5,
                                 });
        metadataLoader.active = true
    }

    function initializeModel()
    {
        if (!proxyModel) {
            return
        }

        if (!realModel) {
            return
        }

        if (!elisa.musicManager) {
            return
        }

        if (modelIsInitialized) {
            return
        }

        proxyModel.sourceModel = realModel
        proxyModel.dataType = modelType
        proxyModel.playList = elisa.mediaPlayListProxyModel
        listView.contentModel = proxyModel

        if (!displaySingleAlbum) {
            proxyModel.sortModel(sortAscending)
        }

        realModel.initialize(elisa.musicManager, elisa.musicManager.viewDatabase,
                             modelType, filterType, mainTitle, secondaryTitle, databaseId,
                             pathFilter)
    }

    function goToBack() {
        listView.goToBack()
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

            focus: true

            trackUrl: model.url
            dataType: model.dataType
            title: model.display ? model.display : ''
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

            onEnqueue: elisa.mediaPlayListProxyModel.enqueue(model.fullData, model.display,
                                                             ElisaUtils.AppendPlayList,
                                                             ElisaUtils.DoNotTriggerPlay)

            onReplaceAndPlay: elisa.mediaPlayListProxyModel.enqueue(model.fullData, model.display,
                                                                    ElisaUtils.ReplacePlayList,
                                                                    ElisaUtils.TriggerPlay)


            onClicked: listView.currentIndex = index

            onActiveFocusChanged: {
                if (activeFocus && listView.currentIndex !== index) {
                    listView.currentIndex = index
                }
            }

            onCallOpenMetaDataView: {
                openMetaDataView(databaseId, url, entryType)
            }
        }
    }

    Component {
        id: detailedTrackDelegate

        ListBrowserDelegate {
            id: entry

            width: listView.delegateWidth

            focus: true

            trackUrl: model.url
            dataType: model.dataType
            title: model.display ? model.display : ''
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

            onEnqueue: elisa.mediaPlayListProxyModel.enqueue(model.fullData, model.display,
                                                             ElisaUtils.AppendPlayList,
                                                             ElisaUtils.DoNotTriggerPlay)

            onReplaceAndPlay: elisa.mediaPlayListProxyModel.enqueue(model.fullData, model.display,
                                                                    ElisaUtils.ReplacePlayList,
                                                                    ElisaUtils.TriggerPlay)

            onClicked: {
                listView.currentIndex = index
                entry.forceActiveFocus()
            }

            onCallOpenMetaDataView: {
                openMetaDataView(databaseId, url, entryType)
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

        showCreateRadioButton: modelType === ElisaUtils.Radio
        showEnqueueButton: modelType !== ElisaUtils.Radio

        onShowArtist: {
            viewManager.openChildView(secondaryTitle, '', elisaTheme.artistIcon, 0, ElisaUtils.Artist)
        }

        onGoBackRequested: viewManager.goBack()

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

        onMusicManagerChanged: initializeModel()
    }

    Connections {
        target: listView.navigationBar

        onCreateRadio: {
            openCreateRadioView()
        }
    }

    Component.onCompleted: {
        initializeModel()
    }
}
