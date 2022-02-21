/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.2 as Kirigami
import org.kde.elisa 1.0

Kirigami.AbstractListItem {
    property var index
    property bool isSingleDiscAlbum
    property int isPlaying
    property bool isSelected
    property bool isValid
    property bool isAlternateColor
    property int databaseId: 0
    property var entryType
    property string title
    property string artist
    property string album
    property string albumArtist
    property string duration
    property bool hideDiscNumber
    property url fileName
    property url imageUrl
    property int trackNumber
    property url trackUrl
    property int discNumber
    property int rating
    property bool hasValidDiscNumber: true
    property bool metadataModifiableRole: false

    property var listView
    property bool simpleMode: false
    property bool showDragHandle: true

    signal startPlayback()
    signal pausePlayback()
    signal removeFromPlaylist(int trackIndex)
    signal switchToTrack(int trackIndex)

    onSwitchToTrack: ElisaApplication.mediaPlayListProxyModel.switchTo(trackIndex)
    onStartPlayback: ElisaApplication.audioControl.ensurePlay()
    onPausePlayback: ElisaApplication.audioControl.playPause()
    onRemoveFromPlaylist: ElisaApplication.mediaPlayListProxyModel.removeRow(trackIndex)

    onClicked: {
        listView.currentIndex = index
        forceActiveFocus()

        if (model.isValid && (simpleMode || Kirigami.Settings.isMobile)) {
            switchToTrack(index)
            startPlayback()
        }
    }

    onDoubleClicked: {
        if (model.isValid) {
            switchToTrack(index)
            startPlayback()
        }
    }
}
