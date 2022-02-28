/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami

Kirigami.BasicListItem {
    Accessible.role: Accessible.ListItem
    Accessible.description: model.display
    Accessible.name: model.display

    padding: Kirigami.Units.smallSpacing
    separatorVisible: false
    alternatingBackground: false

    icon: model.image
    label: model.display

    ToolTip.visible: labelItem.truncated && hovered
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.text: model.display
}
