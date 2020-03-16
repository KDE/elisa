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
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0
import Qt.labs.settings 1.0
import Qt.labs.platform 1.1

ApplicationWindow {
    id: mainWindow
    
    visible: true
    
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
        property int width : 900
        property int height : 650

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
        onOpenMenu: {
            if (applicationMenu.visible) {
                applicationMenu.close()
            } else {
                applicationMenu.popup(mainWindow.width - applicationMenu.width, headerBar.height)
            }
        }
    }

    Connections {
        target: Qt.application
        onAboutToQuit:
        {
            persistentSettings.x = mainWindow.x;
            persistentSettings.y = mainWindow.y;
            persistentSettings.width = mainWindow.width;
            persistentSettings.height = mainWindow.height;

            persistentSettings.playListState = elisa.mediaPlayListProxyModel.persistentState;
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

            playListModel: elisa.mediaPlayListProxyModel
            audioPlayerManager: elisa.audioControl
            player: elisa.audioPlayer
            headerBarManager: elisa.manageHeaderBar
            manageMediaPlayerControl: elisa.playerControl
            showProgressOnTaskBar: elisa.showProgressOnTaskBar
            showSystemTrayIcon: elisa.showSystemTrayIcon
            elisaMainWindow: mainWindow

            onRaisePlayer: {
                mainWindow.visible = true
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

                tracksCount: elisa.mediaPlayListProxyModel.remainingTracks
                album: (elisa.manageHeaderBar.album !== undefined ? elisa.manageHeaderBar.album : '')
                title: elisa.manageHeaderBar.title
                artist: (elisa.manageHeaderBar.artist !== undefined ? elisa.manageHeaderBar.artist : '')
                albumArtist: (elisa.manageHeaderBar.albumArtist !== undefined ? elisa.manageHeaderBar.albumArtist : '')
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

                playerControl.repeat: elisa.mediaPlayListProxyModel.repeatPlay
                playerControl.shuffle: elisa.mediaPlayListProxyModel.shufflePlayList

                playerControl.onSeek: elisa.audioPlayer.seek(position)

                playerControl.onPlay: elisa.audioControl.playPause()
                playerControl.onPause: elisa.audioControl.playPause()
                playerControl.onPlayPrevious: elisa.mediaPlayListProxyModel.skipPreviousTrack()
                playerControl.onPlayNext: elisa.mediaPlayListProxyModel.skipNextTrack()

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
        elisa.initialize()

        if (persistentSettings.playListState) {
            elisa.mediaPlayListProxyModel.persistentState = persistentSettings.playListState
        }

        if (persistentSettings.audioPlayerState) {
            elisa.audioControl.persistentState = persistentSettings.audioPlayerState
        }

        elisa.mediaPlayListProxyModel.shufflePlayList = Qt.binding(function() { return headerBar.playerControl.shuffle })
        elisa.mediaPlayListProxyModel.repeatPlay = Qt.binding(function() { return headerBar.playerControl.repeat })
        elisa.audioPlayer.muted = Qt.binding(function() { return headerBar.playerControl.muted })
        elisa.audioPlayer.volume = Qt.binding(function() { return headerBar.playerControl.volume })

        mprisloader.active = true
    }
}
