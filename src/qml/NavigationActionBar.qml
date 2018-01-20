/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

import QtQml 2.2
import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Window 2.0

Item {
    id: navigationBar

    property string mainTitle
    property string secondaryTitle
    property url image
    property bool allowArtistNavigation: false

    signal enqueue();
    signal replaceAndPlay();
    signal goBack();
    signal showArtist();

    Action {
        id: goPreviousAction
        text: i18nc("navigate back in the views stack", "Back")
        iconName: (Qt.application.layoutDirection == Qt.RightToLeft) ? "go-next" : "go-previous"
        onTriggered: goBack()
    }

    RowLayout {
        anchors.fill: parent

        anchors.margins: {
            top: elisaTheme.layoutVerticalMargin
            bottom: elisaTheme.layoutVerticalMargin
        }

        spacing: 0

        ToolButton {
            action: goPreviousAction
            Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
        }

        Image {
            id: mainIcon
            source: image

            asynchronous: true

            sourceSize.height: elisaTheme.coverImageSize / 2
            sourceSize.width: elisaTheme.coverImageSize / 2

            fillMode: Image.PreserveAspectFit

            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

            Layout.preferredHeight: elisaTheme.coverImageSize / 2
            Layout.minimumHeight: elisaTheme.coverImageSize / 2
            Layout.maximumHeight: elisaTheme.coverImageSize / 2
            Layout.preferredWidth: elisaTheme.coverImageSize / 2
            Layout.minimumWidth: elisaTheme.coverImageSize / 2
            Layout.maximumWidth: elisaTheme.coverImageSize / 2
            Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
        }

        ColumnLayout {
            Layout.fillHeight: true

            spacing: 0

            Layout.fillWidth: true
            Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0

            TextMetrics {
                id: albumTextSize
                text: albumLabel.text
                font.pixelSize: albumLabel.font.pixelSize
                font.bold: albumLabel.font.bold
            }

            LabelWithToolTip {
                id: albumLabel

                text: secondaryTitle

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                elide: Text.ElideRight

                color: myPalette.text

                font {
                    pixelSize: elisaTheme.defaultFontPixelSize * 1.5
                }

                visible: secondaryTitle !== ""
            }

            TextMetrics {
                id: authorTextSize
                text: authorLabel.text
                font.pixelSize: authorLabel.font.pixelSize
                font.bold: authorLabel.font.bold
            }

            LabelWithToolTip {
                id: authorLabel

                text: mainTitle

                color: myPalette.text

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                font {
                    pixelSize: (secondaryTitle !== "" ? elisaTheme.defaultFontPixelSize : elisaTheme.defaultFontPixelSize * 1.5)
                }

                elide: Text.ElideRight
            }

            Item {
                id: emptyBottomFiller

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 0

                Button {
                    text: i18nc("Add current list to playlist", "Enqueue")
                    iconName: "media-track-add-amarok"

                    onClicked: enqueue()

                    Layout.leftMargin: 0
                    Layout.rightMargin: 0
                }

                Button {
                    text: i18nc("Clear playlist and play", "Replace and Play")
                    tooltip: i18nc("Clear playlist and add current list to it", "Replace PlayList and Play Now")
                    iconName: "media-playback-start"

                    onClicked: replaceAndPlay()

                    Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                    Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                }

                Button {
                    id: showArtistButton

                    visible: allowArtistNavigation
                    text: i18nc("Button to navigate to the artist of the album", "Display Artist")
                    iconName: "view-media-artist"

                    onClicked: showArtist()

                    Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                    Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                }
            }
        }
    }
}
