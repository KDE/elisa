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

Item {
    property string albumCover: 'media-optical-audio.svg'
    property string artistImage: 'view-media-artist.svg'
    property string clearIcon: 'edit-clear.svg'
    property string skipBackwardIcon: 'media-skip-backward.svg'
    property string pauseIcon: 'media-playback-pause.svg'
    property string playIcon: 'media-playback-start.svg'
    property string skipForwardIcon: 'media-skip-forward.svg'
    property string playerVolumeMutedIcon: 'player-volume-muted.svg'
    property string playerVolumeIcon: 'player-volume.svg'
    property string ratingIcon: 'rating.svg'
    property string ratingUnratedIcon: 'rating-unrated.svg'
    property string errorIcon: 'error.svg'

    property int layoutHorizontalMargin: 8
    property int layoutVerticalMargin: 6

    property int delegateHeight: 28
    property int delegateWithHeaderHeight: 84

    property int coverImageSize: 180
    property int smallImageSize: 32

    property int tooltipRadius: 3
    property int shadowOffset: 2

    property int delegateToolButtonSize: 34
    property int smallDelegateToolButtonSize: 20

    property int ratingStarSize: 10

    property int mediaPlayerControlHeight: 48
    property int smallControlButtonHeight: 32
    property int bigControlButtonHeight: 44
    property int volumeSliderWidth: 140

    property int dragDropPlaceholderHeight: 28

    property int navigationBarHeight: 100

    property int gridDelegateHeight: 168
    property int gridDelegateWidth: 112

    property int viewSelectorDelegateHeight: 32

    property int filterClearButtonMargin: 1
}
