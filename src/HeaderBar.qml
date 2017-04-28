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

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

Item {
    id: headerBar

    property string title
    property string artist
    property string album
    property string image
    property string tracksCount
    property int trackRating
    property bool ratingVisible

    Theme {
        id: elisaTheme
    }

    Image {
        id: background
        source: (image ? image : Qt.resolvedUrl('background.jpg'))


        anchors.margins: -2
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop

        sourceSize.width: parent.width

        visible: false
    }

    Desaturate {
        id: desaturateBackground

        anchors.fill: background
        source: background
        cached: true

        desaturation: -0.2

        visible: false
    }

    BrightnessContrast {
        id: contrastBackground

        anchors.fill: desaturateBackground
        source: desaturateBackground
        cached: true

        brightness: -0.2
        contrast: -0.1

        visible: false
    }

    GaussianBlur {
        anchors.fill: contrastBackground
        source: contrastBackground
        cached: true

        radius: 32
        deviation: 12
        samples: 65

        SystemPalette {
            id: myPalette
            colorGroup: SystemPalette.Active
        }

        Text {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.bottomMargin: Screen.pixelDensity * 1.
            anchors.rightMargin: Screen.pixelDensity * 1.

            text: i18nc("Copyright text shown for an image", "by Edward Betts (Own work) [CC BY-SA 3.0]")
            color: myPalette.highlightedText
            opacity: 0.4

            visible: !image
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            RowLayout {
                spacing: 0

                Layout.alignment: Qt.AlignVCenter
                Layout.preferredHeight: Screen.pixelDensity * 34.
                Layout.minimumHeight: Screen.pixelDensity * 34.
                Layout.maximumHeight: Screen.pixelDensity * 34.
                Layout.fillWidth: true

                Item {
                    Layout.preferredWidth: Screen.pixelDensity * 50.
                    Layout.minimumWidth: Screen.pixelDensity * 50.
                    Layout.maximumWidth: Screen.pixelDensity * 50.
                    Layout.fillHeight: true
                }

                Image {
                    id: mainIcon
                    source: (image ? image : elisaTheme.albumCover)

                    sourceSize {
                        width: Screen.pixelDensity * 34.
                        height: Screen.pixelDensity * 34.
                    }

                    fillMode: Image.PreserveAspectFit

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    Layout.preferredHeight: Screen.pixelDensity * 34.
                    Layout.minimumHeight: Screen.pixelDensity * 34.
                    Layout.maximumHeight: Screen.pixelDensity * 34.
                    Layout.preferredWidth: Screen.pixelDensity * 34.
                    Layout.minimumWidth: Screen.pixelDensity * 34.
                    Layout.maximumWidth: Screen.pixelDensity * 34.

                    visible: false
                }

                DropShadow {
                    source: mainIcon

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    Layout.preferredHeight: Screen.pixelDensity * 34.
                    Layout.minimumHeight: Screen.pixelDensity * 34.
                    Layout.maximumHeight: Screen.pixelDensity * 34.
                    Layout.preferredWidth: Screen.pixelDensity * 34.
                    Layout.minimumWidth: Screen.pixelDensity * 34.
                    Layout.maximumWidth: Screen.pixelDensity * 34.

                    horizontalOffset: Screen.pixelDensity * 0.02
                    verticalOffset: Screen.pixelDensity * 0.02

                    radius: 5.0
                    samples: 11

                    color: myPalette.shadow
                }

                Item {
                    Layout.preferredWidth: Screen.pixelDensity * 2.
                    Layout.minimumWidth: Screen.pixelDensity * 2.
                    Layout.maximumWidth: Screen.pixelDensity * 2.
                    Layout.fillHeight: true
                }

                ColumnLayout {
                    spacing: 0

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    Layout.preferredWidth: Screen.pixelDensity * 80.
                    Layout.minimumWidth: Screen.pixelDensity * 80.
                    Layout.maximumWidth: Screen.pixelDensity * 80.
                    Layout.fillHeight: true

                    Item {
                        Layout.fillHeight: true
                    }

                    Text {
                        id: mainLabel
                        text: title
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignLeft
                        elide: Text.ElideRight
                        color: myPalette.highlightedText
                        font.pixelSize: 18
                        font.bold: true

                        visible: false
                    }

                    Glow {
                        source: mainLabel

                        Layout.fillWidth: true
                        Layout.preferredHeight: mainLabel.height
                        Layout.alignment: Qt.AlignLeft

                        cached: true

                        color: myPalette.shadow

                        radius: 4.0
                        samples: 9
                    }

                    Item {
                        Layout.fillHeight: true
                    }

                    Text {
                        id: authorLabel
                        text: artist
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignLeft
                        elide: "ElideRight"
                        color: myPalette.highlightedText
                        font.pixelSize: 15

                        visible: false
                    }

                    Glow {
                        source: authorLabel

                        Layout.fillWidth: true
                        Layout.preferredHeight: authorLabel.height
                        Layout.alignment: Qt.AlignLeft

                        cached: true

                        color: myPalette.shadow

                        radius: 4.0
                        samples: 9
                    }

                    Item {
                        Layout.fillHeight: true
                    }

                    Text {
                        id: albumLabel
                        text: album
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignLeft
                        elide: "ElideRight"
                        color: myPalette.highlightedText
                        font.pixelSize: 12
                        font.weight: Font.Light

                        visible: false
                    }

                    Glow {
                        source: albumLabel

                        Layout.fillWidth: true
                        Layout.preferredHeight: albumLabel.height
                        Layout.alignment: Qt.AlignLeft

                        cached: true

                        color: myPalette.shadow

                        radius: 4.0
                        samples: 9
                    }

                    Item {
                        Layout.fillHeight: true
                    }

                    RatingStar {
                        id: mainRating
                        visible: ratingVisible
                        starSize: 20
                        starRating: trackRating
                        Layout.alignment: Qt.AlignLeft
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                Text {
                    id: remainingTracksLabel
                    text: i18np("1 track remaining", "%1 tracks remaining", tracksCount)
                    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                    Layout.bottomMargin: Screen.pixelDensity * 2
                    elide: "ElideRight"
                    visible: tracksCount > 0
                    color: myPalette.highlightedText
                }

                Item {
                    Layout.preferredWidth: Screen.pixelDensity * 40.
                    Layout.minimumWidth: Screen.pixelDensity * 40.
                    Layout.maximumWidth: Screen.pixelDensity * 40.
                    Layout.fillHeight: true
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}
