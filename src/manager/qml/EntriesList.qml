/*
   SPDX-FileCopyrightText: 2021 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import org.kde.kirigami 2.18 as Kirigami
import org.kde.elisa 1.0

Kirigami.ScrollablePage {
    property alias model: listView.model
    property AbstractItemModel realModel

    ListView {
        id: listView

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
            visible: listView.count === 0
            text: i18n("No data found")
        }

        delegate: ItemDelegate {
            text: model.display
        }
    }
}
