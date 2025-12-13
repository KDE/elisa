/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Effects as FX
import org.kde.kirigami as Kirigami
import org.kde.elisa
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
        from: footerBar.contentY
        to: footerBar.contentHeight / 2
        duration: Kirigami.Units.longDuration * 2
        easing.type: Easing.OutCubic
        running: false
    }
    NumberAnimation on contentY {
        id: toClose
        from: footerBar.contentY
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

        component BlurredImage : ImageWithFallback {
            fallback: Theme.defaultBackgroundImage
            asynchronous: true

            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop

            sourceSize.width: Screen.width

            opacity: 1

            layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
            layer.effect: FX.MultiEffect {
                autoPaddingEnabled: false
                blurEnabled: true
                blur: 1
                blurMax: 64
                blurMultiplier: 2

                brightness: -0.2
                saturation: -0.3
            }

            Rectangle {
                visible: GraphicsInfo.api === GraphicsInfo.Software
                anchors.fill: parent
                color: "black"
                opacity: 0.8
            }
        }

        BlurredImage {
            id: oldBackground
            source: footerBar.oldImage
        }

        BlurredImage {
            id: newBackground
            source: footerBar.newImage
        }
    }

    ColumnLayout {
        id: contentZone

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: mainWindow.height + playControlItem.height
        spacing: 0

        MobileMinimizedPlayerControl {
            id: playControlItem

            Layout.fillWidth: true
            Layout.minimumHeight: implicitHeight
            Layout.alignment: Qt.AlignTop
            focus: true

            image: footerBar.newImage
            title: footerBar.title
            artist: footerBar.artist
        }

        MobileTrackPlayer {
            id: mobileTrackPlayer
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: Kirigami.Units.largeSpacing * 2

            image: footerBar.newImage
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
            newBackground.source = (footerBar.newImage ? footerBar.newImage : Qt.resolvedUrl(Theme.defaultBackgroundImage));
        }

        onStopped: {
            footerBar.oldImage = footerBar.image;
            oldBackground.source = (footerBar.oldImage ? footerBar.oldImage : Qt.resolvedUrl(Theme.defaultBackgroundImage));
            oldBackground.opacity = 1;
            newBackground.visible = false;
            footerBar.oldImage = footerBar.newImage;
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
