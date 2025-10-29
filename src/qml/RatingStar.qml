/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Effects as FX
import org.kde.kirigami as Kirigami

QQC2.Control {
    id: control

    property int starRating
    property bool readOnly: true

    property double hoverBrightness: 0
    property double hoverContrast: -0.3

    property bool highlighted
    property color iconColor: palette.text

    readonly property int hoveredStar: mouseArea.containsMouse ? Math.ceil(5 * mouseArea.mouseX / mouseArea.width) : 0
    readonly property int hoveredRating: 2 * hoveredStar

    signal ratingEdited()

    function decreaseRating() {
        if (!readOnly) {
            starRating = Math.max(0, starRating - 2);
            ratingEdited();
        }
    }

    function increaseRating() {
        if (!readOnly) {
            starRating = Math.min(10, starRating + 2);
            ratingEdited();
        }
    }

    Keys.onLeftPressed: event => {
        if (readOnly) {
            event.accepted = false;
        } else {
            event.accepted = true;
            decreaseRating();
        }
    }

    Keys.onRightPressed: event => {
        if (readOnly) {
            event.accepted = false;
        } else {
            event.accepted = true;
            increaseRating();
        }
    }

    focusPolicy: readOnly ? Qt.NoFocus : Qt.StrongFocus

    padding: Kirigami.Units.smallSpacing
    // Reset paddings after qqc2-desktop-style Control
    topPadding: undefined
    leftPadding: undefined
    rightPadding: undefined
    bottomPadding: undefined

    contentItem: Row {

        spacing: 0

        Repeater {
            model: 5

            Item {
                id: delegate

                required property int index

                readonly property int ratingThreshold: 2 + index * 2

                width: Kirigami.Units.iconSizes.small
                height: Kirigami.Units.iconSizes.small

                Kirigami.Icon {
                    width: Kirigami.Units.iconSizes.small
                    height: Kirigami.Units.iconSizes.small
                    anchors.centerIn: parent

                    layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software && control.hoveredRating >= delegate.ratingThreshold

                    layer.effect: FX.MultiEffect {
                        brightness: control.hoverBrightness
                        contrast: control.hoverContrast
                    }

                    animated: false
                    source: (control.starRating >= delegate.ratingThreshold || control.hoveredRating >= delegate.ratingThreshold)
                        ? "starred"
                        : "non-starred"

                    opacity: (control.starRating >= delegate.ratingThreshold || control.hoveredRating >= delegate.ratingThreshold)
                        ? 1 : 0.7

                    selected: control.highlighted
                    color: control.iconColor
                }
            }
        }
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent

        enabled: !control.readOnly

        acceptedButtons: Qt.LeftButton
        hoverEnabled: true

        onClicked: {
            if (control.starRating !== control.hoveredRating) {
                control.starRating = control.hoveredRating
            } else {
                control.starRating = 0
            }
            control.ratingEdited()
        }
    }

    background: Rectangle {
        color: "transparent"
        border.color: control.palette.highlight
        border.width: 1
        radius: Kirigami.Units.cornerRadius

        opacity: control.activeFocus && [Qt.TabFocusReason, Qt.BacktabFocusReason].includes(control.focusReason)
            ? 1 : 0

        Behavior on opacity {
            OpacityAnimator {
                duration: Kirigami.Units.shortDuration
                easing.type: Easing.InOutCubic
            }
        }
    }
}
