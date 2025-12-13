/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2021 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC
import org.kde.kirigami as Kirigami
import org.kde.elisa
import org.kde.ki18n

QQC.ItemDelegate {
    id: dataEntry

    property string mainText
    property string secondaryText
    property bool displaySecondaryText: true

    property url imageUrl
    property url imageFallbackUrl
    property var multipleImageUrls

    property url fileUrl
    property bool isPartial
    property bool isSelected
    property bool hasChildren: true
    property bool showPlayButton: true
    property bool showEnqueueButton: true
    readonly property alias mobileContextMenuAction: mobileContextMenuAction

    signal enqueue()
    signal replaceAndPlay()
    signal playNext()
    signal open()
    signal selected()

    property list<Kirigami.Action> actions: [
        Kirigami.Action {
            text: KI18n.i18nc("@action:button", "Play now, replacing current playlist")
            icon.name: "media-playback-start-symbolic"
            visible: dataEntry.showPlayButton
            onTriggered: dataEntry.replaceAndPlay()
        },
        Kirigami.Action {
            text: KI18n.i18nc("@action:button", "Play next")
            icon.name: "media-playlist-append-next-symbolic"
            onTriggered: dataEntry.playNext()
        },
        Kirigami.Action {
            text: KI18n.i18nc("@action:button", "Add to playlist")
            icon.name: 'list-add'
            visible: dataEntry.showEnqueueButton
            onTriggered: dataEntry.enqueue()
        },
        Kirigami.Action {
            visible: dataEntry.fileUrl.toString().substring(0, 7) === 'file://' && Kirigami.Settings.isMobile
            onTriggered: ElisaApplication.showInFolder(dataEntry.fileUrl)
            icon.name: "document-open-folder"
            text: KI18n.i18nc("@action:button Show the file for this song in the file manager", "Show in folder")
        }
    ]

    property bool delegateLoaded: true
    ListView.onPooled: delegateLoaded = false
    ListView.onReused: delegateLoaded = true

    function openMobileContextMenu() {
        mobileContextMenuLoader.active = true;
        (mobileContextMenuLoader.item as Kirigami.MenuDialog).open();
    }

    Keys.onReturnPressed: open()
    Keys.onEnterPressed: open()

    Accessible.role: Accessible.ListItem
    Accessible.name: mainText

    Kirigami.Action {
        id: mobileContextMenuAction

        text: KI18n.i18nc("@action:button", "Options")
        icon.name: "view-more-symbolic"
        onTriggered: dataEntry.openMobileContextMenu()
        visible: Kirigami.Settings.isMobile
    }

    Loader {
        id: mobileContextMenuLoader
        active: false

        sourceComponent: Kirigami.MenuDialog {
            id: contextMenu
            title: dataEntry.mainText
            preferredWidth: Kirigami.Units.gridUnit * 20

            actions: dataEntry.actions
        }
    }
}
