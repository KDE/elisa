/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import org.kde.kirigami 2.5 as Kirigami

import org.kde.elisa 1.0

import QtQuick 2.0

Rectangle {
    id: background

    property var headerData
    property string album: headerData[0]
    property string albumArtist: headerData[1]
    property url imageUrl: headerData[2]
    property alias backgroundColor: background.color

    implicitHeight: contentLayout.implicitHeight

    color: myPalette.midlight

    RowLayout {
        id: contentLayout

        width: parent.width
        spacing: Kirigami.Units.smallSpacing

        ImageWithFallback {
            Layout.preferredWidth: height
            Layout.fillHeight: true
            Layout.margins: Kirigami.Units.largeSpacing

            source: imageUrl
            fallback: elisaTheme.defaultAlbumImage

            sourceSize.width: height
            sourceSize.height: height

            fillMode: Image.PreserveAspectFit
            asynchronous: true
        }

        ColumnLayout {
            id: albumHeaderTextColumn

            Layout.fillWidth: true
            Layout.preferredHeight: elisaTheme.sectionHeight

            Layout.leftMargin: !LayoutMirroring.enabled ? - Kirigami.Units.smallSpacing : 0
            Layout.rightMargin: LayoutMirroring.enabled ? - Kirigami.Units.smallSpacing : 0
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.bottomMargin: Kirigami.Units.smallSpacing

            spacing: Kirigami.Units.smallSpacing

            LabelWithToolTip {
                id: mainLabel

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBottom | Qt.AlignLeft

                text: album
                level: 2
                font.weight: Font.Bold

                elide: Text.ElideRight
                wrapMode: Text.WordWrap
                maximumLineCount: 2
            }

            LabelWithToolTip {
                id: authorLabel

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop | Qt.AlignLeft

                text: albumArtist

                elide: Text.ElideRight
                wrapMode: Text.WordWrap
                maximumLineCount: 3 - mainLabel.lineCount
            }
        }
    }
}
