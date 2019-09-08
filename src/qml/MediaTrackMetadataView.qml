/*
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
import QtQml.Models 2.2
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.elisa 1.0

Window {
    id: trackMetadata

    property int initialDatabaseId: 0
    property var modelType
    property string fileName
    property bool editableMetadata
    property alias showImage: metadataImage.visible
    property alias showTrackFileName: fileNameRow.visible
    property alias showDeleteButton: deleteButtonBox.visible
    property alias showApplyButton: applyButton.visible

    signal rejected()

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    title: (initialDatabaseId === -1) ? i18nc("Window title for track metadata", "Create a Radio") : i18nc("Window title for track metadata", "View Details")

    TrackMetadataModel {
        id: realModel

        manager: elisa.musicManager
    }

    modality: Qt.NonModal
    flags: Qt.Dialog | Qt.WindowCloseButtonHint

    color: myPalette.window

    minimumHeight: elisaTheme.coverImageSize * 1.8
    minimumWidth: elisaTheme.coverImageSize * 2.8

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: elisaTheme.layoutVerticalMargin

        spacing: elisaTheme.layoutVerticalMargin

        RowLayout {
            id: metadataView

            Layout.fillHeight: true
            Layout.fillWidth: true

            spacing: 0

            Image {
                id: metadataImage

                source: realModel.coverUrl

                sourceSize.width: elisaTheme.coverImageSize
                sourceSize.height: elisaTheme.coverImageSize

                fillMode: Image.PreserveAspectFit

                Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                Layout.preferredHeight: elisaTheme.coverImageSize
                Layout.preferredWidth: elisaTheme.coverImageSize
                Layout.minimumHeight: elisaTheme.coverImageSize
                Layout.minimumWidth: elisaTheme.coverImageSize
                Layout.maximumHeight: elisaTheme.coverImageSize
                Layout.maximumWidth: elisaTheme.coverImageSize
            }

            ListView {
                id: trackData

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 2 * elisaTheme.layoutHorizontalMargin

                focus: true

                ScrollBar.vertical: ScrollBar {
                    id: scrollBar
                }
                boundsBehavior: Flickable.StopAtBounds
                clip: true

                ScrollHelper {
                    id: scrollHelper
                    flickable: trackData
                    anchors.fill: trackData
                }
                model: realModel

                Component {
                    id: metaDataDelegate

                    MetaDataDelegate {
                        width: scrollBar.visible ? (!LayoutMirroring.enabled ? trackData.width - scrollBar.width : trackData.width) : trackData.width
                    }
                }

                Component {
                    id: editableMetaDataDelegate

                    EditableMetaDataDelegate {
                        width: scrollBar.visible ? (!LayoutMirroring.enabled ? trackData.width - scrollBar.width : trackData.width) : trackData.width
                        onRadioEdited: applyButton.enabled = true
                    }
                }

                delegate: editableMetadata ? editableMetaDataDelegate: metaDataDelegate
            }
        }

        RowLayout {
            id: fileNameRow

            Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
            Layout.topMargin: elisaTheme.layoutVerticalMargin
            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

            spacing: elisaTheme.layoutHorizontalMargin

            Image {
                Layout.preferredWidth: fileNameLabel.height
                Layout.preferredHeight: fileNameLabel.height

                sourceSize.width: fileNameLabel.height
                sourceSize.height: fileNameLabel.height

                source: elisaTheme.folderIcon
            }

            LabelWithToolTip {
                id: fileNameLabel

                Layout.fillWidth: true

                text: realModel.fileUrl

                elide: Text.ElideRight
            }
        }

        RowLayout {
            spacing: elisaTheme.layoutVerticalMargin

            DialogButtonBox {
                id: deleteButtonBox

                Layout.minimumHeight: implicitHeight
                alignment: Qt.AlignLeft

                Button {
                    id: deleteButton
                    text: qsTr("Delete")
                    DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
                    onClicked:
                    {
                        elisa.musicManager.deleteElementById(modelType, realModel.databaseId)
                        trackMetadata.close()
                    }
                }
            }

            DialogButtonBox {
                id: buttons

                Layout.fillWidth: true
                Layout.minimumHeight: implicitHeight
                alignment: Qt.AlignRight

                Button {
                    id: applyButton

                    text: qsTr("Apply")
                    DialogButtonBox.buttonRole: DialogButtonBox.ApplyRole
                    onClicked:
                    {
                        realModel.saveData()
                        enabled = false
                        if (!deleteButtonBox.visible && editableMetadata) {
                            deleteButtonBox.visible = true
                        }
                    }
                    enabled: false
                }
                Button {
                    text: qsTr("Close")
                    DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
                    onClicked: trackMetadata.close()
                }
            }
        }
    }

    Connections {
        target: elisa

        onMusicManagerChanged: {
            if (initialDatabaseId === -1) {
                realModel.initializeForNewRadio()
            } else if (initialDatabaseId !== 0) {
                realModel.initializeById(modelType, initialDatabaseId)
            } else {
                realModel.initializeByTrackFileName(fileName)
            }
        }
    }

    Component.onCompleted: {
        if (elisa.musicManager) {
            if (initialDatabaseId === -1) {
                realModel.initializeForNewRadio()
            } else if (initialDatabaseId !== 0) {
                realModel.initializeById(modelType, initialDatabaseId)
            } else {
                realModel.initializeByTrackFileName(fileName)
            }
        }
    }
}
