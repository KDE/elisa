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
import QtMultimedia 5.4

ApplicationWindow {
    visible: true
    minimumWidth: 1000
    minimumHeight: 600
    title: 'Music'
    id: mainWindow

    property UpnpDeviceDescription aDevice
    property UpnpControlConnectionManager connectionManager

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
        text: qsTr("&Quit")
        shortcut: StandardKey.Quit
        onTriggered: Qt.quit()
    }

    Mpris2 {
        id: mpris2Interface

        playerName: 'elisa'
    }

    UpnpSsdpEngine {
        id: mySsdpEngine

        Component.onCompleted: {
            mySsdpEngine.initialize();
            mySsdpEngine.searchAllUpnpDevice();
        }

        onNewService: viewModeModel.newDevice(serviceDiscovery)

        onRemovedService: viewModeModel.removedDevice(serviceDiscovery)
    }

    DatabaseInterface {
        id: localAlbumDatabase
    }

    ViewPagesModel {
        id: viewModeModel

        deviceId: 'urn:schemas-upnp-org:device:MediaServer:1'

        browseFlag: globalBrowseFlag
        filter: globalFilter
        sortCriteria: globalSortCriteria
        albumDatabase: localAlbumDatabase

        property var mediaServiceComponent
        property var mediaViewComponent

        onRowsInserted: {
            mainContentView.insertTab(first, '', Qt.createComponent(Qt.resolvedUrl('MediaContentDirectory.qml')))
            mainContentView.getTab(first).active = true
            mainContentView.getTab(first).item.pagesModel = viewModeModel
            mainContentView.getTab(first).item.playListModel = playListModelItem
            mainContentView.getTab(first).item.width = mainContentView.width
            mainContentView.getTab(first).item.height = mainContentView.height
            mainContentView.getTab(first).item.z = 0
            mainContentView.getTab(first).item.contentDirectoryModel = viewModeModel.remoteAlbumModel(first)
        }
    }

    Audio {
        id: audioPlayer

        muted: playControlItem.muted

        volume: playControlItem.volume
        source: playListControler.playerSource

        onPlaying: playListControler.playerPlaying()
        onPaused: playListControler.playerPaused()
        onStopped: playListControler.playerStopped()
        onPositionChanged: playListControler.audioPlayerPositionChanged(position)
        onStatusChanged: playListControler.audioPlayerFinished(status == Audio.EndOfMedia)
    }

    MediaPlayList {
        id: playListModelItem

        onTrackHasBeenAdded: mainWindow.showPassiveNotification('Track ' + title + ' has been added', 1500, "", {})
    }

    PlayListControler {
        id: playListControler

        playListModel: playListModelItem

        urlRole: MediaPlayList.ResourceRole
        isPlayingRole: MediaPlayList.IsPlayingRole
        artistRole: MediaPlayList.ArtistRole
        titleRole: MediaPlayList.TitleRole
        albumRole: MediaPlayList.AlbumRole
        imageRole: MediaPlayList.ImageRole

        audioDuration: audioPlayer.duration

        onPlayMusic: audioPlayer.play()
        onPauseMusic: audioPlayer.pause()
        onStopMusic: audioPlayer.stop()
        onAudioPositionChanged: audioPlayer.seek(audioPosition)
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

            tracksCount: playListControler.remainingTracks
            album: playListControler.album
            title: playListControler.title
            artist: playListControler.artist
            image: playListControler.image
            ratingVisible: false
        }

        MediaPlayerControl {
            id: playControlItem

            duration: audioPlayer.duration
            seekable: audioPlayer.seekable

            volume: 1.0
            position: playListControler.playControlPosition
            skipBackwardEnabled: playListControler.skipBackwardControlEnabled
            skipForwardEnabled: playListControler.skipForwardControlEnabled
            playEnabled: playListControler.playControlEnabled
            isPlaying: playListControler.musicPlaying

            Layout.preferredHeight: Screen.pixelDensity * 10.
            Layout.minimumHeight: Layout.preferredHeight
            Layout.maximumHeight: Layout.preferredHeight
            Layout.fillWidth: true

            onSeek: playListControler.playerSeek(position)
            onPlay: playListControler.playPause()
            onPause: playListControler.playPause()
            onPlayPrevious: playListControler.skipPreviousTrack()
            onPlayNext: playListControler.skipNextTrack()
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

                model: viewModeModel

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
                    MediaContentDirectory {
                        id: localAlbums

                        pagesModel: viewModeModel
                        playListModel: playListModelItem
                        contentDirectoryModel: LocalAlbumModel {
                        }

                        anchors.fill: parent
                    }
                }

                Tab {
                    Rectangle {
                        anchors.fill: parent
                        color: "#FFFFFF"
                    }

                    MediaPlayListView {
                        id: playList

                        playControl: playControlItem
                        playListModel: playListModelItem

                        artistName: playListControler.artist
                        albumName: playListControler.album
                        albumArtUrl: playListControler.image

                        anchors.fill: parent
                        anchors.margins: 3

                        Component.onCompleted:
                        {
                            playListControler.randomPlay = Qt.binding(function() { return playList.randomPlayChecked })
                            playListControler.repeatPlay = Qt.binding(function() { return playList.repeatPlayChecked })
                        }
                    }
                }
            }
        }
    }
}

