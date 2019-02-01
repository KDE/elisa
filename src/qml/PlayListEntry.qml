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
import QtQuick.Controls 1.4 as Controls1
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
    property bool noBackground: false

    signal startPlayback()
    signal pausePlayback()
    signal removeFromPlaylist(var trackIndex)
    signal switchToTrack(var trackIndex)

    height: elisaTheme.playListDelegateHeight

    Controls1.Action {
        id: removeFromPlayList
        text: i18nc("Remove current track from play list", "Remove")
        iconName: "error"
        onTriggered: {
            playListEntry.removeFromPlaylist(playListEntry.index)
        }
    }

    Controls1.Action {
        id: playNow
        text: i18nc("Play now current track from play list", "Play Now")
        iconName: "media-playback-start"
        enabled: !(isPlaying === MediaPlayList.IsPlaying) && isValid
        onTriggered: {
            if (isPlaying === MediaPlayList.NotPlaying) {
                playListEntry.switchToTrack(playListEntry.index)
            }
            playListEntry.startPlayback()
        }
    }

    Controls1.Action {
        id: pauseNow
        text: i18nc("Pause current track from play list", "Pause")
        iconName: "media-playback-pause"
        enabled: isPlaying == MediaPlayList.IsPlaying && isValid
        onTriggered: playListEntry.pausePlayback()
    }

    Controls1.Action {
        id: showInfo
        text: i18nc("Show track metadata", "View Details")
        iconName: "help-about"
        enabled: isValid
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

    Loader {
        id: metadataLoader
        active: false
        onLoaded: item.show()

        sourceComponent:  MediaTrackMetadataView {
            databaseId: playListEntry.databaseId
            fileName: playListEntry.fileName
            onRejected: metadataLoader.active = false;
        }
    }

    Rectangle {
        id: entryBackground

        anchors.fill: parent
        anchors.rightMargin: LayoutMirroring.enabled ? scrollBarWidth : 0

        color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)

        height: elisaTheme.playListDelegateHeight

        focus: true

        ColumnLayout {
            spacing: 0

            anchors.fill: parent

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                RowLayout {
                    id: trackRow

                    anchors.fill: parent

                    spacing: elisaTheme.layoutHorizontalMargin / 4

                    Item {
                        id: playIconItem

                        implicitHeight: elisaTheme.smallDelegateToolButtonSize
                        implicitWidth: elisaTheme.smallDelegateToolButtonSize
                        Layout.maximumWidth: elisaTheme.smallDelegateToolButtonSize
                        Layout.maximumHeight: elisaTheme.smallDelegateToolButtonSize
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                        Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin / 2 : 0
                        Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin / 2 : 0

                        Image {
                            id: playIcon

                            anchors.fill: parent

                            opacity: 0

                            source: (isPlaying === MediaPlayList.IsPlaying ?
                                         Qt.resolvedUrl(elisaTheme.playingIndicatorIcon) : Qt.resolvedUrl(elisaTheme.pausedIndicatorIcon))

                            width: parent.height * 1.
                            height: parent.height * 1.

                            sourceSize.width: parent.height * 1.
                            sourceSize.height: parent.height * 1.
                            fillMode: Image.PreserveAspectFit
                            mirror: LayoutMirroring.enabled
                            visible: opacity > 0.0
                        }
                    }

                    Item {
                        id: fakeDiscNumberItem

                        visible: isValid && (!hasValidDiscNumber || isSingleDiscAlbum)

                        Layout.preferredWidth: fakeDiscNumberSize.width + (elisaTheme.layoutHorizontalMargin / 4)
                        Layout.minimumWidth: fakeDiscNumberSize.width + (elisaTheme.layoutHorizontalMargin / 4)
                        Layout.maximumWidth: fakeDiscNumberSize.width + (elisaTheme.layoutHorizontalMargin / 4)

                        TextMetrics {
                            id: fakeDiscNumberSize

                            text: '/9'
                        }
                    }

                    Label {
                        id: trackNumberLabel

                        horizontalAlignment: Text.AlignRight

                        text: trackNumber !== 0 ? Number(trackNumber).toLocaleString(Qt.locale(), 'f', 0) : ''

                        font.weight: (isPlaying ? Font.Bold : Font.Light)
                        color: myPalette.text

                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                        visible: isValid

                        Layout.preferredWidth: (trackNumberSize.width > realTrackNumberSize.width ? trackNumberSize.width : realTrackNumberSize.width)
                        Layout.minimumWidth: (trackNumberSize.width > realTrackNumberSize.width ? trackNumberSize.width : realTrackNumberSize.width)
                        Layout.maximumWidth: (trackNumberSize.width > realTrackNumberSize.width ? trackNumberSize.width : realTrackNumberSize.width)

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
                        color: myPalette.text

                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                        Layout.preferredWidth: numberSeparatorSize.width
                        Layout.minimumWidth: numberSeparatorSize.width
                        Layout.maximumWidth: numberSeparatorSize.width

                        TextMetrics {
                            id: numberSeparatorSize

                            text: '/'
                        }
                    }

                    Label {
                        horizontalAlignment: Text.AlignRight

                        font.weight: (isPlaying ? Font.Bold : Font.Light)
                        color: myPalette.text

                        text: Number(discNumber).toLocaleString(Qt.locale(), 'f', 0)

                        visible: isValid && discNumber !== 0 && !isSingleDiscAlbum

                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                        Layout.preferredWidth: (discNumberSize.width > realDiscNumberSize.width ? discNumberSize.width : realDiscNumberSize.width)
                        Layout.minimumWidth: (discNumberSize.width > realDiscNumberSize.width ? discNumberSize.width : realDiscNumberSize.width)
                        Layout.maximumWidth: (discNumberSize.width > realDiscNumberSize.width ? discNumberSize.width : realDiscNumberSize.width)

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
                        color: myPalette.text

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

                        font.weight: Font.Normal
                        color: myPalette.text

                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                        visible: !isValid

                        elide: Text.ElideRight
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 0
                    }

                    Controls1.ToolButton {
                        id: infoButton
                        objectName: 'infoButton'

                        implicitHeight: elisaTheme.smallDelegateToolButtonSize
                        implicitWidth: elisaTheme.smallDelegateToolButtonSize

                        opacity: 0

                        visible: opacity > 0.1

                        action: showInfo
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }

                    Controls1.ToolButton {
                        id: playPauseButton
                        objectName: 'playPauseButton'

                        implicitHeight: elisaTheme.smallDelegateToolButtonSize
                        implicitWidth: elisaTheme.smallDelegateToolButtonSize

                        opacity: 0

                        scale: LayoutMirroring.enabled ? -1 : 1 // We can mirror the symmetrical pause icon

                        visible: opacity > 0.1
                        action: !(isPlaying === MediaPlayList.IsPlaying) ? playNow : pauseNow
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }

                    Item {
                        implicitHeight: elisaTheme.smallDelegateToolButtonSize
                        implicitWidth: elisaTheme.smallDelegateToolButtonSize
                        Layout.maximumWidth: elisaTheme.smallDelegateToolButtonSize
                        Layout.maximumHeight: elisaTheme.smallDelegateToolButtonSize
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                        Controls1.ToolButton {
                            id: removeButton
                            objectName: 'removeButton'

                            anchors.fill: parent

                            opacity: 0

                            visible: opacity > 0.1
                            action: removeFromPlayList
                        }
                    }

                    RatingStar {
                        id: ratingWidget

                        starRating: rating

                        starSize: elisaTheme.ratingStarSize
                    }

                    TextMetrics {
                        id: durationTextMetrics
                        text: i18nc("This is used to preserve a fixed width for the duration text.", "00:00")
                    }

                    LabelWithToolTip {
                        id: durationLabel

                        text: duration

                        font.weight: (isPlaying ? Font.Bold : Font.Normal)
                        color: myPalette.text

                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                        Layout.preferredWidth: durationTextMetrics.width + 1
                        Layout.leftMargin: elisaTheme.layoutHorizontalMargin / 2
                        Layout.rightMargin: elisaTheme.layoutHorizontalMargin / 2

                        horizontalAlignment: Text.AlignRight
                    }
                }
            }
        }
    }

    states: [
        State {
            name: 'notSelected'
            when: !containsMouse && (!playListEntry.activeFocus || !isSelected)
            PropertyChanges {
                target: removeButton
                opacity: 0
            }
            PropertyChanges {
                target: infoButton
                opacity: 0
            }
            PropertyChanges {
                target: playPauseButton
                opacity: 0
            }
            PropertyChanges {
                target: playIcon
                opacity: (isPlaying === MediaPlayList.IsPlaying || isPlaying === MediaPlayList.IsPaused ? 1.0 : 0.0)
            }
            PropertyChanges {
                target: entryBackground
                color: (isAlternateColor ? myPalette.alternateBase : myPalette.base)
            }
            PropertyChanges {
                target: ratingWidget
                hoverWidgetOpacity: 0.0
            }
        },
        State {
            name: 'hoveredOrSelected'
            when: containsMouse || (playListEntry.activeFocus && isSelected)
            PropertyChanges {
                target: removeButton
                opacity: 1
            }
            PropertyChanges {
                target: playPauseButton
                opacity: 1
            }
            PropertyChanges {
                target: infoButton
                opacity: 1
            }
            PropertyChanges {
                target: playIcon
                opacity: (isPlaying === MediaPlayList.IsPlaying || isPlaying === MediaPlayList.IsPaused ? 1.0 : 0.0)
            }
            PropertyChanges {
                target: entryBackground
                color: myPalette.mid
            }
            PropertyChanges {
                target: ratingWidget
                hoverWidgetOpacity: 1.0
            }
        }
    ]
    transitions: Transition {
        ParallelAnimation {
            NumberAnimation {
                properties: "opacity, hoverWidgetOpacity"
                easing.type: Easing.InOutQuad
                duration: 250
            }
            ColorAnimation {
                properties: "color"
                duration: 250
            }
        }
    }
}

