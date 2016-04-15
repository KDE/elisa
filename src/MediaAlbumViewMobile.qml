import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import org.kde.kirigami 1.0 as MobileComponents
import QtMultimedia 5.5
import QtQuick.Layouts 1.2

MobileComponents.Page {
    id: topListing

    title: albumName

    property var rootIndex
    property var contentModel
    property MediaPlayList playListModel
    property var albumName
    property var artistName
    property var tracksCount
    property var albumArtUrl

    ColumnLayout {
        anchors.fill: parent

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ListView {
                id: contentDirectoryView

                boundsBehavior: Flickable.StopAtBounds
                focus: true
                clip: true

                model: DelegateModel {
                    model: contentModel
                    rootIndex: topListing.rootIndex

                    delegate: AlbumTrackDelegateMobile {
                        title: (model != undefined && model.title !== undefined) ? model.title : ''
                        artist: (model != undefined && model.artist !== undefined) ? model.artist : ''
                        album: (model != undefined && model.album !== undefined) ? model.album : ''
                        itemDecoration: (model != undefined && model.image !== undefined) ? model.image : ''
                        duration: (model != undefined && model.duration !== undefined) ? model.duration : ''

                        onClicked: playListModel.enqueue(contentDirectoryView.model.modelIndex(index))
                    }
                }

                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}
