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

#ifndef MUSICAUDIOTRACK_H
#define MUSICAUDIOTRACK_H

#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtCore/QUrl>
#include <QtCore/QMetaType>

class MusicAudioTrackPrivate;

class MusicAudioTrack
{

public:

    MusicAudioTrack();

    MusicAudioTrack(bool aValid, QString aId, QString aParentId, QString aTitle, QString aArtist, QString aAlbumName,
                    int aTrackNumber, QTime aDuration, QUrl aResourceURI);

    MusicAudioTrack(MusicAudioTrack &&other);

    MusicAudioTrack(const MusicAudioTrack &other);

    ~MusicAudioTrack();

    MusicAudioTrack& operator=(MusicAudioTrack &&other);

    MusicAudioTrack& operator=(const MusicAudioTrack &other);

    bool operator <(const MusicAudioTrack &other) const;

    void setValid(bool value);

    bool isValid() const;

    void setDatabaseId(qlonglong value);

    qlonglong databaseId() const;

    void setId(const QString &value) const;

    QString id() const;

    void setParentId(const QString &value) const;

    QString parentId() const;

    void setTitle(const QString &value) const;

    QString title() const;

    void setArtist(const QString &value) const;

    QString artist() const;

    void setAlbumName(const QString &value) const;

    QString albumName() const;

    void setTrackNumber(int value);

    int trackNumber() const;

    void setDuration(const QTime &value);

    const QTime& duration() const;

    void setResourceURI(const QUrl &value);

    const QUrl& resourceURI() const;

private:

    MusicAudioTrackPrivate *d = nullptr;

};

Q_DECLARE_METATYPE(MusicAudioTrack)

#endif // MUSICAUDIOTRACK_H
