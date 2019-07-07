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
    property bool isValid
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

    signal switchToTrack(var trackIndex)

    Accessible.role: Accessible.ListItem
    Accessible.name: title + ' ' + album + ' ' + artist

    height: elisaTheme.playListDelegateHeight

    Action {
        id: playNow
        text: i18nc("Play now current track from play list", "Play Now")
        icon.name: "media-playback-start"
        enabled: !(isPlaying === MediaPlayList.IsPlaying) && isValid
        onTriggered: {
            playListEntry.switchToTrack(playListEntry.index)
        }
    }

    MouseArea {
            anchors.fill: parent

            onClicked: playNow.trigger(this)
    }

    RowLayout {
        id: trackRow

        anchors.fill: parent

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

                opacity: ((isPlaying === MediaPlayList.IsPlaying || isPlaying === MediaPlayList.IsPaused) ? 1.0 : 0.0)

                source: (isPlaying === MediaPlayList.IsPlaying ?
                             Qt.resolvedUrl(elisaTheme.playingIndicatorIcon) : Qt.resolvedUrl(elisaTheme.pausedIndicatorIcon))

                width: elisaTheme.smallControlButtonSize
                height: elisaTheme.smallControlButtonSize

                sourceSize.width: elisaTheme.smallControlButtonSize
                sourceSize.height: elisaTheme.smallControlButtonSize
                fillMode: Image.PreserveAspectFit
                mirror: LayoutMirroring.enabled
                visible: opacity > 0.0

                layer.enabled: true
                layer.effect: ColorOverlay {
                    cached: true

                    color: myPalette.highlightedText
                }

                Behavior on opacity {
                    NumberAnimation {
                        easing.type: Easing.InOutQuad
                        duration: 250
                    }
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
            color: myPalette.highlightedText

            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

            visible: isValid

            Layout.preferredWidth: ((trackNumberSize.boundingRect.width - trackNumberSize.boundingRect.x) > (realTrackNumberSize.boundingRect.width - realTrackNumberSize.boundingRect.x) ?
                                        (trackNumberSize.boundingRect.width - trackNumberSize.boundingRect.x) :
                                        (realTrackNumberSize.boundingRect.width - realTrackNumberSize.boundingRect.x))
            Layout.minimumWidth: ((trackNumberSize.boundingRect.width - trackNumberSize.boundingRect.x) > (realTrackNumberSize.boundingRect.width - realTrackNumberSize.boundingRect.x) ?
                                      (trackNumberSize.boundingRect.width - trackNumberSize.boundingRect.x) :
                                      (realTrackNumberSize.boundingRect.width - realTrackNumberSize.boundingRect.x))
            Layout.maximumWidth: ((trackNumberSize.boundingRect.width - trackNumberSize.boundingRect.x) > (realTrackNumberSize.boundingRect.width - realTrackNumberSize.boundingRect.x) ?
                                      (trackNumberSize.boundingRect.width - trackNumberSize.boundingRect.x) :
                                      (realTrackNumberSize.boundingRect.width - realTrackNumberSize.boundingRect.x))

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
            color: myPalette.highlightedText

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
            color: myPalette.highlightedText

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
            color: myPalette.highlightedText

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
            color: myPalette.highlightedText

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

            visible: !isValid

            elide: Text.ElideRight
        }

        Item {
            Layout.fillWidth: true
            Layout.preferredWidth: 0
        }

        RatingStar {
            id: ratingWidget

            starRating: rating

            starSize: elisaTheme.ratingStarSize
        }

        LabelWithToolTip {
            id: durationLabel

            text: duration

            font.weight: (isPlaying ? Font.Bold : Font.Normal)
            color: myPalette.highlightedText

            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            Layout.leftMargin: elisaTheme.layoutHorizontalMargin / 2
            Layout.rightMargin: elisaTheme.layoutHorizontalMargin / 2

            horizontalAlignment: Text.AlignRight
        }
    }


}

