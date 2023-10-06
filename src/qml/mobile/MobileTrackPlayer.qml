/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import ElisaGraphicalEffects 1.15
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0
import ".."
import "../shared"

BasePlayerControl {
    id: trackPlayer

    property alias volume: volumeButton.sliderValue

    property string image
    property string title
    property string artist
    property string albumArtist
    property string album

    property bool portrait

    property int imageSourceSize: 512

    // hardcode slider colours because theming doesn't really make sense on top of the blurred background
    property color sliderElapsedColor: "white"
    property color sliderRemainingColor: "grey"
    property color sliderHandleColor: "white"

    ColumnLayout {
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing

        // hide arrow button
        FlatButtonWithToolTip {
            id: minimizePlayer
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.maximumHeight: parent.height
            Layout.preferredHeight: Kirigami.Units.gridUnit * 2
            Layout.maximumWidth: parent.height
            Layout.preferredWidth: Kirigami.Units.gridUnit * 2
            text: i18nc("@action:button", "Minimize Player")
            icon.name: "arrow-down"
            icon.color: "white"
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false
            onClicked: toClose.restart()
        }

        // album art
        ImageWithFallback {
            property double specWidth: {
                let allowedWidth = mainWindow.width - Kirigami.Units.largeSpacing * 4;
                let allowedHeight = mainWindow.height - Kirigami.Units.largeSpacing * 8 - (minimizePlayer.height + bottomPlayerControls.height);
                return Math.min(allowedWidth, allowedHeight);
            }
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.preferredWidth: specWidth
            Layout.minimumWidth: specWidth
            Layout.maximumWidth: specWidth
            Layout.preferredHeight: specWidth

            asynchronous: true
            mipmap: true

            source: trackPlayer.image
            fallback: Qt.resolvedUrl(elisaTheme.defaultAlbumImage)

            sourceSize {
                width: imageSourceSize * Screen.devicePixelRatio
                height: imageSourceSize * Screen.devicePixelRatio
            }

            fillMode: Image.PreserveAspectFit
        }

        Item { Layout.fillHeight: true }

        // bottom player controls
        ColumnLayout {
            id: bottomPlayerControls
            spacing: Kirigami.Units.smallSpacing

            Layout.alignment: Qt.AlignBottom
            Layout.fillWidth: true

            property int maxWidth: mainWindow.width - Kirigami.Units.largeSpacing * 4
            Layout.maximumWidth: maxWidth

            LabelWithToolTip {
                id: mainLabel
                text: title
                wrapMode: Text.Wrap
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.fillWidth: true
                elide: Text.ElideRight
                maximumLineCount: 1
                // Hardcoded because the footerbar blur always makes a dark-ish
                // background, so we don't want to use a color scheme color that
                // might also be dark
                color: "white"
                level: 4
                font.weight: Font.Bold
                font.bold: true

                MouseArea {
                    id: titleMouseArea
                    hoverEnabled: true
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: parent.implicitWidth
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        openNowPlaying()
                    }
                }
            }

            LabelWithToolTip {
                id: authorLabel
                text: artist
                visible: text.length > 0
                wrapMode: Text.Wrap
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.fillWidth: true
                elide: Text.ElideRight
                maximumLineCount: 1
                // Hardcoded because the footerbar blur always makes a dark-ish
                // background, so we don't want to use a color scheme color that
                // might also be dark
                color: "white"
                level: 4

                MouseArea {
                    id: authorMouseArea
                    hoverEnabled: true
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: parent.implicitWidth
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        openArtist()
                    }
                }
            }

            LabelWithToolTip {
                id: albumLabel
                text: album
                visible: text.length > 0
                wrapMode: Text.Wrap
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.fillWidth: true
                elide: Text.ElideRight
                maximumLineCount: 1
                // Hardcoded because the footerbar blur always makes a dark-ish
                // background, so we don't want to use a color scheme color that
                // might also be dark
                color: "white"
                level: 4

                MouseArea {
                    id: albumMouseArea
                    hoverEnabled: true
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: parent.implicitWidth
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        openAlbum()
                    }
                }
            }

            // misc. player controls
            RowLayout {
                Layout.preferredHeight: Kirigami.Units.gridUnit * 3

                // ensure white icons
                Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                Kirigami.Theme.inherit: false

                FlatButtonWithToolTip {
                    id: infoButton
                    Layout.maximumHeight: parent.height
                    Layout.preferredHeight: Math.floor(Kirigami.Units.gridUnit * 2.5)
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height
                    text: i18nc("@action:button show track information", "Show Info")
                    icon.name: "documentinfo"
                    icon.color: "white"
                    onClicked: openNowPlaying()
                }

                FlatButtonWithToolTip {
                    id: shuffleButton
                    Layout.maximumHeight: parent.height
                    Layout.preferredHeight: Math.floor(Kirigami.Units.gridUnit * 2.5)
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height
                    text: i18nc("@action:button", "Toggle Shuffle")
                    icon.name: "media-playlist-shuffle"
                    icon.color: "white"
                    checked: ElisaApplication.mediaPlayListProxyModel.shufflePlayList
                    onClicked: ElisaApplication.mediaPlayListProxyModel.shufflePlayList = !ElisaApplication.mediaPlayListProxyModel.shufflePlayList
                }

                FlatButtonWithToolTip {
                    id: repeatButton
                    Layout.maximumHeight: parent.height
                    Layout.preferredHeight: Math.floor(Kirigami.Units.gridUnit * 2.5)
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height
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
                    icon.color: "white"

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
                }
            }

            // duration slider
            DurationSlider {
                Layout.fillWidth: true
                Layout.maximumHeight: Math.floor(Kirigami.Units.gridUnit * 2.5)
                position: trackPlayer.position
                duration: trackPlayer.duration
                seekable: trackPlayer.seekable
                playEnabled: trackPlayer.playEnabled
                onSeek: position => trackPlayer.seek(position)

                // this color works well over the blurred/darkened background
                labelColor: "white"
            }

            // bottom play controls
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredHeight: Math.floor(Kirigami.Units.gridUnit * 2.5)

                // ensure white icons
                Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                Kirigami.Theme.inherit: false

                // volume button
                MobileVolumeButton {
                    id: volumeButton
                    muted: trackPlayer.muted
                    Layout.maximumHeight: parent.height
                    Layout.preferredHeight: Math.floor(Kirigami.Units.gridUnit * 2.5)
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height
                }

                Item { Layout.fillWidth: true }

                FlatButtonWithToolTip {
                    id: skipBackwardButton
                    Layout.maximumHeight: parent.height
                    Layout.preferredHeight: Math.floor(Kirigami.Units.gridUnit * 2.5)
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height
                    enabled: skipBackwardEnabled
                    text: i18nc("@action:button", "Skip Backward")
                    onClicked: trackPlayer.playPrevious()
                    icon.name: trackPlayer.LayoutMirroring.enabled ? "media-skip-forward" : "media-skip-backward"
                    icon.width: Kirigami.Units.gridUnit
                    icon.height: Kirigami.Units.gridUnit
                    icon.color: "white"
                }

                FlatButtonWithToolTip {
                    id: playPauseButton
                    Layout.maximumHeight: parent.height
                    Layout.preferredHeight: Math.floor(Kirigami.Units.gridUnit * 2.5)
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height
                    enabled: trackPlayer.playEnabled
                    text: trackPlayer.isPlaying ? i18nc("@action:button Pause any media that is playing", "Pause") : i18nc("@action:button Start playing media", "Play")
                    onClicked: trackPlayer.isPlaying ? trackPlayer.pause() : trackPlayer.play()
                    icon.name: trackPlayer.isPlaying ? "media-playback-pause" : "media-playback-start"
                    icon.width: Kirigami.Units.gridUnit
                    icon.height: Kirigami.Units.gridUnit
                    icon.color: "white"
                }

                FlatButtonWithToolTip {
                    id: skipForwardButton
                    Layout.maximumHeight: parent.height
                    Layout.preferredHeight: Math.floor(Kirigami.Units.gridUnit * 2.5)
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height
                    enabled: skipForwardEnabled
                    text: i18nc("@action:button", "Skip Forward")
                    onClicked: trackPlayer.playNext()
                    icon.name: trackPlayer.LayoutMirroring.enabled ? "media-skip-backward" : "media-skip-forward"
                    icon.width: Kirigami.Units.gridUnit
                    icon.height: Kirigami.Units.gridUnit
                    icon.color: "white"
                }

                Item { Layout.fillWidth: true }

                FlatButtonWithToolTip {
                    id: showPlaylistButton
                    Layout.maximumHeight: parent.height
                    Layout.preferredHeight: Math.floor(Kirigami.Units.gridUnit * 2.5)
                    Layout.maximumWidth: height
                    Layout.preferredWidth: height
                    text: i18nc("@action:button", "Show Playlist")
                    onClicked: playlistDrawer.open()
                    icon.name: "view-media-playlist"
                    icon.width: Kirigami.Units.gridUnit
                    icon.height: Kirigami.Units.gridUnit
                    icon.color: "white"
                }
            }
        }
    }
}
