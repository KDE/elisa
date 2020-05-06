/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQml.Models 2.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

FocusScope {
    id: rootFocusScope

    readonly property alias currentIndex: viewModeView.currentIndex
    property double textOpacity
    property alias model: pageDelegateModel.model

    signal switchView(int viewIndex)

    function setCurrentIndex(index)
    {
        viewModeView.ignoreCurrentItemChanges = true
        viewModeView.currentIndex = index
        viewModeView.ignoreCurrentItemChanges = false
    }

    implicitWidth: 225

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

            anchors.topMargin: Kirigami.Units.largeSpacing * 2

            model: DelegateModel {
                id: pageDelegateModel

                delegate: ViewSelectorDelegate {
                    id: entry

                    height: Kirigami.Units.iconSizes.smallMedium + 3 * Kirigami.Units.smallSpacing
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

            onCurrentItemChanged: if (!ignoreCurrentItemChanges) switchView(currentIndex)
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
            easing.type: Easing.InOutQuad
            duration: Kirigami.Units.longDuration
        }
    }

    Behavior on width {
        NumberAnimation {
            easing.type: Easing.InOutQuad
            duration: Kirigami.Units.longDuration
        }
    }

    states: [
        State {
            name: 'iconsAndText'
            when: mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold
            PropertyChanges {
                target: rootFocusScope
                textOpacity: 1
                implicitWidth: 225
            }
        },
        State {
            name: 'iconsOnly'
            when: mainWindow.width < elisaTheme.viewSelectorSmallSizeThreshold
            PropertyChanges {
                target: rootFocusScope
                textOpacity: 0
                implicitWidth: Kirigami.Units.iconSizes.smallMedium + 2 * Kirigami.Units.largeSpacing

            }
        }
    ]
}
