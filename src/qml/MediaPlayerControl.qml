/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import org.kde.elisa 1.0

FocusScope {
    property alias volume: volumeSlider.value
    property int position
    property int duration
    property bool muted
    property bool isPlaying
    property bool seekable
    property bool playEnabled
    property bool skipForwardEnabled
    property bool skipBackwardEnabled
    property bool isMaximized

    property bool shuffle
    property bool repeat

    signal play()
    signal pause()
    signal playPrevious()
    signal playNext()
    signal seek(int position)
    signal openMenu()

    signal maximize()
    signal minimize()

    id: musicWidget

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    Action {
        id: applicationMenuAction
        text: i18nc("open application menu", "Application Menu")
        icon.name: "application-menu"
        onTriggered: openMenu()
    }

    Action {
        id: repeatAction
        text: i18nc("toggle repeat mode for playlist", "Toggle Repeat")
        icon.name: musicWidget.repeat ? "media-repeat-all" : "media-repeat-none"
        onTriggered: musicWidget.repeat = !musicWidget.repeat
    }

    Action {
        id: shuffleAction
        text: i18nc("toggle shuffle mode for playlist", "Toggle Shuffle")
        icon.name: musicWidget.shuffle ? "media-playlist-shuffle" : "media-playlist-normal"
        onTriggered: musicWidget.shuffle = !musicWidget.shuffle
    }

    Action {
        id: muteAction
        text: i18nc("toggle mute mode for player", "Toggle Mute")
        icon.name: musicWidget.muted ? "player-volume-muted" : "player-volume"
        onTriggered: musicWidget.muted = !musicWidget.muted
    }

    Action {
        id: playPauseAction
        text: i18nc("toggle play and pause for the audio player", "Toggle Play and Pause")
        icon.name: musicWidget.isPlaying? "media-playback-pause" : "media-playback-start"
        onTriggered: musicWidget.isPlaying ? musicWidget.pause() : musicWidget.play()
        enabled: playEnabled
    }

    Action {
        id: skipBackwardAction
        text: i18nc("skip backward in playlists", "Skip Backward")
        icon.name: musicWidget.LayoutMirroring.enabled ? "media-skip-forward" : "media-skip-backward"
        onTriggered: musicWidget.playPrevious()
        enabled: skipBackwardEnabled
    }

    Action {
        id: skipForwardAction
        text: i18nc("skip forward in playlists", "Skip Forward")
        icon.name: musicWidget.LayoutMirroring.enabled ? "media-skip-backward" : "media-skip-forward"
        onTriggered: musicWidget.playNext()
        enabled: skipForwardEnabled
    }

    Action {
        id: minimizeMaximizeAction
        text: i18nc("toggle between maximized and minimized ivre", "Toggle Maximize")
        icon.name: musicWidget.isMaximized ? "draw-arrow-up" : "draw-arrow-down"
        onTriggered: musicWidget.isMaximized = !musicWidget.isMaximized
    }

    Rectangle {
        anchors.fill: parent

        color: myPalette.midlight
        opacity: elisaTheme.mediaPlayerControlOpacity
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        FlatButtonWithToolTip {
            id: minimizeMaximizeButton
            action: minimizeMaximizeAction

            icon.width: elisaTheme.smallControlButtonSize
            icon.height: elisaTheme.smallControlButtonSize

            Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.mediaPlayerHorizontalMargin : 0
            Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.mediaPlayerHorizontalMargin : 0
        }

        FlatButtonWithToolTip {
            id: skipBackwardButton
            action: skipBackwardAction
            focus: skipBackwardEnabled
            icon.width: elisaTheme.smallControlButtonSize
            icon.height: elisaTheme.smallControlButtonSize
        }

        FlatButtonWithToolTip {
            id: playPauseButton
            action: playPauseAction
            focus: playEnabled

            icon.width: elisaTheme.smallControlButtonSize
            icon.height: elisaTheme.smallControlButtonSize
        }

        FlatButtonWithToolTip {
            id: skipForwardButton
            action: skipForwardAction
            focus: skipForwardEnabled

            icon.width: elisaTheme.smallControlButtonSize
            icon.height: elisaTheme.smallControlButtonSize
        }

        TextMetrics {
            id: durationTextMetrics
            text: i18nc("This is used to preserve a fixed width for the duration text.", "00:00:00")
        }

        LabelWithToolTip {
            id: positionLabel

            text: timeIndicator.progressDuration

            color: myPalette.text

            Layout.alignment: Qt.AlignVCenter
            Layout.fillHeight: true
            Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin  : elisaTheme.layoutHorizontalMargin * 2
            Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : elisaTheme.layoutHorizontalMargin * 2
            Layout.preferredWidth: (durationTextMetrics.boundingRect.width - durationTextMetrics.boundingRect.x) + 5

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight

            ProgressIndicator {
                id: timeIndicator
                position: musicWidget.position
            }
        }

        MouseArea {
            id: seekWheelHandler
            Layout.alignment: Qt.AlignVCenter
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0

            acceptedButtons: Qt.NoButton
            onWheel: {
                if (wheel.angleDelta.y > 0) {
                    musicWidget.seek(position + 10000)
                } else {
                    musicWidget.seek(position - 10000)
                }
            }

            // Synthesized slider background that's not actually a part of the
            // slider. This is done so the slider's own background can be full
            // height yet transparent, for easier clicking
            Rectangle {
                x: musicProgress.leftPadding
                y: musicProgress.topPadding + musicProgress.availableHeight / 2 - height / 2
                implicitWidth: seekWheelHandler.width
                implicitHeight: 6
                color: myPalette.dark
                radius: 3
            }

            Slider {
                property bool seekStarted: false
                property int seekValue

                id: musicProgress

                from: 0
                to: musicWidget.duration
                width: parent.width
                anchors.centerIn: parent

                enabled: musicWidget.seekable && musicWidget.playEnabled

                live: true

                onValueChanged: {
                    if (seekStarted) {
                        seekValue = value
                    }
                }

                onPressedChanged: {
                    if (pressed) {
                        seekStarted = true;
                        seekValue = value
                    } else {
                        musicWidget.seek(seekValue)
                        seekStarted = false;
                    }
                }

                // This only provides a full-height area for clicking; see
                // https://bugs.kde.org/show_bug.cgi?id=408703. The actual visual
                // background is generated above ^^
                background: Rectangle {
                    x: musicProgress.leftPadding
                    y: musicProgress.topPadding + musicProgress.availableHeight / 2 - height / 2
                    anchors.fill: parent
                    implicitWidth: seekWheelHandler.width
                    implicitHeight: seekWheelHandler.height
                    color: "transparent"

                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        x: (LayoutMirroring.enabled ? musicProgress.visualPosition * parent.width : 0)
                        width: LayoutMirroring.enabled ? parent.width - musicProgress.visualPosition * parent.width: musicProgress.handle.x + radius
                        height: 6
                        color: myPalette.text
                        radius: 3
                    }
                }

                handle: Rectangle {
                    x: musicProgress.leftPadding + musicProgress.visualPosition * (musicProgress.availableWidth - width)
                    y: musicProgress.topPadding + musicProgress.availableHeight / 2 - height / 2
                    implicitWidth: 18
                    implicitHeight: 18
                    radius: 9
                    color: myPalette.base
                    border.width: 1
                    border.color: musicProgress.pressed ? myPalette.text : myPalette.dark

                }
            }
        }

        LabelWithToolTip {
            id: durationLabel

            text: durationIndicator.progressDuration

            color: myPalette.text

            Layout.alignment: Qt.AlignVCenter
            Layout.fillHeight: true
            Layout.rightMargin: !LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin * 2) : 0
            Layout.leftMargin: LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin * 2) : 0
            Layout.preferredWidth: (durationTextMetrics.boundingRect.width - durationTextMetrics.boundingRect.x)

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft

            ProgressIndicator {
                id: durationIndicator
                position: musicWidget.duration
            }
        }

        FlatButtonWithToolTip {
            id: muteButton
            action: muteAction
            focus: true

            icon.width: elisaTheme.smallControlButtonSize
            icon.height: elisaTheme.smallControlButtonSize
        }

        MouseArea {
            id: audioWheelHandler

            Layout.preferredWidth: elisaTheme.volumeSliderWidth
            Layout.maximumWidth: elisaTheme.volumeSliderWidth
            Layout.minimumWidth: elisaTheme.volumeSliderWidth
            Layout.fillHeight: true

            acceptedButtons: Qt.NoButton
            onWheel: {
                if (wheel.angleDelta.y > 0) {
                    volumeSlider.increase()
                } else {
                    volumeSlider.decrease()
                }
            }

            // Synthesized slider background that's not actually a part of the
            // slider. This is done so the slider's own background can be full
            // height yet transparent, for easier clicking
            Rectangle {
                x: volumeSlider.leftPadding
                y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                implicitWidth: audioWheelHandler.width
                implicitHeight: 6
                radius: 3
                color: myPalette.dark
                opacity: muted ? 0.5 : 1
            }

            Slider {
                id: volumeSlider

                from: 0
                to: 100
                stepSize: 5

                enabled: !muted

                anchors.centerIn: parent
                width: elisaTheme.volumeSliderWidth

                // This only provides a full-height area for clicking; see
                // https://bugs.kde.org/show_bug.cgi?id=408703. The actual visual
                // background is generated above ^^
                background: Rectangle {
                    anchors.fill: parent
                    implicitWidth: audioWheelHandler.width
                    implicitHeight: audioWheelHandler.height
                    color: "transparent"

                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        x: (LayoutMirroring.enabled ? volumeSlider.visualPosition * parent.width : 0)
                        width: (LayoutMirroring.enabled ? parent.width - volumeSlider.visualPosition * parent.width : volumeSlider.visualPosition * parent.width)
                        height: 6
                        color: myPalette.text
                        radius: 3
                        opacity: muted ? 0.5 : 1
                    }
                }

                handle: Rectangle {
                    x: volumeSlider.leftPadding + volumeSlider.visualPosition * (volumeSlider.availableWidth - width)
                    y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                    implicitWidth: 18
                    implicitHeight: 18
                    radius: 9
                    color: myPalette.base
                    border.width: 1
                    border.color: volumeSlider.pressed ? myPalette.text : (muted ? myPalette.mid : myPalette.dark)
                }
            }
        }



        FlatButtonWithToolTip {
            focus: true
            action: shuffleAction
            id: shuffleButton

            icon.width: elisaTheme.smallControlButtonSize
            icon.height: elisaTheme.smallControlButtonSize
        }

        FlatButtonWithToolTip {
            focus: true
            action: repeatAction
            id: repeatButton

            icon.width: elisaTheme.smallControlButtonSize
            icon.height: elisaTheme.smallControlButtonSize
        }

        // Not a FlatButtonWithToolTip because we want text
        Button {
            id: showHidePlaylistAction
            action: Action {
                shortcut: elisa.action("toggle_playlist").shortcut
                onTriggered: contentView.showPlaylist = !contentView.showPlaylist
            }

            visible: mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold

            flat: true
            text: i18n("Show Playlist")
            icon.name: "view-media-playlist"

            checkable: true
            checked: contentView.showPlaylist

            activeFocusOnTab: true
            Keys.onReturnPressed: action.trigger()
            Accessible.onPressAction: action.trigger()

        }

        FlatButtonWithToolTip {
            id: menuButton
            action: applicationMenuAction
            focus: true

            icon.width: elisaTheme.smallControlButtonSize
            icon.height: elisaTheme.smallControlButtonSize
        }
    }

    onPositionChanged:
    {
        if (!musicProgress.seekStarted) {
            musicProgress.value = position
        }
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
            elisa.installKeyEventFilter(element)
        }
    }
}

