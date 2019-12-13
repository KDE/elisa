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

BaseTheme {
    property real mediaPlayerControlOpacity: 0.8

    property string skipBackwardIcon: 'image://icon/media-skip-backward'
    property string pauseIcon: 'image://icon/media-playback-pause'
    property string playIcon: 'image://icon/media-playback-start'
    property string skipForwardIcon: 'image://icon/media-skip-forward'
}
