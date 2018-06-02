/*
 * Copyright 2016-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2018 Alexander Stippich <a.stippich@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as Controls1
import QtQuick.Layouts 1.2

import org.kde.elisa 1.0

FocusScope {
    id: fileDelegate

    property var fileName
    property var fileUrl
    property var imageUrl
    property var contentModel
    property bool isDirectory

    signal enqueue(var data)
    signal replaceAndPlay(var data)
    signal open(var data)
    signal selected()

    Controls1.Action {
        id: replaceAndPlayAction
        text: i18nc("Clear play list and enqueue current track", "Play Now and Replace Play List")
        iconName: "media-playback-start"
        onTriggered: replaceAndPlay(fileUrl)
    }

    Controls1.Action {
        id: enqueueAction
        text: i18nc("Enqueue current track", "Enqueue")
        iconName: "media-track-add-amarok"
        onTriggered: enqueue(fileUrl)
    }

    Controls1.Action {
        id: openAction
        text: i18nc("Enqueue current track", "Enqueue")
        iconName: 'document-open-folder'
        onTriggered: open(fileUrl)
    }

    Controls1.Action {
        id: viewDetailsAction
        text: i18nc("Show track metadata", "View Details")
        iconName: "help-about"
        onTriggered: {
            if (metadataLoader.active === false) {
                metadataLoader.active = true
                metadataLoader.item.trackDataHelper.trackData = contentModel.loadMetaDataFromUrl(fileUrl)
            }
            else {
                metadataLoader.item.close();
                metadataLoader.active = false
            }
        }
    }

    Loader {
        id: metadataLoader
        active: false
        onLoaded: item.show()

        sourceComponent:  MediaTrackMetadataView {
            trackDataHelper: TrackDataHelper {
                id: dataHelper
            }

            onRejected: metadataLoader.active = false;
        }
    }

    Keys.onReturnPressed: isDirectory ? fileDelegate.open(fileUrl) : fileDelegate.enqueue(fileUrl)
    Keys.onEnterPressed: isDirectory ? fileDelegate.open(fileUrl) : fileDelegate.enqueue(fileUrl)

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        MouseArea {
            id: hoverArea

            hoverEnabled: true
            acceptedButtons: Qt.LeftButton

            Layout.preferredHeight: fileDelegate.width * 0.85 + elisaTheme.layoutVerticalMargin * 0.5 + mainLabelSize.height
            Layout.fillWidth: true

            onClicked: fileDelegate.selected()

            onDoubleClicked: isDirectory ? fileDelegate.open(fileUrl) : fileDelegate.enqueue(fileUrl)

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

                        anchors.centerIn: parent
                        z: 1

                        opacity: 0

                        sourceComponent: Row {

                            Controls1.ToolButton {
                                id: detailsButton

                                Layout.preferredHeight: elisaTheme.delegateHeight
                                Layout.preferredWidth: elisaTheme.delegateHeight

                                action: viewDetailsAction
                                visible: !isDirectory
                            }

                            Controls1.ToolButton {
                                id: enqueueOpenButton

                                Layout.preferredHeight: elisaTheme.delegateHeight
                                Layout.preferredWidth: elisaTheme.delegateHeight

                                action: isDirectory ? openAction : enqueueAction
                            }

                            Controls1.ToolButton {
                                id: replaceAndPlayButton

                                Layout.preferredHeight: elisaTheme.delegateHeight
                                Layout.preferredWidth: elisaTheme.delegateHeight

                                scale: LayoutMirroring.enabled ? -1 : 1
                                action: replaceAndPlayAction
                                visible: !isDirectory
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

                    horizontalAlignment: Text.AlignLeft

                    Layout.topMargin: elisaTheme.layoutVerticalMargin * 0.5
                    Layout.preferredWidth: fileDelegate.width * 0.85
                    Layout.maximumHeight: mainLabelSize.height * 2
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
            PropertyChanges {
                target: icon
                opacity: 1
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
            PropertyChanges {
                target: icon
                opacity: 0.2
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

