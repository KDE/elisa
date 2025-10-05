/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

ScrollView {
    id: scrollView

    // The index of the view being displayed, not the one that is "highlighted" by the ListView
    required property int displayedViewIndex

    property alias model: viewModeView.model

    property alias viewIndex: viewModeView.currentIndex
    readonly property int wideWidth: Kirigami.Units.gridUnit * 12
    readonly property real iconsOnlyMinWidth: viewModeView.iconsOnlyWidth + (ScrollBar.vertical.visible ? ScrollBar.vertical.implicitWidth : 0)
    readonly property real iconsOnlyMaxWidth: viewModeView.iconsOnlyWidth + ScrollBar.vertical.implicitWidth

    signal switchView(int viewIndex)

    // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    contentItem: ListView {
        id: viewModeView

        readonly property int iconsOnlyWidth: Kirigami.Units.iconSizes.smallMedium + 2 * Kirigami.Units.largeSpacing

        reuseItems: true
        clip: true
        focus: true
        activeFocusOnTab: true
        keyNavigationEnabled: true
        interactive: true

        delegate: ItemDelegate {
            id: delegate

            required property int index
            required property string title
            required property url image

            readonly property bool usingThemeIcon: delegate.image.toString().startsWith("image://icon/")

            width: viewModeView.width

            // We need to pass theme icons (i.e. not cover images) by icon.name so that Kirigami.Icon
            // applies the correct color when the delegate is highlighted.
            // HACK: the images provided by the model are in the form "image://icon/view-media-genre"
            // remove the "image://icon/" in order to use icons.
            icon.name: usingThemeIcon ? delegate.image.toString().substring(13) : ""
            icon.source: image
            text: title
            highlighted: index === scrollView.displayedViewIndex

            // Prevent icon recoloring for styles that don't set `icon.color: "transparent"` by default
            // otherwise it applies a single-color mask above the entire album cover image
            Binding {
                when: !delegate.usingThemeIcon
                delegate.icon.color: "transparent"
            }

            onClicked: {
                scrollView.switchView(index)
                forceActiveFocus()
            }
        }

        section.property: 'entryCategory'
        section.delegate: Loader {
            id: sectionDelegateLoader

            required property string section

            active: section !== "default" && opacity > 0
            height: (item as Item)?.implicitHeight ?? 0
            sourceComponent: Kirigami.ListSectionHeader {
                text: sectionDelegateLoader.section
                width: viewModeView.width
            }
            opacity: scrollView.width > scrollView.iconsOnlyMaxWidth ? 1 : 0
            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.InOutQuad
                    duration: Kirigami.Units.longDuration
                }
            }
        }
    }
}
