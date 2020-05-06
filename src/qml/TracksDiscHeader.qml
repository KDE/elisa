/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5 as Kirigami

Rectangle {
    property int discNumber

    color: myPalette.mid
    height: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing

    LabelWithToolTip {
        id: discHeaderLabel

        text: i18nc("disc header text when showing an album", "Disc %1", discNumber)

        font.weight: Font.Bold
        font.italic: true
        color: myPalette.text

        verticalAlignment: Text.AlignVCenter

        anchors.fill: parent
        anchors.leftMargin: Kirigami.Units.largeSpacing

        elide: Text.ElideRight
    }
}
