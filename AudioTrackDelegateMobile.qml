import QtQuick 2.2
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1

Item {
    id: viewAlbumDelegate

    property string title
    property string artist
    property string itemDecoration
    property string duration
    property int trackRating
    property bool isPlaying

    RowLayout {
        width: parent.width
        height: parent.height
        spacing: 0

        Image {
            id: mainIcon
            source: itemDecoration
            Layout.preferredWidth: parent.height * 0.9
            Layout.preferredHeight: parent.height * 0.9
            width: parent.height * 0.9
            height: parent.height * 0.9
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
        }

        ColumnLayout {
            Layout.preferredWidth: Screen.pixelDensity * 2
            Layout.preferredHeight: viewAlbumDelegate.height
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            spacing: 0

            Item {
                Layout.preferredHeight: Screen.pixelDensity * 2
                Layout.minimumHeight: Screen.pixelDensity * 2
                Layout.maximumHeight: Screen.pixelDensity * 2
            }

            Label {
                id: mainLabel
                text: title
                Layout.preferredWidth: Screen.pixelDensity * 25
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                elide: "ElideRight"
            }

            Item {
                Layout.fillHeight: true
            }

            Label {
                id: authorLabel
                text: artist
                Layout.preferredWidth: Screen.pixelDensity * 20
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                elide: "ElideRight"
            }

            Item {
                Layout.preferredHeight: Screen.pixelDensity * 2
                Layout.minimumHeight: Screen.pixelDensity * 2
                Layout.maximumHeight: Screen.pixelDensity * 2
            }
        }

        Image {
            id: playIcon
            source: 'qrc:/audio-x-generic.svg'
            Layout.preferredWidth: parent.height * 0.5
            Layout.preferredHeight: parent.height * 0.5
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.maximumWidth: parent.height * 0.7
            Layout.maximumHeight: parent.height * 0.7
            width: parent.height * 0.7
            height: parent.height * 0.7
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit
            visible: isPlaying
        }

        Item {
            Layout.preferredWidth: width
            Layout.minimumWidth: width
            Layout.maximumWidth: width
            width: Screen.pixelDensity * 5
        }

        ColumnLayout {
            Layout.preferredHeight: viewAlbumDelegate.height
            Layout.minimumHeight: viewAlbumDelegate.height
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            spacing: 0

            Item {
                Layout.preferredHeight: Screen.pixelDensity * 2
                Layout.minimumHeight: Screen.pixelDensity * 2
                Layout.maximumHeight: Screen.pixelDensity * 2
            }

            Label {
                id: durationLabel
                text: duration
                elide: "ElideRight"
                Layout.alignment: Qt.AlignRight
            }

            Item {
                Layout.fillHeight: true
            }

            RatingStar {
                id: mainRating
                starSize: 20
                starRating: trackRating
                Layout.alignment: Qt.AlignRight
            }

            Item {
                Layout.preferredHeight: Screen.pixelDensity * 2
                Layout.minimumHeight: Screen.pixelDensity * 2
                Layout.maximumHeight: Screen.pixelDensity * 2
            }
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 4
            Layout.minimumWidth: Screen.pixelDensity * 4
            Layout.maximumWidth: Screen.pixelDensity * 4
        }
    }
}

