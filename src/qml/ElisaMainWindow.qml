/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.3
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
            showPassiveNotification(i18nc("@label", "Loading failed"), 7000, i18nc("@action:button", "Retry"), function() { loadPlaylistButton.clicked(); })
        }

        function onDisplayUndoNotification() {
            showPassiveNotification(i18nc("@label", "Playlist cleared"), 7000, i18nc("@action:button", "Undo"), function() { ElisaApplication.mediaPlayListProxyModel.undoClearPlayList(); })
        }
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: playlistDrawer
        handleClosedIcon.source: "view-media-playlist"
        handleOpenIcon.source: "view-right-close"

        handle.visible: !Kirigami.Settings.isMobile
        Component.onCompleted: close() // drawer is opened when the layout is narrow, we want it closed

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

    minimumWidth: Kirigami.Settings.isMobile ? 320 : 620
    property int minHeight: 320

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    x: persistentSettings.x
    y: persistentSettings.y
    width: persistentSettings.width
    height: persistentSettings.height

    title: ElisaApplication.manageHeaderBar.title ? i18nc("@title:window", "%1 — Elisa", ElisaApplication.manageHeaderBar.title) : i18nc("@title:window", "Elisa")

    readonly property int initialViewIndex: 3

    property var goBackAction: ElisaApplication.action("go_back")
    property var seekAction: ElisaApplication.action("Seek")
    property var scrubAction: ElisaApplication.action("Scrub")
    property var nextTrackAction : ElisaApplication.action("NextTrack")
    property var previousTrackAction: ElisaApplication.action("PreviousTrack")
    property var playPauseAction: ElisaApplication.action("Play-Pause")
    property var findAction: ElisaApplication.action("edit_find")
    property var togglePartyModeAction: ElisaApplication.action("togglePartyMode")

    property var mediaPlayerControl: Kirigami.Settings.isMobile ? mobileFooterBarLoader.item : headerBarLoader.item
    property var fileDialog: fileDialog

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

        onAccepted:
        {
            if (fileMode === FileDialog.SaveFile) {
                if (!ElisaApplication.mediaPlayListProxyModel.savePlayList(fileDialog.file)) {
                    showPassiveNotification(i18nc("@label", "Saving failed"), 7000, i18nc("@action:button", "Retry"), function() { savePlaylistButton.clicked(); })
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
            if (mediaPlayerControl != null) {
                mediaPlayerControl.playerControl.volume = ElisaApplication.audioPlayer.volume
            }
        }
        function onMutedChanged() {
            if (mediaPlayerControl != null) {
                mediaPlayerControl.playerControl.muted = ElisaApplication.audioPlayer.muted
            }
        }
    }

    // track import notification
    TrackImportNotification {
        z: 2
        id: importedTracksCountNotification

        anchors {
            right: mainContent.right
            top: mainContent.top
            rightMargin: Kirigami.Units.largeSpacing * 2
            topMargin: Kirigami.Units.largeSpacing * 3
        }
    }

    Binding {
        id: indexerBusyBinding

        target: importedTracksCountNotification
        property: 'indexingRunning'
        value: ElisaApplication.musicManager.indexerBusy
        when: ElisaApplication.musicManager !== undefined
    }

    Binding {
        target: importedTracksCountNotification
        property: 'importedTracksCount'
        value: ElisaApplication.musicManager.importedTracksCount
        when: ElisaApplication.musicManager !== undefined
    }

    // mobile footer bar
    Loader {
        id: mobileFooterBarLoader
        anchors.fill: parent

        active: Kirigami.Settings.isMobile
        visible: active

        // footer bar fills the whole page, so only be in front of the main view when it is opened
        // otherwise, it captures all mouse/touch events on the main view
        z: (!item || item.contentY == 0) ? (mainWindow.layerOnTop ? -1 : 0) : 999

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
                trackControl.volume = Qt.binding(function() { return mobileFooterBar.playerControl.volume })
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
        anchors.bottomMargin: Kirigami.Settings.isMobile? elisaTheme.mediaPlayerControlHeight : 0

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // desktop header bar
            Loader {
                id: headerBarLoader
                active: !Kirigami.Settings.isMobile
                visible: active

                Layout.minimumHeight: Math.round(mainWindow.height * 0.2 + elisaTheme.mediaPlayerControlHeight)
                Layout.maximumHeight: Layout.minimumHeight
                Layout.fillWidth: true

                sourceComponent: HeaderBar {
                    id: headerBar

                    focus: true

                    album: (ElisaApplication.manageHeaderBar.album !== undefined ? ElisaApplication.manageHeaderBar.album : '')
                    title: ElisaApplication.manageHeaderBar.title
                    artist: (ElisaApplication.manageHeaderBar.artist !== undefined ? ElisaApplication.manageHeaderBar.artist : '')
                    albumArtist: (ElisaApplication.manageHeaderBar.albumArtist !== undefined ? ElisaApplication.manageHeaderBar.albumArtist : '')
                    image: ElisaApplication.manageHeaderBar.image
                    albumID: ElisaApplication.manageHeaderBar.albumId

                    ratingVisible: false

                    playerControl.isMaximized: persistentSettings.headerBarIsMaximized
                    onOpenArtist: { contentView.openArtist(artist) }
                    onOpenNowPlaying: { contentView.openNowPlaying() }
                    onOpenAlbum: { contentView.openAlbum(album, albumArtist, image, albumID) }

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
                                        Layout.minimumHeight: Math.round(mainWindow.height * 0.2 + elisaTheme.mediaPlayerControlHeight)
                                        Layout.maximumHeight: Layout.minimumHeight
                                    }
                                ]
                            },
                            State {
                                name: "headerBarIsMaximized"
                                when: headerBar.isMaximized
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
                                    },
                                    PropertyChanges {
                                        target: playlistDrawer
                                        collapsed: true
                                        visible: false
                                        drawerOpen: false
                                        handleVisible: false
                                    }
                                ]
                            }
                        ]
                        transitions: Transition {
                            NumberAnimation {
                                properties: "Layout.minimumHeight, Layout.maximumHeight, minimumHeight"
                                easing.type: Easing.InOutQuad
                                duration: Kirigami.Units.longDuration
                            }
                        }
                    }
                }
            }

            Kirigami.Separator {
                visible: !Kirigami.Settings.isMobile
                Layout.fillWidth: true
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
        }
    }

    // capture mouse events behind flickable when it is open
    MouseArea {
        visible: Kirigami.Settings.isMobile && mobileFooterBarLoader.item.contentY != 0 // only capture when the mobile footer panel is open
        anchors.fill: mobileFooterBarLoader
        preventStealing: true
        onClicked: mouse.accepted = true
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

        ElisaApplication.mediaPlayListProxyModel.shufflePlayList = Qt.binding(function() { return mediaPlayerControl.playerControl.shuffle })
        ElisaApplication.mediaPlayListProxyModel.repeatMode = Qt.binding(function() { return mediaPlayerControl.playerControl.repeat })
        ElisaApplication.audioPlayer.muted = Qt.binding(function() { return mediaPlayerControl.playerControl.muted })
        ElisaApplication.audioPlayer.volume = Qt.binding(function() { return mediaPlayerControl.playerControl.volume })

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
