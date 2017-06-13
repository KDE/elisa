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

#ifndef MUSICAUDIOTRACK_H
#define MUSICAUDIOTRACK_H

#include <QString>
#include <QTime>
#include <QUrl>
#include <QMetaType>

class MusicAudioTrackPrivate;
class QDebug;

class MusicAudioTrack
{

public:

    MusicAudioTrack();

    MusicAudioTrack(bool aValid, const QString &aId, const QString &aParentId, const QString &aTitle, const QString &aArtist, const QString &aAlbumName,
                    const QString &aAlbumArtist, int aTrackNumber, QTime aDuration, const QUrl &aResourceURI, const QUrl &aAlbumCover, int rating);

    MusicAudioTrack(bool aValid, const QString &aId, const QString &aParentId, const QString &aTitle, const QString &aArtist, const QString &aAlbumName,
                    const QString &aAlbumArtist, int aTrackNumber, int aDiscNumber, QTime aDuration, const QUrl &aResourceURI,
                    const QUrl &aAlbumCover, int rating);

    MusicAudioTrack(MusicAudioTrack &&other);

    MusicAudioTrack(const MusicAudioTrack &other);

    ~MusicAudioTrack();

    MusicAudioTrack& operator=(MusicAudioTrack &&other);

    MusicAudioTrack& operator=(const MusicAudioTrack &other);

    bool operator <(const MusicAudioTrack &other) const;

    bool operator ==(const MusicAudioTrack &other) const;

    bool operator !=(const MusicAudioTrack &other) const;

    void setValid(bool value);

    bool isValid() const;

    void setDatabaseId(qulonglong value);

    qulonglong databaseId() const;

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

    void setAlbumArtist(const QString &value) const;

    QString albumArtist() const;

    bool isValidAlbumArtist() const;

    void setAlbumCover(const QUrl &value) const;

    QUrl albumCover() const;

    void setTrackNumber(int value);

    int trackNumber() const;

    void setDiscNumber(int value);

    int discNumber() const;

    void setDuration(QTime value);

    QTime duration() const;

    void setResourceURI(const QUrl &value);

    const QUrl& resourceURI() const;

    void setRating(int value) const;

    int rating() const;

private:

    MusicAudioTrackPrivate *d = nullptr;

};

QDebug& operator<<(QDebug &stream, const MusicAudioTrack &data);

Q_DECLARE_METATYPE(MusicAudioTrack)

#endif // MUSICAUDIOTRACK_H
