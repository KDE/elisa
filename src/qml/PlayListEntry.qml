/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

import "shared"

BasePlayListDelegate {
    id: playListEntry

    property bool editingRating: false
    // wideMode means there is enough room for the button row
    // otherwise display a menu button
    readonly property bool wideMode: width >= elisaTheme.playListEntryMinWidth

    property var listDelegate

    readonly property string previousAlbum: listDelegate.ListView.previousSection ? JSON.parse(listDelegate.ListView.previousSection)[0] : null
    readonly property string currentAlbum: model.album || null
    readonly property string nextAlbum: listDelegate.ListView.nextSection ? JSON.parse(listDelegate.ListView.nextSection)[0] : null

    readonly property bool grouped: currentAlbum && (previousAlbum === currentAlbum || nextAlbum === currentAlbum)
    readonly property bool sectionVisible: currentAlbum && (previousAlbum !== currentAlbum && nextAlbum === currentAlbum)

    Accessible.role: Accessible.ListItem
    Accessible.name: title + ' ' + album + ' ' + artist

    Keys.onReturnPressed: {
        playListEntry.switchToTrack(playListEntry.index)
        playListEntry.startPlayback()
    }

    topPadding: grouped ? 0 : Kirigami.Units.smallSpacing
    bottomPadding: grouped ? 0 : Kirigami.Units.smallSpacing
    leftPadding: 0
    rightPadding: Kirigami.Units.smallSpacing
    alternatingBackground: false
    separatorVisible: false

    contentItem: Item {
        implicitWidth: playListEntry.width
        implicitHeight: childrenRect.height

        Loader {
            id: metadataLoader
            active: false
            onLoaded: item.show()

            sourceComponent:  MediaTrackMetadataView {
                fileName: playListEntry.fileName
                showImage: entryType !== ElisaUtils.Radio
                modelType: entryType
                showTrackFileName: entryType !== ElisaUtils.Radio
                showDeleteButton: entryType === ElisaUtils.Radio
                editableMetadata: playListEntry.metadataModifiableRole
                canAddMoreMetadata: entryType !== ElisaUtils.Radio

                onRejected: metadataLoader.active = false
            }
        }

        Loader {
            id: actionsLoader
            active: false
            sourceComponent: QtObject {
                property var locateFileAction: Action {
                    text: i18nc("Show the file for this song in the file manager", "Show in folder")
                    icon.name: "document-open-folder"
                    onTriggered: {
                        ElisaApplication.showInFolder(playListEntry.fileName)
                    }
                }
                property var infoAction: Action {
                    text: i18nc("Show track metadata", "View Details")
                    icon.name: "help-about"
                    onTriggered: {
                        if (metadataLoader.active === false) {
                            metadataLoader.active = true
                        }
                        else {
                            metadataLoader.item.close();
                            metadataLoader.active = false
                        }
                    }
                }
                property var ratingAction: Action {
                    text: i18nc("Show track rating", "Set track rating")
                    icon.name: "view-media-favorite"

                    onTriggered: {
                        playListEntry.editingRating = true;
                    }
                }
                property var favoriteAction: Action {
                    text: rating == 10 ? i18n("Un-mark this song as a favorite") : i18n("Mark this song as a favorite")
                    icon.name: rating == 10 ? "rating" : "rating-unrated"

                    onTriggered: {
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
                        ElisaApplication.musicManager.updateSingleFileMetaData(playListEntry.fileName, DataTypes.RatingRole, newRating);
                    }
                }
                property var playPauseAction: Action {
                    text: (isPlaying === MediaPlayList.IsPlaying) ? i18nc("Pause current track from play list", "Pause") : i18nc("Play this track from play list", "Play")
                    icon.name: (isPlaying === MediaPlayList.IsPlaying) ? "media-playback-pause" : "media-playback-start"
                    onTriggered: if (isPlaying === MediaPlayList.IsPlaying) {
                        playListEntry.pausePlayback()
                    } else if (isPlaying === MediaPlayList.IsPaused) {
                        playListEntry.startPlayback()
                    } else {
                        playListEntry.switchToTrack(playListEntry.index)
                        playListEntry.startPlayback()
                    }
                }
                property var removeAction: Action {
                    text: i18nc("Remove current track from play list", "Remove")
                    icon.name: "edit-delete-remove"
                    onTriggered: playListEntry.removeFromPlaylist(playListEntry.index)
                }
            }
        }

        RowLayout {
            id: trackRow

            width: parent.width
            height: Math.max((playListEntry.grouped ? Kirigami.Units.gridUnit : Kirigami.Units.gridUnit * 2), elisaTheme.toolButtonHeight)

            spacing: Kirigami.Units.smallSpacing

            Loader {
                active: !simpleMode && playListEntry.showDragHandle
                sourceComponent: Kirigami.ListItemDragHandle {
                    listItem: playListEntry
                    listView: playListEntry.listView

                    onMoveRequested: {
                        playListEntry.listView.dragging = true
                        ElisaApplication.mediaPlayListProxyModel.moveRow(oldIndex, newIndex)
                    }
                    onDropped: {
                        playListEntry.listView.dragging = false
                    }
                }
            }

            // Container for the play/pause icon and the track/disc label
            Item {
                Layout.preferredWidth: Math.max(elisaTheme.trackNumberWidth, elisaTheme.coverArtSize)
                Layout.fillHeight: true

                Loader {
                    active: !playListEntry.grouped
                    opacity: playIcon.visible ? 0.2 : 1
                    anchors.fill: parent

                    sourceComponent: ImageWithFallback {
                        source: imageUrl
                        fallback: elisaTheme.defaultAlbumImage

                        sourceSize.width: height
                        sourceSize.height: height

                        fillMode: Image.PreserveAspectFit
                        asynchronous: true
                    }
                }

                Kirigami.Icon {
                    id: playIcon

                    anchors.centerIn: parent

                    source: (isPlaying === MediaPlayList.IsPlaying ?
                    Qt.resolvedUrl(elisaTheme.playingIndicatorIcon) : Qt.resolvedUrl(elisaTheme.pausedIndicatorIcon))

                    width: Kirigami.Units.iconSizes.smallMedium
                    height: Kirigami.Units.iconSizes.smallMedium

                    visible: isPlaying === MediaPlayList.IsPlaying || isPlaying === MediaPlayList.IsPaused

                    isMask: simpleMode
                    color: Kirigami.Theme.textColor
                }

                Loader {
                    active: playListEntry.grouped && isValid && !playIcon.visible
                    anchors.fill: parent

                    sourceComponent: Label {
                        //trackAndDiscNumberLabel

                        horizontalAlignment: Text.AlignRight

                        text: {
                            var trackNumberString;
                            if (trackNumber !== -1) {
                                trackNumberString = Number(trackNumber).toLocaleString(Qt.locale(), 'f', 0);
                            } else {
                                trackNumberString = ''
                            }
                            if (!isSingleDiscAlbum && discNumber !== 0 ) {
                                return trackNumberString + "/" + Number(discNumber).toLocaleString(Qt.locale(), 'f', 0)
                            } else {
                                return trackNumberString
                            }
                        }
                        textFormat: Text.PlainText
                        font.weight: (isPlaying ? Font.Bold : Font.Normal)
                    }
                }
            }

            ColumnLayout {
                spacing: 0
                Layout.fillWidth: true

                LabelWithToolTip {
                    text: title
                    font.weight: isPlaying ? Font.Bold : Font.Normal
                    visible: isValid
                    Layout.fillWidth: true
                }

                Loader {
                    active: !playListEntry.grouped && (artist || album)
                    visible: active
                    Layout.fillWidth: true
                    sourceComponent: LabelWithToolTip {
                        text: [artist, album].filter(Boolean).join(" - ")
                        type: Kirigami.Heading.Type.Secondary
                    }
                }
            }

            // button row
            Loader {
                active: actionsLoader.item && playListEntry.wideMode
                visible: active && !playListEntry.editingRating
                sourceComponent: Row {
                    FlatButtonWithToolTip {
                        id: locateFileButton
                        enabled: isValid
                        visible: playListEntry.wideMode && playListEntry.fileName.toString().substring(0, 7) === 'file://'
                        action: actionsLoader.item.locateFileAction
                    }

                    FlatButtonWithToolTip {
                        id: infoButton
                        objectName: 'infoButton'
                        enabled: isValid
                        visible: playListEntry.wideMode
                        action: actionsLoader.item.infoAction
                    }

                    FlatButtonWithToolTip {
                        id: ratingButton
                        objectName: 'ratingButton'
                        visible: playListEntry.wideMode && !ElisaApplication.useFavoriteStyleRatings
                        enabled: isValid
                        action: actionsLoader.item.ratingAction
                    }

                    FlatButtonWithToolTip {
                        objectName: 'favoriteButton'
                        visible: playListEntry.wideMode && ElisaApplication.useFavoriteStyleRatings
                        enabled: isValid
                        action: actionsLoader.item.favoriteAction
                    }

                    FlatButtonWithToolTip {
                        objectName: 'playPauseButton'
                        enabled: isValid
                        scale: LayoutMirroring.enabled ? -1 : 1 // We can mirror the symmetrical pause icon
                        visible: playListEntry.wideMode
                        action: actionsLoader.item.playPauseAction
                    }

                    FlatButtonWithToolTip {
                        id: removeButton
                        objectName: 'removeButton'
                        visible: playListEntry.wideMode
                        action: actionsLoader.item.removeAction
                    }
                }
            }

            FlatButtonWithToolTip {
                visible: playListEntry.editingRating && playListEntry.wideMode
                text: i18nc("Cancel rating this track", "Cancel rating this track")
                icon.name: "dialog-cancel"
                onClicked: { playListEntry.editingRating = false; }
            }

            RatingStar {
                id: ratingWidget

                readOnly: false
                starRating: rating

                visible: (playListEntry.editingRating || (rating > 0 && !containsMouse && !isSelected && !playListEntry.activeFocus && !simpleMode && !ElisaApplication.useFavoriteStyleRatings)) && playListEntry.wideMode

                onRatingEdited: {
                    ElisaApplication.musicManager.updateSingleFileMetaData(playListEntry.fileName, DataTypes.RatingRole, starRating);
                    playListEntry.editingRating = false;
                }
            }

            Kirigami.Icon {
                visible: rating == 10 && !containsMouse && !isSelected && !playListEntry.activeFocus && !simpleMode && ElisaApplication.useFavoriteStyleRatings

                implicitWidth: Kirigami.Units.iconSizes.smallMedium
                implicitHeight: Kirigami.Units.iconSizes.smallMedium

                source: "rating"
            }

            LabelWithToolTip {
                id: durationLabel
                text: duration
                font.weight: isPlaying ? Font.Bold : Font.Normal
            }

            Loader {
                id: menuButtonLoader
                active: !playListEntry.wideMode
                visible: active
                sourceComponent: FlatButtonWithToolTip {
                    icon.name: "overflow-menu"
                    down: pressed || menuLoader.menuVisible
                    onPressed: menuLoader.item.open()
                }
            }
            Loader {
                id: menuLoader
                property bool menuVisible: false
                active: !playListEntry.wideMode && actionsLoader.item
                onActiveChanged: {
                    if (!active) {
                        menuVisible = false
                    }
                }
                sourceComponent: Menu {
                    dim: false
                    modal: true
                    x: -width
                    parent: menuButtonLoader.item

                    onVisibleChanged: menuLoader.menuVisible = visible

                    MenuItem {
                        action: actionsLoader.item.locateFileAction
                    }
                    MenuItem {
                        action: actionsLoader.item.infoAction
                    }
                    MenuItem {
                        action: actionsLoader.item.playPauseAction
                    }
                    MenuItem {
                        action: actionsLoader.item.removeAction
                    }
                }
            }
        }

        states: [
        State {
            name: "dragging"
            when: playListEntry.listView.dragging
        },
        State {
            name: "menuVisible"
            when: menuLoader.menuVisible && !playListEntry.wideMode
            PropertyChanges {
                target: actionsLoader
                active: true
            }
        },
        State {
            name: 'notSelected'
            when: !containsMouse && !isSelected && !playListEntry.activeFocus && !simpleMode
            PropertyChanges {
                target: ratingWidget
                hoverWidgetOpacity: 0.0
            }
        },
        State {
            name: 'hovered'
            when: containsMouse && !playListEntry.activeFocus && !simpleMode
            PropertyChanges {
                target: actionsLoader
                active: true
            }
            PropertyChanges {
                target: ratingWidget
                hoverWidgetOpacity: 1.0
            }
        },
        State {
            name: 'selected'
            when: !playListEntry.activeFocus && isSelected && !simpleMode
            PropertyChanges {
                target: ratingWidget
                hoverWidgetOpacity: 1.0
            }
        },
        State {
            name: 'focused'
            when: playListEntry.activeFocus && !simpleMode
            PropertyChanges {
                target: actionsLoader
                active: true
            }
            PropertyChanges {
                target: ratingWidget
                hoverWidgetOpacity: 1.0
            }
        }
        ]
    }
}
