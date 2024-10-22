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
import QtQuick.Dialogs as Dialogs

import org.kde.kirigami 2.13 as Kirigami
import org.kde.elisa

Kirigami.FormLayout {
    id: form
    property var metadataModel

    property var modelType
    property bool showDeleteButton
    property bool isCreating
    property bool isModifying
    property bool canAddMoreMetadata
    property alias imageItem: imageParent
    property alias showImage: imageParent.visible
    readonly property bool readOnly: metadataModel.isReadOnly || (!isModifying && !isCreating)

    signal close()

    function apply() {
        metadataModel.saveData()
        isCreating = false
        isModifying = false
    }

    function applyAndClose() {
        apply()
        close()
    }

    function cancel() {
        metadataModel.resetData()
        isCreating = false
        isModifying = false
    }

    function cancelAndClose() {
        cancel()
        close()
    }

    function deleteItem() {
        ElisaApplication.musicManager.deleteElementById(modelType, metadataModel.databaseId)
        metadataModel.resetData() // Need this otherwise the closing dialog appears if the data has been modified
        close()
    }

    readonly property Dialogs.MessageDialog dirtyClosingDialog: Dialogs.MessageDialog {
        id: dirtyClosingDialog

        title: i18nc("@title:window", "Warning")
        text: i18nc("@info", 'You have unsaved changes. Do you want to apply the changes or discard them?')
        buttons: Dialogs.MessageDialog.Save | Dialogs.MessageDialog.Discard | Dialogs.MessageDialog.Cancel

        onButtonClicked: (button, role) => {
            switch(button) {
                case Dialogs.MessageDialog.Save: {
                    form.metadataModel.saveData()
                    form.close()
                }
                case Dialogs.MessageDialog.Discard: {
                    form.metadataModel.resetData()
                    form.close()
                }
            }
            close()
        }
    }

    TrackMetadataProxyModel {
        id: proxyModel

        sourceModel: form.metadataModel
        showTagsWithNoData: !form.readOnly
    }

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
        model: proxyModel

        delegate: RowLayout {
            id: delegateRoot

            readonly property string formLabelText: i18nc("Track metadata form label, e.g. 'Artist:'", "%1:", model.name)
            // Make labels bold on mobile read-only mode to help differentiate label from metadata
            readonly property bool singleColumnPlainText: !form.wideMode && !form.isCreating && !form.isModifying
            Kirigami.FormData.label: singleColumnPlainText ? "<b>" + formLabelText + "</b>" : formLabelText

            MediaTrackMetadataDelegate {
                index: model.index
                name: model.name
                display: model.display
                type: model.type
                isRemovable: model.isRemovable
                hasData: model.hasData

                onEdited: model.display = display
                readOnly: form.readOnly || model.isReadOnly

                onAddField: metadataModel.addData(proxyModel.mapRowToSource(index))
                onDeleteField: metadataModel.removeData(proxyModel.mapRowToSource(index))
                Layout.minimumHeight: Kirigami.Units.gridUnit * 1.5
            }
        }
    }
}
