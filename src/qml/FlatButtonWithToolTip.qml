/*
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

Button {
    id: flatButtonWithToolTip

    activeFocusOnTab: true

    Keys.onReturnPressed: flatButtonWithToolTip.clicked()
    Accessible.onPressAction: flatButtonWithToolTip.clicked()

    flat: true
    display: AbstractButton.IconOnly

    icon.width: Kirigami.Units.iconSizes.smallMedium
    icon.height: Kirigami.Units.iconSizes.smallMedium

    implicitWidth: icon.width + Kirigami.Units.largeSpacing * 2
    implicitHeight: icon.height + Kirigami.Units.largeSpacing * 2

    ToolTip.visible: hovered && flatButtonWithToolTip.text.length > 0
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.text: flatButtonWithToolTip.text
}
