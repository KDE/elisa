import QtQml 2.2
import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Window 2.0

Item {
    id: navigationBar

    property alias parentStackView: backButton.parentStackView
    property var playList
    property string artist
    property string album
    property string image
    property string tracksCount

    RowLayout {
        anchors.fill: parent

        spacing: 0

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }

        Button {
            id: backButton

            property var parentStackView

            iconName: 'go-previous'

            height: Screen.pixelDensity * 8.
            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height

            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

            onClicked: if (listingView.depth > 1) {
                           listingView.pop()
                       } else {
                           parentStackView.pop()
                       }
            text: i18nc("Back navigation button", "Back")
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

            Label {
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

            Label {
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

        Image {
            id: addAlbum
            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    console.log('enqueue ' + album + ' ' + artist)
                    playList.enqueue(album, artist)
                }
            }
            source: 'image://icon/media-track-add-amarok'
            Layout.preferredWidth: Screen.pixelDensity * 11.
            Layout.preferredHeight: Screen.pixelDensity * 11.
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: Screen.pixelDensity * 11.
            Layout.maximumHeight: Screen.pixelDensity * 11.
            Layout.minimumWidth: Screen.pixelDensity * 11.
            Layout.minimumHeight: Screen.pixelDensity * 11.
            sourceSize.width: Screen.pixelDensity * 11.
            sourceSize.height: Screen.pixelDensity * 11.
            height: Screen.pixelDensity * 11.
            width: Screen.pixelDensity * 11.
            fillMode: Image.PreserveAspectFit
            enabled: true
            opacity: enabled ? 1.0 : 0.6
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }

        Image {
            id: clearAddAlbum
            MouseArea {
                anchors.fill: parent
                onClicked: playList.clearAndEnqueue(album, artist)
            }
            source: 'image://icon/media-playback-start'
            Layout.preferredWidth: Screen.pixelDensity * 11.
            Layout.preferredHeight: Screen.pixelDensity * 11.
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: Screen.pixelDensity * 11.
            Layout.maximumHeight: Screen.pixelDensity * 11.
            Layout.minimumWidth: Screen.pixelDensity * 11.
            Layout.minimumHeight: Screen.pixelDensity * 11.
            sourceSize.width: Screen.pixelDensity * 11.
            sourceSize.height: Screen.pixelDensity * 11.
            height: Screen.pixelDensity * 11.
            width: Screen.pixelDensity * 11.
            fillMode: Image.PreserveAspectFit
            enabled: true
            opacity: enabled ? 1.0 : 0.6
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 3.
            Layout.minimumWidth: Screen.pixelDensity * 3.
            Layout.maximumWidth: Screen.pixelDensity * 3.
            Layout.fillHeight: true
        }
    }
}
