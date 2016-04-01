import QtQuick 2.4
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.2
import org.kde.kirigami 1.0 as MobileComponents

MobileComponents.SwipeListItem {
    id: viewAlbumDelegate
    enabled: true

    property string title
    property string artist
    property string itemDecoration
    property string duration
    property int trackRating
    property bool isPlaying
    property bool showHoverButtons
    property int itemIndex

    signal remove(int indexToRemove)

    RowLayout {
        spacing: 0
        anchors.fill: parent

        ColumnLayout {
            Layout.preferredWidth: parent.height
            Layout.preferredHeight: parent.height
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            spacing: 0

            Item {
                Layout.preferredHeight: 1
                Layout.minimumHeight: 1
                Layout.maximumHeight: 1
            }

            Image {
                id: mainIcon
                source: itemDecoration
                Layout.preferredWidth: parent.height - 2
                Layout.preferredHeight: parent.height - 2
                width: parent.height - 2
                sourceSize.width: width
                sourceSize.height: width
                fillMode: Image.PreserveAspectFit
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                Button {
                    id: removeButton
                    iconName: 'list-remove'

                    anchors.fill: parent

                    visible: showHoverButtons
                    opacity: 0.8
                    enabled: true

                    onClicked: remove(itemIndex)
                }
            }

            Item {
                Layout.preferredHeight: Screen.pixelDensity * 0.5
                Layout.minimumHeight: Screen.pixelDensity * 0.5
                Layout.maximumHeight: Screen.pixelDensity * 0.5
            }
        }

        ColumnLayout {
            Layout.preferredWidth: Screen.pixelDensity * 12.
            Layout.preferredHeight: viewAlbumDelegate.height
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            spacing: 0

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            MobileComponents.Label {
                id: mainLabel
                text: title
                Layout.preferredWidth: Screen.pixelDensity * 12
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                elide: "ElideRight"
                verticalAlignment: Text.AlignTop
            }

            Item {
                Layout.preferredHeight: Screen.pixelDensity * 3.
                Layout.minimumHeight: Screen.pixelDensity * 3.
                Layout.maximumHeight: Screen.pixelDensity * 3.
                Layout.fillWidth: true
            }

            MobileComponents.Label {
                id: authorLabel
                text: artist + ' - ' + album
                Layout.preferredWidth: Screen.pixelDensity * 12
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                elide: "ElideRight"
                verticalAlignment: Text.AlignTop
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 0.5
            Layout.minimumWidth: Screen.pixelDensity * 0.5
            Layout.maximumWidth: Screen.pixelDensity * 0.5
        }

        Button {
            id: playButton
            iconName: 'media-playback-start'

            visible: showHoverButtons && !isPlaying

            Layout.preferredWidth: parent.height * 0.5
            Layout.preferredHeight: parent.height * 0.5
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.maximumWidth: parent.height * 0.7
            Layout.maximumHeight: parent.height * 0.7
            width: parent.height * 0.7
            height: parent.height * 0.7
        }

        Image {
            id: playIcon
            source: 'image://icon/media-playback-start'
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
            Layout.preferredWidth: Screen.pixelDensity * 0.5
            Layout.minimumWidth: Screen.pixelDensity * 0.5
            Layout.maximumWidth: Screen.pixelDensity * 0.5

            visible: (showHoverButtons && !isPlaying) || isPlaying
        }

        ColumnLayout {
            Layout.preferredHeight: viewAlbumDelegate.height
            Layout.minimumHeight: viewAlbumDelegate.height
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            spacing: 0

            Item {
                Layout.preferredHeight: Screen.pixelDensity * 2.
                Layout.minimumHeight: Screen.pixelDensity * 2.
                Layout.maximumHeight: Screen.pixelDensity * 2.
            }

            MobileComponents.Label {
                id: durationLabel
                text: duration
                elide: "ElideRight"
                Layout.alignment: Qt.AlignRight
            }

            Item {
                Layout.fillHeight: true
            }
        }

        Item {
            Layout.preferredWidth: Screen.pixelDensity * 0.5
            Layout.minimumWidth: Screen.pixelDensity * 0.5
            Layout.maximumWidth: Screen.pixelDensity * 0.5
        }
    }
}

