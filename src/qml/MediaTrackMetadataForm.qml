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
import QtQuick.Dialogs as Dialogs

import org.kde.kirigami as Kirigami
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
                    break
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

        text: i18nc("@label", "Data is not valid. %1", form.metadataModel.errorMessage)
        type: Kirigami.MessageType.Error
        showCloseButton: false
        visible: !form.metadataModel.isDataValid && form.metadataModel.isDirty

        Layout.topMargin: Kirigami.Units.largeSpacing
        Layout.fillWidth: true
        Layout.rightMargin: Kirigami.Units.largeSpacing
        Layout.leftMargin: Kirigami.Units.largeSpacing
    }

    Item {
        id: imageParent
        implicitHeight: Theme.coverImageSize
        implicitWidth: Theme.coverImageSize

        ImageWithFallback {
            id: metadataImage

            source: form.metadataModel.coverUrl
            fallback: Qt.resolvedUrl(Theme.defaultAlbumImage)

            sourceSize.width: Theme.coverImageSize * Screen.devicePixelRatio
            sourceSize.height: Theme.coverImageSize * Screen.devicePixelRatio

            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
        }
    }

    // metadata rows
    Repeater {
        model: proxyModel

        delegate: RowLayout {
            id: delegateRoot

            // NOTE: Modifying a required property does not call setData on the
            // model itself and instead breaks the property binding. Modifying
            // subproperties of the `model` role will result in setData being
            // called. Hence why we use `model.display` instead of `display` here.
            // TODO: remove workaround when we can depend on Qt 6.10 (QTBUG-132420)
            required property var model

            required property int index
            // required property var display
            required property string name
            required property int type
            required property bool hasData
            required property bool isReadOnly
            required property bool isRemovable

            readonly property string formLabelText: i18nc("Track metadata form label, e.g. 'Artist:'", "%1:", name)
            // Make labels bold on mobile read-only mode to help differentiate label from metadata
            readonly property bool singleColumnPlainText: !form.wideMode && !form.isCreating && !form.isModifying
            Kirigami.FormData.label: singleColumnPlainText ? "<b>" + formLabelText + "</b>" : formLabelText
            Kirigami.FormData.labelAlignment: {
                if (singleColumnPlainText) {
                    return Text.AlignBottom;
                } else if (type === EditableTrackMetadataModel.LongTextEntry) {
                    return Text.AlignTop;
                } else {
                    return Text.AlignVCenter;
                }
            }

            MediaTrackMetadataDelegate {
                index: delegateRoot.index
                name: delegateRoot.name
                display: delegateRoot.model.display
                type: delegateRoot.type
                isRemovable: delegateRoot.isRemovable
                hasData: delegateRoot.hasData

                onEdited: delegateRoot.model.display = display
                readOnly: form.readOnly || delegateRoot.isReadOnly

                onAddField: form.metadataModel.addData(proxyModel.mapRowToSource(index))
                onDeleteField: form.metadataModel.removeData(proxyModel.mapRowToSource(index))
                Layout.minimumHeight: Kirigami.Units.gridUnit * 1.5
            }
        }
    }
}
