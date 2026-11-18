/*
   SPDX-FileCopyrightText: 2026 (c) Morgan Astra <morganastra@proton.me>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami as Kirigami

import org.kde.elisa

Item {
    id: root

    property ListView listView

    property int dragRow: -1
    property int blockCount: 0
    property int insertBeforeRow: -1
    property real lastCursorX: 0
    property real lastCursorY: 0

    anchors.fill: parent

    function startAlbumDrag(row, headerData, cursorX, cursorY) {
        if (dragRow >= 0) {
            return false
        }

        const block = ElisaApplication.mediaPlayListProxyModel.albumBlockAt(row)
        if (!block.isValid) {
            return false
        }

        dragRow = block.firstRow
        blockCount = block.count
        albumDragGhost.headerData = headerData
        updateAlbumDrag(cursorX, cursorY)
        return true
    }

    function insertionRowAt(cursorY, hoveredRow) {
        if (hoveredRow < 0) {
            return cursorY + listView.contentY >= listView.contentHeight ? listView.count : dragRow
        }

        if (hoveredRow >= dragRow && hoveredRow < dragRow + blockCount) {
            return dragRow
        }

        const block = ElisaApplication.mediaPlayListProxyModel.albumBlockAt(hoveredRow)
        if (!block.isValid) {
            return dragRow
        }

        const offset = hoveredRow - block.firstRow
        const halfOffset = (block.count - 1) / 2
        let above
        if (block.count % 2 === 1 && offset === Math.floor(halfOffset)) {
            const hoveredItem = listView.itemAtIndex(hoveredRow)
            above = !hoveredItem || (cursorY + listView.contentY - hoveredItem.y) < hoveredItem.height / 2
        } else {
            above = offset < halfOffset
        }

        return above ? block.firstRow : block.firstRow + block.count
    }

    function placeAlbumDragGhost(insertBefore, hoveredRow) {
        let ghostY
        if (insertBefore >= listView.count) {
            ghostY = listView.contentHeight - listView.contentY
        } else {
            const boundaryItem = listView.itemAtIndex(insertBefore)
            ghostY = boundaryItem ? boundaryItem.y - listView.contentY
                : (insertBefore < hoveredRow ? 0 : listView.height)
        }
        albumDragGhost.y = Math.max(0, Math.min(listView.height - albumDragGhost.height, ghostY))
    }

    function updateAlbumDrag(cursorX, cursorY) {
        if (dragRow < 0) {
            return
        }

        lastCursorX = cursorX
        lastCursorY = cursorY

        const clampedX = Math.max(0, Math.min(listView.width - 1, cursorX))
        const clampedY = Math.max(0, Math.min(listView.height - 1, cursorY))

        const hoveredRow = listView.indexAt(clampedX, clampedY + listView.contentY)
        insertBeforeRow = insertionRowAt(clampedY, hoveredRow)
        placeAlbumDragGhost(insertBeforeRow, hoveredRow)

        const scrollZone = Kirigami.Units.gridUnit * 2
        albumDragScrollTimer.running = listView.contentHeight > listView.height
            && ((lastCursorY < scrollZone && !listView.atYBeginning)
                || (lastCursorY > listView.height - scrollZone && !listView.atYEnd))
    }

    function endAlbumDrag() {
        if (dragRow < 0) {
            return
        }

        ElisaApplication.mediaPlayListProxyModel.moveRowsBefore(dragRow, blockCount, insertBeforeRow)
        reset()
    }

    function reset() {
        dragRow = -1
        blockCount = 0
        insertBeforeRow = -1
        albumDragScrollTimer.running = false
    }

    Connections {
        target: root.listView

        function onCountChanged() {
            if (root.dragRow >= 0) {
                root.reset()
            }
        }
    }

    MouseArea {
        id: albumHandleArea

        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        preventStealing: true

        function handleRowAt(x, y) {
            if (ElisaApplication.mediaPlayListProxyModel.shuffleMode === MediaPlayListProxyModel.Track) {
                return -1
            }

            const row = listView.indexAt(x, y + listView.contentY)
            if (row < 0) {
                return -1
            }

            const rowItem = listView.itemAtIndex(row)
            const headerItem = rowItem ? rowItem.albumHeader : null
            if (!headerItem) {
                return -1
            }

            const point = albumHandleArea.mapToItem(headerItem, x, y)
            if (!headerItem.dragHandleContains(point.x, point.y)) {
                return -1
            }

            return row
        }

        onPressed: (mouse) => {
            const row = handleRowAt(mouse.x, mouse.y)
            if (row < 0) {
                mouse.accepted = false
                return
            }

            root.startAlbumDrag(row, listView.itemAtIndex(row).albumHeaderData, mouse.x, mouse.y)
        }

        onPositionChanged: (mouse) => {
            if (root.dragRow >= 0) {
                root.updateAlbumDrag(mouse.x, mouse.y)
            }
        }

        onReleased: {
            if (root.dragRow >= 0) {
                root.endAlbumDrag()
            }
        }

        onCanceled: {
            root.reset()
        }
    }

    BasicPlayListAlbumHeader {
        id: albumDragGhost

        headerData: ['', '', '']

        visible: root.dragRow >= 0
        width: parent.width
        z: 100
        opacity: 0.6
    }

    Timer {
        id: albumDragScrollTimer

        interval: 50
        repeat: true

        onTriggered: {
            if (root.dragRow < 0) {
                running = false
                return
            }

            const scrollZone = Kirigami.Units.gridUnit * 2
            if (root.lastCursorY < scrollZone && !listView.atYBeginning) {
                listView.contentY -= Kirigami.Units.gridUnit
            } else if (root.lastCursorY > listView.height - scrollZone && !listView.atYEnd) {
                listView.contentY += Kirigami.Units.gridUnit
            } else {
                running = false
                return
            }

            root.updateAlbumDrag(root.lastCursorX, root.lastCursorY)
        }
    }
}
