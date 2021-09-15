/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.7
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

import ".."
import "../shared"

BasePlayerControl {
    id: musicWidget

    property alias volume: volumeButton.sliderValue
    property alias volumeSlider: volumeButton.slider

    property string image
    property string artist
    property string title

    property int imageSourceSize: 512

    property bool isWidescreen: mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    // background image
    ImageWithFallback {
        id: oldBackground

        source: image
        fallback: elisaTheme.defaultBackgroundImage
        asynchronous: true

        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop

        // make the FastBlur effect more strong
        sourceSize.height: 10

        opacity: 1

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

    // darken background
    Rectangle {
        anchors.fill: parent

        color: myPalette.dark
        opacity: elisaTheme.mediaPlayerControlOpacity
    }

    // progress bar for limited width (phones)
    Rectangle {
        z: 1
        anchors.top: parent.top
        anchors.left: parent.left
        height: Kirigami.Units.gridUnit / 6
        color: Kirigami.Theme.highlightColor
        width: parent.width * position / duration
        visible: !musicWidget.isWidescreen
    }

    // actual player bar
    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: Kirigami.Units.gridUnit * 16
            Layout.preferredWidth: Kirigami.Units.gridUnit * 16
            color: Qt.rgba(0, 0, 0, trackClick.containsMouse? 0.1 : trackClick.pressed? 0.3 : 0)

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

                        source: image
                        fallback: Qt.resolvedUrl(elisaTheme.defaultAlbumImage)

                        sourceSize {
                            width: imageSourceSize
                            height: imageSourceSize
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
                        text: title
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
                        text: artist
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
                Layout.maximumHeight: Math.floor(Kirigami.Units.gridUnit * 2.5)
                visible: musicWidget.isWidescreen
                position: musicWidget.position
                duration: musicWidget.duration
                seekable: musicWidget.seekable
                playEnabled: musicWidget.playEnabled
                onSeek: musicWidget.seek(position)

                // these colours work over the blurred/darkened background
                labelColor: "white"
                sliderElapsedColor: "white"
                sliderRemainingColor: "grey"
                sliderHandleColor: "white"
                sliderBorderInactiveColor: "white"
                sliderBorderActiveColor: "grey"
            }
        }

        // volume button
        MobileVolumeButton {
            id: volumeButton
            visible: musicWidget.isWidescreen
            muted: musicWidget.muted
            Layout.maximumHeight: parent.height
            Layout.preferredHeight: Math.floor(Kirigami.Units.gridUnit * 2.5)
            Layout.maximumWidth: height
            Layout.preferredWidth: height
        }

        FlatButtonWithToolTip {
            id: skipBackwardButton
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            Layout.maximumWidth: height
            Layout.preferredWidth: height
            enabled: skipBackwardEnabled
            text: i18nc("skip backward in playlists", "Skip Backward")
            onClicked: musicWidget.playPrevious()
            icon.name: musicWidget.LayoutMirroring.enabled ? "media-skip-forward" : "media-skip-backward"
            icon.width: Kirigami.Units.gridUnit
            icon.height: Kirigami.Units.gridUnit
            icon.color: "white"
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false
        }

        FlatButtonWithToolTip {
            id: playPauseButton
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            Layout.maximumWidth: height
            Layout.preferredWidth: height
            enabled: playEnabled
            text: i18nc("toggle play and pause for the audio player", "Toggle Play and Pause")
            onClicked: musicWidget.isPlaying ? musicWidget.pause() : musicWidget.play()
            icon.name: musicWidget.isPlaying? "media-playback-pause" : "media-playback-start"
            icon.width: Kirigami.Units.gridUnit
            icon.height: Kirigami.Units.gridUnit
            icon.color: "white"
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false
        }

        FlatButtonWithToolTip {
            id: skipForwardButton
            Layout.preferredHeight: Kirigami.Units.gridUnit * 3
            Layout.maximumWidth: height
            Layout.preferredWidth: height
            enabled: skipForwardEnabled
            text: i18nc("skip forward in playlists", "Skip Forward")
            onClicked: musicWidget.playNext()
            icon.name: musicWidget.LayoutMirroring.enabled ? "media-skip-backward" : "media-skip-forward"
            icon.width: Kirigami.Units.gridUnit
            icon.height: Kirigami.Units.gridUnit
            icon.color: "white"
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false
        }

        Item { implicitWidth: Math.floor(Kirigami.Units.smallSpacing / 2) }
    }

    Component.onCompleted: {

        var elementList = [skipForwardButton, skipBackwardButton, playPauseButton]

        for (var i=0; i<elementList.length; i++)
            installFilter(elementList[i])

        function installFilter(element) {
            ElisaApplication.installKeyEventFilter(element)
        }
    }
}


