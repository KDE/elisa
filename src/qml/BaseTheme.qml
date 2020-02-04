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
    property string defaultAlbumImage: 'image://icon/media-optical-audio'
    property string defaultArtistImage: 'image://icon/view-media-artist'
    property string defaultBackgroundImage: 'qrc:///background.png'
    property string nowPlayingIcon: 'image://icon/view-media-lyrics'
    property string artistIcon: 'image://icon/view-media-artist'
    property string albumIcon: 'image://icon/view-media-album-cover'
    property string albumCoverIcon: 'image://icon/media-optical-audio'
    property string playlistIcon: 'image://icon/view-media-playlist'
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

    property int layoutHorizontalMargin: 8
    property int layoutVerticalMargin: 6

    // FIXME: don't hardcode these; derive them from the layouts they're used in
    FontMetrics {
        id: playListTrackTextHeight
        font.weight: Font.Bold
    }
    property int playListDelegateHeight: (playListTrackTextHeight.height > 28) ? playListTrackTextHeight.height : 28
    // END FIXME

    property int playListAlbumArtSize: 70

    property int coverImageSize: 180
    property int contextCoverImageSize: 100
    property int smallImageSize: 32

    property int tooltipRadius: 3
    property int shadowOffset: 2

    property int delegateToolButtonSize: 34

    property int ratingStarSize: 15

    property int mediaPlayerControlHeight: 42
    property real mediaPlayerControlOpacity: 0.6
    property int smallControlButtonSize: 22
    property int volumeSliderWidth: 100

    property int dragDropPlaceholderHeight: 28

    property int gridDelegateSize: 170

    property int viewSelectorDelegateHeight: 24

    property int headerToolbarHeight: 48
    property int footerToolbarHeight: 30

    property int viewSelectorSmallSizeThreshold: 800

    Label {
        id: fontSize
    }
}
