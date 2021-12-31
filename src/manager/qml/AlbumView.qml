/*
   SPDX-FileCopyrightText: 2021 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import QtQml.Models 2.15
import org.kde.kirigami 2.18 as Kirigami
import org.kde.elisa 1.0

Controls.ScrollView {
    property alias model: delegateModel.model
    property AbstractItemModel realModel

    implicitHeight: listView.implicitHeight
    implicitWidth: listView.implicitWidth

    Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
    Controls.ScrollBar.vertical.snapMode: Controls.ScrollBar.SnapAlways

    ListView {
        id: listView

        implicitHeight: count * 25

        model: DelegateModel {
            id: delegateModel

            delegate: Controls.ItemDelegate {
                text: i18n('<b>%1</b> - <i>%2</i> - %3', model.title, model.duration, model.artist)
            }
        }
    }
}
