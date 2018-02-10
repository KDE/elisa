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

#ifndef MUSICALBUM_H
#define MUSICALBUM_H

#include "musicaudiotrack.h"

#include <QString>
#include <QUrl>
#include <QMap>
#include <QStringList>
#include <QMetaType>

#include <memory>

class MusicAlbumPrivate;
class QDebug;

class MusicAlbum
{

public:

    MusicAlbum();

    MusicAlbum(MusicAlbum &&other);

    MusicAlbum(const MusicAlbum &other);

    MusicAlbum& operator=(MusicAlbum &&other);

    MusicAlbum& operator=(const MusicAlbum &other);

    ~MusicAlbum();

    void setValid(bool value);

    bool isValid() const;

    void setDatabaseId(qulonglong value);

    qulonglong databaseId() const;

    void setId(const QString &value);

    QString id() const;

    void setParentId(const QString &value);

    QString parentId() const;

    void setTracksCount(int count);

    int tracksCount() const;

    void setTitle(const QString &value);

    QString title() const;

    void setArtist(const QString &value);

    QString artist() const;

    bool isValidArtist() const;

    bool canUpdateArtist(const MusicAudioTrack &currentTrack) const;

    void setAlbumArtURI(const QUrl &value);

    QUrl albumArtURI() const;

    void setResourceURI(const QUrl &value);

    QUrl resourceURI() const;

    void setIsSingleDiscAlbum(bool value);

    bool isSingleDiscAlbum() const;

    void setTracks(const QList<MusicAudioTrack> &allTracks);

    const MusicAudioTrack &trackFromIndex(int index) const;

    qulonglong trackIdFromIndex(int index) const;

    int trackIndexFromId(qulonglong id) const;

    QStringList allArtists() const;

    QStringList allTracksTitle() const;

    bool isEmpty() const;

    void removeTrackFromIndex(int index);

    void insertTrack(const MusicAudioTrack &newTrack, int index);

    void updateTrack(const MusicAudioTrack &modifiedTrack, int index);

    int highestTrackRating() const;

private:

    std::unique_ptr<MusicAlbumPrivate> d;

};

QDebug& operator<<(QDebug &stream, const MusicAlbum &data);

bool operator==(const MusicAlbum &album1, const MusicAlbum &album2);

Q_DECLARE_METATYPE(MusicAlbum)

#endif // MUSICALBUM_H
