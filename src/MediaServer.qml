/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2015 Marco Martin <mart@kde.org>
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

import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.6
import Qt.labs.settings 1.0

ApplicationWindow {
    id: mainWindow

    visible: true

    minimumWidth: 1000
    minimumHeight: 600

    x: persistentSettings.x
    y: persistentSettings.y
    width: persistentSettings.width
    height: persistentSettings.height

    title: 'Elisa'

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Settings {
        id: persistentSettings

        property int x
        property int y
        property int width
        property int height

        property var playListState

        property var playListControlerState

        property var audioPlayerState

        property double playControlItemVolume
        property bool playControlItemMuted
    }

    property string globalBrowseFlag: 'BrowseDirectChildren'
    property string globalFilter: '*'
    property string globalSortCriteria: ''

    /**
     * Shows a little passive notification at the bottom of the app window
     * lasting for few seconds, with an optional action button.
     *
     * @param message The text message to be shown to the user.
     * @param timeout How long to show the message:
     *            possible values: "short", "long" or the number of milliseconds
     * @param actionText Text in the action button, if any.
     * @param callBack A JavaScript function that will be executed when the
     *            user clicks the button.
     */
    function showPassiveNotification(message, timeout, actionText, callBack) {
        if (!internal.__passiveNotification) {
            var component = Qt.createComponent(Qt.resolvedUrl('PassiveNotification.qml'));
            internal.__passiveNotification = component.createObject(mainWindow);
        }

        internal.__passiveNotification.showNotification(message, timeout, actionText, callBack);
    }

    /**
     * Hide the passive notification, if any is shown
     */
    function hidePassiveNotification() {
        if(internal.__passiveNotification) {
            internal.__passiveNotification.hideNotification();
        }
    }

    QtObject {
        id: internal
        property Item __passiveNotification
    }

    Action {
        id: quitAction
        text: i18nc("Action to quit the application", "&Quit")
        shortcut: StandardKey.Quit
        onTriggered: Qt.quit()
    }

    Connections {
        target: Qt.application
        onAboutToQuit:
        {
            persistentSettings.x = mainWindow.x;
            persistentSettings.y = mainWindow.y;
            persistentSettings.width = mainWindow.width;
            persistentSettings.height = mainWindow.height;

            persistentSettings.playListState = playListModelItem.persistentState;
            persistentSettings.playListControlerState = playListControlerItem.persistentState;
            persistentSettings.audioPlayerState = manageAudioPlayer.persistentState

            persistentSettings.playControlItemVolume = playControlItem.volume
            persistentSettings.playControlItemMuted = playControlItem.muted
        }
    }

    Mpris2 {
        id: mpris2Interface

        playerName: 'elisa'
        playListModel: playListModelItem
        playListControler: playListControlerItem
        audioPlayerManager: manageAudioPlayer
        headerBarManager: myHeaderBarManager
        manageMediaPlayerControl: myPlayControlManager

        onRaisePlayer:
        {
            mainWindow.raise()
            mainWindow.requestActivate()
        }
    }

    MusicListenersManager {
        id: allListeners
    }

    Audio {
        id: audioPlayer

        audioRole: Audio.MusicRole

        muted: playControlItem.muted

        volume: playControlItem.volume
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

        onTrackHasBeenAdded: mainWindow.showPassiveNotification(i18n("Track %1 has been added", title), 1500, "", {})
    }

    PlayListControler {
        id: playListControlerItem

        playListModel: playListModelItem

        isValidRole: MediaPlayList.IsValidRole

        onPlayListFinished: manageAudioPlayer.playListFinished()

        persistentState: persistentSettings.playListControlerState

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
        currentTrack: playListControlerItem.currentTrack

        artistRole: MediaPlayList.ArtistRole
        titleRole: MediaPlayList.TitleRole
        albumRole: MediaPlayList.AlbumRole
        imageRole: MediaPlayList.ImageRole
        isValidRole: MediaPlayList.IsValidRole
    }

    ManageAudioPlayer {
        id: manageAudioPlayer

        currentTrack: playListControlerItem.currentTrack
        playListModel: playListModelItem
        urlRole: MediaPlayList.ResourceRole
        isPlayingRole: MediaPlayList.IsPlayingRole

        playerStatus: audioPlayer.status
        playerPlaybackState: audioPlayer.playbackState
        playerError: audioPlayer.error
        audioDuration: audioPlayer.duration
        playerIsSeekable: audioPlayer.seekable

        persistentState: persistentSettings.audioPlayerState

        onPlayerPlay: audioPlayer.play()
        onPlayerPause: audioPlayer.pause()
        onPlayerStop: audioPlayer.stop()
        onSkipNextTrack: playListControlerItem.skipNextTrack()
    }

    ManageMediaPlayerControl {
        id: myPlayControlManager

        playListModel: playListModelItem
        currentTrack: playListControlerItem.currentTrack
    }

    AllAlbumsModel {
        id: allAlbumsModel
    }

    Connections {
        target: allListeners

        onAlbumAdded: allAlbumsModel.albumAdded(newAlbum)
    }

    AllArtistsModel {
        id: allArtistsModel
    }

    Connections {
        target: allListeners

        onArtistAdded: allArtistsModel.artistAdded(newArtist)
    }

    Rectangle {
        color: myPalette.base
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            HeaderBar {
                id: headerBar

                Layout.preferredHeight: Screen.pixelDensity * 28.
                Layout.minimumHeight: Layout.preferredHeight
                Layout.maximumHeight: Layout.preferredHeight
                Layout.fillWidth: true

                tracksCount: myHeaderBarManager.remainingTracks
                album: myHeaderBarManager.album
                title: myHeaderBarManager.title
                artist: myHeaderBarManager.artist
                image: myHeaderBarManager.image

                ratingVisible: false
            }

            MediaPlayerControl {
                id: playControlItem

                duration: audioPlayer.duration
                seekable: audioPlayer.seekable

                volume: persistentSettings.playControlItemVolume
                muted: persistentSettings.playControlItemMuted
                position: audioPlayer.position
                skipBackwardEnabled: myPlayControlManager.skipBackwardControlEnabled
                skipForwardEnabled: myPlayControlManager.skipForwardControlEnabled
                playEnabled: myPlayControlManager.playControlEnabled
                isPlaying: myPlayControlManager.musicPlaying

                Layout.preferredHeight: Screen.pixelDensity * 12.
                Layout.minimumHeight: Layout.preferredHeight
                Layout.maximumHeight: Layout.preferredHeight
                Layout.fillWidth: true

                onSeek: audioPlayer.seek(position)

                onPlay: manageAudioPlayer.playPause()
                onPause: manageAudioPlayer.playPause()

                onPlayPrevious: playListControlerItem.skipPreviousTrack()
                onPlayNext: playListControlerItem.skipNextTrack()
            }

            RowLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 0

                Rectangle {
                    color: myPalette.window

                    Layout.fillHeight: true
                    Layout.preferredWidth: Screen.pixelDensity * 50.
                    Layout.minimumWidth: Screen.pixelDensity * 50.
                    Layout.maximumWidth: Screen.pixelDensity * 50.

                    TableView {
                        id: viewModeView

                        flickableItem.boundsBehavior: Flickable.StopAtBounds
                        anchors.fill: parent

                        headerVisible: false
                        frameVisible: false
                        focus: true
                        backgroundVisible: false
                        z: 2

                        rowDelegate: Rectangle {
                            color: myPalette.window

                            height: Screen.pixelDensity * 8.
                            width: viewModeView.width
                        }

                        model: ListModel {
                            id: pageModel

                            ListElement {
                                name: 'Now Playing'
                            }

                            ListElement {
                                name: 'Albums'
                            }

                            ListElement {
                                name: 'Artists'
                            }
                        }

                        itemDelegate: Rectangle {
                            height: Screen.pixelDensity * 8.
                            width: viewModeView.width
                            color: (styleData.selected ? myPalette.highlight : myPalette.window)
                            Text {
                                id: nameLabel
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: Screen.pixelDensity * 2.
                                anchors.rightMargin: Screen.pixelDensity * 2.
                                verticalAlignment: "AlignVCenter"

                                text: model.name

                                color: (styleData.selected ? myPalette.highlightedText : myPalette.text)

                            }
                        }

                        TableViewColumn {
                            role: 'name'
                            width: viewModeView.width
                        }

                        onRowCountChanged:
                        {
                            viewModeView.selection.clear()
                            viewModeView.currentRow = 1
                            viewModeView.selection.select(1)
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    RowLayout {
                        anchors.fill: parent
                        spacing: 0

                        id: contentZone

                        Item {
                            id: mainContentView

                            Layout.leftMargin: Screen.pixelDensity * 0.5
                            Layout.rightMargin: Screen.pixelDensity * 0.5

                            Layout.fillHeight: true

                            Layout.minimumWidth: 0
                            Layout.maximumWidth: 0
                            Layout.preferredWidth: 0

                            //z: 1
                            visible: Layout.minimumWidth != 0

                            MediaBrowser {
                                id: localAlbums

                                anchors.fill: parent

                                firstPage: MediaAllAlbumView {
                                    playListModel: playListModelItem
                                    playerControl: manageAudioPlayer
                                    stackView: localAlbums.stackView
                                    contentDirectoryModel: allAlbumsModel
                                }

                                visible: opacity > 0
                            }

                            MediaBrowser {
                                id: localArtists

                                anchors.fill: parent

                                firstPage: MediaAllArtistView {
                                    playListModel: playListModelItem
                                    artistsModel: allArtistsModel
                                    playerControl: manageAudioPlayer
                                    stackView: localArtists.stackView
                                    contentDirectoryModel: allAlbumsModel
                                }

                                visible: opacity > 0
                            }
                        }

                        Rectangle {
                            id: firstViewSeparatorItem

                            border.width: 1
                            border.color: myPalette.mid
                            color: myPalette.mid
                            visible: true

                            Layout.bottomMargin: Screen.pixelDensity * 0.5
                            Layout.topMargin: Screen.pixelDensity * 0.5

                            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                            Layout.preferredHeight: parent.height - Screen.pixelDensity * 5.
                            Layout.minimumHeight: parent.height - Screen.pixelDensity * 5.
                            Layout.maximumHeight: parent.height - Screen.pixelDensity * 5.

                            Layout.preferredWidth: 1
                            Layout.minimumWidth: 1
                            Layout.maximumWidth: 1
                        }

                        MediaPlayListView {
                            id: playList

                            playListModel: playListModelItem
                            playListControler: playListControlerItem

                            randomPlayChecked: playListControlerItem.randomPlayControl
                            repeatPlayChecked: playListControlerItem.repeatPlayControl

                            Layout.fillHeight: true

                            Layout.minimumWidth: contentZone.width
                            Layout.maximumWidth: contentZone.width
                            Layout.preferredWidth: contentZone.width

                            Component.onCompleted:
                            {
                                playListControlerItem.randomPlay = Qt.binding(function() { return playList.randomPlayChecked })
                                playListControlerItem.repeatPlay = Qt.binding(function() { return playList.repeatPlayChecked })
                                myPlayControlManager.randomOrContinuePlay = Qt.binding(function() { return playList.randomPlayChecked || playList.repeatPlayChecked })
                            }
                        }

                        Rectangle {
                            id: viewSeparatorItem

                            border.width: 1
                            border.color: myPalette.mid
                            color: myPalette.mid
                            visible: Layout.minimumWidth != 0

                            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                            Layout.preferredHeight: parent.height - Screen.pixelDensity * 5.

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

                    states: [
                        State {
                            name: 'allAlbums'
                            when: viewModeView.currentRow === 1
                            PropertyChanges {
                                target: mainContentView
                                Layout.minimumWidth: contentZone.width * 0.66
                                Layout.maximumWidth: contentZone.width * 0.66
                                Layout.preferredWidth: contentZone.width * 0.66
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
                        },
                        State {
                            name: 'allArtists'
                            when: viewModeView.currentRow === 2
                            PropertyChanges {
                                target: mainContentView
                                Layout.minimumWidth: contentZone.width * 0.66
                                Layout.maximumWidth: contentZone.width * 0.66
                                Layout.preferredWidth: contentZone.width * 0.66
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
                        },
                        State {
                            name: 'full'
                            when: viewModeView.currentRow == 0
                            PropertyChanges {
                                target: mainContentView
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

