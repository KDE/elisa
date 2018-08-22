/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import org.kde.elisa 1.0

import QtQuick.Controls 1.4 as Controls1

FocusScope {
    property double volume
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
        spacing: 5

        Button {
            Layout.preferredWidth: elisaTheme.smallControlButtonSize
            Layout.preferredHeight: elisaTheme.smallControlButtonSize
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonSize
            Layout.maximumHeight: elisaTheme.smallControlButtonSize
            Layout.minimumWidth: elisaTheme.smallControlButtonSize
            Layout.minimumHeight: elisaTheme.smallControlButtonSize
            Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.mediaPlayerHorizontalMargin : 0
            Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.mediaPlayerHorizontalMargin : 0

            contentItem: Image {
                anchors.fill: parent
                source: Qt.resolvedUrl(musicWidget.isMaximized ? elisaTheme.minimizeIcon : elisaTheme.maximizeIcon)

                width: elisaTheme.smallControlButtonSize
                height: elisaTheme.smallControlButtonSize

                sourceSize.width: elisaTheme.smallControlButtonSize
                sourceSize.height: elisaTheme.smallControlButtonSize

                fillMode: Image.PreserveAspectFit
                opacity: 1.0
            }

            background: Rectangle {
                border.width: 0
                opacity: 0.0
            }
            
            onClicked: {
                musicWidget.isMaximized = !musicWidget.isMaximized
            }
        }

        RoundButton {
            focus: skipBackwardEnabled

            Layout.preferredWidth: elisaTheme.smallControlButtonSize
            Layout.preferredHeight: elisaTheme.smallControlButtonSize
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonSize
            Layout.maximumHeight: elisaTheme.smallControlButtonSize
            Layout.minimumWidth: elisaTheme.smallControlButtonSize
            Layout.minimumHeight: elisaTheme.smallControlButtonSize
            Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.mediaPlayerHorizontalMargin : 0
            Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.mediaPlayerHorizontalMargin : 0

            enabled: skipBackwardEnabled
            hoverEnabled: true

            onClicked: {
                musicWidget.playPrevious()
            }

            contentItem: Image {
                anchors.fill: parent

                source: Qt.resolvedUrl(LayoutMirroring.enabled ? elisaTheme.skipForwardIcon : elisaTheme.skipBackwardIcon)

                width: elisaTheme.smallControlButtonSize
                height: elisaTheme.smallControlButtonSize

                sourceSize.width: elisaTheme.smallControlButtonSize
                sourceSize.height: elisaTheme.smallControlButtonSize

                fillMode: Image.PreserveAspectFit

                opacity: skipBackwardEnabled ? 1.0 : 0.6
            }

            background: Rectangle {
                color: "transparent"

                border.color: (parent.hovered || parent.activeFocus) ? myPalette.highlight : "transparent"
                border.width: 1

                radius: elisaTheme.smallControlButtonSize

                Behavior on border.color {
                    ColorAnimation {
                        duration: 300
                    }
                }
            }
        }

        RoundButton {
            focus: playEnabled

            Layout.preferredWidth: elisaTheme.smallControlButtonSize
            Layout.preferredHeight: elisaTheme.smallControlButtonSize
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonSize
            Layout.maximumHeight: elisaTheme.smallControlButtonSize
            Layout.minimumWidth: elisaTheme.smallControlButtonSize
            Layout.minimumHeight: elisaTheme.smallControlButtonSize

            enabled: playEnabled
            hoverEnabled: true

            onClicked: {
                if (musicWidget.isPlaying) {
                    musicWidget.pause()
                } else {
                    musicWidget.play()
                }
            }

            contentItem: Image {
                anchors.fill: parent

                source: {
                    if (musicWidget.isPlaying)
                        Qt.resolvedUrl(elisaTheme.pauseIcon)
                    else
                        Qt.resolvedUrl(elisaTheme.playIcon)
                }

                width: elisaTheme.smallControlButtonSize
                height: elisaTheme.smallControlButtonSize

                sourceSize.width: elisaTheme.smallControlButtonSize
                sourceSize.height: elisaTheme.smallControlButtonSize

                fillMode: Image.PreserveAspectFit
                mirror: LayoutMirroring.enabled
                opacity: playEnabled ? 1.0 : 0.6
            }

            background: Rectangle {
                color: "transparent"

                border.color: (parent.hovered || parent.activeFocus) ? myPalette.highlight : "transparent"
                border.width: 1

                radius: elisaTheme.smallControlButtonSize

                Behavior on border.color {
                    ColorAnimation {
                        duration: 300
                    }
                }
            }
        }

        RoundButton {
            focus: skipForwardEnabled

            Layout.preferredWidth: elisaTheme.smallControlButtonSize
            Layout.preferredHeight: elisaTheme.smallControlButtonSize
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonSize
            Layout.maximumHeight: elisaTheme.smallControlButtonSize
            Layout.minimumWidth: elisaTheme.smallControlButtonSize
            Layout.minimumHeight: elisaTheme.smallControlButtonSize

            enabled: skipForwardEnabled
            hoverEnabled: true

            onClicked: {
                musicWidget.playNext()
            }

            contentItem: Image {
                 anchors.fill: parent

                source: Qt.resolvedUrl(LayoutMirroring.enabled ? elisaTheme.skipBackwardIcon : elisaTheme.skipForwardIcon)

                width: elisaTheme.smallControlButtonSize
                height: elisaTheme.smallControlButtonSize

                sourceSize.width: elisaTheme.smallControlButtonSize
                sourceSize.height: elisaTheme.smallControlButtonSize

                fillMode: Image.PreserveAspectFit

                opacity: skipForwardEnabled ? 1.0 : 0.6
            }

            background: Rectangle {
                color: "transparent"

                border.color: (parent.hovered || parent.activeFocus) ? myPalette.highlight : "transparent"
                border.width: 1

                radius: elisaTheme.smallControlButtonSize

                Behavior on border.color {
                    ColorAnimation {
                        duration: 300
                    }
                }
            }
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
            Layout.preferredWidth: durationTextMetrics.width+5 // be in the safe side

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight

            ProgressIndicator {
                id: timeIndicator
                position: musicWidget.position
            }
        }

        Slider {
            property bool seekStarted: false
            property int seekValue

            id: musicProgress

            from: 0
            to: musicWidget.duration

            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0

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

            background: Rectangle {
                x: musicProgress.leftPadding
                y: musicProgress.topPadding + musicProgress.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 6
                width: musicProgress.availableWidth
                height: implicitHeight
                radius: 3
                color: myPalette.dark

                Rectangle {
                    x: (LayoutMirroring.enabled ? musicProgress.visualPosition * parent.width : 0)
                    width: LayoutMirroring.enabled ? parent.width - musicProgress.visualPosition * parent.width: musicProgress.handle.x + radius
                    height: parent.height
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

        LabelWithToolTip {
            id: durationLabel

            text: durationIndicator.progressDuration

            color: myPalette.text

            Layout.alignment: Qt.AlignVCenter
            Layout.fillHeight: true
            Layout.rightMargin: !LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin * 2) : 0
            Layout.leftMargin: LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin * 2) : 0
            Layout.preferredWidth: durationTextMetrics.width

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft

            ProgressIndicator {
                id: durationIndicator
                position: musicWidget.duration
            }
        }

        Image {
            id: volumeIcon

            source: if (musicWidget.muted)
                        Qt.resolvedUrl(elisaTheme.playerVolumeMutedIcon)
                    else
                        Qt.resolvedUrl(elisaTheme.playerVolumeIcon)

            Layout.preferredWidth: elisaTheme.smallControlButtonSize
            Layout.preferredHeight: elisaTheme.smallControlButtonSize
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonSize
            Layout.maximumHeight: elisaTheme.smallControlButtonSize
            Layout.minimumWidth: elisaTheme.smallControlButtonSize
            Layout.minimumHeight: elisaTheme.smallControlButtonSize
            Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0

            sourceSize.width: elisaTheme.smallControlButtonSize
            sourceSize.height: elisaTheme.smallControlButtonSize

            fillMode: Image.PreserveAspectFit

            visible: false
        }


        RoundButton {
            focus: true

            Layout.preferredWidth: elisaTheme.smallControlButtonSize
            Layout.preferredHeight: elisaTheme.smallControlButtonSize
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonSize
            Layout.maximumHeight: elisaTheme.smallControlButtonSize
            Layout.minimumWidth: elisaTheme.smallControlButtonSize
            Layout.minimumHeight: elisaTheme.smallControlButtonSize

            hoverEnabled: true

            onClicked: musicWidget.muted = !musicWidget.muted

            contentItem: Image {
                anchors.fill: parent

                source: if (musicWidget.muted)
                            Qt.resolvedUrl(elisaTheme.playerVolumeMutedIcon)
                        else
                            Qt.resolvedUrl(elisaTheme.playerVolumeIcon)

                width: elisaTheme.smallControlButtonSize
                height: elisaTheme.smallControlButtonSize

                sourceSize.width: elisaTheme.smallControlButtonSize
                sourceSize.height: elisaTheme.smallControlButtonSize

                fillMode: Image.PreserveAspectFit
            }

            background: Rectangle {
                color: "transparent"

                border.color: (parent.hovered || parent.activeFocus) ? myPalette.highlight : "transparent"
                border.width: 1

                radius: elisaTheme.smallControlButtonSize

                Behavior on border.color {
                    ColorAnimation {
                        duration: 300
                    }
                }
            }
        }

        Slider {
            id: volumeSlider

            from: 0
            to: 100
            value: musicWidget.volume

            onValueChanged: musicWidget.volume = value

            enabled: !muted

            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: elisaTheme.volumeSliderWidth
            Layout.maximumWidth: elisaTheme.volumeSliderWidth
            Layout.minimumWidth: elisaTheme.volumeSliderWidth
            Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin * 3 : 0
            Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin * 3 : 0

            width: elisaTheme.volumeSliderWidth

            background: Rectangle {
                x: volumeSlider.leftPadding
                y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 6
                width: volumeSlider.availableWidth
                height: implicitHeight
                radius: 3
                color: myPalette.dark
                opacity: muted ? 0.5 : 1

                Rectangle {
                    x: (LayoutMirroring.enabled ? volumeSlider.visualPosition * parent.width : 0)
                    width: (LayoutMirroring.enabled ? parent.width - volumeSlider.visualPosition * parent.width : volumeSlider.visualPosition * parent.width)
                    height: parent.height
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

        RoundButton {
            focus: true
            id: shuffleButton

            Layout.preferredWidth: elisaTheme.smallControlButtonSize
            Layout.preferredHeight: elisaTheme.smallControlButtonSize
            Layout.alignment: Qt.AlignCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonSize
            Layout.maximumHeight: elisaTheme.smallControlButtonSize
            Layout.minimumWidth: elisaTheme.smallControlButtonSize
            Layout.minimumHeight: elisaTheme.smallControlButtonSize

            hoverEnabled: true
            onClicked: musicWidget.shuffle = !musicWidget.shuffle

            contentItem: Image {
                anchors.fill: parent

                source: musicWidget.shuffle ? Qt.resolvedUrl(elisaTheme.shuffleIcon) : Qt.resolvedUrl(elisaTheme.noShuffleIcon)

                width: elisaTheme.smallControlButtonSize
                height: elisaTheme.smallControlButtonSize

                sourceSize.width: elisaTheme.smallControlButtonSize
                sourceSize.height: elisaTheme.smallControlButtonSize

                fillMode: Image.PreserveAspectFit
            }

            background: Rectangle {
                color: "transparent"
                }
            }


        RoundButton {
            focus: true
            id: repeatButton

            Layout.preferredWidth: elisaTheme.smallControlButtonSize
            Layout.preferredHeight: elisaTheme.smallControlButtonSize
            Layout.alignment: Qt.AlignCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonSize
            Layout.maximumHeight: elisaTheme.smallControlButtonSize
            Layout.minimumWidth: elisaTheme.smallControlButtonSize
            Layout.minimumHeight: elisaTheme.smallControlButtonSize

            hoverEnabled: true
            onClicked: musicWidget.repeat = !musicWidget.repeat

            contentItem: Image {
                anchors.fill: parent

                source: musicWidget.repeat ? Qt.resolvedUrl(elisaTheme.repeatIcon) : Qt.resolvedUrl(elisaTheme.noRepeatIcon)

                width: elisaTheme.smallControlButtonSize
                height: elisaTheme.smallControlButtonSize

                sourceSize.width: elisaTheme.smallControlButtonSize
                sourceSize.height: elisaTheme.smallControlButtonSize

                fillMode: Image.PreserveAspectFit
            }

            background: Rectangle {
                color: "transparent"
            }

        }

        Controls1.ToolButton {
            id: menuButton
            action: applicationMenuAction

            Layout.preferredWidth: elisaTheme.smallControlButtonSize
            Layout.preferredHeight: elisaTheme.smallControlButtonSize
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonSize
            Layout.maximumHeight: elisaTheme.smallControlButtonSize
            Layout.minimumWidth: elisaTheme.smallControlButtonSize
            Layout.minimumHeight: elisaTheme.smallControlButtonSize
            Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.mediaPlayerHorizontalMargin : elisaTheme.mediaPlayerHorizontalMargin * 1.5
            Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.mediaPlayerHorizontalMargin : elisaTheme.mediaPlayerHorizontalMargin * 1.5
        }
    }

    onPositionChanged:
    {
        if (!musicProgress.seekStarted) {
            musicProgress.value = position
        }
    }

    onVolumeChanged:
    {
        console.log('volume of player controls changed: ' + volume)
    }

    onIsMaximizedChanged:
    {
        if (musicWidget.isMaximized) {
            musicWidget.maximize()
        } else {
            musicWidget.minimize()
        }
    }
}

