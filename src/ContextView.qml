import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4
import QtQuick.Layouts 1.2

Item {
    id: topItem

    property var albumName
    property var artistName
    property var tracksCount
    property var albumArtUrl

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        Image {
            id: albumIcon
            source: albumArtUrl
            Layout.preferredWidth: width
            Layout.preferredHeight: height
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.maximumWidth: width
            Layout.maximumHeight: height
            width: Screen.pixelDensity * 45.
            height: Screen.pixelDensity * 45.
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit
        }

        Item {
            Layout.preferredHeight: Screen.pixelDensity * 0.5
            Layout.minimumHeight: Screen.pixelDensity * 0.5
            Layout.maximumHeight: Screen.pixelDensity * 0.5
        }

        Label {
            id: titleLabel
            text: if (albumName !== undefined)
                      albumName
                  else
                      ''
            font.weight: Font.Bold
            horizontalAlignment: Text.AlignHCenter

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

            elide: "ElideRight"
        }

        Label {
            id: artistLabel
            text: if (artistName !== undefined)
                      artistName
                  else
                      ''
            font.weight: Font.Normal
            horizontalAlignment: Text.AlignHCenter

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

            elide: "ElideRight"
        }

        Label {
            id: numberLabel
            text: if (tracksCount === 1)
                      tracksCount + ' Song'
                  else
                      tracksCount + ' Songs'

            visible: tracksCount !== undefined

            font.weight: Font.Light
            horizontalAlignment: Text.AlignHCenter

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

            elide: "ElideRight"
        }

        Item {
            Layout.preferredHeight: Screen.pixelDensity * 2.
            Layout.minimumHeight: Screen.pixelDensity * 2.
            Layout.maximumHeight: Screen.pixelDensity * 2.
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: Screen.pixelDensity * 2.

            spacing: 0

            Item {
                Layout.preferredWidth: Screen.pixelDensity * 2.
                Layout.minimumWidth: Screen.pixelDensity * 2.
                Layout.maximumWidth: Screen.pixelDensity * 2.
            }

            Image {
                id: artistJumpIcon
                source: 'image://icon/view-media-artist'
                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                Layout.maximumWidth: width
                Layout.maximumHeight: height
                width: Screen.pixelDensity * 8.
                height: Screen.pixelDensity * 8.
                sourceSize.width: width
                sourceSize.height: width
                fillMode: Image.PreserveAspectFit
            }

            Label {
                text: if (artistName !== undefined)
                          artistName
                      else
                          ''
                font.weight: Font.Normal
                horizontalAlignment: Text.AlignLeft
            }
        }
    }
}
