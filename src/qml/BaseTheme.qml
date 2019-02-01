/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.7
import QtQuick.Controls 2.2

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
    property string albumCoverIcon: 'image://icon/media-optical-audio'
    property string playlistIcon: 'image://icon/view-media-playlist'
    property string tracksIcon: 'image://icon/view-media-track'
    property string genresIcon: 'image://icon/view-media-genre'
    property string clearIcon: 'image://icon/edit-clear'
    property string recentlyPlayedTracksIcon: 'image://icon/media-playlist-play'
    property string frequentlyPlayedTracksIcon: 'image://icon/amarok_playcount'
    property string pausedIndicatorIcon: 'image://icon/media-playback-paused'
    property string playingIndicatorIcon: 'image://icon/media-playback-playing'
    property string ratingIcon: 'image://icon/rating'
    property string ratingUnratedIcon: 'image://icon/rating-unrated'
    property string errorIcon: 'image://icon/error'
    property string folderIcon: 'image://icon/document-open-folder'

    property int layoutHorizontalMargin: dp(8)
    property int layoutVerticalMargin: dp(6)

    property int delegateHeight: dp(28)

    FontMetrics {
        id: playListAuthorTextHeight
        font.weight: Font.Light
    }

    FontMetrics {
        id: playListAlbumTextHeight
        font.weight: Font.Bold
        font.pointSize: elisaTheme.defaultFontPointSize * 1.4
    }

    FontMetrics {
        id: playListTrackTextHeight
        font.weight: Font.Bold
    }

    property int playListDelegateHeight: (playListTrackTextHeight.height > dp(28)) ? playListTrackTextHeight.height : dp(28)
    property int playListDelegateWithHeaderHeight: playListDelegateHeight +
                                                   elisaTheme.layoutVerticalMargin * 5 +
                                                   playListAuthorTextHeight.height +
                                                   playListAlbumTextHeight.height
    property int playListHeaderHeight: elisaTheme.layoutVerticalMargin * 5 +
                                       playListAuthorTextHeight.height +
                                       playListAlbumTextHeight.height

    property int trackDelegateHeight: dp(45)

    property int coverImageSize: dp(180)
    property int smallImageSize: dp(32)

    property int maximumMetadataWidth: dp(300)

    property int tooltipRadius: dp(3)
    property int shadowOffset: dp(2)

    property int delegateToolButtonSize: dp(34)
    property int smallDelegateToolButtonSize: dp(20)

    property int ratingStarSize: dp(15)

    property int mediaPlayerControlHeight: dp(42)
    property int mediaPlayerHorizontalMargin: dp(10)
    property real mediaPlayerControlOpacity: 0.6
    property int smallControlButtonSize: dp(22)
    property int volumeSliderWidth: dp(100)

    property int dragDropPlaceholderHeight: dp(28)

    property int navigationBarHeight: dp(100)
    property int navigationBarFilterHeight: dp(44)

    property int gridDelegateHeight: dp(100) + layoutVerticalMargin + fontSize.height * 2
    property int gridDelegateWidth: dp(100)

    property int viewSelectorDelegateHeight: dp(24)

    property int filterClearButtonMargin: layoutVerticalMargin

    property alias defaultFontPointSize: fontSize.font.pointSize

    TextMetrics {
        id: bigTextSize
        font.pointSize: defaultFontPointSize * 1.4
        text: "Albums"
    }

    property int viewSelectorSmallSizeThreshold: 3 * layoutHorizontalMargin + viewSelectorDelegateHeight + bigTextSize.width

    Label {
        id: fontSize
    }
}
