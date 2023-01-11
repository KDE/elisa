/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>

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
    property string image: ElisaApplication.manageHeaderBar.image
    property string title: ElisaApplication.manageHeaderBar.title
    property string artist: ElisaApplication.manageHeaderBar.artist !== undefined ?
                            ElisaApplication.manageHeaderBar.artist : ''
    property string album:  ElisaApplication.manageHeaderBar.album !== undefined ?
                            ElisaApplication.manageHeaderBar.album : ''

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

        Kirigami.Theme.colorSet: Kirigami.Theme.Header
        Kirigami.Theme.inherit: false
        color: ElisaApplication.showHeader ? myPalette.midlight : Kirigami.Theme.backgroundColor
        opacity: ElisaApplication.showHeader ? elisaTheme.mediaPlayerControlOpacity : 1.0
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
            enabled: playEnabled
            text: musicWidget.isPlaying ? i18nc("@action:button Pause any media that is playing", "Pause") : i18nc("@action:button Start playing media", "Play")
            icon.name: musicWidget.isPlaying? "media-playback-pause" : "media-playback-start"
            onClicked: musicWidget.isPlaying ? musicWidget.pause() : musicWidget.play()
        }

        FlatButtonWithToolTip {
            id: skipForwardButton
            enabled: skipForwardEnabled
            text: i18nc("@action:button skip forward in playlists", "Skip Forward")
            icon.name: musicWidget.LayoutMirroring.enabled ? "media-skip-backward" : "media-skip-forward"
            onClicked: musicWidget.playNext()
        }

        Loader {
            active: !ElisaApplication.showHeader && !headerBar.isMaximized
            visible: active

            Layout.preferredWidth: elisaTheme.coverArtSize
            Layout.preferredHeight: Layout.preferredWidth
            Layout.leftMargin: !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : Kirigami.Units.largeSpacing * 2

            sourceComponent: ImageWithFallback {
                asynchronous: true
                mipmap: true

                source: image
                fallback: Qt.resolvedUrl(elisaTheme.defaultAlbumImage)

                sourceSize {
                    width: elisaTheme.coverArtSize
                    height: width
                }

                fillMode: Image.PreserveAspectFit
            }
        }

        ColumnLayout {
            spacing: 0
            visible: !ElisaApplication.showHeader && !headerBar.isMaximized

            Layout.minimumWidth: parent.width * 0.1
            Layout.maximumWidth: Layout.minimumWidth
            Layout.fillHeight: true
            Layout.leftMargin: Kirigami.Units.largeSpacing

            LabelWithToolTip {
                text: title
                //font.weight: isPlaying ? Font.Bold : Font.Normal
                Layout.fillWidth: true
            }

            LabelWithToolTip {
                text: [artist, album].filter(Boolean).join(" - ")
                type: Kirigami.Heading.Type.Secondary
                Layout.fillWidth: true
                visible: !playListEntry.grouped && (artist || album)
            }
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

            muted: muted
        }

        Item { implicitWidth: Kirigami.Units.largeSpacing }

        FlatButtonWithToolTip {
            id: shuffleButton
            text: i18nc("@action:button", "Toggle Shuffle")
            icon.name: "media-playlist-shuffle"
            onClicked: musicWidget.shuffle = !musicWidget.shuffle
            checkable: true
            checked: musicWidget.shuffle
        }

        FlatButtonWithToolTip {
            id: repeatButton
            text: {
                const map = {
                    0: i18nc("@info:tooltip", "Current: Don't repeat tracks"),
                    1: i18nc("@info:tooltip", "Current: Repeat current track"),
                    2: i18nc("@info:tooltip", "Current: Repeat all tracks in playlist")
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
            id: showHidePlaylistAction
            action: Action {
                shortcut: ElisaApplication.actionShortcut(ElisaApplication.action("toggle_playlist"))
                onTriggered: contentView.showPlaylist = !contentView.showPlaylist
            }

            visible: !musicWidget.isMaximized && mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold

            display: AbstractButton.TextBesideIcon
            text: i18nc("@action:button", "Show Playlist")
            icon.name: "view-media-playlist"

            checkable: true
            checked: contentView.showPlaylist
        }

        FlatButtonWithToolTip {
            id: menuButton

            function openMenu() {
                if (!applicationMenu.visible) {
                    applicationMenu.open()
                } else {
                    applicationMenu.dismiss()
                }
            }

            text: i18nc("@action:button", "Application Menu")
            icon.name: "open-menu-symbolic"

            down: pressed || applicationMenu.visible
            Accessible.role: Accessible.ButtonMenu

            onPressed: openMenu()
            // Need this too because the base control sends onClicked for return/enter
            onClicked: openMenu()

            ApplicationMenu {
                id: applicationMenu

                y: menuButton.height
                // without modal, clicking on menuButton will not close the menu
                modal: true
                dim: false

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

