/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0
import Qt.labs.settings 1.0
import Qt.labs.platform 1.1

import "mobile"

Kirigami.ApplicationWindow {
    id: mainWindow

    visible: true

    Connections {
        target: ElisaApplication.mediaPlayListProxyModel
        function onPlayListLoadFailed() {
            showPassiveNotification(i18nc("@label", "Loading failed"), 7000, i18nc("@action:button", "Retry"), () => loadPlaylistButton.clicked())
        }

        function onDisplayUndoNotification() {
            showPassiveNotification(i18nc("@label", "Playlist cleared"), 7000, i18nc("@action:button", "Undo"), () => ElisaApplication.mediaPlayListProxyModel.undoClearPlayList())
        }
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: playlistDrawer
        handleClosedIcon.source: "view-media-playlist"
        handleOpenIcon.source: "view-right-close"

        handle.visible: !Kirigami.Settings.isMobile &&
                        (playlistDrawer.drawerOpen || headerBarLoader.height > elisaTheme.mediaPlayerControlHeight * 2)

        Component.onCompleted: close() // drawer is opened when the layout is narrow, we want it closed

        // Don't allow dragging it as the UX is not so great; see
        // https://bugs.kde.org/show_bug.cgi?id=468211
        interactive: false

        // without this drawer button is never shown
        enabled: true
        MediaPlayListView {
            anchors.fill: parent
        }
    }

    DropArea {
        anchors.fill: parent
        onDropped: {
            if (drop.hasUrls) {
                if (!ElisaApplication.openFiles(drop.urls)) {
                    showPassiveNotification(i18nc("@info:status", "Could not load some files. Elisa can only open audio and playlist files."), 7000, "", function() {})
                }
            }
        }
    }

    // HACK: since elisa's main view hasn't been ported to a page, but page layers are used for mobile settings
    // lower the main view and mobile footer's z to be behind the layer when there are layers added (normally it is in front)
    property bool layerOnTop: pageStack.layers.depth > 1

    // disable certain transitions at startup
    property bool transitionsEnabled: false
    Timer {
        interval: 10
        running: true
        repeat: false
        onTriggered: { transitionsEnabled = true }
    }

    minimumWidth: Kirigami.Settings.isMobile ? 320 : 620
    property int minHeight: 320

    property string previousStateBeforeFullScreen: "windowed"
    function restorePreviousStateBeforeFullScreen() {
        if (previousStateBeforeFullScreen === "windowed") {
            showNormal()
        } else if (previousStateBeforeFullScreen === "maximized") {
            // Need to make it windowed before showMaximized will work, apparently
            showNormal()
            showMaximized()
        } else if (previousStateBeforeFullScreen === "minimized") {
            showMinimized()
        } else if (previousStateBeforeFullScreen === "hidden") {
            show()
        } else if (previousStateBeforeFullScreen === "automatic") {
            show()
        }
    }
    function goFullScreen() {
        if (visibility === Window.Windowed) {
            previousStateBeforeFullScreen = "windowed"
        } else if (visibility === Window.Maximized) {
            previousStateBeforeFullScreen = "maximized"
        } else if (visibility === Window.Minimized) {
            previousStateBeforeFullScreen = "minimized"
        } else if (visibility === Window.Hidden) {
            previousStateBeforeFullScreen = "hidden"
        } else if (visibility === Window.Automatic){
            previousStateBeforeFullScreen = "automatic"
        }
        showFullScreen()
    }

    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    x: persistentSettings.x
    y: persistentSettings.y
    width: persistentSettings.width
    height: persistentSettings.height

    title: ElisaApplication.manageHeaderBar.title ? i18nc("@title:window", "%1 — Elisa", ElisaApplication.manageHeaderBar.title) : i18nc("@title:window", "Elisa")

    readonly property int initialViewIndex: 3

    readonly property var goBackAction: ElisaApplication.action("go_back")
    readonly property var seekAction: ElisaApplication.action("Seek")
    readonly property var scrubAction: ElisaApplication.action("Scrub")
    readonly property var nextTrackAction : ElisaApplication.action("NextTrack")
    readonly property var previousTrackAction: ElisaApplication.action("PreviousTrack")
    readonly property var playPauseAction: ElisaApplication.action("Play-Pause")
    readonly property var findAction: ElisaApplication.action("edit_find")
    readonly property var togglePartyModeAction: ElisaApplication.action("togglePartyMode")

    readonly property var mediaPlayerControl: Kirigami.Settings.isMobile ? mobileFooterBarLoader.item : headerBarLoader.item
    readonly property alias fileDialog: fileDialog

    Action {
        shortcut: ElisaApplication.actionShortcut(goBackAction)
        onTriggered: contentView.goBack()
    }

    Action {
        shortcut: ElisaApplication.actionShortcut(seekAction)
        onTriggered: ElisaApplication.audioControl.seek(mediaPlayerControl.playerControl.position + 10000)
    }

    Action {
        shortcut: ElisaApplication.actionShortcut(scrubAction)
        onTriggered: ElisaApplication.audioControl.seek(mediaPlayerControl.playerControl.position - 10000)
    }

    Action {
        shortcut: ElisaApplication.actionShortcut(nextTrackAction)
        onTriggered: ElisaApplication.mediaPlayListProxyModel.skipNextTrack(ElisaApplication.audioPlayer.position)
    }

    Action {
        shortcut: ElisaApplication.actionShortcut(previousTrackAction)
        onTriggered: ElisaApplication.mediaPlayListProxyModel.skipPreviousTrack(ElisaApplication.audioPlayer.position)
    }

    Action {
        shortcut: ElisaApplication.actionShortcut(playPauseAction)
        onTriggered: ElisaApplication.audioControl.playPause()
    }

    Action {
        shortcut: ElisaApplication.actionShortcut(findAction)
        onTriggered: persistentSettings.expandedFilterView = !persistentSettings.expandedFilterView
    }

    Action {
        shortcut: ElisaApplication.actionShortcut(togglePartyModeAction)
        onTriggered: mediaPlayerControl.isMaximized = !mediaPlayerControl.isMaximized
    }

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    FileDialog {
        id: fileDialog

        function savePlaylist() {
            fileDialog.nameFilters = [i18nc("@option file type (mime type) for m3u, m3u8 playlist file formats; do not translate *.m3u*", "m3u8, m3u Playlist File (*.m3u*)"), i18nc("@option file type (mime type) for pls playlist file formats; do not translate *.pls", "pls Playlist File (*.pls)")]
            fileDialog.defaultSuffix = 'm3u8'
            fileDialog.fileMode = FileDialog.SaveFile
            fileDialog.file = ''
            fileDialog.open()
        }
        function loadPlaylist() {
            fileDialog.nameFilters = [i18nc("@option file type (mime type) for m3u, m3u8 and pls playlist file formats; do not translate *.m3u8 *.m3u *.pls", "m3u8, m3u, pls Playlist File (*.m3u8 *.m3u *.pls)")]
            fileDialog.fileMode = FileDialog.OpenFile
            fileDialog.file = ''
            fileDialog.open()
        }

        folder: StandardPaths.writableLocation(StandardPaths.MusicLocation)

        onAccepted: {
            if (fileMode === FileDialog.SaveFile) {
                if (!ElisaApplication.mediaPlayListProxyModel.savePlayList(fileDialog.file)) {
                    showPassiveNotification(i18nc("@label", "Saving failed"), 7000, i18nc("@action:button", "Retry"), () => savePlaylistButton.clicked())
                }
            } else {
                ElisaApplication.mediaPlayListProxyModel.loadPlayList(fileDialog.file)
            }
        }
    }

    Settings {
        id: persistentSettings

        property int x
        property int y
        property int width : 900
        property int height : 650

        property var playListState

        property var audioPlayerState

        property double playControlItemVolume : 100.0
        property bool playControlItemMuted : false

        property bool expandedFilterView: false

        property bool showPlaylist: true

        property bool headerBarIsMaximized: false
        property real headerBarHeight : 100000.0

        property bool nowPlayingPreferLyric: false
    }

    Connections {
        target: Qt.application
        function onAboutToQuit() {
            persistentSettings.x = mainWindow.x;
            persistentSettings.y = mainWindow.y;
            persistentSettings.width = mainWindow.width;
            persistentSettings.height = mainWindow.height;

            persistentSettings.playListState = ElisaApplication.mediaPlayListProxyModel.persistentState;
            persistentSettings.audioPlayerState = ElisaApplication.audioControl.persistentState

            persistentSettings.playControlItemVolume = mediaPlayerControl.playerControl.volume
            persistentSettings.playControlItemMuted = mediaPlayerControl.playerControl.muted

            persistentSettings.showPlaylist = contentView.showPlaylist

            if (Kirigami.Settings.isMobile) {
                persistentSettings.headerBarIsMaximized = mobileFooterBarLoader.item.isMaximized
            } else {
                persistentSettings.headerBarIsMaximized = headerBarLoader.item.isMaximized
            }
        }
    }

    Loader {
        id: mprisloader
        active: false

        sourceComponent:  PlatformIntegration {
            id: platformInterface

            playListModel: ElisaApplication.mediaPlayListProxyModel
            audioPlayerManager: ElisaApplication.audioControl
            player: ElisaApplication.audioPlayer
            headerBarManager: ElisaApplication.manageHeaderBar
            manageMediaPlayerControl: ElisaApplication.playerControl
            showProgressOnTaskBar: ElisaApplication.showProgressOnTaskBar
            showSystemTrayIcon: ElisaApplication.showSystemTrayIcon
            elisaMainWindow: mainWindow

            onRaisePlayer: function() {
                mainWindow.visible = true
                mainWindow.raise()
                mainWindow.requestActivate()
            }
        }
    }

    Connections {
        target: ElisaApplication.audioPlayer
        function onVolumeChanged() {
            if (mediaPlayerControl !== null) {
                mediaPlayerControl.playerControl.volume = ElisaApplication.audioPlayer.volume
            }
        }
        function onMutedChanged() {
            if (mediaPlayerControl !== null) {
                mediaPlayerControl.playerControl.muted = ElisaApplication.audioPlayer.muted
            }
        }
    }

    // mobile footer bar
    Loader {
        id: mobileFooterBarLoader
        anchors.fill: parent

        active: Kirigami.Settings.isMobile
        visible: active

        // footer bar fills the whole page, so only be in front of the main view when it is opened
        // otherwise, it captures all mouse/touch events on the main view
        z: (!item || item.contentY === 0) ? (mainWindow.layerOnTop ? -1 : 0) : 999

        sourceComponent: MobileFooterBar {
            id: mobileFooterBar
            contentHeight: mainWindow.height * 2

            focus: true

            album: (ElisaApplication.manageHeaderBar.album !== undefined ? ElisaApplication.manageHeaderBar.album : '')
            title: ElisaApplication.manageHeaderBar.title
            artist: (ElisaApplication.manageHeaderBar.artist !== undefined ? ElisaApplication.manageHeaderBar.artist : '')
            albumArtist: (ElisaApplication.manageHeaderBar.albumArtist !== undefined ? ElisaApplication.manageHeaderBar.albumArtist : '')
            image: ElisaApplication.manageHeaderBar.image
            albumID: ElisaApplication.manageHeaderBar.albumId

            ratingVisible: false

            // since we have multiple volume bars, and only one is linked directly to audio, sync the other one (trackControl)
            Binding on playerControl.volume {
                when: mobileFooterBar.trackControl.volumeSlider.moved
                value: mobileFooterBar.trackControl.volume
            }
            Component.onCompleted: {
                trackControl.volume = Qt.binding(() => mobileFooterBar.playerControl.volume);
            }

            onOpenArtist: { contentView.openArtist(artist) }
            onOpenNowPlaying: { contentView.openNowPlaying() }
            onOpenAlbum: { contentView.openAlbum(album, albumArtist, image, albumID) }
        }
    }

    Rectangle {
        id: mainContent

        visible: !mainWindow.layerOnTop

        color: myPalette.base
        anchors.fill: parent
        anchors.bottomMargin: Kirigami.Settings.isMobile ? elisaTheme.mediaPlayerControlHeight : 0


        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // desktop header bar
            Loader {
                id: headerBarLoader
                active: !Kirigami.Settings.isMobile
                visible: active

                Layout.minimumHeight: persistentSettings.isMaximized ? Layout.maximumHeight : elisaTheme.mediaPlayerControlHeight
                Layout.maximumHeight: persistentSettings.isMaximized ? Layout.maximumHeight : Math.round(mainWindow.height * 0.2 + elisaTheme.mediaPlayerControlHeight)
                Layout.fillWidth: true
                Layout.preferredHeight: status === Loader.Ready ? item.handlePosition : normalHeight

                // height when HeaderBar is not maximized
                property int normalHeight : persistentSettings.headerBarHeight

                Component.onDestruction: {
                    // saving height in onAboutToQuit() leads to invalid values, so we do it here
                    if (!Kirigami.Settings.isMobile) {
                        if (headerBarLoader.item.isMaximized) {
                            persistentSettings.headerBarHeight = normalHeight
                        } else {
                            persistentSettings.headerBarHeight = Layout.preferredHeight
                        }
                    }
                }

                sourceComponent: HeaderBar {
                    id: headerBar

                    focus: true
                    transitionsEnabled: mainWindow.transitionsEnabled

                    album: (ElisaApplication.manageHeaderBar.album !== undefined ? ElisaApplication.manageHeaderBar.album : '')
                    title: ElisaApplication.manageHeaderBar.title
                    artist: (ElisaApplication.manageHeaderBar.artist !== undefined ? ElisaApplication.manageHeaderBar.artist : '')
                    albumArtist: (ElisaApplication.manageHeaderBar.albumArtist !== undefined ? ElisaApplication.manageHeaderBar.albumArtist : '')
                    image: ElisaApplication.manageHeaderBar.image
                    albumID: ElisaApplication.manageHeaderBar.albumId
                    handlePosition: persistentSettings.headerBarHeight

                    ratingVisible: false

                    playerControl.isMaximized: persistentSettings.headerBarIsMaximized
                    onOpenArtist: contentView.openArtist(artist)
                    onOpenNowPlaying: contentView.openNowPlaying()
                    onOpenAlbum: contentView.openAlbum(album, albumArtist, image, albumID)

                    // animations
                    StateGroup {
                        id: mainWindowState
                        states: [
                            State {
                                name: "headerBarIsNormal"
                                when: !headerBar.isMaximized
                                changes: [
                                    PropertyChanges {
                                        target: mainWindow
                                        minimumHeight: mainWindow.minHeight * 1.5
                                        explicit: true
                                    },
                                    PropertyChanges {
                                        target: headerBarLoader
                                        Layout.preferredHeight: headerBar.handlePosition
                                    }
                                ]
                            },
                            State {
                                name: "headerBarIsMaximized"
                                // Workaround: only do this when transitions are enabled, or the playlist layout will be messed up
                                when: headerBar.isMaximized && transitionsEnabled
                                changes: [
                                    PropertyChanges {
                                        target: mainWindow
                                        minimumHeight: mainWindow.minHeight
                                        explicit: true
                                    },
                                    PropertyChanges {
                                        target: headerBarLoader
                                        Layout.minimumHeight: mainWindow.height
                                        Layout.maximumHeight: mainWindow.height
                                        Layout.preferredHeight: Layout.maximumHeight
                                    },
                                    PropertyChanges {
                                        target: playlistDrawer
                                        collapsed: true
                                        visible: false
                                        drawerOpen: false
                                        handleVisible: false
                                    },
                                    StateChangeScript {
                                        script: headerBarLoader.normalHeight = headerBarLoader.height
                                    }
                                ]
                            }
                        ]
                        transitions: Transition {
                            enabled: mainWindow.transitionsEnabled
                            NumberAnimation {
                                properties: "Layout.minimumHeight, Layout.maximumHeight, minimumHeight"
                                easing.type: Easing.InOutQuad
                                duration: Kirigami.Units.longDuration
                            }
                        }
                    }
                }
            }

            ContentView {
                id: contentView
                Layout.fillHeight: true
                Layout.fillWidth: true
                showPlaylist: persistentSettings.showPlaylist
                showExpandedFilterView: persistentSettings.expandedFilterView
                playlistDrawer: playlistDrawer
                initialIndex: ElisaApplication.initialViewIndex
            }

            FooterBar {
                Layout.fillWidth: true
            }
        }
    }

    // capture mouse events behind flickable when it is open
    MouseArea {
        visible: Kirigami.Settings.isMobile && mobileFooterBarLoader.item.contentY !== 0 // only capture when the mobile footer panel is open
        anchors.fill: mobileFooterBarLoader
        preventStealing: true
        onClicked: mouse => mouse.accepted = true
    }

    Component.onCompleted:
    {
        ElisaApplication.initialize()
        ElisaApplication.activateColorScheme(ElisaConfigurationDialog.colorScheme)

        if (persistentSettings.playListState) {
            ElisaApplication.mediaPlayListProxyModel.persistentState = persistentSettings.playListState
        }

        if (persistentSettings.audioPlayerState) {
            ElisaApplication.audioControl.persistentState = persistentSettings.audioPlayerState
        }

        // it seems the header/footer bars load before settings are loaded, so we need to set their settings here
        mediaPlayerControl.playerControl.volume = persistentSettings.playControlItemVolume;
        mediaPlayerControl.playerControl.muted = persistentSettings.playControlItemMuted;

        ElisaApplication.audioPlayer.muted = Qt.binding(() => mediaPlayerControl.playerControl.muted);
        ElisaApplication.audioPlayer.volume = Qt.binding(() => mediaPlayerControl.playerControl.volume);

        mprisloader.active = true

        if (!ElisaApplication.openFiles(elisaStartupArguments)) {
            showPassiveNotification(i18nc("@info:status", "Could not load some files. Elisa can only open audio and playlist files."), 7000, "", function() {})
        }

        // use global drawer on mobile
        if (Kirigami.Settings.isMobile) {
            globalDrawer = contentView.sidebar;
        }
    }
}
