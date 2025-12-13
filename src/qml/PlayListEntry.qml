/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
import org.kde.kirigami as Kirigami
import org.kde.elisa

import "shared"

BasePlayListDelegate {
    id: playListEntry

    property bool editingRating: false
    // wideMode means there is enough room for the button row
    // otherwise display a menu button
    readonly property bool wideMode: width >= Theme.playListEntryMinWidth

    property var listDelegate

    readonly property string previousAlbum: listDelegate.ListView.previousSection ? JSON.parse(listDelegate.ListView.previousSection)[0] : null
    readonly property string currentAlbum: model.album || null
    readonly property string nextAlbum: listDelegate.ListView.nextSection ? JSON.parse(listDelegate.ListView.nextSection)[0] : null

    readonly property bool grouped: currentAlbum && (previousAlbum === currentAlbum || nextAlbum === currentAlbum)
    readonly property bool sectionVisible: currentAlbum && (previousAlbum !== currentAlbum && nextAlbum === currentAlbum)

    readonly property bool hasActiveFocus: playListEntry.activeFocus || entryFocusScope.activeFocus

    readonly property color textColor: highlighted || down ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
    readonly property color iconColor: textColor

    Accessible.role: Accessible.ListItem
    Accessible.name: title + ' ' + album + ' ' + artist

    Keys.onReturnPressed: {
        playListEntry.switchToTrack(playListEntry.index)
        playListEntry.startPlayback()
    }

    topPadding: grouped ? 0 : Kirigami.Units.smallSpacing
    bottomPadding: grouped ? 0 : Kirigami.Units.smallSpacing
    leftPadding: mirrored ? Kirigami.Units.smallSpacing : 0
    rightPadding: mirrored ? 0 : Kirigami.Units.smallSpacing

    // Set this explicitly since we rely on it; we don't want the theme setting
    // it to false under us!
    hoverEnabled: true

    onHasActiveFocusChanged: {
        if (!hasActiveFocus) {
            editingRating = false
        }
    }

    contentItem: FocusScope {
        id: entryFocusScope
        implicitHeight: childrenRect.height

        Loader {
            id: metadataLoader
            active: false
            onLoaded: (item as MediaTrackMetadataView).show()

            sourceComponent: MediaTrackMetadataView {
                fileName: playListEntry.fileName
                showImage: playListEntry.entryType !== ElisaUtils.Radio
                modelType: playListEntry.entryType
                showTrackFileName: playListEntry.entryType !== ElisaUtils.Radio
                showDeleteButton: playListEntry.entryType === ElisaUtils.Radio
                editableMetadata: playListEntry.metadataModifiableRole
                canAddMoreMetadata: playListEntry.entryType !== ElisaUtils.Radio

                onRejected: metadataLoader.active = false
            }
        }

        QtObject {
            id: actionList
            property var locateFileAction: Kirigami.Action {
                text: i18nc("@action:button Show the file for this song in the file manager", "Show in folder")
                icon.name: "document-open-folder"
                visible: playListEntry.fileName.toString().substring(0, 7) === 'file://'
                enabled: playListEntry.isValid
                onTriggered: ElisaApplication.showInFolder(playListEntry.fileName)
            }
            property var infoAction: Kirigami.Action {
                text: i18nc("@action:button Show track metadata", "View details")
                icon.name: "help-about"
                enabled: playListEntry.isValid
                onTriggered: {
                    if (metadataLoader.active === false) {
                        metadataLoader.active = true
                    }
                    else {
                        (metadataLoader.item as MediaTrackMetadataView).close();
                        metadataLoader.active = false
                    }
                }
            }
            property var ratingAction: Kirigami.Action {
                text: i18nc("@action:button", "Set track rating")
                icon.name: "view-media-favorite"
                visible: !ElisaApplication.useFavoriteStyleRatings
                enabled: playListEntry.isValid

                onTriggered: {
                    playListEntry.editingRating = true;
                }
            }
            property var favoriteAction: Kirigami.Action {
                text: playListEntry.isFavorite ? i18nc("@action:button", "Un-mark this song as a favorite") : i18nc("@action:button", "Mark this song as a favorite")
                icon.name: playListEntry.isFavorite ? "starred" : "non-starred"
                visible: ElisaApplication.useFavoriteStyleRatings
                enabled: playListEntry.isValid

                onTriggered: {
                    const newRating = playListEntry.isFavorite ? 0 : 10;
                    // Change icon immediately in case backend is slow
                    icon.name = playListEntry.isFavorite ? "non-starred" : "starred";
                    ElisaApplication.musicManager.updateSingleFileMetaData(playListEntry.fileName, DataTypes.RatingRole, newRating);
                }
            }
            property var playPauseAction: Kirigami.Action {
                text: (playListEntry.isPlaying === MediaPlayList.IsPlaying) ? i18nc("@action:button Pause current track from playlist", "Pause") : i18nc("@action:button Play this track from playlist", "Play")
                icon.name: (playListEntry.isPlaying === MediaPlayList.IsPlaying) ? "media-playback-pause" : "media-playback-start"
                enabled: playListEntry.isValid
                onTriggered: {
                    if (playListEntry.isPlaying === MediaPlayList.IsPlaying) {
                        playListEntry.pausePlayback()
                    } else if (playListEntry.isPlaying === MediaPlayList.IsPaused) {
                        playListEntry.startPlayback()
                    } else {
                        playListEntry.switchToTrack(playListEntry.index)
                        playListEntry.startPlayback()
                    }
                }
            }
            property var removeAction: Kirigami.Action {
                text: i18nc("@action:button Remove current track from play list", "Remove")
                icon.name: "edit-delete-remove"
                onTriggered: {
                    playListEntry.removeFromPlaylist(playListEntry.index)
                }
            }
        }

        RowLayout {
            id: trackRow

            width: parent.width
            // We want the list items to be a bit taller in touch mode
            height: Math.max(
                (playListEntry.grouped && !Kirigami.Settings.hasTransientTouchInput ? Kirigami.Units.gridUnit : Kirigami.Units.gridUnit * 2),
                (Theme.toolButtonHeight + Kirigami.Units.smallSpacing))

            spacing: Kirigami.Units.smallSpacing

            Loader {
                Layout.leftMargin: trackRow.spacing
                active: !playListEntry.simpleMode && playListEntry.showDragHandle
                sourceComponent: Kirigami.ListItemDragHandle {
                    listItem: playListEntry
                    listView: playListEntry.listView

                    onMoveRequested: (oldIndex, newIndex) => {
                        playListEntry.listView.draggingEntry = true
                        ElisaApplication.mediaPlayListProxyModel.moveRow(oldIndex, newIndex)
                    }
                    onDropped: {
                        playListEntry.listView.draggingEntry = false
                    }
                }
            }

            // Container for the play/pause icon and the track/disc label
            Item {
                Layout.preferredWidth: Math.max(Theme.trackNumberWidth, Theme.coverArtSize)
                Layout.fillHeight: true

                Loader {
                    active: !playListEntry.grouped
                    opacity: playIcon.visible ? 0.2 : 1
                    anchors.fill: parent

                    sourceComponent: ImageWithFallback {
                        source: playListEntry.imageUrl
                        fallback: Theme.defaultAlbumImage

                        sourceSize.width: height
                        sourceSize.height: height

                        fillMode: Image.PreserveAspectFit
                        asynchronous: true
                    }
                }

                Kirigami.Icon {
                    id: playIcon

                    anchors.centerIn: parent

                    source: playListEntry.isPlaying === MediaPlayList.IsPlaying ? "media-playback-playing" : "media-playback-pause"

                    width: Kirigami.Units.iconSizes.smallMedium
                    height: Kirigami.Units.iconSizes.smallMedium

                    visible: playListEntry.isPlaying === MediaPlayList.IsPlaying || playListEntry.isPlaying === MediaPlayList.IsPaused

                    isMask: playListEntry.simpleMode
                    color: playListEntry.iconColor
                }

                Loader {
                    active: playListEntry.grouped && playListEntry.isValid && !playIcon.visible
                    anchors.fill: parent

                    sourceComponent: Label {
                        //trackAndDiscNumberLabel

                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter

                        text: {
                            let trackNumberString = "";
                            if (playListEntry.trackNumber !== -1) {
                                trackNumberString = Number(playListEntry.trackNumber).toLocaleString(Qt.locale(), 'f', 0);
                            } else {
                                trackNumberString = ''
                            }
                            if (!playListEntry.isSingleDiscAlbum && playListEntry.discNumber !== 0 ) {
                                return trackNumberString + "/" + Number(playListEntry.discNumber).toLocaleString(Qt.locale(), 'f', 0)
                            } else {
                                return trackNumberString
                            }
                        }
                        textFormat: Text.PlainText
                        font.weight: (playListEntry.isPlaying ? Font.Bold : Font.Normal)
                        font.features: { "tnum": 1 }
                        color: playListEntry.textColor
                    }
                }
            }

            ColumnLayout {
                spacing: 0
                Layout.fillWidth: true

                LabelWithToolTip {
                    text: playListEntry.title
                    font.weight: playListEntry.isPlaying ? Font.Bold : Font.Normal
                    visible: playListEntry.isValid
                    Layout.fillWidth: true
                    color: playListEntry.textColor
                }

                Loader {
                    active: !playListEntry.grouped && (playListEntry.artist || playListEntry.album)
                    visible: active
                    Layout.fillWidth: true
                    sourceComponent: LabelWithToolTip {
                        text: [playListEntry.artist, playListEntry.album].filter(Boolean).join(" - ")
                        type: Kirigami.Heading.Type.Secondary
                        color: playListEntry.textColor
                    }
                }
            }

            // button row
            Loader {
                id: buttonRowLoader

                active: false
                visible: active && !playListEntry.editingRating

                sourceComponent: Row {
                    id: buttonRow

                    FlatButtonWithToolTip {
                        id: locateFileButton
                        action: actionList.locateFileAction
                        visible: actionList.locateFileAction.visible
                        icon.color: playListEntry.iconColor

                        KeyNavigation.left: favoriteButton
                        KeyNavigation.right: infoButton
                    }

                    FlatButtonWithToolTip {
                        id: infoButton
                        action: actionList.infoAction
                        visible: actionList.infoAction.visible
                        icon.color: playListEntry.iconColor

                        KeyNavigation.right: playPauseButton
                    }

                    FlatButtonWithToolTip {
                        id: playPauseButton
                        action: actionList.playPauseAction
                        visible: actionList.playPauseAction.visible
                        icon.color: playListEntry.iconColor

                        KeyNavigation.right: removeButton
                    }

                    FlatButtonWithToolTip {
                        id: removeButton
                        action: actionList.removeAction
                        visible: actionList.removeAction.visible
                        icon.color: playListEntry.iconColor

                        KeyNavigation.right: ratingButton
                    }

                    FlatButtonWithToolTip {
                        id: ratingButton
                        action: actionList.ratingAction
                        visible: actionList.ratingAction.visible
                        icon.color: playListEntry.iconColor

                        KeyNavigation.right: favoriteButton
                    }

                    FlatButtonWithToolTip {
                        id: favoriteButton
                        action: actionList.favoriteAction
                        visible: actionList.favoriteAction.visible
                        icon.color: playListEntry.iconColor

                        KeyNavigation.right: locateFileButton
                    }

                    data: Binding {
                        playListEntry.KeyNavigation.right: buttonRow.children[0]
                        playListEntry.KeyNavigation.left: buttonRow.children[buttonRow.children.length - 1]
                    }
                }
            }

            FlatButtonWithToolTip {
                visible: playListEntry.editingRating && playListEntry.wideMode
                text: i18nc("@action:button", "Cancel rating this track")
                icon.name: "dialog-cancel"
                icon.color: playListEntry.iconColor
                onClicked: { playListEntry.editingRating = false; }
            }

            RatingStar {
                id: ratingWidget

                readOnly: !playListEntry.editingRating
                starRating: playListEntry.rating

                visible: (playListEntry.editingRating || (playListEntry.rating > 0 && !playListEntry.hovered && !playListEntry.hasActiveFocus && !playListEntry.simpleMode && !ElisaApplication.useFavoriteStyleRatings)) && playListEntry.wideMode

                iconColor: playListEntry.iconColor

                onRatingEdited: {
                    ElisaApplication.musicManager.updateSingleFileMetaData(playListEntry.fileName, DataTypes.RatingRole, starRating);
                    playListEntry.editingRating = false;
                }
            }

            Loader {
                id: favoriteMark

                visible: playListEntry.isFavorite && !playListEntry.hovered && !playListEntry.hasActiveFocus && !playListEntry.simpleMode && ElisaApplication.useFavoriteStyleRatings

                sourceComponent: FlatButtonWithToolTip {
                    visible: action.visible
                    action: actionList.favoriteAction
                    icon.color: playListEntry.iconColor
                }
            }


            LabelWithToolTip {
                id: durationLabel
                text: playListEntry.duration
                font.weight: playListEntry.isPlaying ? Font.Bold : Font.Normal
                font.features: { "tnum": 1 }
                color: playListEntry.textColor
            }

            Loader {
                id: menuButtonLoader
                active: !playListEntry.wideMode || Kirigami.Settings.hasTransientTouchInput
                visible: active
                sourceComponent: FlatButtonWithToolTip {
                    id: menuButton

                    icon.name: "overflow-menu"
                    icon.color: playListEntry.iconColor
                    text: playListEntry.entryType === ElisaUtils.Track ? i18nc("@action:button", "Track Options") : i18nc("@action:button", "Radio Options")
                    down: pressed || menuLoader.menuVisible
                    onPressed: (menuLoader.item as Menu).open()
                    Keys.onReturnPressed: (menuLoader.item as Menu).open()
                    Keys.onEnterPressed: (menuLoader.item as Menu).open()
                    activeFocusOnTab: playListEntry.isSelected

                    Binding {
                        playListEntry.KeyNavigation.right: menuButton
                        playListEntry.KeyNavigation.left: menuButton
                    }
                }
            }
            Loader {
                id: menuLoader
                property bool menuVisible: false
                active: menuButtonLoader.active
                onActiveChanged: {
                    if (!active) {
                        menuVisible = false
                    }
                }
                sourceComponent: Menu {
                    dim: false
                    modal: true
                    x: -width
                    parent: menuButtonLoader.item as FlatButtonWithToolTip

                    onVisibleChanged: menuLoader.menuVisible = visible

                    MenuItem {
                        action: actionList.playPauseAction
                    }
                    MenuItem {
                        action: actionList.infoAction
                    }
                    MenuItem {
                        action: actionList.locateFileAction
                    }
                    MenuItem {
                        action: actionList.ratingAction
                        visible: action.visible
                    }
                    MenuItem {
                        action: actionList.favoriteAction
                        visible: action.visible
                    }
                    MenuSeparator { }
                    MenuItem {
                        action: actionList.removeAction
                    }
                }
            }
        }

        states: [
            State {
                name: "dragging"
                when: playListEntry.listView.draggingEntry
            },
            State {
                name: 'hovered'
                when: playListEntry.hovered && !playListEntry.hasActiveFocus && !playListEntry.simpleMode
                PropertyChanges {
                    buttonRowLoader.active: playListEntry.wideMode
                }
            },
            State {
                name: 'focused'
                when: playListEntry.hasActiveFocus && !playListEntry.simpleMode
                PropertyChanges {
                    buttonRowLoader.active: playListEntry.wideMode
                }
            }
        ]
    }
}
