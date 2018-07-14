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

#include <QDateTime>
#include <QDebug>
#include <utility>

class MusicAudioTrackPrivate
{
public:

    MusicAudioTrackPrivate() = default;

    MusicAudioTrackPrivate(bool aValid, QString aId, QString aParentId,
                           QString aTitle, QString aArtist, QString aAlbumName, QString aAlbumArtist,
                           int aTrackNumber, int aDiscNumber, QTime aDuration, QUrl aResourceURI,
                           QDateTime fileModificationTime, QUrl aAlbumCover, int rating,
                           bool aIsSingleDiscAlbum, QString aGenre, QString aComposer,
                           QString aLyricist)
        : mId(std::move(aId)), mParentId(std::move(aParentId)), mTitle(std::move(aTitle)), mArtist(std::move(aArtist)),
          mAlbumName(std::move(aAlbumName)), mAlbumArtist(std::move(aAlbumArtist)),
          mGenre(std::move(aGenre)), mComposer(std::move(aComposer)), mLyricist(std::move(aLyricist)),
          mResourceURI(std::move(aResourceURI)),
          mAlbumCover(std::move(aAlbumCover)),
          mFileModificationTime(std::move(fileModificationTime)), mDuration(aDuration),
          mTrackNumber(aTrackNumber), mDiscNumber(aDiscNumber), mRating(rating),
          mIsValid(aValid), mIsSingleDiscAlbum(aIsSingleDiscAlbum)
    {
    }

    QString mId;

    QString mParentId;

    QString mTitle;

    QString mArtist;

    QString mAlbumName;

    QString mAlbumArtist;

    QString mGenre;

    QString mComposer;

    QString mLyricist;

    QString mComment;

    QUrl mResourceURI;

    QUrl mAlbumCover;

    qulonglong mDatabaseId = 0;

    qulonglong mAlbumId = 0;

    QDateTime mFileModificationTime;

    QTime mDuration;

    int mTrackNumber = -1;

    int mDiscNumber = -1;

    int mChannels = -1;

    int mBitRate = -1;

    int mSampleRate = -1;

    int mYear = 0;

    int mRating = -1;

    bool mIsValid = false;

    bool mIsSingleDiscAlbum = true;

};

MusicAudioTrack::MusicAudioTrack() : d(std::make_unique<MusicAudioTrackPrivate>())
{
}

MusicAudioTrack::MusicAudioTrack(bool aValid, QString aId, QString aParentId, QString aTitle, QString aArtist, QString aAlbumName,
                                 QString aAlbumArtist, int aTrackNumber, int aDiscNumber, QTime aDuration, QUrl aResourceURI,
                                 const QDateTime &fileModificationTime, QUrl aAlbumCover, int rating, bool aIsSingleDiscAlbum,
                                 QString aGenre, QString aComposer, QString aLyricist)
    : d(std::make_unique<MusicAudioTrackPrivate>(aValid, std::move(aId), std::move(aParentId),
                                                 std::move(aTitle), std::move(aArtist),
                                                 std::move(aAlbumName), std::move(aAlbumArtist),
                                                 aTrackNumber, aDiscNumber, aDuration,
                                                 std::move(aResourceURI), fileModificationTime,
                                                 std::move(aAlbumCover), rating,
                                                 aIsSingleDiscAlbum, std::move(aGenre),
                                                 std::move(aComposer), std::move(aLyricist)))
{
}

MusicAudioTrack::MusicAudioTrack(MusicAudioTrack &&other) noexcept
{
    d.swap(other.d);
}

MusicAudioTrack::MusicAudioTrack(const MusicAudioTrack &other) : d(std::make_unique<MusicAudioTrackPrivate>(*other.d))
{
}

MusicAudioTrack::~MusicAudioTrack()
= default;

MusicAudioTrack& MusicAudioTrack::operator=(MusicAudioTrack &&other) noexcept
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
            d->mFileModificationTime == other.d->mFileModificationTime &&
            d->mAlbumCover == other.d->mAlbumCover && d->mRating == other.d->mRating &&
            d->mGenre == other.d->mGenre && d->mComposer == other.d->mComposer &&
            d->mLyricist == other.d->mLyricist && d->mComment == other.d->mComment &&
            d->mYear == other.d->mYear && d->mChannels == other.d->mChannels &&
            d->mBitRate == other.d->mBitRate && d->mSampleRate == other.d->mSampleRate;

}

bool MusicAudioTrack::operator !=(const MusicAudioTrack &other) const
{
    return d->mTitle != other.d->mTitle || d->mArtist != other.d->mArtist ||
            d->mAlbumName != other.d->mAlbumName || d->mAlbumArtist != other.d->mAlbumArtist ||
            d->mTrackNumber != other.d->mTrackNumber || d->mDiscNumber != other.d->mDiscNumber ||
            d->mDuration != other.d->mDuration || d->mResourceURI != other.d->mResourceURI ||
            d->mFileModificationTime != other.d->mFileModificationTime ||
            d->mAlbumCover != other.d->mAlbumCover || d->mRating != other.d->mRating ||
            d->mGenre != other.d->mGenre || d->mComposer != other.d->mComposer ||
            d->mLyricist != other.d->mLyricist || d->mComment != other.d->mComment ||
            d->mYear != other.d->mYear || d->mChannels != other.d->mChannels ||
            d->mBitRate != other.d->mBitRate || d->mSampleRate != other.d->mSampleRate;
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

void MusicAudioTrack::setAlbumId(qulonglong value)
{
    d->mAlbumId = value;
}

qulonglong MusicAudioTrack::albumId() const
{
    return d->mAlbumId;
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

void MusicAudioTrack::setGenre(const QString &value) const
{
    d->mGenre = value;
}

QString MusicAudioTrack::genre() const
{
    return d->mGenre;
}

void MusicAudioTrack::setComposer(const QString &value) const
{
    d->mComposer = value;
}

QString MusicAudioTrack::composer() const
{
    return d->mComposer;
}

void MusicAudioTrack::setLyricist(const QString &value) const
{
    d->mLyricist = value;
}

QString MusicAudioTrack::lyricist() const
{
    return d->mLyricist;
}

void MusicAudioTrack::setComment(const QString &value) const
{
    d->mComment = value;
}

QString MusicAudioTrack::comment() const
{
    return d->mComment;
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

void MusicAudioTrack::setYear(int value)
{
    d->mYear = value;
}

int MusicAudioTrack::year() const
{
    return d->mYear;
}

void MusicAudioTrack::setChannels(int value)
{
    d->mChannels = value;
}

int MusicAudioTrack::channels() const
{
    return d->mChannels;
}

void MusicAudioTrack::setBitRate(int value)
{
    d->mBitRate = value;
}

int MusicAudioTrack::bitRate() const
{
    return d->mBitRate;
}

void MusicAudioTrack::setSampleRate(int value)
{
    d->mSampleRate = value;
}

int MusicAudioTrack::sampleRate() const
{
    return d->mSampleRate;
}

void MusicAudioTrack::setDuration(QTime value)
{
    d->mDuration = value;
}

QTime MusicAudioTrack::duration() const
{
    return d->mDuration;
}

void MusicAudioTrack::setFileModificationTime(const QDateTime &value)
{
    d->mFileModificationTime = value;
}

const QDateTime &MusicAudioTrack::fileModificationTime() const
{
    return d->mFileModificationTime;
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

void MusicAudioTrack::setIsSingleDiscAlbum(bool value)
{
    d->mIsSingleDiscAlbum = value;
}

bool MusicAudioTrack::isSingleDiscAlbum() const
{
    return d->mIsSingleDiscAlbum;
}

ELISALIB_EXPORT QDebug operator<<(QDebug stream, const MusicAudioTrack &data)
{
    stream << data.title() << data.artist() << data.albumName() << data.albumArtist() << data.duration() << data.resourceURI();
    return stream;
}
