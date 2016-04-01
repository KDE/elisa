/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.1
import QtQuick.Controls 1.3 as Controls

/**
 * An item that represents an abstract Action
 *
 * @inherit Controls.Action
 */
Controls.Action {
    id: root

    /**
     * visible: bool
     * True (default) when the graphic representation of the action
     * is supposed to be visible.
     * It's up to the action representation to honor this property.
     */
    property bool visible: true

    /**
     * children: list<Action>
     * A list of children actions.
     * Useful for tree-like menus
     * @code
     * Action {
     *    text: "Tools"
     *    Action {
     *        text: "Action1"
     *    }
     *    Action {
     *        text: "Action2"
     *    }
     * }
     * @endcode
     */
    default property alias children: root.__children
    property list<QtObject> __children
}
