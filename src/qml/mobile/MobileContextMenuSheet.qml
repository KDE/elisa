/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import org.kde.kirigami 2.13 as Kirigami

Kirigami.OverlaySheet {
    id: contextMenu

    property alias title: heading.text

    leftPadding: Kirigami.Units.smallSpacing
    rightPadding: Kirigami.Units.smallSpacing

    header: Kirigami.Heading {
        id: heading
        level: 2
        wrapMode: Text.Wrap
    }
    footer: RowLayout {
        Button {
            Layout.alignment: Qt.AlignRight
            text: i18n("Close")
            icon.name: "dialog-close"
            onClicked: contextMenu.close()
        }
    }
}
