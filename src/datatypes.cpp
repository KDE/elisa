/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "datatypes.h"

bool DataTypes::TrackDataType::albumInfoIsSame(const TrackDataType &other) const
{
    return hasAlbum() == other.hasAlbum() && album() == other.album() &&
           hasAlbumArtist() == other.hasAlbumArtist() && albumArtist() == other.albumArtist();
}

bool DataTypes::TrackDataType::isSameTrack(const TrackDataType& other) const
{
    return title() == other.title() &&
           hasAlbum() == other.hasAlbum() && (hasAlbum() ? album() == other.album() : true) &&
           hasArtist() == other.hasArtist() && (hasArtist() ? artist() == other.artist() : true) &&
           hasAlbumArtist() == other.hasAlbumArtist() && (hasAlbumArtist() ? albumArtist() == other.albumArtist() : true) &&
           hasTrackNumber() == other.hasTrackNumber() && (hasTrackNumber() ? trackNumber() == other.trackNumber() : true) &&
           hasDiscNumber() == other.hasDiscNumber() && (hasDiscNumber() ? discNumber() == other.discNumber() : true) &&
           duration() == other.duration() &&
           rating() == other.rating() &&
           resourceURI() == other.resourceURI() &&
           hasGenre() == other.hasGenre() && (hasGenre() ? genre() == other.genre() : true) &&
           hasComposer() == other.hasComposer() && (hasComposer() ? composer() == other.composer() : true) &&
           hasLyricist() == other.hasLyricist() && (hasLyricist() ? lyricist() == other.lyricist() : true) &&
           hasComment() == other.hasComment() && (hasComment() ? comment() == other.comment() : true) &&
           hasYear() == other.hasYear() && (hasYear() ? year() == other.year() : true) &&
           hasChannels() == other.hasChannels() && (hasChannels() ? channels() == other.channels() : true) &&
           hasBitRate() == other.hasBitRate() && (hasBitRate() ? bitRate() == other.bitRate() : true) &&
           hasSampleRate() == other.hasSampleRate() && (hasSampleRate() ? sampleRate() == other.sampleRate() : true);
}


#include "moc_datatypes.cpp"
