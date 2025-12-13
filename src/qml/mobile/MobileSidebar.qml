/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import QtQuick

Kirigami.GlobalDrawer {
    id: drawer

    property alias model: repeater.model
    signal switchView(int viewIndex)

    property int viewIndex: 0

    title: KI18n.i18nc("@title:window", "Elisa")
    titleIcon: "elisa"

    // disable default handle as it covers content, also we implement our own handle for pages
    handleVisible: false
    modal: true
    width: Kirigami.Units.gridUnit * 11

    actions: []

    Component {
        id: action
        Kirigami.Action {}
    }

    Component.onCompleted: {
        const settings = action.createObject(drawer, {"icon.name": "settings-configure", text: KI18n.i18nc("@title:window", "Settings")}) as Kirigami.Action;
        settings.triggered.connect(() => {
            mainWindow.pageStack.layers.push("MobileSettingsPage.qml");
        });
        drawer.actions.push(settings);
    }

    // add sidebar actions
    Repeater {
        id: repeater
        Item {
            required property int index
            required property string title
            required property url image

            Component.onCompleted: {
                // HACK: the images provided by the model are in the form "image://icon/view-media-genre"
                // remove the "image://icon/" in order to use icons
                const icon = String(image).substring(13);
                const object = action.createObject(drawer, {"icon.name": icon, text: title}) as Kirigami.Action;
                object.triggered.connect(() => {
                    drawer.viewIndex = index;
                    drawer.switchView(index);
                });
                drawer.actions.push(object);
            }
        }
    }
}
