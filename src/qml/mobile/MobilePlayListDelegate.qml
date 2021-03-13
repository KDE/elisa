/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.13 as Kirigami
import org.kde.elisa 1.0

import ".."
import "../shared"

BasePlayListDelegate {
    id: playListEntry

    signal clicked()
    signal moveItemRequested(int from, int to)

    Accessible.role: Accessible.ListItem
    Accessible.name: title + ' ' + album + ' ' + artist

    Keys.onReturnPressed: {
        playListEntry.switchToTrack(playListEntry.index)
        playListEntry.startPlayback()
    }

    property double singleLineHeight: 4 * Kirigami.Units.smallSpacing + Kirigami.Units.gridUnit * 2
    height: singleLineHeight

    function openContextMenu() {
        mainWindow.contextDrawer.close();
        contextMenuLoader.active = true;
        contextMenuLoader.item.open();
    }

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

    // background
    Rectangle {
        id: entryBackground
        anchors.fill: parent

        color: mouseArea.containsMouse ? myPalette.highlight : (isPlaying ? myPalette.mid : myPalette.base)
        opacity: 0.2
        height: playListEntry.height
    }
    Loader {
        active: isPlaying === MediaPlayList.IsPlaying || isPlaying === MediaPlayList.IsPaused
        visible: active
        anchors.fill: parent

        sourceComponent: Rectangle {
            anchors.left: parent.left
            color: myPalette.mid
            height: playListEntry.height
            width: playListEntry.width * (ElisaApplication.audioPlayer.position / ElisaApplication.audioPlayer.duration)
        }
    }

    // click event
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: playListEntry.clicked()
    }

    // row contents
    RowLayout {
        id: trackRow

        anchors.fill: parent
        anchors.leftMargin: Kirigami.Units.largeSpacing

        spacing: Math.round(Kirigami.Units.smallSpacing / 2)

        // drag handle (for list reordering)
        Kirigami.ListItemDragHandle {
            listItem: playListEntry
            listView: playListEntry.listView
            onMoveRequested: playListEntry.moveItemRequested(oldIndex, newIndex)
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            Layout.leftMargin: Kirigami.Units.largeSpacing

            spacing: Kirigami.Units.smallSpacing / 2

            Label {
                id: mainLabelDetailed

                text: title
                horizontalAlignment: Text.AlignLeft
                color: myPalette.text

                Layout.alignment: Qt.AlignLeft
                Layout.fillWidth: true

                elide: Text.ElideRight
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
                horizontalAlignment: Text.AlignLeft

                visible: text.length > 0
                opacity: 0.6
                color: myPalette.text

                Layout.alignment: Qt.AlignLeft
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

            horizontalAlignment: Text.AlignRight

            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            Layout.rightMargin: !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
            Layout.leftMargin: LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
        }

        // context menu
        FlatButtonWithToolTip {
            id: contextMenuButton
            scale: LayoutMirroring.enabled ? -1 : 1
            Layout.alignment: Qt.AlignVCenter
            Layout.maximumHeight: parent.height
            Layout.preferredHeight: singleLineHeight - Kirigami.Units.smallSpacing * 2
            Layout.preferredWidth: height

            text: i18nc("Track options", "Track Options")
            icon.name: "view-more-symbolic"
            onClicked: openContextMenu()
        }
    }

    Loader {
        id: contextMenuLoader
        active: false

        // context menu sheet
        sourceComponent: MobileContextMenuSheet {
            id: contextMenu
            title: playListEntry.title

            ColumnLayout {
                Layout.preferredWidth: Kirigami.Units.gridUnit * 20
                spacing: 0
                MobileContextMenuEntry {
                    visible: playListEntry.fileName.toString().substring(0, 7) === 'file://'
                    onClicked: {
                        ElisaApplication.showInFolder(playListEntry.fileName)
                        contextMenu.close();
                    }
                    icon: "document-open-folder"
                    text: i18nc("Show the file for this song in the file manager", "Show in folder")
                }

                MobileContextMenuEntry {
                    visible: isValid
                    onClicked: {
                        if (metadataLoader.active === false) {
                            metadataLoader.active = true
                        }
                        else {
                            metadataLoader.item.close();
                            metadataLoader.active = false
                        }
                        contextMenu.close();
                    }
                    icon: "documentinfo"
                    text: i18nc("Show track metadata", "View details")
                }

                MobileContextMenuEntry {
                    visible: isValid
                    onClicked: {
                        playListEntry.removeFromPlaylist(playListEntry.index)
                        contextMenu.close();
                    }
                    icon: "error"
                    text: i18nc("Remove current track from play list", "Remove from queue")
                }
            }
        }
    }

}


