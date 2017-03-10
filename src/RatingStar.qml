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

RowLayout {
    property int starSize
    property int starRating

    spacing: 0

    Item {
        Layout.preferredHeight: starSize
        Layout.preferredWidth: starSize
        Layout.maximumHeight: starSize
        Layout.maximumWidth: starSize
        Layout.minimumHeight: starSize
        Layout.minimumWidth: starSize

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: starSize * 1.5
            sourceSize.height: starSize * 1.5
            fillMode: Image.PreserveAspectFit

            source: if (starRating >= 1)
                        'image://icon/rating'
                    else
                        'image://icon/draw-star'
            opacity: if (starRating >= 1)
                        1
                    else
                        0.1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: rating = 1
        }
    }
    Item {
        Layout.preferredHeight: starSize
        Layout.preferredWidth: starSize
        Layout.maximumHeight: starSize
        Layout.maximumWidth: starSize
        Layout.minimumHeight: starSize
        Layout.minimumWidth: starSize

        Layout.leftMargin: starSize * 0.5

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: starSize * 1.5
            sourceSize.height: starSize * 1.5
            fillMode: Image.PreserveAspectFit

            source: if (starRating >= 2)
                        'image://icon/rating'
                    else
                        'image://icon/draw-star'
            opacity: if (starRating >= 2)
                        1
                    else
                        0.1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: rating = 1
        }
    }
    Item {
        Layout.preferredHeight: starSize
        Layout.preferredWidth: starSize
        Layout.maximumHeight: starSize
        Layout.maximumWidth: starSize
        Layout.minimumHeight: starSize
        Layout.minimumWidth: starSize

        Layout.leftMargin: starSize * 0.5

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: starSize * 1.5
            sourceSize.height: starSize * 1.5
            fillMode: Image.PreserveAspectFit

            source: if (starRating >= 3)
                        'image://icon/rating'
                    else
                        'image://icon/draw-star'
            opacity: if (starRating >= 3)
                        1
                    else
                        0.1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: rating = 1
        }
    }
    Item {
        Layout.preferredHeight: starSize
        Layout.preferredWidth: starSize
        Layout.maximumHeight: starSize
        Layout.maximumWidth: starSize
        Layout.minimumHeight: starSize
        Layout.minimumWidth: starSize

        Layout.leftMargin: starSize * 0.5

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: starSize * 1.5
            sourceSize.height: starSize * 1.5
            fillMode: Image.PreserveAspectFit

            source: if (starRating >= 4)
                        'image://icon/rating'
                    else
                        'image://icon/draw-star'
            opacity: if (starRating >= 4)
                        1
                    else
                        0.1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: rating = 1
        }
    }
    Item {
        Layout.preferredHeight: starSize
        Layout.preferredWidth: starSize
        Layout.maximumHeight: starSize
        Layout.maximumWidth: starSize
        Layout.minimumHeight: starSize
        Layout.minimumWidth: starSize

        Layout.leftMargin: starSize * 0.5
        Layout.rightMargin: starSize * 0.5

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: starSize * 1.5
            sourceSize.height: starSize * 1.5
            fillMode: Image.PreserveAspectFit

            source: if (starRating === 5)
                        'image://icon/rating'
                    else
                        'image://icon/draw-star'
            opacity: if (starRating === 5)
                        1
                    else
                        0.1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: rating = 1
        }
    }
}

