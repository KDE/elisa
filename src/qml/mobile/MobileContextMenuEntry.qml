/*
   SPDX-FileCopyrightText: 2021 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

Kirigami.BasicListItem {
    Layout.fillWidth: true
    Layout.preferredHeight: Kirigami.Units.gridUnit * 2
    iconSize: Kirigami.Units.gridUnit
    leftPadding: Kirigami.Units.smallSpacing
    rightPadding: 0
}
