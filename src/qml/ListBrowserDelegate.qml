/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>
   SPDX-FileCopyrightText: 2021 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

import "mobile"

FocusScope {
    id: mediaTrack

    property url trackUrl
    property var dataType
    property string title
    property string artist
    property string album
    property string albumArtist
    property string duration
    property url imageUrl
    property int trackNumber
    property int discNumber
    property int rating
    property bool hideDiscNumber
    property bool isSelected
    property bool isAlternateColor
    property bool detailedView: true

    signal clicked()
    signal enqueue()
    signal replaceAndPlay()
    signal callOpenMetaDataView(var url, var entryType)

    Accessible.role: Accessible.ListItem
    Accessible.name: title
    Accessible.description: title

    Keys.onReturnPressed: enqueue()
    Keys.onEnterPressed: enqueue()

    property bool delegateLoaded: true

    ListView.onPooled: delegateLoaded = false
    ListView.onReused: delegateLoaded = true

    property int singleLineHeight: {
        if (Kirigami.Settings.isMobile) {
            return 4 * Kirigami.Units.smallSpacing + Kirigami.Units.gridUnit * 2;
        } else {
            return 3 * Kirigami.Units.smallSpacing + Kirigami.Units.gridUnit;
        }
    }
    height: singleLineHeight + (!Kirigami.Settings.isMobile && detailedView ? Kirigami.Units.gridUnit : 0)

    // open mobile context menu
    function openContextMenu() {
        contextMenuLoader.active = true;
        contextMenuLoader.item.open();
    }

    Rectangle {
        id: rowRoot

        anchors.fill: parent
        z: 1

        color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)
    }

    MouseArea {
        id: hoverArea

        anchors.fill: parent
        z: 2

        hoverEnabled: true
        acceptedButtons: Qt.LeftButton

        onClicked: {
            mediaTrack.clicked()

            // open track on click for mobile
            if (Kirigami.Settings.isMobile) {
                replaceAndPlay();
            }
        }

        onDoubleClicked: {
            if (!Kirigami.Settings.isMobile) {
                enqueue();
            }
        }

        RowLayout {
            anchors.fill: parent
            spacing: 0

            Loader {
                active: mediaTrack.delegateLoaded && (detailedView || Kirigami.Settings.isMobile) // cover is always visible on mobile

                // mobile delegate needs more margins
                Layout.preferredHeight: mediaTrack.height - Kirigami.Units.smallSpacing * (Kirigami.Settings.isMobile ? 2 : 1)
                Layout.preferredWidth: mediaTrack.height - Kirigami.Units.smallSpacing * (Kirigami.Settings.isMobile ? 2 : 1)
                Layout.leftMargin: !LayoutMirroring.enabled ? Kirigami.Units.smallSpacing : 0
                Layout.rightMargin: LayoutMirroring.enabled ? Kirigami.Units.smallSpacing : 0

                Layout.alignment: Qt.AlignCenter

                sourceComponent: ImageWithFallback {
                    id: coverImageElement

                    sourceSize.width: mediaTrack.height - Kirigami.Units.smallSpacing * (Kirigami.Settings.isMobile ? 2 : 1)
                    sourceSize.height: mediaTrack.height - Kirigami.Units.smallSpacing * (Kirigami.Settings.isMobile ? 2 : 1)
                    fillMode: Image.PreserveAspectFit
                    smooth: true

                    source: imageUrl
                    fallback: elisaTheme.defaultAlbumImage

                    asynchronous: true

                    layer.enabled: !usingFallback && !Kirigami.Settings.isMobile // disable drop shadow for mobile

                    layer.effect: DropShadow {
                        source: coverImageElement

                        radius: 10
                        spread: 0.1
                        samples: 21

                        color: myPalette.shadow
                    }
                }
            }

            ColumnLayout {
                id: labels

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.leftMargin: Kirigami.Units.largeSpacing

                spacing: Kirigami.Units.smallSpacing / 2

                // first row (main label)
                Label {
                    id: mainLabel

                    text: {
                        if (detailedView) {
                            return title;
                        } else if (Kirigami.Settings.isMobile) {
                            // specific artist/album page (mobile)
                            // not detailed view refers to an album page, in which we should put track numbers
                            if (trackNumber !== 0 && trackNumber !== -1 && trackNumber !== undefined) {
                                return i18nc("%1: track number. %2: track title.",
                                        "%1 - %2",
                                        trackNumber.toLocaleString(Qt.locale(), 'f', 0),
                                        title);
                            } else {
                                return title;
                            }
                        } else {
                            // specific artist/album page (desktop)
                            if (trackNumber !== 0 && trackNumber !== -1 && trackNumber !== undefined) {
                                if (albumArtist !== undefined && artist !== albumArtist) {
                                    return i18nc("%1: track number. %2: track title. %3: artist name",
                                                "%1 - %2 - %3",
                                                trackNumber.toLocaleString(Qt.locale(), 'f', 0),
                                                title, artist);
                                } else {
                                    return i18nc("%1: track number. %2: track title.",
                                                "%1 - %2",
                                                trackNumber.toLocaleString(Qt.locale(), 'f', 0),
                                                title);
                                }
                            } else {
                                if (albumArtist !== undefined && artist !== albumArtist) {
                                    return i18nc("%1: track title. %2: artist name",
                                                "%1 - %2",
                                                title, artist);
                                } else {
                                    return title;
                                }
                            }
                        }
                    }
                    horizontalAlignment: Text.AlignLeft
                    color: myPalette.text

                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: !LayoutMirroring.enabled ? (Kirigami.Settings.isMobile ? 0 : Kirigami.Units.smallSpacing) : 0
                    Layout.rightMargin: LayoutMirroring.enabled ? (Kirigami.Settings.isMobile ? 0 : Kirigami.Units.smallSpacing) : 0
                    Layout.fillWidth: true

                    elide: Text.ElideRight
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize
                }

                // second row (shown for mobile, and desktop detailed view)
                Label {
                    id: artistLabel

                    text: {
                        var labelText = ""
                        if (artist) {
                            labelText += artist
                        }
                        if (album !== '' && detailedView) { // don't show album name on not detailed view
                            labelText += ' - ' + album
                            if (!hideDiscNumber && discNumber !== -1) {
                                labelText += ' - CD ' + discNumber
                            }
                        }
                        return labelText;
                    }
                    horizontalAlignment: Text.AlignLeft

                    visible: text.length > 0 && (Kirigami.Settings.isMobile || detailedView)
                    opacity: 0.6
                    color: myPalette.text

                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: !LayoutMirroring.enabled ? (Kirigami.Settings.isMobile ? 0 : Kirigami.Units.smallSpacing) : 0
                    Layout.rightMargin: LayoutMirroring.enabled ? (Kirigami.Settings.isMobile ? 0 : Kirigami.Units.smallSpacing) : 0
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    font: Kirigami.Theme.smallFont
                }
            }

            // hover actions (for desktop)
            Loader {
                id: hoverLoader
                active: !Kirigami.Settings.isMobile && (hoverArea.containsMouse || mediaTrack.activeFocus)
                visible: active

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                Layout.rightMargin: 10

                z: 1

                sourceComponent: Row {
                    anchors.centerIn: parent

                    FlatButtonWithToolTip {
                        height: singleLineHeight
                        width: singleLineHeight

                        visible: trackUrl.toString().substring(0, 7) === 'file://'

                        text: i18nc("Show the file for this song in the file manager", "Show in folder")
                        icon.name: "document-open-folder"
                        onClicked: {
                            ElisaApplication.showInFolder(mediaTrack.trackUrl)
                        }
                    }

                    FlatButtonWithToolTip {
                        id: detailsButton
                        height: singleLineHeight
                        width: singleLineHeight

                        text: i18nc("Show track metadata", "View Details")
                        icon.name: "help-about"
                        onClicked: callOpenMetaDataView(trackUrl, dataType)
                    }

                    FlatButtonWithToolTip {
                        id: enqueueButton
                        height: singleLineHeight
                        width: singleLineHeight

                        text: i18nc("Enqueue current track", "Enqueue")
                        icon.name: "list-add"
                        onClicked: enqueue()
                    }

                    FlatButtonWithToolTip {
                        id: clearAndEnqueueButton
                        scale: LayoutMirroring.enabled ? -1 : 1
                        height: singleLineHeight
                        width: singleLineHeight

                        text: i18nc("Clear play list and enqueue current track", "Play Now and Replace Play List")
                        icon.name: "media-playback-start"
                        onClicked: replaceAndPlay()
                    }
                }
            }

            // ratings (desktop)
            RatingStar {
                id: ratingWidget
                visible: !Kirigami.Settings.isMobile

                starRating: rating

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
            }

            LabelWithToolTip {
                id: durationLabel

                text: duration

                horizontalAlignment: Text.AlignRight

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                Layout.rightMargin: !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
                Layout.leftMargin: LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
            }

            // mobile context actions menu button
            FlatButtonWithToolTip {
                id: contextMenuButton
                visible: Kirigami.Settings.isMobile
                scale: LayoutMirroring.enabled ? -1 : 1
                Layout.alignment: Qt.AlignVCenter
                Layout.maximumHeight: parent.height
                Layout.preferredHeight: singleLineHeight - Kirigami.Units.smallSpacing * 2
                Layout.preferredWidth: height

                text: i18nc("Song options", "Song Options")
                icon.name: "view-more-symbolic"
                onClicked: openContextMenu()
            }
        }
    }

    // mobile context menu
    Loader {
        id: contextMenuLoader
        active: false

        // context menu sheet
        sourceComponent: MobileContextMenuSheet {
            id: contextMenu
            title: mediaTrack.title

            ColumnLayout {
                Layout.preferredWidth: Kirigami.Units.gridUnit * 20
                spacing: 0

                MobileContextMenuEntry {
                    visible: trackUrl.toString().substring(0, 7) === 'file://'
                    onClicked: {
                        ElisaApplication.showInFolder(mediaTrack.trackUrl);
                        contextMenu.close();
                    }
                    icon: "document-open-folder"
                    text: i18nc("Show the file for this song in the file manager", "Show in folder")
                }

                MobileContextMenuEntry {
                    onClicked: {
                        callOpenMetaDataView(trackUrl, dataType);
                        contextMenu.close();
                    }
                    icon: "documentinfo"
                    text: i18nc("Show track metadata", "View details")
                }

                MobileContextMenuEntry {
                    onClicked: {
                        enqueue();
                        contextMenu.close();
                    }
                    icon: "list-add"
                    text: i18nc("Enqueue current track", "Add to queue")
                }

                RatingStar {
                    id: ratingWidgetMobile

                    starRating: rating

                    Layout.alignment: Qt.AlignHCenter
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                }
            }
        }
    }

    states: [
        State {
            name: 'notSelected'
            when: !mediaTrack.activeFocus && !hoverArea.containsMouse && !mediaTrack.isSelected
            PropertyChanges {
                target: ratingWidget
                hoverWidgetOpacity: 0.0
            }
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
            when: !mediaTrack.activeFocus && hoverArea.containsMouse
            PropertyChanges {
                target: ratingWidget
                hoverWidgetOpacity: 1.0
            }
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
            when: !mediaTrack.activeFocus && mediaTrack.isSelected
            PropertyChanges {
                target: ratingWidget
                hoverWidgetOpacity: 1.0
            }
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
            when: mediaTrack.activeFocus
            PropertyChanges {
                target: ratingWidget
                hoverWidgetOpacity: 1.0
            }
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
