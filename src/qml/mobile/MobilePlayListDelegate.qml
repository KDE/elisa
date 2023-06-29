/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import ElisaGraphicalEffects 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.elisa 1.0

import ".."
import "../shared"

BasePlayListDelegate {
    id: playListEntry

    Accessible.role: Accessible.ListItem
    Accessible.name: title + ' ' + album + ' ' + artist

    alternatingBackground: true
    padding: 0

    Keys.onReturnPressed: {
        playListEntry.switchToTrack(playListEntry.index)
        playListEntry.startPlayback()
    }

    function openContextMenu() {
        mainWindow.contextDrawer.close();
        contextMenuLoader.active = true;
        contextMenuLoader.item.open();
    }

    contentItem: Item {
        implicitWidth: playListEntry.width
        implicitHeight: 4 * Kirigami.Units.smallSpacing + Kirigami.Units.gridUnit * 2

        Loader {
            id: metadataLoader
            active: false
            onLoaded: mainWindow.pageStack.layers.push(item)

            sourceComponent: MobileMediaTrackMetadataView {
                fileName: playListEntry.fileName
                showImage: entryType !== ElisaUtils.Radio
                modelType: entryType
                showTrackFileName: entryType !== ElisaUtils.Radio
                showDeleteButton: entryType === ElisaUtils.Radio
                editableMetadata: playListEntry.metadataModifiableRole
                canAddMoreMetadata: entryType !== ElisaUtils.Radio
            }
        }

        Loader {
            active: isPlaying === MediaPlayList.IsPlaying || isPlaying === MediaPlayList.IsPaused
            sourceComponent: Rectangle {
                width: background.width * (ElisaApplication.audioPlayer.position / ElisaApplication.audioPlayer.duration)
                height: background.height
                color: myPalette.mid
            }
        }

        // row contents
        RowLayout {
            id: trackRow

            anchors.fill: parent
            spacing: Math.round(Kirigami.Units.smallSpacing / 2)

            // drag handle (for list reordering)
            Kirigami.ListItemDragHandle {
                visible: playListEntry.showDragHandle
                listItem: playListEntry
                listView: playListEntry.listView
                onMoveRequested: (oldIndex, newIndex) => {
                    ElisaApplication.mediaPlayListProxyModel.moveRow(oldIndex, newIndex);
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: Kirigami.Units.largeSpacing

                spacing: Kirigami.Units.smallSpacing / 2

                Label {
                    id: mainLabelDetailed

                    text: title
                    textFormat: Text.PlainText
                    color: myPalette.text
                    elide: Text.ElideRight

                    Layout.fillWidth: true
                }

                Label {
                    id: artistLabel

                    text: {
                        var labelText = ""
                        if (artist) {
                            labelText += artist
                        }
                        if (album !== '') {
                            labelText += ' - ' + album
                            if (!hideDiscNumber && discNumber !== -1) {
                                labelText += ' - CD ' + discNumber
                            }
                        }
                        return labelText;
                    }
                    textFormat: Text.PlainText

                    visible: text.length > 0
                    opacity: 0.6
                    color: myPalette.text

                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    font: Kirigami.Theme.smallFont
                }
            }

            LabelWithToolTip {
                id: durationLabel

                text: duration

                font.weight: Font.Light
                color: myPalette.text

                Layout.rightMargin: !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
                Layout.leftMargin: LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
            }

            // context menu
            FlatButtonWithToolTip {
                id: contextMenuButton
                scale: LayoutMirroring.enabled ? -1 : 1
                text: i18nc("@action:button", "Track Options")
                icon.name: "view-more-symbolic"
                onClicked: openContextMenu()
            }
        }

        Loader {
            id: contextMenuLoader
            active: false

            // context menu sheet
            sourceComponent: Kirigami.MenuDialog {
                id: contextMenu
                title: playListEntry.title

                preferredWidth: Kirigami.Units.gridUnit * 20

                actions: [
                    Kirigami.Action {
                        visible: playListEntry.fileName.toString().substring(0, 7) === 'file://'
                        icon.name: "document-open-folder"
                        text: i18nc("@action:button Show the file for this song in the file manager", "Show in folder")
                        onTriggered: {
                            ElisaApplication.showInFolder(playListEntry.fileName)
                            contextMenu.close();
                        }
                    },
                    Kirigami.Action {
                        visible: isValid
                        icon.name: "documentinfo"
                        text: i18nc("@action:button Show track metadata", "View details")
                        onTriggered: {
                            if (metadataLoader.active === false) {
                                metadataLoader.active = true
                            }
                            else {
                                metadataLoader.item.close();
                                metadataLoader.active = false
                            }
                            contextMenu.close();
                        }
                    },
                    Kirigami.Action {
                        visible: isValid
                        icon.name: "error"
                        text: i18nc("@action:button", "Remove from queue")
                        onTriggered: {
                            playListEntry.removeFromPlaylist(playListEntry.index)
                            contextMenu.close();
                        }
                    }
                ]
            }
        }
    }
}
