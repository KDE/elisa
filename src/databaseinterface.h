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

#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H

#include "musicalbum.h"
#include "musicaudiotrack.h"
#include "musicartist.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QVector>
#include <QtCore/QVariant>

class DatabaseInterfacePrivate;
class QMutex;

class DatabaseInterface : public QObject
{
    Q_OBJECT

public:

    enum class AlbumData {
        Title,
        Artist,
        Image,
        TracksCount,
        Id,
    };

    explicit DatabaseInterface(QObject *parent = 0);

    virtual ~DatabaseInterface();

    void setMutex(QMutex *value);

    Q_INVOKABLE void init(const QString &dbName);

    MusicAlbum albumFromTitleAndAuthor(QString title, QString author) const;

    QVector<MusicAlbum> allAlbums(QString filter) const;

    QVector<MusicAlbum> allAlbumsFromArtist(QString artistName) const;

    QVector<MusicArtist> allArtists() const;

    QVector<MusicAudioTrack> tracksFromAuthor(QString artistName) const;

    MusicAudioTrack trackFromDatabaseId(qulonglong id) const;

    MusicAlbum albumFromId(qulonglong albumId) const;

    MusicArtist artistFromId(qulonglong artistId) const;

    qulonglong trackIdFromTitleAlbumArtist(QString title, QString album, QString artist) const;

Q_SIGNALS:

    void artistAdded(qulonglong newArtistId);

    void albumAdded(qulonglong newAlbumId);

    void trackAdded(qulonglong newTrackId);

    void requestsInitDone();

public Q_SLOTS:

    void insertTracksList(QHash<QString, QVector<MusicAudioTrack> > tracks, QHash<QString, QUrl> covers);

    void databaseArtistAdded(qulonglong newArtistId);

    void databaseAlbumAdded(qulonglong newAlbumId);

    void databaseTrackAdded(qulonglong newTrackId);

private:

    bool startTransaction() const;

    bool finishTransaction() const;

    bool rollBackTransaction() const;

    QMap<qulonglong, MusicAudioTrack> fetchTracks(qulonglong albumId) const;

    void updateTracksCount(qulonglong albumId, int tracksCount) const;

    MusicAlbum internalAlbumFromId(qulonglong albumId) const;

    QVariant internalAlbumDataFromId(qulonglong albumId, AlbumData dataType) const;

    void initDatabase() const;

    void initRequest();

    qulonglong insertArtist(QString name);

    DatabaseInterfacePrivate *d;

};

#endif // DATABASEINTERFACE_H
