/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.10
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5 as Kirigami
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
    Kirigami.Separator {
        visible: type == "footer" && type != "other"
        Layout.fillWidth: true
    }

    // Background rectangle + content layout
    Rectangle {
        id: headerBackground

        color: myPalette.window

        Layout.fillWidth: true
        height: type == "footer" ?
            elisaTheme.footerToolbarHeight :
            elisaTheme.headerToolbarHeight



        // Content layout
        RowLayout {
            id: contentLayout

            anchors {
                left: parent.left
                leftMargin: Kirigami.Units.largeSpacing
                right: parent.right
                rightMargin: Kirigami.Units.largeSpacing
                verticalCenter: parent.verticalCenter
            }
            spacing: 0

            // Items provided by the contentItems property will go here
        }
    }

    // Separator line under the header
    Kirigami.Separator {
        visible: type == "header" && type != "other"
        Layout.fillWidth: true
    }
}
