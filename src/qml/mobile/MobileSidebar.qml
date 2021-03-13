/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import org.kde.kirigami 2.11 as Kirigami
import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

Kirigami.GlobalDrawer {
    id: drawer

    property alias model: repeater.model
    signal switchView(int viewIndex)

    property int viewIndex: 0

    title: i18n("Elisa")
    titleIcon: "elisa"

    modal: true
    width: Kirigami.Units.gridUnit * 11
    bannerVisible: true

    actions: []

    Component {
        id: action
        Kirigami.Action {}
    }

    Component.onCompleted: {
        // disable default handle as it covers content
        handle.visible = false;

        let settings = action.createObject(drawer, {iconName: "settings-configure", text: i18n("Settings")});
        settings.onTriggered.connect(() => {
            mainWindow.pageStack.layers.push("MobileSettingsPage.qml");
        });
        drawer.actions.push(settings);
    }

    // add sidebar actions
    Repeater {
        id: repeater
        Item {
            Component.onCompleted: {
                // HACK: the images provided by the model are in the form "image://icon/view-media-genre"
                // remove the "image://icon/" in order to use icons
                let icon = String(model.image).substring(13);
                let object = action.createObject(drawer, {iconName: icon, text: model.display});
                object.onTriggered.connect(() => {
                    viewIndex = model.index;
                    switchView(model.index);
                });
                drawer.actions.push(object);
            }
        }
    }
}
