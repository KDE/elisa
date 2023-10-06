/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2023 (c) Fushan Wen <qydwhotmail@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.15
import QtQuick.Window 2.2
import ElisaGraphicalEffects 1.15
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

FocusScope {
    id: headerBar

    property string title
    property string artist
    property string albumArtist
    property string album
    property string image
    property int trackRating
    property int albumID
    property bool ratingVisible
    property alias playerControl: playControlItem
    property alias isMaximized: playControlItem.isMaximized
    property int imageSourceSize: 512

    property bool portrait: (contentZone.height/contentZone.width) > 0.7
    property bool transitionsEnabled: true

    property double smallerDimension: Math.min(contentZone.height, contentZone.width) - 4 * Kirigami.Units.largeSpacing

    property int handlePosition: implicitHeight

    signal openArtist()
    signal openAlbum()
    signal openNowPlaying()

    onImageChanged: {
        if (changeBackgroundTransition.running) {
            changeBackgroundTransition.complete()
        }

        loadImage();

        if (transitionsEnabled) {
            changeBackgroundTransition.start()
        }
    }

    function loadImage() {
        if (background.pendingImageIncubator) {
            background.pendingImageIncubator.forceCompletion();
            background.pendingImageIncubator.object.statusChanged.disconnect(replaceWhenLoaded);
            background.pendingImageIncubator.object.destroy();
            background.pendingImageIncubator = undefined;
        }

        if (images.pendingImageIncubator) {
            images.pendingImageIncubator.forceCompletion();
            images.pendingImageIncubator.object.statusChanged.disconnect(replaceIconWhenLoaded);
            images.pendingImageIncubator.object.destroy();
            images.pendingImageIncubator = undefined;
        }

        background.doesSkipAnimation = background.currentItem == undefined || !headerBar.transitionsEnabled;
        background.pendingImageIncubator = backgroundComponent.incubateObject(background, {
            "source": image,
            "opacity": 0,
        });
        images.pendingImageIncubator = mainIconComponent.incubateObject(images, {
            "source": image,
            "opacity": 0,
        });
    }

    function replaceWhenLoaded() {
        background.pendingImageIncubator.object.statusChanged.disconnect(replaceWhenLoaded);
        background.replace(background.pendingImageIncubator.object, {}, StackView.Transition);
        background.pendingImageIncubator = undefined;
    }

    function replaceIconWhenLoaded() {
        images.pendingImageIncubator.object.statusChanged.disconnect(replaceIconWhenLoaded);
        images.replace(images.pendingImageIncubator.object, {}, StackView.Transition);
        images.pendingImageIncubator = undefined;
    }

    StackView {
        id: background

        anchors.fill: parent
        visible: headerBar.height > playControlItem.height

        property var pendingImageIncubator
        property bool doesSkipAnimation: true

        replaceEnter: Transition {
            OpacityAnimator {
                id: replaceEnterOpacityAnimator
                from: 0
                to: 1
                // 1 is HACK for https://bugreports.qt.io/browse/QTBUG-106797 to avoid flickering
                duration: background.doesSkipAnimation ? 1 : Kirigami.Units.longDuration
            }
        }
        // Keep the old image around till the new one is fully faded in
        // If we fade both at the same time you can see the background behind glimpse through
        replaceExit: Transition {
            PauseAnimation {
                duration: replaceEnterOpacityAnimator.duration
            }
        }

        layer.enabled: true
        layer.effect: HueSaturation {
            cached: true

            lightness: -0.5
            saturation: 0.9

            layer.enabled: true
            layer.effect: FastBlur {
                cached: true
                radius: 64
                transparentBorder: false
            }
        }
    }

    Component {
        id: backgroundComponent

        ImageWithFallback {
            fallback: Qt.resolvedUrl(elisaTheme.defaultBackgroundImage)
            asynchronous: true

            // make the FastBlur effect more strong
            sourceSize.height: 10
            // Switch to Stretch if the effective height of the image to blur would be 0 with PreserveAspectCrop
            // We need to know the aspect ratio, which we compute from oldMainIcon.painted*, because:
            // - QML does not currently provide a direct way to get original source dimensions
            // - painted* of this image won't get us the right value when fillMode is set to Stretch
            // - oldMainIcon uses the same source and is set to preserve aspect ratio
            fillMode: width / height < (images.currentItem ? images.currentItem.paintedWidth / images.currentItem.paintedHeight * sourceSize.height : 1)
                      ? Image.PreserveAspectCrop : Image.Stretch

            StackView.onRemoved: {
                destroy();
            }

            Component.onCompleted: {
                if (status === Image.Loading) {
                    statusChanged.connect(headerBar.replaceWhenLoaded);
                } else {
                    headerBar.replaceWhenLoaded();
                }
            }
        }
    }
    // Not a flat button because we need a background to ensure adequate contrast
    // against the HeaderBar's album art background
    Button {
        anchors.top: parent.top
        anchors.right: parent.right

        visible: mainWindow.visibility == Window.FullScreen

        text: i18nc("@action:button", "Exit Full Screen")
        icon.name: "view-restore"

        onClicked: mainWindow.restorePreviousStateBeforeFullScreen();
    }

    MediaPlayerControl {
        id: playControlItem

        focus: true
        z: 1

        anchors.left: background.left
        anchors.right: background.right
        anchors.bottom: background.bottom

        height: elisaTheme.mediaPlayerControlHeight
        isTranslucent: headerBar.height > elisaTheme.mediaPlayerControlHeight
        isNearCollapse: headerBar.height < elisaTheme.mediaPlayerControlHeight * 2

        onHandlePositionChanged: (y, offset) => {
            const newHeight = headerBar.height - offset + y
            handlePosition = Math.max(newHeight, 0)
        }
    }

    ColumnLayout {
        id: contentZone

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: playControlItem.top

        spacing: 0

        // Hardcoded because the headerbar blur always makes a dark-ish
        // background, so we don't want to use a color scheme color that
        // might also be dark. This is the text color of Breeze
        Kirigami.Theme.textColor: "#eff0f1"


        GridLayout {
            id: gridLayoutContent
            visible: contentZone.height > mainLabel.height

            columns: portrait? 1: 2

            columnSpacing: Kirigami.Units.largeSpacing * (isMaximized ? 4 : 1)
            rowSpacing: Kirigami.Units.largeSpacing

            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.rightMargin: (LayoutMirroring.enabled && !portrait && !isMaximized)? contentZone.width * 0.15: 4 * Kirigami.Units.largeSpacing
            Layout.leftMargin: (!LayoutMirroring.enabled && !portrait && !isMaximized)? contentZone.width * 0.15: 4 * Kirigami.Units.largeSpacing
            Layout.topMargin: isMaximized? 4 * Kirigami.Units.largeSpacing : 0
            Layout.bottomMargin: isMaximized? 4 * Kirigami.Units.largeSpacing : 0
            Layout.maximumWidth: contentZone.width - 2 * ((!portrait && !isMaximized) ? contentZone.width * 0.15 : 4 * Kirigami.Units.largeSpacing)

            Behavior on Layout.topMargin {
                enabled: transitionsEnabled
                NumberAnimation {
                    easing.type: Easing.InOutQuad
                    duration: Kirigami.Units.shortDuration
                }
            }

            Behavior on Layout.leftMargin {
                enabled: transitionsEnabled
                NumberAnimation {
                    easing.type: Easing.InOutQuad
                    duration: Kirigami.Units.shortDuration
                }
            }

            StackView {
                id: images
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                property double imageSize: Math.min(smallerDimension * 0.9, portrait ? gridLayoutContent.height/3 : gridLayoutContent.width/2)
                property var pendingImageIncubator

                Layout.preferredHeight: imageSize
                Layout.preferredWidth: imageSize
                Layout.minimumHeight: mainLabel.height - Kirigami.Units.smallSpacing
                Layout.minimumWidth: mainLabel.height - Kirigami.Units.smallSpacing

                replaceEnter: Transition {
                    OpacityAnimator {
                        from: 0
                        to: 1
                        duration: replaceEnterOpacityAnimator.duration
                    }
                }
                // Keep the old image around till the new one is fully faded in
                // If we fade both at the same time you can see the background behind glimpse through
                replaceExit: Transition {
                    PauseAnimation {
                        duration: replaceEnterOpacityAnimator.duration
                    }
                }
            }

            Component {
                id: mainIconComponent

                ImageWithFallback {
                    asynchronous: true
                    mipmap: true

                    fallback: Qt.resolvedUrl(elisaTheme.defaultAlbumImage)

                    sourceSize {
                        width: imageSourceSize * Screen.devicePixelRatio
                        height: imageSourceSize * Screen.devicePixelRatio
                    }

                    fillMode: Image.PreserveAspectFit

                    StackView.onRemoved: {
                        destroy();
                    }

                    Component.onCompleted: {
                        if (status === Image.Loading) {
                            statusChanged.connect(headerBar.replaceIconWhenLoaded);
                        } else {
                            headerBar.replaceIconWhenLoaded();
                        }
                    }
                }
            }

            ColumnLayout {
                // fillHeight needs to adapt to playlist height when isMaximized && !portrait
                Layout.fillHeight: trackInfoGrid.height + playLoader.implicitHeight > images.height
                Layout.alignment: Qt.AlignTop
                Grid {
                    // Part of HeaderBar that shows track information. Depending on it's size, that information is
                    // shown in a Column, Row or completely hidden (visibility handled in parent).
                    id: trackInfoGrid
                    flow: Grid.TopToBottom
                    rows: 4
                    columns: 1
                    verticalItemAlignment: Grid.AlignVCenter
                    horizontalItemAlignment: portrait && isMaximized ? Grid.AlignHCenter : Grid.AlignLeft

                    rowSpacing: Kirigami.Units.largeSpacing
                    columnSpacing: Kirigami.Units.largeSpacing * 6
                    Layout.alignment:  (portrait && isMaximized ? Qt.AlignHCenter: Qt.AlignLeft) | Qt.AlignTop

                    Layout.fillWidth: !(portrait && isMaximized)
                    Layout.fillHeight: isMaximized
                    Layout.maximumHeight: {
                        var h = headerBar.height - playControlItem.height - 8 * Kirigami.Units.largeSpacing
                        if (h < gridLayoutContent.height)
                            return h
                        return gridLayoutContent.height + 8
                    }

                    states: State {
                        name: "leftToRight"
                        when: contentZone.height < (mainLabel.height * 3 + Kirigami.Units.largeSpacing * 4
                                                + (ratingVisible ? mainRating.height + Kirigami.Units.largeSpacing : 0) )
                        PropertyChanges {
                            target: trackInfoGrid
                            flow: Grid.LeftToRight
                            Layout.alignment: (portrait? Qt.AlignHCenter: Qt.AlignLeft) | Qt.AlignVCenter
                            Layout.maximumHeight: gridLayoutContent.height
                            rows: 1
                            columns: 4
                        }
                    }

                    move: Transition {
                        NumberAnimation {
                            // if Maximized, this would happen after change from and to portrait-layout, which we don't want
                            properties: isMaximized ? "" : "x,y"
                            easing.type: Easing.InOutCubic
                            duration: Kirigami.Units.longDuration
                        }
                    }

                    LabelWithToolTip {
                        id: mainLabel
                        text: title
                        Layout.alignment: (portrait? Qt.AlignHCenter: Qt.AlignLeft) | Qt.AlignVCenter
                        Layout.fillWidth: true
                        horizontalAlignment: portrait? Text.AlignHCenter : Text.AlignLeft
                        level: 1
                        font.bold: true

                        MouseArea {
                            id: titleMouseArea
                            width: Math.min(parent.implicitWidth, parent.width)
                            height: parent.height
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                openNowPlaying()
                            }
                        }
                    }

                    LabelWithToolTip {
                        id: authorLabel
                        text: artist
                        Layout.alignment: portrait? Qt.AlignHCenter: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.fillWidth: false
                        horizontalAlignment: portrait? Text.AlignHCenter : Text.AlignLeft

                        level: 3

                        MouseArea {
                            id: authorMouseArea
                            width: Math.min(parent.implicitWidth, parent.width)
                            height: parent.height
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                openArtist()
                            }
                        }
                    }

                    LabelWithToolTip {
                        id: albumLabel
                        text: album
                        Layout.alignment: (portrait? Qt.AlignHCenter: Qt.AlignLeft) | Qt.AlignVCenter
                        Layout.fillWidth: true
                        horizontalAlignment: portrait? Text.AlignHCenter : Text.AlignLeft

                        level: 3

                        MouseArea {
                            id: albumMouseArea
                            width: Math.min(parent.implicitWidth, parent.width)
                            height: parent.height
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                openAlbum()
                            }
                        }
                    }

                    RatingStar {
                        id: mainRating
                        visible: ratingVisible
                        starRating: trackRating
                        Layout.fillWidth: true
                        Layout.alignment: (portrait? Qt.AlignHCenter: Qt.AlignLeft) | Qt.AlignVCenter
                    }
                }
                Loader {
                    id: playLoader
                    active: headerBar.isMaximized
                    asynchronous: true
                    visible: headerBar.isMaximized

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    Layout.topMargin: Kirigami.Units.largeSpacing

                    sourceComponent: SimplePlayListView {
                        model: ElisaApplication.mediaPlayListProxyModel
                    }
                }
            }
        }
    }

    SequentialAnimation {
        id: changeBackgroundTransition

        ParallelAnimation {
            NumberAnimation {
                targets: [mainLabel, authorLabel, albumLabel]
                property: 'opacity'
                from: 0
                to: 1
                duration: Kirigami.Units.longDuration
                easing.type: Easing.Linear
            }
        }
    }

    Component.onCompleted: {
        loadImage();
    }
}
