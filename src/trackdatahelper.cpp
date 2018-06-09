/*
 * Copyright 2018 Alexander Stippich <a.stippich@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "trackdatahelper.h"

#include <QDebug>

TrackDataHelper::TrackDataHelper(QObject *parent) : QObject(parent)
{
}

TrackDataHelper::~TrackDataHelper()
= default;


const MusicAudioTrack &TrackDataHelper::trackData() const
{
    return *this;
}

void TrackDataHelper::setTrackData(const MusicAudioTrack &track)
{
    MusicAudioTrack::operator=(track);
    Q_EMIT trackDataChanged();
}

QString TrackDataHelper::title() const
{
    if (MusicAudioTrack::title().isEmpty()) {
        return fileName();
    } else {
        return MusicAudioTrack::title();
    }
}

QString TrackDataHelper::duration() const
{
    QString result;
    const QTime &trackDuration = MusicAudioTrack::duration();
    if (trackDuration.hour() == 0) {
        result = trackDuration.toString(QStringLiteral("mm:ss"));
    } else {
        result = trackDuration.toString(QStringLiteral("h:mm:ss"));
    }
    return result;
}

QString TrackDataHelper::resourceURI() const
{
    return MusicAudioTrack::resourceURI().toString();
}

QString TrackDataHelper::fileName() const
{
    return MusicAudioTrack::resourceURI().fileName();
}

bool TrackDataHelper::hasValidTrackNumber() const
{
    return trackNumber() > -1;
}

bool TrackDataHelper::hasValidDiscNumber() const
{
    return discNumber() > -1;
}

bool TrackDataHelper::hasValidChannels() const
{
    return channels() > -1;
}

bool TrackDataHelper::hasValidBitRate() const
{
    return bitRate() > -1;
}

bool TrackDataHelper::hasValidSampleRate() const
{
    return sampleRate() > -1;
}

bool TrackDataHelper::hasValidYear() const
{
    return MusicAudioTrack::year() != 0;
}

bool TrackDataHelper::hasValidRating() const
{
    return rating() > -1;
}

bool TrackDataHelper::hasValidTitle() const
{
    return !title().isEmpty();
}

bool TrackDataHelper::hasValidArtist() const
{
    return !artist().isEmpty();
}

bool TrackDataHelper::hasValidAlbumArtist() const
{
    return !albumArtist().isEmpty();
}

bool TrackDataHelper::hasValidAlbumName() const
{
    return !albumName().isEmpty();
}

bool TrackDataHelper::hasValidGenre() const
{
    return !genre().isEmpty();
}

bool TrackDataHelper::hasValidComposer() const
{
    return !composer().isEmpty();
}

bool TrackDataHelper::hasValidLyricist() const
{
    return !lyricist().isEmpty();
}

bool TrackDataHelper::hasValidComment() const
{
    return !comment().isEmpty();
}

bool TrackDataHelper::hasValidAlbumCover() const
{
    return !albumCover().isEmpty();
}

#include "moc_trackdatahelper.cpp"

