/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import org.kde.kirigami 2.10 as Kirigami

import org.kde.elisa

import QtQuick 2.0

Rectangle {
    property var headerData
    property string album: headerData[0]
    property string albumArtist: headerData[1]
    property url imageUrl: headerData[2]

    property bool simpleMode: false

    implicitHeight: contentLayout.height + contentLayout.anchors.topMargin * 2

    color: simpleMode ? "transparent" : myPalette.window

    RowLayout {
        id: contentLayout

        anchors {
            top: parent.top
            topMargin: Kirigami.Units.smallSpacing
            left: parent.left
            right: parent.right
        }

        spacing: Kirigami.Units.smallSpacing

        // keep section's elements aligned with playlistEntry's ones
        Item {
            visible: !simpleMode
            Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
            Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
        }

        ImageWithFallback {
            Layout.preferredWidth: elisaTheme.coverArtSize
            Layout.preferredHeight: elisaTheme.coverArtSize

            source: imageUrl
            fallback: elisaTheme.defaultAlbumImage

            sourceSize.width: height * Screen.devicePixelRatio
            sourceSize.height: height * Screen.devicePixelRatio

            fillMode: Image.PreserveAspectFit
            asynchronous: true
        }

        ColumnLayout {
            id: albumHeaderTextColumn

            Layout.fillWidth: true
            spacing: 0

            LabelWithToolTip {
                id: mainLabel

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBottom | Qt.AlignLeft

                text: album
                level: 2
            }

            LabelWithToolTip {
                id: authorLabel

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop | Qt.AlignLeft

                text: albumArtist
                type: Kirigami.Heading.Type.Secondary
            }
        }
    }
}
