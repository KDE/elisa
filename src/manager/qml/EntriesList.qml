/*
   SPDX-FileCopyrightText: 2021 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

Kirigami.ScrollablePage {
    property alias model: list.model
    property AbstractItemModel realModel

    ListView {
        id: list

        delegate: ItemDelegate {
            text: model.display
        }
    }
}
