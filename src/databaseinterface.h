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
#include <QList>
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

    Q_INVOKABLE void init(const QString &dbName, const QString &databaseFileName = {});

    MusicAlbum albumFromTitle(QString title);

    QList<MusicAudioTrack> allTracks() const;

    QList<MusicAudioTrack> allTracksFromSource(QString musicSource) const;

    QList<MusicAudioTrack> allInvalidTracksFromSource(QString musicSource) const;

    QList<MusicAlbum> allAlbums();

    QList<MusicArtist> allArtists() const;

    QList<MusicAudioTrack> tracksFromAuthor(QString artistName) const;

    MusicAudioTrack trackFromDatabaseId(qulonglong id);

    qulonglong trackIdFromTitleAlbumArtist(QString title, QString album, QString artist) const;

Q_SIGNALS:

    void artistAdded(MusicArtist newArtist);

    void albumAdded(MusicAlbum newAlbum);

    void trackAdded(qulonglong id);

    void artistRemoved(MusicArtist removedArtist);

    void albumRemoved(MusicAlbum removedAlbum);

    void trackRemoved(qulonglong id);

    void artistModified(MusicArtist modifiedArtist);

    void albumModified(MusicAlbum modifiedAlbum);

    void trackModified(qulonglong id);

    void requestsInitDone();

public Q_SLOTS:

    void insertTracksList(QList<MusicAudioTrack> tracks, const QHash<QString, QUrl> &covers, QString musicSource);

    void removeTracksList(const QList<QUrl> removedTracks);

    void modifyTracksList(const QList<MusicAudioTrack> &modifiedTracks, const QHash<QString, QUrl> &covers);

private:

    bool startTransaction() const;

    bool finishTransaction() const;

    bool rollBackTransaction() const;

    QList<MusicAudioTrack> fetchTracks(qulonglong albumId);

    bool updateTracksCount(qulonglong albumId);

    MusicArtist internalArtistFromId(qulonglong artistId) const;

    MusicAlbum internalAlbumFromId(qulonglong albumId);

    MusicAlbum internalAlbumFromTitle(QString title);

    qulonglong internalAlbumIdFromTitle(QString title);

    MusicAudioTrack internalTrackFromDatabaseId(qulonglong id);

    qulonglong internalTrackIdFromTitleAlbumArtist(QString title, QString album, QString artist) const;

    qulonglong internalTrackIdFromFileName(const QUrl fileName) const;

    QVariant internalAlbumDataFromId(qulonglong albumId, AlbumData dataType);

    QList<MusicAudioTrack> internalTracksFromAuthor(QString artistName) const;

    void initDatabase() const;

    void initRequest();

    qulonglong insertAlbum(QString title, QString albumArtist, QUrl albumArtURI, int tracksCount, bool isSingleDiscAlbum);

    void updateIsSingleDiscAlbumFromId(qulonglong albumId) const;

    qulonglong insertArtist(QString name);

    qulonglong internalArtistIdFromName(QString name);

    void removeTrackInDatabase(qulonglong trackId);

    void removeAlbumInDatabase(qulonglong albumId);

    void removeArtistInDatabase(qulonglong artistId);

    void reloadExistingDatabase();

    qulonglong insertMusicSource(QString name);

    void insertTrackOrigin(QUrl fileNameURI, qulonglong discoverId);

    void updateTrackOrigin(qulonglong trackId, QUrl fileName);

    int computeTrackPriority(qulonglong trackId, QUrl fileName);

    void internalInsertTrack(const MusicAudioTrack &oneModifiedTrack, const QHash<QString, QUrl> &covers, int originTrackId, QSet<qulonglong> &modifiedAlbumIds);

    DatabaseInterfacePrivate *d;

};

#endif // DATABASEINTERFACE_H
