/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.ki18n

QQC2.Menu {
    id: root

    property int sortRole
    property string sortRoleName
    property var sortRoles
    property var sortRoleNames
    property /*Qt::SortOrder*/int sortOrder
    property var sortOrderNames

    title: KI18n.i18nc("@title:menu", "Sort By")

    function refreshSortOrderNames() {
        if (!sortOrderNames) {
            return
        }

        for (let i = 0; i < sortRoleNames.length; ++i) {
            if (sortRoles[i] === sortRole && sortOrderNames.length >= (i * 2 + 1)) {
                ascendingSortOrder.text = sortOrderNames[i * 2]
                descendingSortOrder.text = sortOrderNames[i * 2 + 1]
                sortRoleName = sortRoleNames[i]
            }
        }
    }

    QQC2.ButtonGroup {
        id: rolesButtonGroup
    }

    Repeater {
        id: rolesRepeater

        model: root.sortRoleNames.length > 1 ? root.sortRoleNames.length : 0

        delegate: QQC2.MenuItem {
            required property int index

            text: root.sortRoleNames[index]

            checkable: true

            checked: root.sortRoles[index] === root.sortRole

            onToggled: {
                root.sortRole = root.sortRoles[index]
                checked = (root.sortRoles[index] === root.sortRole)
            }

            QQC2.ButtonGroup.group: rolesButtonGroup
        }
    }

    QQC2.MenuSeparator {
        visible: root.sortRoleNames.length > 1
    }

    QQC2.ButtonGroup {
        id: orderButtonGroup
    }

    QQC2.MenuItem {
        id: ascendingSortOrder

        checkable: true
        checked: root.sortOrder === Qt.AscendingOrder

        onToggled: root.sortOrder = Qt.AscendingOrder

        QQC2.ButtonGroup.group: orderButtonGroup
    }

    QQC2.MenuItem {
        id: descendingSortOrder

        checkable: true
        checked: root.sortOrder === Qt.DescendingOrder

        onToggled: root.sortOrder = Qt.DescendingOrder

        QQC2.ButtonGroup.group: orderButtonGroup
    }

    onSortRoleChanged: refreshSortOrderNames()

    onSortOrderNamesChanged: refreshSortOrderNames()
}
