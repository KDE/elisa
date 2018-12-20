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

#ifndef MUSICAUDIOTRACK_H
#define MUSICAUDIOTRACK_H

#include "elisaLib_export.h"

#include "databaseinterface.h"

#include <QString>
#include <QTime>
#include <QUrl>
#include <QMetaType>
#include <QSharedDataPointer>

#include <memory>

class MusicAudioTrackPrivate;
class QDebug;

class ELISALIB_EXPORT MusicAudioTrack
{

public:

    using TrackDataType = DatabaseInterface::TrackDataType;

    MusicAudioTrack();

    MusicAudioTrack(bool aValid, QString aId, QString aParentId, QString aTitle, QString aArtist, QString aAlbumName,
                    QString aAlbumArtist, int aTrackNumber, int aDiscNumber, QTime aDuration, QUrl aResourceURI,
                    const QDateTime &fileModificationTime, QUrl aAlbumCover, int rating, bool aIsSingleDiscAlbum,
                    QString aGenre, QString aComposer, QString aLyricist, bool aHasEmbeddedCover);

    MusicAudioTrack(MusicAudioTrack &&other) noexcept;

    MusicAudioTrack(const MusicAudioTrack &other);

    ~MusicAudioTrack();

    MusicAudioTrack& operator=(MusicAudioTrack &&other) noexcept;

    MusicAudioTrack& operator=(const MusicAudioTrack &other);

    bool operator <(const MusicAudioTrack &other) const;

    bool operator ==(const MusicAudioTrack &other) const;

    bool operator !=(const MusicAudioTrack &other) const;

    static MusicAudioTrack trackFromData(const DatabaseInterface::TrackDataType &data);

    void setValid(bool value);

    bool isValid() const;

    void setDatabaseId(qulonglong value);

    qulonglong databaseId() const;

    void setAlbumId(qulonglong value);

    qulonglong albumId() const;

    void setId(const QString &value);

    QString id() const;

    void setParentId(const QString &value);

    QString parentId() const;

    void setTitle(const QString &value);

    QString title() const;

    void setArtist(const QString &value);

    QString artist() const;

    void setAlbumName(const QString &value);

    QString albumName() const;

    void setAlbumArtist(const QString &value);

    QString albumArtist() const;

    void setGenre(const QString &value);

    QString genre() const;

    void setComposer(const QString &value);

    QString composer() const;

    void setLyricist(const QString &value);

    QString lyricist() const;

    void setComment(const QString &value);

    QString comment() const;

    bool isValidAlbumArtist() const;

    void setAlbumCover(const QUrl &value);

    QUrl albumCover() const;

    void setTrackNumber(int value);

    int trackNumber() const;

    void setDiscNumber(int value);

    int discNumber() const;

    void setYear(int value);

    int year() const;

    void setChannels(int value);

    int channels() const;

    void setBitRate(int value);

    int bitRate() const;

    void setSampleRate(int value);

    int sampleRate() const;

    void setDuration(QTime value);

    QTime duration() const;

    void setFileModificationTime(const QDateTime &value);

    const QDateTime &fileModificationTime() const;

    void setResourceURI(const QUrl &value);

    const QUrl& resourceURI() const;

    void setRating(int value);

    int rating() const;

    void setIsSingleDiscAlbum(bool value);

    bool isSingleDiscAlbum() const;

    void setHasEmbeddedCover(bool value);

    bool hasEmbeddedCover() const;

    TrackDataType toTrackData() const;

private:

    QSharedDataPointer<MusicAudioTrackPrivate> d;

};

ELISALIB_EXPORT QDebug operator<<(QDebug stream, const MusicAudioTrack &data);

Q_DECLARE_TYPEINFO(MusicAudioTrack, Q_MOVABLE_TYPE);

Q_DECLARE_METATYPE(MusicAudioTrack)

#endif // MUSICAUDIOTRACK_H
