/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>
   SPDX-FileCopyrightText: 2021 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Effects as FX
import org.kde.kirigami as Kirigami
import org.kde.elisa

AbstractBrowserDelegate {
    id: listEntry

    required property int index

    property bool hideActions: false

    readonly property bool hasActiveFocus: activeFocus || focusScope.activeFocus

    readonly property color textColor: highlighted || down ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
    readonly property color iconColor: textColor

    Kirigami.Theme.useAlternateBackgroundColor: index % 2 === 1

    onClicked: listEntry.hasChildren ? listEntry.open() : listEntry.enqueue()

    contentItem: FocusScope {
        id: focusScope

        implicitHeight: childrenRect.height

        RowLayout {
            width: parent.width
            height: implicitHeight

            spacing: Kirigami.Units.largeSpacing

            Loader {
                active: listEntry.delegateLoaded

                Layout.alignment: Qt.AlignCenter

                Layout.preferredWidth: Theme.listDelegateIconHeight
                Layout.preferredHeight: Theme.listDelegateIconHeight

                sourceComponent: ImageWithFallback {
                    id: coverImageElement

                    sourceSize.width: Theme.listDelegateIconHeight * Screen.devicePixelRatio
                    sourceSize.height: Theme.listDelegateIconHeight * Screen.devicePixelRatio
                    fillMode: Image.PreserveAspectFit
                    smooth: true

                    source: listEntry.imageUrl
                    fallback: listEntry.imageFallbackUrl

                    asynchronous: true

                    layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software && !usingFallback && !Kirigami.Settings.isMobile // disable drop shadow for mobile

                    layer.effect: FX.MultiEffect {
                        source: coverImageElement

                        blurMax: 8
                        shadowEnabled: true
                        shadowColor: palette.shadow
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                spacing: Kirigami.Units.smallSpacing / 2

                // Main label
                Label {
                    Layout.fillWidth: true

                    text: listEntry.mainText
                    textFormat: Text.PlainText
                    elide: Text.ElideRight
                    color: listEntry.textColor
                }

                // Secondary label
                Label {
                    Layout.fillWidth: true

                    visible: listEntry.displaySecondaryText && text.length > 0
                    opacity: Theme.subtitleOpacity

                    text: listEntry.secondaryText
                    horizontalAlignment: Text.AlignLeft
                    textFormat: Text.PlainText
                    elide: Text.ElideRight
                    font: Kirigami.Theme.smallFont
                    color: listEntry.textColor
                }
            }

            // hover actions (for desktop)
            Loader {
                active: !listEntry.hideActions && !Kirigami.Settings.isMobile && (listEntry.hovered || listEntry.hasActiveFocus) && !listEntry.editingRating
                visible: active

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                z: 1

                sourceComponent: Row {
                    anchors.centerIn: parent

                    Repeater {
                        model: listEntry.actions

                        delegate: FlatButtonWithToolTip {
                            required property Kirigami.Action modelData

                            width: Theme.listDelegateButtonHeight
                            height: width
                            action: modelData
                            visible: modelData.visible
                            icon.color: listEntry.iconColor
                        }
                    }
                }
            }

            // mobile context menu button
            FlatButtonWithToolTip {
                action: listEntry.mobileContextMenuAction
                visible: !listEntry.hideActions && listEntry.mobileContextMenuAction.visible
                scale: LayoutMirroring.enabled ? -1 : 1
                Layout.alignment: Qt.AlignVCenter
                Layout.maximumHeight: parent.height
                Layout.preferredWidth: height
                Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                icon.color: listEntry.iconColor
            }
        }
    }
}
