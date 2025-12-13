/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.elisa

import ".."
import "../shared"

BasePlayListDelegate {
    id: playListEntry

    Accessible.role: Accessible.ListItem
    Accessible.name: title + ' ' + album + ' ' + artist

    Kirigami.Theme.useAlternateBackgroundColor: true
    padding: 0

    Keys.onReturnPressed: {
        playListEntry.switchToTrack(playListEntry.index)
        playListEntry.startPlayback()
    }

    function openContextMenu() {
        mainWindow.contextDrawer.close();
        contextMenuLoader.active = true;
        (contextMenuLoader.item as Kirigami.MenuDialog).open();
    }

    contentItem: Item {
        implicitHeight: 4 * Kirigami.Units.smallSpacing + Kirigami.Units.gridUnit * 2

        Loader {
            id: metadataLoader
            active: false
            onLoaded: mainWindow.pageStack.layers.push(item)

            sourceComponent: MobileMediaTrackMetadataView {
                fileName: playListEntry.fileName
                showImage: playListEntry.entryType !== ElisaUtils.Radio
                modelType: playListEntry.entryType
                showTrackFileName: playListEntry.entryType !== ElisaUtils.Radio
                showDeleteButton: playListEntry.entryType === ElisaUtils.Radio
                editableMetadata: playListEntry.metadataModifiableRole
                canAddMoreMetadata: playListEntry.entryType !== ElisaUtils.Radio
            }
        }

        Loader {
            active: playListEntry.isPlaying === MediaPlayList.IsPlaying || playListEntry.isPlaying === MediaPlayList.IsPaused
            sourceComponent: Rectangle {
                width: playListEntry.background.width * (ElisaApplication.audioPlayer.position / ElisaApplication.audioPlayer.duration)
                height: playListEntry.background.height
                color: palette.mid
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

                    text: playListEntry.title
                    textFormat: Text.PlainText
                    color: palette.text
                    elide: Text.ElideRight

                    Layout.fillWidth: true
                }

                Label {
                    id: artistLabel

                    text: {
                        let labelText = ""
                        if (playListEntry.artist) {
                            labelText += playListEntry.artist
                        }
                        if (playListEntry.album !== '') {
                            labelText += ' - ' + playListEntry.album
                            if (!playListEntry.hideDiscNumber && playListEntry.discNumber !== -1) {
                                labelText += ' - CD ' + playListEntry.discNumber
                            }
                        }
                        return labelText;
                    }
                    textFormat: Text.PlainText

                    visible: text.length > 0
                    opacity: Theme.subtitleOpacity
                    color: palette.text

                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    font: Kirigami.Theme.smallFont
                }
            }

            LabelWithToolTip {
                id: durationLabel

                text: playListEntry.duration

                font.weight: Font.Light
                color: palette.text

                Layout.rightMargin: !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
                Layout.leftMargin: LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
            }

            // context menu
            FlatButtonWithToolTip {
                id: contextMenuButton
                scale: LayoutMirroring.enabled ? -1 : 1
                text: playListEntry.entryType === ElisaUtils.Track ? KI18n.i18nc("@action:button", "Track Options") : KI18n.i18nc("@action:button", "Radio Options")
                icon.name: "view-more-symbolic"
                onClicked: playListEntry.openContextMenu()
                activeFocusOnTab: playListEntry.isSelected
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
                        text: KI18n.i18nc("@action:button Show the file for this song in the file manager", "Show in folder")
                        onTriggered: ElisaApplication.showInFolder(playListEntry.fileName)
                    },
                    Kirigami.Action {
                        visible: playListEntry.isValid
                        icon.name: "documentinfo"
                        text: KI18n.i18nc("@action:button Show track metadata", "View details")
                        onTriggered: {
                            if (metadataLoader.active === false) {
                                metadataLoader.active = true
                            } else {
                                metadataLoader.item.close();
                                metadataLoader.active = false
                            }
                        }
                    },
                    Kirigami.Action {
                        visible: playListEntry.isValid
                        icon.name: "error"
                        text: KI18n.i18nc("@action:button", "Remove from queue")
                        onTriggered: playListEntry.removeFromPlaylist(playListEntry.index)
                    }
                ]
            }
        }
    }
}
