/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami

FocusScope {
    id: gridEntry

    property var imageUrl
    property bool shadowForImage
    property url fileUrl
    property var entryType
    property alias mainText: mainLabel.text
    property alias secondaryText: secondaryLabel.text
    property var databaseId
    property bool delegateDisplaySecondaryText: true
    property bool isPartial
    property bool isSelected
    property bool showDetailsButton: false
    property bool showOpenButton: true
    property bool showPlayButton: true
    property bool showEnqueueButton: true

    signal enqueue(var databaseId, var name, var url)
    signal replaceAndPlay(var databaseId, var name, var url)
    signal open()
    signal selected()

    Loader {
        id: metadataLoader
        active: false && gridEntry.fileUrl
        onLoaded: item.show()

        sourceComponent:  MediaTrackMetadataView {
            fileName: gridEntry.fileUrl ? gridEntry.fileUrl : ''
            showImage: true
            modelType: gridEntry.entryType
            showTrackFileName: true
            showDeleteButton: false
            showApplyButton: false
            editableMetadata: false

            onRejected: metadataLoader.active = false;
        }
    }


    Keys.onReturnPressed: open()
    Keys.onEnterPressed: open()

    Accessible.role: Accessible.ListItem
    Accessible.name: mainText

    Rectangle {
        id: stateIndicator

        anchors.fill: parent
        z: 1

        color: "transparent"
        opacity: 0.4

        radius: 3
    }

    ColumnLayout {
        anchors.fill: parent
        z: 2

        spacing: 0

        MouseArea {
            id: hoverHandle

            hoverEnabled: true
            acceptedButtons: Qt.LeftButton

            Layout.preferredHeight: gridEntry.height
            Layout.fillWidth: true

            onClicked: {
                gridEntry.selected()
            }

            onDoubleClicked: open()

            TextMetrics {
                id: mainLabelSize
                font: mainLabel.font
                text: mainLabel.text
            }

            TextMetrics {
                id: secondaryLabelSize
                font: secondaryLabel.font
                text: secondaryLabel.text
            }

            ColumnLayout {
                id: mainData

                spacing: 0
                anchors.fill: parent

                Item {
                    Layout.preferredHeight: gridEntry.width * elisaTheme.gridDelegateThumbnail
                    Layout.preferredWidth: gridEntry.width * elisaTheme.gridDelegateThumbnail

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
                                objectName: 'detailsButton'

                                icon.name: 'help-about'

                                hoverEnabled: true
                                ToolTip.visible: hovered
                                ToolTip.delay: 1000
                                ToolTip.text: i18nc("Show track metadata", "View Details")

                                Accessible.role: Accessible.Button
                                Accessible.name: ToolTip.text
                                Accessible.description: ToolTip.text
                                Accessible.onPressAction: clicked()

                                onClicked: {
                                    if (metadataLoader.active === false) {
                                        metadataLoader.active = true
                                    }
                                    else {
                                        metadataLoader.item.close();
                                        metadataLoader.active = false
                                    }
                                }

                                Keys.onReturnPressed: clicked()
                                Keys.onEnterPressed: clicked()
                                visible: showDetailsButton

                                width: elisaTheme.delegateToolButtonSize
                                height: elisaTheme.delegateToolButtonSize
                            }

                            Button {
                                id: replaceAndPlayButton
                                objectName: 'replaceAndPlayButton'

                                icon.name: 'media-playback-start'

                                hoverEnabled: true
                                ToolTip.visible: hovered
                                ToolTip.delay: 1000
                                ToolTip.text: i18nc("Clear play list and add whole container to play list", "Play now, replacing current playlist")

                                Accessible.role: Accessible.Button
                                Accessible.name: ToolTip.text
                                Accessible.description: ToolTip.text
                                Accessible.onPressAction: onClicked

                                onClicked: replaceAndPlay(databaseId, mainText, fileUrl)
                                Keys.onReturnPressed: replaceAndPlay(databaseId, mainText, fileUrl)
                                Keys.onEnterPressed: replaceAndPlay(databaseId, mainText, fileUrl)

                                visible: showPlayButton

                                width: elisaTheme.delegateToolButtonSize
                                height: elisaTheme.delegateToolButtonSize
                            }

                            Button {
                                id: enqueueButton
                                objectName: 'enqueueButton'

                                icon.name: 'list-add'
                                hoverEnabled: true
                                ToolTip.visible: hovered
                                ToolTip.delay: 1000
                                ToolTip.text: i18nc("Add whole container to play list", "Add to playlist")

                                Accessible.role: Accessible.Button
                                Accessible.name: ToolTip.text
                                Accessible.description: ToolTip.text
                                Accessible.onPressAction: onClicked

                                onClicked: enqueue(databaseId, mainText, fileUrl)
                                Keys.onReturnPressed: enqueue(databaseId, mainText, fileUrl)
                                Keys.onEnterPressed: enqueue(databaseId, mainText, fileUrl)

                                visible: showEnqueueButton

                                width: elisaTheme.delegateToolButtonSize
                                height: elisaTheme.delegateToolButtonSize
                            }

                            Button {
                                id: openButton
                                objectName: 'openButton'

                                icon.name: 'go-next-view-page'
                                hoverEnabled: true
                                ToolTip.visible: hovered
                                ToolTip.delay: 1000
                                ToolTip.text: i18nc("Open view of the container", "Open")

                                Accessible.role: Accessible.Button
                                Accessible.name: ToolTip.text
                                Accessible.description: ToolTip.text
                                Accessible.onPressAction: onClicked

                                onClicked: open()

                                visible: showOpenButton

                                width: elisaTheme.delegateToolButtonSize
                                height: elisaTheme.delegateToolButtonSize
                            }
                        }
                    }

                    Loader {
                        id: coverImageLoader

                        active: !isPartial

                        anchors.fill: parent

                        sourceComponent: Image {
                            id: coverImage

                            anchors.fill: parent

                            sourceSize.width: parent.width
                            sourceSize.height: parent.height
                            fillMode: Image.PreserveAspectFit
                            smooth: true

                            source: (gridEntry.imageUrl !== undefined ? gridEntry.imageUrl : "")

                            asynchronous: true

                            layer.enabled: shadowForImage
                            layer.effect: DropShadow {
                                source: coverImage

                                radius: 10
                                spread: 0.1
                                samples: 21

                                color: myPalette.shadow
                            }
                        }
                    }
                    Loader {
                        active: isPartial

                        anchors.centerIn: parent
                        height: Kirigami.Units.gridUnit * 5
                        width: height


                        sourceComponent: BusyIndicator {
                            anchors.centerIn: parent

                            running: true
                        }
                    }
                }

                LabelWithToolTip {
                    id: mainLabel

                    level: 4

                    color: myPalette.text

                    // FIXME: Center-aligned text looks better overall, but
                    // sometimes results in font kerning issues
                    // See https://bugreports.qt.io/browse/QTBUG-49646
                    horizontalAlignment: Text.AlignHCenter

                    Layout.topMargin: elisaTheme.layoutVerticalMargin * 0.5
                    Layout.maximumWidth: gridEntry.width * 0.9
                    Layout.minimumWidth: Layout.maximumWidth
                    Layout.maximumHeight: delegateDisplaySecondaryText
                                            ? (mainLabelSize.boundingRect.height - mainLabelSize.boundingRect.y)
                                            : (mainLabelSize.boundingRect.height - mainLabelSize.boundingRect.y) * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                    Layout.bottomMargin: delegateDisplaySecondaryText ? 0 : elisaTheme.layoutVerticalMargin

                    wrapMode: delegateDisplaySecondaryText ? Label.NoWrap : Label.Wrap
                    elide: Text.ElideRight
                }

                LabelWithToolTip {
                    id: secondaryLabel

                    opacity: 0.6
                    color: myPalette.text

                    // FIXME: Center-aligned text looks better overall, but
                    // sometimes results in font kerning issues
                    // See https://bugreports.qt.io/browse/QTBUG-49646
                    horizontalAlignment: Text.AlignHCenter

                    Layout.bottomMargin: elisaTheme.layoutVerticalMargin
                    Layout.maximumWidth: gridEntry.width * 0.9
                    Layout.minimumWidth: Layout.maximumWidth
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

                    visible: delegateDisplaySecondaryText

                    elide: Text.ElideRight
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
            when: !gridEntry.activeFocus && !hoverHandle.containsMouse && !gridEntry.isSelected
            PropertyChanges {
                target: stateIndicator
                color: 'transparent'
            }
            PropertyChanges {
                target: stateIndicator
                opacity: 1.0
            }
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
            name: 'hovered'
            when: hoverHandle.containsMouse && !gridEntry.activeFocus
            PropertyChanges {
                target: stateIndicator
                color: myPalette.highlight
            }
            PropertyChanges {
                target: stateIndicator
                opacity: 0.2
            }
            PropertyChanges {
                target: hoverLoader
                active: true
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 1.0
            }
        },
        State {
            name: 'selected'
            when: gridEntry.isSelected && !gridEntry.activeFocus
            PropertyChanges {
                target: stateIndicator
                color: myPalette.mid
            }
            PropertyChanges {
                target: stateIndicator
                opacity: 0.6
            }
            PropertyChanges {
                target: hoverLoader
                active: false
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 0.
            }
        },
        State {
            name: 'hoveredOrSelected'
            when: gridEntry.activeFocus
            PropertyChanges {
                target: stateIndicator
                color: myPalette.highlight
            }
            PropertyChanges {
                target: stateIndicator
                opacity: 0.6
            }
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
}
