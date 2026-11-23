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
    required property var model

    property var index: model.index
    property bool isSingleDiscAlbum: model.isSingleDiscAlbum ?? true
    property int isPlaying: model.isPlaying ?? 0
    property bool isSelected
    property bool isValid: model.isValid ?? false
    property bool isAlternateColor
    property int databaseId: model.databaseId ?? 0
    property var entryType: model.entryType ?? ElisaUtils.Unknown
    property string title: model.title ?? ''
    property string artist: model.artist ?? ''
    property string album: model.album ?? ''
    property string albumArtist: model.albumArtist ?? ''
    property string duration: model.duration ?? ''
    property bool hideDiscNumber
    property url fileName: model.trackResource ?? ''
    property url imageUrl: model.imageUrl ?? ''
    property int trackNumber: model.trackNumber ?? -1
    property url trackUrl
    property int discNumber: model.discNumber ?? -1
    property int rating: model.rating ?? 0
    property bool hasValidDiscNumber: true
    property bool metadataModifiableRole: model.metadataModifiableRole ?? false
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
