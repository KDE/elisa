/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
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


import QtQuick 2.7
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2

FocusScope {
    id: filterBar

    property string labelText
    property bool showRating: true
    property alias filterText: filterTextInput.text
    property alias filterRating: ratingFilter.starRating

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TextMetrics {
            id: titleHeight
            text: viewTitleHeight.text
            font
            {
                pixelSize: viewTitleHeight.font.pixelSize
                bold: viewTitleHeight.font.bold
            }
        }

        LabelWithToolTip {
            id: viewTitleHeight
            text: labelText

            color: myPalette.text
            font.pixelSize: elisaTheme.defaultFontPixelSize * 2

            Layout.preferredHeight: implicitHeight
            Layout.leftMargin: elisaTheme.layoutHorizontalMargin
            Layout.rightMargin: elisaTheme.layoutHorizontalMargin
            Layout.topMargin: elisaTheme.layoutVerticalMargin
            Layout.bottomMargin: elisaTheme.layoutVerticalMargin
        }

        Item {
            Layout.fillHeight: true
        }

        RowLayout {
            id: filterRow

            spacing: 0

            Layout.fillWidth: true
            Layout.bottomMargin: elisaTheme.layoutVerticalMargin
            Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0

            LabelWithToolTip {
                text: i18nc("before the TextField input of the filter", "Filter: ")

                font.bold: true

                Layout.bottomMargin: 0

                color: myPalette.text
            }

            TextField {
                id: filterTextInput

                horizontalAlignment: TextInput.AlignLeft

                placeholderText: i18nc("Placeholder text in the filter text box", "Filter")

                Layout.bottomMargin: 0
                Layout.preferredWidth: rootElement.width / 2

                Image {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.margins: elisaTheme.filterClearButtonMargin
                    id: clearText
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    visible: parent.text
                    source: Qt.resolvedUrl(elisaTheme.clearIcon)
                    height: parent.height
                    width: parent.height
                    sourceSize.width: parent.height
                    sourceSize.height: parent.height
                    mirror: LayoutMirroring.enabled

                    MouseArea {
                        id: clear
                        anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
                        height: parent.parent.height
                        width: parent.parent.height
                        onClicked: {
                            parent.parent.text = ""
                            parent.parent.forceActiveFocus()
                        }
                    }
                }
            }

            LabelWithToolTip {
                text: i18nc("before the Rating widget input of the filter", "Rating: ")

                visible: showRating

                font.bold: true

                color: myPalette.text

                Layout.bottomMargin: 0
                Layout.leftMargin: !LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin * 2) : 0
                Layout.rightMargin: LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin * 2) : 0
            }

            RatingStar {
                id: ratingFilter

                visible: showRating

                readOnly: false

                starSize: elisaTheme.ratingStarSize

                Layout.bottomMargin: 0
            }
        }
    }
}
