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


#ifndef TRACKDATAHELPER_H
#define TRACKDATAHELPER_H

#include <QObject>
#include "musicaudiotrack.h"

class TrackDataHelper : public QObject, public MusicAudioTrack
{
    Q_OBJECT

    Q_PROPERTY(MusicAudioTrack trackData
               READ trackData
               WRITE setTrackData
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString title
               READ title
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString artist
               READ artist
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString albumName
               READ albumName
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString albumArtist
               READ albumArtist
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString genre
               READ genre
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString composer
               READ composer
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString lyricist
               READ lyricist
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString comment
               READ comment
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString year
               READ year
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString trackNumber
               READ trackNumber
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString discNumber
               READ discNumber
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString channels
               READ channels
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString bitRate
               READ bitRate
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString sampleRate
               READ sampleRate
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString resourceURI
               READ resourceURI
               NOTIFY trackDataChanged)

    Q_PROPERTY(QString duration
               READ duration
               NOTIFY trackDataChanged)

    Q_PROPERTY(int rating
               READ rating
               NOTIFY trackDataChanged)

    Q_PROPERTY(QUrl albumCover
               READ albumCover
               NOTIFY trackDataChanged)

    Q_PROPERTY(qulonglong databaseId
               READ databaseId
               NOTIFY trackDataChanged)

public:

    explicit TrackDataHelper(QObject *parent = nullptr);

    ~TrackDataHelper() override;

    QString trackNumber() const;

    QString discNumber() const;

    QString channels() const;

    QString bitRate() const;

    QString sampleRate() const;

    QString year() const;

    QString duration() const;

    QString resourceURI() const;

    const MusicAudioTrack& trackData() const;

    void setTrackData(const MusicAudioTrack &track);

Q_SIGNALS:

    void trackDataChanged();

public:

    Q_INVOKABLE bool hasValidTitle() const;

    Q_INVOKABLE bool hasValidArtist() const;

    Q_INVOKABLE bool hasValidAlbumName() const;

    Q_INVOKABLE bool hasValidAlbumArtist() const;

    Q_INVOKABLE bool hasValidGenre() const;

    Q_INVOKABLE bool hasValidComposer() const;

    Q_INVOKABLE bool hasValidLyricist() const;

    Q_INVOKABLE bool hasValidComment() const;

    Q_INVOKABLE bool hasValidTrackNumber() const;

    Q_INVOKABLE bool hasValidDiscNumber() const;

    Q_INVOKABLE bool hasValidChannels() const;

    Q_INVOKABLE bool hasValidRating() const;

    Q_INVOKABLE bool hasValidBitRate() const;

    Q_INVOKABLE bool hasValidSampleRate() const;

    Q_INVOKABLE bool hasValidYear() const;

    Q_INVOKABLE bool hasValidAlbumCover() const;
};


#endif // TRACKDATAHELPER_H

