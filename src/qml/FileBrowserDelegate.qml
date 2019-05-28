/*
 * Copyright 2016-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2018 Alexander Stippich <a.stippich@gmx.net>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2

import org.kde.elisa 1.0

FocusScope {
    id: fileDelegate

    property var fileName
    property var fileUrl
    property var imageUrl
    property var contentModel
    property bool isDirectory
    property bool isPlayList

    signal enqueue(var data)
    signal replaceAndPlay(var data)
    signal loadPlayList(var data)
    signal open(var data)
    signal selected()

    Loader {
        id: metadataLoader
        active: false
        onLoaded: item.show()

        sourceComponent:  MediaTrackMetadataView {
            fileName: fileDelegate.fileUrl
            onRejected: metadataLoader.active = false;
        }
    }

    Keys.onReturnPressed: fileDelegate.enqueue(fileUrl)
    Keys.onEnterPressed: fileDelegate.enqueue(fileUrl)

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        MouseArea {
            id: hoverArea

            hoverEnabled: true
            acceptedButtons: Qt.LeftButton

            Layout.preferredHeight: fileDelegate.width * 0.85 + elisaTheme.layoutVerticalMargin * 0.5 +
                                    (mainLabelSize.boundingRect.height - mainLabelSize.boundingRect.y)
            Layout.fillWidth: true

            onClicked: fileDelegate.selected()

            onDoubleClicked: fileDelegate.open(fileUrl)

            TextMetrics {
                id: mainLabelSize
                font: mainLabel.font
                text: mainLabel.text
            }

            ColumnLayout {
                id: mainData

                spacing: 0
                anchors.fill: parent

                Item {
                    Layout.preferredHeight: fileDelegate.width * 0.85
                    Layout.preferredWidth: fileDelegate.width * 0.85

                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    Loader {
                        id: hoverLoader
                        active: false

                        anchors {
                            bottom: parent.bottom
                            bottomMargin: 2
                            left: parent.left
                            leftMargin: 2
                        }

                        z: 1

                        opacity: 0

                        sourceComponent: Row {
                            spacing: 2

                            Button {
                                id: detailsButton

                                Layout.preferredHeight: elisaTheme.delegateHeight
                                Layout.preferredWidth: elisaTheme.delegateHeight

                                visible: !isDirectory && !isPlayList

                                icon.name: "help-about"
                                onClicked: {
                                    if (metadataLoader.active === false) {
                                        metadataLoader.active = true
                                        metadataLoader.item.trackDataHelper.trackData = contentModel.loadMetaDataFromUrl(fileUrl)
                                    }
                                    else {
                                        metadataLoader.item.close();
                                        metadataLoader.active = false
                                    }
                                }
                                ToolTip {
                                    text: i18nc("Show track metadata", "View Details")
                                }
                            }

                            Button {
                                id: enqueueOpenButton

                                Layout.preferredHeight: elisaTheme.delegateHeight
                                Layout.preferredWidth: elisaTheme.delegateHeight

                                visible: !isPlayList

                                icon.name: isDirectory ?
                                        "go-next-view-page" :
                                        "media-track-add-amarok"
                                onClicked: isDirectory ?
                                        open(fileUrl) :
                                        enqueue(fileUrl)
                                ToolTip {
                                    text: isDirectory ?
                                        i18nc("Open view of the container", "Open") :
                                        i18nc("Enqueue current track", "Enqueue")
                                }
                            }

                            Button {
                                id: replaceAndPlayButton

                                Layout.preferredHeight: elisaTheme.delegateHeight
                                Layout.preferredWidth: elisaTheme.delegateHeight

                                scale: LayoutMirroring.enabled ? -1 : 1
                                visible: !isDirectory

                                icon.name: "media-playback-start"
                                onClicked: replaceAndPlay(fileUrl)
                                ToolTip {
                                    text: i18nc("Clear play list and enqueue current track", "Play Now and Replace Play List")
                                }
                            }
                        }
                    }

                    Image {
                        id: icon
                        anchors.fill: parent

                        sourceSize.width: parent.width
                        sourceSize.height: parent.height
                        fillMode: Image.PreserveAspectFit
                        smooth: true

                        source: imageUrl

                        asynchronous: true
                    }
                }

                LabelWithToolTip {
                    id: mainLabel

                    font.weight: Font.Bold
                    color: myPalette.text

                    // FIXME: Center-aligned text looks better overall, but
                    // sometimes results in font kerning issues
                    // See https://bugreports.qt.io/browse/QTBUG-49646
                    horizontalAlignment: Text.AlignHCenter

                    Layout.topMargin: elisaTheme.layoutVerticalMargin * 0.5
                    Layout.maximumWidth: fileDelegate.width * 0.9
                    Layout.minimumWidth: Layout.maximumWidth
                    Layout.maximumHeight: (mainLabelSize.boundingRect.height - mainLabelSize.boundingRect.y) * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                    text: fileName
                    wrapMode: Label.Wrap
                    elide: Text.ElideRight
                }

                Item {
                    Layout.fillHeight: true
                }
            }
       }

        Item {
            Layout.fillHeight: true
        }
    }

    states: [
        State {
            name: 'notSelected'
            when: !fileDelegate.activeFocus && !hoverArea.containsMouse
            PropertyChanges {
                target: hoverLoader
                active: false
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 0.0
            }
        },
        State {
            name: 'hoveredOrSelected'
            when: fileDelegate.activeFocus || hoverArea.containsMouse
            PropertyChanges {
                target: hoverLoader
                active: true
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 1.0
            }
        }
    ]

    transitions: [
        Transition {
            to: 'hoveredOrSelected'
            SequentialAnimation {
                PropertyAction {
                    properties: "active"
                }
                NumberAnimation {
                    properties: "opacity"
                    easing.type: Easing.InOutQuad
                    duration: 100
                }
            }
        },
        Transition {
            to: 'notSelected'
            SequentialAnimation {
                NumberAnimation {
                    properties: "opacity"
                    easing.type: Easing.InOutQuad
                    duration: 100
                }
                PropertyAction {
                    properties: "active"
                }
            }
        }
    ]
}

