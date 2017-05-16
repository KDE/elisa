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

import QtQuick 2.0
import QtQuick.Window 2.2

Item {
    property string albumCover: 'image://icon/media-optical-audio'
    property string artistImage: 'image://icon/view-media-artist'
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

    property int layoutHorizontalMargin: 8 * Screen.devicePixelRatio
    property int layoutVerticalMargin: 6 * Screen.devicePixelRatio

    property int delegateHeight: 28 * Screen.devicePixelRatio
    property int delegateWithHeaderHeight: 84 * Screen.devicePixelRatio

    property int coverImageSize: 180 * Screen.devicePixelRatio
    property int smallImageSize: 32 * Screen.devicePixelRatio

    property int tooltipRadius: 3 * Screen.devicePixelRatio
    property int shadowOffset: 2 * Screen.devicePixelRatio

    property int delegateToolButtonSize: 40 * Screen.devicePixelRatio
    property int smallDelegateToolButtonSize: 20 * Screen.devicePixelRatio

    property int ratingStarSize: 10 * Screen.devicePixelRatio

    property int smallControlButtonHeight: 32 * Screen.devicePixelRatio
    property int bigControlButtonHeight: 44 * Screen.devicePixelRatio
    property int volumeSliderWidth: 140 * Screen.devicePixelRatio

    property int dragDropPlaceholderHeight: 28 * Screen.devicePixelRatio

    property int navigationBarHeight: 100 * Screen.devicePixelRatio
    property int mediaPlayerControlHeight: 48 * Screen.devicePixelRatio

    property int gridDelegateHeight: 168 * Screen.devicePixelRatio
    property int gridDelegateWidth: 112 * Screen.devicePixelRatio

    property int viewSelectorDelegateHeight: 32

    property int filterClearButtonMargin: layoutVerticalMargin
}
