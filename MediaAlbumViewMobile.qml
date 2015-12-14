import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import org.kde.plasma.mobilecomponents 0.2 as MobileComponents
import QtMultimedia 5.4
import QtQuick.Layouts 1.2

MobileComponents.Page {
    property UpnpControlContentDirectory contentDirectoryService
    property string rootId
    property StackView stackView
    property UpnpContentDirectoryModel contentModel
    property MediaPlayList playListModel

    id: rootElement

    color: MobileComponents.Theme.viewBackgroundColor
    flickable: contentDirectoryView

    ColumnLayout {
        anchors.fill: parent

        MobileComponents.Heading {
            Layout.bottomMargin: MobileComponents.Units.largeSpacing
            text: "Album"
            Layout.fillWidth: true
        }

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
                    rootIndex: contentModel.indexFromId(rootId)

                    delegate: AudioTrackDelegate {
                        height: Screen.pixelDensity * 12.
                        width: contentDirectoryView.width
                        title: if (model != undefined && model.title !== undefined)
                                   model.title
                               else
                                   ''
                        artist: if (model != undefined && model.artist !== undefined)
                                    model.artist
                                else
                                    ''
                        itemDecoration: if (model != undefined && model.image !== undefined)
                                            model.image
                                        else
                                            ''
                        duration: if (model != undefined && model.duration !== undefined)
                                      model.duration
                                  else
                                      ''
                        trackRating: if (model != undefined && model.rating !== undefined)
                                         model.rating
                                     else
                                         ''
                        isPlaying: if (model != undefined && model.isPlaying !== undefined)
                                       model.isPlaying
                                   else
                                       false
                    }
                }

                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}
