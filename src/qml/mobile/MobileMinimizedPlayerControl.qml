/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

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

import ".."
import "../shared"

BasePlayerControl {
    id: musicWidget

    implicitHeight: Math.max(toolBarLayout.implicitHeight, Math.round(Kirigami.Units.gridUnit * 3.5))

    property alias volume: volumeButton.sliderValue
    property alias volumeSlider: volumeButton.slider

    property string image
    property string artist
    property string title

    property int imageSourceSize: 512

    property bool isWidescreen: mainWindow.width >= Theme.viewSelectorSmallSizeThreshold

    // background image
    ImageWithFallback {
        id: oldBackground

        source: musicWidget.image
        fallback: Theme.defaultBackgroundImage
        asynchronous: true

        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop

        opacity: 1

        layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
        layer.effect: FX.MultiEffect {
            autoPaddingEnabled: false
            blurEnabled: true
            blur: 1
            blurMax: 64
            blurMultiplier: 2

            brightness: -0.3
        }

        Rectangle {
            visible: GraphicsInfo.api === GraphicsInfo.Software
            anchors.fill: parent
            color: "black"
            opacity: 0.8
        }
    }

    // darken background
    Rectangle {
        anchors.fill: parent

        color: palette.dark
        opacity: Theme.mediaPlayerControlOpacity
    }

    // progress bar for limited width (phones)
    Rectangle {
        z: 1
        anchors.top: parent.top
        anchors.left: parent.left
        height: Kirigami.Units.gridUnit / 6
        color: Kirigami.Theme.highlightColor
        width: parent.width * musicWidget.position / musicWidget.duration
        visible: !musicWidget.isWidescreen
    }

    // actual player bar
    RowLayout {
        id: toolBarLayout

        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: Kirigami.Units.gridUnit * 16
            Layout.preferredWidth: Kirigami.Units.gridUnit * 16
            color: Qt.rgba(0, 0, 0, trackClick.containsMouse ? 0.1 : trackClick.pressed ? 0.3 : 0)

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: Kirigami.Units.largeSpacing
                spacing: Kirigami.Units.largeSpacing

                // track image
                Item {
                    Layout.alignment: Qt.AlignVCenter
                    property double imageSize: musicWidget.height - Kirigami.Units.largeSpacing * 2
                    Layout.maximumWidth: imageSize
                    Layout.preferredWidth: imageSize
                    Layout.maximumHeight: imageSize
                    Layout.minimumHeight: imageSize

                    ImageWithFallback {
                        id: mainIcon
                        anchors.fill: parent

                        asynchronous: true
                        mipmap: true

                        source: musicWidget.image
                        fallback: Qt.resolvedUrl(Theme.defaultAlbumImage)

                        sourceSize {
                            width: musicWidget.imageSourceSize * Screen.devicePixelRatio
                            height: musicWidget.imageSourceSize * Screen.devicePixelRatio
                        }

                        fillMode: Image.PreserveAspectFit
                    }
                }

                // track information
                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Label {
                        id: mainLabel
                        text: musicWidget.title
                        textFormat: Text.PlainText
                        wrapMode: Text.Wrap
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignLeft
                        elide: Text.ElideRight
                        maximumLineCount: 1
                        // Hardcoded because the footerbar blur always makes a dark-ish
                        // background, so we don't want to use a color scheme color that
                        // might also be dark
                        color: "white"
                        font.weight: Font.Bold
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1
                    }

                    Label {
                        id: authorLabel
                        text: musicWidget.artist
                        textFormat: Text.PlainText
                        wrapMode: Text.Wrap
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignLeft
                        elide: Text.ElideRight
                        maximumLineCount: 1
                        // Hardcoded because the footerbar blur always makes a dark-ish
                        // background, so we don't want to use a color scheme color that
                        // might also be dark
                        color: "white"
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1
                    }
                }
            }
            MouseArea {
                id: trackClick
                anchors.fill: parent
                hoverEnabled: true
                onClicked: toOpen.restart()
            }
        }

        Item {
            Layout.preferredWidth: 0
            Layout.fillHeight: true
            Layout.fillWidth: true

            // duration slider for widescreen
            DurationSlider {
                anchors.fill: parent
                Layout.fillWidth: true
                Layout.maximumHeight: Math.floor(Kirigami.Units.gridUnit * 3)
                visible: musicWidget.isWidescreen
                position: musicWidget.position
                duration: musicWidget.duration
                seekable: musicWidget.seekable
                playEnabled: musicWidget.playEnabled
                onSeek: position => musicWidget.seek(position)

                // this color works well over the blurred/darkened background
                labelColor: "white"
            }
        }

        // volume button
        MobileVolumeButton {
            id: volumeButton
            visible: musicWidget.isWidescreen
            muted: musicWidget.muted
            Layout.maximumHeight: parent.height
            Layout.preferredHeight: Math.floor(Kirigami.Units.gridUnit * 3)
            Layout.maximumWidth: height
            Layout.preferredWidth: height
        }

        FlatButtonWithToolTip {
            id: skipBackwardButton
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            Layout.maximumWidth: height
            Layout.preferredWidth: height
            enabled: musicWidget.skipBackwardEnabled
            text: i18nc("@action:button", "Skip Backward")
            onClicked: musicWidget.playPrevious()
            icon.name: musicWidget.LayoutMirroring.enabled ? "media-skip-forward" : "media-skip-backward"
            icon.color: "white"
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false
        }

        FlatButtonWithToolTip {
            id: playPauseButton
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            Layout.maximumWidth: height
            Layout.preferredWidth: height
            enabled: musicWidget.playEnabled
            text: musicWidget.isPlaying ? i18nc("@action:button Pause any media that is playing", "Pause") : i18nc("@action:button Start playing media", "Play")
            onClicked: musicWidget.isPlaying ? musicWidget.pause() : musicWidget.play()
            icon.name: musicWidget.isPlaying ? "media-playback-pause" : "media-playback-start"
            icon.color: "white"
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false
        }

        FlatButtonWithToolTip {
            id: skipForwardButton
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            Layout.maximumWidth: height
            Layout.preferredWidth: height
            enabled: musicWidget.skipForwardEnabled
            text: i18nc("@action:button", "Skip Forward")
            onClicked: musicWidget.playNext()
            icon.name: musicWidget.LayoutMirroring.enabled ? "media-skip-backward" : "media-skip-forward"
            icon.color: "white"
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false
        }

        Item { implicitWidth: Math.floor(Kirigami.Units.smallSpacing / 2) }
    }
}
