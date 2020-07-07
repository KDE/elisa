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

RowLayout {
    id: contentViewContainer
    spacing: 0

    property bool showPlaylist
    property bool showExpandedFilterView
    property alias currentViewIndex: listViews.currentIndex

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
        viewManager.closeAllViews();
    }

    ViewManager {
        id: viewManager

        viewsData: viewsData

        onOpenGridView: {
            if (configurationData.expectedDepth === 1) {
                listViews.setCurrentIndex(viewManager.viewIndex)
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
                                     opacity: 0,
                                 })
        }

        onOpenListView: {
            if (configurationData.expectedDepth === 1) {
                listViews.setCurrentIndex(viewManager.viewIndex)
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
                                     opacity: 0,
                                     radioCase: configurationData.radioCase,
                                     haveTreeModel: configurationData.isTreeModel,
                                 })
        }

        onSwitchFilesBrowserView: {
            listViews.setCurrentIndex(viewManager.viewIndex)

            while(browseStackView.depth > expectedDepth) {
                browseStackView.pop()
            }

            browseStackView.push(filesBrowserView, {
                                     mainTitle: mainTitle,
                                     image: imageUrl,
                                     opacity: 0,
                                 })
        }

        onSwitchContextView: {
            listViews.setCurrentIndex(viewManager.viewIndex)

            while(browseStackView.depth > expectedDepth) {
                browseStackView.pop()
            }

            browseStackView.push(albumContext, {
                                     mainTitle: mainTitle,
                                     image: imageUrl,
                                     opacity: 0,
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

    ViewSelector {
        id: listViews

        model: pageProxyModel

        Layout.fillHeight: true

        onSwitchView: viewManager.openView(viewIndex)
    }

    Kirigami.Separator {
        id: viewSelectorSeparatorItem
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

    MediaPlayListView {
        id: playList

        Layout.fillHeight: true

        onStartPlayback: ElisaApplication.audioControl.ensurePlay()
        onPausePlayback: ElisaApplication.audioControl.playPause()
    }

    states: [
        State {
            name: "browsingViewsNoPlaylist"
            when: contentViewContainer.showPlaylist === false || mainWindow.width < elisaTheme.viewSelectorSmallSizeThreshold
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
        },
        State {
            name: 'browsingViews'
            when: contentViewContainer.showPlaylist === true || mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold
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
            title: ElisaApplication.manageHeaderBar.title
            artistName: ElisaApplication.manageHeaderBar.artist
            albumName: ElisaApplication.manageHeaderBar.album
            albumArtUrl: ElisaApplication.manageHeaderBar.image
            fileUrl: ElisaApplication.manageHeaderBar.fileUrl
        }
    }
}
