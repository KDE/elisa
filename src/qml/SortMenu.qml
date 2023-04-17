/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2

QQC2.Menu {
    id: root

    property int sortRole
    property string sortRoleName
    property var sortRoles
    property var sortRoleNames
    property var sortOrder
    property var sortOrderNames

    title: i18nc("@title:menu", "Sort By")

    function refreshSortOrderNames() {
        if (!root.sortOrderNames) {
            return
        }

        for (let i = 0; i < sortRoleNames.length; ++i) {
            if (sortRoles[i] === root.sortRole && root.sortOrderNames.length >= (i * 2 + 1)) {
                ascendingSortOrder.text = root.sortOrderNames[i * 2]
                descendingSortOrder.text = root.sortOrderNames[i * 2 + 1]
                root.sortRoleName = root.sortRoleNames[i]
            }
        }
    }

    QQC2.ButtonGroup {
        id: rolesButtonGroup
    }

    Repeater {
        id: rolesRepeater

        model: sortRoleNames.length > 1 ? sortRoleNames.length : 0

        delegate: QQC2.MenuItem {
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
        visible: sortRoleNames.length > 1
    }

    QQC2.ButtonGroup {
        id: orderButtonGroup
    }

    QQC2.MenuItem {
        id: ascendingSortOrder

        checkable: true
        checked: sortOrder === Qt.AscendingOrder

        onToggled: sortOrder = Qt.AscendingOrder

        QQC2.ButtonGroup.group: orderButtonGroup
    }

    QQC2.MenuItem {
        id: descendingSortOrder

        checkable: true
        checked: sortOrder === Qt.DescendingOrder

        onToggled: sortOrder = Qt.DescendingOrder

        QQC2.ButtonGroup.group: orderButtonGroup
    }

    onSortRoleChanged: refreshSortOrderNames()

    onSortOrderNamesChanged: refreshSortOrderNames()
}
