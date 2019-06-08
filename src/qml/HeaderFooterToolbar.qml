/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2019 Nate Graham <nate@kde.org>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.10
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2

import org.kde.elisa 1.0

ColumnLayout {
    spacing: 0
    Layout.fillWidth: true

    // Is this a header or a footer? Acceptable values are
    // "header" (separator drawn on bottom)
    // "footer" (separator drawn on top)
    // "other" (no separator drawn)
    property string type

    // A list of items to be shown within the header or footer
    property alias contentItems: contentLayout.children

    // Spacing of content items. Defaults to 0
    property alias contentLayoutSpacing: contentLayout.spacing


    // Separator line above the header
    Rectangle {
        visible: type == "footer" && type != "other"
        Layout.fillWidth: true
        height: 1
        color: myPalette.mid
    }

    // Background rectangle + content layout
    Rectangle {
        id: headerBackground

        color: myPalette.window

        Layout.fillWidth: true
        height: contentLayout.height

        // Content layout
        RowLayout {
            id: contentLayout

            anchors {
                left: parent.left
                leftMargin: elisaTheme.layoutHorizontalMargin
                right: parent.right
                rightMargin: elisaTheme.layoutHorizontalMargin
                verticalCenter: parent.verticalCenter
            }
            height: childrenRect.height + (elisaTheme.layoutVerticalMargin * 2)
            spacing: 0

            // Items provided by the contentItems property will go here
        }
    }

    // Separator line under the header
    Rectangle {
        visible: type == "header" && type != "other"
        Layout.fillWidth: true
        height: 1
        color: myPalette.mid
    }
}
