/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.7
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

import "shared"

BasePlayerControl {
    id: musicWidget

    property alias volume: volumeSlider.sliderValue
    property bool isMaximized

    signal maximize()
    signal minimize()

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    Rectangle {
        anchors.fill: parent

        color: myPalette.midlight
        opacity: elisaTheme.mediaPlayerControlOpacity
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Item { implicitWidth: Math.floor(Kirigami.Units.smallSpacing / 2) }

        FlatButtonWithToolTip {
            id: minimizeMaximizeButton
            text: i18nc("toggle the party mode", "Toggle Party Mode")
            icon.name: musicWidget.isMaximized ? "draw-arrow-up" : "draw-arrow-down"
            onClicked: musicWidget.isMaximized = !musicWidget.isMaximized
        }

        FlatButtonWithToolTip {
            id: skipBackwardButton
            enabled: skipBackwardEnabled
            text: i18nc("skip backward in playlists", "Skip Backward")
            icon.name: musicWidget.LayoutMirroring.enabled ? "media-skip-forward" : "media-skip-backward"
            onClicked: musicWidget.playPrevious()
        }

        FlatButtonWithToolTip {
            id: playPauseButton
            enabled: playEnabled
            text: i18nc("toggle play and pause for the audio player", "Toggle Play and Pause")
            icon.name: musicWidget.isPlaying? "media-playback-pause" : "media-playback-start"
            onClicked: musicWidget.isPlaying ? musicWidget.pause() : musicWidget.play()
        }

        FlatButtonWithToolTip {
            id: skipForwardButton
            enabled: skipForwardEnabled
            text: i18nc("skip forward in playlists", "Skip Forward")
            icon.name: musicWidget.LayoutMirroring.enabled ? "media-skip-backward" : "media-skip-forward"
            onClicked: musicWidget.playNext()
        }

        DurationSlider {
            Layout.fillWidth: true
            Layout.fillHeight: true
            position: musicWidget.position
            duration: musicWidget.duration
            seekable: musicWidget.seekable
            playEnabled: musicWidget.playEnabled
            onSeek: musicWidget.seek(position)

            labelColor: myPalette.text
            sliderElapsedColor: myPalette.text
            sliderRemainingColor: myPalette.dark
            sliderHandleColor: myPalette.base
            sliderBorderInactiveColor: myPalette.dark
            sliderBorderActiveColor: myPalette.text
        }

        FlatButtonWithToolTip {
            id: muteButton
            text: i18nc("toggle mute mode for player", "Toggle Mute")
            icon.name: musicWidget.muted ? "player-volume-muted" : "player-volume"
            onClicked: musicWidget.muted = !musicWidget.muted
        }

        VolumeSlider {
            id: volumeSlider
            Layout.preferredWidth: elisaTheme.volumeSliderWidth
            Layout.maximumWidth: elisaTheme.volumeSliderWidth
            Layout.minimumWidth: elisaTheme.volumeSliderWidth
            Layout.fillHeight: true

            interactHeight: height
            interactWidth: width
            muted: muted
            sliderElapsedColor: myPalette.text
            sliderRemainingColor: myPalette.dark
            sliderHandleColor: myPalette.base
            sliderBorderInactiveColor: myPalette.dark
            sliderBorderActiveColor: myPalette.text
        }

        Item { implicitWidth: Kirigami.Units.largeSpacing }

        FlatButtonWithToolTip {
            id: shuffleButton
            text: i18nc("toggle shuffle mode for playlist", "Toggle Shuffle")
            icon.name: "media-playlist-shuffle"
            onClicked: musicWidget.shuffle = !musicWidget.shuffle
            checkable: true
            checked: musicWidget.shuffle
        }

        FlatButtonWithToolTip {
            id: repeatButton
            text: {
                const map = {
                    0: i18n("Current: Don't repeat tracks"),
                    1: i18n("Current: Repeat current track"),
                    2: i18n("Current: Repeat all tracks in playlist")
                }
                return map[musicWidget.repeat]
            }
            icon.name: {
                const map = {
                    0: "media-repeat-none",
                    1: "media-repeat-single",
                    2: "media-repeat-all"
                }
                return map[musicWidget.repeat]
            }

            checkable: true
            checked: repeat !== 0
            onClicked: {
                let nextRepeat = musicWidget.repeat + 1
                if (nextRepeat >= 3) {
                    nextRepeat = 0
                }
                musicWidget.repeat = nextRepeat
            }
            onPressAndHold: {
                playlistModeMenu.popup()
            }

            Menu {
                id: playlistModeMenu

                PlaylistModeItem {
                    text: i18n("Playlist")
                    mode: MediaPlayListProxyModel.Playlist
                }
                PlaylistModeItem {
                    text: i18n("One")
                    mode: MediaPlayListProxyModel.One
                }
                PlaylistModeItem {
                    text: i18n("None")
                    mode: MediaPlayListProxyModel.None
                }
            }
        }

        FlatButtonWithToolTip {
            id: showHidePlaylistAction
            action: Action {
                shortcut: ElisaApplication.action("toggle_playlist").shortcut
                onTriggered: contentView.showPlaylist = !contentView.showPlaylist
            }

            visible: !musicWidget.isMaximized && mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold

            display: AbstractButton.TextBesideIcon
            text: i18n("Show Playlist")
            icon.name: "view-media-playlist"

            checkable: true
            checked: contentView.showPlaylist
        }

        FlatButtonWithToolTip {
            id: menuButton

            text: i18nc("open application menu", "Application Menu")
            icon.name: "application-menu"

            checkable: true
            checked: applicationMenu.visible

            onClicked: {
                if (applicationMenu.visible) {
                    applicationMenu.close()
                } else {
                    var pos = menuButton.mapFromItem(headerBar, headerBar.width - applicationMenu.width, headerBar.height)
                    applicationMenu.popup(pos.x, pos.y)
                }
            }

            ApplicationMenu {
                id: applicationMenu

                // otherwise clicking on the menu button will not close it
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
            }
        }

        Item { implicitWidth: Math.floor(Kirigami.Units.smallSpacing / 2) }
    }

    onIsMaximizedChanged:
    {
        if (musicWidget.isMaximized) {
            musicWidget.maximize()
        } else {
            musicWidget.minimize()
        }
    }

    Component.onCompleted: {

        var elementList = [menuButton, repeatButton, shuffleButton, muteButton, skipForwardButton, skipBackwardButton, playPauseButton, minimizeMaximizeButton]

        for (var i=0; i<elementList.length; i++)
            installFilter(elementList[i])

        function installFilter(element) {
            ElisaApplication.installKeyEventFilter(element)
        }
    }
}

