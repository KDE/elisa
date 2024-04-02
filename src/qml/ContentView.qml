/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import org.kde.elisa
import org.kde.kirigami 2.8 as Kirigami

import "mobile"

RowLayout {
    id: contentViewContainer
    spacing: 0

    property bool showPlaylist
    property bool showExpandedFilterView
    property Kirigami.ContextDrawer playlistDrawer
    property alias initialIndex: viewManager.initialIndex

    property alias sidebar: mobileSidebar.item
    property Loader activeSidebarLoader: Kirigami.Settings.isMobile ? mobileSidebar : desktopSidebar

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

    function openViewCommon(viewDelegate, viewProperties, expectedDepth) {
        if (expectedDepth === 1) {
            // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-117720
            const operation = StackView.ReplaceTransition
            browseStackView.replace(null, viewDelegate, viewProperties, operation)
            updateSidebarIndex()
        } else {
            browseStackView.push(viewDelegate, viewProperties)
        }
    }

    function updateSidebarIndex() {
        // Sometimes the sidebar loads after the page is pushed
        if (activeSidebarLoader.status === Loader.Ready) {
            activeSidebarLoader.item.viewIndex = pageProxyModel.mapRowFromSource(viewManager.viewIndex)
        }
    }

    ViewManager {
        id: viewManager

        viewsData: viewsData
        initialFilesViewPath: ElisaApplication.initialFilesViewPath

        onOpenGridView: configurationData => {
            openViewCommon(dataGridView, {
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
                isSubPage: configurationData.expectedDepth > 1,
                sortRole: configurationData.sortRole,
                sortRoles: configurationData.sortRoles,
                sortRoleNames: configurationData.sortRoleNames,
                sortOrder: configurationData.sortOrder,
                sortOrderNames: configurationData.sortOrderNames,
                viewManager: viewManager,
                opacity: 1,
            }, configurationData.expectedDepth)
        }

        onOpenListView: configurationData => {
            openViewCommon(dataListView, {
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
                sortOrder: configurationData.sortOrder,
                displaySingleAlbum: configurationData.displaySingleAlbum,
                showSection: configurationData.showDiscHeaders,
                sortRole: configurationData.sortRole,
                sortRoles: configurationData.sortRoles,
                sortRoleNames: configurationData.sortRoleNames,
                sortOrder: configurationData.sortOrder,
                sortOrderNames: configurationData.sortOrderNames,
                viewManager: viewManager,
                opacity: 1,
            }, configurationData.expectedDepth)
        }

        onSwitchContextView: (expectedDepth, mainTitle, imageUrl) => {
            openViewCommon(albumContext, {opacity: 1}, expectedDepth)
        }

        onPopOneView: {
            if (browseStackView.depth > 1) {
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
        embeddedCategory: Kirigami.Settings.isMobile ? ElisaUtils.Unknown : ElisaApplication.embeddedView

        onEmbeddedCategoryChanged: updateSidebarIndex()
    }

    ViewsListData {
        id: viewsData

        manager: ElisaApplication.musicManager
        database: ElisaApplication.musicManager.viewDatabase
        embeddedCategory: Kirigami.Settings.isMobile ? ElisaUtils.Unknown : ElisaApplication.embeddedView
    }

    // sidebar used on desktop
    Loader {
        id: desktopSidebar
        active: !Kirigami.Settings.isMobile
        Layout.fillHeight: true

        sourceComponent: ViewSelector {
            model: pageProxyModel
            viewIndex: model.mapRowFromSource(viewManager.viewIndex)
            onSwitchView: viewIndex => viewManager.openView(model.mapRowToSource(viewIndex))
        }
    }

    // sidebar used on mobile
    Loader {
        id: mobileSidebar
        active: Kirigami.Settings.isMobile

        sourceComponent: MobileSidebar {
            model: pageProxyModel
            viewIndex: model.mapRowFromSource(viewManager.viewIndex)
            onSwitchView: viewIndex => viewManager.openView(model.mapRowToSource(viewIndex))
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

        StackView {
            id: browseStackView

            anchors.fill: parent

            clip: true

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

    Kirigami.Separator {
        id: playListSeparatorItem
        visible: playList.width > 0
        Layout.fillHeight: true
    }

    // playlist right sidebar
    MediaPlayListView {
        id: playList
        Layout.preferredWidth: 0
        Layout.fillWidth: false
        Layout.fillHeight: true
        enabled: Layout.preferredWidth !== 0 // Avoid taking keyboard focus when not visible

        states: [
            State {
                name: "playlistVisible"
                when: mainWindow.isWideScreen && contentViewContainer.showPlaylist
                PropertyChanges {
                    target: playList
                    Layout.preferredWidth: contentViewContainer.width * 0.28
                }
            }
        ]

        transitions: Transition {
            NumberAnimation {
                property: "Layout.preferredWidth"
                easing.type: Easing.InOutQuad
                duration: Kirigami.Units.longDuration
            }
        }
    }

    Component {
        id: dataGridView

        DataGridView {
            expandedFilterView: showExpandedFilterView
        }
    }

    Component {
        id: dataListView

        DataListView {
            expandedFilterView: showExpandedFilterView
        }
    }

    Component {
        id: albumContext

        ContextView {
            databaseId: ElisaApplication.manageHeaderBar.databaseId
            trackType: ElisaApplication.manageHeaderBar.trackType
            songTitle: ElisaApplication.manageHeaderBar.title
            artistName: ElisaApplication.manageHeaderBar.artist !== undefined ? ElisaApplication.manageHeaderBar.artist : ''
            albumName: ElisaApplication.manageHeaderBar.album
            albumArtUrl: ElisaApplication.manageHeaderBar.image
            fileUrl: ElisaApplication.manageHeaderBar.fileUrl
            albumId: ElisaApplication.manageHeaderBar.albumId
            albumArtist: ElisaApplication.manageHeaderBar.albumArtist !== undefined ? ElisaApplication.manageHeaderBar.albumArtist : ''
            onOpenArtist: contentViewContainer.openArtist(artistName)
            onOpenAlbum: contentViewContainer.openAlbum(albumName, albumArtist, albumArtUrl, albumId)
        }
    }
}
