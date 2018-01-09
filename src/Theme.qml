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
    property string defaultAlbumImage: 'image://icon/media-optical-audio'
    property string defaultArtistImage: 'image://icon/view-media-artist'
    property string defaultBackgroundImage: 'background.jpg'
    property string artistIcon: 'image://icon/view-media-artist'
    property string albumIcon: 'image://icon/media-album-cover'
    property string playlistIcon: 'image://icon/amarok_playlist'
    property string tracksIcon: 'image://icon/media-album-track'
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

    property int layoutHorizontalMargin: 8
    property int layoutVerticalMargin: 6

    property int delegateHeight: 28
    property int delegateWithHeaderHeight: 68
    property int trackDelegateHeight: 45

    property int coverImageSize: 180
    property int smallImageSize: 32

    property int tooltipRadius: 3
    property int shadowOffset: 2

    property int delegateToolButtonSize: 34
    property int smallDelegateToolButtonSize: 20

    property int ratingStarSize: 15

    property int mediaPlayerControlHeight: 48
    property int smallControlButtonHeight: 32
    property int bigControlButtonHeight: 44
    property int volumeSliderWidth: 140

    property int dragDropPlaceholderHeight: 28

    property int navigationBarHeight: 100

    property int gridDelegateHeight: 100 + layoutVerticalMargin + fontSize.height * 2
    property int gridDelegateWidth: 100

    property int viewSelectorDelegateHeight: 32

    property int filterClearButtonMargin: layoutVerticalMargin

    property alias defaultFontPixelSize: fontSize.font.pixelSize

    Label {
        id: fontSize
    }
}
