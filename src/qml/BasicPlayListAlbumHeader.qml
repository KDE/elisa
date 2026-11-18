/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.elisa

Rectangle {
    id: root

    property var headerData
    property string album: headerData[0]
    property string albumArtist: headerData[1]
    property url imageUrl: headerData[2]

    property bool simpleMode: false

    property bool beingDragged: false

    property bool showDragHandle: false

    property bool showRemoveButton: false

    signal removeRequested()

    visible: !beingDragged

    function dragHandleContains(x, y) {
        if (!dragHandleIcon.visible || x < 0 || x > width || y < 0 || y > height) {
            return false
        }

        const point = dragHandleIcon.mapFromItem(root, x, y)
        return point.x >= 0 && point.x <= dragHandleIcon.width
    }

    implicitHeight: contentLayout.height + contentLayout.anchors.topMargin * 2

    color: simpleMode ? "transparent" : palette.window

    HoverHandler {
        id: headerHover
    }

    RowLayout {
        id: contentLayout

        anchors {
            top: parent.top
            topMargin: Kirigami.Units.smallSpacing
            left: parent.left
            right: parent.right
        }

        spacing: Kirigami.Units.smallSpacing

        // keep section's elements aligned with playlistEntry's ones
        Item {
            visible: !root.simpleMode && !root.showDragHandle
            Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
            Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
        }

        Kirigami.Icon {
            id: dragHandleIcon

            visible: root.showDragHandle && !root.simpleMode

            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
            Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium

            source: "handle-sort"

            HoverHandler {
                id: dragHandleHover
            }

            opacity: dragHandleHover.hovered ? 1 : 0.6
        }

        Kirigami.Icon {
            Layout.preferredWidth: Theme.coverArtSize
            Layout.preferredHeight: Theme.coverArtSize

            readonly property url image: root.imageUrl
            source: (image === "" || image === Qt.url("") || image === undefined || image === null) ? "not-an-icon" : image
            fallback: Theme.defaultAlbumImage
            roundToIconSize: false
        }

        ColumnLayout {
            id: albumHeaderTextColumn

            Layout.fillWidth: true
            spacing: 0

            LabelWithToolTip {
                id: mainLabel

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBottom | Qt.AlignLeft

                text: root.album
                level: 2
            }

            LabelWithToolTip {
                id: authorLabel

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop | Qt.AlignLeft

                text: root.albumArtist
                type: Kirigami.Heading.Type.Secondary
            }
        }

        FlatButtonWithToolTip {
            visible: root.showRemoveButton && headerHover.hovered

            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            Layout.preferredWidth: Theme.listDelegateButtonHeight
            Layout.preferredHeight: Theme.listDelegateButtonHeight

            text: KI18n.i18nc("@action:button Remove album from play list", "Remove Album")
            icon.name: "edit-delete-remove"

            onClicked: root.removeRequested()
        }
    }
}
