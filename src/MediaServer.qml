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
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
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

    property var helpAction: elisa.action("help_contents")
    property var quitApplication: elisa.action("file_quit")
    property var reportBugAction: elisa.action("help_report_bug")
    property var aboutAppAction: elisa.action("help_about_app")
    property var configureShortcutsAction: elisa.action("options_configure_keybinding")

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

        property double playControlItemVolume : 1.0
        property bool playControlItemMuted : false
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
            persistentSettings.playListControlerState = playListControlerItem.persistentState;
            persistentSettings.audioPlayerState = manageAudioPlayer.persistentState

            persistentSettings.playControlItemVolume = headerBar.playerControl.volume
            persistentSettings.playControlItemMuted = headerBar.playerControl.muted
        }
    }

    PlatformIntegration {
        id: platformInterface

        playListModel: playListModelItem
        playListControler: playListControlerItem
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
    }

    AudioWrapper {
        id: audioPlayer

        muted: headerBar.playerControl.muted

        volume: headerBar.playerControl.volume * 100

        onVolumeChanged: headerBar.playerControl.volume = volume / 100.0
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
        playerPosition: audioPlayer.position

        persistentState: persistentSettings.audioPlayerState

        onPlayerPlay: audioPlayer.play()
        onPlayerPause: audioPlayer.pause()
        onPlayerStop: audioPlayer.stop()
        onSkipNextTrack: playListControlerItem.skipNextTrack()
        onSeek: audioPlayer.seek(position)
    }

    ManageMediaPlayerControl {
        id: myPlayControlManager

        playListModel: playListModelItem
        currentTrack: playListControlerItem.currentTrack
    }

    AllAlbumsModel {
        id: allAlbumsModel
    }

    AllTracksModel {
        id: allTracksModel
    }

    Connections {
        target: allListeners

        onAlbumAdded: allAlbumsModel.albumAdded(newAlbum)
    }

    Connections {
        target: allListeners

        onAlbumRemoved: allAlbumsModel.albumRemoved(removedAlbum)
    }

    Connections {
        target: allListeners

        onAlbumModified: allAlbumsModel.albumModified(modifiedAlbum)
    }

    Connections {
        target: allListeners

        onTrackAdded: allTracksModel.trackAdded(id)
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
            text: configureShortcutsAction.text
            shortcut: configureShortcutsAction.shortcut
            iconName: elisa.iconName(configureShortcutsAction.icon)
            onTriggered: configureShortcutsAction.trigger()
            visible: configureShortcutsAction.text !== ""
        }

        MenuSeparator {
            visible: configureShortcutsAction.text !== ""
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

                    playerControl.onPlayPrevious: playListControlerItem.skipPreviousTrack()
                    playerControl.onPlayNext: playListControlerItem.skipNextTrack()

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
                }
            }

            RowLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 0

                Rectangle {
                    color: myPalette.window

                    Layout.fillHeight: true
                    Layout.preferredWidth: mainWindow.width * 0.15
                    Layout.maximumWidth: mainWindow.width * 0.15

                    ScrollView {
                        flickableItem.boundsBehavior: Flickable.StopAtBounds

                        anchors.fill: parent

                        ListView {
                            id: viewModeView

                            focus: true
                            z: 2

                            model: DelegateModel {
                                id: pageDelegateModel
                                groups: [
                                    DelegateModelGroup { name: "selected" }
                                ]

                                model: ListModel {
                                    id: pageModel
                                }

                                delegate: Rectangle {
                                    id: item

                                    height: elisaTheme.viewSelectorDelegateHeight
                                    width: viewModeView.width
                                    color: (DelegateModel.inSelected ? myPalette.highlight : myPalette.window)

                                    MouseArea {
                                        anchors.fill: parent

                                        acceptedButtons: Qt.LeftButton

                                        LabelWithToolTip {
                                            id: nameLabel
                                            anchors.verticalCenter: parent.verticalCenter
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            anchors.leftMargin: elisaTheme.layoutHorizontalMargin
                                            anchors.rightMargin: elisaTheme.layoutHorizontalMargin
                                            verticalAlignment: "AlignVCenter"

                                            text: model.name

                                            color: (item.DelegateModel.inSelected ? myPalette.highlightedText : myPalette.text)
                                        }

                                        onClicked:
                                        {
                                            var myGroup = pageDelegateModel.groups[2]
                                            if (myGroup.count > 0 && !item.DelegateModel.inSelected) {
                                                myGroup.remove(0, myGroup.count)
                                            }

                                            item.DelegateModel.inSelected = !item.DelegateModel.inSelected

                                            if (item.DelegateModel.inSelected)
                                            {
                                                viewModeView.currentIndex = index
                                            }
                                        }
                                    }
                                }

                                Component.onCompleted:
                                {
                                    pageModel.insert(0, {"name": i18nc("Title of the view of the playlist", "Now Playing")})
                                    pageModel.insert(1, {"name": i18nc("Title of the view of all albums", "Albums")})
                                    pageModel.insert(2, {"name": i18nc("Title of the view of all artists", "Artists")})
                                    pageModel.insert(3, {"name": i18nc("Title of the view of all tracks", "Tracks")})

                                    items.get(1).inSelected = 1
                                    viewModeView.currentIndex = 1
                                }
                            }
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

                            Layout.leftMargin: elisaTheme.layoutHorizontalMargin
                            Layout.rightMargin: elisaTheme.layoutHorizontalMargin

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
                                    musicListener: allListeners
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
                                    musicListener: allListeners
                                    contentDirectoryModel: allAlbumsModel
                                }

                                visible: opacity > 0
                            }

                            MediaBrowser {
                                id: localTracks

                                anchors.fill: parent

                                firstPage: MediaAllTracksView {
                                    playListModel: playListModelItem
                                    tracksModel: allTracksModel
                                    playerControl: manageAudioPlayer
                                    stackView: localTracks.stackView
                                    musicListener: allListeners
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

                            Layout.bottomMargin: elisaTheme.layoutVerticalMargin
                            Layout.topMargin: elisaTheme.layoutVerticalMargin

                            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                            Layout.preferredHeight: parent.height - elisaTheme.layoutVerticalMargin
                            Layout.minimumHeight: parent.height - elisaTheme.layoutVerticalMargin
                            Layout.maximumHeight: parent.height - elisaTheme.layoutVerticalMargin

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

                            Layout.preferredHeight: parent.height - elisaTheme.layoutVerticalMargin

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
                            name: 'full'
                            when: viewModeView.currentIndex === 0
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
                            when: viewModeView.currentIndex === 1
                            PropertyChanges {
                                target: mainContentView
                                Layout.fillWidth: true
                                Layout.minimumWidth: contentZone.width * 0.64
                                Layout.maximumWidth: contentZone.width * 0.66
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
                            when: viewModeView.currentIndex === 2
                            PropertyChanges {
                                target: mainContentView
                                Layout.fillWidth: true
                                Layout.minimumWidth: contentZone.width * 0.64
                                Layout.maximumWidth: contentZone.width * 0.66
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
                            when: viewModeView.currentIndex === 3
                            PropertyChanges {
                                target: mainContentView
                                Layout.fillWidth: true
                                Layout.minimumWidth: contentZone.width * 0.64
                                Layout.maximumWidth: contentZone.width * 0.66
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
