/*
   SPDX-FileCopyrightText: 2026 (c) Frederick F. Ong <frederick.f.ong@pm.me>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma Singleton

import QtQml
import org.kde.kirigami as Kirigami
import org.kde.elisa

QtObject {
    property var quitQAction: ElisaApplication.action("file_quit")
    property Kirigami.Action quit: Kirigami.Action {
        fromQAction: quitQAction
    }

    property var goBackQAction: ElisaApplication.action("go_back")
    property Kirigami.Action goBack: Kirigami.Action {
        fromQAction: goBackQAction
    }

    property var clearPlaylistQAction: ElisaApplication.action("clear_playlist")
    property Kirigami.Action clearPlaylist: Kirigami.Action {
        fromQAction: clearPlaylistQAction
    }

    property var seekQAction: ElisaApplication.action("Seek")
    property Kirigami.Action seek: Kirigami.Action {
        fromQAction: seekQAction
    }

    property var scrubQAction: ElisaApplication.action("Scrub")
    property Kirigami.Action scrub: Kirigami.Action {
        fromQAction: scrubQAction
    }

    property var nextTrackQAction: ElisaApplication.action("NextTrack")
    property Kirigami.Action nextTrack: Kirigami.Action {
        fromQAction: nextTrackQAction
    }

    property var previousTrackQAction: ElisaApplication.action("PreviousTrack")
    property Kirigami.Action previousTrack: Kirigami.Action {
        fromQAction: previousTrackQAction
    }

    property var playPauseQAction: ElisaApplication.action("Play-Pause")
    property Kirigami.Action playPause: Kirigami.Action {
        fromQAction: playPauseQAction
    }

    property var editFindQAction: ElisaApplication.action("edit_find")
    property Kirigami.Action editFind: Kirigami.Action {
        fromQAction: editFindQAction
    }

    property var togglePartyModeQAction: ElisaApplication.action("togglePartyMode")
    property Kirigami.Action togglePartyMode: Kirigami.Action {
        fromQAction: togglePartyModeQAction
    }

    property var togglePlayListQAction: ElisaApplication.action("toggle_playlist")
    property Kirigami.Action togglePlayList: Kirigami.Action {
        fromQAction: togglePlayListQAction
    }

    property var configureElisaQAction: ElisaApplication.action("options_configure")
    property Kirigami.Action configureElisa: Kirigami.Action {
        fromQAction: configureElisaQAction
    }

    property var configureKeybindingQAction: ElisaApplication.action("options_configure_keybinding")
    property Kirigami.Action configureKeybinding: Kirigami.Action {
        fromQAction: configureKeybindingQAction
    }

    property var aboutElisaQAction: ElisaApplication.action("help_about_app")
    property Kirigami.Action aboutElisa: Kirigami.Action {
        fromQAction: aboutElisaQAction
    }

    property var aboutKDEQAction: ElisaApplication.action("help_about_kde")
    property Kirigami.Action aboutKDE: Kirigami.Action {
        fromQAction: aboutKDEQAction
    }

    property var reportBugQAction: ElisaApplication.action("help_report_bug")
    property Kirigami.Action reportBug: Kirigami.Action {
        fromQAction: reportBugQAction
    }
}
