/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import org.kde.kirigami as Kirigami
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

Kirigami.OverlayDrawer {
    id: drawer

    property alias model: repeater.model
    signal switchView(int viewIndex)

    property int viewIndex: 0

    edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
    parent: QQC2.Overlay.overlay
    x: 0

    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    width: Kirigami.Units.gridUnit * 12
    modal: true
    handleVisible: false

    contentItem: ColumnLayout {
        spacing: 0

        Kirigami.AbstractApplicationHeader {
            Layout.fillWidth: true

            contentItem: Kirigami.Heading {
                anchors.fill: parent
                anchors.margins: Kirigami.Units.largeSpacing
                verticalAlignment: Text.AlignVCenter
                text: i18n("Elisa")
            }
        }

        QQC2.ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true

            QQC2.ScrollBar.vertical.policy: QQC2.ScrollBar.AlwaysOff
            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
            contentWidth: -1 // disable horizontal scroll

            ColumnLayout {
                id: column
                width: scrollView.width
                spacing: 0

                Repeater {
                    id: repeater

                    Kirigami.NavigationTabButton {
                        Layout.fillWidth: true
                        display: Kirigami.NavigationTabButton.TextBesideIcon

                        required property int index
                        required property string title
                        required property url image

                        text: title
                        icon.name: image
                        onClicked: {
                            viewIndex = index;
                            switchView(index);
                            drawer.close();
                        }
                    }
                }
            }
        }

        Kirigami.Separator { Layout.fillWidth: true }
        Kirigami.NavigationTabButton {
            Layout.fillWidth: true
            display: Kirigami.NavigationTabButton.TextBesideIcon

            icon.name: "settings-configure"
            text: i18nc("@title:window", "Settings")
            onClicked: {
                checked = false;
                applicationWindow().pageStack.layers.push("MobileSettingsPage.qml");
                drawer.close();
            }
        }
    }
}
