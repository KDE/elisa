/*
   SPDX-FileCopyrightText: 2021 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.18 as Kirigami
import org.kde.elisa 1.0

Kirigami.AbstractCard {
    property alias fallbackIcon: albumCover.fallback

    contentItem: Item {
        // implicitWidth/Height define the natural width/height of an item if no width or height is specified.
        // The setting below defines a component's preferred size based on its content
        implicitWidth: delegateLayout.implicitWidth
        implicitHeight: delegateLayout.implicitHeight

        GridLayout {
            id: delegateLayout

            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }

            rowSpacing: Kirigami.Units.largeSpacing
            columnSpacing: Kirigami.Units.largeSpacing
            columns: 4

            Kirigami.Icon {
                id: albumCover

                implicitWidth: 64
                implicitHeight: 64
                source: model.imageUrl ? model.imageUrl : fallbackIcon
            }

            ColumnLayout {
                Layout.fillWidth: true

                Kirigami.Heading {
                    Layout.fillHeight: true
                    level: 1
                    text: model.display
                }

                Kirigami.Separator {
                    Layout.fillWidth: true
                }

                Kirigami.Heading {
                    level: 2
                    text: model.artist
                }
            }
        }
    }
}
