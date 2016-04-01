/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
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
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 1.0

Item {
    id: canvas
    width: height
    height: Units.iconSizes.smallMedium
    property real morph: root.position
    property color color: Theme.textColor
    opacity: 0.8
    layer.enabled: true

    Rectangle {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            leftMargin: canvas.width/4 * morph
        }
        antialiasing: true
        transformOrigin: Item.Left
        width: (1 - morph) * height + morph * ((parent.width / Math.sqrt(2)) - height/2)
        height: Math.round(Units.smallSpacing / 2)
        color: canvas.color
        rotation: 45 * morph
    }

    Rectangle {
        anchors.centerIn: parent
        width: height * (1 - morph)
        height: Math.round(Units.smallSpacing / 2)
        color: canvas.color
    }


    Rectangle {
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            leftMargin: canvas.width/4 * morph
        }
        antialiasing: true
        transformOrigin: Item.Left
        width: (1 - morph) * height + morph * ((parent.width / Math.sqrt(2)) - height/2)
        height: Math.round(Units.smallSpacing / 2)
        color: canvas.color
        rotation: -45 * morph
    }
}

