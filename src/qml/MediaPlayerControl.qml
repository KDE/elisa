/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.7
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa

import "shared"

BasePlayerControl {
    id: musicWidget

    property alias volume: volumeSlider.value
    property bool isMaximized
    property bool isTranslucent
    property bool isNearCollapse

    implicitHeight: Math.max(toolBarLayout.implicitHeight, Math.round(Kirigami.Units.gridUnit * 2.5))

    signal maximize()
    signal minimize()
    /*
      Emmited when User uses the Item as a handle to resize the layout.
      y: difference to previous position
      offset: cursor offset (y coordinate relative to this Item, where dragging begun)
      */
    signal handlePositionChanged(int y, int offset)

    onIsMaximizedChanged: isMaximized ? maximize() : minimize()

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

            onPressed: mouse => {
                dragStartOffset = mouse.y
            }

            onPositionChanged: mouse => musicWidget.handlePositionChanged(mouse.y, dragStartOffset)

            drag.axis: Drag.YAxis
            drag.threshold: 1
        }
    }

    RowLayout {
        id: toolBarLayout

        anchors.fill: parent
        spacing: 0

        // media controls mirror IRL media playback devices,
        // which are not mirrored in countries that use RtL languages
        LayoutMirroring.enabled: false
        LayoutMirroring.childrenInherit: true

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
            icon.name: "media-skip-backward"
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
            icon.name: "media-skip-forward"
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

        RowLayout {
            spacing: 0

            LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
            LayoutMirroring.childrenInherit: true

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
        }

        Item { implicitWidth: Kirigami.Units.largeSpacing }

        FlatButtonWithToolTip {
            id: shuffleButton
            text: {
                const map = {
                    [MediaPlayListProxyModel.NoShuffle]: i18nc("@info:tooltip", "Current: No shuffle"),
                    [MediaPlayListProxyModel.Track]: i18nc("@info:tooltip", "Current: Shuffle tracks"),
                    [MediaPlayListProxyModel.Album]: i18nc("@info:tooltip", "Current: Shuffle albums"),
                }
                return map[ElisaApplication.mediaPlayListProxyModel.shuffleMode]
            }
            icon.name: {
                const map = {
                    [MediaPlayListProxyModel.NoShuffle]: "media-playlist-no-shuffle-symbolic",
                    [MediaPlayListProxyModel.Track]: "media-playlist-shuffle",
                    [MediaPlayListProxyModel.Album]: "media-random-albums-amarok",
                }
                return map[ElisaApplication.mediaPlayListProxyModel.shuffleMode]
            }

            down: pressed || menu.visible
            Accessible.role: Accessible.ButtonMenu

            checkable: true
            checked: ElisaApplication.mediaPlayListProxyModel.shuffleMode !== 0

            onClicked: {
                ElisaApplication.mediaPlayListProxyModel.shuffleMode = (ElisaApplication.mediaPlayListProxyModel.shuffleMode + 1) % 3
            }
            onPressAndHold: {
                menu.popup()
            }

            menu: Menu {
                ShuffleModeItem {
                    text: i18nc("@action:inmenu", "Track")
                    mode: MediaPlayListProxyModel.Track
                }
                ShuffleModeItem {
                    text: i18nc("@action:inmenu", "Album")
                    mode: MediaPlayListProxyModel.Album
                }
                ShuffleModeItem {
                    text: i18nc("@action:inmenu", "None")
                    mode: MediaPlayListProxyModel.NoShuffle
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

            down: pressed || menu.visible
            Accessible.role: Accessible.ButtonMenu

            checkable: true
            checked: ElisaApplication.mediaPlayListProxyModel.repeatMode !== 0
            onClicked: {
                ElisaApplication.mediaPlayListProxyModel.repeatMode = (ElisaApplication.mediaPlayListProxyModel.repeatMode + 1) % 3
            }
            onPressAndHold: {
                menu.popup()
            }

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
                checkable: true
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

            checked: _togglesDrawer ? playlistDrawer.visible : contentView.showPlaylist
        }

        FlatButtonWithToolTip {
            id: menuButton

            function openMenu() {
                if (!menu.visible) {
                    menu.open()
                } else {
                    menu.dismiss()
                }
            }

            text: i18nc("@action:button", "Application Menu")
            icon.name: "open-menu-symbolic"

            down: pressed || menu.visible
            Accessible.role: Accessible.ButtonMenu

            onPressed: openMenu()
            // Need this too because the base control sends onClicked for return/enter
            onClicked: openMenu()

            menu: ApplicationMenu {
                y: menuButton.height
                // without modal, clicking on menuButton will not close the menu
                modal: true
                dim: false
            }
        }

        Item {
            implicitWidth: Math.floor(Kirigami.Units.smallSpacing / 2)
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
