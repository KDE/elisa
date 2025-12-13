/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick 2.7
import QtQuick.Controls as QQC2

import org.kde.ki18n
import org.kde.elisa

QQC2.ItemDelegate {
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
    readonly property bool isFavorite: rating === 10

    property var listView
    property bool simpleMode: false
    property bool showDragHandle: true

    activeFocusOnTab: isSelected

    signal startPlayback()
    signal pausePlayback()
    signal removeFromPlaylist(int trackIndex)
    signal switchToTrack(int trackIndex)

    onSwitchToTrack: trackIndex => ElisaApplication.mediaPlayListProxyModel.switchTo(trackIndex)
    onStartPlayback: ElisaApplication.audioControl.ensurePlay()
    onPausePlayback: ElisaApplication.audioControl.playPause()
    onRemoveFromPlaylist: trackIndex => ElisaApplication.mediaPlayListProxyModel.removeRow(trackIndex)

    onClicked: {
        listView.currentIndex = index
        forceActiveFocus()

        if (isValid) {
            switchToTrack(index)
            startPlayback()
        }
    }

    onIsValidChanged: {
        if (!isValid) {
            showPassiveNotification(KI18n.i18nc("@info:status", "\"%1\" could not be played", title))
            // Can't make it too long due to https://bugs.kde.org/show_bug.cgi?id=476021.
            // TODO: Once that's fixed, use this longer and more descriptive string!
            // showPassiveNotification(KI18n.i18nc("@label", "\"%1\" could not be played; make sure it has not been deleted or renamed", title))
        }
    }
}
