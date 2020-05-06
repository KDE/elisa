/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

ColumnLayout {
    spacing: 0

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    CheckBox {
        checked: config.showProgressInTaskBar

        text: i18n("Show progress on Task Manager entries")

        onCheckedChanged: config.showProgressInTaskBar = checked
    }

    CheckBox {
        checked: config.showSystemTrayIcon

        text: i18n("Keep running in System Tray when main window is closed")

        onToggled: config.showSystemTrayIcon = checked
    }
}
