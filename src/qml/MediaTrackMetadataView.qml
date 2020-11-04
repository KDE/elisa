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
    property bool isModifying: false
    property bool isCreation: false
    property alias showImage: metadataImage.visible
    property alias showTrackFileName: fileNameRow.visible
    property bool showDeleteButton: false

    signal rejected()

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    title: isCreation ? i18nc("Window title for track metadata", "Create a Radio") :
                        i18nc("Window title for track metadata", "View Details")

    EditableTrackMetadataModel {
        id: realModel

        manager: ElisaApplication.musicManager
    }

    modality: Qt.NonModal
    flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint
        | Qt.WindowCloseButtonHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint

    color: myPalette.window

    minimumHeight: elisaTheme.metaDataDialogHeight
    minimumWidth: elisaTheme.metaDataDialogWidth

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

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 2 * Kirigami.Units.largeSpacing

                ListView {
                    id: trackData

                    clip: true
                    focus: true

                    model: realModel

                    Component {
                        id: metaDataDelegate

                        MetaDataDelegate {
                            width: scrollBar.visible ? (!LayoutMirroring.enabled ? trackData.width - scrollBar.width : trackData.width) : trackData.width

                            index: model.index
                            name: model.name
                            display: model.display
                            type: model.type
                        }
                    }

                    Component {
                        id: editableMetaDataDelegate

                        EditableMetaDataDelegate {
                            width: scrollBar.visible ? (!LayoutMirroring.enabled ? trackData.width - scrollBar.width : trackData.width) : trackData.width

                            index: model.index
                            name: model.name
                            display: model.display
                            type: model.type
                            isRemovable: model.isRemovable

                            onEdited: model.display = display

                            onDeleteField: realModel.removeData(model.index)
                        }
                    }

                    delegate: ((dialogStates.state === 'readWrite' || dialogStates.state === 'readWriteAndDirty' ||
                                dialogStates.state === 'create' || dialogStates.state === 'createAndDirty') && !realModel.isReadOnly) ? editableMetaDataDelegate: metaDataDelegate
                }
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

                visible: showDeleteButton && !isCreation

                Button {
                    id: deleteButton
                    text: i18n("Delete")
                    icon.name: 'delete'
                    DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
                    onClicked:
                    {
                        ElisaApplication.musicManager.deleteElementById(modelType, realModel.databaseId)
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
                    id: modifyButton

                    text: i18n("Modify")
                    icon.name: 'document-edit'
                    DialogButtonBox.buttonRole: DialogButtonBox.ActionRole
                    onClicked: isModifying = true
                }

                Button {
                    id: applyButton

                    text: i18n("Apply")
                    icon.name: 'dialog-ok-apply'
                    DialogButtonBox.buttonRole: DialogButtonBox.ApplyRole
                    onClicked:
                    {
                        realModel.saveData()
                        if (isCreation) {
                            isCreation = false
                            isModifying = true
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
        target: ElisaApplication

        function onMusicManagerChanged() {
            if (isCreation) {
                realModel.initializeForNewRadio()
            } else {
                realModel.initializeByUrl(modelType, fileName)
            }
        }
    }

    Component.onCompleted: {
        if (ElisaApplication.musicManager) {
            if (isCreation) {
                realModel.initializeForNewRadio()
            } else {
                realModel.initializeByUrl(modelType, fileName)
            }
        }
    }

    StateGroup {
        id: dialogStates

        states: [
            State {
                name: 'consultOnly'

                when: !editableMetadata

                changes: [
                    PropertyChanges {
                        target: modifyButton
                        enabled: false
                        visible: false
                    },
                    PropertyChanges {
                        target: applyButton
                        enabled: false
                        visible: false
                    }
                ]
            },
            State {
                name: 'readOnly'

                when: editableMetadata && !isModifying && !isCreation

                changes: [
                    PropertyChanges {
                        target: modifyButton
                        enabled: true
                        visible: true
                    },
                    PropertyChanges {
                        target: applyButton
                        enabled: false
                        visible: true
                    }
                ]
            },
            State {
                name: 'readWrite'

                when: editableMetadata && isModifying && !isCreation && (!realModel.isDataValid || !realModel.isDirty)

                changes: [
                    PropertyChanges {
                        target: modifyButton
                        enabled: false
                        visible: true
                    },
                    PropertyChanges {
                        target: applyButton
                        enabled: false
                        visible: true
                    }
                ]
            },
            State {
                name: 'readWriteAndDirty'

                when: editableMetadata && isModifying && !isCreation && realModel.isDataValid && realModel.isDirty

                changes: [
                    PropertyChanges {
                        target: modifyButton
                        enabled: false
                        visible: true
                    },
                    PropertyChanges {
                        target: applyButton
                        enabled: true
                        visible: true
                    }
                ]
            },
            State {
                name: 'create'

                when: editableMetadata && !isModifying && isCreation && (!realModel.isDataValid || !realModel.isDirty)

                changes: [
                    PropertyChanges {
                        target: modifyButton
                        enabled: false
                        visible: true
                    },
                    PropertyChanges {
                        target: applyButton
                        enabled: false
                        visible: true
                    }
                ]
            },
            State {
                name: 'createAndDirty'

                when: editableMetadata && !isModifying && isCreation && realModel.isDataValid && realModel.isDirty

                changes: [
                    PropertyChanges {
                        target: modifyButton
                        enabled: false
                        visible: true
                    },
                    PropertyChanges {
                        target: applyButton
                        enabled: true
                        visible: true
                    }
                ]
            }
        ]
    }
}
