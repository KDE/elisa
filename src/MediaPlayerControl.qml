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

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.1 as Controls2
import org.mgallien.QmlExtension 1.0


Item {
    property double volume
    property int position
    property int duration
    property bool muted
    property bool isPlaying
    property bool seekable
    property bool playEnabled
    property bool skipForwardEnabled
    property bool skipBackwardEnabled

    signal play()
    signal pause()
    signal playPrevious()
    signal playNext()
    signal seek(int position)

    id: musicWidget

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    Rectangle {
        color: myPalette.mid

        opacity: 0.5

        anchors.fill: parent
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Image {
            id: previousButton

            source: Qt.resolvedUrl(LayoutMirroring.enabled ? elisaTheme.skipForwardIcon : elisaTheme.skipBackwardIcon)

            Layout.preferredWidth: elisaTheme.smallControlButtonHeight
            Layout.preferredHeight: elisaTheme.smallControlButtonHeight
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonHeight
            Layout.maximumHeight: elisaTheme.smallControlButtonHeight
            Layout.minimumWidth: elisaTheme.smallControlButtonHeight
            Layout.minimumHeight: elisaTheme.smallControlButtonHeight

            sourceSize.width: elisaTheme.smallControlButtonHeight
            sourceSize.height: elisaTheme.smallControlButtonHeight

            fillMode: Image.PreserveAspectFit

            visible: false
        }

        BrightnessContrast {
            source: previousButton

            Layout.preferredWidth: elisaTheme.smallControlButtonHeight
            Layout.preferredHeight: elisaTheme.smallControlButtonHeight
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonHeight
            Layout.maximumHeight: elisaTheme.smallControlButtonHeight
            Layout.minimumWidth: elisaTheme.smallControlButtonHeight
            Layout.minimumHeight: elisaTheme.smallControlButtonHeight

            enabled: skipBackwardEnabled

            opacity: skipBackwardEnabled ? 1.0 : 0.6

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    musicWidget.playPrevious()
                }
            }

            brightness: 0.0
            contrast: skipBackwardEnabled ? 0.7 : -0.7
        }

        Image {
            id: playPauseButton

            source: {
                if (musicWidget.isPlaying)
                    Qt.resolvedUrl(elisaTheme.pauseIcon)
                else
                    Qt.resolvedUrl(elisaTheme.playIcon)
            }

            Layout.preferredWidth: elisaTheme.bigControlButtonHeight
            Layout.preferredHeight: elisaTheme.bigControlButtonHeight
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.bigControlButtonHeight
            Layout.maximumHeight: elisaTheme.bigControlButtonHeight
            Layout.minimumWidth: elisaTheme.bigControlButtonHeight
            Layout.minimumHeight: elisaTheme.bigControlButtonHeight

            sourceSize.width: elisaTheme.bigControlButtonHeight
            sourceSize.height: elisaTheme.bigControlButtonHeight

            fillMode: Image.PreserveAspectFit
            mirror: true

            visible: false
        }

        BrightnessContrast {
            source: playPauseButton

            Layout.preferredWidth: elisaTheme.bigControlButtonHeight
            Layout.preferredHeight: elisaTheme.bigControlButtonHeight
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.bigControlButtonHeight
            Layout.maximumHeight: elisaTheme.bigControlButtonHeight
            Layout.minimumWidth: elisaTheme.bigControlButtonHeight
            Layout.minimumHeight: elisaTheme.bigControlButtonHeight

            enabled: playEnabled

            opacity: playEnabled ? 1.0 : 0.6

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (musicWidget.isPlaying) {
                        musicWidget.pause()
                    } else {
                        musicWidget.play()
                    }
                }
            }

            brightness: 0.0
            contrast: playEnabled ? 0.7 : -0.7
        }

        Image {
            id: nextButton

            source: Qt.resolvedUrl(LayoutMirroring.enabled ? elisaTheme.skipBackwardIcon : elisaTheme.skipForwardIcon)

            Layout.preferredWidth: elisaTheme.smallControlButtonHeight
            Layout.preferredHeight: elisaTheme.smallControlButtonHeight
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonHeight
            Layout.maximumHeight: elisaTheme.smallControlButtonHeight
            Layout.minimumWidth: elisaTheme.smallControlButtonHeight
            Layout.minimumHeight: elisaTheme.smallControlButtonHeight
            Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.smallControlButtonHeight : 0
            Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.smallControlButtonHeight : 0

            sourceSize.width: elisaTheme.smallControlButtonHeight
            sourceSize.height: elisaTheme.smallControlButtonHeight

            fillMode: Image.PreserveAspectFit

            visible: false
        }

        BrightnessContrast {
            source: nextButton

            Layout.preferredWidth: elisaTheme.smallControlButtonHeight
            Layout.preferredHeight: elisaTheme.smallControlButtonHeight
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonHeight
            Layout.maximumHeight: elisaTheme.smallControlButtonHeight
            Layout.minimumWidth: elisaTheme.smallControlButtonHeight
            Layout.minimumHeight: elisaTheme.smallControlButtonHeight
            Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.smallControlButtonHeight : 0
            Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.smallControlButtonHeight : 0

            enabled: skipForwardEnabled

            opacity: skipForwardEnabled ? 1.0 : 0.6

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    musicWidget.playNext()
                }
            }

            brightness: 0.0
            contrast: skipForwardEnabled ? 0.7 : -0.7
        }

        LabelWithToolTip {
            id: positionLabel

            text: timeIndicator.progressDuration

            color: myPalette.text

            Layout.alignment: Qt.AlignVCenter
            Layout.fillHeight: true
            Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0

            verticalAlignment: Text.AlignVCenter

            ProgressIndicator {
                id: timeIndicator
                position: musicWidget.position
            }
        }

        Controls2.Slider {
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
                color: myPalette.mid

                Rectangle {
                    x: (LayoutMirroring.enabled ? musicProgress.visualPosition * parent.width : 0)
                    width: (LayoutMirroring.enabled ? parent.width - musicProgress.visualPosition * parent.width : musicProgress.visualPosition * parent.width)
                    height: parent.height
                    color: myPalette.highlight
                    radius: 3
                }
            }

            handle: Rectangle {
                x: musicProgress.leftPadding + musicProgress.visualPosition * (musicProgress.availableWidth - width)
                y: musicProgress.topPadding + musicProgress.availableHeight / 2 - height / 2
                implicitWidth: 18
                implicitHeight: 18
                radius: 9
                color: myPalette.button
                border.color: musicProgress.pressed ? myPalette.highlight : myPalette.dark
            }
        }

        LabelWithToolTip {
            id: durationLabel

            text: durationIndicator.progressDuration

            color: myPalette.text

            Layout.alignment: Qt.AlignVCenter
            Layout.fillHeight: true
            Layout.rightMargin: !LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin * 10) : 0
            Layout.leftMargin: LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin * 10) : 0

            verticalAlignment: Text.AlignVCenter

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

            Layout.preferredWidth: elisaTheme.smallControlButtonHeight
            Layout.preferredHeight: elisaTheme.smallControlButtonHeight
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonHeight
            Layout.maximumHeight: elisaTheme.smallControlButtonHeight
            Layout.minimumWidth: elisaTheme.smallControlButtonHeight
            Layout.minimumHeight: elisaTheme.smallControlButtonHeight
            Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0

            sourceSize.width: elisaTheme.smallControlButtonHeight
            sourceSize.height: elisaTheme.smallControlButtonHeight

            fillMode: Image.PreserveAspectFit

            visible: false
        }

        BrightnessContrast {
            source: volumeIcon

            Layout.preferredWidth: elisaTheme.smallControlButtonHeight
            Layout.preferredHeight: elisaTheme.smallControlButtonHeight
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonHeight
            Layout.maximumHeight: elisaTheme.smallControlButtonHeight
            Layout.minimumWidth: elisaTheme.smallControlButtonHeight
            Layout.minimumHeight: elisaTheme.smallControlButtonHeight
            Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0

            MouseArea {
                anchors.fill: parent
                onClicked: musicWidget.muted = !musicWidget.muted
            }

            brightness: 0.0
            contrast: 0.7
        }

        Controls2.Slider {
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
            Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0

            width: elisaTheme.volumeSliderWidth

            background: Rectangle {
                x: volumeSlider.leftPadding
                y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 6
                width: volumeSlider.availableWidth
                height: implicitHeight
                radius: 3
                color: myPalette.mid

                Rectangle {
                    x: (LayoutMirroring.enabled ? volumeSlider.visualPosition * parent.width : 0)
                    width: (LayoutMirroring.enabled ? parent.width - volumeSlider.visualPosition * parent.width : volumeSlider.visualPosition * parent.width)
                    height: parent.height
                    color: myPalette.highlight
                    radius: 3
                }
            }

            handle: Rectangle {
                x: volumeSlider.leftPadding + volumeSlider.visualPosition * (volumeSlider.availableWidth - width)
                y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2
                implicitWidth: 18
                implicitHeight: 18
                radius: 9
                color: myPalette.button
                border.color: volumeSlider.pressed ? myPalette.highlight : myPalette.dark
            }
        }
    }

    onPositionChanged: if (!musicProgress.seekStarted) {
                           musicProgress.value = position
                       }
}

