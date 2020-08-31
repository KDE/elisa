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

Rectangle {
    // Is this a header or a footer? Acceptable values are
    // "header" (separator drawn on bottom)
    // "footer" (separator drawn on top)
    // "other" (no separator drawn)
    property string type

    // A list of items to be shown within the header or footer
    property alias contentItems: contentLayout.children

    // Spacing of content items. Defaults to Kirigami.Units.smallSpacing
    property alias contentLayoutSpacing: contentLayout.spacing

    implicitHeight: Math.round(Kirigami.Units.gridUnit * 2.5)

    color: myPalette.window

    // Separator line above the header
    Kirigami.Separator {
        visible: type == "footer" && type != "other"
        width: parent.width
        anchors.top: parent.top
    }

    // Content layout
    RowLayout {
        id: contentLayout

        anchors.fill: parent
        anchors.leftMargin: Kirigami.Units.smallSpacing
        anchors.rightMargin: Kirigami.Units.smallSpacing

        spacing: Kirigami.Units.smallSpacing

        // Items provided by the contentItems property will go here
    }

    // Separator line under the header
    Kirigami.Separator {
        visible: type == "header" && type != "other"
        width: parent.width
        anchors.bottom: parent.bottom
    }
}
