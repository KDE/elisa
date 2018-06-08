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

RowLayout {
    property int starSize
    property int starRating
    property bool readOnly: true

    property double hoverBrightness: 0.5
    property double hoverContrast: 0.5
    property double hoverWidgetOpacity: 0

    property int hoveredRating: 0

    spacing: 0
    opacity: starRating > 0 ? 1 : (readOnly ? 0 : hoverWidgetOpacity)

    Repeater {
        model: 5

        Item {
            property int ratingThreshold: 2 + index * 2
            Layout.preferredHeight: starSize
            Layout.preferredWidth: starSize
            Layout.maximumHeight: starSize
            Layout.maximumWidth: starSize
            Layout.minimumHeight: starSize
            Layout.minimumWidth: starSize

            Image {
                width: starSize
                height: starSize
                anchors.centerIn: parent
                sourceSize.width: starSize
                sourceSize.height: starSize
                fillMode: Image.PreserveAspectFit

                layer.enabled: hoveredRating >= ratingThreshold

                layer.effect: BrightnessContrast {
                    brightness: hoverBrightness
                    contrast: hoverContrast
                }

                source: if (starRating >= ratingThreshold || hoveredRating >= ratingThreshold)
                            Qt.resolvedUrl(elisaTheme.ratingIcon)
                        else
                            Qt.resolvedUrl(elisaTheme.ratingUnratedIcon)
                opacity: if (starRating >= ratingThreshold || hoveredRating >= ratingThreshold)
                            1
                        else
                            0.7
            }

            MouseArea {
                anchors.fill: parent

                enabled: !readOnly

                acceptedButtons: Qt.LeftButton
                hoverEnabled: true

                onClicked: if (starRating !== ratingThreshold) {
                               starRating = ratingThreshold
                           } else {
                               starRating = 0
                           }

                onEntered: hoveredRating = ratingThreshold
                onExited: hoveredRating = 0
            }
        }
    }
}
