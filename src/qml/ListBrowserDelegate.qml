/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

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

    property int singleLineHeight: 3 * Kirigami.Units.smallSpacing + Kirigami.Units.gridUnit
    height: singleLineHeight + (detailedView ? Kirigami.Units.gridUnit : 0)

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
        }

        onDoubleClicked: enqueue()

        RowLayout {
            anchors.fill: parent
            spacing: 0

            LabelWithToolTip {
                id: mainLabel

                visible: !detailedView

                text: {
                    if (trackNumber !== 0 && trackNumber !== -1 && trackNumber !== undefined) {
                        if (albumArtist !== undefined && artist !== albumArtist)
                            return i18nc("%1: track number. %2: track title. %3: artist name",
                                         "%1 - %2 - %3",
                                         trackNumber.toLocaleString(Qt.locale(), 'f', 0),
                                         title, artist);
                        else
                            return i18nc("%1: track number. %2: track title.",
                                         "%1 - %2",
                                         trackNumber.toLocaleString(Qt.locale(), 'f', 0),
                                         title);
                    } else {
                        if (albumArtist !== undefined && artist !== albumArtist)
                            return i18nc("%1: track title. %2: artist name",
                                         "%1 - %2",
                                         title, artist);
                        else
                            return title;
                    }
                }

                elide: Text.ElideRight
                horizontalAlignment: Text.AlignLeft

                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.fillWidth: true
                Layout.leftMargin: {
                    if (!LayoutMirroring.enabled)
                        return (!hideDiscNumber ? Kirigami.Units.largeSpacing * 4 : Kirigami.Units.largeSpacing)
                    else
                        return 0
                }
                Layout.rightMargin: {
                    if (LayoutMirroring.enabled)
                        return (!hideDiscNumber ? Kirigami.Units.largeSpacing * 4 : Kirigami.Units.largeSpacing)
                    else
                        return 0
                }
            }

            ImageWithFallback {
                id: coverImageElement

                Layout.preferredHeight: mediaTrack.height - Kirigami.Units.smallSpacing
                Layout.preferredWidth: mediaTrack.height - Kirigami.Units.smallSpacing
                Layout.leftMargin: !LayoutMirroring.enabled ? Kirigami.Units.smallSpacing : 0
                Layout.rightMargin: LayoutMirroring.enabled ? Kirigami.Units.smallSpacing : 0

                Layout.alignment: Qt.AlignCenter

                visible: detailedView

                sourceSize.width: mediaTrack.height - Kirigami.Units.smallSpacing
                sourceSize.height: mediaTrack.height - Kirigami.Units.smallSpacing
                fillMode: Image.PreserveAspectFit
                smooth: true

                source: imageUrl
                fallback: elisaTheme.defaultAlbumImage

                asynchronous: true

                layer.enabled: !usingFallback

                layer.effect: DropShadow {
                    source: coverImageElement

                    radius: 10
                    spread: 0.1
                    samples: 21

                    color: myPalette.shadow
                }
            }

            ColumnLayout {
                visible: detailedView

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                spacing: 0

                LabelWithToolTip {
                    id: mainLabelDetailed

                    level: 4

                    text: {
                        if (trackNumber >= 0) {
                            return i18nc("%1: track number. %2: track title", "%1 - %2",
                                         trackNumber.toLocaleString(Qt.locale(), 'f', 0), title);
                        } else {
                            return title;
                        }
                    }

                    horizontalAlignment: Text.AlignLeft

                    color: myPalette.text

                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                    Layout.leftMargin: !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
                    Layout.rightMargin: LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
                    Layout.fillWidth: true
                    Layout.topMargin: Kirigami.Units.smallSpacing / 2

                    elide: Text.ElideRight
                }

                LabelWithToolTip {
                    id: artistLabel

                    text: {
                        var labelText = ""
                        if (artist) {
                            labelText += artist
                        }
                        if (album !== '') {
                            labelText += ' - ' + album
                            if (!hideDiscNumber && discNumber !== -1) {
                                labelText += ' - CD ' + discNumber
                            }
                        }
                        return labelText;
                    }
                    horizontalAlignment: Text.AlignLeft

                    visible: text.length > 0
                    opacity: 0.6
                    color: myPalette.text

                    Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                    Layout.leftMargin: !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
                    Layout.rightMargin: LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
                    Layout.fillWidth: true
                    Layout.bottomMargin: Kirigami.Units.smallSpacing / 2

                    elide: Text.ElideRight
                }
            }

            Loader {
                id: hoverLoader
                active: false

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                Layout.rightMargin: 10

                z: 1
                opacity: 0

                sourceComponent: Row {
                    anchors.centerIn: parent

                    FlatButtonWithToolTip {
                        height: singleLineHeight
                        width: singleLineHeight

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

            RatingStar {
                id: ratingWidget

                starRating: rating

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
            }

            LabelWithToolTip {
                id: durationLabel

                text: duration

                font.weight: Font.Light
                color: myPalette.text

                horizontalAlignment: Text.AlignRight

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                Layout.rightMargin: !LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
                Layout.leftMargin: LayoutMirroring.enabled ? Kirigami.Units.largeSpacing : 0
            }
        }
    }

    states: [
        State {
            name: 'notSelected'
            when: !mediaTrack.activeFocus && !hoverArea.containsMouse && !mediaTrack.isSelected
            PropertyChanges {
                target: hoverLoader
                active: false
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 0.0
            }
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
                target: hoverLoader
                active: true
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 1.0
            }
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
                target: hoverLoader
                active: false
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 0.0
            }
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
                target: hoverLoader
                active: true
            }
            PropertyChanges {
                target: hoverLoader
                opacity: 1.0
            }
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
