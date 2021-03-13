/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2021 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

import "mobile"

FocusScope {
    id: gridEntry

    property url imageUrl
    property url imageFallbackUrl
    property url fileUrl
    property var entryType
    property string mainText
    property string secondaryText
    property var databaseId
    property bool delegateDisplaySecondaryText: true
    property bool isPartial
    property bool isSelected
    property bool hasChildren: true

    signal enqueue()
    signal replaceAndPlay()
    signal open()
    signal selected()

    property bool showDetailsButton: false
    property bool showPlayButton: true
    property bool showEnqueueButton: true

    property color stateIndicatorColor: {
        if (gridEntry.activeFocus || hoverHandle.pressed || hoverHandle.containsMouse) {
            return myPalette.highlight;
        } else if (gridEntry.isSelected && !Kirigami.Settings.isMobile) {
            return myPalette.mid;
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

    Loader {
        id: metadataLoader
        active: false && gridEntry.fileUrl
        onLoaded: item.show()

        sourceComponent: MediaTrackMetadataView {
            fileName: gridEntry.fileUrl ? gridEntry.fileUrl : ''
            showImage: true
            modelType: gridEntry.entryType
            showTrackFileName: true
            showDeleteButton: false
            editableMetadata: false
            canAddMoreMetadata: false

            onRejected: metadataLoader.active = false;
        }
    }

    property bool delegateLoaded: true

    ListView.onPooled: delegateLoaded = false
    ListView.onReused: delegateLoaded = true

    // open mobile context menu
    function openContextMenu() {
        contextMenuLoader.active = true;
        contextMenuLoader.item.open();
    }

    Keys.onReturnPressed: open()
    Keys.onEnterPressed: open()

    Accessible.role: Accessible.ListItem
    Accessible.name: mainText

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
            radius: Kirigami.Settings.isMobile ? Kirigami.Units.smallSpacing : 3
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
            cursorShape: hasChildren ? Qt.PointingHandCursor : Qt.ArrowCursor

            onClicked: hasChildren ? open() : enqueue()

            TextMetrics {
                id: mainLabelSize
                font: mainLabel.font
                text: mainLabel.text
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

                active: gridEntry.delegateLoaded && !isPartial

                sourceComponent: ImageWithFallback {
                    id: coverImage

                    sourceSize.width: parent.width
                    sourceSize.height: parent.height
                    fillMode: Image.PreserveAspectFit

                    source: gridEntry.imageUrl
                    fallback: gridEntry.imageFallbackUrl

                    asynchronous: true

                    layer.enabled: !coverImage.usingFallback && !Kirigami.Settings.isMobile // don't use drop shadow on mobile
                    layer.effect: DropShadow {
                        source: coverImage

                        radius: 10
                        spread: 0.1
                        samples: 21

                        color: myPalette.shadow
                    }
                }
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

                    Button {
                        icon.name: 'document-open-folder'

                        hoverEnabled: true
                        ToolTip.visible: hovered
                        ToolTip.delay: 1000
                        ToolTip.text: i18nc("Show the file for this song in the file manager", "Show in folder")

                        Accessible.role: Accessible.Button
                        Accessible.name: ToolTip.text
                        Accessible.description: ToolTip.text
                        Accessible.onPressAction: clicked()

                        onClicked: {
                            ElisaApplication.showInFolder(gridEntry.fileUrl)
                        }

                        Keys.onReturnPressed: clicked()
                        Keys.onEnterPressed: clicked()

                        visible: showDetailsButton && (trackUrl.toString().substring(0, 7) === 'file://')
                    }
                    Button {
                        id: detailsButton
                        objectName: 'detailsButton'

                        icon.name: 'help-about'

                        hoverEnabled: true
                        ToolTip.visible: hovered
                        ToolTip.delay: 1000
                        ToolTip.text: i18nc("Show track metadata", "View Details")

                        Accessible.role: Accessible.Button
                        Accessible.name: ToolTip.text
                        Accessible.description: ToolTip.text
                        Accessible.onPressAction: clicked()

                        onClicked: {
                            if (metadataLoader.active === false) {
                                metadataLoader.active = true
                            }
                            else {
                                metadataLoader.item.close();
                                metadataLoader.active = false
                            }
                        }

                        Keys.onReturnPressed: clicked()
                        Keys.onEnterPressed: clicked()
                        visible: showDetailsButton
                    }

                    Button {
                        id: replaceAndPlayButton
                        objectName: 'replaceAndPlayButton'

                        icon.name: 'media-playback-start'

                        hoverEnabled: true
                        ToolTip.visible: hovered
                        ToolTip.delay: 1000
                        ToolTip.text: i18nc("Clear play list and add whole container to play list", "Play now, replacing current playlist")

                        Accessible.role: Accessible.Button
                        Accessible.name: ToolTip.text
                        Accessible.description: ToolTip.text
                        Accessible.onPressAction: onClicked

                        onClicked: replaceAndPlay()
                        Keys.onReturnPressed: replaceAndPlay()
                        Keys.onEnterPressed: replaceAndPlay()

                        visible: showPlayButton
                    }

                    Button {
                        id: enqueueButton
                        objectName: 'enqueueButton'

                        icon.name: 'list-add'
                        hoverEnabled: true
                        ToolTip.visible: hovered
                        ToolTip.delay: 1000
                        ToolTip.text: i18nc("Add whole container to play list", "Add to playlist")

                        Accessible.role: Accessible.Button
                        Accessible.name: ToolTip.text
                        Accessible.description: ToolTip.text
                        Accessible.onPressAction: onClicked

                        onClicked: enqueue()
                        Keys.onReturnPressed: enqueue()
                        Keys.onEnterPressed: enqueue()

                        visible: showEnqueueButton
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

                        color: myPalette.text

                        // FIXME: Center-aligned text looks better overall, but
                        // sometimes results in font kerning issues
                        // See https://bugreports.qt.io/browse/QTBUG-49646
                        horizontalAlignment: Kirigami.Settings.isMobile ? Text.AlignLeft : Text.AlignHCenter

                        Layout.fillWidth: true
                        Layout.maximumHeight: delegateDisplaySecondaryText ? mainLabelSize.boundingRect.height : mainLabelSize.boundingRect.height * 2
                        Layout.alignment: Kirigami.Settings.isMobile ? Qt.AlignLeft : Qt.AlignVCenter
                        Layout.leftMargin: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.largeSpacing
                        Layout.rightMargin: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.largeSpacing

                        wrapMode: !Kirigami.Settings.isMobile && delegateDisplaySecondaryText ? Label.NoWrap : Label.Wrap
                        maximumLineCount: Kirigami.Settings.isMobile ? 1 : 2
                        elide: Text.ElideRight
                    }

                    LabelWithToolTip {
                        id: secondaryLabel
                        visible: delegateDisplaySecondaryText
                        text: gridEntry.secondaryText

                        opacity: 0.6
                        color: myPalette.text

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
                    id: contextMenuButton
                    visible: Kirigami.Settings.isMobile
                    scale: LayoutMirroring.enabled ? -1 : 1
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    Layout.preferredHeight: Math.round(Kirigami.Units.gridUnit * 2.5)
                    Layout.preferredWidth: Math.round(Kirigami.Units.gridUnit * 1.5)

                    text: i18nc("Options", "Options")
                    icon.name: "view-more-symbolic"
                    onClicked: openContextMenu()
                }
            }
        }

        Loader {
            active: isPartial
            anchors.centerIn: parent
            height: Kirigami.Units.gridUnit * 5
            width: height

            sourceComponent: BusyIndicator {
                anchors.centerIn: parent
                running: true
            }
        }
    }

    // mobile context menu sheet
    Loader {
        id: contextMenuLoader
        active: false

        sourceComponent: MobileContextMenuSheet {
            id: contextMenu
            title: gridEntry.mainText

            ColumnLayout {
                Layout.preferredWidth: Kirigami.Units.gridUnit * 20
                spacing: 0

                MobileContextMenuEntry {
                    onClicked: {
                        replaceAndPlay();
                        contextMenu.close();
                    }
                    icon: "media-playback-start"
                    text: i18nc("Clear play list and add whole container to play list", "Play now, replacing current queue")
                }

                MobileContextMenuEntry {
                    visible: fileUrl.toString().substring(0, 7) === 'file://'
                    onClicked: {
                       ElisaApplication.showInFolder(gridEntry.fileUrl)
                        contextMenu.close();
                    }
                    icon: "document-open-folder"
                    text: i18nc("Show the file for this song in the file manager", "Show in folder")
                }

                MobileContextMenuEntry {
                    onClicked: {
                        enqueue();
                        contextMenu.close();
                    }
                    icon: "list-add"
                    text: i18nc("Add whole container to play list", "Add to queue")
                }
            }
        }
    }
}
