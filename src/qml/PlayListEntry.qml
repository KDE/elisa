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

    property bool containsMouse
    property bool simpleMode

    Accessible.role: Accessible.ListItem
    Accessible.name: title + ' ' + album + ' ' + artist

    Keys.onReturnPressed: {
        playListEntry.switchToTrack(playListEntry.index)
        playListEntry.startPlayback()
    }

    height: Kirigami.Units.gridUnit + (Kirigami.Units.largeSpacing * 2)

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

    Rectangle {
        id: entryBackground

        anchors.fill: parent
        z: 1

        color: simpleMode ? "transparent" : myPalette.base

        height: playListEntry.height
    }

    RowLayout {
        id: trackRow
        z: 3

        anchors.fill: parent
        anchors.leftMargin: Kirigami.Units.largeSpacing

        spacing: Kirigami.Units.smallSpacing / 2

        // Container for the play/pause icon and the track/disc label
        Item {
            TextMetrics {
                id: fakeLabel
                text: '99/9'
            }

            Layout.preferredWidth: fakeLabel.width
            Layout.preferredHeight: playListEntry.height
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.leftMargin: !LayoutMirroring.enabled ? Kirigami.Units.smallSpacing : 0
            Layout.rightMargin: LayoutMirroring.enabled ? Kirigami.Units.smallSpacing : 0

            Image {
                id: playIcon

                anchors.centerIn: parent

                source: (isPlaying === MediaPlayList.IsPlaying ?
                             Qt.resolvedUrl(elisaTheme.playingIndicatorIcon) : Qt.resolvedUrl(elisaTheme.pausedIndicatorIcon))

                width: Kirigami.Units.iconSizes.smallMedium
                height: Kirigami.Units.iconSizes.smallMedium
                sourceSize.width: Kirigami.Units.iconSizes.smallMedium
                sourceSize.height: Kirigami.Units.iconSizes.smallMedium
                fillMode: Image.PreserveAspectFit
                mirror: LayoutMirroring.enabled

                layer.enabled: simpleMode
                layer.effect: ColorOverlay {
                    cached: true
                    color: Kirigami.Theme.textColor
                }

                visible: isPlaying === MediaPlayList.IsPlaying || isPlaying === MediaPlayList.IsPaused
            }

            Label {
                id: trackAndDiscNumberLabel

                anchors.fill: parent
                anchors.rightMargin: LayoutMirroring.enabled ? 0 : Kirigami.Units.largeSpacing
                anchors.leftMargin: !LayoutMirroring.enabled ? 0 : Kirigami.Units.largeSpacing

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

                font.weight: (isPlaying ? Font.Bold : Font.Normal)

                visible: isValid && !playIcon.visible
            }
        }

        LabelWithToolTip {
            id: mainCompactLabel

            text: title

            font.weight: (isPlaying ? Font.Bold : Font.Normal)

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

            visible: isValid

            elide: Text.ElideRight
            horizontalAlignment: Text.AlignLeft
        }

        LabelWithToolTip {
            id: mainInvalidCompactLabel

            text: title

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

            visible: !isValid

            elide: Text.ElideRight
        }

        Loader {
            id: hoverLoader
            active: false
            visible: active

            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

            sourceComponent: Row {
                anchors.centerIn: parent

                FlatButtonWithToolTip {
                    visible: playListEntry.fileName.toString().substring(0, 7) === 'file://'

                    enabled: isValid

                    text: i18nc("Show the file for this song in the file manager", "Show in folder")
                    icon.name: "document-open-folder"
                    onClicked: {
                        ElisaApplication.showInFolder(playListEntry.fileName)
                    }
                }

                FlatButtonWithToolTip {
                    id: infoButton
                    objectName: 'infoButton'

                    enabled: isValid

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

                    enabled: isValid

                    scale: LayoutMirroring.enabled ? -1 : 1 // We can mirror the symmetrical pause icon

                    text: (isPlaying === MediaPlayList.IsPlaying) ? i18nc("Pause current track from play list", "Pause") : i18nc("Play this track from play list", "Play")
                    icon.name: (isPlaying === MediaPlayList.IsPlaying) ? "media-playback-pause" : "media-playback-start"
                    onClicked: if (isPlaying === MediaPlayList.IsPlaying) {
                        playListEntry.pausePlayback()
                    } else if (isPlaying === MediaPlayList.IsPaused) {
                        playListEntry.startPlayback()
                    } else {
                        playListEntry.switchToTrack(playListEntry.index)
                        playListEntry.startPlayback()
                    }
                }

                FlatButtonWithToolTip {
                    id: removeButton
                    objectName: 'removeButton'

                    text: i18nc("Remove current track from play list", "Remove")
                    icon.name: "error"
                    onClicked: playListEntry.removeFromPlaylist(playListEntry.index)
                }

            }
        }

        RatingStar {
            id: ratingWidget

            starRating: rating

            visible: rating > 0 && !containsMouse && !isSelected && !playListEntry.activeFocus && !simpleMode
        }

        LabelWithToolTip {
            id: durationLabel

            text: duration

            font.weight: (isPlaying ? Font.Bold : Font.Normal)

            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            Layout.leftMargin: Kirigami.Units.largeSpacing
            Layout.rightMargin: Kirigami.Units.largeSpacing

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

