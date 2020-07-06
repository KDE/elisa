/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

import org.kde.elisa 1.0

ColumnLayout {
    spacing: 0

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    CheckBox {
        checked: ElisaConfigurationDialog.showProgressInTaskBar

        text: i18n("Show progress on Task Manager entries")

        onCheckedChanged: ElisaConfigurationDialog.showProgressInTaskBar = checked
    }

    CheckBox {
        checked: ElisaConfigurationDialog.showSystemTrayIcon

        text: i18n("Keep running in System Tray when main window is closed")

        onToggled: ElisaConfigurationDialog.showSystemTrayIcon = checked
    }
}
