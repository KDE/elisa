/*
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.elisa

import ".."

Kirigami.ScrollablePage {
    id: trackMetadata

    property url fileName
    property bool editableMetadata
    property bool showTrackFileName

    property alias modelType: metadataForm.modelType
    property alias isModifying: metadataForm.isModifying
    property alias isCreating: metadataForm.isCreating
    property alias canAddMoreMetadata: metadataForm.canAddMoreMetadata
    property alias showImage: metadataForm.showImage
    property alias showDeleteButton: metadataForm.showDeleteButton

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.Titles

    signal rejected()

    onBackRequested: event => {
        if (realModel.isDirty) {
            metadataForm.dirtyClosingDialog.open()
            event.accepted = true
        }
    }

    LayoutMirroring.enabled: Application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    title: isCreating ? i18nc("@title:window", "Create a Radio") :
                        i18nc("@title:window", "View details")

    actions: [
        Kirigami.Action {
            icon.name: 'document-edit'
            text: i18nc("@action:button", "Modify")
            onTriggered: metadataForm.isModifying = true
            visible: !metadataForm.isModifying && !metadataForm.isCreating
        },
        Kirigami.Action {
            icon.name: "dialog-ok-apply"
            text: i18nc("@action:button", "Save")
            onTriggered: metadataForm.isCreating ? metadataForm.applyAndClose() : metadataForm.apply()
            visible: metadataForm.isModifying || metadataForm.isCreating
        },
        Kirigami.Action {
            icon.name: 'dialog-cancel'
            text: i18nc("@action:button", "Cancel")
            onTriggered: metadataForm.isCreating ? metadataForm.cancelAndClose() : metadataForm.cancel()
            visible: metadataForm.isModifying || metadataForm.isCreating
        },
        Kirigami.Action {
            icon.name: 'delete'
            text: i18nc("@action:button", "Delete")
            onTriggered: metadataForm.deleteItem()
            visible: metadataForm.showDeleteButton && !metadataForm.isCreating
        }
    ]

    Component.onCompleted: {
        if (ElisaApplication.musicManager) {
            if (isCreating) {
                realModel.initializeForNewRadio()
            } else {
                realModel.initializeByUrl(modelType, fileName)
            }
        }
    }

    ColumnLayout {
        Layout.minimumWidth: parent.width
        Layout.maximumWidth: parent.width

        Connections {
            target: ElisaApplication

            function onMusicManagerChanged() {
                if (trackMetadata.isCreating) {
                    realModel.initializeForNewRadio()
                } else {
                    realModel.initializeByUrl(trackMetadata.modelType, trackMetadata.fileName)
                }
            }
        }

        EditableTrackMetadataModel {
            id: realModel
            manager: ElisaApplication.musicManager
        }

        // file location
        RowLayout {
            id: fileNameRow
            visible: trackMetadata.showTrackFileName

            spacing: Kirigami.Units.largeSpacing

            Kirigami.Icon {
                Layout.preferredWidth: fileNameLabel.height
                Layout.preferredHeight: fileNameLabel.height

                source: Theme.folderIcon
            }

            LabelWithToolTip {
                id: fileNameLabel

                Layout.fillWidth: true
                text: realModel.fileUrl
                wrapMode: Text.Wrap
                elide: Text.ElideRight
            }
        }

        MediaTrackMetadataForm {
            id: metadataForm

            Layout.fillWidth: true

            metadataModel: realModel
            onClose: mainWindow.pageStack.layers.pop()
        }
    }
}
