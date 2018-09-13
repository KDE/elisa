/*
 * Copyright 2018 Alexander Stippich <a.stippich@gmx.net>
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


#ifndef TRACKDATAHELPER_H
#define TRACKDATAHELPER_H

#include "elisaLib_export.h"

#include <QObject>
#include "musicaudiotrack.h"

class ELISALIB_EXPORT TrackDataHelper : public QObject, public MusicAudioTrack
{
    Q_OBJECT

    Q_PROPERTY(MusicAudioTrack trackData
               READ trackData
               WRITE setTrackData
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString title
               READ title
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidTitle
               READ hasValidTitle
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString artist
               READ artist
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidArtist
               READ hasValidArtist
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString albumName
               READ albumName
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidAlbumName
               READ hasValidAlbumName
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString albumArtist
               READ albumArtist
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidAlbumArtist
               READ hasValidAlbumArtist
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString genre
               READ genre
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidGenre
               READ hasValidGenre
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString composer
               READ composer
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidComposer
               READ hasValidComposer
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString lyricist
               READ lyricist
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidLyricist
               READ hasValidLyricist
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString comment
               READ comment
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidComment
               READ hasValidComment
               NOTIFY trackDataChanged)

    Q_PROPERTY(int year
               READ year
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidYear
               READ hasValidYear
               NOTIFY trackDataChanged)

    Q_PROPERTY(int trackNumber
               READ trackNumber
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidTrackNumber
               READ hasValidTrackNumber
               NOTIFY trackDataChanged)

    Q_PROPERTY(int discNumber
               READ discNumber
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidDiscNumber
               READ hasValidDiscNumber
               NOTIFY trackDataChanged)

    Q_PROPERTY(int channels
               READ channels
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidChannels
               READ hasValidChannels
               NOTIFY trackDataChanged)

    Q_PROPERTY(int bitRate
               READ bitRate
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidBitRate
               READ hasValidBitRate
               NOTIFY trackDataChanged)

    Q_PROPERTY(int sampleRate
               READ sampleRate
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidSampleRate
               READ hasValidSampleRate
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString resourceURI
               READ resourceURI
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString fileName
               READ fileName
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString duration
               READ duration
               NOTIFY trackDataChanged)

    Q_PROPERTY(int rating
               READ rating
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidRating
               READ hasValidRating
               NOTIFY trackDataChanged)

    Q_PROPERTY(QUrl albumCover
               READ albumCover
               NOTIFY trackDataChanged)

    Q_PROPERTY(bool hasValidAlbumCover
               READ hasValidAlbumCover
               NOTIFY trackDataChanged)

    Q_PROPERTY(qulonglong databaseId
               READ databaseId
               NOTIFY trackDataChanged)

public:

    explicit TrackDataHelper(QObject *parent = nullptr);

    ~TrackDataHelper() override;

    QString title() const;

    QString duration() const;

    QString resourceURI() const;

    QString fileName() const;

    const MusicAudioTrack& trackData() const;

    void setTrackData(const MusicAudioTrack &track);

    bool hasValidTitle() const;

    bool hasValidArtist() const;

    bool hasValidAlbumName() const;

    bool hasValidAlbumArtist() const;

    bool hasValidGenre() const;

    bool hasValidComposer() const;

    bool hasValidLyricist() const;

    bool hasValidComment() const;

    bool hasValidTrackNumber() const;

    bool hasValidDiscNumber() const;

    bool hasValidChannels() const;

    bool hasValidRating() const;

    bool hasValidBitRate() const;

    bool hasValidSampleRate() const;

    bool hasValidYear() const;

    bool hasValidAlbumCover() const;

Q_SIGNALS:

    void trackDataChanged();

public:

};


#endif // TRACKDATAHELPER_H

