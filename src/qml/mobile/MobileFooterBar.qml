/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0
import ".."

Flickable {
    id: footerBar

    property string title
    property string artist
    property string albumArtist
    property string album
    property string image
    property string newImage
    property string oldImage
    property int trackRating
    property int albumID
    property bool ratingVisible
    property alias trackControl: playControlItem
    property alias playerControl: mobileTrackPlayer
    property int imageSourceSize: 512

    property bool portrait: (contentZone.height/contentZone.width) > 0.7

    signal openArtist()
    signal openAlbum()
    signal openNowPlaying()

    property bool isMaximized: contentY == contentHeight / 2

    boundsBehavior: Flickable.StopAtBounds

    onOpenArtist: toClose.restart()
    onOpenAlbum: toClose.restart()
    onOpenNowPlaying: toClose.restart()

    NumberAnimation on contentY {
        id: toOpen
        from: contentY
        to: contentHeight / 2
        duration: Kirigami.Units.longDuration * 2
        easing.type: Easing.OutCubic
        running: false
    }
    NumberAnimation on contentY {
        id: toClose
        from: contentY
        to: 0
        duration: Kirigami.Units.longDuration * 2
        easing.type: Easing.OutCubic
        running: false
    }

    // snap to end
    MouseArea {
        anchors.fill: contentZone
        propagateComposedEvents: true
        onPressed: {
            toOpen.stop();
            toClose.stop();
            propagateComposedEvents = true;
        }
        onReleased: footerBar.resetToBoundsOnFlick()
    }

    function resetToBoundsOnFlick() {
        if (!atYBeginning || !atYEnd) {
            if (footerBar.verticalVelocity > 0) {
                toOpen.restart();
            } else if (footerBar.verticalVelocity < 0) {
                toClose.restart();
            } else { // i.e. when verticalVelocity === 0
                if (contentY > contentHeight / 4) {
                    toOpen.restart();
                } else  {
                    toClose.restart();
                }
            }
        }
    }

    function resetToBoundsOnResize() {
        if (contentY > contentHeight / 4) {
            contentY = contentHeight / 2;
        } else {
            contentY = 0;
        }
    }

    onMovementStarted: {
        toOpen.stop();
        toClose.stop();
    }
    onFlickStarted: resetToBoundsOnFlick()
    onMovementEnded: resetToBoundsOnFlick()
    onHeightChanged: resetToBoundsOnResize()

    onImageChanged: {
        if (changeBackgroundTransition.running) {
            changeBackgroundTransition.complete()
        }

        newImage = image
        changeBackgroundTransition.start()
    }

    Item {
        id: background
        anchors.fill: contentZone

        // a cover for content underneath the panel
        Rectangle {
            id: coverUnderneath
            color: "#424242" // since background blurs have a dark hue, it doesn't make sense to theme
            anchors.fill: parent
        }

        ImageWithFallback {
            id: oldBackground

            source: oldImage
            fallback: elisaTheme.defaultBackgroundImage
            asynchronous: true

            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop

            sourceSize.width: imageSourceSize
            sourceSize.height: imageSourceSize

            opacity: 1

            layer.enabled: true
            layer.effect: HueSaturation {
                cached: true

                lightness: -0.4
                saturation: 0.9

                layer.enabled: true
                layer.effect: FastBlur {
                    cached: true
                    radius: 100
                }
            }
        }

        ImageWithFallback {
            id: newBackground

            source: newImage
            fallback: Qt.resolvedUrl(elisaTheme.defaultBackgroundImage)

            asynchronous: true

            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop

            sourceSize.width: imageSourceSize
            sourceSize.height: imageSourceSize

            visible: false
            opacity: 0

            layer.enabled: true
            layer.effect: HueSaturation {
                cached: true

                lightness: -0.5
                saturation: 0.9

                layer.enabled: true
                layer.effect: FastBlur {
                    cached: true
                    radius: 100
                }
            }
        }
    }

    ColumnLayout {
        id: contentZone

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: mainWindow.height + elisaTheme.mediaPlayerControlHeight
        spacing: 0

        MobileMinimizedPlayerControl {
            id: playControlItem

            Layout.fillWidth: true
            Layout.minimumHeight: elisaTheme.mediaPlayerControlHeight
            Layout.alignment: Qt.AlignTop
            focus: true

            image: newImage
            title: footerBar.title
            artist: footerBar.artist
        }

        MobileTrackPlayer {
            id: mobileTrackPlayer
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: Kirigami.Units.largeSpacing * 2

            image: newImage
            title: footerBar.title
            artist: footerBar.artist
            album: footerBar.album
            albumArtist: footerBar.albumArtist

            portrait: footerBar.portrait
        }
    }

    ParallelAnimation {
        id: changeBackgroundTransition

        onStarted: {
            newBackground.opacity = 0;
            newBackground.visible = true;
            newBackground.source = (newImage ? newImage : Qt.resolvedUrl(elisaTheme.defaultBackgroundImage));
        }

        onStopped: {
            footerBar.oldImage = image;
            oldBackground.source = (footerBar.oldImage ? footerBar.oldImage : Qt.resolvedUrl(elisaTheme.defaultBackgroundImage));
            oldBackground.opacity = 1;
            newBackground.visible = false;
            oldImage = newImage;
        }

        NumberAnimation {
            targets: [newBackground]
            property: 'opacity'
            from: 0
            to: 1
            duration: Kirigami.Units.longDuration
            easing.type: Easing.Linear
        }

        NumberAnimation {
            targets: [oldBackground]
            property: 'opacity'
            from: 1
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.Linear
        }
    }
}

