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

    enum class TrackData {
        Title,
        Artist,
        Album,
        Image,
        Duration,
        MilliSecondsDuration,
        TrackNumber,
        Resource,
    };

    explicit DatabaseInterface(QObject *parent = 0);

    virtual ~DatabaseInterface();

    Q_INVOKABLE void init(const QString &dbName);

    MusicAlbum albumFromTitleAndAuthor(QString title, QString author) const;

    QVector<MusicAlbum> allAlbums(QString filter) const;

    QVector<MusicAlbum> allAlbumsFromArtist(QString artistName) const;

    QVector<MusicArtist> allArtists(QString filter) const;

    QVariant trackDataFromDatabaseId(qulonglong id, TrackData dataType) const;

    qulonglong trackIdFromTitleAlbumArtist(QString title, QString album, QString artist) const;

    int albumCount(QString artist) const;

Q_SIGNALS:

    void databaseChanged(QVector<qulonglong> newAlbums, QVector<qulonglong> newTracks);

    void beginAlbumAdded(QVector<qulonglong> newAlbums);

    void endAlbumAdded(QVector<qulonglong> newAlbums);

    void beginTrackAdded(QVector<qulonglong> newTracks);

    void endTrackAdded(QVector<qulonglong> newTracks);

    void requestsInitDone();

public Q_SLOTS:

    void insertTracksList(QHash<QString, QVector<MusicAudioTrack> > tracks, QHash<QString, QUrl> covers);

    void databaseHasChanged(QVector<qulonglong> newAlbums, QVector<qulonglong> newTracks);

private:

    bool startTransaction() const;

    bool finishTransaction() const;

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
