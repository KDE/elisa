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
    property alias showSection: listView.showSection
    property alias expandedFilterView: listView.expandedFilterView
    property alias haveTreeModel: listView.haveTreeModel
    property var filter
    property alias image: listView.image
    property var modelType
    property AbstractItemModel realModel
    property AbstractProxyModel proxyModel
    property alias sortRole: listView.sortRole
    property alias sortRoles: listView.sortRoles
    property alias sortRoleNames: listView.sortRoleNames
    property alias sortOrderNames: listView.sortOrderNames
    property alias sortOrder: listView.sortOrder
    property bool displaySingleAlbum: false
    property alias radioCase: listView.showCreateRadioButton
    property bool modelIsInitialized: false
    property alias viewManager: listView.viewManager

    function openMetaDataView(databaseId, url, entryType) {
        metadataLoader.setSource(Kirigami.Settings.isMobile ? "mobile/MobileMediaTrackMetadataView.qml" : "MediaTrackMetadataView.qml",
                                 {
                                     "fileName": url,
                                     "modelType": entryType,
                                     "showImage": entryType !== ElisaUtils.Radio,
                                     "showTrackFileName": entryType !== ElisaUtils.Radio,
                                     "showDeleteButton": entryType === ElisaUtils.Radio,
                                     "editableMetadata": true,
                                     "canAddMoreMetadata": entryType !== ElisaUtils.Radio,
                                 });
        metadataLoader.active = true
    }
    function openCreateRadioView()
    {
        metadataLoader.setSource(Kirigami.Settings.isMobile ? "mobile/MobileMediaTrackMetadataView.qml" : "MediaTrackMetadataView.qml",
                                 {
                                     "modelType": ElisaUtils.Radio,
                                     "isCreating": true,
                                     "showImage": false,
                                     "showTrackFileName": false,
                                     "showDeleteButton": true,
                                     "editableMetadata": true,
                                     "canAddMoreMetadata": false,
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

        if (!ElisaApplication.musicManager) {
            return
        }

        if (modelIsInitialized) {
            return
        }

        proxyModel.sourceModel = realModel
        proxyModel.dataType = modelType
        proxyModel.playList = Qt.binding(function() { return ElisaApplication.mediaPlayListProxyModel })
        listView.contentModel = proxyModel

        if (!displaySingleAlbum) {
            proxyModel.sortModel(sortOrder)
        }

        realModel.initializeByData(ElisaApplication.musicManager, ElisaApplication.musicManager.viewDatabase,
                                   modelType, filterType, filter)
    }

    function goToBack() {
        listView.goToBack()
    }

    Loader {
        id: metadataLoader
        active: false
        onLoaded: {
            // on mobile, the metadata loader is a page
            // on desktop, it's a window
            if (Kirigami.Settings.isMobile) {
                mainWindow.pageStack.layers.push(item);
            } else {
                item.show();
            }
        }
    }

    // desktop delegates
    Component {
        id: trackDelegate

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
            hideDiscNumber: !viewHeader.displaySingleAlbum && model.isSingleDiscAlbum
            isSelected: listView.currentIndex === index
            isAlternateColor: (index % 2) === 1
            detailedView: !viewHeader.displaySingleAlbum

            onEnqueue: ElisaApplication.mediaPlayListProxyModel.enqueue(model.fullData, model.display,
                                                             ElisaUtils.AppendPlayList,
                                                             ElisaUtils.DoNotTriggerPlay)

            onReplaceAndPlay: ElisaApplication.mediaPlayListProxyModel.enqueue(model.fullData, model.display,
                                                                    ElisaUtils.ReplacePlayList,
                                                                    ElisaUtils.TriggerPlay)

            onClicked: {
                listView.currentIndex = index;
                entry.forceActiveFocus();
            }

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

    ListBrowserView {
        id: listView

        focus: true

        anchors.fill: parent

        contentModel: proxyModel

        delegate: trackDelegate

        enableSorting: !displaySingleAlbum

        allowArtistNavigation: isSubPage

        showCreateRadioButton: modelType === ElisaUtils.Radio
        showEnqueueButton: modelType !== ElisaUtils.Radio

        onShowArtist: {
            viewManager.openArtistView(secondaryTitle)
        }

        onGoBackRequested: viewManager.goBack()

        suppressNoDataPlaceholderMessage: busyIndicatorLoader.active

        Loader {
            id: busyIndicatorLoader
            anchors.centerIn: parent
            height: Kirigami.Units.gridUnit * 5
            width: height

            visible: realModel ? realModel.isBusy : true
            active: realModel ? realModel.isBusy : true

            sourceComponent: BusyIndicator {
                anchors.centerIn: parent
            }
        }
    }

    Connections {
        target: ElisaApplication

        function onMusicManagerChanged() {
            initializeModel()
        }
    }

    Connections {
        target: listView.navigationBar

        function onCreateRadio() {
            openCreateRadioView()
        }
    }

    Component.onCompleted: {
        initializeModel()
    }
}
