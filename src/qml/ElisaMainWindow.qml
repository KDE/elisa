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
import org.kde.elisa.host 1.0
import Qt.labs.settings 1.0
import Qt.labs.platform 1.1

Kirigami.ApplicationWindow {
    id: mainWindow

    visible: true

    contextDrawer: Kirigami.ContextDrawer {
        id: playlistDrawer
        handleClosedIcon.source: "view-media-playlist"
        handleOpenIcon.source: "view-right-close"
        // without this drawer button is never shown
        enabled: true
        MediaPlayListView {
            id: playList
            anchors.fill: parent
            onStartPlayback: ElisaApplication.audioControl.ensurePlay()
            onPausePlayback: ElisaApplication.audioControl.playPause()
        }
    }

    minimumWidth: 590
    property int minHeight: 320

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    x: persistentSettings.x
    y: persistentSettings.y
    width: persistentSettings.width
    height: persistentSettings.height

    title: i18n("Elisa")

    Accessible.role: Accessible.Application
    Accessible.name: title

    readonly property int initialViewIndex: 3

    property var goBackAction: ElisaApplication.action("go_back")
    property var seekAction: ElisaApplication.action("Seek")
    property var scrubAction: ElisaApplication.action("Scrub")
    property var nextTrackAction : ElisaApplication.action("NextTrack")
    property var previousTrackAction: ElisaApplication.action("PreviousTrack")
    property var playPauseAction: ElisaApplication.action("Play-Pause")
    property var findAction: ElisaApplication.action("edit_find")

    Action {
        shortcut: goBackAction.shortcut
        onTriggered: contentView.goBack()
    }

    Action {
        shortcut: seekAction.shortcut
        onTriggered: ElisaApplication.audioControl.seek(headerBar.playerControl.position + 10000)
    }

    Action {
        shortcut: scrubAction.shortcut
        onTriggered: ElisaApplication.audioControl.seek(headerBar.playerControl.position - 10000)
    }

    Action {
        shortcut: nextTrackAction.shortcut
        onTriggered: ElisaApplication.mediaPlayListProxyModel.skipPreviousTrack(ElisaApplication.audioPlayer.position)
    }

    Action {
        shortcut: previousTrackAction.shortcut
        onTriggered: ElisaApplication.mediaPlayListProxyModel.skipNextTrack(ElisaApplication.audioPlayer.position)
    }

    Action {
        shortcut: playPauseAction.shortcut
        onTriggered: ElisaApplication.audioControl.playPause()
    }

    Action {
        shortcut: findAction.shortcut
        onTriggered: persistentSettings.expandedFilterView = !persistentSettings.expandedFilterView
    }

    ApplicationMenu {
        id: applicationMenu
    }

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
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

        property int initialIndex
    }

    Connections {
        target: headerBar.playerControl
        function onOpenMenu() {
            if (applicationMenu.visible) {
                applicationMenu.close()
            } else {
                applicationMenu.popup(mainWindow.width - applicationMenu.width, headerBar.height)
            }
        }
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

            persistentSettings.playControlItemVolume = headerBar.playerControl.volume
            persistentSettings.playControlItemMuted = headerBar.playerControl.muted

            persistentSettings.showPlaylist = contentView.showPlaylist

            persistentSettings.headerBarIsMaximized = headerBar.isMaximized

            persistentSettings.initialIndex = contentView.currentViewIndex
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

            function onRaisePlayer() {
                mainWindow.visible = true
                mainWindow.raise()
                mainWindow.requestActivate()
            }

        }
    }

    Connections {
        target: ElisaApplication.audioPlayer
        function onVolumeChanged() {
            headerBar.playerControl.volume = ElisaApplication.audioPlayer.volume
        }
        function onMutedChanged() {
            headerBar.playerControl.muted = ElisaApplication.audioPlayer.muted
        }
    }

    Rectangle {
        color: myPalette.base
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            HeaderBar {
                id: headerBar

                focus: true

                Layout.minimumHeight: mainWindow.height * 0.2 + elisaTheme.mediaPlayerControlHeight
                Layout.maximumHeight: mainWindow.height * 0.2 + elisaTheme.mediaPlayerControlHeight
                Layout.fillWidth: true

                tracksCount: ElisaApplication.mediaPlayListProxyModel.remainingTracks
                album: (ElisaApplication.manageHeaderBar.album !== undefined ? ElisaApplication.manageHeaderBar.album : '')
                title: ElisaApplication.manageHeaderBar.title
                artist: (ElisaApplication.manageHeaderBar.artist !== undefined ? ElisaApplication.manageHeaderBar.artist : '')
                albumArtist: (ElisaApplication.manageHeaderBar.albumArtist !== undefined ? ElisaApplication.manageHeaderBar.albumArtist : '')
                image: ElisaApplication.manageHeaderBar.image
                albumID: ElisaApplication.manageHeaderBar.albumId

                ratingVisible: false

                playerControl.duration: ElisaApplication.audioControl.audioDuration
                playerControl.seekable: ElisaApplication.audioPlayer.seekable

                playerControl.volume: persistentSettings.playControlItemVolume
                playerControl.muted: persistentSettings.playControlItemMuted
                playerControl.position: ElisaApplication.audioControl.playerPosition
                playerControl.skipBackwardEnabled: ElisaApplication.playerControl.skipBackwardControlEnabled
                playerControl.skipForwardEnabled: ElisaApplication.playerControl.skipForwardControlEnabled
                playerControl.playEnabled: ElisaApplication.playerControl.playControlEnabled
                playerControl.isPlaying: ElisaApplication.playerControl.musicPlaying

                playerControl.repeat: ElisaApplication.mediaPlayListProxyModel.repeatPlay
                playerControl.shuffle: ElisaApplication.mediaPlayListProxyModel.shufflePlayList

                playerControl.onSeek: ElisaApplication.audioPlayer.seek(position)

                playerControl.onPlay: ElisaApplication.audioControl.playPause()
                playerControl.onPause: ElisaApplication.audioControl.playPause()
                playerControl.onPlayPrevious: ElisaApplication.mediaPlayListProxyModel.skipPreviousTrack(ElisaApplication.audioPlayer.position)
                playerControl.onPlayNext: ElisaApplication.mediaPlayListProxyModel.skipNextTrack()

                playerControl.isMaximized: persistentSettings.headerBarIsMaximized
                onOpenArtist: { contentView.openArtist(artist) }
                onOpenNowPlaying: { contentView.openNowPlaying() }
                onOpenAlbum: { contentView.openAlbum(album, albumArtist, image, albumID) }

                TrackImportNotification {
                    id: importedTracksCountNotification

                    anchors
                    {
                        right: headerBar.right
                        top: headerBar.top
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
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            ContentView {
                id: contentView
                Layout.fillHeight: true
                Layout.fillWidth: true
                showPlaylist: persistentSettings.showPlaylist
                showExpandedFilterView: persistentSettings.expandedFilterView
                playlistDrawer: playlistDrawer
                initialIndex: persistentSettings.value('initialIndex', initialViewIndex)
            }
        }
    }

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
                        target: headerBar
                        Layout.minimumHeight: mainWindow.height * 0.2 + elisaTheme.mediaPlayerControlHeight
                        Layout.maximumHeight: mainWindow.height * 0.2 + elisaTheme.mediaPlayerControlHeight
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
                        target: headerBar
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

        ElisaApplication.mediaPlayListProxyModel.shufflePlayList = Qt.binding(function() { return headerBar.playerControl.shuffle })
        ElisaApplication.mediaPlayListProxyModel.repeatPlay = Qt.binding(function() { return headerBar.playerControl.repeat })
        ElisaApplication.audioPlayer.muted = Qt.binding(function() { return headerBar.playerControl.muted })
        ElisaApplication.audioPlayer.volume = Qt.binding(function() { return headerBar.playerControl.volume })

        mprisloader.active = true

        ElisaApplication.arguments = ElisaArguments.arguments
    }
}
