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
import ElisaGraphicalEffects 1.15
import org.kde.kirigami 2.19 as Kirigami
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
    property bool editingRating: false

    signal clicked()
    signal enqueue()
    signal replaceAndPlay()
    signal callOpenMetaDataView(var url, var entryType)
    signal trackRatingChanged(var url, var rating)

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
            anchors {
                fill: parent
                leftMargin: Kirigami.Units.largeSpacing
                rightMargin: Kirigami.Units.largeSpacing
            }

            spacing: Kirigami.Units.largeSpacing

            Loader {
                active: mediaTrack.delegateLoaded && (detailedView || Kirigami.Settings.isMobile) // cover is always visible on mobile

                // mobile delegate needs more margins
                Layout.preferredWidth: mediaTrack.height - Kirigami.Units.smallSpacing * (Kirigami.Settings.isMobile ? 2 : 1)
                Layout.preferredHeight: mediaTrack.height - Kirigami.Units.smallSpacing * (Kirigami.Settings.isMobile ? 2 : 1)

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

                        color: myPalette.shadow
                    }
                }
            }

            ColumnLayout {
                id: labels

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                spacing: Kirigami.Units.smallSpacing / 2

                // first row (main label)
                Label {
                    id: mainLabel

                    Layout.fillWidth: true

                    text: {
                        if (detailedView) {
                            return title;
                        } else if (Kirigami.Settings.isMobile) {
                            // specific artist/album page (mobile)
                            // not detailed view refers to an album page, in which we should put track numbers
                            if (trackNumber !== 0 && trackNumber !== -1 && trackNumber !== undefined) {
                                return i18nc("@item:intable %1: track number. %2: track title.",
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
                                    return i18nc("@item:intable %1: track number. %2: track title. %3: artist name",
                                                "%1 - %2 - %3",
                                                trackNumber.toLocaleString(Qt.locale(), 'f', 0),
                                                title, artist);
                                } else {
                                    return i18nc("@item:intable %1: track number. %2: track title.",
                                                "%1 - %2",
                                                trackNumber.toLocaleString(Qt.locale(), 'f', 0),
                                                title);
                                }
                            } else {
                                if (albumArtist !== undefined && artist !== albumArtist) {
                                    return i18nc("@item:intable %1: track title. %2: artist name",
                                                "%1 - %2",
                                                title, artist);
                                } else {
                                    return title;
                                }
                            }
                        }
                    }
                    textFormat: Text.PlainText

                    elide: Text.ElideRight
                }

                // second row (shown for mobile, and desktop detailed view)
                Label {
                    id: artistLabel

                    Layout.fillWidth: true

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
                    textFormat: Text.PlainText

                    elide: Text.ElideRight
                    font: Kirigami.Theme.smallFont
                }
            }

            // hover actions (for desktop)
            Loader {
                id: hoverLoader
                active: !Kirigami.Settings.isMobile && (hoverArea.containsMouse || mediaTrack.activeFocus) && !mediaTrack.editingRating
                visible: active

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                z: 1

                sourceComponent: Row {
                    anchors.centerIn: parent

                    FlatButtonWithToolTip {
                        width: singleLineHeight
                        height: singleLineHeight

                        visible: trackUrl.toString().substring(0, 7) === 'file://'

                        text: i18nc("@action:button Show the file for this song in the file manager", "Show in folder")
                        icon.name: "document-open-folder"
                        onClicked: {
                            ElisaApplication.showInFolder(mediaTrack.trackUrl)
                        }
                    }

                    FlatButtonWithToolTip {
                        id: detailsButton
                        width: singleLineHeight
                        height: singleLineHeight

                        text: i18nc("@action:button Show track metadata", "View details")
                        icon.name: "help-about"
                        onClicked: callOpenMetaDataView(trackUrl, dataType)
                    }

                    FlatButtonWithToolTip {
                        id: enqueueButton
                        width: singleLineHeight
                        height: singleLineHeight

                        text: i18nc("@action:button", "Add to playlist")
                        icon.name: "list-add"
                        onClicked: enqueue()
                    }

                    FlatButtonWithToolTip {
                        id: clearAndEnqueueButton
                        scale: LayoutMirroring.enabled ? -1 : 1
                        width: singleLineHeight
                        height: singleLineHeight

                        text: i18nc("@action:button", "Play now, replacing current playlist")
                        icon.name: "media-playback-start"
                        onClicked: replaceAndPlay()
                    }

                    FlatButtonWithToolTip {
                        id: ratingButton

                        visible: !ElisaApplication.useFavoriteStyleRatings

                        width: singleLineHeight
                        height: singleLineHeight

                        text: i18nc("@action:button", "Set track rating")
                        icon.name: "view-media-favorite"
                        onClicked: {
                            mediaTrack.editingRating = true;
                        }
                    }

                    FlatButtonWithToolTip {
                        visible: ElisaApplication.useFavoriteStyleRatings

                        width: singleLineHeight
                        height: singleLineHeight

                        text: rating == 10 ? i18nc("@action:button", "Un-mark this song as a favorite") : i18nc("@action:button", "Mark this song as a favorite")
                        icon.name: rating == 10 ? "rating" : "rating-unrated"

                        onClicked: {
                            var newRating = 0;
                            if (rating == 10) {
                                newRating = 0;
                                // Change icon immediately in case backend is slow
                                icon.name = "rating-unrated";
                            } else {
                                newRating = 10;
                                // Change icon immediately in case backend is slow
                                icon.name = "rating";
                            }
                            trackRatingChanged(trackUrl, newRating);
                        }
                    }
                }
            }

            // ratings (desktop)
            Loader {
                id: cancelRatingLoader
                active: !Kirigami.Settings.isMobile && (hoverArea.containsMouse || mediaTrack.activeFocus)
                visible: active && mediaTrack.editingRating

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                z: 1

                sourceComponent: Row {
                    anchors.centerIn: parent
                        FlatButtonWithToolTip {
                        width: singleLineHeight
                        height: singleLineHeight
                        text: i18nc("@action:button", "Cancel rating this track")
                        icon.name: "dialog-cancel"
                        onClicked: { mediaTrack.editingRating = false; }
                    }
                }
            }
            RatingStar {
                id: ratingWidget
                visible: !Kirigami.Settings.isMobile && (mediaTrack.editingRating || (rating > 0 && !hoverArea.containsMouse && !mediaTrack.activeFocus && !ElisaApplication.useFavoriteStyleRatings))

                readOnly: false

                starRating: rating

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                onRatingEdited: {
                    mediaTrack.editingRating = false
                    trackRatingChanged(trackUrl, starRating);
                }
            }
            Loader {
                id: favoriteMark

                visible: !Kirigami.Settings.isMobile && ElisaApplication.useFavoriteStyleRatings && !hoverLoader.active && rating == 10

                sourceComponent: Row {
                    FlatButtonWithToolTip {
                        width: singleLineHeight
                        height: singleLineHeight
                        icon.name: rating == 10 ? "rating" : "rating-unrated"
                    }
                }
            }

            LabelWithToolTip {
                id: durationLabel

                text: duration

                horizontalAlignment: Text.AlignRight

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            }

            // mobile context actions menu button
            FlatButtonWithToolTip {
                id: contextMenuButton
                visible: Kirigami.Settings.isMobile
                scale: LayoutMirroring.enabled ? -1 : 1
                Layout.alignment: Qt.AlignVCenter
                Layout.maximumHeight: parent.height
                Layout.preferredWidth: height
                Layout.preferredHeight: singleLineHeight - Kirigami.Units.smallSpacing * 2

                text: i18nc("@action:button", "Song Options")
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
        sourceComponent: Kirigami.MenuDialog {
            id: contextMenu
            title: mediaTrack.title
            preferredWidth: Kirigami.Units.gridUnit * 20

            actions: [
                Kirigami.Action {
                    visible: trackUrl.toString().substring(0, 7) === 'file://'
                    onTriggered: {
                        ElisaApplication.showInFolder(mediaTrack.trackUrl);
                        contextMenu.close();
                    }
                    iconName: "document-open-folder"
                    text: i18nc("@action:button Show the file for this song in the file manager", "Show in folder")
                },
                Kirigami.Action {
                    onTriggered: {
                        callOpenMetaDataView(trackUrl, dataType);
                        contextMenu.close();
                    }
                    iconName: "documentinfo"
                    text: i18nc("@action:button", "View details")
                },
                Kirigami.Action {
                    visible: ElisaApplication.useFavoriteStyleRatings
                    onTriggered: {
                        var newRating = 0;
                        if (rating == 10) {
                            newRating = 0;
                        } else {
                            newRating = 10;
                        }
                        contextMenu.close();
                    }
                    iconName: rating == 10 ? "rating-unrated" : "rating"
                    text: rating == 10 ? i18nc("@action:button", "Un-mark as favorite") : i18nc("@action:button", "Mark as favorite")
                },
                Kirigami.Action {
                    onTriggered: {
                        enqueue();
                        contextMenu.close();
                    }
                    iconName: "list-add"
                    text: i18nc("@action:button Enqueue current track", "Add to queue")
                }
            ]
        }
    }

    states: [
        State {
            name: 'notSelected'
            when: !mediaTrack.activeFocus && !hoverArea.containsMouse && !mediaTrack.isSelected
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
