/*
   SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window
import Qt.labs.platform
import org.kde.elisa
import org.kde.ki18n
import org.kde.kirigami as Kirigami

MenuBar {
    Menu {
        title: KI18n.i18nc("@title:menu", "File")

        MenuItem {
            text: KI18n.i18nc("@action:inmenu", "Save Playlist…")
            icon.name: "document-save"
            onTriggered: {
                mainWindow.fileDialog.savePlaylist()
            }
        }

        MenuItem {
            text: KI18n.i18nc("@action:inmenu", "Open Playlist…")
            icon.name: "document-open"
            onTriggered: {
                mainWindow.fileDialog.loadPlaylist()
            }
        }

        MenuItem {
            text: KI18n.i18nc("@action:inmenu", "Reset Database and Re-Scan Everything")
            icon.name: "view-refresh"
            onTriggered: ElisaApplication.musicManager.scanCollection(MusicListenersManager.Hard)
        }

        MenuSeparator {
        }

        NativeMenuItemFromAction {
            action: Kirigami.Action {
                fromQAction: ElisaApplication.action("options_configure")
            }
        }

        MenuSeparator {
        }

        NativeMenuItemFromAction {
            action: Kirigami.Action {
                fromQAction: ElisaApplication.action("file_quit")
            }
        }
    }
    Menu {
        title: KI18n.i18nc("@title:menu", "View")

        MenuItem {
            text: KI18n.i18nc("@action:inmenu", "Toggle Party Mode")
            icon.name: mediaPlayerControl && mediaPlayerControl.isMaximized ? "arrow-up" : "expand"
            onTriggered: mediaPlayerControl.isMaximized = !mediaPlayerControl.isMaximized
        }

        MenuItem {
            text: mainWindow.visibility === Window.FullScreen ? KI18n.i18nc("@action:inmenu", "Exit Full Screen") : KI18n.i18nc("@action:inmenu", "Enter Full Screen")
            icon.name: mainWindow.visibility === Window.FullScreen ? "view-restore" : "view-fullscreen"
            onTriggered: mainWindow.visibility === Window.FullScreen ? mainWindow.restorePreviousStateBeforeFullScreen() : mainWindow.goFullScreen()
        }

        Kirigami.Action {
            fromQAction: ElisaApplication.action("toggle_playlist")
            text: contentView.showPlaylist ? KI18n.i18nc("@action:inmenu", "Hide Playlist") : KI18n.i18nc("@action:inmenu", "Show Playlist")
            icon.name: contentView.showPlaylist ? "show-menu" : "view-media-playlist"
        }
    }
    Menu {
        title: KI18n.i18nc("@title:menu", "Controls")

        MenuItem {
            text: ElisaApplication.audioControl.playerPlaybackState === 1 ? KI18n.i18nc("@action:inmenu", "Pause") : KI18n.i18nc("@action:inmenu", "Play")
            icon.name: ElisaApplication.audioControl.playerPlaybackState === 1 ? "media-playback-pause" : "media-playback-start"
            onTriggered: ElisaApplication.audioControl.playPause()
        }

        MenuItem {
            enabled: ElisaApplication.audioControl.playerPlaybackState !== 0
            text: KI18n.i18nc("@action:inmenu", "Stop")
            icon.name: "media-playback-stop"
            onTriggered: ElisaApplication.audioControl.stop()
        }

        MenuSeparator {
        }

        MenuItem {
            text: KI18n.i18nc("@action:inmenu", "Increase Volume")
            enabled: ElisaApplication.audioPlayer.volume < 100.0
            onTriggered: ElisaApplication.audioPlayer.setVolume(ElisaApplication.audioPlayer.volume + 5)
        }
        MenuItem {
            text: KI18n.i18nc("@action:inmenu", "Decrease Volume")
            enabled: ElisaApplication.audioPlayer.volume > 0.0
            onTriggered: ElisaApplication.audioPlayer.setVolume(ElisaApplication.audioPlayer.volume - 5)
        }

        MenuSeparator {
        }

        Menu {
            id: shuffleMenu
            title: KI18n.i18nc("@title:menu", "Shuffle")

            MenuItemGroup {
                items: shuffleMenu.items
            }

            NativeGlobalMenuShuffleModeItem {
                text: KI18n.i18nc("@action:inmenu shuffle all tracks in playlist", "Track")
                mode: MediaPlayListProxyModel.Track
            }
            NativeGlobalMenuShuffleModeItem {
                text: KI18n.i18nc("@action:inmenu shuffle albums in playlist", "Album")
                mode: MediaPlayListProxyModel.Album
            }
            NativeGlobalMenuShuffleModeItem {
                text: KI18n.i18nc("@action:inmenu disable shuffle mode", "Off")
                mode: MediaPlayListProxyModel.NoShuffle
            }
        }

        Menu {
            id: repeatMenu
            title: KI18n.i18nc("@title:menu", "Repeat")

            MenuItemGroup {
                items: repeatMenu.items
            }

            NativeGlobalMenuPlaylistModeItem {
                text: KI18n.i18nc("@action:inmenu repeat all songs in this playlist", "Playlist")
                mode: MediaPlayListProxyModel.Playlist
            }
            NativeGlobalMenuPlaylistModeItem {
                text: KI18n.i18nc("@action:inmenu repeat this one song", "One")
                mode: MediaPlayListProxyModel.One
            }
            NativeGlobalMenuPlaylistModeItem {
                text: KI18n.i18nc("@action:inmenu no repeat mode set; stop playback after last song in the playlist", "None")
                mode: MediaPlayListProxyModel.None
            }
        }
    }
    Menu {
        title: KI18n.i18nc("@title:menu", "Help")

        NativeMenuItemFromAction {
            action: Kirigami.Action {
                fromQAction: ElisaApplication.action("help_about_app")
            }
        }

        NativeMenuItemFromAction {
            action: Kirigami.Action {
                fromQAction: ElisaApplication.action("help_about_kde")
            }
        }

        NativeMenuItemFromAction {
            action: Kirigami.Action {
                fromQAction: ElisaApplication.action("help_report_bug")
            }
        }
    }
}
