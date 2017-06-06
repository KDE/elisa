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

import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
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

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    musicWidget.playPrevious()
                }
            }

            source: Qt.resolvedUrl(elisaTheme.skipBackwardIcon)

            Layout.preferredWidth: elisaTheme.smallControlButtonHeight
            Layout.preferredHeight: elisaTheme.smallControlButtonHeight
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonHeight
            Layout.maximumHeight: elisaTheme.smallControlButtonHeight
            Layout.minimumWidth: elisaTheme.smallControlButtonHeight
            Layout.minimumHeight: elisaTheme.smallControlButtonHeight

            sourceSize.width: elisaTheme.smallControlButtonHeight
            sourceSize.height: elisaTheme.smallControlButtonHeight

            height: elisaTheme.smallControlButtonHeight
            width: elisaTheme.smallControlButtonHeight

            fillMode: Image.PreserveAspectFit

            enabled: skipBackwardEnabled

            opacity: enabled ? 1.0 : 0.6
        }

        Image {
            id: playPauseButton

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

            height: elisaTheme.bigControlButtonHeight
            width: elisaTheme.bigControlButtonHeight

            fillMode: Image.PreserveAspectFit

            enabled: playEnabled

            opacity: enabled ? 1.0 : 0.6
        }

        Image {
            id: nextButton

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    musicWidget.playNext()
                }
            }

            source: Qt.resolvedUrl(elisaTheme.skipForwardIcon)

            Layout.preferredWidth: elisaTheme.smallControlButtonHeight
            Layout.preferredHeight: elisaTheme.smallControlButtonHeight
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: elisaTheme.smallControlButtonHeight
            Layout.maximumHeight: elisaTheme.smallControlButtonHeight
            Layout.minimumWidth: elisaTheme.smallControlButtonHeight
            Layout.minimumHeight: elisaTheme.smallControlButtonHeight
            Layout.rightMargin: elisaTheme.smallControlButtonHeight

            sourceSize.width: elisaTheme.smallControlButtonHeight
            sourceSize.height: elisaTheme.smallControlButtonHeight

            height: elisaTheme.smallControlButtonHeight
            width: elisaTheme.smallControlButtonHeight

            fillMode: Image.PreserveAspectFit

            enabled: skipForwardEnabled

            opacity: enabled ? 1.0 : 0.6
        }

        LabelWithToolTip {
            id: positionLabel

            text: timeIndicator.progressDuration

            color: myPalette.text

            Layout.alignment: Qt.AlignVCenter
            Layout.rightMargin: elisaTheme.layoutHorizontalMargin

            ProgressIndicator {
                id: timeIndicator
                position: musicWidget.position
            }
        }

        Slider {
            property bool seekStarted: false
            property int seekValue

            id: musicProgress

            minimumValue: 0
            maximumValue: musicWidget.duration

            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.rightMargin: elisaTheme.layoutHorizontalMargin * 10

            enabled: musicWidget.seekable && musicWidget.playEnabled

            updateValueWhileDragging: true

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
        }

        Image {
            id: volumeIcon

            MouseArea {
                anchors.fill: parent
                onClicked: musicWidget.muted = !musicWidget.muted
            }

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
            Layout.rightMargin: elisaTheme.layoutHorizontalMargin

            sourceSize.width: elisaTheme.smallControlButtonHeight
            sourceSize.height: elisaTheme.smallControlButtonHeight

            height: elisaTheme.smallControlButtonHeight
            width: elisaTheme.smallControlButtonHeight

            fillMode: Image.PreserveAspectFit
        }

        Slider {
            id: volumeSlider

            minimumValue: 0.0
            maximumValue: 1.0
            value: musicWidget.volume

            onValueChanged: musicWidget.volume = value

            enabled: !muted

            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: elisaTheme.volumeSliderWidth
            Layout.maximumWidth: elisaTheme.volumeSliderWidth
            Layout.minimumWidth: elisaTheme.volumeSliderWidth
            Layout.rightMargin: elisaTheme.layoutHorizontalMargin

            width: elisaTheme.volumeSliderWidth
        }
    }

    onPositionChanged: if (!musicProgress.seekStarted) {
                           musicProgress.value = position
                       }
}

