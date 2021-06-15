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
    property bool isCreating: false
    property bool canAddMoreMetadata: false
    property bool showImage
    property bool showTrackFileName
    property bool showDeleteButton: false

    signal rejected()

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    title: isCreating ? i18nc("Window title for track metadata", "Create a Radio") :
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
        id: column
        anchors.fill: parent
        spacing: 0

        ScrollView {
            id: metadataView

            Layout.fillHeight: true
            Layout.fillWidth: true
            leftPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
            rightPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing

            ColumnLayout {
                MediaTrackMetadataForm {
                    Layout.maximumWidth: column.width
                    implicitWidth: metadataView.width - 2 * (Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing)
                    metadataModel: realModel
                    modelType: trackMetadata.modelType
                    showDeleteButton: trackMetadata.showDeleteButton
                    isCreating: trackMetadata.isCreating
                    isModifying: trackMetadata.isModifying
                    canAddMoreMetadata: trackMetadata.canAddMoreMetadata
                    showImage: trackMetadata.showImage
                    showModifyDeleteButtons: false

                    imageItem.anchors.horizontalCenter: horizontalCenter
                    
                    onClose: trackMetadata.close()
                }
            }
        }
        
        Kirigami.Separator { Layout.fillWidth: true }

        // file location
        RowLayout {
            id: fileNameRow
            visible: showTrackFileName
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.leftMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing

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

                visible: showDeleteButton && !isCreating

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
                    onCheckedChanged: isModifying = checked
                    checkable: true
                }

                Button {
                    id: applyButton

                    text: i18n("Apply")
                    icon.name: 'dialog-ok-apply'
                    DialogButtonBox.buttonRole: DialogButtonBox.ApplyRole
                    onClicked:
                    {
                        realModel.saveData()
                        if (isCreating) {
                            isCreating = false
                            isModifying = true
                        }
                    }
                }

                Button {
                    text: i18n("Close")
                    DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
                    onClicked: trackMetadata.close()
                }
            }
        }
    }

    Connections {
        target: ElisaApplication

        function onMusicManagerChanged() {
            if (isCreating) {
                realModel.initializeForNewRadio()
            } else {
                realModel.initializeByUrl(modelType, fileName)
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

                when: editableMetadata && !isModifying && !isCreating

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

                when: editableMetadata && isModifying && !isCreating && (!realModel.isDataValid || !realModel.isDirty)

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
                name: 'readWriteAndDirty'

                when: editableMetadata && isModifying && !isCreating && realModel.isDataValid && realModel.isDirty

                changes: [
                    PropertyChanges {
                        target: modifyButton
                        enabled: true
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

                when: editableMetadata && !isModifying && isCreating && (!realModel.isDataValid || !realModel.isDirty)

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

                when: editableMetadata && !isModifying && isCreating && realModel.isDataValid && realModel.isDirty

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
