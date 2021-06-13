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

FocusScope {
    id: headerBar

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
    property alias playerControl: playControlItem
    property alias isMaximized: playControlItem.isMaximized
    property int imageSourceSize: 512

    property bool portrait: (contentZone.height/contentZone.width) > 0.7

    property double smallerDimension: contentZone.height < contentZone.width?
                                        contentZone.height - 4 * Kirigami.Units.largeSpacing:
                                        contentZone.width - 4 * Kirigami.Units.largeSpacing

    signal openArtist()
    signal openAlbum()
    signal openNowPlaying()

    onImageChanged:
    {
        if (changeBackgroundTransition.running) {
            changeBackgroundTransition.complete()
        }

        newImage = image
        changeBackgroundTransition.start()
    }

    Item {
        id: background
        anchors.fill: parent

        ImageWithFallback {
            id: oldBackground

            source: oldImage
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

        ImageWithFallback {
            id: newBackground

            source: newImage
            fallback: Qt.resolvedUrl(elisaTheme.defaultBackgroundImage)

            asynchronous: true

            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop

            sourceSize.height: oldBackground.sourceSize.height

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
                    radius: 64
                    transparentBorder: false
                }
            }
        }
    }

    MediaPlayerControl {
        id: playControlItem

        focus: true

        anchors.left: background.left
        anchors.right: background.right
        anchors.bottom: background.bottom

        height: elisaTheme.mediaPlayerControlHeight
    }

    ColumnLayout {
        id: contentZone

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: playControlItem.top
        anchors.leftMargin: Kirigami.Units.smallSpacing

        spacing: 0

        // Hardcoded because the headerbar blur always makes a dark-ish
        // background, so we don't want to use a color scheme color that
        // might also be dark. This is the text color of Breeze
        Kirigami.Theme.textColor: "#eff0f1"


        GridLayout {
            id: gridLayoutContent

            columns: portrait? 1: 2

            columnSpacing: Kirigami.Units.largeSpacing
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
                NumberAnimation {
                    easing.type: Easing.InOutQuad
                    duration: Kirigami.Units.shortDuration
                }
            }

            Behavior on Layout.leftMargin {
                NumberAnimation {
                    easing.type: Easing.InOutQuad
                    duration: Kirigami.Units.shortDuration
                }
            }

            Item {
                id: images
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                property double imageSize: (smallerDimension * 0.9 < (portrait?
                                                                      gridLayoutContent.height/3:
                                                                      gridLayoutContent.width/2
                                        ))?
                                          smallerDimension * 0.9:
                                          portrait?
                                                   gridLayoutContent.height/3:
                                                   gridLayoutContent.width/2

                Layout.preferredHeight: imageSize
                Layout.preferredWidth: imageSize


                ImageWithFallback {
                    id: oldMainIcon
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                    anchors.fill: parent

                    asynchronous: true
                    mipmap: true

                    source: oldImage
                    fallback: Qt.resolvedUrl(elisaTheme.defaultAlbumImage)

                    sourceSize {
                        width: imageSourceSize
                        height: imageSourceSize
                    }

                    fillMode: Image.PreserveAspectFit
                }

                ImageWithFallback {
                    id: newMainIcon
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                    anchors.fill: parent

                    asynchronous: true
                    mipmap: true

                    source: newImage
                    fallback: Qt.resolvedUrl(elisaTheme.defaultAlbumImage)

                    visible: false
                    opacity: 0

                    sourceSize {
                        width: imageSourceSize
                        height: imageSourceSize
                    }

                    fillMode: Image.PreserveAspectFit
                }
            }

            ColumnLayout {
                spacing: 0
                Layout.alignment: (portrait? Qt.AlignHCenter: Qt.AlignLeft) | Qt.AlignTop

                Layout.fillWidth: true
                Layout.fillHeight: true

                Layout.maximumHeight: (headerBar.height - playControlItem.height - 8 * Kirigami.Units.largeSpacing) < gridLayoutContent.height ? (headerBar.height - playControlItem.height - 8 * Kirigami.Units.largeSpacing): gridLayoutContent.height

                LabelWithToolTip {
                    id: mainLabel
                    text: title
                    Layout.alignment: (portrait? Qt.AlignHCenter: Qt.AlignLeft) | Qt.AlignTop
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
                    Layout.alignment: (portrait? Qt.AlignHCenter: Qt.AlignLeft) | Qt.AlignTop
                    Layout.fillWidth: true
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
                    Layout.alignment: (portrait? Qt.AlignHCenter: Qt.AlignLeft) | Qt.AlignTop
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
                    Layout.alignment: (portrait? Qt.AlignHCenter: Qt.AlignLeft) | Qt.AlignTop
                }

                Loader {
                    id: playLoader
                    active: headerBar.isMaximized
                    visible: headerBar.isMaximized

                    Layout.fillWidth: true
                    Layout.fillHeight:  true
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    Layout.topMargin: Kirigami.Units.largeSpacing

                    sourceComponent: SimplePlayListView {
                        anchors.fill: parent
                        playListModel: ElisaApplication.mediaPlayListProxyModel
                    }

                }
            }
        }
    }

    SequentialAnimation {
        id: changeBackgroundTransition

        PropertyAction {
            targets: [newBackground, newMainIcon]
            property: 'opacity'
            value: 0
        }

        PropertyAction {
            targets: [newBackground, newMainIcon]
            property: 'visible'
            value: true
        }

        PropertyAction {
            target: newBackground
            property: "source"
            value: (newImage ? newImage : Qt.resolvedUrl(elisaTheme.defaultBackgroundImage))
        }

        PropertyAction {
            target: newMainIcon
            property: "source"
            value: (newImage ? newImage : Qt.resolvedUrl(elisaTheme.defaultAlbumImage))
        }

        ParallelAnimation {
            NumberAnimation {
                targets: [newBackground, newMainIcon, mainLabel, authorLabel, albumLabel]
                property: 'opacity'
                from: 0
                to: 1
                duration: Kirigami.Units.longDuration
                easing.type: Easing.Linear
            }

            NumberAnimation {
                targets: [oldBackground, oldMainIcon]
                property: 'opacity'
                from: 1
                to: 0
                duration: Kirigami.Units.longDuration
                easing.type: Easing.Linear
            }
        }

        PropertyAction {
            target: headerBar
            property: "oldImage"
            value: image
        }

        PropertyAction {
            target: oldBackground
            property: 'source'
            value: (headerBar.oldImage ? headerBar.oldImage : Qt.resolvedUrl(elisaTheme.defaultBackgroundImage))
        }

        PropertyAction {
            target: oldMainIcon
            property: 'source'
            value: (headerBar.oldImage ? headerBar.oldImage : Qt.resolvedUrl(elisaTheme.defaultAlbumImage))
        }

        PropertyAction {
            targets: [oldBackground, oldMainIcon]
            property: 'opacity'
            value: 1
        }

        PropertyAction {
            targets: [newBackground, newMainIcon]
            property: 'visible'
            value: false
        }

        onStopped:
        {
            oldImage = newImage
        }
    }
}
