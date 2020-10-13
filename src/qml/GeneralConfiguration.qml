/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.8 as Kirigami

import org.kde.elisa 1.0

ColumnLayout {
    spacing: Kirigami.Units.smallSpacing

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    CheckBox {
        checked: ElisaConfigurationDialog.showProgressInTaskBar

        text: i18n("Show progress on Task Manager entries")

        onCheckedChanged: ElisaConfigurationDialog.showProgressInTaskBar = checked
    }

    CheckBox {
        checked: ElisaConfigurationDialog.showSystemTrayIcon

        text: i18n("Keep running in System Tray when main window is closed")

        onToggled: ElisaConfigurationDialog.showSystemTrayIcon = checked
    }

    CheckBox {
        checked: ElisaConfigurationDialog.playAtStartup

        text: i18n("Start playing on startup")

        onToggled: ElisaConfigurationDialog.playAtStartup = checked
    }

    RowLayout {
        spacing: Kirigami.Units.smallSpacing

        Label {
            text: i18n("Embed a category in the views navigation list:")
        }

        ComboBox {
            id: embeddedCategoryCombo

            property bool isFinished: false

            model: [i18nc("Configure dialog, embed no category in views navigation list", "No embedded data"),
                i18nc("Configure dialog, embed all albums in views navigation list", "Embed Albums"),
                i18nc("Configure dialog, embed all artists in views navigation list", "Embed Artists"),
                i18nc("Configure dialog, embed all genres in views navigation list", "Embed Genres")]

            editable: false
            currentIndex: (ElisaConfigurationDialog.embeddedView === ElisaUtils.Genre ? 3 : (ElisaConfigurationDialog.embeddedView === ElisaUtils.Album ? 1 : (ElisaConfigurationDialog.embeddedView === ElisaUtils.Artist ? 2 : 0)))

            onCurrentIndexChanged: {
                if (!isFinished) {
                    return
                }

                ElisaConfigurationDialog.embeddedView = (currentIndex === 0 ? ElisaUtils.Unknown : (currentIndex === 1 ? ElisaUtils.Album : (currentIndex === 2 ? ElisaUtils.Artist : ElisaUtils.Genre)))
            }
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

        Component.onCompleted: {
            if (ElisaConfigurationDialog.embeddedView == ElisaUtils.Unknown) {
                embeddedCategoryCombo.currentIndex = 0
            } else if (ElisaConfigurationDialog.embeddedView == ElisaUtils.Album) {
                embeddedCategoryCombo.currentIndex = 1
            } else if (ElisaConfigurationDialog.embeddedView == ElisaUtils.Artist) {
                embeddedCategoryCombo.currentIndex = 2
            } else if (ElisaConfigurationDialog.embeddedView == ElisaUtils.Genre) {
                embeddedCategoryCombo.currentIndex = 3
            }

            embeddedCategoryCombo.isFinished = true
        }
    }

    RowLayout {
        spacing: Kirigami.Units.smallSpacing

        Label {
            text: i18n("Initial view at start:")
        }

        ComboBox {
            id: initialViewCombo

            property bool isFinished: false

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

            onCurrentIndexChanged: {
                if (!isFinished) {
                    return
                }

                ElisaConfigurationDialog.initialViewIndex = currentIndex
            }
        }

        Connections {
            target: ElisaConfigurationDialog

            onInitialViewIndexChanged: initialViewCombo.currentIndex = ElisaConfigurationDialog.initialViewIndex
        }

        Component.onCompleted: {
            initialViewCombo.currentIndex = ElisaConfigurationDialog.initialViewIndex
            initialViewCombo.isFinished = true
        }
    }
}
