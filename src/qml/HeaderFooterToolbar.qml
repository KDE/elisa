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
    enum ToolbarType {
        Header, // separator drawn on bottom
        Footer, // separator drawn on top
        Other   // no separator drawn; used for stacking toolbars
    }

    // The type of toolbar it is
    property int toolbarType: HeaderFooterToolbar.ToolbarType.Header

    // A list of items to be shown within the header or footer
    property alias contentItems: contentLayout.children

    // Spacing of content items. Defaults to Kirigami.Units.smallSpacing
    property alias contentLayoutSpacing: contentLayout.spacing

    implicitHeight: Math.round(Kirigami.Units.gridUnit * 2.5)

    color: myPalette.window

    // Top separator line for a footer
    Kirigami.Separator {
        visible: toolbarType == HeaderFooterToolbar.ToolbarType.Footer
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

    // Bottom separator line for a header
    Kirigami.Separator {
        visible: toolbarType == HeaderFooterToolbar.ToolbarType.Header
        width: parent.width
        anchors.bottom: parent.bottom
    }
}
