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

    ListView {
        id: viewModeView

        anchors.fill: parent

        Accessible.role: Accessible.List

        focus: true

        activeFocusOnTab: true
        keyNavigationEnabled: true
        interactive: true

        ScrollBar.vertical: ScrollBar {
            id: scrollBar

            anchors.topMargin: Kirigami.Units.largeSpacing * 2

            policy: ScrollBar.AlwaysOn
            active: true
            interactive: true
        }

        boundsBehavior: Flickable.StopAtBounds
        clip: true

        ScrollHelper {
            id: scrollHelper
            flickable: viewModeView
            anchors.fill: viewModeView
        }

        property bool ignoreCurrentItemChanges: false
        property int childWidth: scrollBar.visible ? (!LayoutMirroring.enabled ? width - scrollBar.width : width) : width

        model: DelegateModel {
            id: pageDelegateModel

            delegate: ViewSelectorDelegate {
                id: entry

                height: Kirigami.Units.iconSizes.smallMedium + 3 * Kirigami.Units.smallSpacing
                width: viewModeView.childWidth

                focus: true

                isSelected: viewModeView.currentIndex === index

                colorizeIcon: model.useColorOverlay
                image: model.image
                title: model.display
                secondTitle: model.secondTitle
                useSecondTitle: model.useSecondTitle
                databaseId: model.databaseId

                onClicked: {
                    viewModeView.currentIndex = index
                    entry.forceActiveFocus()
                }
            }
        }

        section.property: 'entryCategory'
        section.delegate: Kirigami.ListSectionHeader {
            label: (section != 'default' ? section : '')
            height: if (section == 'default') 0
            width: viewModeView.childWidth
        }

        onCurrentItemChanged: if (!ignoreCurrentItemChanges) switchView(currentIndex)
    }

    Connections {
        target: ElisaApplication

        function onInitializationDone() {
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
