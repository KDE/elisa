/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
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

    title: 'Elisa'

    property var helpAction: elisa.action("help_contents")
    property var quitApplication: elisa.action("file_quit")
    property var reportBugAction: elisa.action("help_report_bug")
    property var aboutAppAction: elisa.action("help_about_app")
    property var configureShortcutsAction: elisa.action("options_configure_keybinding")
    property var configureAction: elisa.action("options_configure")
    property var goBackAction: elisa.action("go_back")

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
    }


     Action {
          text: goBackAction.text
          shortcut: goBackAction.shortcut
          iconName: elisa.iconName(goBackAction.icon)
          onTriggered: {
              localAlbums.goBack()
              localArtists.goBack()
          }
      }

    Action {
        id: qmlQuitAction
        text: quitApplication.text
        shortcut: quitApplication.shortcut
        iconName: elisa.iconName(quitApplication.icon)
        onTriggered: quitApplication.trigger()
    }

    property string globalBrowseFlag: 'BrowseDirectChildren'
    property string globalFilter: '*'
    property string globalSortCriteria: ''

    Connections {
        target: Qt.application
        onAboutToQuit:
        {
            persistentSettings.x = mainWindow.x;
            persistentSettings.y = mainWindow.y;
            persistentSettings.width = mainWindow.width;
            persistentSettings.height = mainWindow.height;

            persistentSettings.playListState = playListModelItem.persistentState;
            persistentSettings.playListControlerState = playListModelItem.persistentState;
            persistentSettings.audioPlayerState = manageAudioPlayer.persistentState

            persistentSettings.playControlItemVolume = headerBar.playerControl.volume
            persistentSettings.playControlItemMuted = headerBar.playerControl.muted
        }
    }

    PlatformIntegration {
        id: platformInterface

        playListModel: playListModelItem
        playListControler: playListModelItem
        audioPlayerManager: manageAudioPlayer
        headerBarManager: myHeaderBarManager
        manageMediaPlayerControl: myPlayControlManager
        player: audioPlayer

        onRaisePlayer:
        {
            mainWindow.show()
            mainWindow.raise()
            mainWindow.requestActivate()
        }
    }

    MusicListenersManager {
        id: allListeners

        elisaApplication: elisa
    }

    AudioWrapper {
        id: audioPlayer

        muted: headerBar.playerControl.muted

        volume: headerBar.playerControl.volume

        onVolumeChanged: headerBar.playerControl.volume = volume
        onMutedChanged: headerBar.playerControl.muted = muted

        source: manageAudioPlayer.playerSource

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

    MediaPlayList {
        id: playListModelItem

        persistentState: persistentSettings.playListState
        musicListenersManager: allListeners

        onPlayListFinished: manageAudioPlayer.playListFinished()

        Component.onCompleted:
        {
            var d = new Date();
            var n = d.getMilliseconds();
            seedRandomGenerator(n);
        }
    }

    ManageHeaderBar {
        id: myHeaderBarManager

        playListModel: playListModelItem
        currentTrack: playListModelItem.currentTrack

        artistRole: MediaPlayList.ArtistRole
        titleRole: MediaPlayList.TitleRole
        albumRole: MediaPlayList.AlbumRole
        imageRole: MediaPlayList.ImageRole
        isValidRole: MediaPlayList.IsValidRole
    }

    ManageAudioPlayer {
        id: manageAudioPlayer

        currentTrack: playListModelItem.currentTrack
        playListModel: playListModelItem
        urlRole: MediaPlayList.ResourceRole
        isPlayingRole: MediaPlayList.IsPlayingRole
        titleRole: MediaPlayList.TitleRole
        artistNameRole: MediaPlayList.ArtistRole
        albumNameRole: MediaPlayList.AlbumRole

        playerStatus: audioPlayer.status
        playerPlaybackState: audioPlayer.playbackState
        playerError: audioPlayer.error
        audioDuration: audioPlayer.duration
        playerIsSeekable: audioPlayer.seekable
        playerPosition: audioPlayer.position

        persistentState: persistentSettings.audioPlayerState

        onPlayerPlay: audioPlayer.play()
        onPlayerPause: audioPlayer.pause()
        onPlayerStop: audioPlayer.stop()
        onSkipNextTrack: playListModelItem.skipNextTrack()
        onSeek: audioPlayer.seek(position)
        onSourceInError: allListeners.playBackError(source, playerError)
    }

    ManageMediaPlayerControl {
        id: myPlayControlManager

        playListModel: playListModelItem
        currentTrack: playListModelItem.currentTrack
    }

    AllAlbumsModel {
        id: allAlbumsModel
    }

    AllTracksModel {
        id: allTracksModel
    }

    Connections {
        target: allListeners

        onAlbumAdded: {
            busyScanningMusic.running = false
            allAlbumsModel.albumAdded(newAlbum)
        }
    }

    Connections {
        target: allListeners

        onAlbumRemoved: {
            allAlbumsModel.albumRemoved(removedAlbum)
        }
    }

    Connections {
        target: allListeners

        onAlbumModified: allAlbumsModel.albumModified(modifiedAlbum)
    }

    Connections {
        target: allListeners

        onTracksAdded: allTracksModel.tracksAdded(allTracks)
    }

    Connections {
        target: allListeners

        onTrackRemoved: allTracksModel.trackRemoved(id)
    }

    Connections {
        target: allListeners

        onTrackModified: allTracksModel.trackModified(modifiedTrack)
    }

    AllArtistsModel {
        id: allArtistsModel
    }

    Connections {
        target: allListeners

        onArtistAdded: allArtistsModel.artistAdded(newArtist)
    }

    Connections {
        target: allListeners

        onArtistRemoved: allArtistsModel.artistRemoved(removedArtist)
    }

    Connections {
        target: allListeners

        onArtistModified: allArtistsModel.artistModified(modifiedArtist)
    }

    Menu {
        id: applicationMenu
        title: i18nc("open application menu", "Application Menu")

        MenuItem {
            action: qmlQuitAction
            visible: qmlQuitAction.text !== ""
        }

        MenuSeparator {
            visible: qmlQuitAction.text !== ""
        }

        MenuItem {
            text: helpAction.text
            shortcut: helpAction.shortcut
            iconName: elisa.iconName(helpAction.icon)
            onTriggered: helpAction.trigger()
            visible: helpAction.text !== ""
        }

        MenuSeparator {
            visible: helpAction.text !== ""
        }

        MenuItem {
            text: reportBugAction.text
            shortcut: reportBugAction.shortcut
            iconName: elisa.iconName(reportBugAction.icon)
            onTriggered: reportBugAction.trigger()
            visible: reportBugAction.text !== ""
        }

        MenuSeparator {
            visible: reportBugAction.text !== ""
        }

        MenuItem {
            text: configureAction.text
            shortcut: configureAction.shortcut
            iconName: 'configure'
            onTriggered: configureAction.trigger()
            visible: configureAction.text !== ""
        }

        MenuItem {
            text: configureShortcutsAction.text
            shortcut: configureShortcutsAction.shortcut
            iconName: elisa.iconName(configureShortcutsAction.icon)
            onTriggered: configureShortcutsAction.trigger()
            visible: configureShortcutsAction.text !== ""
        }

        MenuSeparator {
            visible: configureAction.text !== "" || configureShortcutsAction.text !== ""
        }

        MenuItem {
            text: aboutAppAction.text
            shortcut: aboutAppAction.shortcut
            iconName: elisa.iconName(aboutAppAction.icon)
            onTriggered: aboutAppAction.trigger()
            visible: aboutAppAction.text !== ""
        }
    }

    Action {
        id: applicationMenuAction
        text: i18nc("open application menu", "Application Menu")
        iconName: "application-menu"
        onTriggered: applicationMenu.popup()
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

                    playerControl.duration: audioPlayer.duration
                    playerControl.seekable: audioPlayer.seekable

                    playerControl.volume: persistentSettings.playControlItemVolume
                    playerControl.muted: persistentSettings.playControlItemMuted
                    playerControl.position: audioPlayer.position
                    playerControl.skipBackwardEnabled: myPlayControlManager.skipBackwardControlEnabled
                    playerControl.skipForwardEnabled: myPlayControlManager.skipForwardControlEnabled
                    playerControl.playEnabled: myPlayControlManager.playControlEnabled
                    playerControl.isPlaying: myPlayControlManager.musicPlaying

                    playerControl.onSeek: audioPlayer.seek(position)

                    playerControl.onPlay: manageAudioPlayer.playPause()
                    playerControl.onPause: manageAudioPlayer.playPause()

                    playerControl.onPlayPrevious: playListModelItem.skipPreviousTrack()
                    playerControl.onPlayNext: playListModelItem.skipNextTrack()

                    ToolButton {
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

                        indexingRunning: allListeners.indexingRunning
                        importedTracksCount: allListeners.importedTracksCount
                        musicManager: allListeners
                    }
                }
            }

            RowLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 0

                ViewSelector {
                    id: listViews

                    Layout.fillHeight: true
                    Layout.preferredWidth: mainWindow.width * 0.15
                    Layout.maximumWidth: mainWindow.width * 0.15
                }

                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    spacing: 0

                    TopNotification {
                        id: invalidBalooConfiguration

                        Layout.fillWidth: true

                        musicManager: allListeners

                        focus: true
                    }

                    Item {
                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        RowLayout {
                            anchors.fill: parent

                            spacing: 0

                            id: contentZone

                            FocusScope {
                                id: mainContentView

                                focus: true

                                Layout.fillHeight: true

                                Layout.minimumWidth: 0
                                Layout.maximumWidth: 0
                                Layout.preferredWidth: 0


                                visible: Layout.minimumWidth != 0

                                Rectangle {
                                    border {
                                        color: (mainContentView.activeFocus ? myPalette.highlight : myPalette.base)
                                        width: 1
                                    }

                                    radius: 3
                                    color: myPalette.base

                                    anchors.fill: parent

                                    BusyIndicator {
                                        id: busyScanningMusic

                                        anchors.fill: parent

                                        anchors.leftMargin: parent.width / 3
                                        anchors.rightMargin: parent.width / 3
                                        anchors.topMargin: parent.height / 3
                                        anchors.bottomMargin: parent.height / 3

                                        opacity: 0.8

                                        z: 2

                                        running: true
                                    }

                                    MediaBrowser {
                                        id: localAlbums

                                        focus: true

                                        anchors {
                                            fill: parent

                                            leftMargin: elisaTheme.layoutHorizontalMargin
                                            rightMargin: elisaTheme.layoutHorizontalMargin
                                        }

                                        firstPage: MediaAllAlbumView {
                                            focus: true
                                            playListModel: playListModelItem
                                            playerControl: manageAudioPlayer
                                            stackView: localAlbums.stackView
                                            musicListener: allListeners
                                            contentDirectoryModel: allAlbumsModel

                                            onShowArtist: {
                                                listViews.currentIndex = 2
                                                allArtistsView.showArtistsAlbums(name)
                                            }
                                        }

                                        visible: opacity > 0
                                    }

                                    MediaBrowser {
                                        id: localArtists

                                        focus: true

                                        anchors {
                                            fill: parent

                                            leftMargin: elisaTheme.layoutHorizontalMargin
                                            rightMargin: elisaTheme.layoutHorizontalMargin
                                        }

                                        firstPage: MediaAllArtistView {
                                            id: allArtistsView

                                            focus: true
                                            playListModel: playListModelItem
                                            artistsModel: allArtistsModel
                                            playerControl: manageAudioPlayer
                                            stackView: localArtists.stackView
                                            musicListener: allListeners
                                            contentDirectoryModel: allAlbumsModel
                                        }

                                        visible: opacity > 0
                                    }

                                    MediaBrowser {
                                        id: localTracks

                                        focus: true

                                        anchors {
                                            fill: parent

                                            leftMargin: elisaTheme.layoutHorizontalMargin
                                            rightMargin: elisaTheme.layoutHorizontalMargin
                                        }

                                        firstPage: MediaAllTracksView {
                                            focus: true
                                            playListModel: playListModelItem
                                            tracksModel: allTracksModel
                                            playerControl: manageAudioPlayer
                                            stackView: localTracks.stackView
                                            musicListener: allListeners
                                        }

                                        visible: opacity > 0
                                    }

                                    Behavior on border.color {
                                        ColorAnimation {
                                            duration: 300
                                        }
                                    }
                                }
                            }

                            Rectangle {
                                id: firstViewSeparatorItem

                                border.width: 1
                                border.color: myPalette.mid
                                color: myPalette.mid
                                visible: true

                                Layout.bottomMargin: elisaTheme.layoutVerticalMargin
                                Layout.topMargin: elisaTheme.layoutVerticalMargin

                                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                                Layout.fillHeight: true

                                Layout.preferredWidth: 1
                                Layout.minimumWidth: 1
                                Layout.maximumWidth: 1
                            }

                            MediaPlayListView {
                                id: playList

                                playListModel: playListModelItem
                                playListControler: playListModelItem

                                randomPlayChecked: playListModelItem.randomPlay
                                repeatPlayChecked: playListModelItem.repeatPlay

                                Layout.fillHeight: true
                                Layout.leftMargin: elisaTheme.layoutHorizontalMargin
                                Layout.rightMargin: elisaTheme.layoutHorizontalMargin

                                Layout.minimumWidth: contentZone.width
                                Layout.maximumWidth: contentZone.width
                                Layout.preferredWidth: contentZone.width

                                Component.onCompleted:
                                {
                                    playListModelItem.randomPlay = Qt.binding(function() { return playList.randomPlayChecked })
                                    playListModelItem.repeatPlay = Qt.binding(function() { return playList.repeatPlayChecked })
                                    myPlayControlManager.randomOrContinuePlay = Qt.binding(function() { return playList.randomPlayChecked || playList.repeatPlayChecked })
                                }

                                onStartPlayback: manageAudioPlayer.ensurePlay()

                                onPausePlayback: manageAudioPlayer.playPause()
                            }

                            Rectangle {
                                id: viewSeparatorItem

                                border.width: 1
                                border.color: myPalette.mid
                                color: myPalette.mid
                                visible: Layout.minimumWidth != 0

                                Layout.bottomMargin: elisaTheme.layoutVerticalMargin
                                Layout.topMargin: elisaTheme.layoutVerticalMargin

                                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                                Layout.fillHeight: true

                                Layout.preferredWidth: 1
                                Layout.minimumWidth: 1
                                Layout.maximumWidth: 1
                            }

                            ContextView {
                                id: albumContext

                                Layout.fillHeight: true

                                Layout.minimumWidth: contentZone.width
                                Layout.maximumWidth: contentZone.width
                                Layout.preferredWidth: contentZone.width

                                visible: Layout.minimumWidth != 0

                                artistName: myHeaderBarManager.artist
                                albumName: myHeaderBarManager.album
                                albumArtUrl: myHeaderBarManager.image
                            }
                        }
                    }

                    states: [
                        State {
                            name: 'full'
                            when: listViews.currentIndex === 0
                            PropertyChanges {
                                target: mainContentView
                                Layout.fillWidth: false
                                Layout.minimumWidth: 0
                                Layout.maximumWidth: 0
                                Layout.preferredWidth: 0
                            }
                            PropertyChanges {
                                target: firstViewSeparatorItem
                                Layout.minimumWidth: 0
                                Layout.maximumWidth: 0
                                Layout.preferredWidth: 0
                            }
                            PropertyChanges {
                                target: playList
                                Layout.minimumWidth: contentZone.width / 2
                                Layout.maximumWidth: contentZone.width / 2
                                Layout.preferredWidth: contentZone.width / 2
                            }
                            PropertyChanges {
                                target: viewSeparatorItem
                                Layout.minimumWidth: 1
                                Layout.maximumWidth: 1
                                Layout.preferredWidth: 1
                            }
                            PropertyChanges {
                                target: albumContext
                                Layout.minimumWidth: contentZone.width / 2
                                Layout.maximumWidth: contentZone.width / 2
                                Layout.preferredWidth: contentZone.width / 2
                            }
                            PropertyChanges {
                                target: localAlbums
                                opacity: 0
                            }
                            PropertyChanges {
                                target: localArtists
                                opacity: 0
                            }
                            PropertyChanges {
                                target: localTracks
                                opacity: 0
                            }
                        },
                        State {
                            name: 'allAlbums'
                            when: listViews.currentIndex === 1
                            StateChangeScript {
                                script: {
                                    localAlbums.stackView.pop({item: null, immediate: true})
                                }
                            }
                            PropertyChanges {
                                target: mainContentView
                                Layout.fillWidth: true
                                Layout.minimumWidth: contentZone.width * 0.66
                                Layout.maximumWidth: contentZone.width * 0.68
                                Layout.preferredWidth: contentZone.width * 0.68
                            }
                            PropertyChanges {
                                target: firstViewSeparatorItem
                                Layout.minimumWidth: 1
                                Layout.maximumWidth: 1
                                Layout.preferredWidth: 1
                            }
                            PropertyChanges {
                                target: playList
                                Layout.minimumWidth: contentZone.width * 0.33
                                Layout.maximumWidth: contentZone.width * 0.33
                                Layout.preferredWidth: contentZone.width * 0.33
                            }
                            PropertyChanges {
                                target: viewSeparatorItem
                                Layout.minimumWidth: 0
                                Layout.maximumWidth: 0
                                Layout.preferredWidth: 0
                            }
                            PropertyChanges {
                                target: albumContext
                                Layout.minimumWidth: 0
                                Layout.maximumWidth: 0
                                Layout.preferredWidth: 0
                            }
                            PropertyChanges {
                                target: localAlbums
                                opacity: 1
                            }
                            PropertyChanges {
                                target: localArtists
                                opacity: 0
                            }
                            PropertyChanges {
                                target: localTracks
                                opacity: 0
                            }
                        },
                        State {
                            name: 'allArtists'
                            when: listViews.currentIndex === 2
                            StateChangeScript {
                                script: {
                                    localArtists.stackView.pop({item: null, immediate: true})
                                }
                            }
                            PropertyChanges {
                                target: mainContentView
                                Layout.fillWidth: true
                                Layout.minimumWidth: contentZone.width * 0.66
                                Layout.maximumWidth: contentZone.width * 0.68
                                Layout.preferredWidth: contentZone.width * 0.68
                            }
                            PropertyChanges {
                                target: firstViewSeparatorItem
                                Layout.minimumWidth: 1
                                Layout.maximumWidth: 1
                                Layout.preferredWidth: 1
                            }
                            PropertyChanges {
                                target: playList
                                Layout.minimumWidth: contentZone.width * 0.33
                                Layout.maximumWidth: contentZone.width * 0.33
                                Layout.preferredWidth: contentZone.width * 0.33
                            }
                            PropertyChanges {
                                target: viewSeparatorItem
                                Layout.minimumWidth: 0
                                Layout.maximumWidth: 0
                                Layout.preferredWidth: 0
                            }
                            PropertyChanges {
                                target: albumContext
                                Layout.minimumWidth: 0
                                Layout.maximumWidth: 0
                                Layout.preferredWidth: 0
                            }
                            PropertyChanges {
                                target: localAlbums
                                opacity: 0
                            }
                            PropertyChanges {
                                target: localArtists
                                opacity: 1
                            }
                            PropertyChanges {
                                target: localTracks
                                opacity: 0
                            }
                        },
                        State {
                            name: 'allTracks'
                            when: listViews.currentIndex === 3
                            PropertyChanges {
                                target: mainContentView
                                Layout.fillWidth: true
                                Layout.minimumWidth: contentZone.width * 0.66
                                Layout.maximumWidth: contentZone.width * 0.68
                                Layout.preferredWidth: contentZone.width * 0.68
                            }
                            PropertyChanges {
                                target: firstViewSeparatorItem
                                Layout.minimumWidth: 1
                                Layout.maximumWidth: 1
                                Layout.preferredWidth: 1
                            }
                            PropertyChanges {
                                target: playList
                                Layout.minimumWidth: contentZone.width * 0.33
                                Layout.maximumWidth: contentZone.width * 0.33
                                Layout.preferredWidth: contentZone.width * 0.33
                            }
                            PropertyChanges {
                                target: viewSeparatorItem
                                Layout.minimumWidth: 0
                                Layout.maximumWidth: 0
                                Layout.preferredWidth: 0
                            }
                            PropertyChanges {
                                target: albumContext
                                Layout.minimumWidth: 0
                                Layout.maximumWidth: 0
                                Layout.preferredWidth: 0
                            }
                            PropertyChanges {
                                target: localAlbums
                                opacity: 0
                            }
                            PropertyChanges {
                                target: localArtists
                                opacity: 0
                            }
                            PropertyChanges {
                                target: localTracks
                                opacity: 1
                            }
                        }
                    ]
                    transitions: Transition {
                        NumberAnimation {
                            properties: "Layout.minimumWidth, Layout.maximumWidth, Layout.preferredWidth, opacity"
                            easing.type: Easing.InOutQuad
                            duration: 300
                        }
                    }
                }
            }
        }
    }
}
