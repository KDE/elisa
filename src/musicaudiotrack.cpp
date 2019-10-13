/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "musicaudiotrack.h"

#include <QDateTime>
#include <QDebug>
#include <utility>
#include <QSharedData>

class MusicAudioTrackPrivate : public QSharedData
{
public:

    MusicAudioTrackPrivate() = default;

    MusicAudioTrackPrivate(bool aValid, QString aId, QString aParentId,
                           QString aTitle, QString aArtist, QString aAlbumName, QString aAlbumArtist,
                           int aTrackNumber, int aDiscNumber, QTime aDuration, QUrl aResourceURI,
                           QDateTime fileModificationTime, QUrl aAlbumCover, int rating,
                           bool aIsSingleDiscAlbum, QString aGenre, QString aComposer,
                           QString aLyricist, bool aHasEmbeddedCover)
        : QSharedData(), mId(std::move(aId)), mParentId(std::move(aParentId)),
          mTitle(std::move(aTitle)), mArtist(std::move(aArtist)),
          mAlbumName(std::move(aAlbumName)), mAlbumArtist(std::move(aAlbumArtist)),
          mGenre(std::move(aGenre)), mComposer(std::move(aComposer)), mLyricist(std::move(aLyricist)),
          mLyrics(), mResourceURI(std::move(aResourceURI)), mAlbumCover(std::move(aAlbumCover)),
          mFileModificationTime(std::move(fileModificationTime)), mDuration(aDuration),
          mTrackNumber(aTrackNumber), mDiscNumber(aDiscNumber), mRating(rating),
          mIsValid(aValid), mIsSingleDiscAlbum(aIsSingleDiscAlbum), mHasBooleanCover(aHasEmbeddedCover),
          mTrackNumberIsValid(true), mDiscNumberIsValid(true)
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

    QString mLyrics;

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

    int mRating = 0;

    bool mIsValid = false;

    bool mIsSingleDiscAlbum = true;

    bool mHasBooleanCover = false;

    bool mTrackNumberIsValid = false;

    bool mDiscNumberIsValid = false;

    bool mChannelsIsValid = false;

    bool mBitRateIsValid = false;

    bool mSampleRateIsValid = false;

};

MusicAudioTrack::MusicAudioTrack() : d(new MusicAudioTrackPrivate())
{
}

MusicAudioTrack::MusicAudioTrack(bool aValid, QString aId, QString aParentId, QString aTitle, QString aArtist, QString aAlbumName,
                                 QString aAlbumArtist, int aTrackNumber, int aDiscNumber, QTime aDuration, QUrl aResourceURI,
                                 const QDateTime &fileModificationTime, QUrl aAlbumCover, int rating, bool aIsSingleDiscAlbum,
                                 QString aGenre, QString aComposer, QString aLyricist, bool aHasEmbeddedCover)
    : d(new MusicAudioTrackPrivate(aValid, std::move(aId), std::move(aParentId),
                                   std::move(aTitle), std::move(aArtist),
                                   std::move(aAlbumName), std::move(aAlbumArtist),
                                   aTrackNumber, aDiscNumber, aDuration,
                                   std::move(aResourceURI), fileModificationTime,
                                   std::move(aAlbumCover), rating,
                                   aIsSingleDiscAlbum, std::move(aGenre),
                                   std::move(aComposer), std::move(aLyricist), aHasEmbeddedCover))
{
}

MusicAudioTrack::MusicAudioTrack(MusicAudioTrack &&other) noexcept = default;

MusicAudioTrack::MusicAudioTrack(const MusicAudioTrack &other) = default;

MusicAudioTrack::~MusicAudioTrack()
= default;

MusicAudioTrack& MusicAudioTrack::operator=(MusicAudioTrack &&other) noexcept = default;

MusicAudioTrack& MusicAudioTrack::operator=(const MusicAudioTrack &other) = default;

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
            d->mLyricist == other.d->mLyricist && d->mLyrics == other.d->mLyrics &&
            d->mComment == other.d->mComment &&
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
            d->mLyricist != other.d->mLyricist || d->mLyrics != other.d->mLyrics ||
            d->mComment != other.d->mComment ||
            d->mYear != other.d->mYear || d->mChannels != other.d->mChannels ||
            d->mBitRate != other.d->mBitRate || d->mSampleRate != other.d->mSampleRate;
}

MusicAudioTrack MusicAudioTrack::trackFromData(const DataTypes::TrackDataType &data)
{
    auto result = MusicAudioTrack{};

    result.setDatabaseId(data.databaseId());
    result.setTitle(data.title());
    result.setArtist(data.artist());
    result.setAlbumName(data.album());
    result.setAlbumArtist(data.albumArtist());
    result.setGenre(data.genre());
    result.setComposer(data.composer());
    result.setLyricist(data.lyricist());
    result.setLyrics(data.lyrics());
    result.setComment(data.comment());
    result.setAlbumCover(data.albumCover());
    result.setTrackNumber(data.trackNumber());
    result.setDiscNumber(data.discNumber());
    result.setYear(data.year());
    result.setChannels(data.channels());
    result.setBitRate(data.bitRate());
    result.setSampleRate(data.sampleRate());
    result.setResourceURI(data.resourceURI());
    result.setRating(data.rating());
    result.setDuration(data.duration());
    result.setFileModificationTime(data.fileModificationTime());
    result.setValid(data.isValid());

    return result;
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

void MusicAudioTrack::setId(const QString &value)
{
    d->mId = value;
}

QString MusicAudioTrack::id() const
{
    return d->mId;
}

void MusicAudioTrack::setParentId(const QString &value)
{
    d->mParentId = value;
}

QString MusicAudioTrack::parentId() const
{
    return d->mParentId;
}

void MusicAudioTrack::setTitle(const QString &value)
{
    d->mTitle = value;
}

QString MusicAudioTrack::title() const
{
    return d->mTitle;
}

void MusicAudioTrack::setArtist(const QString &value)
{
    d->mArtist = value;
}

QString MusicAudioTrack::artist() const
{
    return d->mArtist;
}

void MusicAudioTrack::setAlbumName(const QString &value)
{
    d->mAlbumName = value;
}

QString MusicAudioTrack::albumName() const
{
    return d->mAlbumName;
}

void MusicAudioTrack::setAlbumArtist(const QString &value)
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

void MusicAudioTrack::setAlbumCover(const QUrl &value)
{
    d->mAlbumCover = value;
}

QUrl MusicAudioTrack::albumCover() const
{
    if (d->mAlbumCover.isValid() || !hasEmbeddedCover()) {
        return d->mAlbumCover;
    } else {
        return QUrl(QStringLiteral("image://cover/") + d->mResourceURI.toLocalFile());
    }
}

void MusicAudioTrack::setGenre(const QString &value)
{
    d->mGenre = value;
}

QString MusicAudioTrack::genre() const
{
    return d->mGenre;
}

void MusicAudioTrack::setComposer(const QString &value)
{
    d->mComposer = value;
}

QString MusicAudioTrack::composer() const
{
    return d->mComposer;
}

void MusicAudioTrack::setLyricist(const QString &value)
{
    d->mLyricist = value;
}

QString MusicAudioTrack::lyricist() const
{
    return d->mLyricist;
}

void MusicAudioTrack::setLyrics(const QString &value)
{
    d->mLyrics = value;
}

QString MusicAudioTrack::lyrics() const
{
    return d->mLyrics;
}

void MusicAudioTrack::setComment(const QString &value)
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
    d->mTrackNumberIsValid = true;
}

int MusicAudioTrack::trackNumber() const
{
    return d->mTrackNumber;
}

bool MusicAudioTrack::trackNumberIsValid() const
{
    return d->mTrackNumberIsValid;
}

void MusicAudioTrack::setDiscNumber(int value)
{
    d->mDiscNumber = value;
    d->mDiscNumberIsValid = true;
}

int MusicAudioTrack::discNumber() const
{
    return d->mDiscNumber;
}

bool MusicAudioTrack::discNumberIsValid() const
{
    return d->mDiscNumberIsValid;
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
    d->mChannelsIsValid = true;
}

int MusicAudioTrack::channels() const
{
    return d->mChannels;
}

bool MusicAudioTrack::channelsIsValid() const
{
    return d->mChannelsIsValid;
}

void MusicAudioTrack::setBitRate(int value)
{
    d->mBitRate = value;
    d->mBitRateIsValid = true;
}

int MusicAudioTrack::bitRate() const
{
    return d->mBitRate;
}

bool MusicAudioTrack::bitRateIsValid() const
{
    return d->mBitRateIsValid;
}

void MusicAudioTrack::setSampleRate(int value)
{
    d->mSampleRate = value;
    d->mSampleRateIsValid = true;
}

int MusicAudioTrack::sampleRate() const
{
    return d->mSampleRate;
}

bool MusicAudioTrack::sampleRateIsValid() const
{
    return d->mSampleRateIsValid;
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

void MusicAudioTrack::setRating(int value)
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

void MusicAudioTrack::setHasEmbeddedCover(bool value)
{
    d->mHasBooleanCover = value;
}

bool MusicAudioTrack::hasEmbeddedCover() const
{
    return d->mHasBooleanCover;
}

MusicAudioTrack::TrackDataType MusicAudioTrack::toTrackData() const
{
    auto result = MusicAudioTrack::TrackDataType{};

    result[MusicAudioTrack::TrackDataType::key_type::DatabaseIdRole] = databaseId();

    if (!title().isEmpty()) {
        result[MusicAudioTrack::TrackDataType::key_type::TitleRole] = title();
    }

    if (!artist().isEmpty()) {
        result[MusicAudioTrack::TrackDataType::key_type::ArtistRole] = artist();
    }

    if (!albumName().isEmpty()) {
        result[MusicAudioTrack::TrackDataType::key_type::AlbumRole] = albumName();
    }

    if (!albumArtist().isEmpty()) {
        result[MusicAudioTrack::TrackDataType::key_type::AlbumArtistRole] = albumArtist();
    }

    if (!genre().isEmpty()) {
        result[MusicAudioTrack::TrackDataType::key_type::GenreRole] = genre();
    }

    if (!composer().isEmpty()) {
        result[MusicAudioTrack::TrackDataType::key_type::ComposerRole] = composer();
    }

    if (!lyricist().isEmpty()) {
        result[MusicAudioTrack::TrackDataType::key_type::LyricistRole] = lyricist();
    }

    if (!comment().isEmpty()) {
        result[MusicAudioTrack::TrackDataType::key_type::CommentRole] = comment();
    }

    if (!albumCover().isEmpty()) {
        result[MusicAudioTrack::TrackDataType::key_type::ImageUrlRole] = albumCover();
    }

    if (trackNumber() != -1) {
        result[MusicAudioTrack::TrackDataType::key_type::TrackNumberRole] = trackNumber();
    }

    if (discNumber()) {
        result[MusicAudioTrack::TrackDataType::key_type::DiscNumberRole] = discNumber();
    }

    if (year()) {
        result[MusicAudioTrack::TrackDataType::key_type::YearRole] = year();
    }

    if (channels()) {
        result[MusicAudioTrack::TrackDataType::key_type::ChannelsRole] = channels();
    }

    if (bitRate()) {
        result[MusicAudioTrack::TrackDataType::key_type::BitRateRole] = bitRate();
    }

    if (sampleRate()) {
        result[MusicAudioTrack::TrackDataType::key_type::SampleRateRole] = sampleRate();
    }

    if (!resourceURI().isEmpty()) {
        result[MusicAudioTrack::TrackDataType::key_type::ResourceRole] = resourceURI();
    }

    if (rating()) {
        result[MusicAudioTrack::TrackDataType::key_type::RatingRole] = rating();
    }

    if (duration().isValid()) {
        result[MusicAudioTrack::TrackDataType::key_type::DurationRole] = duration();
    }

    return result;
}

ELISALIB_EXPORT QDebug operator<<(QDebug stream, const MusicAudioTrack &data)
{
    stream << data.title() << data.artist() << data.albumName() << data.albumArtist() << data.duration() << data.resourceURI();
    return stream;
}
