/*
 *   Copyright 2016 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
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
import QtGraphicalEffects 1.0
import org.kde.kirigami 1.0

LinearGradient {
    id: shadow
    /**
     * edge: enumeration
     * This property holds the edge of the shadow that will determine
     * the direction of the gradient.
     * The acceptable values are:
     * Qt.TopEdge: The top edge of the content item.
     * Qt.LeftEdge: The left edge of the content item (default).
     * Qt.RightEdge: The right edge of the content item.
     * Qt.BottomEdge: The bottom edge of the content item.
     */
    property int edge: Qt.LeftEdge

    width: Units.gridUnit/2
    height: Units.gridUnit/2

    start: Qt.point((edge != Qt.RightEdge ? 0 : width), (edge != Qt.BottomEdge ? 0 : height))
    end: Qt.point((edge != Qt.LeftEdge ? 0 : width), (edge != Qt.TopEdge ? 0 : height))
    gradient: Gradient {
        GradientStop {
            position: 0.0
            color: Qt.rgba(0, 0, 0, 0.2)
        }
        GradientStop {
            position: 0.3
            color: Qt.rgba(0, 0, 0, 0.1)
        }
        GradientStop {
            position: 1.0
            color:  "transparent"
        }
    }
}

