/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import ElisaGraphicalEffects 1.15
import org.kde.kirigami 2.17 as Kirigami
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

    readonly property bool hasActiveFocus: playListEntry.activeFocus || buttonRowLoader.activeFocus || menuButtonLoader.activeFocus

    Accessible.role: Accessible.ListItem
    Accessible.name: title + ' ' + album + ' ' + artist

    Keys.onReturnPressed: {
        playListEntry.switchToTrack(playListEntry.index)
        playListEntry.startPlayback()
    }

    activeFocusOnTab: isSelected

    KeyNavigation.right: (buttonRowLoader.item && buttonRowLoader.item.children[0]) || menuButtonLoader.item || null
    KeyNavigation.left:  (buttonRowLoader.item && buttonRowLoader.item.children[buttonRowLoader.item.children.length -1]) || menuButtonLoader.item || null

    topPadding: grouped ? 0 : Kirigami.Units.smallSpacing
    bottomPadding: grouped ? 0 : Kirigami.Units.smallSpacing
    leftPadding: mirrored ? Kirigami.Units.smallSpacing : 0
    rightPadding: mirrored ? 0 : Kirigami.Units.smallSpacing
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

        QtObject {
            id: actionList
            property var locateFileAction: Action {
                text: i18nc("@action:button Show the file for this song in the file manager", "Show in folder")
                icon.name: "document-open-folder"
                onTriggered: {
                    ElisaApplication.showInFolder(playListEntry.fileName)
                }
            }
            property var infoAction: Action {
                text: i18nc("@action:button Show track metadata", "View details")
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
                text: i18nc("@action:button", "Set track rating")
                icon.name: "view-media-favorite"

                onTriggered: {
                    playListEntry.editingRating = true;
                }
            }
            property var favoriteAction: Action {
                text: rating == 10 ? i18nc("@action:button", "Un-mark this song as a favorite") : i18nc("@action:button", "Mark this song as a favorite")
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
                text: (isPlaying === MediaPlayList.IsPlaying) ? i18nc("@action:button Pause current track from playlist", "Pause") : i18nc("@action:button Play this track from playlist", "Play")
                icon.name: (isPlaying === MediaPlayList.IsPlaying) ? "media-playback-pause" : "media-playback-start"
                onTriggered: {
                    if (isPlaying === MediaPlayList.IsPlaying) {
                        playListEntry.pausePlayback()
                    } else if (isPlaying === MediaPlayList.IsPaused) {
                        playListEntry.startPlayback()
                    } else {
                        playListEntry.switchToTrack(playListEntry.index)
                        playListEntry.startPlayback()
                    }
                }
            }
            property var removeAction: Action {
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
            height: Math.max((playListEntry.grouped && !Kirigami.Settings.hasTransientTouchInput ? Kirigami.Units.gridUnit : Kirigami.Units.gridUnit * 2), elisaTheme.toolButtonHeight)

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
                id: buttonRowLoader

                active: false
                visible: active && !playListEntry.editingRating

                sourceComponent: Row {
                    FlatButtonWithToolTip {
                        id: locateFileButton
                        objectName: 'locateFileButton'
                        enabled: isValid
                        visible: playListEntry.wideMode && playListEntry.fileName.toString().substring(0, 7) === 'file://'
                        action: actionList.locateFileAction
                        KeyNavigation.left: favoriteButton
                        KeyNavigation.right: infoButton
                        activeFocusOnTab: isSelected
                    }

                    FlatButtonWithToolTip {
                        id: infoButton
                        objectName: 'infoButton'
                        enabled: isValid
                        visible: playListEntry.wideMode
                        action: actionList.infoAction
                        KeyNavigation.right: playPauseButton
                        activeFocusOnTab: isSelected
                    }

                    FlatButtonWithToolTip {
                        id: playPauseButton
                        objectName: 'playPauseButton'
                        enabled: isValid
                        scale: LayoutMirroring.enabled ? -1 : 1 // We can mirror the symmetrical pause icon
                        visible: playListEntry.wideMode
                        action: actionList.playPauseAction
                        KeyNavigation.right: removeButton
                        activeFocusOnTab: isSelected
                    }

                    FlatButtonWithToolTip {
                        id: removeButton
                        objectName: 'removeButton'
                        visible: playListEntry.wideMode
                        action: actionList.removeAction
                        KeyNavigation.right: ratingButton
                        activeFocusOnTab: isSelected
                    }

                    FlatButtonWithToolTip {
                        id: ratingButton
                        objectName: 'ratingButton'
                        visible: playListEntry.wideMode && !ElisaApplication.useFavoriteStyleRatings
                        enabled: isValid
                        action: actionList.ratingAction
                        KeyNavigation.right: favoriteButton
                        activeFocusOnTab: isSelected
                    }

                    FlatButtonWithToolTip {
                        id: favoriteButton
                        objectName: 'favoriteButton'
                        visible: playListEntry.wideMode && ElisaApplication.useFavoriteStyleRatings
                        enabled: isValid
                        action: actionList.favoriteAction
                        KeyNavigation.right: locateFileButton
                        activeFocusOnTab: isSelected
                    }
                }
            }

            FlatButtonWithToolTip {
                visible: playListEntry.editingRating && playListEntry.wideMode
                text: i18nc("@action:button", "Cancel rating this track")
                icon.name: "dialog-cancel"
                onClicked: { playListEntry.editingRating = false; }
            }

            RatingStar {
                id: ratingWidget

                readOnly: false
                starRating: rating

                visible: (playListEntry.editingRating || (rating > 0 && !containsMouse && !playListEntry.hasActiveFocus && !simpleMode && !ElisaApplication.useFavoriteStyleRatings)) && playListEntry.wideMode

                onRatingEdited: {
                    ElisaApplication.musicManager.updateSingleFileMetaData(playListEntry.fileName, DataTypes.RatingRole, starRating);
                    playListEntry.editingRating = false;
                }
            }

            Loader {
                id: favoriteMark

                visible: rating == 10 && !containsMouse && !playListEntry.hasActiveFocus && !simpleMode && ElisaApplication.useFavoriteStyleRatings

                sourceComponent: Row {
                    FlatButtonWithToolTip {
                        visible: playListEntry.wideMode && ElisaApplication.useFavoriteStyleRatings
                        enabled: isValid
                        action: actionList.favoriteAction
                    }
                }
            }


            LabelWithToolTip {
                id: durationLabel
                text: duration
                font.weight: isPlaying ? Font.Bold : Font.Normal
            }

            Loader {
                id: menuButtonLoader
                active: !playListEntry.wideMode || Kirigami.Settings.hasTransientTouchInput
                visible: active
                sourceComponent: FlatButtonWithToolTip {
                    icon.name: "overflow-menu"
                    down: pressed || menuLoader.menuVisible
                    onPressed: menuLoader.item.open()
                    Keys.onReturnPressed: menuLoader.item.open()
                    Keys.onEnterPressed: menuLoader.item.open()
                    activeFocusOnTab: isSelected
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
                    parent: menuButtonLoader.item

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
                        visible: !ElisaApplication.useFavoriteStyleRatings
                    }
                    MenuItem {
                        action: actionList.favoriteAction
                        visible: ElisaApplication.useFavoriteStyleRatings
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
            when: playListEntry.listView.dragging
        },
        State {
            name: 'hovered'
            when: containsMouse && !playListEntry.hasActiveFocus && !simpleMode
            PropertyChanges {
                target: buttonRowLoader
                active: playListEntry.wideMode
            }
        },
        State {
            name: 'focused'
            when: playListEntry.hasActiveFocus && !simpleMode
            PropertyChanges {
                target: buttonRowLoader
                active: playListEntry.wideMode
            }
        }
        ]
    }
}
