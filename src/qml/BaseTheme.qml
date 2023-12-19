/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import org.kde.kirigami 2.12 as Kirigami

Item {
    property string defaultAlbumImage: 'image://icon/media-default-album'
    property string defaultBackgroundImage: 'qrc:///background.png'
    property string pausedIndicatorIcon: 'image://icon/media-playback-paused'
    property string playingIndicatorIcon: 'image://icon/media-playback-playing'
    property string ratingIcon: 'image://icon/rating'
    property string ratingUnratedIcon: 'image://icon/rating-unrated'
    property string folderIcon: 'image://icon/document-open-folder'

    property int coverImageSize: 180

    property int metaDataDialogHeight: 500
    property int metaDataDialogWidth: 600

    property int mediaPlayerControlHeight: Kirigami.Settings.isMobile? Math.round(Kirigami.Units.gridUnit * 3.5) : Math.round(Kirigami.Units.gridUnit * 2.5)
    property real mediaPlayerControlOpacity: 0.6
    property int volumeSliderWidth: 100

    property int gridDelegateSize: 170

    property int viewSelectorSmallSizeThreshold: 800

    readonly property alias toolButtonHeight: button.height
    readonly property alias trackNumberWidth: trackNumber.width
    readonly property int playListEntryMinWidth: button.width * 6 + duration.width + trackNumber.width * 2
    readonly property int coverArtSize: Kirigami.Units.gridUnit * 2

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
    }
    TextMetrics {
        id: duration
        text: '0:00:00'
        font.bold: true
    }
}
