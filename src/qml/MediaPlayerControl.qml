/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import org.kde.kirigami 2.5 as Kirigami
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
            text: i18nc("toggle between maximized and minimized ivre", "Toggle Maximize")
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
            Layout.rightMargin: !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : Kirigami.Units.largeSpacing * 2
            Layout.leftMargin: LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : Kirigami.Units.largeSpacing * 2
            Layout.preferredWidth: (durationTextMetrics.boundingRect.width - durationTextMetrics.boundingRect.x) + Kirigami.Units.smallSpacing

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
            Layout.rightMargin: !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
            Layout.leftMargin: LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0

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
                anchors.left: musicProgress.left
                anchors.verticalCenter: musicProgress.verticalCenter
                implicitWidth: seekWheelHandler.width
                implicitHeight: 6
                color: myPalette.dark
                radius: 3
            }

            Slider {
                property bool seekStarted: false
                property int seekValue

                id: musicProgress

                anchors.fill: parent

                from: 0
                to: musicWidget.duration

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
                    border.width: elisaTheme.hairline
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
            Layout.rightMargin: !LayoutMirroring.enabled ? (Kirigami.Units.largeSpacing* 2) : 0
            Layout.leftMargin: LayoutMirroring.enabled ? (Kirigami.Units.largeSpacing* 2) : 0
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
            text: i18nc("toggle mute mode for player", "Toggle Mute")
            icon.name: musicWidget.muted ? "player-volume-muted" : "player-volume"
            onClicked: musicWidget.muted = !musicWidget.muted
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
                anchors.left: volumeSlider.left
                anchors.verticalCenter: volumeSlider.verticalCenter
                implicitWidth: audioWheelHandler.width
                implicitHeight: 6
                radius: 3
                color: myPalette.dark
                opacity: muted ? 0.5 : 1
            }

            Slider {
                id: volumeSlider

                anchors.fill: parent

                from: 0
                to: 100
                stepSize: 5

                enabled: !muted

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
                    border.width: elisaTheme.hairline
                    border.color: volumeSlider.pressed ? myPalette.text : (muted ? myPalette.mid : myPalette.dark)
                }
            }
        }



        FlatButtonWithToolTip {
            id: shuffleButton
            text: i18nc("toggle shuffle mode for playlist", "Toggle Shuffle")
            icon.name: musicWidget.shuffle ? "media-playlist-shuffle" : "media-playlist-normal"
            onClicked: musicWidget.shuffle = !musicWidget.shuffle
        }

        FlatButtonWithToolTip {
            id: repeatButton
            text: i18nc("toggle repeat mode for playlist", "Toggle Repeat")
            icon.name: musicWidget.repeat ? "media-repeat-all" : "media-repeat-none"
            onClicked: musicWidget.repeat = !musicWidget.repeat
        }

        // Not a FlatButtonWithToolTip because we want text
        Button {
            id: showHidePlaylistAction
            action: Action {
                shortcut: ElisaApplication.action("toggle_playlist").shortcut
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
            text: i18nc("open application menu", "Application Menu")
            icon.name: "application-menu"
            onClicked: openMenu()
        }

        Item { implicitWidth: Math.floor(Kirigami.Units.smallSpacing / 2) }
    }

    onPositionChanged:
    {
        if (!musicProgress.seekStarted) {
            musicProgress.value = position
        }
    }

    onDurationChanged:
    {
        musicProgress.to = musicWidget.duration
        musicProgress.value = Qt.binding(function() { return musicWidget.position })
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

