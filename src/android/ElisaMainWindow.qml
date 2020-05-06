/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import Qt.labs.settings 1.0
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

Kirigami.ApplicationWindow {
    id: mainWindow

    visible: true

    globalDrawer: Kirigami.GlobalDrawer {
        id: drawer
        title: i18n("Elisa")
        titleIcon: "elisa"
        contentItem.implicitWidth: Math.min (Kirigami.Units.gridUnit * 15, mainWindow.width * 0.8)
    }

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    title: i18n("Elisa")

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    Settings {
        id: persistentSettings

        property var playListState

        property var audioPlayerState

        property double playControlItemVolume : 100.0
        property bool playControlItemMuted : false

        property bool playControlItemRepeat : false
        property bool playControlItemShuffle : false

        property bool expandedFilterView: false

        property bool showPlaylist: true
    }

    Connections {
        target: Qt.application
        onAboutToQuit:
        {
            persistentSettings.playListState = elisa.mediaPlayList.persistentState;
            persistentSettings.audioPlayerState = elisa.audioControl.persistentState
        }
    }

    Connections {
        target: elisa.mediaPlayList

        onPlayListLoadFailed: {
            messageNotification.showNotification(i18nc("message of passive notification when playlist load failed", "Load of playlist failed"), 3000)
        }
    }

    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.TabBar

    Component {
        id: albumsView

        AlbumsView {
        }
    }

    Component {
        id: artistsView

        ArtistsView {
        }
    }

    Component {
        id: tracksView

        TracksView {
        }
    }

    Component {
        id: genresView

        GenresView {
        }
    }

    Component.onCompleted:
    {
        elisa.initialize()

        if (persistentSettings.playListState) {
            elisa.mediaPlayList.persistentState = persistentSettings.playListState
        }

        if (persistentSettings.audioPlayerState) {
            elisa.audioControl.persistentState = persistentSettings.audioPlayerState
        }

        pageStack.push(albumsView)
        pageStack.push(artistsView)
        pageStack.push(tracksView)
        pageStack.push(genresView)
    }
}
