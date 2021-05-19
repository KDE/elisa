/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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

            // play
            playListEntry.isPlaying = MediaPlayList.NotPlaying
            mouseClick(playPauseButtonItem);

            compare(startPlaybackSpy.count, 1);
            compare(pausePlaybackSpy.count, 0);
            compare(removeFromPlaylistSpy.count, 0);
            compare(switchToTrackSpy.count, 1);

            // pause
            playListEntry.isPlaying = MediaPlayList.IsPlaying
            mouseClick(playPauseButtonItem);

            compare(startPlaybackSpy.count, 1);
            compare(pausePlaybackSpy.count, 1);
            compare(removeFromPlaylistSpy.count, 0);
            compare(switchToTrackSpy.count, 1);

            // resume
            playListEntry.isPlaying = MediaPlayList.IsPaused
            mouseClick(playPauseButtonItem);

            compare(startPlaybackSpy.count, 2);
            compare(pausePlaybackSpy.count, 1);
            compare(removeFromPlaylistSpy.count, 0);
            compare(switchToTrackSpy.count, 1);

        }
    }
}
