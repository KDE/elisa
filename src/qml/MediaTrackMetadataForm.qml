/*
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */


import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQml.Models 2.2
import QtQuick.Layouts 1.2
import ElisaGraphicalEffects 1.15
import org.kde.kirigami 2.13 as Kirigami
import org.kde.elisa 1.0

Kirigami.FormLayout {
    id: form
    property var metadataModel

    property var modelType
    property bool showDeleteButton
    property bool isCreating
    property bool isModifying
    property bool canAddMoreMetadata
    property bool showModifyDeleteButtons
    property alias imageItem: imageParent
    property alias showImage: metadataImage.visible

    signal close()

    Kirigami.InlineMessage {
        id: formInvalidNotification

        text: i18nc("@label", "Data is not valid. %1", metadataModel.errorMessage)
        type: Kirigami.MessageType.Error
        showCloseButton: false
        visible: !metadataModel.isDataValid && metadataModel.isDirty

        Layout.topMargin: Kirigami.Units.largeSpacing
        Layout.fillWidth: true
        Layout.rightMargin: Kirigami.Units.largeSpacing
        Layout.leftMargin: Kirigami.Units.largeSpacing
    }

    Item {
        id: imageParent
        implicitHeight: elisaTheme.coverImageSize
        implicitWidth: elisaTheme.coverImageSize

        ImageWithFallback {
            id: metadataImage

            source: metadataModel.coverUrl
            fallback: Qt.resolvedUrl(elisaTheme.defaultAlbumImage)

            sourceSize.width: elisaTheme.coverImageSize * Screen.devicePixelRatio
            sourceSize.height: elisaTheme.coverImageSize * Screen.devicePixelRatio

            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
        }
    }

    // metadata rows
    Repeater {
        model: metadataModel

        delegate: RowLayout {
            Kirigami.FormData.label: model.name + ":"

            MediaTrackMetadataDelegate {
                index: model.index
                name: model.name
                display: model.display
                type: model.type
                isRemovable: model.isRemovable

                onEdited: model.display = display
                readOnly: (!isModifying && !isCreating) || (metadataModel.isReadOnly || model.isReadOnly)

                onDeleteField: metadataModel.removeData(model.index)
                Layout.minimumHeight: Kirigami.Units.gridUnit * 1.5
            }
        }
    }

    // add tag row
    ComboBox {
        id: selectedField
        Kirigami.FormData.label: i18nc("@label:listbox", "Add new tag:")
        visible: isModifying && !metadataModel.isReadOnly && canAddMoreMetadata

        textRole: "modelData"
        valueRole: "modelData"

        model: metadataModel.extraMetadata
        enabled: metadataModel.extraMetadata.length

        onActivated: metadataModel.addData(selectedField.currentValue)
    }

    RowLayout {
        spacing: Kirigami.Units.smallSpacing
        visible: showModifyDeleteButtons

        Button {
            id: modifyButton

            text: i18nc("@action:button", "Modify")
            icon.name: 'document-edit'
            DialogButtonBox.buttonRole: DialogButtonBox.ActionRole
            onCheckedChanged: isModifying = checked
            checkable: true
        }

        DialogButtonBox {
            id: deleteButtonBox

            Layout.minimumHeight: implicitHeight
            alignment: Qt.AlignLeft

            visible: showDeleteButton && !isCreating

            Button {
                id: deleteButton
                text: i18nc("@action:button", "Delete")
                icon.name: 'delete'
                DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
                onClicked: {
                    ElisaApplication.musicManager.deleteElementById(modelType, metadataModel.databaseId)
                    form.close()
                }
            }
        }
    }
}
