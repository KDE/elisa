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

    title: i18n("Elisa")

    property var goBackAction: elisa.action("go_back")

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
        player: audioPlayer

        onRaisePlayer:
        {
            mainWindow.show()
            mainWindow.raise()
            mainWindow.requestActivate()
        }
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
        onSkipNextTrack: elisa.mediaPlayList.skipNextTrack()
        onSeek: audioPlayer.seek(position)
        onSourceInError:
        {
            elisa.mediaPlayList.trackInError(source, playerError)
            allListeners.playBackError(source, playerError)
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

                    playerControl.onPlayPrevious: elisa.mediaPlayList.skipPreviousTrack()
                    playerControl.onPlayNext: elisa.mediaPlayList.skipNextTrack()

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

                        musicManager: elisa.musicManager

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
                                    }

                                    Loader {
                                        sourceComponent: Binding {
                                            target: busyScanningMusic
                                            property: 'running'
                                            value: elisa.musicManager.indexerBusy
                                        }

                                        active: elisa.musicManager !== undefined
                                    }

                                    MediaBrowser {
                                        id: localAlbums

                                        focus: true

                                        anchors {
                                            fill: parent

                                            leftMargin: elisaTheme.layoutHorizontalMargin
                                            rightMargin: elisaTheme.layoutHorizontalMargin
                                        }

                                        firstPage: GridBrowserView {
                                            id: allAlbumsView

                                            focus: true

                                            contentModel: elisa.allAlbumsProxyModel

                                            image: elisaTheme.albumIcon
                                            mainTitle: i18nc("Title of the view of all albums", "Albums")

                                            onOpen: {
                                                elisa.singleAlbumProxyModel.sourceModel.loadAlbumData(databaseId)
                                                localAlbums.stackView.push(albumView, {
                                                                               stackView: localAlbums.stackView,
                                                                               albumName: innerMainTitle,
                                                                               artistName:  innerSecondaryTitle,
                                                                               albumArtUrl: innerImage,
                                                                           })
                                            }
                                            onGoBack: localAlbums.stackView.pop()
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

                                        firstPage: GridBrowserView {
                                            id: allArtistsView
                                            focus: true

                                            showRating: false
                                            delegateDisplaySecondaryText: false

                                            contentModel: elisa.allArtistsProxyModel

                                            image: elisaTheme.artistIcon
                                            mainTitle: i18nc("Title of the view of all artists", "Artists")

                                            onOpen: {
                                                elisa.singleArtistProxyModel.setArtistFilterText(innerMainTitle)
                                                localArtists.stackView.push(innerAlbumView, {
                                                                                mainTitle: innerMainTitle,
                                                                                secondaryTitle: innerSecondaryTitle,
                                                                                image: innerImage,
                                                                                stackView: localArtists.stackView
                                                                            })

                                            }
                                            onGoBack: localArtists.stackView.pop()
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
                                            stackView: localTracks.stackView

                                            contentModel: elisa.allTracksProxyModel
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

                                playListModel: elisa.mediaPlayList

                                randomPlayChecked: elisa.mediaPlayList.randomPlay
                                repeatPlayChecked: elisa.mediaPlayList.repeatPlay

                                Layout.fillHeight: true
                                Layout.leftMargin: elisaTheme.layoutHorizontalMargin
                                Layout.rightMargin: elisaTheme.layoutHorizontalMargin

                                Layout.minimumWidth: contentZone.width
                                Layout.maximumWidth: contentZone.width
                                Layout.preferredWidth: contentZone.width

                                onStartPlayback: manageAudioPlayer.ensurePlay()

                                onPausePlayback: manageAudioPlayer.playPause()

                                onDisplayError: messageNotification.showNotification(errorText)
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

    Component {
        id: innerAlbumView

        GridBrowserView {
            property var stackView

            contentModel: elisa.singleArtistProxyModel

            isSubPage: true

            onOpen: {
                elisa.singleAlbumProxyModel.sourceModel.loadAlbumData(databaseId)
                localArtists.stackView.push(albumView, {
                                                stackView: localArtists.stackView,
                                                albumName: innerMainTitle,
                                                artistName: innerSecondaryTitle,
                                                albumArtUrl: innerImage,
                                            })
            }
            onGoBack: stackView.pop()
        }
    }

    Component {
        id: albumView

        MediaAlbumView {
            property var stackView

            contentModel: elisa.singleAlbumProxyModel

            onShowArtist: {
                listViews.currentIndex = 2
                if (localArtists.stackView.depth === 3) {
                    localArtists.stackView.pop()
                }
                if (localArtists.stackView.depth === 2) {
                    var artistPage = localArtists.stackView.get(1)
                    if (artistPage.mainTitle === name) {
                        return
                    } else {
                        localArtists.stackView.pop()
                    }
                }
                allArtistsView.open(name, name, elisaTheme.defaultArtistImage, '')
            }
            onGoBack: stackView.pop()
        }
    }

    Component.onCompleted:
    {
        elisa.initialize()

        var d = new Date();
        var n = d.getMilliseconds();
        elisa.mediaPlayList.seedRandomGenerator(n);

        elisa.mediaPlayList.randomPlay = Qt.binding(function() { return playList.randomPlayChecked })
        elisa.mediaPlayList.repeatPlay = Qt.binding(function() { return playList.repeatPlayChecked })
        myPlayControlManager.randomOrContinuePlay = Qt.binding(function() { return playList.randomPlayChecked || playList.repeatPlayChecked })

        if (persistentSettings.playListState) {
            elisa.mediaPlayList.persistentState = persistentSettings.playListState
        }

        elisa.mediaPlayList.enqueue(elisa.arguments)
    }
}
