/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2022 (c) Nate Graham <kate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml.Models 2.1
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.12 as Kirigami
import org.kde.elisa 1.0

FocusScope {
    id: listView

    property AbstractItemModel realModel
    property AbstractProxyModel contentModel
    property AbstractProxyModel proxyModel

    property string mainTitle
    property string secondaryTitle
    property url image
    property int depth: 1
    property int databaseId

    property bool showSection: false
    property bool isSubPage: false
    property bool haveTreeModel: false
    property bool radioCase: false
    property bool displaySingleAlbum: false
    property bool modelIsInitialized: false

    property var filter
    property var modelType
    property var filterType

    property alias currentIndex: contentDirectoryView.currentIndex
    property alias expandedFilterView: navigationBar.expandedFilterView
    property alias showRating: navigationBar.showRating
    property alias sortRole: navigationBar.sortRole
    property alias sortRoles: navigationBar.sortRoles
    property alias sortRoleNames: navigationBar.sortRoleNames
    property alias sortOrderNames: navigationBar.sortOrderNames
    property alias sortOrder: navigationBar.sortOrder
    property alias viewManager: navigationBar.viewManager
    property alias delegate: delegateModel.delegate


    signal showArtist(var name)

    onShowArtist: {
        viewManager.openArtistView(secondaryTitle)
    }


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
        modelIsInitialized = true
    }

    function goToBack() {
        if (haveTreeModel) {
            delegateModel.rootIndex = delegateModel.parentModelIndex()
            --depth
        } else {
            viewManager.goBack()
        }
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


    // Model
    DelegateModel {
        id: delegateModel

        model: listView.contentModel

        delegate: ListBrowserDelegate {
            id: entry

            width: contentDirectoryView.width

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
            hideDiscNumber: !listView.displaySingleAlbum && model.isSingleDiscAlbum
            isSelected: listView.currentIndex === index
            isAlternateColor: (index % 2) === 1
            detailedView: !listView.displaySingleAlbum

            onTrackRatingChanged: {
                ElisaApplication.musicManager.updateSingleFileMetaData(url, DataTypes.RatingRole, rating)
            }

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


    // Main view components
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        NavigationActionBar {
            id: navigationBar

            z: 1 // on top of track list

            mainTitle: listView.mainTitle
            secondaryTitle: listView.secondaryTitle
            image: listView.image
            enableGoBack: listView.isSubPage || depth > 1
            allowArtistNavigation: listView.isSubPage
            showCreateRadioButton: listView.modelType === ElisaUtils.Radio
            showEnqueueButton: listView.modelType !== ElisaUtils.Radio

            Layout.fillWidth: true

            Binding {
                target: listView.contentModel
                property: 'filterText'
                when: listView.contentModel
                value: navigationBar.filterText
            }

            Binding {
                target: listView.contentModel
                property: 'filterRating'
                when: listView.contentModel
                value: navigationBar.filterRating
            }

            Binding {
                target: listView.contentModel
                property: 'sortRole'
                when: listView.contentModel && navigationBar.enableSorting
                value: navigationBar.sortRole
            }

            onEnqueue: contentModel.enqueueToPlayList(delegateModel.rootIndex)

            onReplaceAndPlay: contentModel.replaceAndPlayOfPlayList(delegateModel.rootIndex)

            onGoBack: {
                listView.goToBack()
            }

            onShowArtist: listView.showArtist(listView.contentModel.sourceModel.author)

            onSortOrderChanged: {
                if (!contentModel || !navigationBar.enableSorting) {
                    return
                }

                if ((contentModel.sortedAscending && sortOrder !== Qt.AscendingOrder) ||
                    (!contentModel.sortedAscending && sortOrder !== Qt.DescendingOrder)) {
                    contentModel.sortModel(sortOrder)
                    }
            }
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            contentItem: ListView {
                id: contentDirectoryView

                Accessible.role: Accessible.List
                Accessible.name: mainTitle
                Accessible.description: mainTitle

                activeFocusOnTab: true
                keyNavigationEnabled: true

                reuseItems: true

                model: delegateModel

                // HACK: setting currentIndex to -1 in mobile for some reason causes segfaults, no idea why
                currentIndex: Kirigami.Settings.isMobile ? 0 : -1

                section.property: (showSection ? 'discNumber' : '')
                section.criteria: ViewSection.FullString
                section.labelPositioning: ViewSection.InlineLabels
                section.delegate: TracksDiscHeader {
                    discNumber: section
                    width: contentDirectoryView.width
                }

                Kirigami.PlaceholderMessage {
                    anchors.centerIn: parent
                    width: parent.width - (Kirigami.Units.largeSpacing * 4)
                    visible: contentDirectoryView.count === 0 && !busyIndicatorLoader.active
                    text: i18n("Nothing to display")
                }

                onCountChanged: if (count === 0) {
                    currentIndex = -1;
                }
            }
        }
    }


    // Placeholder spinner while loading
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


    Connections {
        target: ElisaApplication

        function onMusicManagerChanged() {
            initializeModel()
        }
    }

    Connections {
        target: navigationBar

        function onCreateRadio() {
            openCreateRadioView()
        }
    }

    Component.onCompleted: {
        initializeModel()
    }
}
