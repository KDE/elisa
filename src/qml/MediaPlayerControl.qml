/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>
   SPDX-FileCopyrightText: 2023 (c) ivan tkachenko <me@ratijas.tk>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import ElisaGraphicalEffects 1.15
import QtQuick.Controls 2.7
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

import "shared"

BasePlayerControl {
    id: musicWidget

    property alias volume: volumeSlider.value
    property bool isMaximized
    property bool isTranslucent
    property bool isNearCollapse

    signal maximize()
    signal minimize()
    /*
      Emmited when User uses the Item as a handle to resize the layout.
      y: difference to previous position
      offset: cursor offset (y coordinate relative to this Item, where dragging begun)
      */
    signal handlePositionChanged(int y, int offset)

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    Rectangle {
        anchors.fill: parent

        Kirigami.Theme.colorSet: Kirigami.Theme.Header
        Kirigami.Theme.inherit: false

        color: isTranslucent ? myPalette.midlight : Kirigami.Theme.backgroundColor
        opacity: isTranslucent ? elisaTheme.mediaPlayerControlOpacity : 1.0

        Kirigami.Separator {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            visible: !isTranslucent
        }

        MouseArea {
            property int dragStartOffset: 0

            anchors.fill: parent
            cursorShape: isMaximized ? Qt.ArrowCursor : Qt.SizeVerCursor

            onPressed: {
                dragStartOffset = mouse.y
            }

            onPositionChanged: {
                musicWidget.handlePositionChanged(mouse.y, dragStartOffset)
            }

            drag.axis: Drag.YAxis
            drag.threshold: 1
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Item { implicitWidth: Math.floor(Kirigami.Units.smallSpacing / 2) }

        FlatButtonWithToolTip {
            id: minimizeMaximizeButton
            text: i18nc("@action:button", "Toggle Party Mode")
            icon.name: musicWidget.isMaximized ? "draw-arrow-up" : "draw-arrow-down"
            onClicked: musicWidget.isMaximized = !musicWidget.isMaximized
        }

        FlatButtonWithToolTip {
            id: skipBackwardButton
            enabled: skipBackwardEnabled
            text: i18nc("@action:button", "Skip Backward")
            icon.name: musicWidget.LayoutMirroring.enabled ? "media-skip-forward" : "media-skip-backward"
            onClicked: musicWidget.playPrevious()
        }

        FlatButtonWithToolTip {
            id: playPauseButton
            enabled: musicWidget.playEnabled
            text: musicWidget.isPlaying ? i18nc("@action:button Pause any media that is playing", "Pause") : i18nc("@action:button Start playing media", "Play")
            icon.name: musicWidget.isPlaying ? "media-playback-pause" : "media-playback-start"
            onClicked: musicWidget.isPlaying ? musicWidget.pause() : musicWidget.play()
        }

        FlatButtonWithToolTip {
            id: skipForwardButton
            enabled: skipForwardEnabled
            text: i18nc("@action:button skip forward in playlists", "Skip Forward")
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
            onSeek: position => musicWidget.seek(position)

            labelColor: myPalette.text
        }

        FlatButtonWithToolTip {
            id: muteButton
            text: i18nc("@action:button", "Toggle Mute")
            icon.name: musicWidget.muted ? "player-volume-muted" : "player-volume"
            onClicked: musicWidget.muted = !musicWidget.muted
        }

        VolumeSlider {
            id: volumeSlider
            Layout.preferredWidth: elisaTheme.volumeSliderWidth
            Layout.maximumWidth: elisaTheme.volumeSliderWidth
            Layout.minimumWidth: elisaTheme.volumeSliderWidth
            Layout.fillHeight: true

            muted: musicWidget.muted
        }

        Item { implicitWidth: Kirigami.Units.largeSpacing }

        FlatButtonWithToolTip {
            id: shuffleButton
            text: i18nc("@action:button", "Toggle Shuffle")
            icon.name: "media-playlist-shuffle"
            checkable: true
            checked: ElisaApplication.mediaPlayListProxyModel.shufflePlayList
            onClicked: ElisaApplication.mediaPlayListProxyModel.shufflePlayList = !ElisaApplication.mediaPlayListProxyModel.shufflePlayList
        }

        FlatButtonWithToolTip {
            id: repeatButton
            text: {
                const map = {
                    [MediaPlayListProxyModel.None]: i18nc("@info:tooltip", "Current: Don't repeat tracks"),
                    [MediaPlayListProxyModel.One]: i18nc("@info:tooltip", "Current: Repeat current track"),
                    [MediaPlayListProxyModel.Playlist]: i18nc("@info:tooltip", "Current: Repeat all tracks in playlist"),
                }
                return map[ElisaApplication.mediaPlayListProxyModel.repeatMode]
            }
            icon.name: {
                const map = {
                    [MediaPlayListProxyModel.None]: "media-repeat-none",
                    [MediaPlayListProxyModel.One]: "media-repeat-single",
                    [MediaPlayListProxyModel.Playlist]: "media-repeat-all",
                }
                return map[ElisaApplication.mediaPlayListProxyModel.repeatMode]
            }

            checkable: true
            checked: ElisaApplication.mediaPlayListProxyModel.repeatMode !== 0
            onClicked: {
                ElisaApplication.mediaPlayListProxyModel.repeatMode = (ElisaApplication.mediaPlayListProxyModel.repeatMode + 1) % 3
            }

            menuPolicy: FlatButtonWithToolTip.MenuPolicy.OnPressAndHold
            menu: Menu {
                PlaylistModeItem {
                    text: i18nc("@action:inmenu", "Playlist")
                    mode: MediaPlayListProxyModel.Playlist
                }
                PlaylistModeItem {
                    text: i18nc("@action:inmenu", "One")
                    mode: MediaPlayListProxyModel.One
                }
                PlaylistModeItem {
                    text: i18nc("@action:inmenu", "None")
                    mode: MediaPlayListProxyModel.None
                }
            }

            Kirigami.Icon {
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: Kirigami.Units.smallSpacing
                }
                width: Math.round(Kirigami.Units.iconSizes.small / 2)
                height: Math.round(Kirigami.Units.iconSizes.small / 2)
                source: "arrow-down"
            }
        }

        FlatButtonWithToolTip {
            // normally toggles the playlist in contentView, but when the headerbar is too narrow to
            // show the playlistDrawer handle, this opens the drawer instead

            id: showHidePlaylistAction
            property bool _togglesDrawer: mainWindow.width < elisaTheme.viewSelectorSmallSizeThreshold

            action: Action {
                shortcut: ElisaApplication.actionShortcut(ElisaApplication.action("toggle_playlist"))
                onTriggered: {
                    if (showHidePlaylistAction._togglesDrawer) {
                        playlistDrawer.visible = !playlistDrawer.visible
                    } else {
                        contentView.showPlaylist = !contentView.showPlaylist
                    }
                }
            }

            visible: !musicWidget.isMaximized && (!_togglesDrawer || musicWidget.isNearCollapse)

            display: _togglesDrawer ? AbstractButton.IconOnly : AbstractButton.TextBesideIcon
            text: i18nc("@action:button", "Show Playlist")
            icon.name: "view-media-playlist"

            checkable: true
            checked: _togglesDrawer ? playlistDrawer.visible : contentView.showPlaylist
        }

        FlatButtonWithToolTip {
            id: menuButton

            text: i18nc("@action:button", "Application Menu")
            icon.name: "open-menu-symbolic"

            menuPolicy: FlatButtonWithToolTip.MenuPolicy.OnClickOrPressOrEnter
            menu: ApplicationMenu {
                // without modal, clicking on menuButton will not close the menu
                modal: true
                dim: false
            }
        }

        Item {
            implicitWidth: Math.floor(Kirigami.Units.smallSpacing / 2)
        }
    }

    onIsMaximizedChanged: {
        if (musicWidget.isMaximized) {
            musicWidget.maximize()
        } else {
            musicWidget.minimize()
        }
    }

    Component.onCompleted: {
        for (const element of [
                menuButton,
                repeatButton,
                shuffleButton,
                muteButton,
                skipForwardButton,
                skipBackwardButton,
                playPauseButton,
                minimizeMaximizeButton,
        ]) {
            ElisaApplication.installKeyEventFilter(element)
        }
    }
}
