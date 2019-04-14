/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.10
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2

import org.kde.elisa 1.0

ColumnLayout {
    property string lyrics

    Label {
        id: title

        horizontalAlignment: Label.AlignHCenter

        text: i18nc("Lyrics label for track metadata view", "Lyrics")

        font.weight: Font.Bold

        Layout.fillWidth: true
    }

    Label {
        text: lyrics

        horizontalAlignment: Text.AlignLeft

        Layout.fillWidth: true

        wrapMode: Text.WordWrap
    }
}
