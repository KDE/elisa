/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "musicstatistics.h"

#include <QtCore/QDebug>

MusicStatistics::MusicStatistics(QObject *parent) : QObject(parent)
{
}

void MusicStatistics::newAlbum(const MusicAlbum &album)
{
    Q_UNUSED(album);
}

void MusicStatistics::newAudioTrack(const MusicAudioTrack &audioTrack)
{
qDebug() << "new audio track" << audioTrack.mAlbumName << audioTrack.mTitle << audioTrack.mArtist << audioTrack.mDuration;
}


#include "moc_musicstatistics.cpp"

