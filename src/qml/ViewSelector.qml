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

    // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    contentItem: ListView {
        id: viewModeView

        Accessible.role: Accessible.List

        focus: true
        clip: true
        activeFocusOnTab: true
        keyNavigationEnabled: true
        interactive: true

        property bool ignoreCurrentItemChanges: false

        model: DelegateModel {
            id: pageDelegateModel

            delegate: ViewSelectorDelegate {
                id: entry

                height: Math.round((Kirigami.Settings.tabletMode ? 1.25 : 1) * (Kirigami.Units.iconSizes.smallMedium + 3 * Kirigami.Units.smallSpacing))
                width: viewModeView.width

                focus: true

                isSelected: viewModeView.currentIndex === index

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
            width: viewModeView.width
        }

        onCurrentItemChanged: if (!ignoreCurrentItemChanges) switchView(currentIndex)
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
                target: scrollView
                textOpacity: 1
                implicitWidth: 225
            }
        },
        State {
            name: 'iconsOnly'
            when: mainWindow.width < elisaTheme.viewSelectorSmallSizeThreshold
            PropertyChanges {
                target: scrollView
                textOpacity: 0
                implicitWidth: Kirigami.Units.iconSizes.smallMedium + 2 * Kirigami.Units.largeSpacing

            }
        }
    ]
}
