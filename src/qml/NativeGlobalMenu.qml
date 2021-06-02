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
        title: i18nc("menu category", "File")

        MenuItem {
            text: i18nc("menu entry", "Save Playlist...")
            iconName: "document-save"
            onTriggered: ElisaApplication.musicManager.resetMusicData()
        }

        MenuItem {
            text: i18nc("menu entry", "Open Playlist...")
            iconName: "document-open"
            onTriggered: ElisaApplication.musicManager.resetMusicData()
        }

        MenuItem {
            text: i18nc("Refresh Music Collection application menu entry", "Refresh Music Collection")
            iconName: "view-refresh"
            onTriggered: ElisaApplication.musicManager.resetMusicData()
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
        title: i18nc("menu category", "View")

        MenuItem {
            text: i18nc("menu entry", "Toggle Party Mode")
            iconName: mediaPlayerControl && mediaPlayerControl.isMaximized ? "arrow-up" : "expand"
            onTriggered: mediaPlayerControl.isMaximized = !mediaPlayerControl.isMaximized
        }

        MenuItem {
            text: mainWindow.visibility == Window.FullScreen ? i18nc("menu entry", "Leave Fullscreen") : i18nc("menu entry", "Enter Fullscreen")
            iconName: "view-fullscreen"
            onTriggered: mainWindow.visibility == Window.FullScreen ? mainWindow.showNormal() : mainWindow.showFullScreen()
        }

        MenuItem {
            text: contentView.showPlaylist ? i18n("Hide Playlist") : i18n("Show Playlist")
            iconName: contentView.showPlaylist ? "show-menu" : "view-media-playlist"
            onTriggered: contentView.showPlaylist = !contentView.showPlaylist
        }
    }
    Menu {
        title: i18nc("menu category", "Controls")

        MenuItem {
            text: ElisaApplication.audioControl.playerPlaybackState == 1 ? i18n("Pause") : i18n("Play")
            iconName: ElisaApplication.audioControl.playerPlaybackState == 1 ? "media-playback-pause" : "media-playback-start"
            onTriggered: ElisaApplication.audioControl.playPause()
        }

        MenuItem {
            enabled: ElisaApplication.audioControl.playerPlaybackState != 0
            text: i18n("Stop")
            iconName: "media-playback-stop"
            onTriggered: ElisaApplication.audioControl.stop()
        }

        MenuSeparator {
        }

        MenuItem {
            text: i18n("Increase Volume")
            enabled: ElisaApplication.audioPlayer.volume < 100.0
            onTriggered: ElisaApplication.audioPlayer.setVolume(ElisaApplication.audioPlayer.volume + 5)
        }
        MenuItem {
            text: i18n("Decrease Volume")
            enabled: ElisaApplication.audioPlayer.volume > 100.0
            onTriggered: ElisaApplication.audioPlayer.setVolume(ElisaApplication.audioPlayer.volume - 5)
        }

        MenuSeparator {
        }

        Menu {
            id: shuffleMenu
            title: i18n("Shuffle")

            MenuItemGroup {
                items: shuffleMenu.items
            }

            MenuItem {
                text: i18n("On")
                checkable: true
                checked: ElisaApplication.mediaPlayListProxyModel.shufflePlayList
                onTriggered: ElisaApplication.mediaPlayListProxyModel.shufflePlayList = true
            }
            MenuItem {
                text: i18n("Off")
                checkable: true
                checked: !ElisaApplication.mediaPlayListProxyModel.shufflePlayList
                onTriggered: ElisaApplication.mediaPlayListProxyModel.shufflePlayList = false
            }
        }

        Menu {
            id: repeatMenu
            title: i18n("Repeat")

            MenuItemGroup {
                items: repeatMenu.items
            }

            NativeGlobalMenuPlaylistModeItem {
                text: i18n("Playlist")
                mode: MediaPlayListProxyModel.Playlist
            }
            NativeGlobalMenuPlaylistModeItem {
                text: i18n("One")
                mode: MediaPlayListProxyModel.One
            }
            NativeGlobalMenuPlaylistModeItem {
                text: i18n("None")
                mode: MediaPlayListProxyModel.None
            }
        }
    }
    Menu {
        title: i18nc("menu category", "Help")

        NativeMenuItemFromAction {
            elisaAction: "help_contents"
        }

        MenuSeparator {
        }

        NativeMenuItemFromAction {
            elisaAction: "help_about_app"
        }

        NativeMenuItemFromAction {
            elisaAction: "help_report_bug"
        }
    }
}
