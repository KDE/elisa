/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.5

Menu {
    id: sortMenu

    property int sortRole
    property string sortRoleName
    property var sortRoles
    property var sortRoleNames
    property var sortOrder
    property var sortOrderNames

    title: i18nc("Sort By Menu", "Sort By")

    function refreshSortOrderNames() {
        if (!sortMenu.sortOrderNames) {
            return
        }

        for (let i = 0; i < sortRoleNames.length; ++i) {
            if (sortRoles[i] === sortMenu.sortRole && sortMenu.sortOrderNames.length >= (i * 2 + 1)) {
                ascendingSortOrder.text = sortMenu.sortOrderNames[i * 2]
                descendingSortOrder.text = sortMenu.sortOrderNames[i * 2 + 1]
                sortMenu.sortRoleName = sortMenu.sortRoleNames[i]
            }
        }
    }

    ButtonGroup {
        id: rolesButtonGroup
    }

    Repeater {
        id: rolesRepeater

        model: sortRoleNames.length > 1 ? sortRoleNames.length : 0

        delegate: MenuItem {
            text: sortMenu.sortRoleNames[index]

            checkable: true

            checked: sortMenu.sortRoles[index] === sortMenu.sortRole

            onToggled: {
                sortMenu.sortRole = sortMenu.sortRoles[index]
                checked = (sortMenu.sortRoles[index] === sortMenu.sortRole)
            }

            ButtonGroup.group: rolesButtonGroup
        }
    }

    MenuSeparator {
        visible: sortRoleNames.length > 1
    }

    ButtonGroup {
        id: orderButtonGroup
    }

    MenuItem {
        id: ascendingSortOrder

        checkable: true
        checked: sortOrder === Qt.AscendingOrder

        onToggled: sortOrder = Qt.AscendingOrder

        ButtonGroup.group: orderButtonGroup
    }

    MenuItem {
        id: descendingSortOrder

        checkable: true
        checked: sortOrder === Qt.DescendingOrder

        onToggled: sortOrder = Qt.DescendingOrder

        ButtonGroup.group: orderButtonGroup
    }

    onSortRoleChanged: refreshSortOrderNames()

    onSortOrderNamesChanged: refreshSortOrderNames()
}
