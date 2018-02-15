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

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import org.kde.elisa 1.0

FocusScope {
    id: contentDirectoryRoot

    property MediaPlayList playListModel
    property alias firstPage: listingView.initialItem
    property alias stackView: listingView

    function goBack() {
        if (listingView.depth > 1) {
            listingView.pop()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        StackView {
            id: listingView

            Layout.fillHeight: true
            Layout.fillWidth: true

            focus: true

            pushEnter:  Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to: 1
                }
            }

            pushExit:  Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 1
                    to: 0
                }
            }

            popEnter:  Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to: 1
                }
            }

            popExit:  Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 1
                    to: 0
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.BackButton
        onClicked: goBack()
    }
}
