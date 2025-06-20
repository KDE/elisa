/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2021 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtQuick.Effects as FX
import org.kde.kirigami 2.19 as Kirigami
import org.kde.elisa

AbstractBrowserDelegate {
    id: gridEntry

    property color stateIndicatorColor: {
        if (gridEntry.activeFocus || hoverHandle.pressed || hoverHandle.containsMouse) {
            return palette.highlight;
        } else if (gridEntry.isSelected && !Kirigami.Settings.isMobile) {
            return palette.mid;
        } else {
            return "transparent";
        }
    }

    property real stateIndicatorOpacity: {
        if ((!Kirigami.Settings.isMobile && gridEntry.activeFocus) ||
            (!Kirigami.Settings.isMobile && gridEntry.isSelected) || hoverHandle.pressed || hoverHandle.containsMouse) {
            return 0.3;
        } else {
            return 0;
        }
    }

    Item {
        id: parentItem
        anchors.fill: parent
        // mobile uses more spacing between delegates
        anchors.margins: Kirigami.Settings.isMobile ? Kirigami.Units.largeSpacing : 0

        // highlight colour
        Rectangle {
            id: stateIndicator

            z: Kirigami.Settings.isMobile ? 1 : 0 // on desktop, we want hover actions to be above highlight

            anchors.fill: parent
            // expand margins of highlight box on mobile, so that it doesn't look like it's clipping the text
            anchors.leftMargin: Kirigami.Settings.isMobile ? -Kirigami.Units.smallSpacing : 0
            anchors.rightMargin: Kirigami.Settings.isMobile ? -Kirigami.Units.smallSpacing : 0
            anchors.bottomMargin: Kirigami.Settings.isMobile ? -Kirigami.Units.smallSpacing : 0
            color: stateIndicatorColor
            opacity: stateIndicatorOpacity
            radius: Kirigami.Units.cornerRadius
        }

        // click handler
        MouseArea {
            id: hoverHandle

            anchors.fill: parent
            // fix mousearea from stealing swipes from flickable
            propagateComposedEvents: false
            onReleased: {
                if (!propagateComposedEvents) {
                    propagateComposedEvents = true
                }
            }

            hoverEnabled: true

            onClicked: hasChildren ? open() : enqueue()

            TextMetrics {
                id: mainLabelSize
                font: mainLabel.font
                text: mainLabel.text
            }

            component CoverImage: ImageWithFallback {
                id: coverImage
                property var imageSource

                sourceSize.width: width * Screen.devicePixelRatio
                sourceSize.height: height * Screen.devicePixelRatio
                fillMode: Image.PreserveAspectFit

                source: imageSource ? imageSource : ""
                fallback: gridEntry.imageFallbackUrl

                asynchronous: true

                layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software && !coverImage.usingFallback && !Kirigami.Settings.isMobile // don't use drop shadow on mobile
                layer.effect: FX.MultiEffect {
                    source: coverImage

                    shadowBlur: 0.8
                    shadowEnabled: true
                    shadowColor: palette.shadow
                }
            }

            Component {
                id: quartersCover
                Grid {
                    rows: 2
                    columns: 2
                    component QuarterImage: CoverImage {
                        width: parent.width / 2
                        height: parent.height / 2
                    }

                    QuarterImage {imageSource: gridEntry.multipleImageUrls[0]}
                    QuarterImage {imageSource: gridEntry.multipleImageUrls[1]}
                    QuarterImage {imageSource: gridEntry.multipleImageUrls[2]}
                    QuarterImage {imageSource: gridEntry.multipleImageUrls[3]}
                }
            }

            Component {
                id: singleCover
                CoverImage {
                    width: parent.width
                    height: parent.height

                    imageSource: gridEntry.imageUrl
                }
            }

            // cover image
            Loader {
                id: coverImageLoader
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.largeSpacing
                width: gridEntry.width - 2 * Kirigami.Units.largeSpacing
                height: gridEntry.width - 2 * Kirigami.Units.largeSpacing

                active: gridEntry.delegateLoaded

                sourceComponent: gridEntry.multipleImageUrls && gridEntry.multipleImageUrls.length == 4
                                  ? quartersCover : singleCover
            }

            // ========== desktop hover actions ==========
            Loader {
                id: hoverLoader
                visible: !Kirigami.Settings.isMobile
                active: gridEntry.activeFocus || hoverHandle.containsMouse

                anchors {
                    bottom: parent.bottom
                    bottomMargin: labels.height
                    left: parent.left
                    leftMargin: 2 + Kirigami.Units.largeSpacing
                }

                opacity: gridEntry.activeFocus || hoverHandle.containsMouse

                sourceComponent: Row {
                    spacing: 2

                    Repeater {
                        model: gridEntry.actions

                        delegate: ButtonWithToolTip {
                            required property Kirigami.Action modelData

                            action: modelData
                            visible: modelData.visible
                            hoverEnabled: true
                            display: AbstractButton.IconOnly
                        }
                    }
                }
            }


            // labels
            RowLayout {
                id: labels
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.top: coverImageLoader.bottom

                ColumnLayout {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.fillWidth: true
                    spacing: 0

                    LabelWithToolTip {
                        id: mainLabel
                        text: gridEntry.mainText

                        level: Kirigami.Settings.isMobile ? 6 : 4

                        color: palette.text

                        // FIXME: Center-aligned text looks better overall, but
                        // sometimes results in font kerning issues
                        // See https://bugreports.qt.io/browse/QTBUG-49646
                        horizontalAlignment: Kirigami.Settings.isMobile ? Text.AlignLeft : Text.AlignHCenter

                        Layout.fillWidth: true
                        Layout.maximumHeight: gridEntry.displaySecondaryText ? mainLabelSize.boundingRect.height : mainLabelSize.boundingRect.height * 2
                        Layout.alignment: Kirigami.Settings.isMobile ? Qt.AlignLeft : Qt.AlignVCenter
                        Layout.leftMargin: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.largeSpacing
                        Layout.rightMargin: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.largeSpacing

                        wrapMode: !Kirigami.Settings.isMobile && gridEntry.displaySecondaryText ? Label.NoWrap : Label.Wrap
                        maximumLineCount: Kirigami.Settings.isMobile ? 1 : 2
                        elide: Text.ElideRight
                    }

                    LabelWithToolTip {
                        id: secondaryLabel
                        visible: gridEntry.displaySecondaryText
                        text: gridEntry.secondaryText

                        opacity: 0.6
                        color: palette.text

                        // FIXME: Center-aligned text looks better overall, but
                        // sometimes results in font kerning issues
                        // See https://bugreports.qt.io/browse/QTBUG-49646
                        horizontalAlignment: Kirigami.Settings.isMobile ? Text.AlignLeft : Text.AlignHCenter

                        Layout.fillWidth: true
                        Layout.alignment: Kirigami.Settings.isMobile ? Qt.AlignLeft : Qt.AlignVCenter
                        Layout.topMargin: Kirigami.Settings.isMobile ? Kirigami.Units.smallSpacing : 0
                        Layout.leftMargin: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.largeSpacing
                        Layout.rightMargin: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.largeSpacing

                        maximumLineCount: Kirigami.Settings.isMobile ? 1 : -1
                        elide: Text.ElideRight
                        font: Kirigami.Settings.isMobile ? Kirigami.Theme.smallFont : Kirigami.Theme.defaultFont
                    }
                }

                // mobile context menu button
                FlatButtonWithToolTip {
                    action: gridEntry.mobileContextMenuAction
                    visible: gridEntry.mobileContextMenuAction.visible
                    scale: LayoutMirroring.enabled ? -1 : 1
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    Layout.preferredHeight: Math.round(Kirigami.Units.gridUnit * 2.5)
                    Layout.preferredWidth: Math.round(Kirigami.Units.gridUnit * 1.5)
                }
            }
        }
    }
}
