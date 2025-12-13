/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.ki18n
import org.kde.kirigami as Kirigami

Rectangle {
    id: root
    property int discNumber

    color: palette.mid
    height: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing

    LabelWithToolTip {
        id: discHeaderLabel

        text: KI18n.i18nc("@title:row disc header text when showing an album", "Disc %1", root.discNumber)

        font.weight: Font.Bold
        font.italic: true
        color: palette.text

        verticalAlignment: Text.AlignVCenter

        anchors.fill: parent
        anchors.leftMargin: Kirigami.Units.largeSpacing

        elide: Text.ElideRight
    }
}
