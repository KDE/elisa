import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import org.mgallien.QmlExtension 1.0
import org.kde.plasma.mobilecomponents 0.2 as MobileComponents
import QtMultimedia 5.4

MobileComponents.Page {
    property var pagesModel
    property var contentDirectoryModel
    property MediaPlayList playListModel
    property var remoteMediaServer: ({})

    id: contentDirectoryRoot

    objectName: "ServerContent"
    color: MobileComponents.Theme.viewBackgroundColor

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        MobileComponents.Heading {
            Layout.bottomMargin: MobileComponents.Units.largeSpacing
            text: "Server Content"
            Layout.fillWidth: true
        }

        /*Button {
            id: backButton

            height: Screen.pixelDensity * 8.
            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height
            Layout.fillWidth: true

            onClicked: parentStackView.pop()
            text: 'Back'
        }*/
    }

    MediaServerListing {
        contentDirectoryService: contentDirectoryModel.contentDirectory
        rootIndex: '0'
        contentModel: contentDirectoryModel
        playListModel: contentDirectoryRoot.playListModel
        visible: false
    }
}
