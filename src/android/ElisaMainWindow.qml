/*
 * Copyright 2016-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
