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

#ifndef MUSICALBUM_H
#define MUSICALBUM_H

#include "musicaudiotrack.h"

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QMap>

class MusicAlbumPrivate;

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

    void setDatabaseId(qlonglong value);

    qlonglong databaseId() const;

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

    void setAlbumArtURI(const QUrl &value);

    QUrl albumArtURI() const;

    void setResourceURI(const QUrl &value);

    QUrl resourceURI() const;

    void setTracks(const QMap<qlonglong, MusicAudioTrack> &allTracks);

    QList<qlonglong> tracksKeys() const;

    MusicAudioTrack trackFromIndex(int index) const;

    void setTrackIds(const QList<qlonglong> &allTracksIds);

    qlonglong trackIdFromIndex(int index) const;

private:

    MusicAlbumPrivate *d;

};

#endif // MUSICALBUM_H
