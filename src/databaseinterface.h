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

#include <memory>

class DatabaseInterfacePrivate;
class QMutex;
class QSqlRecord;

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

    explicit DatabaseInterface(QObject *parent = nullptr);

    ~DatabaseInterface() override;

    Q_INVOKABLE void init(const QString &dbName, const QString &databaseFileName = {});

    MusicAlbum albumFromTitleAndArtist(const QString &title, const QString &artist);

    QList<MusicAudioTrack> allTracks();

    QList<MusicAudioTrack> allTracksFromSource(const QString &musicSource);

    QList<MusicAudioTrack> allInvalidTracksFromSource(const QString &musicSource);

    QList<MusicAlbum> allAlbums();

    QList<MusicArtist> allArtists();

    QList<MusicAudioTrack> tracksFromAuthor(const QString &artistName);

    MusicAudioTrack trackFromDatabaseId(qulonglong id);

    qulonglong trackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &artist, const QString &album,
                                                    int trackNumber, int discNumber);

    qulonglong trackIdFromFileName(const QUrl &fileName);

    void applicationAboutToQuit();

Q_SIGNALS:

    void artistsAdded(const QList<MusicArtist> &newArtist);

    void albumsAdded(const QList<MusicAlbum> &newAlbum);

    void tracksAdded(const QList<MusicAudioTrack> &allTracks);

    void artistRemoved(const MusicArtist &removedArtist);

    void albumRemoved(const MusicAlbum &removedAlbum, qulonglong removedAlbumId);

    void trackRemoved(qulonglong id);

    void artistModified(const MusicArtist &modifiedArtist);

    void albumModified(const MusicAlbum &modifiedAlbum, qulonglong modifiedAlbumId);

    void trackModified(const MusicAudioTrack &modifiedTrack);

    void sentAlbumData(const MusicAlbum albumData);

    void requestsInitDone();

    void databaseError();

public Q_SLOTS:

    void insertTracksList(const QList<MusicAudioTrack> &tracks, const QHash<QString, QUrl> &covers, const QString &musicSource);

    void removeTracksList(const QList<QUrl> &removedTracks);

    void modifyTracksList(const QList<MusicAudioTrack> &modifiedTracks, const QHash<QString, QUrl> &covers, const QString &musicSource);

    void removeAllTracksFromSource(const QString &sourceName);

    void getAlbumFromAlbumId(qulonglong id);

    void cleanInvalidTracks();

private:

    enum class TrackFileInsertType {
        NewTrackFileInsert,
        ModifiedTrackFileInsert,
    };

    bool startTransaction() const;

    bool finishTransaction() const;

    bool rollBackTransaction() const;

    QList<MusicAudioTrack> fetchTracks(qulonglong albumId);

    bool updateTracksCount(qulonglong albumId);

    MusicArtist internalArtistFromId(qulonglong artistId);

    MusicAlbum internalAlbumFromId(qulonglong albumId);

    MusicAlbum internalAlbumFromTitleAndArtist(const QString &title, const QString &artist);

    qulonglong internalAlbumIdFromTitleAndArtist(const QString &title, const QString &artist);

    MusicAudioTrack internalTrackFromDatabaseId(qulonglong id);

    qulonglong internalTrackIdFromTitleAlbumTracDiscNumber(const QString &title, const QString &artist, const QString &album,
                                                           int trackNumber, int discNumber);

    qulonglong getDuplicateTrackIdFromTitleAlbumTracDiscNumber(const QString &title, const QString &album, const QString &albumArtist,
                                                               int trackNumber, int discNumber);

    qulonglong internalTrackIdFromFileName(const QUrl &fileName);

    QList<MusicAudioTrack> internalTracksFromAuthor(const QString &artistName);

    QList<qulonglong> internalAlbumIdsFromAuthor(const QString &artistName);

    void initDatabase() const;

    void initRequest();

    qulonglong insertAlbum(const QString &title, const QString &albumArtist, const QString &trackArtist,
                           const QUrl &albumArtURI, int tracksCount, bool isSingleDiscAlbum,
                           QList<qulonglong> &newAlbumIds, QList<qulonglong> &newArtistsIds);

    bool updateAlbumFromId(qulonglong albumId, const QUrl &albumArtUri, const MusicAudioTrack &currentTrack,
                           QList<qulonglong> &newArtistsIds);

    qulonglong insertArtist(const QString &name, QList<qulonglong> &newArtistsIds);

    qulonglong internalArtistIdFromName(const QString &name);

    void removeTrackInDatabase(qulonglong trackId);

    void removeAlbumInDatabase(qulonglong albumId);

    void removeArtistInDatabase(qulonglong artistId);

    void reloadExistingDatabase();

    qulonglong insertMusicSource(const QString &name);

    void insertTrackOrigin(const QUrl &fileNameURI, qulonglong discoverId);

    void updateTrackOrigin(qulonglong trackId, const QUrl &fileName);

    int computeTrackPriority(qulonglong trackId, const QUrl &fileName);

    qulonglong internalInsertTrack(const MusicAudioTrack &oneModifiedTrack, const QHash<QString, QUrl> &covers,
                                   qulonglong originTrackId, QSet<qulonglong> &modifiedAlbumIds, TrackFileInsertType insertType,
                                   QList<qulonglong> &newAlbumIds, QList<qulonglong> &newArtistsIds);

    MusicAudioTrack buildTrackFromDatabaseRecord(const QSqlRecord &trackRecord) const;

    void internalRemoveTracksList(const QList<QUrl> &removedTracks, QList<qulonglong> &newArtistsIds);

    void internalRemoveTracksList(const QList<QUrl> &removedTracks, qulonglong sourceId, QList<qulonglong> &newArtistsIds);

    void internalRemoveTracksWithoutMapping(QList<qulonglong> &newArtistsIds);

    QUrl internalAlbumArtUriFromAlbumId(qulonglong albumId);

    bool isValidArtist(qulonglong albumId);

    std::unique_ptr<DatabaseInterfacePrivate> d;

};

#endif // DATABASEINTERFACE_H
