/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.3
import QtTest 1.0
import "../../src/qml"
import org.kde.elisa 1.0

Item {
    height: 50
    width: 300

    PlayListEntry {
        id: playListEntry

        height: 50
        width: 300

        isAlternateColor: false
        isSingleDiscAlbum: false
        title: "hello"
        isValid: true
        isPlaying: MediaPlayList.IsPaused
        isSelected: true
        containsMouse: false

        function i18nc() {
            return ""
        }

        SystemPalette {
            id: myPalette
        }

        Item {
            id: elisaTheme

            property int layoutHorizontalMargin: 8
            property int playListDelegateHeight: 28
        }
    }

    TestCase {
        name: "TestPlayListEntry"

        SignalSpy {
            id: startPlaybackSpy
            target: playListEntry
            signalName: "startPlayback"
        }

        SignalSpy {
            id: pausePlaybackSpy
            target: playListEntry
            signalName: "pausePlayback"
        }

        SignalSpy {
            id: removeFromPlaylistSpy
            target: playListEntry
            signalName: "removeFromPlaylist"
        }

        SignalSpy {
            id: switchToTrackSpy
            target: playListEntry
            signalName: "switchToTrack"
        }

        when: windowShown

        function init() {
            startPlaybackSpy.clear();
            pausePlaybackSpy.clear();
            removeFromPlaylistSpy.clear();
            switchToTrackSpy.clear();
        }

        function test_playnow_click() {
            compare(startPlaybackSpy.count, 0);
            compare(pausePlaybackSpy.count, 0);
            compare(removeFromPlaylistSpy.count, 0);
            compare(switchToTrackSpy.count, 0);

            playListEntry.forceActiveFocus();

            compare(startPlaybackSpy.count, 0);
            compare(pausePlaybackSpy.count, 0);
            compare(removeFromPlaylistSpy.count, 0);
            compare(switchToTrackSpy.count, 0);

            var playPauseButtonItem = findChild(playListEntry, "playPauseButton");
            verify(playPauseButtonItem !== null, "valid playPauseButton")

            mouseMove(playPauseButtonItem, playPauseButtonItem.width / 2, playPauseButtonItem.height / 2);
            wait(300);

            mouseClick(playPauseButtonItem);

            compare(startPlaybackSpy.count, 1);
            compare(pausePlaybackSpy.count, 0);
            compare(removeFromPlaylistSpy.count, 0);
            compare(switchToTrackSpy.count, 1);


        }
    }
}
