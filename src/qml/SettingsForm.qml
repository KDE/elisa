/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Nate Graham <nate@kde.org>
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtCore
import QtQuick 2.11
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.4 as QQC2
import QtQuick.Dialogs as Dialogs

import org.kde.kirigami 2.14 as Kirigami

import org.kde.elisa 1.0

import ".."

ColumnLayout {
    signal closeForm()

    function saveAndCloseForm() {
        ElisaConfigurationDialog.save()
        closeForm()
    }

    function discardAndCloseForm() {
        ElisaConfigurationDialog.cancel()
        closeForm()
    }

    function applyChanges() {
        ElisaConfigurationDialog.save()
    }

    // General settings
    // ================
    Kirigami.FormLayout {
        Layout.fillWidth: true

        QQC2.CheckBox {
            Kirigami.FormData.label: i18nc("@title:group", "General:")

            Layout.fillWidth: true

            text: i18nc("@option:check", "Show background on Now Playing page")

            checked: ElisaConfigurationDialog.showNowPlayingBackground
            onToggled: ElisaConfigurationDialog.showNowPlayingBackground = checked
            Accessible.onToggleAction: onToggled
            Accessible.onPressAction: onToggled
        }

        QQC2.CheckBox {
            id: progressCheckBox

            Layout.fillWidth: true

            text: i18nc("@option:check", "Show progress on Task Manager entries")

            checked: ElisaConfigurationDialog.showProgressInTaskBar
            onToggled: ElisaConfigurationDialog.showProgressInTaskBar = checked
            Accessible.onToggleAction: onToggled
            Accessible.onPressAction: onToggled
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18nc("@option:check", "Keep running in System Tray when main window is closed")

            checked: ElisaConfigurationDialog.showSystemTrayIcon
            onToggled: ElisaConfigurationDialog.showSystemTrayIcon = checked
            Accessible.onToggleAction: onToggled
            Accessible.onPressAction: onToggled
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18nc("@option:check", "Start playing on startup")

            checked: ElisaConfigurationDialog.playAtStartup
            onToggled: ElisaConfigurationDialog.playAtStartup = checked
            Accessible.onToggleAction: onToggled
            Accessible.onPressAction: onToggled
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18nc("@option:check", "Scan for New Music on startup")

            checked: ElisaConfigurationDialog.scanAtStartup
            onToggled: {
                startupScanWarningMessage.visible = !checked
                ElisaConfigurationDialog.scanAtStartup = checked
            }
            Accessible.onToggleAction: onToggled
            Accessible.onPressAction: onToggled
        }

        Kirigami.InlineMessage {
            id: startupScanWarningMessage
            Layout.fillWidth: true

            // Not visible by default, the message only becomes visible when "Scan for New Music on startup" checkbox is unchecked. See onToggled implementation of the checkbox.
            visible: false

            type: Kirigami.MessageType.Warning
            text: xi18nc("@info", "When using this setting, you will need to manually refresh the music collection whenever new files are added to configured music folders. You can do this with the <interface>Scan for new music</interface> item in Elisa's hamburger menu.")
        }


        Item {
            Kirigami.FormData.isSection: true
        }

        QQC2.ButtonGroup { id: ratingStyleGroup }

        QQC2.RadioButton {
            Kirigami.FormData.label: i18nc("@title:group", "Song rating style:")

            Layout.fillWidth: true

            text: i18nc("@option:radio", "0-5 stars")

            QQC2.ButtonGroup.group: ratingStyleGroup

            checked: !ElisaConfigurationDialog.useFavoriteStyleRatings
            onToggled: ElisaConfigurationDialog.useFavoriteStyleRatings = !checked
            Accessible.onToggleAction: onToggled
            Accessible.onPressAction: onToggled
        }
        QQC2.RadioButton {
            Layout.fillWidth: true

            text: i18nc("@option:radio", "Favorite/not favorite")

            QQC2.ButtonGroup.group: ratingStyleGroup

            checked: ElisaConfigurationDialog.useFavoriteStyleRatings
            onToggled: ElisaConfigurationDialog.useFavoriteStyleRatings = checked

            Accessible.onToggleAction: onToggled
            Accessible.onPressAction: onToggled
        }

        Item {
            Kirigami.FormData.isSection: true
            visible: Kirigami.Settings.isMobile
        }

        // select colour scheme (mobile only, since desktop has it in the application menu)
        QQC2.ComboBox {
            Kirigami.FormData.label: i18nc("@label:listbox", "Color Scheme:")

            visible: Kirigami.Settings.isMobile

            model: ElisaApplication.colorSchemesModel
            textRole: "display"

            delegate: QQC2.ItemDelegate {
                width: parent.width

                required property var model

                icon.name: "image://colorScheme/" + model.display
                text: model.display
                checked: model.display === ElisaConfigurationDialog.colorScheme
                onClicked: {
                    ElisaApplication.activateColorScheme(model.display)
                    ElisaConfigurationDialog.setColorScheme(model.display)
                    ElisaConfigurationDialog.save()
                }
            }
        }

        Item {
            Kirigami.FormData.isSection: true
            visible: Kirigami.Settings.isMobile
        }

        // scan for new music (mobile only, since on desktop it is in the application menu)
        QQC2.Button {
            visible: Kirigami.Settings.isMobile
            text: i18nc("@action:button", "Scan for New Music")
            icon.name: "view-refresh"
            onClicked: {
                ElisaApplication.musicManager.scanCollection(MusicListenersManager.Soft)
                showPassiveNotification(i18nc("@info", "Started scanning for music"))
            }
        }

        QQC2.Button {
            visible: Kirigami.Settings.isMobile
            text: i18nc("@action:button", "Reset Database and Re-Scan Everything")
            icon.name: "edit-clear-all"
            onClicked: {
                ElisaApplication.musicManager.scanCollection(MusicListenersManager.Hard)
                showPassiveNotification(i18nc("@info", "Database has been reset"))
                showPassiveNotification(i18nc("@info", "Started scanning for music"))
            }
        }

        Item {
            Kirigami.FormData.isSection: true
            visible: !Kirigami.Settings.isMobile
        }

        // desktop only, since the mobile sidebar does not support it
        QQC2.ComboBox {
            id: embeddedCategoryCombo
            visible: !Kirigami.Settings.isMobile

            Kirigami.FormData.label: i18nc("@label:listbox", "Embed category in sidebar:")

            model: [i18nc("@item:inlistbox Configure dialog, embed no category in views navigation list", "Nothing"),
                i18nc("@item:inlistbox Configure dialog, embed all albums in views navigation list", "Albums"),
                i18nc("@item:inlistbox Configure dialog, embed all artists in views navigation list", "Artists"),
                i18nc("@item:inlistbox Configure dialog, embed all genres in views navigation list", "Genres")]

            editable: false
            currentIndex: (ElisaConfigurationDialog.embeddedView === ElisaUtils.Genre ? 3 : (ElisaConfigurationDialog.embeddedView === ElisaUtils.Album ? 1 : (ElisaConfigurationDialog.embeddedView === ElisaUtils.Artist ? 2 : 0)))

            onActivated: {
                ElisaConfigurationDialog.embeddedView = (currentIndex === 0 ? ElisaUtils.Unknown : (currentIndex === 1 ? ElisaUtils.Album : (currentIndex === 2 ? ElisaUtils.Artist : ElisaUtils.Genre)))
            }

            Connections {
                target: ElisaConfigurationDialog

                function onEmbeddedViewChanged() {
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

            Kirigami.FormData.label: i18nc("@label:listbox", "Initial view on startup:")

            model: [i18nc("@item:inlistbox Title of the view of the playlist", "Now Playing"),
                i18nc("@item:inlistbox Title of the view of recently played tracks", "Recently Played"),
                i18nc("@item:inlistbox Title of the view of frequently played tracks", "Frequently Played"),
                i18nc("@item:inlistbox Title of the view of all albums", "Albums"),
                i18nc("@item:inlistbox Title of the view of all artists", "Artists"),
                i18nc("@item:inlistbox Title of the view of all tracks", "Tracks"),
                i18nc("@item:inlistbox Title of the view of all genres", "Genres"),
                i18nc("@item:inlistbox Title of the file browser view", "Files"),
                i18nc("@item:inlistbox Title of the file radios browser view", "Radio Stations"),
            ]

            editable: false
            currentIndex: ElisaConfigurationDialog.initialViewIndex

            onActivated: {
                ElisaConfigurationDialog.initialViewIndex = currentIndex
            }

            Connections {
                target: ElisaConfigurationDialog

                function onInitialViewIndexChanged() {
                    initialViewCombo.currentIndex = ElisaConfigurationDialog.initialViewIndex
                }
            }
        }

        RowLayout {
            Kirigami.FormData.label: i18nc("@label:textbox", "Initial location for Files view:")
            spacing: Kirigami.Units.smallSpacing

            QQC2.TextField {
                id: initialFilesViewPathTextField

                text: ElisaConfigurationDialog.initialFilesViewPath
                onTextChanged: {
                    ElisaConfigurationDialog.initialFilesViewPath = text
                }
            }

            QQC2.Button {
                icon.name: "document-open-folder"
                text: i18nc("@action:button as in, choose a file path on disk", "Choose…")
                onClicked: {
                    filesViewPathChooserDialog.visible = true
                }

                Dialogs.FolderDialog {
                    id: filesViewPathChooserDialog

                    title: i18nc("@title:window", "Choose a Folder")
                    currentFolder: ElisaConfigurationDialog.initialFilesViewPath

                    onAccepted: {
                        const url = selectedFolder
                        initialFilesViewPathTextField.text = url.toString().replace("file://", "")
                    }
                }
            }
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        // Indexing settings
        // =================
        QQC2.ComboBox {
            id: indexingTypeCombo
            Kirigami.FormData.label: i18nc("@label:listbox", "Music indexing:")

            editable: false

            model: [i18nc("@item:inlistbox Configure dialog, indexing type", "Use fast native indexer"),
                    i18nc("@item:inlistbox Configure dialog, indexing type", "Scan the filesystem directly")]

            currentIndex: ElisaConfigurationDialog.forceUsageOfSlowFileSystemIndexing ? 1 : 0
            onActivated: {
                ElisaConfigurationDialog.forceUsageOfSlowFileSystemIndexing = currentIndex === 0 ? false : true
            }
        }

        Kirigami.InlineMessage {
            Layout.fillWidth: true

            visible: indexingTypeCombo.currentIndex === 1

            type: Kirigami.MessageType.Warning
            text: i18nc("@info", "This is slower than the fast indexer. Please activate it only if Elisa cannot find your music and searching for one of the missing music files using your file manager also does not work. Please report this as a bug.")

            actions: [
                Kirigami.Action {
                    text: i18nc("@action:button", "Report Bug")
                    icon.name: "tools-report-bug"
                    onTriggered: Qt.openUrlExternally("https://bugs.kde.org/enter_bug.cgi?product=frameworks-baloo")
                }
            ]
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        // Playlist save settings
        // ======================

        ColumnLayout {
            Kirigami.FormData.label: i18n("When saving playlist files:")
            Kirigami.FormData.buddyFor: playlistFilePathTypeBox

            Layout.fillWidth: true

            spacing: Kirigami.Units.smallSpacing

            QQC2.ComboBox {
                id: playlistFilePathTypeBox
                model: [i18nc("@item:inlistbox Configure dialog, playlist save type", "Prefer relative paths"),
                        i18nc("@item:inlistbox Configure dialog, playlist save type", "Always use absolute paths")]

                currentIndex: ElisaConfigurationDialog.alwaysUseAbsolutePlaylistPaths ? 1 : 0
                onActivated: {
                    ElisaConfigurationDialog.alwaysUseAbsolutePlaylistPaths = currentIndex === 0 ? false : true
                }
            }

            QQC2.Label {
                Layout.fillWidth: true
                Layout.maximumWidth: Kirigami.Units.gridUnit * 16
                text: xi18nc("@info:tooltip Playlist Relative Paths", "When <interface>Prefer relative paths</interface> is selected, files in the same folder as the playlist will be referred with only the filename. Absolute paths are used in other cases.")
                wrapMode: Text.Wrap
                font: Kirigami.Theme.smallFont
            }
        }
    }

    // Music locations list
    // ====================
    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        Kirigami.Heading {
            Layout.fillWidth: true
            text: i18nc("@title The configured folders where the user's music collection can be found", "Music folders:")
            level: 4
        }

        QQC2.ScrollView {
            id: scrollview

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumHeight: Kirigami.Units.gridUnit * 12

            // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

            // Show the border
            Component.onCompleted: {
                scrollview.background.visible = true;
            }

            contentItem: ListView {
                id: pathList

                clip: true

                model: ElisaConfigurationDialog.rootPath

                delegate: Kirigami.SwipeListItem {
                    id: delegate
                    // Don't need a highlight effect on hover
                    hoverEnabled: false

                    QQC2.Label {
                        text: modelData
                        width: delegate.width - delegate.leftPadding
                                              - delegate.rightPadding
                                              - (action.visible ? action.width : 0)
                        elide: Text.ElideMiddle
                    }

                    actions: Kirigami.Action {
                        id: action
                        icon.name: "edit-delete"
                        text: i18nc("@action:button", "Stop looking for music here")

                        visible: pathList.count > 1
                        onTriggered: ElisaConfigurationDialog.removeMusicLocation(modelData)
                    }
                }
            }
        }

        RowLayout {
            spacing: Kirigami.Units.largeSpacing * 2

            QQC2.Button {
                text: i18nc("@action:button", "Add New Location")
                icon.name: "list-add"

                Layout.alignment: Qt.AlignTop | Qt.AlignLeft

                onClicked: fileDialog.open()
                Accessible.onPressAction: onClicked

                Dialogs.FolderDialog {
                    id: fileDialog
                    title: i18nc("@title:window", "Choose a Folder")

                    currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]

                    visible: false

                    onAccepted: {
                        var oldPaths = ElisaConfigurationDialog.rootPath
                        oldPaths.push(fileDialog.selectedFolder)
                        ElisaConfigurationDialog.rootPath = oldPaths
                    }
                }
            }

            QQC2.Label {
                Layout.fillWidth: true
                visible: indexingTypeCombo.currentIndex === 0 // Native indexer
                text: i18nc("@label", "The fast native file indexer does not follow symbolic links. If any of the locations in this list contain symbolic links to other locations, you will need to add those other locations to the list as well.")
                wrapMode: Text.Wrap
                font: Kirigami.Theme.smallFont
            }
        }
    }
}
