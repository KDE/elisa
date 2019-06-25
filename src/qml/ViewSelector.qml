/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQml.Models 2.2
import QtGraphicalEffects 1.0

import org.kde.elisa 1.0

FocusScope {
    id: rootFocusScope

    readonly property alias currentIndex: viewModeView.currentIndex
    property double textOpacity
    property alias model: pageDelegateModel.model

    signal switchView(var viewType)

    function setCurrentIndex(index)
    {
        viewModeView.ignoreCurrentItemChanges = true
        viewModeView.currentIndex = index
        viewModeView.ignoreCurrentItemChanges = false
    }

    implicitWidth: elisaTheme.dp(225)

    ScrollView {
        focus: true

        anchors.fill: parent
        z: 2

        clip: true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        ListView {
            id: viewModeView

            Accessible.role: Accessible.List

            focus: true
            activeFocusOnTab: true
            keyNavigationEnabled: true

            property bool ignoreCurrentItemChanges: false

            z: 2

            anchors.topMargin: elisaTheme.layoutHorizontalMargin * 2

            model: DelegateModel {
                id: pageDelegateModel

                delegate: ViewSelectorDelegate {
                    id: entry

                    height: Math.round(elisaTheme.viewSelectorDelegateHeight * 1.4)
                    width: viewModeView.width

                    focus: true

                    isSelected: viewModeView.currentIndex === index

                    onClicked: {
                        viewModeView.currentIndex = index
                        entry.forceActiveFocus()
                    }
                }
            }

            footer: MouseArea {
                width: viewModeView.width
                height: viewModeView.height - y

                acceptedButtons: Qt.LeftButton

                onClicked:
                {
                    rootFocusScope.focus = true
                }
            }

            onCurrentItemChanged: if (!ignoreCurrentItemChanges) switchView(currentItem.viewType)
        }
    }

    Connections {
        target: elisa

        onInitializationDone: {
            viewModeView.currentIndex = 3
        }
    }

    Behavior on implicitWidth {
        NumberAnimation {
            duration: 150
        }
    }

    Behavior on width {
        NumberAnimation {
            duration: 150
        }
    }

    states: [
        State {
            name: 'iconsAndText'
            when: mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold
            PropertyChanges {
                target: rootFocusScope
                textOpacity: 1
                implicitWidth: elisaTheme.dp(225)
            }
        },
        State {
            name: 'iconsOnly'
            when: mainWindow.width < elisaTheme.viewSelectorSmallSizeThreshold
            PropertyChanges {
                target: rootFocusScope
                textOpacity: 0
                implicitWidth: elisaTheme.viewSelectorDelegateHeight + 2 * elisaTheme.layoutHorizontalMargin
            }
        }
    ]
}
