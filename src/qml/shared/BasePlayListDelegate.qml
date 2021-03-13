/*
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2

FocusScope {
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
    property int discNumber
    property int rating
    property bool hasValidDiscNumber: true
    property bool metadataModifiableRole: false

    property var listView

    signal startPlayback()
    signal pausePlayback()
    signal removeFromPlaylist(var trackIndex)
    signal switchToTrack(var trackIndex)
}
