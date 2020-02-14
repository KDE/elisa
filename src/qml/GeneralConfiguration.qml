/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import QtQuick.Dialogs 1.2 as Dialogs

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
}
