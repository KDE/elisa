/*
   SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Window 2.7
import Qt.labs.platform 1.1
import org.kde.elisa 1.0

MenuBar {
    Menu {
        title: i18nc("@title:menu", "File")

        MenuItem {
            text: i18nc("@action:inmenu", "Save Playlist…")
            icon.name: "document-save"
            onTriggered: {
                mainWindow.fileDialog.savePlaylist()
            }
        }

        MenuItem {
            text: i18nc("@action:inmenu", "Open Playlist…")
            icon.name: "document-open"
            onTriggered: {
                mainWindow.fileDialog.loadPlaylist()
            }
        }

        MenuItem {
            text: i18nc("@action:inmenu", "Reset Database and Re-Scan Everything")
            icon.name: "view-refresh"
            onTriggered: ElisaApplication.musicManager.scanCollection(MusicListenersManager.Hard)
        }

        MenuSeparator {
        }

        NativeMenuItemFromAction {
            elisaAction: "options_configure"
        }

        MenuSeparator {
        }

        NativeMenuItemFromAction {
            elisaAction: "file_quit"
        }
    }
    Menu {
        title: i18nc("@title:menu", "View")

        MenuItem {
            text: i18nc("@action:inmenu", "Toggle Party Mode")
            icon.name: mediaPlayerControl && mediaPlayerControl.isMaximized ? "arrow-up" : "expand"
            onTriggered: mediaPlayerControl.isMaximized = !mediaPlayerControl.isMaximized
        }

        MenuItem {
            text: mainWindow.visibility === Window.FullScreen ? i18nc("@action:inmenu", "Exit Full Screen") : i18nc("@action:inmenu", "Enter Full Screen")
            icon.name: mainWindow.visibility === Window.FullScreen ? "view-restore" : "view-fullscreen"
            onTriggered: mainWindow.visibility === Window.FullScreen ? mainWindow.restorePreviousStateBeforeFullScreen() : mainWindow.goFullScreen()
        }

        MenuItem {
            text: contentView.showPlaylist ? i18nc("@action:inmenu", "Hide Playlist") : i18nc("@action:inmenu", "Show Playlist")
            icon.name: contentView.showPlaylist ? "show-menu" : "view-media-playlist"
            onTriggered: contentView.showPlaylist = !contentView.showPlaylist
        }
    }
    Menu {
        title: i18nc("@title:menu", "Controls")

        MenuItem {
            text: ElisaApplication.audioControl.playerPlaybackState === 1 ? i18nc("@action:inmenu", "Pause") : i18nc("@action:inmenu", "Play")
            icon.name: ElisaApplication.audioControl.playerPlaybackState === 1 ? "media-playback-pause" : "media-playback-start"
            onTriggered: ElisaApplication.audioControl.playPause()
        }

        MenuItem {
            enabled: ElisaApplication.audioControl.playerPlaybackState !== 0
            text: i18nc("@action:inmenu", "Stop")
            icon.name: "media-playback-stop"
            onTriggered: ElisaApplication.audioControl.stop()
        }

        MenuSeparator {
        }

        MenuItem {
            text: i18nc("@action:inmenu", "Increase Volume")
            enabled: ElisaApplication.audioPlayer.volume < 100.0
            onTriggered: ElisaApplication.audioPlayer.setVolume(ElisaApplication.audioPlayer.volume + 5)
        }
        MenuItem {
            text: i18nc("@action:inmenu", "Decrease Volume")
            enabled: ElisaApplication.audioPlayer.volume > 0.0
            onTriggered: ElisaApplication.audioPlayer.setVolume(ElisaApplication.audioPlayer.volume - 5)
        }

        MenuSeparator {
        }

        Menu {
            id: shuffleMenu
            title: i18nc("@title:menu", "Shuffle")

            MenuItemGroup {
                items: shuffleMenu.items
            }

            MenuItem {
                text: i18nc("@action:inmenu shuffle mode is turned on", "On")
                checkable: true
                checked: ElisaApplication.mediaPlayListProxyModel.shufflePlayList
                onTriggered: ElisaApplication.mediaPlayListProxyModel.shufflePlayList = true
            }
            MenuItem {
                text: i18nc("@action:inmenu shuffle mode is turned off", "Off")
                checkable: true
                checked: !ElisaApplication.mediaPlayListProxyModel.shufflePlayList
                onTriggered: ElisaApplication.mediaPlayListProxyModel.shufflePlayList = false
            }
        }

        Menu {
            id: repeatMenu
            title: i18nc("@title:menu", "Repeat")

            MenuItemGroup {
                items: repeatMenu.items
            }

            NativeGlobalMenuPlaylistModeItem {
                text: i18nc("@action:inmenu repeat all songs in this playlist", "Playlist")
                mode: MediaPlayListProxyModel.Playlist
            }
            NativeGlobalMenuPlaylistModeItem {
                text: i18nc("@action:inmenu repeat this one song", "One")
                mode: MediaPlayListProxyModel.One
            }
            NativeGlobalMenuPlaylistModeItem {
                text: i18nc("@action:inmenu no repeat mode set; stop playback after last song in the playlist", "None")
                mode: MediaPlayListProxyModel.None
            }
        }
    }
    Menu {
        title: i18nc("@title:menu", "Help")

        NativeMenuItemFromAction {
            elisaAction: "help_contents"
        }

        MenuSeparator {
        }

        NativeMenuItemFromAction {
            elisaAction: "help_about_app"
        }

        NativeMenuItemFromAction {
            elisaAction: "open_about_kde_page"
        }

        NativeMenuItemFromAction {
            elisaAction: "help_report_bug"
        }
    }
}
