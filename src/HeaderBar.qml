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

    Image {
        id: background
        source: (image ? image : Qt.resolvedUrl('background.jpg'))

        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop

        sourceSize.width: parent.width

        layer.enabled: true
        layer.effect: Desaturate {
            cached: true

            desaturation: -0.2

            layer.enabled: true
            layer.effect: BrightnessContrast {
                cached: true

                brightness: -0.2
                contrast: -0.1

                layer.enabled: true
                layer.effect: GaussianBlur {
                    cached: true

                    radius: 32
                    deviation: 12
                    samples: 65
                }
            }
        }
    }

    LabelWithToolTip {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: elisaTheme.layoutVerticalMargin
        anchors.rightMargin: elisaTheme.layoutVerticalMargin

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
            Layout.fillWidth: true
            Layout.fillHeight: true

            Item {
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                Layout.preferredHeight: headerBar.height * 0.9
                Layout.minimumHeight: headerBar.height * 0.9
                Layout.maximumHeight: headerBar.height * 0.9
                Layout.preferredWidth: headerBar.height * 0.9
                Layout.minimumWidth: headerBar.height * 0.9
                Layout.maximumWidth: headerBar.height * 0.9
                Layout.leftMargin: headerBar.width * 0.15

                Image {
                    id: mainIcon

                    anchors.fill: parent

                    source: (image ? image : Qt.resolvedUrl(elisaTheme.albumCover))

                    sourceSize {
                        width: headerBar.height * 0.9
                        height: headerBar.height * 0.9
                    }

                    fillMode: Image.PreserveAspectFit

                    layer.enabled: true
                    layer.effect: DropShadow {
                        horizontalOffset: elisaTheme.shadowOffset
                        verticalOffset: elisaTheme.shadowOffset

                        radius: 5.0
                        samples: 11

                        color: myPalette.shadow
                    }
                }
            }

            ColumnLayout {
                spacing: 0

                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                Layout.leftMargin: elisaTheme.layoutHorizontalMargin
                Layout.fillWidth: true
                Layout.fillHeight: true

                Item {
                    Layout.fillHeight: true
                }

                LabelWithToolTip {
                    id: mainLabel
                    text: title
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                    elide: Text.ElideRight
                    color: myPalette.highlightedText
                    font.pixelSize: font.pixelSize * 2
                    font.bold: true

                    layer.effect: Glow {
                        cached: true

                        color: myPalette.shadow

                        radius: 4.0
                        samples: 9
                    }

                }

                Item {
                    Layout.fillHeight: true
                }

                LabelWithToolTip {
                    id: authorLabel
                    text: artist
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                    elide: Text.ElideRight
                    color: myPalette.highlightedText
                    font.pixelSize: font.pixelSize * 1.5

                    layer.effect: Glow {
                        cached: true

                        color: myPalette.shadow

                        radius: 4.0
                        samples: 9
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                LabelWithToolTip {
                    id: albumLabel
                    text: album
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                    elide: Text.ElideRight
                    color: myPalette.highlightedText
                    font.weight: Font.Light

                    layer.effect: Glow {
                        cached: true

                        color: myPalette.shadow

                        radius: 4.0
                        samples: 9
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                RatingStar {
                    id: mainRating
                    visible: ratingVisible
                    starSize: elisaTheme.ratingStarSize
                    starRating: trackRating
                    Layout.alignment: Qt.AlignLeft
                }

                Item {
                    Layout.fillHeight: true
                }
            }

            LabelWithToolTip {
                id: remainingTracksLabel
                text: i18np("1 track remaining", "%1 tracks remaining", tracksCount)
                Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                Layout.bottomMargin: elisaTheme.layoutVerticalMargin
                Layout.leftMargin: elisaTheme.layoutHorizontalMargin
                Layout.rightMargin: elisaTheme.layoutHorizontalMargin
                elide: Text.ElideRight
                visible: tracksCount > 0
                color: myPalette.highlightedText
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
