import QtQml 2.2
import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Window 2.0

Item {
    id: navigationBar

    property var parentStackView
    property var playList
    property var playerControl
    property string artist
    property string album
    property string image
    property string tracksCount

    Action {
        id: goPreviousAction
        text: i18nc("navigate back in the views stack", "Back")
        iconName: "go-previous"
        onTriggered:
        {
            if (listingView.depth > 1) {
                listingView.pop()
            } else {
                parentStackView.pop()
            }
        }
    }

    Action {
        id: enqueueAction
        text: i18nc("Add whole album to play list", "Enqueue")
        iconName: "media-track-add-amarok"
        onTriggered: playList.enqueue(album, artist)
    }

    Action {
        id: clearAndEnqueueAction
        text: i18nc("Clear play list and add whole album to play list", "Play Now and Replace Play List")
        iconName: "media-playback-start"
        onTriggered: {
            playList.clearAndEnqueue(album, artist)
            playerControl.playPause()
        }
    }

    RowLayout {
        anchors.fill: parent

        spacing: 0

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }

        ToolButton {
            action: goPreviousAction
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }

        Image {
            id: mainIcon
            source: image
            fillMode: Image.PreserveAspectFit

            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

            Layout.preferredHeight: Screen.pixelDensity * 20.
            Layout.minimumHeight: Screen.pixelDensity * 20.
            Layout.maximumHeight: Screen.pixelDensity * 20.
            Layout.preferredWidth: Screen.pixelDensity * 20.
            Layout.minimumWidth: Screen.pixelDensity * 20.
            Layout.maximumWidth: Screen.pixelDensity * 20.
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }

        ColumnLayout {
            Layout.fillHeight: true

            spacing: 0

            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.preferredWidth: Screen.pixelDensity * 40.
            Layout.minimumWidth: Screen.pixelDensity * 40.
            Layout.maximumWidth: Screen.pixelDensity * 40.

            Item {
                Layout.fillHeight: true
            }

            Text {
                id: authorLabel
                text: artist
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                elide: "ElideRight"
                font.weight: Font.Bold
            }

            Item {
                Layout.fillHeight: true
            }

            Text {
                id: albumLabel
                text: album
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                elide: "ElideRight"
            }

            Item {
                Layout.fillHeight: true
            }
        }

        Item {
            Layout.fillWidth: true
        }

        ToolButton {
            action: enqueueAction
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }

        ToolButton {
            action: clearAndEnqueueAction
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }
    }
}
