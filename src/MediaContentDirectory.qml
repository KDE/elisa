import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Item {
    property var remoteMediaServer: ({})
    property var pagesModel
    property StackView parentStackView
    property MediaPlayList playListModel

    property string globalBrowseFlag: 'BrowseDirectChildren'
    property string globalFilter: '*'
    property string globalSortCriteria: ''

    id: contentDirectoryRoot

    UpnpContentDirectoryModel {
        id: contentDirectoryModel
        browseFlag: globalBrowseFlag
        filter: globalFilter
        sortCriteria: globalSortCriteria
        contentDirectory: remoteMediaServer.contentDirectory

        onContentDirectoryChanged: listingView.initialItem.rootIndex = contentDirectoryModel.indexFromId('0')
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Button {
            id: backButton

            height: Screen.pixelDensity * 8.
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

            delegate: StackViewDelegate {
                function transitionFinished(properties)
                {
                    properties.exitItem.opacity = 1
                }

                pushTransition: StackViewTransition {
                    PropertyAnimation {
                        target: enterItem
                        property: "opacity"
                        from: 0
                        to: 1
                    }
                    PropertyAnimation {
                        target: exitItem
                        property: "opacity"
                        from: 1
                        to: 0
                    }
                }
            }

            initialItem: MediaServerListing {
                contentDirectoryService: remoteMediaServer.contentDirectory
                rootIndex: remoteMediaServer ? '0' : ''
                stackView: listingView
                contentModel: contentDirectoryModel
                playListModel: contentDirectoryRoot.playListModel
            }

            // Implements back key navigation
            focus: true
            Keys.onReleased: if (event.key === Qt.Key_Back && stackView.depth > 1) {
                                 stackView.pop();
                                 event.accepted = true;
                             }
        }
    }
}
