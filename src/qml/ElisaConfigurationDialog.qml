/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.11
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.4 as QQC2
import QtQuick.Dialogs 1.3 as Dialogs

import org.kde.kirigami 2.14 as Kirigami

import org.kde.elisa 1.0

Window {
    id: dialog

    title: i18n("Configure")

    visible: true
    modality: Qt.ApplicationModal

    minimumWidth: Kirigami.Units.gridUnit * 36
    minimumHeight: Kirigami.Units.gridUnit * 32

    // Close when pressing Esc key
    Shortcut {
        sequence: StandardKey.Cancel
        onActivated: close()
    }

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    // Draw standard window backround
    Rectangle {
        anchors.fill: parent

        color: myPalette.window

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Kirigami.Units.smallSpacing


            // General settings
            // ================
            Kirigami.FormLayout {

                QQC2.CheckBox {
                    Kirigami.FormData.label: i18n("General:")

                    text: i18n("Show progress on Task Manager entries")

                    checked: ElisaConfigurationDialog.showProgressInTaskBar
                    onToggled: ElisaConfigurationDialog.showProgressInTaskBar = checked
                    Accessible.onToggleAction: onToggled
                    Accessible.onPressAction: onToggled
                }

                QQC2.CheckBox {
                    text: i18n("Keep running in System Tray when main window is closed")

                    checked: ElisaConfigurationDialog.showSystemTrayIcon
                    onToggled: ElisaConfigurationDialog.showSystemTrayIcon = checked
                    Accessible.onToggleAction: onToggled
                    Accessible.onPressAction: onToggled
                }

                QQC2.CheckBox {
                    text: i18n("Start playing on startup")

                    checked: ElisaConfigurationDialog.playAtStartup
                    onToggled: ElisaConfigurationDialog.playAtStartup = checked
                    Accessible.onToggleAction: onToggled
                    Accessible.onPressAction: onToggled
                }


                Item {
                    Kirigami.FormData.isSection: true
                }


                QQC2.ComboBox {
                    id: embeddedCategoryCombo

                    Kirigami.FormData.label: i18n("Embed category in sidebar:")

                    model: [i18nc("Configure dialog, embed no category in views navigation list", "Nothing"),
                        i18nc("Configure dialog, embed all albums in views navigation list", "Albums"),
                        i18nc("Configure dialog, embed all artists in views navigation list", "Artists"),
                        i18nc("Configure dialog, embed all genres in views navigation list", "Genres")]

                    editable: false
                    currentIndex: (ElisaConfigurationDialog.embeddedView === ElisaUtils.Genre ? 3 : (ElisaConfigurationDialog.embeddedView === ElisaUtils.Album ? 1 : (ElisaConfigurationDialog.embeddedView === ElisaUtils.Artist ? 2 : 0)))

                    onActivated: {
                        ElisaConfigurationDialog.embeddedView = (currentIndex === 0 ? ElisaUtils.Unknown : (currentIndex === 1 ? ElisaUtils.Album : (currentIndex === 2 ? ElisaUtils.Artist : ElisaUtils.Genre)))
                    }

                    Connections {
                        target: ElisaConfigurationDialog

                        onEmbeddedViewChanged:
                        {
                            if (ElisaConfigurationDialog.embeddedView == ElisaUtils.Unknown) {
                                embeddedCategoryCombo.currentIndex = 0
                            } else if (ElisaConfigurationDialog.embeddedView == ElisaUtils.Album) {
                                embeddedCategoryCombo.currentIndex = 1
                            } else if (ElisaConfigurationDialog.embeddedView == ElisaUtils.Artist) {
                                embeddedCategoryCombo.currentIndex = 2
                            } else if (ElisaConfigurationDialog.embeddedView == ElisaUtils.Genre) {
                                embeddedCategoryCombo.currentIndex = 3
                            }
                        }
                    }
                }


                Item {
                    Kirigami.FormData.isSection: true
                }


                QQC2.ComboBox {
                    id: initialViewCombo

                    Kirigami.FormData.label: i18n("Initial view on startup:")

                    model: [i18nc("Title of the view of the playlist", "Now Playing"),
                        i18nc("Title of the view of recently played tracks", "Recently Played"),
                        i18nc("Title of the view of frequently played tracks", "Frequently Played"),
                        i18nc("Title of the view of all albums", "Albums"),
                        i18nc("Title of the view of all artists", "Artists"),
                        i18nc("Title of the view of all tracks", "Tracks"),
                        i18nc("Title of the view of all genres", "Genres"),
                        i18nc("Title of the file browser view", "Files"),
                        i18nc("Title of the file radios browser view", "Radios"),
                    ]

                    editable: false
                    currentIndex: ElisaConfigurationDialog.initialViewIndex

                    onActivated: {
                        ElisaConfigurationDialog.initialViewIndex = currentIndex
                    }

                    Connections {
                        target: ElisaConfigurationDialog

                        onInitialViewIndexChanged: initialViewCombo.currentIndex = ElisaConfigurationDialog.initialViewIndex
                    }
                }


                Item {
                    Kirigami.FormData.isSection: true
                }


                // Indexing settings
                // =================
                QQC2.ComboBox {
                    id: indexingTypeCombo
                    Kirigami.FormData.label: i18n("Music indexing:")

                    // Work around https://bugs.kde.org/show_bug.cgi?id=403153
                    implicitWidth: Kirigami.Units.gridUnit * 12

                    editable: false

                    model: [i18nc("Configure dialog, indexing type", "Use fast native indexer"),
                            i18nc("Configure dialog, indexing type", "Scan the filesystem directly")]

                    currentIndex: ElisaConfigurationDialog.forceUsageOfSlowFileSystemIndexing ? 1 : 0
                    onActivated: {
                        ElisaConfigurationDialog.forceUsageOfSlowFileSystemIndexing = currentIndex === 0 ? false : true
                    }
                }
            }

            Kirigami.InlineMessage {
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.largeSpacing * 5
                Layout.rightMargin: Kirigami.Units.largeSpacing * 5

                visible: indexingTypeCombo.currentIndex === 1

                type: Kirigami.MessageType.Warning
                text: xi18n("This is slower than the fast indexer. Please activate it only if Elisa cannot find your music and searching for one of the missing music files using your file manager also does not work. Please report this as a bug.")

                actions: [
                    Kirigami.Action {
                        text: i18n("Report Bug")
                        iconName: "tools-report-bug"
                        onTriggered: Qt.openUrlExternally("https://bugs.kde.org/enter_bug.cgi?product=frameworks-baloo")
                    }
                ]
            }


            // Music locations list
            // ====================
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: Kirigami.Units.largeSpacing * 5
                Layout.rightMargin: Kirigami.Units.largeSpacing * 5

                Kirigami.Heading {
                    Layout.fillWidth: true
                    text: i18nc("The configured folders where the user's music collection can be found", "Music folders:")
                    level: 4
                }

                QQC2.ScrollView {
                    id: scrollview

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    // Show the border
                    Component.onCompleted: {
                        scrollview.background.visible = true;
                    }

                    ListView {
                        id:pathList

                        clip: true

                        model: ElisaConfigurationDialog.rootPath

                        delegate: Kirigami.SwipeListItem {
                            // Don't need a highlight effect on hover
                            hoverEnabled: false

                            QQC2.Label {
                                text: modelData
                            }

                            actions: Kirigami.Action {
                                iconName: "edit-delete"
                                text: i18n("Stop looking for music here")

                                visible: pathList.count > 1
                                onTriggered: ElisaConfigurationDialog.removeMusicLocation(modelData)
                            }
                        }
                    }
                }


                QQC2.Button {
                    text: i18n("Add New Location")
                    icon.name: "list-add"

                    Layout.alignment: Qt.AlignTop | Qt.AlignLeft

                    onClicked: fileDialog.open()
                    Accessible.onPressAction: onClicked

                    Dialogs.FileDialog {
                        id: fileDialog
                        title: i18n("Choose a Folder")
                        folder: shortcuts.home
                        selectFolder: true

                        visible: false

                        onAccepted: {
                            var oldPaths = ElisaConfigurationDialog.rootPath
                            oldPaths.push(fileDialog.fileUrls)
                            ElisaConfigurationDialog.rootPath = oldPaths
                        }
                    }
                }
            }

            QQC2.DialogButtonBox {
                Layout.fillWidth: true

                QQC2.Button {
                    text: i18n("OK")
                    icon.name: 'dialog-ok-apply'
                    QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.AcceptRole
                    Accessible.onPressAction: onClicked
                }
                onAccepted: {
                    ElisaConfigurationDialog.save()
                    close()
                }

                QQC2.Button {
                    text: i18n("Apply")
                    icon.name: 'dialog-ok-apply'
                    QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.ApplyRole
                    Accessible.onPressAction: onClicked

                    enabled: ElisaConfigurationDialog.isDirty
                }
                onApplied: ElisaConfigurationDialog.save()

                QQC2.Button {
                    text: i18n("Cancel")
                    icon.name: 'dialog-cancel'
                    QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.RejectRole
                    Accessible.onPressAction: onClicked
                }
                onRejected: {
                    ElisaConfigurationDialog.cancel()
                    close()
                }
            }
        }
    }

    Dialogs.MessageDialog {
        id: dirtyClosingDialog

        standardButtons: Dialogs.StandardButton.Save | Dialogs.StandardButton.Discard | Dialogs.StandardButton.Cancel

        title: i18n("Warning")

        icon: Dialogs.StandardIcon.Warning
        text: i18n('You have unsaved changes. Do you want to apply the changes or discard them?')

        onDiscard: {
            ElisaConfigurationDialog.cancel()
            dialog.close()
        }

        onAccepted: {
            ElisaConfigurationDialog.save()
            dialog.close()
        }
    }

    onClosing: {
        if (ElisaConfigurationDialog.isDirty) {
            close.accepted = false
            dirtyClosingDialog.open()
        }
    }
}
