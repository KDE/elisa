/*
 * Copyright 2016-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as Controls1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import org.kde.elisa 1.0
import Qt.labs.settings 1.0

ApplicationWindow {
    id: mainWindow

    visible: true

    minimumWidth: 1000
    minimumHeight: 600

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    x: persistentSettings.x
    y: persistentSettings.y
    width: persistentSettings.width
    height: persistentSettings.height

    title: i18n("Elisa")

    property var goBackAction: elisa.action("go_back")

    Controls1.Action  {
        shortcut: goBackAction.shortcut
        onTriggered: contentView.goBack()
    }

    Controls1.Action {
        id: applicationMenuAction
        text: i18nc("open application menu", "Application Menu")
        iconName: "application-menu"
        onTriggered: applicationMenu.popup()
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
        property int width : 1000
        property int height : 600

        property var playListState

        property var playListControlerState

        property var audioPlayerState

        property double playControlItemVolume : 100.0
        property bool playControlItemMuted : false

        property string filterState

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
            persistentSettings.playListControlerState = elisa.mediaPlayList.persistentState;
            persistentSettings.audioPlayerState = manageAudioPlayer.persistentState

            persistentSettings.playControlItemVolume = headerBar.playerControl.volume
            persistentSettings.playControlItemMuted = headerBar.playerControl.muted
        }
    }

    PlatformIntegration {
        id: platformInterface

        playListModel: elisa.mediaPlayList
        playListControler: elisa.mediaPlayList
        audioPlayerManager: manageAudioPlayer
        headerBarManager: myHeaderBarManager
        manageMediaPlayerControl: myPlayControlManager
        player: elisa.audioPlayer

        onRaisePlayer:
        {
            mainWindow.show()
            mainWindow.raise()
            mainWindow.requestActivate()
        }
    }

    Connections {
        target: elisa.audioPlayer

        onVolumeChanged: headerBar.playerControl.volume = elisa.audioPlayer.volume
        onMutedChanged: headerBar.playerControl.muted = elisa.audioPlayer.muted

        onPlaying: {
            myPlayControlManager.playerPlaying()
        }

        onPaused: {
            myPlayControlManager.playerPaused()
        }

        onStopped: {
            myPlayControlManager.playerStopped()
        }
    }

    Connections {
        target: elisa.mediaPlayList

        onPlayListLoadFailed: {
            messageNotification.showNotification(i18nc("message of passive notification when playlist load failed", "Load of playlist failed"), 3000)
        }

        onEnsurePlay: manageAudioPlayer.ensurePlay()

        onPlayListFinished: manageAudioPlayer.playListFinished()
    }

    ManageHeaderBar {
        id: myHeaderBarManager

        playListModel: elisa.mediaPlayList
        currentTrack: elisa.mediaPlayList.currentTrack

        artistRole: MediaPlayList.ArtistRole
        titleRole: MediaPlayList.TitleRole
        albumRole: MediaPlayList.AlbumRole
        imageRole: MediaPlayList.ImageRole
        isValidRole: MediaPlayList.IsValidRole
    }

    ManageAudioPlayer {
        id: manageAudioPlayer

        currentTrack: elisa.mediaPlayList.currentTrack
        playListModel: elisa.mediaPlayList
        urlRole: MediaPlayList.ResourceRole
        isPlayingRole: MediaPlayList.IsPlayingRole
        titleRole: MediaPlayList.TitleRole
        artistNameRole: MediaPlayList.ArtistRole
        albumNameRole: MediaPlayList.AlbumRole

        playerStatus: elisa.audioPlayer.status
        playerPlaybackState: elisa.audioPlayer.playbackState
        playerError: elisa.audioPlayer.error
        audioDuration: elisa.audioPlayer.duration
        playerIsSeekable: elisa.audioPlayer.seekable
        playerPosition: elisa.audioPlayer.position

        persistentState: persistentSettings.audioPlayerState

        onPlayerPlay: elisa.audioPlayer.play()
        onPlayerPause: elisa.audioPlayer.pause()
        onPlayerStop: elisa.audioPlayer.stop()
        onSkipNextTrack: elisa.mediaPlayList.skipNextTrack()
        onSeek: elisa.audioPlayer.seek(position)
        onSourceInError:
        {
            elisa.mediaPlayList.trackInError(source, playerError)
            elisa.musicManager.playBackError(source, playerError)
        }

        onDisplayTrackError: messageNotification.showNotification(i18n("Error when playing %1", "" + fileName), 3000)
    }

    ManageMediaPlayerControl {
        id: myPlayControlManager

        playListModel: elisa.mediaPlayList
        currentTrack: elisa.mediaPlayList.currentTrack
    }

    PassiveNotification {
        id: messageNotification
    }

    Rectangle {
        color: myPalette.base
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Item {
                Layout.preferredHeight: mainWindow.height * 0.2 + elisaTheme.mediaPlayerControlHeight
                Layout.minimumHeight: mainWindow.height * 0.2 + elisaTheme.mediaPlayerControlHeight
                Layout.maximumHeight: mainWindow.height * 0.2 + elisaTheme.mediaPlayerControlHeight
                Layout.fillWidth: true

                HeaderBar {
                    id: headerBar

                    focus: true

                    anchors.fill: parent

                    tracksCount: myHeaderBarManager.remainingTracks
                    album: myHeaderBarManager.album
                    title: myHeaderBarManager.title
                    artist: myHeaderBarManager.artist
                    image: myHeaderBarManager.image

                    ratingVisible: false

                    playerControl.duration: elisa.audioPlayer.duration
                    playerControl.seekable: elisa.audioPlayer.seekable

                    playerControl.volume: persistentSettings.playControlItemVolume
                    playerControl.muted: persistentSettings.playControlItemMuted
                    playerControl.position: elisa.audioPlayer.position
                    playerControl.skipBackwardEnabled: myPlayControlManager.skipBackwardControlEnabled
                    playerControl.skipForwardEnabled: myPlayControlManager.skipForwardControlEnabled
                    playerControl.playEnabled: myPlayControlManager.playControlEnabled
                    playerControl.isPlaying: myPlayControlManager.musicPlaying

                    playerControl.onSeek: elisa.audioPlayer.seek(position)

                    playerControl.onPlay: manageAudioPlayer.playPause()
                    playerControl.onPause: manageAudioPlayer.playPause()

                    playerControl.onPlayPrevious: elisa.mediaPlayList.skipPreviousTrack()
                    playerControl.onPlayNext: elisa.mediaPlayList.skipNextTrack()

                    Controls1.ToolButton {
                        id: menuButton

                        action: applicationMenuAction

                        z: 2

                        anchors
                        {
                            right: parent.right
                            top: parent.top
                            rightMargin: elisaTheme.layoutHorizontalMargin * 3
                            topMargin: elisaTheme.layoutHorizontalMargin * 3
                        }
                    }
                    Rectangle {
                        anchors.fill: menuButton

                        z: 1

                        radius: width / 2

                        color: myPalette.window
                    }

                    TrackImportNotification {
                        id: importedTracksCountNotification

                        anchors
                        {
                            right: menuButton.left
                            top: menuButton.top
                            bottom: menuButton.bottom
                            rightMargin: elisaTheme.layoutHorizontalMargin * 3
                        }
                    }

                    Binding {
                        target: importedTracksCountNotification
                        property: 'musicManager'
                        value: elisa.musicManager
                        when: elisa.musicManager !== undefined
                    }

                    Loader {
                        sourceComponent: Binding {
                            target: importedTracksCountNotification
                            property: 'indexingRunning'
                            value: elisa.musicManager.indexingRunning
                        }

                        active: elisa.musicManager !== undefined
                    }

                    Loader {
                        sourceComponent: Binding {
                            target: importedTracksCountNotification
                            property: 'importedTracksCount'
                            value: elisa.musicManager.importedTracksCount
                        }

                        active: elisa.musicManager !== undefined
                    }
                }
            }

            ContentView {
                id: contentView
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }

    Component.onCompleted:
    {
        elisa.initialize()

        elisa.mediaPlayList.randomPlay = Qt.binding(function() { return contentView.playList.randomPlayChecked })
        elisa.mediaPlayList.repeatPlay = Qt.binding(function() { return contentView.playList.repeatPlayChecked })
        myPlayControlManager.randomOrContinuePlay = Qt.binding(function() { return contentView.playList.randomPlayChecked || contentView.playList.repeatPlayChecked })

        if (persistentSettings.playListState) {
            elisa.mediaPlayList.persistentState = persistentSettings.playListState
        }


        elisa.audioPlayer.muted = Qt.binding(function() { return headerBar.playerControl.muted })
        elisa.audioPlayer.volume = Qt.binding(function() { return headerBar.playerControl.volume })
        elisa.audioPlayer.source = Qt.binding(function() { return manageAudioPlayer.playerSource })

        elisa.mediaPlayList.enqueue(elisa.arguments)
        volume: headerBar.playerControl.volume
    }
}
