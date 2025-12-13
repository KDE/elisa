/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Window
import org.kde.elisa
import org.kde.kirigami as Kirigami

SplitView {
    id: contentViewContainer

    property bool showPlaylist
    property bool showExpandedFilterView
    property Kirigami.ContextDrawer playlistDrawer
    property alias initialIndex: viewManager.initialIndex
    property alias pageProxyModel: pageProxyModel
    property alias viewManager: viewManager
    property alias playListHiddenCachedWidth: playListContainer.hiddenCachedWidth
    readonly property alias playListPreferredWidth: playListContainer.preferredWidth

    signal viewIndexChanged

    onViewIndexChanged: desktopSidebar.updateSidebarIndex()

    function goBack() {
        viewManager.goBack()
    }

    function openArtist(name) {
        viewManager.openArtistView(name)
    }

    function openAlbum(album, artist, image, albumID) {
        image = !image ? Theme.defaultAlbumImage : image;
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
        } else {
            browseStackView.push(viewDelegate, viewProperties)
        }
    }

    ViewManager {
        id: viewManager

        viewsData: viewsData
        initialFilesViewPath: ElisaApplication.initialFilesViewPath

        onViewIndexChanged: contentViewContainer.viewIndexChanged()

        onOpenGridView: configurationData => {
            contentViewContainer.openViewCommon(dataGridView, {
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
                viewStyle: configurationData.viewStyle,
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

        onOpenTrackView: configurationData => {
            contentViewContainer.openViewCommon(dataTrackView, {
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
            contentViewContainer.openViewCommon(albumContext, {opacity: 1}, expectedDepth)
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

        onEmbeddedCategoryChanged: contentViewContainer.viewIndexChanged()
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
        visible: active

        SplitView.minimumWidth: active ? (item as ViewSelector).iconsOnlyMinWidth : 0
        SplitView.preferredWidth: active ? (item as ViewSelector).wideWidth : 0

        function updateSidebarIndex() {
            if (status === Loader.Ready) {
                item.viewIndex = pageProxyModel.mapRowFromSource(viewManager.viewIndex)
            }
        }

        sourceComponent: ViewSelector {
            displayedViewIndex: model.mapRowFromSource(viewManager.viewIndex)
            model: pageProxyModel
            viewIndex: model.mapRowFromSource(viewManager.viewIndex)
            onSwitchView: viewIndex => viewManager.openView(model.mapRowToSource(viewIndex))

            // Make sure the sidebar stays at minimum width if the scrollbar disappears
            onIconsOnlyMinWidthChanged: {
                if (desktopSidebar.SplitView.preferredWidth === iconsOnlyMaxWidth) {
                    desktopSidebar.SplitView.preferredWidth = iconsOnlyMinWidth;
                }
            }
        }
    }

    FocusScope {
        id: mainContentView

        focus: true

        SplitView.fillWidth: true
        SplitView.minimumWidth: Kirigami.Settings.isMobile ? mainWindow.minimumWidth : Theme.contentViewMinimumSize

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.BackButton
            onClicked: contentViewContainer.goBack()
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

    // playlist right sidebar
    // Changing the playlist's width causes many Loaders to activate/deactivate, which results in
    // a choppy animation when hiding the playlist. When hiding the playlist, we fix the PlayList
    // width and change the parent Item's width instead. This gives the appearance of the PlayList
    // sliding off the screen without the performance issues caused by Loaders.
    Item {
        id: playListContainer

        /**
        * Cached width of the playlist when the playlist is hidden
        *
        * This does not get updated when the playlist is resized whilst visible
        */
        property real hiddenCachedWidth: 0

        /**
         * The current preferred width of the playlist, whether the playlist is visible or not
         */
        property real preferredWidth: hiddenCachedWidth

        // Default values for when the playlist is not visible
        SplitView.minimumWidth: 0
        SplitView.preferredWidth: 0

        visible: SplitView.preferredWidth !== 0 // Only hide the playlist *after* the width transition
        enabled: visible // Avoid taking keyboard focus when not visible

        MediaPlayListView {
            id: playList
            width: playListContainer.hiddenCachedWidth
            height: playListContainer.height
        }

        // We use a state here so that the width only animates during
        // the transition, not when using the drag handle.
        states: [
            State {
                name: "playlistVisible"
                when: mainWindow.isWideScreen && contentViewContainer.showPlaylist
                PropertyChanges {
                    playListContainer.visible: true
                    playListContainer.SplitView.minimumWidth: 10 * Kirigami.Units.gridUnit
                    playListContainer.SplitView.preferredWidth: playListContainer.hiddenCachedWidth
                    playListContainer.preferredWidth: playListContainer.SplitView.preferredWidth
                    playList.width: playListContainer.width
                }
            }
        ]

        transitions: Transition {
            ScriptAction {
                script: {
                    if (playListContainer.state === "") {
                        playListContainer.hiddenCachedWidth = playListContainer.SplitView.preferredWidth;
                    }
                }
            }
            NumberAnimation {
                property: "SplitView.preferredWidth"
                easing.type: Easing.InOutQuad
                duration: Kirigami.Units.longDuration
            }
        }
    }

    Component {
        id: dataGridView

        DataGridView {
            expandedFilterView: contentViewContainer.showExpandedFilterView
        }
    }

    Component {
        id: dataTrackView

        DataTrackView {
            expandedFilterView: contentViewContainer.showExpandedFilterView
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
