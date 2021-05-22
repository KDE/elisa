/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import org.kde.kirigami 2.12 as Kirigami

Item {
    property string defaultAlbumImage: 'image://icon/media-optical-audio'
    property string defaultArtistImage: 'image://icon/view-media-artist'
    property string defaultBackgroundImage: 'qrc:///background.png'
    property string nowPlayingIcon: 'image://icon/view-media-lyrics'
    property string artistIcon: 'image://icon/view-media-artist'
    property string albumIcon: 'image://icon/view-media-album-cover'
    property string albumCoverIcon: 'image://icon/media-optical-audio'
    property string tracksIcon: 'image://icon/view-media-track'
    property string genresIcon: 'image://icon/view-media-genre'
    property string clearIcon: 'image://icon/edit-clear'
    property string recentlyPlayedTracksIcon: 'image://icon/media-playlist-play'
    property string frequentlyPlayedTracksIcon: 'image://icon/view-media-playcount'
    property string pausedIndicatorIcon: 'image://icon/media-playback-paused'
    property string playingIndicatorIcon: 'image://icon/media-playback-playing'
    property string ratingIcon: 'image://icon/rating'
    property string ratingUnratedIcon: 'image://icon/rating-unrated'
    property string errorIcon: 'image://icon/error'
    property string folderIcon: 'image://icon/document-open-folder'

    property int hairline: Math.floor(Kirigami.Units.devicePixelRatio)

    property int coverImageSize: 180
    property int contextCoverImageSize: 100
    property int smallImageSize: 32

    property int metaDataDialogHeight: 500 * Kirigami.Units.devicePixelRatio
    property int metaDataDialogWidth: 600 * Kirigami.Units.devicePixelRatio

    property int tooltipRadius: 3
    property int shadowOffset: 2

    property int mediaPlayerControlHeight: Kirigami.Settings.isMobile? Math.round(Kirigami.Units.gridUnit * 3.5) : Math.round(Kirigami.Units.gridUnit * 2.5)
    property real mediaPlayerControlOpacity: 0.6
    property int volumeSliderWidth: 100

    property int dragDropPlaceholderHeight: 28

    property int gridDelegateSize: 170

    property int viewSelectorSmallSizeThreshold: 800

    readonly property alias sectionHeight: sectionSizer.implicitHeight

    // calculate a fixed height for playlist's section delegates
    // workaround for QTBUG-52595
    Column {
        id: sectionSizer
        visible: false
        spacing: Kirigami.Units.smallSpacing
        LabelWithToolTip { text: "M\nM"; level: 2 }
        LabelWithToolTip { text: "M" }
    }
}
