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

TrackDataHelper::TrackDataHelper(QObject *parent) : QObject(parent)
{
}

TrackDataHelper::~TrackDataHelper()
= default;


MusicAudioTrack TrackDataHelper::trackData() const
{
    MusicAudioTrack temp = *this;
    return temp;
}

void TrackDataHelper::setTrackData(const MusicAudioTrack &track)
{
    MusicAudioTrack::operator=(track);
    Q_EMIT trackDataChanged();
}

QString TrackDataHelper::trackNumber() const
{
    return QString::number(MusicAudioTrack::trackNumber());
}

QString TrackDataHelper::discNumber() const
{
    return QString::number(MusicAudioTrack::discNumber());
}

QString TrackDataHelper::channels() const
{
    return QString::number(MusicAudioTrack::channels());
}

QString TrackDataHelper::bitRate() const
{
    return QString::number(MusicAudioTrack::bitRate()/1000);
}

QString TrackDataHelper::sampleRate() const
{
    return QString::number(MusicAudioTrack::sampleRate());
}

QString TrackDataHelper::year() const
{
    return QString::number(MusicAudioTrack::year());
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

bool TrackDataHelper::hasValidTrackNumber() const
{
    return MusicAudioTrack::trackNumber() > -1;
}

bool TrackDataHelper::hasValidDiscNumber() const
{
    return MusicAudioTrack::discNumber() > -1;
}

bool TrackDataHelper::hasValidChannels() const
{
    return MusicAudioTrack::channels() > -1;
}

bool TrackDataHelper::hasValidBitRate() const
{
    return MusicAudioTrack::bitRate() > -1;
}

bool TrackDataHelper::hasValidSampleRate() const
{
    return MusicAudioTrack::sampleRate() > -1;
}

bool TrackDataHelper::hasValidYear() const
{
    return MusicAudioTrack::year() != 0;
}

bool TrackDataHelper::hasValidRating() const
{
    return MusicAudioTrack::rating() > -1;
}

bool TrackDataHelper::hasValidTitle() const
{
    return !MusicAudioTrack::title().isEmpty();
}

bool TrackDataHelper::hasValidArtist() const
{
    return !MusicAudioTrack::artist().isEmpty();
}

bool TrackDataHelper::hasValidAlbumArtist() const
{
    return !MusicAudioTrack::albumArtist().isEmpty();
}

bool TrackDataHelper::hasValidAlbumName() const
{
    return !MusicAudioTrack::albumName().isEmpty();
}

bool TrackDataHelper::hasValidGenre() const
{
    return !MusicAudioTrack::genre().isEmpty();
}

bool TrackDataHelper::hasValidComposer() const
{
    return !MusicAudioTrack::composer().isEmpty();
}

bool TrackDataHelper::hasValidLyricist() const
{
    return !MusicAudioTrack::lyricist().isEmpty();
}

bool TrackDataHelper::hasValidComment() const
{
    return !MusicAudioTrack::comment().isEmpty();
}

bool TrackDataHelper::hasValidAlbumCover() const
{
    return !MusicAudioTrack::albumCover().isEmpty();
}

#include "moc_trackdatahelper.cpp"

