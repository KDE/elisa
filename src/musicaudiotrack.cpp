/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include <QDebug>
#include <utility>

class MusicAudioTrackPrivate
{
public:

    MusicAudioTrackPrivate()
    {
    }

    MusicAudioTrackPrivate(bool aValid, QString aId, QString aParentId,
                           QString aTitle, QString aArtist, QString aAlbumName,
                           QString aAlbumArtist, int aTrackNumber, QTime aDuration,
                           QUrl aResourceURI, QUrl aAlbumCover, int rating)
        : mId(std::move(aId)), mParentId(std::move(aParentId)), mTitle(std::move(aTitle)), mArtist(std::move(aArtist)),
          mAlbumName(std::move(aAlbumName)), mAlbumArtist(std::move(aAlbumArtist)), mTrackNumber(aTrackNumber),
          mDuration(aDuration), mResourceURI(std::move(aResourceURI)), mAlbumCover(std::move(aAlbumCover)),
          mRating(rating), mIsValid(aValid)
    {
    }

    MusicAudioTrackPrivate(bool aValid, QString aId, QString aParentId,
                           QString aTitle, QString aArtist, QString aAlbumName, QString aAlbumArtist,
                           int aTrackNumber, int aDiscNumber, QTime aDuration, QUrl aResourceURI,
                           QUrl aAlbumCover, int rating)
        : mId(std::move(aId)), mParentId(std::move(aParentId)), mTitle(std::move(aTitle)), mArtist(std::move(aArtist)),
          mAlbumName(std::move(aAlbumName)), mAlbumArtist(std::move(aAlbumArtist)), mTrackNumber(aTrackNumber),
          mDiscNumber(aDiscNumber), mDuration(aDuration), mResourceURI(std::move(aResourceURI)),
          mAlbumCover(std::move(aAlbumCover)), mRating(rating), mIsValid(aValid)
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

    int mDiscNumber = -1;

    QTime mDuration;

    QUrl mResourceURI;

    QUrl mAlbumCover;

    int mRating = -1;

    bool mIsValid = false;

};

MusicAudioTrack::MusicAudioTrack() : d(std::make_unique<MusicAudioTrackPrivate>())
{
}

MusicAudioTrack::MusicAudioTrack(bool aValid, const QString &aId, const QString &aParentId,
                                 const QString &aTitle, const QString &aArtist, const QString &aAlbumName, const QString &aAlbumArtist,
                                 int aTrackNumber, QTime aDuration, const QUrl &aResourceURI, const QUrl &aAlbumCover, int rating)
    : d(std::make_unique<MusicAudioTrackPrivate>(aValid, aId, aParentId, aTitle, aArtist, aAlbumName, aAlbumArtist, aTrackNumber, aDuration, aResourceURI, aAlbumCover, rating))
{
}

MusicAudioTrack::MusicAudioTrack(bool aValid, const QString &aId, const QString &aParentId,
                                 const QString &aTitle, const QString &aArtist, const QString &aAlbumName,
                                 const QString &aAlbumArtist, int aTrackNumber, int aDiscNumber,
                                 QTime aDuration, const QUrl &aResourceURI, const QUrl &aAlbumCover, int rating)
    : d(std::make_unique<MusicAudioTrackPrivate>(aValid, aId, aParentId, aTitle, aArtist, aAlbumName, aAlbumArtist, aTrackNumber, aDiscNumber, aDuration, aResourceURI, aAlbumCover, rating))
{
}

MusicAudioTrack::MusicAudioTrack(MusicAudioTrack &&other)
{
    d.swap(other.d);
}

MusicAudioTrack::MusicAudioTrack(const MusicAudioTrack &other) : d(std::make_unique<MusicAudioTrackPrivate>(*other.d))
{
}

MusicAudioTrack::~MusicAudioTrack()
= default;

MusicAudioTrack& MusicAudioTrack::operator=(MusicAudioTrack &&other)
{
    if (this != &other) {
        d.reset();
        d.swap(other.d);
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
    return d->mDiscNumber < other.d->mDiscNumber ||
            (d->mDiscNumber == other.d->mDiscNumber && d->mTrackNumber < other.d->mTrackNumber);
}

bool MusicAudioTrack::operator ==(const MusicAudioTrack &other) const
{
    return d->mTitle == other.d->mTitle && d->mArtist == other.d->mArtist &&
            d->mAlbumName == other.d->mAlbumName && d->mAlbumArtist == other.d->mAlbumArtist &&
            d->mTrackNumber == other.d->mTrackNumber && d->mDiscNumber == other.d->mDiscNumber &&
            d->mDuration == other.d->mDuration && d->mResourceURI == other.d->mResourceURI &&
            d->mAlbumCover == other.d->mAlbumCover && d->mRating == other.d->mRating;
}

bool MusicAudioTrack::operator !=(const MusicAudioTrack &other) const
{
    return d->mTitle != other.d->mTitle || d->mArtist != other.d->mArtist ||
            d->mAlbumName != other.d->mAlbumName || d->mAlbumArtist != other.d->mAlbumArtist ||
            d->mTrackNumber != other.d->mTrackNumber || d->mDiscNumber != other.d->mDiscNumber ||
            d->mDuration != other.d->mDuration || d->mResourceURI != other.d->mResourceURI ||
            d->mAlbumCover != other.d->mAlbumCover || d->mRating != other.d->mRating;}

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
    return (d->mAlbumArtist.isEmpty() ? d->mArtist : d->mAlbumArtist);
}

bool MusicAudioTrack::isValidAlbumArtist() const
{
    return !d->mAlbumArtist.isEmpty();
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

void MusicAudioTrack::setDiscNumber(int value)
{
    d->mDiscNumber = value;
}

int MusicAudioTrack::discNumber() const
{
    return d->mDiscNumber;
}

void MusicAudioTrack::setDuration(QTime value)
{
    d->mDuration = value;
}

QTime MusicAudioTrack::duration() const
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

void MusicAudioTrack::setRating(int value) const
{
    d->mRating = value;
}

int MusicAudioTrack::rating() const
{
    return d->mRating;
}

QDebug& operator<<(QDebug &stream, const MusicAudioTrack &data)
{
    stream << data.title() << data.artist() << data.albumName() << data.albumArtist() << data.duration();
    return stream;
}
