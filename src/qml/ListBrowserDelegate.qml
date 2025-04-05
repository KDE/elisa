/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>
   SPDX-FileCopyrightText: 2021 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import QtQuick.Effects as FX
import org.kde.kirigami 2.19 as Kirigami
import org.kde.elisa

AbstractBrowserDelegate {
    id: listEntry

    required property int index
    readonly property bool isAlternateColor: index % 2 === 1

    property bool hideActions: false

    height: elisaTheme.listDelegateHeight

    Rectangle {
        id: rowRoot

        anchors.fill: parent
        z: 1

        color: listEntry.isAlternateColor ? myPalette.alternateBase : myPalette.base
    }

    MouseArea {
        id: hoverArea

        anchors.fill: parent
        z: 2

        hoverEnabled: true
        // fix mousearea from stealing swipes from flickable
        // propagateComposedEvents: false
        // onReleased: {
        //     if (!propagateComposedEvents) {
        //         propagateComposedEvents = true
        //     }
        // }
        acceptedButtons: Qt.LeftButton

        onClicked: listEntry.hasChildren ? open() : enqueue()

        RowLayout {
            anchors {
                fill: parent
                leftMargin: Kirigami.Units.largeSpacing
                rightMargin: Kirigami.Units.largeSpacing
            }

            spacing: Kirigami.Units.largeSpacing

            Loader {
                active: listEntry.delegateLoaded

                // mobile delegate needs more margins
                Layout.preferredWidth: listEntry.height - Kirigami.Units.smallSpacing * (Kirigami.Settings.isMobile ? 2 : 1)
                Layout.preferredHeight: listEntry.height - Kirigami.Units.smallSpacing * (Kirigami.Settings.isMobile ? 2 : 1)

                Layout.alignment: Qt.AlignCenter

                sourceComponent: ImageWithFallback {
                    id: coverImageElement

                    sourceSize.width: (listEntry.height - Kirigami.Units.smallSpacing * (Kirigami.Settings.isMobile ? 2 : 1)) * Screen.devicePixelRatio
                    sourceSize.height: (listEntry.height - Kirigami.Units.smallSpacing * (Kirigami.Settings.isMobile ? 2 : 1)) * Screen.devicePixelRatio
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
                        shadowColor: myPalette.shadow
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
                }

                // Secondary label
                Label {
                    Layout.fillWidth: true

                    visible: listEntry.displaySecondaryText && text.length > 0
                    opacity: 0.6

                    text: listEntry.secondaryText
                    horizontalAlignment: Text.AlignLeft
                    textFormat: Text.PlainText
                    elide: Text.ElideRight
                    font: Kirigami.Theme.smallFont
                }
            }

            // hover actions (for desktop)
            Loader {
                active: !listEntry.hideActions && !Kirigami.Settings.isMobile && (hoverArea.containsMouse || listEntry.activeFocus) && !listEntry.editingRating
                visible: active

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                z: 1

                sourceComponent: Row {
                    anchors.centerIn: parent

                    Repeater {
                        model: listEntry.actions

                        delegate: FlatButtonWithToolTip {
                            required property Kirigami.Action modelData

                            width: elisaTheme.listDelegateSingleLineHeight
                            height: elisaTheme.listDelegateSingleLineHeight
                            action: modelData
                            visible: modelData.visible
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
                Layout.preferredHeight: elisaTheme.listDelegateSingleLineHeight - Kirigami.Units.smallSpacing * 2
            }
        }
    }

    states: [
        State {
            name: 'notSelected'
            when: !listEntry.activeFocus && !hoverArea.containsMouse && !listEntry.isSelected
            PropertyChanges {
                target: rowRoot
                color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)
            }
            PropertyChanges {
                target: rowRoot
                opacity: 1
            }
        },
        State {
            name: 'hovered'
            when: !listEntry.activeFocus && hoverArea.containsMouse
            PropertyChanges {
                target: rowRoot
                color: myPalette.highlight
            }
            PropertyChanges {
                target: rowRoot
                opacity: 0.2
            }
        },
        State {
            name: 'selected'
            when: !listEntry.activeFocus && listEntry.isSelected
            PropertyChanges {
                target: rowRoot
                color: myPalette.mid
            }
            PropertyChanges {
                target: rowRoot
                opacity: 1.
            }
        },
        State {
            name: 'focused'
            when: listEntry.activeFocus
            PropertyChanges {
                target: rowRoot
                color: myPalette.highlight
            }
            PropertyChanges {
                target: rowRoot
                opacity: 0.6
            }
        }
    ]
}
