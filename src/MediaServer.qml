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

    Settings {
        id: persistentSettings

        property int x
        property int y
        property int width
        property int height

        property var playListState

        property var playListControlerState

        property var audioPlayerState
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
        text: i18nc('Action to quit the application', '&Quit')
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

    DatabaseInterface {
        id: localAlbumDatabase

        Component.onCompleted: {
            init('views');
            initRequest();
        }
    }

    MusicListenersManager {
        viewDatabase: localAlbumDatabase
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

        databaseInterface: localAlbumDatabase

        persistentState: persistentSettings.playListState

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

            volume: 1.0
            position: audioPlayer.position
            skipBackwardEnabled: myPlayControlManager.skipBackwardControlEnabled
            skipForwardEnabled: myPlayControlManager.skipForwardControlEnabled
            playEnabled: myPlayControlManager.playControlEnabled
            isPlaying: myPlayControlManager.musicPlaying

            Layout.preferredHeight: Screen.pixelDensity * 10.
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

            TableView {
                id: viewModeView

                Layout.fillHeight: true
                Layout.preferredWidth: Screen.pixelDensity * 50.
                Layout.minimumWidth: Screen.pixelDensity * 50.
                Layout.maximumWidth: Screen.pixelDensity * 50.

                headerVisible: false
                frameVisible: false
                focus: true
                backgroundVisible: false
                z: 2

                rowDelegate: Rectangle {
                    color: '#EFF0F1'

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
                }

                itemDelegate: Rectangle {
                    height: Screen.pixelDensity * 8.
                    width: viewModeView.width
                    color: if (styleData.selected)
                               '#3DAEE9'
                           else
                               '#EFF0F1'
                    Label {
                        id: nameLabel
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: Screen.pixelDensity * 2.
                        anchors.rightMargin: Screen.pixelDensity * 2.
                        verticalAlignment: "AlignVCenter"

                        text: model.name

                        color: if (styleData.selected)
                                   'white'
                               else
                                   'black'

                    }
                }

                TableViewColumn {
                    role: 'name'
                    width: viewModeView.width
                }

                onCurrentRowChanged:
                {
                    mainContentView.currentIndex = currentRow
                }

                onRowCountChanged:
                {
                    viewModeView.selection.clear()
                    viewModeView.currentRow = 0
                    viewModeView.selection.select(0)
                }
            }

            TabView {
                id: mainContentView

                tabsVisible: false

                Layout.fillHeight: true
                Layout.fillWidth: true
                z: 1

                Rectangle {
                    anchors.fill: parent
                    color: "#FFFFFF"
                }

                Tab {
                    Rectangle {
                        anchors.fill: parent
                        color: "#FFFFFF"
                    }

                    MediaPlayListView {
                        id: playList

                        playListModel: playListModelItem

                        randomPlayChecked: playListControlerItem.randomPlayControl
                        repeatPlayChecked: playListControlerItem.repeatPlayControl

                        artistName: myHeaderBarManager.artist
                        albumName: myHeaderBarManager.album
                        albumArtUrl: myHeaderBarManager.image

                        anchors.fill: parent
                        anchors.margins: 3

                        Component.onCompleted:
                        {
                            playListControlerItem.randomPlay = Qt.binding(function() { return playList.randomPlayChecked })
                            playListControlerItem.repeatPlay = Qt.binding(function() { return playList.repeatPlayChecked })
                        }
                    }
                }

                Tab {
                    MediaContentDirectory {
                        id: localAlbums

                        playListModel: playListModelItem
                        musicDatabase: localAlbumDatabase

                        anchors.fill: parent
                    }
                }
            }
        }
    }
}

