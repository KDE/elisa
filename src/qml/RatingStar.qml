/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami

Row {
    property int starRating
    property bool readOnly: true

    property double hoverBrightness: 0.5
    property double hoverContrast: 0.5
    property double hoverWidgetOpacity: 0

    property int hoveredRating: 0

    signal ratingEdited()

    spacing: 0
    opacity: starRating >= 2 ? 1 : hoverWidgetOpacity

    Repeater {
        model: 5

        Item {
            property int ratingThreshold: 2 + index * 2

            height: Kirigami.Units.iconSizes.small
            width: Kirigami.Units.iconSizes.small

            Image {
                width: Kirigami.Units.iconSizes.small
                height: Kirigami.Units.iconSizes.small
                anchors.centerIn: parent
                sourceSize.width: Kirigami.Units.iconSizes.small
                sourceSize.height: Kirigami.Units.iconSizes.small
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

                onClicked: {
                    if (starRating !== ratingThreshold) {
                        starRating = ratingThreshold
                    } else {
                        starRating = 0
                    }
                    ratingEdited()
                }

                onEntered: hoveredRating = ratingThreshold
                onExited: hoveredRating = 0
            }
        }
    }
}
