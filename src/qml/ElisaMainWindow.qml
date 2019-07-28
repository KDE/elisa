/*
 * Copyright 2016-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import org.kde.elisa 1.0
import Qt.labs.settings 1.0

ApplicationWindow {
    id: mainWindow

    visible: true

    minimumWidth: contentView.showPlaylist ? 1100 : 700
    minimumHeight: 600

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    x: persistentSettings.x
    y: persistentSettings.y
    width: persistentSettings.width
    height: persistentSettings.height

    title: i18n("Elisa")

    Accessible.role: Accessible.Application
    Accessible.name: title

    property var goBackAction: elisa.action("go_back")
    property var seekAction: elisa.action("Seek")
    property var scrubAction: elisa.action("Scrub")
    property var playPauseAction: elisa.action("Play-Pause")
    property var findAction: elisa.action("edit_find")

    Action {
        shortcut: goBackAction.shortcut
        onTriggered: contentView.goBack()
    }

    Action {
        shortcut: seekAction.shortcut
        onTriggered: elisa.audioControl.seek(headerBar.playerControl.position + 10000)
    }

    Action {
        shortcut: scrubAction.shortcut
        onTriggered: elisa.audioControl.seek(headerBar.playerControl.position - 10000)
    }

    Action {
        shortcut: playPauseAction.shortcut
        onTriggered: elisa.audioControl.playPause()
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
        property int width : 1100
        property int height : 600

        property var playListState

        property var audioPlayerState

        property double playControlItemVolume : 100.0
        property bool playControlItemMuted : false

        property bool expandedFilterView: false

        property bool showPlaylist: true

        property bool headerBarIsMaximized: false
    }

    Connections {
        target: headerBar.playerControl
        onOpenMenu: applicationMenu.popup()
    }

    Connections {
        target: Qt.application
        onAboutToQuit:
        {
            persistentSettings.x = mainWindow.x;
            persistentSettings.y = mainWindow.y;
            persistentSettings.width = mainWindow.width;
            persistentSettings.height = mainWindow.height;

            persistentSettings.playListState = elisa.mediaPlayList.persistentState;
            persistentSettings.audioPlayerState = elisa.audioControl.persistentState

            persistentSettings.playControlItemVolume = headerBar.playerControl.volume
            persistentSettings.playControlItemMuted = headerBar.playerControl.muted

            persistentSettings.showPlaylist = contentView.showPlaylist

            persistentSettings.headerBarIsMaximized = headerBar.isMaximized
        }
    }

    Loader {
        id: mprisloader
        active: false

        sourceComponent:  PlatformIntegration {
            id: platformInterface

            playListModel: elisa.mediaPlayList
            audioPlayerManager: elisa.audioControl
            player: elisa.audioPlayer
            headerBarManager: elisa.manageHeaderBar
            manageMediaPlayerControl: elisa.playerControl
            onRaisePlayer: {
                mainWindow.show()
                mainWindow.raise()
                mainWindow.requestActivate()
            }

        }
    }

    Connections {
        target: elisa.audioPlayer
        onVolumeChanged: headerBar.playerControl.volume = elisa.audioPlayer.volume
        onMutedChanged: headerBar.playerControl.muted = elisa.audioPlayer.muted
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

                tracksCount: elisa.manageHeaderBar.remainingTracks
                album: elisa.manageHeaderBar.album
                title: elisa.manageHeaderBar.title
                artist: elisa.manageHeaderBar.artist
                albumArtist: elisa.manageHeaderBar.albumArtist
                image: elisa.manageHeaderBar.image
                albumID: elisa.manageHeaderBar.albumId

                ratingVisible: false

                playerControl.duration: elisa.audioPlayer.duration
                playerControl.seekable: elisa.audioPlayer.seekable

                playerControl.volume: persistentSettings.playControlItemVolume
                playerControl.muted: persistentSettings.playControlItemMuted
                playerControl.position: elisa.audioPlayer.position
                playerControl.skipBackwardEnabled: elisa.playerControl.skipBackwardControlEnabled
                playerControl.skipForwardEnabled: elisa.playerControl.skipForwardControlEnabled
                playerControl.playEnabled: elisa.playerControl.playControlEnabled
                playerControl.isPlaying: elisa.playerControl.musicPlaying

                playerControl.repeat: elisa.mediaPlayList.repeatPlay
                playerControl.shuffle: elisa.mediaPlayList.randomPlay

                playerControl.onSeek: elisa.audioPlayer.seek(position)

                playerControl.onPlay: elisa.audioControl.playPause()
                playerControl.onPause: elisa.audioControl.playPause()
                playerControl.onPlayPrevious: elisa.mediaPlayList.skipPreviousTrack()
                playerControl.onPlayNext: elisa.mediaPlayList.skipNextTrack()

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
                        rightMargin: elisaTheme.layoutHorizontalMargin * 1.75
                        topMargin: elisaTheme.layoutHorizontalMargin * 3
                    }
                }

                Binding {
                    id: indexerBusyBinding

                    target: importedTracksCountNotification
                    property: 'indexingRunning'
                    value: elisa.musicManager.indexerBusy
                    when: elisa.musicManager !== undefined
                }

                Binding {
                    target: importedTracksCountNotification
                    property: 'importedTracksCount'
                    value: elisa.musicManager.importedTracksCount
                    when: elisa.musicManager !== undefined
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: myPalette.mid
            }

            ContentView {
                id: contentView
                Layout.fillHeight: true
                Layout.fillWidth: true
                showPlaylist: persistentSettings.showPlaylist
                showExpandedFilterView: persistentSettings.expandedFilterView
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
                        minimumHeight: 600
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
                        minimumHeight: 120 + elisaTheme.mediaPlayerControlHeight
                        explicit: true
                    },
                    PropertyChanges {
                        target: headerBar
                        Layout.minimumHeight: mainWindow.height
                        Layout.maximumHeight: mainWindow.height
                    }
                ]
            }
        ]
        transitions: Transition {
            NumberAnimation {
                properties: "Layout.minimumHeight, Layout.maximumHeight, minimumHeight"
                easing.type: Easing.InOutQuad
                duration: 300
            }
        }
    }

    Component.onCompleted:
    {
        elisa.initialize()

        if (persistentSettings.playListState) {
            elisa.mediaPlayList.persistentState = persistentSettings.playListState
        }

        if (persistentSettings.audioPlayerState) {
            elisa.audioControl.persistentState = persistentSettings.audioPlayerState
        }

        elisa.mediaPlayList.randomPlay = Qt.binding(function() { return headerBar.playerControl.shuffle })
        elisa.mediaPlayList.repeatPlay = Qt.binding(function() { return headerBar.playerControl.repeat })
        elisa.playerControl.randomOrContinuePlay = Qt.binding(function() { return headerBar.playerControl.shuffle || headerBar.playerControl.repeat})
        elisa.audioPlayer.muted = Qt.binding(function() { return headerBar.playerControl.muted })
        elisa.audioPlayer.volume = Qt.binding(function() { return headerBar.playerControl.volume })

        mprisloader.active = true
    }
}
