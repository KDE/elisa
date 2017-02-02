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

#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H

#include "musicalbum.h"
#include "musicaudiotrack.h"
#include "musicartist.h"

#include <QObject>
#include <QString>
#include <QHash>
#include <QVector>
#include <QVariant>
#include <QUrl>

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

    Q_INVOKABLE void init(const QString &dbName);

    MusicAlbum albumFromTitleAndAuthor(QString title, QString author) const;

    QVector<MusicAlbum> allAlbums() const;

    QVector<MusicArtist> allArtists() const;

    QVector<MusicAudioTrack> tracksFromAuthor(QString artistName) const;

    MusicAudioTrack trackFromDatabaseId(qulonglong id) const;

    qulonglong trackIdFromTitleAlbumArtist(QString title, QString album, QString artist) const;

Q_SIGNALS:

    void artistAdded(MusicArtist newArtist);

    void albumAdded(MusicAlbum newAlbum);

    void trackAdded(MusicAudioTrack newTrack);

    void artistRemoved(MusicArtist newArtist);

    void albumRemoved(MusicAlbum newAlbum);

    void trackRemoved(MusicAudioTrack newTrack);

    void artistModified(MusicArtist newArtist);

    void albumModified(MusicAlbum newAlbum);

    void trackModified(MusicAudioTrack newTrack);

    void requestsInitDone();

public Q_SLOTS:

    void insertTracksList(QHash<QString, QVector<MusicAudioTrack> > tracks, QHash<QString, QUrl> covers);

    void removeTracksList(const QList<QUrl> removedTracks);

    void databaseArtistAdded(MusicArtist newArtist);

    void databaseAlbumAdded(MusicAlbum newAlbum);

    void databaseTrackAdded(MusicAudioTrack newTrack);

private:

    bool startTransaction() const;

    bool finishTransaction() const;

    bool rollBackTransaction() const;

    QMap<qulonglong, MusicAudioTrack> fetchTracks(qulonglong albumId) const;

    void updateTracksCount(qulonglong albumId, int tracksCount) const;

    MusicArtist internalArtistFromId(qulonglong artistId) const;

    MusicAlbum internalAlbumFromId(qulonglong albumId) const;

    MusicAudioTrack internalTrackFromDatabaseId(qulonglong id) const;

    QVariant internalAlbumDataFromId(qulonglong albumId, AlbumData dataType) const;

    void initDatabase() const;

    void initRequest();

    qulonglong insertArtist(QString name);

    DatabaseInterfacePrivate *d;

};

#endif // DATABASEINTERFACE_H
