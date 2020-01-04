/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
    id: playListEntry

    property var index
    property bool isSingleDiscAlbum
    property int isPlaying
    property bool isSelected
    property bool isValid
    property bool isAlternateColor
    property bool containsMouse
    property int databaseId: 0
    property var entryType
    property string title
    property string artist
    property string album
    property string albumArtist
    property string duration
    property url fileName
    property url imageUrl
    property int trackNumber
    property int discNumber
    property int rating
    property bool hasValidDiscNumber: true
    property int scrollBarWidth
    property bool simpleMode: false

    signal startPlayback()
    signal pausePlayback()
    signal removeFromPlaylist(var trackIndex)
    signal switchToTrack(var trackIndex)

    Accessible.role: Accessible.ListItem
    Accessible.name: title + ' ' + album + ' ' + artist

    Keys.onReturnPressed: playNow.trigger()

    height: elisaTheme.playListDelegateHeight

    Loader {
        id: metadataLoader
        active: false
        onLoaded: item.show()

        sourceComponent:  MediaTrackMetadataView {
            initialDatabaseId: playListEntry.databaseId
            fileName: playListEntry.fileName
            showImage: entryType !== ElisaUtils.Radio
            modelType: entryType
            showTrackFileName: entryType !== ElisaUtils.Radio
            showDeleteButton: entryType === ElisaUtils.Radio
            showApplyButton: entryType === ElisaUtils.Radio
            editableMetadata: entryType === ElisaUtils.Radio

            onRejected: metadataLoader.active = false
        }
    }

    Rectangle {
        id: entryBackground

        anchors.fill: parent
        anchors.rightMargin: LayoutMirroring.enabled ? scrollBarWidth : 0
        z: 1

        color: simpleMode ? "transparent" : myPalette.base

        height: elisaTheme.playListDelegateHeight
    }

    RowLayout {
        id: trackRow
        z: 2

        anchors.fill: parent
        anchors.leftMargin: elisaTheme.layoutHorizontalMargin
        anchors.rightMargin: LayoutMirroring.enabled ? scrollBarWidth : 0

        spacing: elisaTheme.layoutHorizontalMargin / 4

        Item {
            id: playIconItem

            implicitHeight: elisaTheme.playListDelegateHeight
            implicitWidth: elisaTheme.playListDelegateHeight
            Layout.maximumWidth: elisaTheme.playListDelegateHeight
            Layout.maximumHeight: elisaTheme.playListDelegateHeight
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin / 2 : 0
            Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin / 2 : 0

            Image {
                id: playIcon

                anchors.centerIn: parent

                visible: isPlaying === MediaPlayList.IsPlaying || isPlaying === MediaPlayList.IsPaused

                source: (isPlaying === MediaPlayList.IsPlaying ?
                             Qt.resolvedUrl(elisaTheme.playingIndicatorIcon) : Qt.resolvedUrl(elisaTheme.pausedIndicatorIcon))

                width: elisaTheme.smallControlButtonSize
                height: elisaTheme.smallControlButtonSize

                sourceSize.width: elisaTheme.smallControlButtonSize
                sourceSize.height: elisaTheme.smallControlButtonSize
                fillMode: Image.PreserveAspectFit
                mirror: LayoutMirroring.enabled

                layer.enabled: simpleMode
                layer.effect: ColorOverlay {
                    cached: true

                    color: myPalette.highlightedText
                }
            }
        }

        Item {
            id: fakeDiscNumberItem

            visible: isValid && (!hasValidDiscNumber || isSingleDiscAlbum)

            Layout.preferredWidth: (fakeDiscNumberSize.boundingRect.width - fakeDiscNumberSize.boundingRect.x) + (elisaTheme.layoutHorizontalMargin / 4)
            Layout.minimumWidth: (fakeDiscNumberSize.boundingRect.width - fakeDiscNumberSize.boundingRect.x) + (elisaTheme.layoutHorizontalMargin / 4)
            Layout.maximumWidth: (fakeDiscNumberSize.boundingRect.width - fakeDiscNumberSize.boundingRect.x) + (elisaTheme.layoutHorizontalMargin / 4)

            TextMetrics {
                id: fakeDiscNumberSize

                text: '/9'
            }
        }

        Label {
            id: trackNumberLabel

            horizontalAlignment: Text.AlignRight

            text: trackNumber !== -1 ? Number(trackNumber).toLocaleString(Qt.locale(), 'f', 0) : ''

            font.weight: (isPlaying ? Font.Bold : Font.Light)
            color: simpleMode ? myPalette.highlightedText : myPalette.text

            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

            visible: isValid

            Layout.preferredWidth: ((trackNumberSize.boundingRect.width - trackNumberSize.boundingRect.x) > (realTrackNumberSize.boundingRect.width - realTrackNumberSize.boundingRect.x) ? (trackNumberSize.boundingRect.width - trackNumberSize.boundingRect.x) : (realTrackNumberSize.boundingRect.width - realTrackNumberSize.boundingRect.x))
            Layout.minimumWidth: ((trackNumberSize.boundingRect.width - trackNumberSize.boundingRect.x) > (realTrackNumberSize.boundingRect.width - realTrackNumberSize.boundingRect.x) ? (trackNumberSize.boundingRect.width - trackNumberSize.boundingRect.x) : (realTrackNumberSize.boundingRect.width - realTrackNumberSize.boundingRect.x))
            Layout.maximumWidth: ((trackNumberSize.boundingRect.width - trackNumberSize.boundingRect.x) > (realTrackNumberSize.boundingRect.width - realTrackNumberSize.boundingRect.x) ? (trackNumberSize.boundingRect.width - trackNumberSize.boundingRect.x) : (realTrackNumberSize.boundingRect.width - realTrackNumberSize.boundingRect.x))

            Layout.rightMargin: !LayoutMirroring.enabled ? (discNumber !== 0 && !isSingleDiscAlbum ?
                                                                0 : elisaTheme.layoutHorizontalMargin / 2) : 0
            Layout.leftMargin: LayoutMirroring.enabled ? (discNumber !== 0 && !isSingleDiscAlbum ?
                                                              0 : elisaTheme.layoutHorizontalMargin / 2) : 0

            TextMetrics {
                id: trackNumberSize

                text: (99).toLocaleString(Qt.locale(), 'f', 0)
            }

            TextMetrics {
                id: realTrackNumberSize

                text: Number(trackNumber).toLocaleString(Qt.locale(), 'f', 0)
            }
        }

        Label {
            horizontalAlignment: Text.AlignCenter

            text: '/'

            visible: isValid && discNumber !== 0 && !isSingleDiscAlbum

            font.weight: (isPlaying ? Font.Bold : Font.Light)
            color: simpleMode ? myPalette.highlightedText : myPalette.text

            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

            Layout.preferredWidth: (numberSeparatorSize.boundingRect.width - numberSeparatorSize.boundingRect.x)
            Layout.minimumWidth: (numberSeparatorSize.boundingRect.width - numberSeparatorSize.boundingRect.x)
            Layout.maximumWidth: (numberSeparatorSize.boundingRect.width - numberSeparatorSize.boundingRect.x)

            TextMetrics {
                id: numberSeparatorSize

                text: '/'
            }
        }

        Label {
            horizontalAlignment: Text.AlignRight

            font.weight: (isPlaying ? Font.Bold : Font.Light)
            color: simpleMode ? myPalette.highlightedText : myPalette.text

            text: Number(discNumber).toLocaleString(Qt.locale(), 'f', 0)

            visible: isValid && discNumber !== 0 && !isSingleDiscAlbum

            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

            Layout.preferredWidth: ((discNumberSize.boundingRect.width - discNumberSize.boundingRect.x) > (realDiscNumberSize.boundingRect.width - realDiscNumberSize.boundingRect.x) ?
                                        (discNumberSize.boundingRect.width - discNumberSize.boundingRect.x) :
                                        (realDiscNumberSize.boundingRect.width - realDiscNumberSize.boundingRect.x))
            Layout.minimumWidth: ((discNumberSize.boundingRect.width - discNumberSize.boundingRect.x) > (realDiscNumberSize.boundingRect.width - realDiscNumberSize.boundingRect.x) ?
                                      (discNumberSize.boundingRect.width - discNumberSize.boundingRect.x) :
                                      (realDiscNumberSize.boundingRect.width - realDiscNumberSize.boundingRect.x))
            Layout.maximumWidth: ((discNumberSize.boundingRect.width - discNumberSize.boundingRect.x) > (realDiscNumberSize.boundingRect.width - realDiscNumberSize.boundingRect.x) ?
                                      (discNumberSize.boundingRect.width - discNumberSize.boundingRect.x) :
                                      (realDiscNumberSize.boundingRect.width - realDiscNumberSize.boundingRect.x))

            Layout.rightMargin: !LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin / 2) : 0
            Layout.leftMargin: LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin / 2) : 0

            TextMetrics {
                id: discNumberSize

                text: '9'
            }

            TextMetrics {
                id: realDiscNumberSize

                text: Number(discNumber).toLocaleString(Qt.locale(), 'f', 0)
            }
        }

        LabelWithToolTip {
            id: mainCompactLabel

            text: title

            font.weight: (isPlaying ? Font.Bold : Font.Normal)
            color: simpleMode ? myPalette.highlightedText : myPalette.text

            Layout.maximumWidth: mainCompactLabel.implicitWidth + 1
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

            visible: isValid

            elide: Text.ElideRight
            horizontalAlignment: Text.AlignLeft
        }

        LabelWithToolTip {
            id: mainInvalidCompactLabel

            text: title

            color: simpleMode ? myPalette.highlightedText : myPalette.text

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

            visible: !isValid

            elide: Text.ElideRight
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredWidth: 0
        }

        Loader {
            id: hoverLoader
            active: false
            visible: active

            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

            sourceComponent: Row {
                anchors.centerIn: parent
                enabled: isValid

                FlatButtonWithToolTip {
                    id: infoButton
                    objectName: 'infoButton'

                    implicitHeight: elisaTheme.playListDelegateHeight
                    implicitWidth: elisaTheme.playListDelegateHeight

                    text: i18nc("Show track metadata", "View Details")
                    icon.name: "help-about"
                    onClicked: {
                        if (metadataLoader.active === false) {
                            metadataLoader.active = true
                        }
                        else {
                            metadataLoader.item.close();
                            metadataLoader.active = false
                        }
                    }
                }

                FlatButtonWithToolTip {
                    id: playPauseButton
                    objectName: 'playPauseButton'

                    implicitHeight: elisaTheme.playListDelegateHeight
                    implicitWidth: elisaTheme.playListDelegateHeight

                    scale: LayoutMirroring.enabled ? -1 : 1 // We can mirror the symmetrical pause icon

                    text: (isPlaying === MediaPlayList.IsPlaying) ? i18nc("Pause current track from play list", "Pause") : i18nc("Play this track from play list", "Play")
                    icon.name: (isPlaying === MediaPlayList.IsPlaying) ? "media-playback-pause" : "media-playback-start"
                    onClicked: if (isPlaying === MediaPlayList.IsPlaying) {
                        playListEntry.pausePlayback()
                    } else {
                        playListEntry.switchToTrack(playListEntry.index)
                        playListEntry.startPlayback()
                    }
                }

                FlatButtonWithToolTip {
                    id: removeButton
                    objectName: 'removeButton'

                    implicitHeight: elisaTheme.playListDelegateHeight
                    implicitWidth: elisaTheme.playListDelegateHeight

                    text: i18nc("Remove current track from play list", "Remove")
                    icon.name: "error"
                    onClicked: playListEntry.removeFromPlaylist(playListEntry.index)
                }

            }
        }

        RatingStar {
            id: ratingWidget

            starRating: rating

            starSize: elisaTheme.ratingStarSize

            visible: rating > 0
        }

        LabelWithToolTip {
            id: durationLabel

            text: duration

            font.weight: (isPlaying ? Font.Bold : Font.Normal)
            color: simpleMode ? myPalette.highlightedText : myPalette.text

            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            Layout.leftMargin: elisaTheme.layoutHorizontalMargin
            Layout.rightMargin: elisaTheme.layoutHorizontalMargin

            horizontalAlignment: Text.AlignRight
        }
    }

    states: [
        State {
            name: 'notSelected'
            when: !containsMouse && !isSelected && !playListEntry.activeFocus && !simpleMode
            PropertyChanges {
                target: hoverLoader
                active: false
            }
            PropertyChanges {
                target: entryBackground
                color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)
            }
            PropertyChanges {
                target: entryBackground
                opacity: 1.
            }
            PropertyChanges {
                target: ratingWidget
                hoverWidgetOpacity: 0.0
            }
        },
        State {
            name: 'hovered'
            when: containsMouse && !playListEntry.activeFocus && !simpleMode
            PropertyChanges {
                target: hoverLoader
                active: true
            }
            PropertyChanges {
                target: entryBackground
                color: myPalette.highlight
            }
            PropertyChanges {
                target: entryBackground
                opacity: 0.2
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
                target: hoverLoader
                active: false
            }
            PropertyChanges {
                target: entryBackground
                color: myPalette.mid
            }
            PropertyChanges {
                target: entryBackground
                opacity: 1.
            }
            PropertyChanges {
                target: ratingWidget
                hoverWidgetOpacity: 1.0
            }
        },
        State {
            name: 'focused'
            when: playListEntry.activeFocus && !simpleMode
            PropertyChanges {
                target: hoverLoader
                active: true
            }
            PropertyChanges {
                target: entryBackground
                color: myPalette.highlight
            }
            PropertyChanges {
                target: entryBackground
                opacity: 0.6
            }
            PropertyChanges {
                target: ratingWidget
                hoverWidgetOpacity: 1.0
            }
        }
    ]
}

