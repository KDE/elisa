/*
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.elisa

Window {
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

    signal rejected()

    LayoutMirroring.enabled: Application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    title: isCreating ? KI18n.i18nc("@title:window", "Create a Radio") :
                        KI18n.i18nc("@title:window", "View details")

    EditableTrackMetadataModel {
        id: realModel

        manager: ElisaApplication.musicManager
    }

    modality: Qt.NonModal
    flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint
           | Qt.WindowCloseButtonHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint

    color: palette.window

    minimumHeight: Theme.metaDataDialogHeight
    minimumWidth: Theme.metaDataDialogWidth

    // Close when pressing Esc key
    Shortcut {
        sequence: StandardKey.Cancel
        onActivated: trackMetadata.close()
    }

    onClosing: close => {
        if (realModel.isDirty) {
            metadataForm.dirtyClosingDialog.open()
            close.accepted = false
        }
    }

    Kirigami.ScrollablePage {
        anchors.fill: parent

        MediaTrackMetadataForm {
            id: metadataForm

            metadataModel: realModel
            imageItem.anchors.horizontalCenter: horizontalCenter

            onClose: trackMetadata.close()
        }

        footer: ColumnLayout {
            anchors.left: parent.left
            anchors.right: parent.right

            Kirigami.Separator { Layout.fillWidth: true }

            // file location
            RowLayout {
                id: fileNameRow
                visible: trackMetadata.showTrackFileName
                Layout.topMargin: Kirigami.Units.largeSpacing
                Layout.leftMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    readonly property int size: Kirigami.Units.iconSizes.roundedIconSize(fileNameLabel.height)

                    implicitWidth: size
                    implicitHeight: size

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

            RowLayout {
                Layout.topMargin: Kirigami.Units.largeSpacing
                Layout.leftMargin: Kirigami.Units.smallSpacing
                Layout.rightMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                spacing: Kirigami.Units.smallSpacing

                DialogButtonBox {
                    id: deleteButtonBox

                    Layout.minimumHeight: implicitHeight
                    alignment: Qt.AlignLeft

                    visible: trackMetadata.showDeleteButton && !trackMetadata.isCreating

                    Button {
                        text: KI18n.i18nc("@action:button", "Delete")
                        icon.name: 'delete'
                        DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
                        onClicked: metadataForm.deleteItem()
                    }
                }

                DialogButtonBox {
                    Layout.fillWidth: true
                    Layout.minimumHeight: implicitHeight
                    alignment: Qt.AlignRight

                    visible: !trackMetadata.isModifying && !trackMetadata.isCreating
                    standardButtons: DialogButtonBox.Ok

                    onAccepted: metadataForm.close()

                    Button {
                        text: KI18n.i18nc("@action:button", "Modify")
                        icon.name: 'document-edit'
                        DialogButtonBox.buttonRole: DialogButtonBox.ActionRole
                        onClicked: metadataForm.isModifying = true
                    }
                }

                DialogButtonBox {
                    Layout.fillWidth: true
                    Layout.minimumHeight: implicitHeight
                    alignment: Qt.AlignRight

                    visible: metadataForm.isModifying
                    standardButtons: DialogButtonBox.Apply | DialogButtonBox.Ok | DialogButtonBox.Cancel

                    onApplied: metadataForm.apply()
                    onAccepted: metadataForm.applyAndClose()
                    onRejected: metadataForm.cancel()
                }

                DialogButtonBox {
                    Layout.fillWidth: true
                    Layout.minimumHeight: implicitHeight
                    alignment: Qt.AlignRight

                    visible: metadataForm.isCreating
                    standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel

                    onAccepted: metadataForm.applyAndClose()
                    onRejected: metadataForm.cancelAndClose()
                }
            }
        }
    }

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

    Component.onCompleted: {
        if (ElisaApplication.musicManager) {
            if (isCreating) {
                realModel.initializeForNewRadio()
            } else {
                realModel.initializeByUrl(modelType, fileName)
            }
        }
    }
}
