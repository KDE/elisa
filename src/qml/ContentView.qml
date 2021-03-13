/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import org.kde.elisa 1.0
import org.kde.kirigami 2.8 as Kirigami

import "mobile"

RowLayout {
    id: contentViewContainer
    spacing: 0

    property bool showPlaylist
    property bool showExpandedFilterView
    property int currentViewIndex: getCurrentViewIndex()
    property Kirigami.ContextDrawer playlistDrawer
    property alias initialIndex: viewManager.initialIndex

    property alias sidebar: mobileSidebar.item

    // setCurrentViewIndex be called before loaders are loaded, so store the value
    property int preloadIndex: -1

    function goBack() {
        viewManager.goBack()
    }

    function openArtist(name) {
        viewManager.openArtistView(name)
    }

    function openAlbum(album, artist, image, albumID) {
        image = !image ? elisaTheme.defaultAlbumImage : image;
        viewManager.openAlbumView(album, artist, albumID, image);
    }

    function openNowPlaying() {
        viewManager.openNowPlaying();
    }

    function getCurrentViewIndex() {
        if (mobileSidebar.item != null) {
            return mobileSidebar.item.viewIndex;
        } else if (desktopSidebar.item != null) {
            return desktopSidebar.item.currentIndex;
        }
    }

    function setCurrentViewIndex(index) {
        if (mobileSidebar.item != null) {
            mobileSidebar.item.switchView(index);
        } else if (desktopSidebar.item != null) {
            desktopSidebar.item.setCurrentIndex(index);
        } else {
            contentViewContainer.preloadIndex = index;
        }
    }

    ViewManager {
        id: viewManager

        viewsData: viewsData

        onOpenGridView: {
            if (configurationData.expectedDepth === 1) {
                contentViewContainer.setCurrentViewIndex(viewManager.viewIndex)
            }

            while(browseStackView.depth > configurationData.expectedDepth) {
                browseStackView.pop()
            }

            browseStackView.push(dataGridView, {
                                     filterType: configurationData.filterType,
                                     mainTitle: configurationData.mainTitle,
                                     secondaryTitle: configurationData.secondaryTitle,
                                     image: configurationData.imageUrl,
                                     modelType: configurationData.dataType,
                                     realModel: configurationData.model,
                                     proxyModel: configurationData.associatedProxyModel,
                                     defaultIcon: configurationData.viewDefaultIcon,
                                     showRating: configurationData.viewShowRating,
                                     delegateDisplaySecondaryText: configurationData.viewDelegateDisplaySecondaryText,
                                     filter: configurationData.dataFilter,
                                     isSubPage: (browseStackView.depth >= 2),
                                     haveTreeModel: configurationData.isTreeModel,
                                     stackView: configurationData.browseStackView,
                                     sortRole: configurationData.sortRole,
                                     sortRoles: configurationData.sortRoles,
                                     sortRoleNames: configurationData.sortRoleNames,
                                     sortOrder: configurationData.sortOrder,
                                     sortOrderNames: configurationData.sortOrderNames,
                                     viewManager: viewManager,
                                     opacity: 1,
                                 })
        }

        onOpenListView: {
            if (configurationData.expectedDepth === 1) {
                contentViewContainer.setCurrentViewIndex(viewManager.viewIndex)
            }

            while(browseStackView.depth > configurationData.expectedDepth) {
                browseStackView.pop()
            }

            browseStackView.push(dataListView, {
                                     filterType: configurationData.filterType,
                                     isSubPage: configurationData.expectedDepth > 1,
                                     mainTitle: configurationData.mainTitle,
                                     secondaryTitle: configurationData.secondaryTitle,
                                     filter: configurationData.dataFilter,
                                     image: configurationData.imageUrl,
                                     modelType: configurationData.dataType,
                                     realModel: configurationData.model,
                                     proxyModel: configurationData.associatedProxyModel,
                                     sortRole: configurationData.sortRole,
                                     sortAscending: configurationData.sortOrder,
                                     stackView: browseStackView,
                                     displaySingleAlbum: configurationData.displaySingleAlbum,
                                     showSection: configurationData.showDiscHeaders,
                                     radioCase: configurationData.radioCase,
                                     haveTreeModel: configurationData.isTreeModel,
                                     sortRole: configurationData.sortRole,
                                     sortRoles: configurationData.sortRoles,
                                     sortRoleNames: configurationData.sortRoleNames,
                                     sortOrder: configurationData.sortOrder,
                                     sortOrderNames: configurationData.sortOrderNames,
                                     viewManager: viewManager,
                                     opacity: 1,
                                 })
        }

        onSwitchContextView: {
            if (preloadIndex == -1) {
                // prevent changing page to viewManager.viewIndex if there is a pending page change
                contentViewContainer.setCurrentViewIndex(viewManager.viewIndex)
            }

            while(browseStackView.depth > expectedDepth) {
                browseStackView.pop()
            }

            browseStackView.push(albumContext, {
                                     mainTitle: mainTitle,
                                     image: imageUrl,
                                     opacity: 1,
                                 })
        }

        onPopOneView: {
            if (browseStackView.currentItem.haveTreeModel) {
                browseStackView.currentItem.goToBack()
            }

            if (browseStackView.depth > 2) {
                browseStackView.pop()
            }
        }
    }

    ViewsModel {
        id: pageModel

        viewsData: viewsData
    }

    ViewsProxyModel {
        id: pageProxyModel

        sourceModel: pageModel
    }

    ViewsListData {
        id: viewsData

        manager: ElisaApplication.musicManager
        database: ElisaApplication.musicManager.viewDatabase
        embeddedCategory: ElisaApplication.embeddedView
    }

    // sidebar used on desktop
    Loader {
        id: desktopSidebar
        active: !Kirigami.Settings.isMobile
        Layout.fillHeight: true

        onLoaded: {
            if (contentViewContainer.preloadIndex != -1) {
                item.setCurrentIndex(contentViewContainer.preloadIndex);
                viewManager.openView(contentViewContainer.preloadIndex);
                contentViewContainer.preloadIndex = -1;
            }
        }
        sourceComponent: ViewSelector {
            model: pageProxyModel
            onSwitchView: viewManager.openView(viewIndex)
        }
    }

    // sidebar used on mobile
    Loader {
        id: mobileSidebar
        active: Kirigami.Settings.isMobile
        onLoaded: {
            if (contentViewContainer.preloadIndex != -1) {
                item.switchView(contentViewContainer.preloadIndex);
                viewManager.openView(contentViewContainer.preloadIndex);
                contentViewContainer.preloadIndex = -1;
            }
        }
        sourceComponent: MobileSidebar {
            model: pageProxyModel
            onSwitchView: viewManager.openView(viewIndex)
        }
    }

    Kirigami.Separator {
        id: viewSelectorSeparatorItem
        visible: !Kirigami.Settings.isMobile
        Layout.fillHeight: true
    }

    FocusScope {
        id: mainContentView

        focus: true

        Layout.fillHeight: true
        Layout.fillWidth: true

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.BackButton
            onClicked: goBack()
        }

        Rectangle {
            radius: 3
            color: myPalette.base

            anchors.fill: parent

            StackView {
                id: browseStackView

                anchors.fill: parent

                clip: true

                initialItem: Item {
                }

                popEnter: Transition {
                    OpacityAnimator {
                        from: 0.0
                        to: 1.0
                        duration: Kirigami.Units.longDuration
                    }
                }

                popExit: Transition {
                    OpacityAnimator {
                        from: 1.0
                        to: 0.0
                        duration: Kirigami.Units.longDuration
                    }
                }

                pushEnter: Transition {
                    OpacityAnimator {
                        from: 0.0
                        to: 1.0
                        duration: Kirigami.Units.longDuration
                    }
                }

                pushExit: Transition {
                    OpacityAnimator {
                        from: 1.0
                        to: 0.0
                        duration: Kirigami.Units.longDuration
                    }
                }

                replaceEnter: Transition {
                    OpacityAnimator {
                        from: 0.0
                        to: 1.0
                        duration: Kirigami.Units.longDuration
                    }
                }

                replaceExit: Transition {
                    OpacityAnimator {
                        from: 1.0
                        to: 0.0
                        duration: Kirigami.Units.longDuration
                    }
                }
            }
        }
    }

    Kirigami.Separator {
        id: playListSeparatorItem
        Layout.fillHeight: true
    }

    // playlist right sidebar
    MediaPlayListView {
        id: playList
        Layout.fillHeight: true
        onStartPlayback: ElisaApplication.audioControl.ensurePlay()
        onPausePlayback: ElisaApplication.audioControl.playPause()
    }

    states: [
        State {
            name: "smallScreen"
            when: mainWindow.width < elisaTheme.viewSelectorSmallSizeThreshold
            PropertyChanges {
                target: playList
                Layout.minimumWidth: 0
                Layout.maximumWidth: 0
                Layout.preferredWidth: 0
                visible: false
            }
            PropertyChanges {
                target: playListSeparatorItem
                visible: false
            }
            PropertyChanges {
                target: playlistDrawer
                modal: true
                drawerOpen: true
                handleVisible: true
            }
        },
        State {
            name: "wideScreenNoPlaylist"
            when: mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold && contentViewContainer.showPlaylist === false
            PropertyChanges {
                target: playList
                Layout.minimumWidth: 0
                Layout.maximumWidth: 0
                Layout.preferredWidth: 0
            }
            PropertyChanges {
                target: playListSeparatorItem
                visible: false
            }
            PropertyChanges {
                target: playlistDrawer
                collapsed: true
                visible: false
                drawerOpen: false
                handleVisible: false
            }
        },
        State {
            name: 'wideScreenPlaylist'
            when: mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold && contentViewContainer.showPlaylist === true
            PropertyChanges {
                target: playList
                Layout.minimumWidth: contentViewContainer.width * 0.28
                Layout.maximumWidth: contentViewContainer.width * 0.28
                Layout.preferredWidth: contentViewContainer.width * 0.28
            }
            PropertyChanges {
                target: playListSeparatorItem
                visible: true
            }
            PropertyChanges {
                target: playlistDrawer
                collapsed: true
                visible: false
                drawerOpen: false
                handleVisible: false
            }

        }
    ]
    transitions: Transition {
        NumberAnimation {
            properties: "Layout.minimumWidth, Layout.maximumWidth, Layout.preferredWidth, opacity"
            easing.type: Easing.InOutQuad
            duration: Kirigami.Units.longDuration
        }
    }

    Component {
        id: dataGridView

        DataGridView {
            StackView.onActivated: viewManager.viewIsLoaded()
            expandedFilterView: showExpandedFilterView
        }
    }

    Component {
        id: dataListView

        DataListView {
            StackView.onActivated: viewManager.viewIsLoaded()
            expandedFilterView: showExpandedFilterView
        }
    }

    Component {
        id: albumContext

        ContextView {
            StackView.onActivated: viewManager.viewIsLoaded()

            databaseId: ElisaApplication.manageHeaderBar.databaseId
            trackType: ElisaApplication.manageHeaderBar.trackType
            songTitle: ElisaApplication.manageHeaderBar.title
            artistName: ElisaApplication.manageHeaderBar.artist
            albumName: ElisaApplication.manageHeaderBar.album
            albumArtUrl: ElisaApplication.manageHeaderBar.image
            fileUrl: ElisaApplication.manageHeaderBar.fileUrl
            albumId: ElisaApplication.manageHeaderBar.albumId
            albumArtist: (ElisaApplication.manageHeaderBar.albumArtist !== undefined ? ElisaApplication.manageHeaderBar.albumArtist : '')
            onOpenArtist: { contentViewContainer.openArtist(artistName) }
            onOpenAlbum: { contentViewContainer.openAlbum(albumName, albumArtist, albumArtUrl, albumId) }
        }
    }
}
