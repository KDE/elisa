/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import org.kde.kirigami as Kirigami
import org.kde.elisa

Item {
    id: lyricItem
    clip: true

    property bool alignLeft: false
    property bool fadeInOut: false
    property real pointSize: Kirigami.Theme.defaultFont.pointSize
    readonly property LyricsModel model: lyricsModel

    Item {
        id: maskContainer
        visible: false
        Rectangle {
            id: mask
            height: lyricsView.height
            width: lyricsView.width
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: "transparent"
                }
                GradientStop {
                    position: 0.2
                    color: "white"
                }
                GradientStop {
                    position: 0.8
                    color: "white"
                }
                GradientStop {
                    position: 1.0
                    color: "transparent"
                }
            }
            layer.enabled: true
        }
    }

    ScrollView {
        id: lyricScroll
        anchors.centerIn: parent
        height: Math.min(lyricItem.height, implicitHeight)
        width: lyricItem.width

        // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        PropertyAnimation {
            id: lyricScrollAnimation

            // the target is a flickable
            target: lyricScroll.contentItem
            property: "contentY"
            onToChanged: restart()
            easing.type: Easing.OutQuad
            duration: Kirigami.Units.shortDuration
        }

        contentItem: ListView {
            id: lyricsView

            layer.enabled: lyricItem.fadeInOut
            layer.effect: MultiEffect {
                visible: lyricItem.fadeInOut
                maskEnabled: lyricItem.fadeInOut
                maskSource: mask
                maskSpreadAtMin: 1
                maskThresholdMin: 0.5
            }

            model: lyricsModel
            delegate: Label {
                id: lyricsDelegate
                required property string lyric
                required property int timestamp
                required property bool isHighlighted

                text: lyric
                width: lyricsView.width
                wrapMode: Text.WordWrap
                font.bold: isHighlighted
                font.pointSize: lyricItem.pointSize

                horizontalAlignment: lyricItem.alignLeft ? Text.AlignLeft : Text.AlignHCenter
                MouseArea {
                    height: parent.height
                    width: Math.min(parent.width, parent.contentWidth)
                    x: lyricItem.alignLeft ? 0 : (parent.width - width) / 2
                    enabled: lyricsModel.isLRC
                    cursorShape: enabled ? Qt.PointingHandCursor : undefined
                    onClicked: {
                        ElisaApplication.audioPlayer.position = timestamp;
                    }
                }
            }

            function scrollToCurrentLyric() {
                if (!lyricsModel.isLRC) {
                    return;
                }

                if (!currentItem || !lyricScroll || currentIndex === -1) {
                    lyricScrollAnimation.to = -headerItem.height;
                    return;
                }

                const toPos = Math.round(currentItem.y + currentItem.height * 0.5 - lyricScroll.height * 0.5);
                lyricScrollAnimation.to = toPos;
            }

            header: Item {
                id: headerItem
                height: lyricItem.height * 0.5
            }

            footer: Item {
                height: lyricItem.height * 0.5
            }

            currentIndex: lyricsModel.highlightedIndex

            onHeightChanged: scrollToCurrentLyric()
            onCurrentIndexChanged: scrollToCurrentLyric()
            onCountChanged: scrollToCurrentLyric()
        }
    }

    LyricsModel {
        id: lyricsModel
    }
    Connections {
        target: ElisaApplication.audioPlayer
        function onPositionChanged(position) {
            lyricsModel.setPosition(position);
        }
    }

    Loader {
        id: lyricPlaceholder
        anchors.centerIn: parent
        width: parent.width

        active: lyricsView.count === 0
        visible: active && status === Loader.Ready

        sourceComponent: Kirigami.PlaceholderMessage {
            text: i18nc("@info:placeholder", "No lyrics found")
            icon.name: "view-media-lyrics"
        }
    }
}
