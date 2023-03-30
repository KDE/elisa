/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import ElisaGraphicalEffects 1.15
import org.kde.kirigami 2.5 as Kirigami

Row {
    id: control

    property int starRating
    property bool readOnly: true

    property double hoverBrightness: 0.5
    property double hoverContrast: 0.5

    property int hoveredRating: 0

    signal ratingEdited()

    spacing: 0

    Repeater {
        model: 5

        Item {
            id: delegate

            readonly property int ratingThreshold: 2 + index * 2

            width: Kirigami.Units.iconSizes.small
            height: Kirigami.Units.iconSizes.small

            Kirigami.Icon {
                width: Kirigami.Units.iconSizes.small
                height: Kirigami.Units.iconSizes.small
                anchors.centerIn: parent

                layer.enabled: control.hoveredRating >= delegate.ratingThreshold

                layer.effect: BrightnessContrast {
                    brightness: control.hoverBrightness
                    contrast: control.hoverContrast
                }

                source: (control.starRating >= delegate.ratingThreshold || control.hoveredRating >= delegate.ratingThreshold)
                    ? Qt.resolvedUrl(elisaTheme.ratingIcon)
                    : Qt.resolvedUrl(elisaTheme.ratingUnratedIcon)

                opacity: (control.starRating >= delegate.ratingThreshold || control.hoveredRating >= delegate.ratingThreshold)
                    ? 1 : 0.7
            }

            MouseArea {
                anchors.fill: parent

                enabled: !control.readOnly

                acceptedButtons: Qt.LeftButton
                hoverEnabled: true

                onClicked: {
                    if (control.starRating !== delegate.ratingThreshold) {
                        control.starRating = delegate.ratingThreshold
                    } else {
                        control.starRating = 0
                    }
                    control.ratingEdited()
                }

                onEntered: control.hoveredRating = delegate.ratingThreshold
                onExited: control.hoveredRating = 0
            }
        }
    }
}
