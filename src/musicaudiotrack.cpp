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

#include "musicaudiotrack.h"

#include <QtCore/QDebug>

class MusicAudioTrackPrivate
{
public:

    MusicAudioTrackPrivate()
    {
    }

    MusicAudioTrackPrivate(bool aValid, QString aId, QString aParentId,
                           QString aTitle, QString aArtist, QString aAlbumName,
                           QString aAlbumArtist, int aTrackNumber, QTime aDuration,
                           QUrl aResourceURI, QUrl aAlbumCover)
        : mId(aId), mParentId(aParentId), mTitle(aTitle), mArtist(aArtist),
          mAlbumName(aAlbumName), mAlbumArtist(aAlbumArtist), mTrackNumber(aTrackNumber),
          mDuration(aDuration), mResourceURI(aResourceURI), mAlbumCover(aAlbumCover),
          mIsValid(aValid)
    {
    }

    qulonglong mDatabaseId = 0;

    QString mId;

    QString mParentId;

    QString mTitle;

    QString mArtist;

    QString mAlbumName;

    QString mAlbumArtist;

    int mTrackNumber = -1;

    QTime mDuration;

    QUrl mResourceURI;

    QUrl mAlbumCover;

    bool mIsValid = false;

};

MusicAudioTrack::MusicAudioTrack() : d(new MusicAudioTrackPrivate)
{
}

MusicAudioTrack::MusicAudioTrack(bool aValid, QString aId, QString aParentId,
                                 QString aTitle, QString aArtist, QString aAlbumName, QString aAlbumArtist,
                                 int aTrackNumber, QTime aDuration, QUrl aResourceURI, QUrl aAlbumCover)
    : d(new MusicAudioTrackPrivate(aValid, aId, aParentId, aTitle, aArtist, aAlbumName, aAlbumArtist, aTrackNumber, aDuration, aResourceURI, aAlbumCover))
{
}

MusicAudioTrack::MusicAudioTrack(MusicAudioTrack &&other) : d(other.d)
{
    other.d = nullptr;
}

MusicAudioTrack::MusicAudioTrack(const MusicAudioTrack &other) : d(new MusicAudioTrackPrivate(*other.d))
{
}

MusicAudioTrack::~MusicAudioTrack()
{
    delete d;
}

MusicAudioTrack& MusicAudioTrack::operator=(MusicAudioTrack &&other)
{
    if (this != &other) {
        delete d;
        d = other.d;
        other.d = nullptr;
    }

    return *this;
}

MusicAudioTrack& MusicAudioTrack::operator=(const MusicAudioTrack &other)
{
    if (this != &other) {
        (*d) = (*other.d);
    }

    return *this;
}

bool MusicAudioTrack::operator <(const MusicAudioTrack &other) const
{
    return d->mTrackNumber < other.d->mTrackNumber;
}

void MusicAudioTrack::setValid(bool value)
{
    d->mIsValid = value;
}

bool MusicAudioTrack::isValid() const
{
    return d->mIsValid;
}

void MusicAudioTrack::setDatabaseId(qulonglong value)
{
    d->mDatabaseId = value;
}

qulonglong MusicAudioTrack::databaseId() const
{
    return d->mDatabaseId;
}

void MusicAudioTrack::setId(const QString &value) const
{
    d->mId = value;
}

QString MusicAudioTrack::id() const
{
    return d->mId;
}

void MusicAudioTrack::setParentId(const QString &value) const
{
    d->mParentId = value;
}

QString MusicAudioTrack::parentId() const
{
    return d->mParentId;
}

void MusicAudioTrack::setTitle(const QString &value) const
{
    d->mTitle = value;
}

QString MusicAudioTrack::title() const
{
    return d->mTitle;
}

void MusicAudioTrack::setArtist(const QString &value) const
{
    d->mArtist = value;
}

QString MusicAudioTrack::artist() const
{
    return d->mArtist;
}

void MusicAudioTrack::setAlbumName(const QString &value) const
{
    d->mAlbumName = value;
}

QString MusicAudioTrack::albumName() const
{
    return d->mAlbumName;
}

void MusicAudioTrack::setAlbumArtist(const QString &value) const
{
    d->mAlbumArtist = value;
}

QString MusicAudioTrack::albumArtist() const
{
    return d->mAlbumArtist;
}

void MusicAudioTrack::setAlbumCover(const QUrl &value) const
{
    d->mAlbumCover = value;
}

QUrl MusicAudioTrack::albumCover() const
{
    return d->mAlbumCover;
}

void MusicAudioTrack::setTrackNumber(int value)
{
    d->mTrackNumber = value;
}

int MusicAudioTrack::trackNumber() const
{
    return d->mTrackNumber;
}

void MusicAudioTrack::setDuration(const QTime &value)
{
    d->mDuration = value;
}

const QTime &MusicAudioTrack::duration() const
{
    return d->mDuration;
}

void MusicAudioTrack::setResourceURI(const QUrl &value)
{
    d->mResourceURI = value;
}

const QUrl &MusicAudioTrack::resourceURI() const
{
    return d->mResourceURI;
}

QDebug& operator<<(QDebug &stream, const MusicAudioTrack &data)
{
    stream << data.title() << data.artist() << data.albumName() << data.albumArtist();
    return stream;
}
