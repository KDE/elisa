/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.elisa

import "shared"

BasePlayerControl {
    id: musicWidget

    property alias volume: volumeSlider.value
    property bool isMaximized
    property bool isTranslucent

    implicitHeight: toolBar.implicitHeight

    signal maximize()
    signal minimize()
    /*
      Emitted when User uses the Item as a handle to resize the layout.
      y: difference to previous position
      offset: cursor offset (y coordinate relative to this Item, where dragging begun)
      */
    signal handlePositionChanged(int y, int offset)

    onIsMaximizedChanged: musicWidget.isMaximized ? maximize() : minimize()

    Rectangle {
        anchors.fill: parent

        color: palette.midlight
        opacity: Theme.mediaPlayerControlOpacity
    }

    ToolBar {
        id: toolBar
        anchors.fill: parent

        Kirigami.Theme.colorSet: Kirigami.Theme.Header
        Kirigami.Theme.inherit: false

        Binding {
            toolBar.background.opacity: musicWidget.isTranslucent ? 0 : 1
        }

        contentItem: Item {
            implicitHeight: toolBarLayout.height

            MouseArea {
                property int dragStartOffset: 0

                anchors.fill: parent
                cursorShape: musicWidget.isMaximized ? Qt.ArrowCursor : Qt.SizeVerCursor

                onPressed: mouse => {
                    dragStartOffset = mouse.y
                }

                onPositionChanged: mouse => musicWidget.handlePositionChanged(mouse.y, dragStartOffset)

                drag.axis: Drag.YAxis
                drag.threshold: 1
            }

            RowLayout {
                id: toolBarLayout

                anchors.fill: parent

                spacing: 0

                // media controls mirror IRL media playback devices,
                // which are not mirrored in countries that use RtL languages
                LayoutMirroring.enabled: false
                LayoutMirroring.childrenInherit: true

                FlatButtonWithToolTip {
                    id: minimizeMaximizeButton
                    text: musicWidget.isMaximized
                        ? KI18n.i18nc("@action:button displayed as @info:tooltip", "Exit party mode")
                        : KI18n.i18nc("@action:button displayed as @info:tooltip", "Enter party mode")
                    icon.name: musicWidget.isMaximized ? "draw-arrow-up" : "draw-arrow-down"
                    onClicked: musicWidget.isMaximized = !musicWidget.isMaximized
                }

                FlatButtonWithToolTip {
                    id: skipBackwardButton
                    enabled: musicWidget.skipBackwardEnabled
                    text: KI18n.i18nc("@action:button displayed as @info:tooltip", "Skip backward")
                    icon.name: "media-skip-backward"
                    onClicked: musicWidget.playPrevious()
                }

                FlatButtonWithToolTip {
                    id: playPauseButton
                    enabled: musicWidget.playEnabled
                    text: musicWidget.isPlaying
                        ? KI18n.i18nc("@action:button displayed as @info:tooltip Pause any media that is playing", "Pause")
                        : KI18n.i18nc("@action:button displayed as @info:tooltip Start playing media", "Play")
                    icon.name: musicWidget.isPlaying ? "media-playback-pause" : "media-playback-start"
                    onClicked: musicWidget.isPlaying ? musicWidget.pause() : musicWidget.play()
                }

                FlatButtonWithToolTip {
                    id: skipForwardButton
                    enabled: musicWidget.skipForwardEnabled
                    text: KI18n.i18nc("@action:button displayed as @info:tooltip skip forward in playlists", "Skip forward")
                    icon.name: "media-skip-forward"
                    onClicked: musicWidget.playNext()
                }

                DurationSlider {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.fillWidth: true
                    position: musicWidget.position
                    duration: musicWidget.duration
                    seekable: musicWidget.seekable
                    playEnabled: musicWidget.playEnabled
                    onSeek: position => musicWidget.seek(position)
                    interactionHeight: toolBarLayout.height

                    labelColor: palette.text
                }

                RowLayout {
                    spacing: 0
                    Layout.alignment: Qt.AlignVCenter

                    LayoutMirroring.enabled: Application.layoutDirection === Qt.RightToLeft
                    LayoutMirroring.childrenInherit: true

                    FlatButtonWithToolTip {
                        id: muteButton
                        text: musicWidget.muted
                            ? KI18n.i18nc("@action:button displayed as @info:tooltip", "Unmute")
                            : KI18n.i18nc("@action:button displayed as @info:tooltip", "Mute")
                        icon.name: musicWidget.muted ? "audio-volume-muted" : "audio-volume-high"
                        onClicked: musicWidget.muted = !musicWidget.muted
                    }

                    VolumeSlider {
                        id: volumeSlider
                        Layout.preferredWidth: Theme.volumeSliderWidth
                        Layout.maximumWidth: Theme.volumeSliderWidth
                        Layout.minimumWidth: Theme.volumeSliderWidth
                        Layout.alignment: Qt.AlignVCenter

                        interactionHeight: toolBarLayout.height

                        muted: musicWidget.muted
                    }
                }

                Item { implicitWidth: Kirigami.Units.largeSpacing }

                // Checkable buttons in this group benefits from the standard spacing
                RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    FlatButtonWithToolTip {
                        id: shuffleButton
                        text: {
                            const map = {
                                [MediaPlayListProxyModel.NoShuffle]: KI18n.i18nc("@info:tooltip", "Current: No shuffle"),
                                [MediaPlayListProxyModel.Track]: KI18n.i18nc("@info:tooltip", "Current: Shuffle tracks"),
                                [MediaPlayListProxyModel.Album]: KI18n.i18nc("@info:tooltip", "Current: Shuffle albums"),
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
                            (menu as Menu).popup()
                        }

                        menu: Menu {
                            ShuffleModeItem {
                                text: KI18n.i18nc("@action:inmenu", "Track")
                                mode: MediaPlayListProxyModel.Track
                            }
                            ShuffleModeItem {
                                text: KI18n.i18nc("@action:inmenu", "Album")
                                mode: MediaPlayListProxyModel.Album
                            }
                            ShuffleModeItem {
                                text: KI18n.i18nc("@action:inmenu", "None")
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
                                [MediaPlayListProxyModel.None]: KI18n.i18nc("@info:tooltip", "Current: Don't repeat tracks"),
                                [MediaPlayListProxyModel.One]: KI18n.i18nc("@info:tooltip", "Current: Repeat current track"),
                                [MediaPlayListProxyModel.Playlist]: KI18n.i18nc("@info:tooltip", "Current: Repeat all tracks in playlist"),
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
                            (menu as Menu).popup()
                        }

                        menu: Menu {
                            PlaylistModeItem {
                                text: KI18n.i18nc("@action:inmenu", "Playlist")
                                mode: MediaPlayListProxyModel.Playlist
                            }
                            PlaylistModeItem {
                                text: KI18n.i18nc("@action:inmenu", "One track")
                                mode: MediaPlayListProxyModel.One
                            }
                            PlaylistModeItem {
                                text: KI18n.i18nc("@action:inmenu", "None")
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
                        id: menuButton

                        function openMenu() {
                            if (!menu.visible) {
                                menu.open()
                            } else {
                                (menu as ApplicationMenu).dismiss()
                            }
                        }

                        text: KI18n.i18nc("@action:button displayed as @info:tooltip", "Application menu")
                        icon.name: "open-menu-symbolic"

                        down: pressed || menu.visible
                        Accessible.role: Accessible.ButtonMenu

                        onPressed: openMenu()
                        Keys.onReturnPressed: openMenu()
                        Keys.onEnterPressed: openMenu()

                        menu: ApplicationMenu {
                            y: menuButton.height
                            // without modal, clicking on menuButton will not close the menu
                            modal: true
                            dim: false
                        }
                    }
                }
            }
        }

        states: State {
            when: musicWidget.isTranslucent
            PropertyChanges {
                toolBar.background.opacity: 0
            }
        }
        transitions: Transition {
            OpacityAnimator {
                target: toolBar.background
                duration: Kirigami.Units.longDuration
            }
        }
    }

    // Some styles do not have a separator for the ToolBar, so we add one ourselves
    // to separate MediaPlayerControl from the page and playlist toolbars.
    Kirigami.Separator {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        visible: !musicWidget.isTranslucent
    }
}
