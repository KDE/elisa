import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls 2.0 as Controls2
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2

Label {
    id: theLabel
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        Controls2.ToolTip {
            delay: Qt.styleHints.mousePressAndHoldInterval
            visible: parent.containsMouse && theLabel.truncated
            text: theLabel.text

            contentItem: Label {
                      text: theLabel.text
                      color: myPalette.highlightedText
            }

            enter: Transition { NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; from: 0.0; to: 1.0; duration: 300; } }
            exit: Transition { NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; from: 1.0; to: 0.0; duration: 300; } }

            background: Rectangle {
                color: myPalette.shadow
                radius: Screen.pixelDensity * 0.8

                layer.enabled: true
                layer.effect: DropShadow {
                    horizontalOffset: Screen.pixelDensity * 0.5
                    verticalOffset: Screen.pixelDensity * 0.5
                    radius: 8
                    samples: 17
                    color: myPalette.shadow
                }
            }
        }
    }
}
