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

import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

RowLayout {
    property int starSize
    property int starRating
    property bool readOnly: true

    property double hoverBrightness: 0.5
    property double hoverContrast: 0.5

    spacing: 0

    Theme {
        id: elisaTheme
    }

    Item {
        Layout.preferredHeight: starSize * 1.5
        Layout.preferredWidth: starSize * 1.5
        Layout.maximumHeight: starSize * 1.5
        Layout.maximumWidth: starSize * 1.5
        Layout.minimumHeight: starSize * 1.5
        Layout.minimumWidth: starSize * 1.5

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: starSize * 1.5
            sourceSize.height: starSize * 1.5
            fillMode: Image.PreserveAspectFit

            layer.enabled: (mouseAreaOne.containsMouse && starRating !== 2) || (mouseAreaTwo.containsMouse && starRating !== 4) || (mouseAreaThree.containsMouse && starRating !== 6) ||
                           (mouseAreaFour.containsMouse && starRating !== 8) || (mouseAreaFive.containsMouse && starRating !== 10)
            layer.effect: BrightnessContrast {
                brightness: hoverBrightness
                contrast: hoverContrast
            }

            source: if (starRating >= 2 || mouseAreaOne.containsMouse || mouseAreaTwo.containsMouse || mouseAreaThree.containsMouse || mouseAreaFour.containsMouse || mouseAreaFive.containsMouse)
                        elisaTheme.ratingIcon
                    else
                        elisaTheme.ratingUnratedIcon
            opacity: if (starRating >= 2 || mouseAreaOne.containsMouse || mouseAreaTwo.containsMouse || mouseAreaThree.containsMouse || mouseAreaFour.containsMouse || mouseAreaFive.containsMouse)
                        1
                    else
                        0.1
        }

        MouseArea {
            id: mouseAreaOne
            enabled: !readOnly
            hoverEnabled: true
            anchors.fill: parent
            onClicked: if (starRating !== 2) {
                           starRating = 2
                       } else {
                           starRating = 0
                       }
        }
    }
    Item {
        Layout.preferredHeight: starSize * 1.5
        Layout.preferredWidth: starSize * 1.5
        Layout.maximumHeight: starSize * 1.5
        Layout.maximumWidth: starSize * 1.5
        Layout.minimumHeight: starSize * 1.5
        Layout.minimumWidth: starSize * 1.5

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: starSize * 1.5
            sourceSize.height: starSize * 1.5
            fillMode: Image.PreserveAspectFit

            layer.enabled: (mouseAreaTwo.containsMouse && starRating !== 4) || (mouseAreaThree.containsMouse && starRating !== 6) ||
                           (mouseAreaFour.containsMouse && starRating !== 8) || (mouseAreaFive.containsMouse && starRating !== 10)
            layer.effect: BrightnessContrast {
                brightness: hoverBrightness
                contrast: hoverContrast
            }

            source: if (starRating >= 4 || mouseAreaTwo.containsMouse || mouseAreaThree.containsMouse || mouseAreaFour.containsMouse || mouseAreaFive.containsMouse)
                        elisaTheme.ratingIcon
                    else
                        elisaTheme.ratingUnratedIcon
            opacity: if (starRating >= 4 || mouseAreaTwo.containsMouse || mouseAreaThree.containsMouse || mouseAreaFour.containsMouse || mouseAreaFive.containsMouse)
                        1
                    else
                        0.1
        }

        MouseArea {
            id: mouseAreaTwo
            enabled: !readOnly
            hoverEnabled: true
            anchors.fill: parent
            onClicked: starRating = 4
        }
    }
    Item {
        Layout.preferredHeight: starSize * 1.5
        Layout.preferredWidth: starSize * 1.5
        Layout.maximumHeight: starSize * 1.5
        Layout.maximumWidth: starSize * 1.5
        Layout.minimumHeight: starSize * 1.5
        Layout.minimumWidth: starSize * 1.5

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: starSize * 1.5
            sourceSize.height: starSize * 1.5
            fillMode: Image.PreserveAspectFit

            layer.enabled: (mouseAreaThree.containsMouse && starRating !== 6) || (mouseAreaFour.containsMouse && starRating !== 8) || (mouseAreaFive.containsMouse && starRating !== 10)
            layer.effect: BrightnessContrast {
                brightness: hoverBrightness
                contrast: hoverContrast
            }

            source: if (starRating >= 6 || mouseAreaThree.containsMouse || mouseAreaFour.containsMouse || mouseAreaFive.containsMouse)
                        elisaTheme.ratingIcon
                    else
                        elisaTheme.ratingUnratedIcon
            opacity: if (starRating >= 6 || mouseAreaThree.containsMouse || mouseAreaFour.containsMouse || mouseAreaFive.containsMouse)
                        1
                    else
                        0.1
        }

        MouseArea {
            id: mouseAreaThree
            enabled: !readOnly
            hoverEnabled: true
            anchors.fill: parent
            onClicked: starRating = 6
        }
    }
    Item {
        Layout.preferredHeight: starSize * 1.5
        Layout.preferredWidth: starSize * 1.5
        Layout.maximumHeight: starSize * 1.5
        Layout.maximumWidth: starSize * 1.5
        Layout.minimumHeight: starSize * 1.5
        Layout.minimumWidth: starSize * 1.5

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: starSize * 1.5
            sourceSize.height: starSize * 1.5
            fillMode: Image.PreserveAspectFit

            layer.enabled: (mouseAreaFour.containsMouse && starRating !== 8) || (mouseAreaFive.containsMouse && starRating !== 10)
            layer.effect: BrightnessContrast {
                brightness: hoverBrightness
                contrast: hoverContrast
            }

            source: if (starRating >= 8 || mouseAreaFour.containsMouse || mouseAreaFive.containsMouse)
                        elisaTheme.ratingIcon
                    else
                        elisaTheme.ratingUnratedIcon
            opacity: if (starRating >= 8 || mouseAreaFour.containsMouse || mouseAreaFive.containsMouse)
                        1
                    else
                        0.1
        }

        MouseArea {
            id: mouseAreaFour
            enabled: !readOnly
            hoverEnabled: true
            anchors.fill: parent
            onClicked: starRating = 8
        }
    }
    Item {
        Layout.preferredHeight: starSize * 1.5
        Layout.preferredWidth: starSize * 1.5
        Layout.maximumHeight: starSize * 1.5
        Layout.maximumWidth: starSize * 1.5
        Layout.minimumHeight: starSize * 1.5
        Layout.minimumWidth: starSize * 1.5

        Layout.rightMargin: starSize * 0.5

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: starSize * 1.5
            sourceSize.height: starSize * 1.5
            fillMode: Image.PreserveAspectFit

            layer.enabled: mouseAreaFive.containsMouse && starRating !== 10
            layer.effect: BrightnessContrast {
                brightness: hoverBrightness
                contrast: hoverContrast
            }

            source: if (starRating === 10 || mouseAreaFive.containsMouse)
                        elisaTheme.ratingIcon
                    else
                        elisaTheme.ratingUnratedIcon
            opacity: if (starRating === 10 || mouseAreaFive.containsMouse)
                        1
                    else
                        0.1
        }

        MouseArea {
            id: mouseAreaFive
            enabled: !readOnly
            hoverEnabled: true
            anchors.fill: parent
            onClicked: starRating = 10
        }
    }
}

