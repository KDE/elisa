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

ItemDelegate {
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
    readonly property bool isFavorite: rating === 10

    readonly property bool imageVisible: delegateLoaded && (detailedView || Kirigami.Settings.isMobile)

    readonly property bool hasActiveFocus: activeFocus || focusScope.activeFocus

    readonly property color textColor: highlighted || down ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
    readonly property color iconColor: textColor

    signal enqueue()
    signal playNext()
    signal replaceAndPlay(var url)
    signal callOpenMetaDataView(var url, var entryType)
    signal trackRatingChanged(var url, var rating)

    Kirigami.Theme.useAlternateBackgroundColor: isAlternateColor

    Accessible.role: Accessible.ListItem
    Accessible.name: title
    Accessible.description: title

    Keys.onReturnPressed: enqueue()
    Keys.onEnterPressed: enqueue()

    property bool delegateLoaded: true

    ListView.onPooled: delegateLoaded = false
    ListView.onReused: delegateLoaded = true

    property list<Kirigami.Action> actions: [
        Kirigami.Action {
            text: i18nc("@action:button Show the file for this song in the file manager", "Show in folder")
            icon.name: "document-open-folder"
            onTriggered: ElisaApplication.showInFolder(mediaTrack.trackUrl)

            visible: mediaTrack.trackUrl.toString().substring(0, 7) === 'file://'
        },
        Kirigami.Action {
            text: i18nc("@action:button Show track metadata", "View details")
            icon.name: "help-about"
            onTriggered: mediaTrack.callOpenMetaDataView(mediaTrack.trackUrl, mediaTrack.dataType)
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Add to playlist")
            icon.name: "list-add"
            onTriggered: mediaTrack.enqueue()
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Play next")
            icon.name: "media-playlist-append-next-symbolic"
            onTriggered: mediaTrack.playNext()
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Play from here, replacing current playlist")
            icon.name: "media-playback-start-symbolic"
            onTriggered: mediaTrack.replaceAndPlay(mediaTrack.trackUrl)
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Set track rating")
            icon.name: "view-media-favorite"
            onTriggered: {
                mediaTrack.editingRating = true;
            }
            visible: !ElisaApplication.useFavoriteStyleRatings && !Kirigami.Settings.isMobile
        },
        Kirigami.Action {
            text: mediaTrack.isFavorite ? i18nc("@action:button", "Un-mark this song as a favorite") : i18nc("@action:button", "Mark this song as a favorite")
            icon.name: mediaTrack.isFavorite ? "starred" : "non-starred"

            onTriggered: {
                const newRating = mediaTrack.isFavorite ? 0 : 10
                // Change icon immediately in case backend is slow
                icon.name = mediaTrack.isFavorite ? "non-starred" : "starred"
                mediaTrack.trackRatingChanged(mediaTrack.trackUrl, newRating);
            }

            visible: ElisaApplication.useFavoriteStyleRatings
        }
    ]

    // open mobile context menu
    function openContextMenu() {
        contextMenuLoader.active = true;
        (contextMenuLoader.item as Kirigami.MenuDialog).open();
    }

    onHasActiveFocusChanged: {
        if (!hasActiveFocus) {
            editingRating = false
        }
    }

    onClicked: replaceAndPlay(trackUrl)

    contentItem: FocusScope {
        id: focusScope

        implicitHeight: childrenRect.height

        RowLayout {
            width: parent.width
            height: implicitHeight

            spacing: Kirigami.Units.largeSpacing

            Loader {
                active: mediaTrack.imageVisible
                visible: active

                // mobile delegate needs more margins
                Layout.preferredWidth: Theme.listDelegateIconHeight
                Layout.preferredHeight: Theme.listDelegateIconHeight

                Layout.alignment: Qt.AlignCenter

                sourceComponent: ImageWithFallback {
                    id: coverImageElement

                    sourceSize.width: Theme.listDelegateIconHeight * Screen.devicePixelRatio
                    sourceSize.height: Theme.listDelegateIconHeight * Screen.devicePixelRatio
                    fillMode: Image.PreserveAspectFit
                    smooth: true

                    source: mediaTrack.imageUrl
                    fallback: Theme.defaultAlbumImage

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
                        if (mediaTrack.detailedView) {
                            return mediaTrack.title;
                        } else if (Kirigami.Settings.isMobile) {
                            // specific artist/album page (mobile)
                            // not detailed view refers to an album page, in which we should put track numbers
                            if (mediaTrack.trackNumber !== 0 && mediaTrack.trackNumber !== -1 && mediaTrack.trackNumber !== undefined) {
                                return i18nc("@item:intable %1: track number. %2: track title.",
                                        "%1 – %2",
                                        mediaTrack.trackNumber.toLocaleString(Qt.locale(), 'f', 0),
                                        mediaTrack.title);
                            } else {
                                return mediaTrack.title;
                            }
                        } else {
                            // specific artist/album page (desktop)
                            if (mediaTrack.trackNumber !== 0 && mediaTrack.trackNumber !== -1 && mediaTrack.trackNumber !== undefined) {
                                if (mediaTrack.albumArtist !== undefined && mediaTrack.artist !== mediaTrack.albumArtist) {
                                    return i18nc("@item:intable %1: track number. %2: track title. %3: artist name",
                                                "%1 – %2 – %3",
                                                mediaTrack.trackNumber.toLocaleString(Qt.locale(), 'f', 0),
                                                mediaTrack.title, mediaTrack.artist);
                                } else {
                                    return i18nc("@item:intable %1: track number. %2: track title.",
                                                "%1 – %2",
                                                mediaTrack.trackNumber.toLocaleString(Qt.locale(), 'f', 0),
                                                mediaTrack.title);
                                }
                            } else {
                                if (mediaTrack.albumArtist !== undefined && mediaTrack.artist !== mediaTrack.albumArtist) {
                                    return i18nc("@item:intable %1: track title. %2: artist name",
                                                "%1 – %2",
                                                mediaTrack.title, mediaTrack.artist);
                                } else {
                                    return mediaTrack.title;
                                }
                            }
                        }
                    }
                    textFormat: Text.PlainText

                    elide: Text.ElideRight
                    color: mediaTrack.textColor
                }

                // second row (shown for mobile, and desktop detailed view)
                Label {
                    id: artistLabel

                    Layout.fillWidth: true

                    text: {
                        let labelText = ""
                        if (mediaTrack.artist) {
                            labelText += mediaTrack.artist
                        }
                        if (mediaTrack.album !== '' && mediaTrack.detailedView) { // don't show album name on not detailed view
                            labelText += ' - ' + mediaTrack.album
                            if (!mediaTrack.hideDiscNumber && mediaTrack.discNumber !== -1) {
                                labelText += ' - CD ' + mediaTrack.discNumber
                            }
                        }
                        return labelText;
                    }
                    horizontalAlignment: Text.AlignLeft

                    visible: text.length > 0 && (Kirigami.Settings.isMobile || mediaTrack.detailedView)
                    opacity: Theme.subtitleOpacity
                    textFormat: Text.PlainText

                    elide: Text.ElideRight
                    font: Kirigami.Theme.smallFont
                    color: mediaTrack.textColor
                }
            }

            // hover actions (for desktop)
            Loader {
                id: hoverLoader
                active: !Kirigami.Settings.isMobile && (mediaTrack.hovered || mediaTrack.hasActiveFocus) && !mediaTrack.editingRating

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                Layout.preferredHeight: Theme.listDelegateButtonHeight

                z: 1

                sourceComponent: Row {
                    anchors.centerIn: parent

                    Repeater {
                        model: mediaTrack.actions

                        delegate: FlatButtonWithToolTip {
                            required property Kirigami.Action modelData

                            width: Theme.listDelegateButtonHeight
                            height: Theme.listDelegateButtonHeight
                            action: modelData
                            visible: modelData.visible
                            icon.color: mediaTrack.iconColor
                        }
                    }
                }
            }

            // ratings (desktop)
            Loader {
                id: cancelRatingLoader
                active: !Kirigami.Settings.isMobile && (mediaTrack.hovered || mediaTrack.hasActiveFocus)
                visible: active && mediaTrack.editingRating

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                z: 1

                sourceComponent: FlatButtonWithToolTip {
                    width: Theme.listDelegateButtonHeight
                    height: Theme.listDelegateButtonHeight
                    text: i18nc("@action:button", "Cancel rating this track")
                    icon.name: "dialog-cancel"
                    icon.color: mediaTrack.iconColor
                    onClicked: { mediaTrack.editingRating = false; }
                }
            }
            RatingStar {
                id: ratingWidget
                visible: !Kirigami.Settings.isMobile && (mediaTrack.editingRating || (mediaTrack.rating > 0 && !mediaTrack.hovered && !mediaTrack.hasActiveFocus && !ElisaApplication.useFavoriteStyleRatings))

                readOnly: !mediaTrack.editingRating

                starRating: mediaTrack.rating

                iconColor: mediaTrack.iconColor

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                onRatingEdited: {
                    mediaTrack.editingRating = false
                    mediaTrack.trackRatingChanged(mediaTrack.trackUrl, starRating);
                }
            }
            Loader {
                id: favoriteMark

                visible: !Kirigami.Settings.isMobile && ElisaApplication.useFavoriteStyleRatings && !hoverLoader.active && mediaTrack.isFavorite

                sourceComponent: FlatButtonWithToolTip {
                    width: Theme.listDelegateButtonHeight
                    height: Theme.listDelegateButtonHeight
                    icon.name: mediaTrack.isFavorite ? "starred" : "non-starred"
                    icon.color: mediaTrack.iconColor
                }
            }

            LabelWithToolTip {
                id: durationLabel

                text: mediaTrack.duration

                horizontalAlignment: Text.AlignRight

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                font.features: { "tnum": 1 }
                color: mediaTrack.textColor
            }

            // mobile context actions menu button
            FlatButtonWithToolTip {
                id: contextMenuButton
                visible: Kirigami.Settings.isMobile
                scale: LayoutMirroring.enabled ? -1 : 1
                Layout.alignment: Qt.AlignVCenter
                Layout.maximumHeight: parent.height
                Layout.preferredWidth: height
                Layout.preferredHeight: Theme.listDelegateButtonHeight

                text: i18nc("@action:button", "Song Options")
                icon.name: "view-more-symbolic"
                icon.color: mediaTrack.iconColor
                onClicked: mediaTrack.openContextMenu()
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

            actions: mediaTrack.actions
        }
    }
}
