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

    minimumWidth: 1100
    minimumHeight: 600

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    x: persistentSettings.x
    y: persistentSettings.y
    width: persistentSettings.width
    height: persistentSettings.height

    title: i18n("Elisa")

    property var goBackAction: elisa.action("go_back")
    property var findAction: elisa.action("edit_find")

    Controls1.Action  {
        shortcut: findAction.shortcut
        onTriggered: {
            if ( persistentSettings.expandedFilterView == true)
            {
                persistentSettings.expandedFilterView = false
            } else {
                persistentSettings.expandedFilterView = true
            }
        }
    }
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
        property int width : 1100
        property int height : 600

        property var playListState

        property var audioPlayerState

        property double playControlItemVolume : 100.0
        property bool playControlItemMuted : false

        property bool playControlItemRepeat : false
        property bool playControlItemShuffle : false

        property bool expandedFilterView: false

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

            persistentSettings.playControlItemRepeat = headerBar.playerControl.repeat
            persistentSettings.playControlItemShuffle = headerBar.playerControl.shuffle
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
            headerBarManager: myHeaderBarManager
            manageMediaPlayerControl: myPlayControlManager
            onRaisePlayer:
            {
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

                    playerControl.repeat: persistentSettings.playControlItemRepeat
                    playerControl.shuffle: persistentSettings.playControlItemShuffle

                    playerControl.onSeek: elisa.audioPlayer.seek(position)

                    playerControl.onPlay: elisa.audioControl.playPause()
                    playerControl.onPause: elisa.audioControl.playPause()
                    playerControl.onPlayPrevious: elisa.mediaPlayList.skipPreviousTrack()
                    playerControl.onPlayNext: elisa.mediaPlayList.skipNextTrack()

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

        elisa.mediaPlayList.randomPlay = Qt.binding(function() { return headerBar.playerControl.shuffle })
        elisa.mediaPlayList.repeatPlay = Qt.binding(function() { return headerBar.playerControl.repeat })
        myPlayControlManager.randomOrContinuePlay = Qt.binding(function() { return headerBar.playerControl.shuffle || headerBar.playerControl.repeat })
        myPlayControlManager.playListModel = Qt.binding(function() { return elisa.mediaPlayList })
        myPlayControlManager.currentTrack = Qt.binding(function() { return elisa.mediaPlayList.currentTrack })

        if (persistentSettings.playListState) {
            elisa.mediaPlayList.persistentState = persistentSettings.playListState
        }

        if (persistentSettings.audioPlayerState) {
            elisa.audioControl.persistentState = persistentSettings.audioPlayerState
        }

        elisa.audioPlayer.muted = Qt.binding(function() { return headerBar.playerControl.muted })
        elisa.audioPlayer.volume = Qt.binding(function() { return headerBar.playerControl.volume })

        volume: headerBar.playerControl.volume

        mprisloader.active = true
    }
}
