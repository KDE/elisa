/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound
pragma Singleton

import QtQuick 2.7
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami 2.12 as Kirigami

Item {
    readonly property string defaultAlbumImage: 'image://icon/media-default-album'
    readonly property string defaultBackgroundImage: 'qrc:/qt/qml/org/kde/elisa/background.png'
    readonly property string pausedIndicatorIcon: 'image://icon/media-playback-paused'
    readonly property string playingIndicatorIcon: 'image://icon/media-playback-playing'
    readonly property string ratingIcon: 'image://icon/rating'
    readonly property string ratingUnratedIcon: 'image://icon/rating-unrated'
    readonly property string folderIcon: 'image://icon/document-open-folder'
    readonly property string trackIcon: 'image://icon/view-media-track'

    readonly property int coverImageSize: Kirigami.Units.gridUnit * 10

    readonly property int metaDataDialogHeight: Kirigami.Units.gridUnit * 27
    readonly property int metaDataDialogWidth: Kirigami.Units.gridUnit * 33

    readonly property real mediaPlayerControlOpacity: 0.6
    readonly property int volumeSliderWidth: Kirigami.Units.gridUnit * 5

    readonly property int gridDelegateSize: Kirigami.Units.gridUnit * 9
    readonly property real listDelegateSingleLineHeight: Kirigami.Settings.isMobile
                                                            ? 4 * Kirigami.Units.smallSpacing + 2 * Kirigami.Units.gridUnit
                                                            : 3 * Kirigami.Units.smallSpacing + Kirigami.Units.gridUnit
    readonly property real listDelegateHeight: listDelegateSingleLineHeight + (Kirigami.Settings.isMobile ? 0 : Kirigami.Units.gridUnit)

    readonly property real contentViewMinimumSize: 2 * gridDelegateSize + scrollBar.width
    readonly property int viewSelectorSmallSizeThreshold: Kirigami.Units.gridUnit * 44

    readonly property alias toolButtonHeight: button.height
    readonly property alias toolBarHeaderMinimumHeight: stackedHeaderToolBarLabels.implicitHeight
    readonly property alias trackNumberWidth: trackNumber.width
    readonly property int playListEntryMinWidth: button.width * 6 + duration.width + trackNumber.width * 2
    readonly property int coverArtSize: Kirigami.Units.gridUnit * 2

    // Color to use for elements in the HeaderBar. As the nackground image
    // is always dark-ish, light theme colors might be hard to see.
    // This is the text color of Breeze Dark
    readonly property string headerForegroundColor: "#eff0f1"

    // get height of buttons inside loaders
    FlatButtonWithToolTip {
        id: button
        visible: false
        icon.name: "document-open-folder"
    }
    TextMetrics {
        id: trackNumber
        text: '99/9'
        font.bold: true
        font.features: { "tnum": 1 }
    }
    TextMetrics {
        id: duration
        text: '0:00:00'
        font.bold: true
        font.features: { "tnum": 1 }
    }
    QQC2.ScrollBar {
        id: scrollBar
        visible: false
    }

    ColumnLayout {
        id: stackedHeaderToolBarLabels
        visible: false
        spacing: 0
        LabelWithToolTip {
            id: mainTitle
            level: 4
        }
        LabelWithToolTip {
            id: subTitle
        }
    }
}
