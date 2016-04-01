import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQml.Models 2.1
import org.kde.kirigami 1.0 as MobileComponents
import org.mgallien.QmlExtension 1.0

MobileComponents.Page {
    property var serverModel
    property var globalPlayListModel
    property string globalBrowseFlag: 'BrowseDirectChildren'
    property string globalFilter: '*'
    property string globalSortCriteria: ''

    id: rootItem

    objectName: "ServerList"

    ColumnLayout {
        anchors.fill: parent

        MobileComponents.Heading {
            Layout.bottomMargin: MobileComponents.Units.largeSpacing
            text: "UPnP Servers"
            Layout.fillWidth: true
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ListView {
                id: serverView
                model: DelegateModel {
                    model: serverModel

                    delegate: MobileComponents.SwipeListItem {
                        height: 120
                        enabled: true

                        MobileComponents.Label {
                            id: nameLabel
                            anchors.fill: parent
                            text: model.name
                        }


                        onClicked: {
                            contentDirectoryItem.contentModel = serverModel.remoteAlbumModel(DelegateModel.itemsIndex)
                            contentDirectoryItem.serverName = serverModel.remoteAlbumModel(DelegateModel.itemsIndex).serverName
                            stackView.push(contentDirectoryItem)
                        }
                    }
                }
            }
        }

        MediaServerListing {
            id: contentDirectoryItem

            playListModel: globalPlayListModel

            visible: false
        }
    }
}
