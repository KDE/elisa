/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

import QtQuick 2.7
import QtQuick.Controls 1.4

Item {
    function dp(pixel) {
        // 96 - common, "base" DPI value
        return Math.round(pixel * logicalDpi / 96);
    }

    property string defaultAlbumImage: 'image://icon/media-optical-audio'
    property string defaultArtistImage: 'image://icon/view-media-artist'
    property string defaultBackgroundImage: 'qrc:///background.png'
    property string artistIcon: 'image://icon/view-media-artist'
    property string albumIcon: 'image://icon/view-media-album-cover'
    property string playlistIcon: 'image://icon/view-media-playlist'
    property string tracksIcon: 'image://icon/view-media-track'
    property string genresIcon: 'image://icon/view-media-genre'
    property string clearIcon: 'image://icon/edit-clear'
    property string skipBackwardIcon: 'image://icon/media-skip-backward'
    property string pauseIcon: 'image://icon/media-playback-pause'
    property string playIcon: 'image://icon/media-playback-start'
    property string skipForwardIcon: 'image://icon/media-skip-forward'
    property string playerVolumeMutedIcon: 'image://icon/player-volume-muted'
    property string playerVolumeIcon: 'image://icon/player-volume'
    property string ratingIcon: 'image://icon/rating'
    property string ratingUnratedIcon: 'image://icon/rating-unrated'
    property string errorIcon: 'image://icon/error'
    property string repeatIcon: 'image://icon/media-repeat-all'
    property string shuffleIcon: 'image://icon/media-playlist-shuffle'
    property string noRepeatIcon: 'image://icon/media-repeat-none'
    property string noShuffleIcon: 'image://icon/media-playlist-normal'
    property string folderIcon: 'image://icon/document-open-folder'

    property int layoutHorizontalMargin: dp(8)
    property int layoutVerticalMargin: dp(6)

    property int delegateHeight: dp(28)
    property int delegateWithHeaderHeight: dp(68)
    property int trackDelegateHeight: dp(45)

    property int coverImageSize: dp(180)
    property int smallImageSize: dp(32)

    property int trackMetadataWidth: dp(300)

    property int tooltipRadius: dp(3)
    property int shadowOffset: dp(2)

    property int delegateToolButtonSize: dp(34)
    property int smallDelegateToolButtonSize: dp(20)

    property int ratingStarSize: dp(15)

    property int mediaPlayerControlHeight: dp(42)
    property int mediaPlayerHorizontalMargin: dp(10)
    property real mediaPlayerControlOpacity: 0.8
    property int smallControlButtonSize: dp(22)
    property int volumeSliderWidth: dp(100)

    property int dragDropPlaceholderHeight: dp(28)

    property int navigationBarHeight: dp(100)
    property int navigationBarFilterHeight: dp(44)

    property int gridDelegateHeight: dp(100) + layoutVerticalMargin + fontSize.height * 2
    property int gridDelegateWidth: dp(100)

    property int viewSelectorDelegateHeight: dp(24)

    property int filterClearButtonMargin: 1

    property alias defaultFontPointSize: fontSize.font.pointSize

    Label {
        id: fontSize
    }
}
