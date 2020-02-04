/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
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
    signal enqueue(var url, var entryType, var name)
    signal replaceAndPlay(var url, var entryType, var name)
    signal callOpenMetaDataView(var url, var entryType)

    Accessible.role: Accessible.ListItem
    Accessible.name: title
    Accessible.description: title

    Keys.onReturnPressed: enqueue(trackUrl, dataType, title)
    Keys.onEnterPressed: enqueue(trackUrl, dataType, title)

    TextMetrics {
        id: mainLabelSize
        font: mainLabel.font
        text: mainLabel.text
    }

    property int singleLineHeight:  elisaTheme.layoutVerticalMargin * 2 + mainLabelSize.height
    height: singleLineHeight + (detailedView ? mainLabelSize.height : 0)

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

        onDoubleClicked: enqueue(trackUrl, dataType, title)

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
                                         "<b>%1 - %2</b> - <i>%3</i>",
                                         trackNumber.toLocaleString(Qt.locale(), 'f', 0),
                                         title, artist);
                        else
                            return i18nc("%1: track number. %2: track title.",
                                         "<b>%1 - %2</b>",
                                         trackNumber.toLocaleString(Qt.locale(), 'f', 0),
                                         title);
                    } else {
                        if (albumArtist !== undefined && artist !== albumArtist)
                            return i18nc("%1: track title. %2: artist name",
                                         "<b>%1</b> - <i>%2</i>",
                                         title, artist);
                        else
                            return i18nc("%1: track title",
                                         "<b>%1</b>",
                                         title);
                    }
                }

                elide: Text.ElideRight
                horizontalAlignment: Text.AlignLeft

                color: myPalette.text

                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.fillWidth: true
                Layout.leftMargin: {
                    if (!LayoutMirroring.enabled)
                        return (!hideDiscNumber ? elisaTheme.layoutHorizontalMargin * 4 : elisaTheme.layoutHorizontalMargin)
                    else
                        return 0
                }
                Layout.rightMargin: {
                    if (LayoutMirroring.enabled)
                        return (!hideDiscNumber ? elisaTheme.layoutHorizontalMargin * 4 : elisaTheme.layoutHorizontalMargin)
                    else
                        return 0
                }
            }

            Image {
                id: coverImageElement

                Layout.preferredHeight: mediaTrack.height - elisaTheme.layoutVerticalMargin
                Layout.preferredWidth: mediaTrack.height - elisaTheme.layoutVerticalMargin
                Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutVerticalMargin : 0
                Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutVerticalMargin : 0

                Layout.alignment: Qt.AlignCenter

                visible: detailedView

                sourceSize.width: mediaTrack.height - elisaTheme.layoutVerticalMargin
                sourceSize.height: mediaTrack.height - elisaTheme.layoutVerticalMargin
                fillMode: Image.PreserveAspectFit
                smooth: true

                source: (imageUrl != '' ? imageUrl : Qt.resolvedUrl(elisaTheme.defaultAlbumImage))

                asynchronous: true

                layer.enabled: imageUrl != ''

                layer.effect: DropShadow {
                    source: coverImageElement

                    radius: 10
                    spread: 0.1
                    samples: 21

                    color: myPalette.shadow
                }

                onStatusChanged: {
                    if (coverImageElement.status === Image.Error) {
                        source = 'image://icon/media-optical-audio'
                    }
                }
            }

            ColumnLayout {
                visible: detailedView

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignLeft

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
                    Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                    Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                    Layout.fillWidth: true
                    Layout.topMargin: elisaTheme.layoutVerticalMargin / 2

                    elide: Text.ElideRight
                }

                Item {
                    Layout.fillHeight: true
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

                    opacity: 0.6
                    color: myPalette.text

                    Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                    Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                    Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                    Layout.fillWidth: true
                    Layout.bottomMargin: elisaTheme.layoutVerticalMargin / 2

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
                        onClicked: enqueue(trackUrl, dataType, title)
                    }

                    FlatButtonWithToolTip {
                        id: clearAndEnqueueButton
                        scale: LayoutMirroring.enabled ? -1 : 1
                        height: singleLineHeight
                        width: singleLineHeight

                        text: i18nc("Clear play list and enqueue current track", "Play Now and Replace Play List")
                        icon.name: "media-playback-start"
                        onClicked: replaceAndPlay(trackUrl, dataType, title)
                    }
                }
            }

            RatingStar {
                id: ratingWidget

                starSize: elisaTheme.ratingStarSize

                starRating: rating

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                Layout.leftMargin: elisaTheme.layoutHorizontalMargin
                Layout.rightMargin: elisaTheme.layoutHorizontalMargin
            }

            LabelWithToolTip {
                id: durationLabel

                text: duration

                font.weight: Font.Light
                color: myPalette.text

                horizontalAlignment: Text.AlignRight

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                Layout.rightMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                Layout.leftMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            }
        }
    }

    Connections {
        target: mediaTrack

        onImageUrlChanged: {
            if (coverImageElement.source !== imageUrl) {
                coverImageElement.source = (imageUrl != '' ? imageUrl : Qt.resolvedUrl(elisaTheme.defaultAlbumImage))
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
