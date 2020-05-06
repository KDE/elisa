/*
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQml.Models 2.2
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

Window {
    id: trackMetadata

    property var modelType
    property url fileName
    property bool editableMetadata
    property bool isCreation: false
    property alias showImage: metadataImage.visible
    property alias showTrackFileName: fileNameRow.visible
    property alias showDeleteButton: deleteButtonBox.visible
    property alias showApplyButton: applyButton.visible
    property double widthIndex: 2.8

    signal rejected()

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    title: isCreation ? i18nc("Window title for track metadata", "Create a Radio") :
                        i18nc("Window title for track metadata", "View Details")

    EditableTrackMetadataModel {
        id: realModel

        manager: elisa.musicManager
    }

    modality: Qt.NonModal
    flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint
        | Qt.WindowCloseButtonHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint

    color: myPalette.window

    minimumHeight: elisaTheme.coverImageSize * 1.8
    minimumWidth: elisaTheme.coverImageSize * trackMetadata.widthIndex

    onClosing: {
        trackMetadata.rejected()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Kirigami.Units.smallSpacing

        spacing: Kirigami.Units.smallSpacing

        RowLayout {
            id: metadataView

            Layout.fillHeight: true
            Layout.fillWidth: true

            spacing: 0

            ImageWithFallback {
                id: metadataImage

                source: realModel.coverUrl
                fallback: elisaTheme.defaultAlbumImage

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
                Layout.leftMargin: 2 * Kirigami.Units.largeSpacing

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
                    }
                }

                delegate: editableMetadata ? editableMetaDataDelegate: metaDataDelegate
            }
        }

        RowLayout {
            id: fileNameRow

            Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.bottomMargin: Kirigami.Units.smallSpacing

            spacing: Kirigami.Units.largeSpacing

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

        Kirigami.InlineMessage {
            id: formInvalidNotification

            text: i18nc("Form validation error message for track data", "Data are not valid. %1", realModel.errorMessage)
            type: Kirigami.MessageType.Error
            showCloseButton: false
            visible: !realModel.isDataValid && realModel.isDirty

            Layout.topMargin: 5
            Layout.fillWidth: true
            Layout.rightMargin: Kirigami.Units.largeSpacing
            Layout.leftMargin: Kirigami.Units.largeSpacing
        }

        RowLayout {
            spacing: Kirigami.Units.smallSpacing

            DialogButtonBox {
                id: deleteButtonBox

                Layout.minimumHeight: implicitHeight
                alignment: Qt.AlignLeft

                Button {
                    id: deleteButton
                    text: i18n("Delete")
                    icon.name: 'delete'
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

                    enabled: realModel.isDataValid && realModel.isDirty

                    text: i18n("Apply")
                    icon.name: 'dialog-ok-apply'
                    DialogButtonBox.buttonRole: DialogButtonBox.ApplyRole
                    onClicked:
                    {
                        realModel.saveData()
                        if (!deleteButtonBox.visible && editableMetadata) {
                            deleteButtonBox.visible = true
                        }
                    }
                }
                Button {
                    text: i18n("Close")
                    icon.name: 'dialog-cancel'
                    DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
                    onClicked: trackMetadata.close()
                }
            }
        }
    }

    Connections {
        target: elisa

        onMusicManagerChanged: {
            if (isCreation) {
                realModel.initializeForNewRadio()
            } else {
                realModel.initializeByUrl(modelType, fileName)
            }
        }
    }

    Component.onCompleted: {
        if (elisa.musicManager) {
            if (isCreation) {
                realModel.initializeForNewRadio()
            } else {
                realModel.initializeByUrl(modelType, fileName)
            }
        }
    }
}
