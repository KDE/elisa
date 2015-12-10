import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Item {
    property UpnpControlMediaServer mediaServerDevice
    property StackView parentStackView
    property UpnpControlConnectionManager connectionManager
    property MediaPlayList playListModel

    property string globalBrowseFlag: 'BrowseDirectChildren'
    property string globalFilter: '*'
    property string globalSortCriteria: ''

    UpnpContentDirectoryModel {
        id: contentDirectoryModel
        browseFlag: globalBrowseFlag
        filter: globalFilter
        sortCriteria: globalSortCriteria
        contentDirectory: mediaServerDevice.serviceById('urn:upnp-org:serviceId:ContentDirectory')
        useLocalIcons: true
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Button {
            id: backButton

            height: 25
            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height
            Layout.fillWidth: true

            onClicked: if (listingView.depth > 1) {
                           listingView.pop()
                       } else {
                           parentStackView.pop()
                       }
            text: 'Back'
        }

        StackView {
            id: listingView

            Layout.fillHeight: true
            Layout.fillWidth: true

            // Implements back key navigation
            focus: true
            Keys.onReleased: if (event.key === Qt.Key_Back && stackView.depth > 1) {
                                 stackView.pop();
                                 event.accepted = true;
                             }
        }
    }

    Component.onCompleted: {
        console.log(parentStackView)
        connectionManager = mediaServerDevice.serviceById('urn:upnp-org:serviceId:ConnectionManager')
        listingView.push({
                             item: Qt.resolvedUrl("mediaServerListingMobile.qml"),
                             properties: {
                                 'contentDirectoryService': contentDirectoryModel.contentDirectory,
                                 'rootId': '0',
                                 'stackView': listingView,
                                 'contentModel': contentDirectoryModel,
                                 'playListModel': playListModel,
                             }
                         })
    }
}
