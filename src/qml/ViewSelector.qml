/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQml.Models 2.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.10 as Kirigami
import org.kde.elisa 1.0

ScrollView {
    id: scrollView

    readonly property alias currentIndex: viewModeView.currentIndex
    property alias model: viewModeView.model

    signal switchView(int viewIndex)

    function setCurrentIndex(index)
    {
        viewModeView.ignoreCurrentItemChanges = true
        viewModeView.currentIndex = index
        viewModeView.ignoreCurrentItemChanges = false
    }

    implicitWidth: 225

    // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    contentItem: ListView {
        id: viewModeView
        property bool ignoreCurrentItemChanges: false

        Accessible.role: Accessible.List

        focus: true
        clip: true
        activeFocusOnTab: true
        keyNavigationEnabled: true
        interactive: true

        delegate: ViewSelectorDelegate {
            id: entry

            height: Kirigami.Units.iconSizes.smallMedium + 3 * Kirigami.Units.smallSpacing
            width: viewModeView.width

            onClicked: {
                viewModeView.currentIndex = index
                entry.forceActiveFocus()
            }
        }

        section.property: 'entryCategory'
        section.delegate: Loader {
            active: section !== "default"
            sourceComponent: Kirigami.ListSectionHeader {
                label: section
                width: viewModeView.width
            }
        }

        onCurrentItemChanged: if (!ignoreCurrentItemChanges) switchView(currentIndex)
    }

    states: [
        State {
            name: 'iconsAndText'
            when: mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold
            PropertyChanges {
                target: scrollView
                implicitWidth: 225
            }
        },
        State {
            name: 'iconsOnly'
            when: mainWindow.width < elisaTheme.viewSelectorSmallSizeThreshold
            PropertyChanges {
                target: scrollView
                implicitWidth: Kirigami.Units.iconSizes.smallMedium + 2 * Kirigami.Units.largeSpacing
            }
        }
    ]

    transitions: Transition {
        NumberAnimation {
            properties: "implicitWidth"
            easing.type: Easing.InOutQuad
            duration: Kirigami.Units.longDuration
        }
    }
}
