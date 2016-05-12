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

import QtQuick 2.2
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import org.mgallien.QmlExtension 1.0
import org.kde.kirigami 1.0 as MobileComponents


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

    Rectangle {
        color: '#C5CBD1'
        width: parent.width
        height: parent.height

        RowLayout {
            anchors.fill: parent
            spacing: 0

            Image {
                id: previousButton
                MouseArea {
                    anchors.fill: parent
                    onClicked: musicWidget.playPrevious()
                }
                source: Qt.resolvedUrl('media-skip-backward.svg')
                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignVCenter
                Layout.maximumWidth: width
                Layout.maximumHeight: height
                Layout.minimumWidth: width
                Layout.minimumHeight: height
                sourceSize.width: width
                sourceSize.height: width
                height: width
                width: Screen.pixelDensity * 6
                fillMode: Image.PreserveAspectFit
                enabled: skipBackwardEnabled
                opacity: enabled ? 1.0 : 0.6
            }

            Image {
                id: playPauseButton
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (musicWidget.isPlaying)
                            musicWidget.pause()
                        else
                            musicWidget.play()
                    }
                }
                source: {
                    if (musicWidget.isPlaying)
                        Qt.resolvedUrl('media-playback-pause.svg')
                    else
                        Qt.resolvedUrl('media-playback-start.svg')
                }
                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignVCenter
                Layout.maximumWidth: width
                Layout.maximumHeight: height
                Layout.minimumWidth: width
                Layout.minimumHeight: height
                sourceSize.width: width
                sourceSize.height: width
                height: width
                width: Screen.pixelDensity * 8
                fillMode: Image.PreserveAspectFit
                enabled: playEnabled
                opacity: enabled ? 1.0 : 0.6
            }

            Image {
                id: nextButton
                MouseArea {
                    anchors.fill: parent
                    onClicked: musicWidget.playNext()
                }
                source: Qt.resolvedUrl('media-skip-forward.svg')
                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignVCenter
                Layout.maximumWidth: width
                Layout.maximumHeight: height
                Layout.minimumWidth: width
                Layout.minimumHeight: height
                sourceSize.width: width
                sourceSize.height: width
                height: width
                width: Screen.pixelDensity * 6
                fillMode: Image.PreserveAspectFit
                enabled: skipForwardEnabled
                opacity: enabled ? 1.0 : 0.6
            }

            Item {
                Layout.preferredWidth: width
                Layout.maximumWidth: width
                Layout.minimumWidth: width
                width: Screen.pixelDensity * 2
            }

            ProgressIndicator {
                id: timeIndicator
                position: musicWidget.position
            }

            Label {
                id: positionLabel
                text: timeIndicator.progressDuration
                Layout.alignment: Qt.AlignVCenter
            }

            Item {
                Layout.preferredWidth: width
                Layout.maximumWidth: width
                Layout.minimumWidth: width
                width: Screen.pixelDensity * 2
            }

            Slider {
                id: musicProgress
                minimumValue: 0
                maximumValue: musicWidget.duration
                value: musicWidget.position
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
                enabled: musicWidget.seekable && musicWidget.playEnabled
                onPressedChanged: {
                    if (!pressed) {
                        musicWidget.seek(value)
                    }
                }
            }

            Item {
                Layout.preferredWidth: width
                Layout.maximumWidth: width
                Layout.minimumWidth: width
                width: Screen.pixelDensity * 2
            }
        }
    }
}

