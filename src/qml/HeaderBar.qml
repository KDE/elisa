/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2023 (c) Fushan Wen <qydwhotmail@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Effects as FX
import org.kde.kirigami as Kirigami
import org.kde.elisa

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

    property bool lyricsVisible: false

    signal openArtist()
    signal openAlbum()
    signal openNowPlaying()

    readonly property int trackType: ElisaApplication.manageHeaderBar.trackType
    readonly property int databaseId: ElisaApplication.manageHeaderBar.databaseId
    readonly property url fileUrl: ElisaApplication.manageHeaderBar.fileUrl

    TrackContextMetaDataModel {
        id: metaDataModel
        onLyricsChanged: lyricsView.model.setLyric(lyrics)
        manager: ElisaApplication.musicManager
    }

    //Required for TrackContextMetaDataModel to work properly

    onFileUrlChanged: {
        if (ElisaApplication.musicManager && trackType !== undefined && fileUrl !== undefined && fileUrl.toString().length !== 0) {
            if (databaseId !== 0) {
                metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl);
            } else {
                metaDataModel.initializeByUrl(trackType, fileUrl);
            }
        }
    }

    onTrackTypeChanged: {
        if (ElisaApplication.musicManager && trackType !== undefined && fileUrl !== undefined && fileUrl.toString().length !== 0) {
            if (databaseId !== 0) {
                metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl);
            } else {
                metaDataModel.initializeByUrl(trackType, fileUrl);
            }
        }
    }

    Connections {
        target: ElisaApplication

        function onMusicManagerChanged() {
            if (ElisaApplication.musicManager && trackType !== undefined && databaseId !== 0) {
                metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl);
            }
        }
    }

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

        MouseArea {
            id: backgroundMouseArea
            anchors.fill: parent
            onDoubleClicked: {
                mediaPlayerControl.isMaximized = !mediaPlayerControl.isMaximized
            }
        }
    }

    Component {
        id: backgroundComponent

        ImageWithFallback {
            fallback: Qt.resolvedUrl(Theme.defaultBackgroundImage)
            asynchronous: true

            sourceSize.width: Screen.width
            fillMode: Image.PreserveAspectCrop

            StackView.onRemoved: {
                destroy();
            }

            Rectangle {
                anchors.fill: parent
                color: Kirigami.Theme.backgroundColor
                z: -1
            }

            Component.onCompleted: {
                if (status === Image.Loading) {
                    statusChanged.connect(() => { if (status == Image.Ready) { headerBar.replaceWhenLoaded() } });
                } else {
                    headerBar.replaceWhenLoaded();
                }
            }

            Rectangle {
                visible: GraphicsInfo.api === GraphicsInfo.Software
                anchors.fill: parent
                color: "black"
                opacity: 0.8
            }
        }
    }

    RowLayout {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: mainWindow.contextDrawer.handleVisible ? Kirigami.Units.smallSpacing : 0
        anchors.rightMargin: {
            let rightPadding = Kirigami.Units.mediumSpacing;
            if (mainWindow.contextDrawer.handleVisible) {
                const handleMargin = Kirigami.Units.smallSpacing;
                rightPadding += mainWindow.contextDrawer.handle.width + handleMargin;
            }
            return rightPadding;
        }

        spacing: Kirigami.Units.smallSpacing

        FlatButtonWithToolTip {
            id: playlistToggleButton
            autoExclusive: true

            visible: headerBar.isMaximized

            text: i18nc("@action:inmenu", "Playlist")
            icon.name: "view-media-playlist"
            display: AbstractButton.TextBesideIcon
            icon.color: Theme.headerForegroundColor
            Kirigami.Theme.textColor: Theme.headerForegroundColor

            checkable: true
            checked: !headerBar.lyricsVisible
            onCheckedChanged: if (checked) headerBar.lyricsVisible = false
        }

        FlatButtonWithToolTip {
            id: lyricsToggleButton
            autoExclusive: true

            visible: headerBar.isMaximized

            text: i18nc("@action:button", "Lyrics")
            icon.name: "view-media-lyrics"
            display: AbstractButton.TextBesideIcon
            icon.color: Theme.headerForegroundColor
            Kirigami.Theme.textColor: Theme.headerForegroundColor

            checkable: true
            checked: headerBar.lyricsVisible
            onCheckedChanged: if (checked) headerBar.lyricsVisible = true
        }

        FlatButtonWithToolTip {
            visible: mainWindow.visibility == Window.FullScreen

            text: i18nc("@action:button", "Exit Full Screen")
            icon.name: "view-restore"
            display: AbstractButton.TextBesideIcon
            icon.color: Theme.headerForegroundColor
            Kirigami.Theme.textColor: Theme.headerForegroundColor

            onClicked: mainWindow.restorePreviousStateBeforeFullScreen()
        }
    }

    MediaPlayerControl {
        id: playControlItem

        focus: true
        z: 1

        anchors.left: background.left
        anchors.right: background.right
        anchors.bottom: background.bottom

        isTranslucent: headerBar.height > height
        isNearCollapse: headerBar.height < height * 2

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
        anchors.topMargin: headerBar.isMaximized ? Kirigami.Units.largeSpacing : 0

        spacing: 0

        // Hardcoded because the headerbar blur always makes a dark-ish
        // background, so we don't want to use a color scheme color that
        // might also be dark.
        Kirigami.Theme.textColor: Theme.headerForegroundColor


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

                    fallback: Qt.resolvedUrl(Theme.defaultAlbumImage)

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
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignTop
                GridLayout {
                    // Part of HeaderBar that shows track information. Depending on it's size, that information is
                    // shown in a Column, Row or completely hidden (visibility handled in parent).
                    id: trackInfoGrid
                    flow: Grid.TopToBottom
                    rows: 4
                    columns: 1

                    rowSpacing: Kirigami.Units.largeSpacing
                    columnSpacing: Kirigami.Units.largeSpacing * 6
                    Layout.alignment:  (portrait && isMaximized ? Qt.AlignHCenter: Qt.AlignLeft) | Qt.AlignTop
                    Layout.fillWidth: true
                    Layout.fillHeight: false
                    Layout.maximumHeight: {
                        const h = headerBar.height - playControlItem.height - 8 * Kirigami.Units.largeSpacing
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
                            isLeftToRight: true
                        }
                    }

                    property bool isLeftToRight: false
                    property double colWidth: width * (1/visibleChildren.length) * 0.8

                    LabelWithToolTip {
                        id: mainLabel
                        text: title
                        Layout.fillWidth: true
                        horizontalAlignment: portrait? Text.AlignHCenter : Text.AlignLeft
                        level: 1
                        font.bold: true

                        states: State {
                            name: "mainLabelLeftToRight"
                            when: trackInfoGrid.isLeftToRight
                            PropertyChanges {
                                target: mainLabel
                                Layout.minimumWidth: Math.min(implicitWidth, trackInfoGrid.colWidth)
                                Layout.maximumWidth: implicitWidth * 1.2
                            }
                        }

                        MouseArea {
                            id: titleMouseArea
                            anchors.left: parent.left
                            width: Math.min(parent.implicitWidth, parent.width)
                            height: parent.height
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                openNowPlaying()
                                playControlItem.isMaximized = false
                            }

                            states: State {
                                name: "titleMouseAreaAnchor"
                                when: portrait && isMaximized
                                AnchorChanges {
                                    target: titleMouseArea
                                    anchors.left: undefined
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                            }
                        }
                    }

                    LabelWithToolTip {
                        id: authorLabel
                        text: artist
                        visible: artist
                        Layout.fillWidth: true
                        horizontalAlignment: portrait? Text.AlignHCenter : Text.AlignLeft
                        level: 3

                        states: State {
                            name: "authorLabelLeftToRight"
                            when: trackInfoGrid.isLeftToRight
                            PropertyChanges {
                                target: authorLabel
                                Layout.minimumWidth: Math.min(implicitWidth, trackInfoGrid.colWidth)
                                Layout.maximumWidth: implicitWidth * 1.2
                            }
                        }

                        MouseArea {
                            id: authorMouseArea
                            anchors.left: parent.left
                            width: Math.min(parent.implicitWidth, parent.width)
                            height: parent.height
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                openArtist()
                                playControlItem.isMaximized = false
                            }

                            states: State {
                                name: "authorMouseAreaAnchor"
                                when: portrait && isMaximized
                                AnchorChanges {
                                    target: authorMouseArea
                                    anchors.left: undefined
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                            }
                        }
                    }

                    LabelWithToolTip {
                        id: albumLabel
                        text: album
                        visible: album
                        Layout.fillWidth: true
                        horizontalAlignment: portrait? Text.AlignHCenter : Text.AlignLeft
                        level: 3

                        states: State {
                            name: "albumLabelLeftToRight"
                            when: trackInfoGrid.isLeftToRight
                            PropertyChanges {
                                target: albumLabel
                                Layout.minimumWidth: Math.min(implicitWidth, trackInfoGrid.colWidth)
                                Layout.maximumWidth: implicitWidth * 1.2
                            }
                        }

                        MouseArea {
                            id: albumMouseArea
                            anchors.left: parent.left
                            width: Math.min(parent.implicitWidth, parent.width)
                            height: parent.height
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                openAlbum()
                                playControlItem.isMaximized = false
                            }

                            states: State {
                                name: "albumMouseAreaAnchor"
                                when: portrait && isMaximized
                                AnchorChanges {
                                    target: albumMouseArea
                                    anchors.left: undefined
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                            }
                        }
                    }

                    RatingStar {
                        id: mainRating
                        visible: ratingVisible
                        starRating: trackRating
                    }
                }
                Loader {
                    id: playLoader
                    active: headerBar.isMaximized
                    asynchronous: true
                    visible: headerBar.isMaximized && !headerBar.lyricsVisible

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    Layout.topMargin: Kirigami.Units.largeSpacing

                    sourceComponent: SimplePlayListView {
                        model: ElisaApplication.mediaPlayListProxyModel
                    }
                }

                LyricsView {
                    id: lyricsView
                    visible: headerBar.isMaximized && headerBar.lyricsVisible

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    Layout.topMargin: Kirigami.Units.largeSpacing

                    fadeInOut: true

                    pointSize: {
                        if (lyricsView.width < 300 || lyricsView.height < 220) {
                            return Kirigami.Theme.defaultFont.pointSize;
                        } else {
                            return Kirigami.Theme.defaultFont.pointSize * 1.5;
                        }
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

        if (ElisaApplication.musicManager && trackType !== undefined) {
            if (ElisaApplication.manageHeaderBar.databaseId !== 0) {
                metaDataModel.initializeByIdAndUrl(trackType, databaseId, fileUrl);
            } else {
                metaDataModel.initializeByUrl(trackType, fileUrl);
            }
        }
    }
}
