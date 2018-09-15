/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H

#include "elisaLib_export.h"

#include "datatype.h"
#include "elisautils.h"
#include "musicalbum.h"
#include "musicaudiotrack.h"
#include "musicartist.h"
#include "musicaudiogenre.h"

#include <QObject>
#include <QString>
#include <QHash>
#include <QList>
#include <QVariant>
#include <QUrl>
#include <QDateTime>
#include <QPair>

#include <memory>

class DatabaseInterfacePrivate;
class QMutex;
class QSqlRecord;
class QSqlQuery;

class ELISALIB_EXPORT DatabaseInterface : public QObject
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

    Q_ENUM(AlbumData)

    enum PropertyType {
        DatabaseId,
        DisplayRole,
        SecondaryRole,
    };

    Q_ENUM(PropertyType)

    enum AlbumDiscsCount {
        SingleDiscAlbum,
        MultipleDiscsAlbum,
    };

    Q_ENUM(AlbumDiscsCount)

    explicit DatabaseInterface(QObject *parent = nullptr);

    ~DatabaseInterface() override;

    Q_INVOKABLE void init(const QString &dbName, const QString &databaseFileName = {});

    MusicAlbum albumFromTitleAndArtist(const QString &title, const QString &artist);

    QList<QMap<PropertyType, QVariant>> allData(DataUtils::DataType aType);

    QList<MusicAudioTrack> allTracks();

    QList<MusicAudioTrack> allTracksFromSource(const QString &musicSource);

    QList<MusicAlbum> allAlbums();

    QList<MusicArtist> allArtists();

    QList<MusicAudioGenre> allGenres();

    QList<MusicArtist> allComposers();

    QList<MusicArtist> allLyricists();

    QList<MusicAudioTrack> tracksFromAuthor(const QString &artistName);

    MusicAudioTrack trackFromDatabaseId(qulonglong id);

    qulonglong trackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &artist, const QString &album,
                                                    int trackNumber, int discNumber);

    qulonglong trackIdFromFileName(const QUrl &fileName);

    void applicationAboutToQuit();

Q_SIGNALS:

    void artistsAdded(const QList<MusicArtist> &newArtist);

    void composersAdded(const QList<MusicArtist> &newComposer);

    void lyricistsAdded(const QList<MusicArtist> &newLyricist);

    void albumsAdded(const QList<MusicAlbum> &newAlbum);

    void tracksAdded(const QList<MusicAudioTrack> &allTracks);

    void genresAdded(const QList<MusicAudioGenre> &allGenres);

    void artistRemoved(const MusicArtist &removedArtist);

    void albumRemoved(const MusicAlbum &removedAlbum, qulonglong removedAlbumId);

    void trackRemoved(qulonglong id);

    void artistModified(const MusicArtist &modifiedArtist);

    void albumModified(const MusicAlbum &modifiedAlbum, qulonglong modifiedAlbumId);

    void trackModified(const MusicAudioTrack &modifiedTrack);

    void sentAlbumData(const MusicAlbum albumData);

    void requestsInitDone();

    void databaseError();

    void restoredTracks(const QString &musicSource, QHash<QUrl, QDateTime> allFiles);

public Q_SLOTS:

    void insertTracksList(const QList<MusicAudioTrack> &tracks, const QHash<QString, QUrl> &covers, const QString &musicSource);

    void removeTracksList(const QList<QUrl> &removedTracks);

    void modifyTracksList(const QList<MusicAudioTrack> &modifiedTracks, const QHash<QString, QUrl> &covers, const QString &musicSource);

    void removeAllTracksFromSource(const QString &sourceName);

    void getAlbumFromAlbumId(qulonglong id);

    void askRestoredTracks(const QString &musicSource);

private:

    enum class TrackFileInsertType {
        NewTrackFileInsert,
        ModifiedTrackFileInsert,
    };

    bool startTransaction() const;

    bool finishTransaction() const;

    bool rollBackTransaction() const;

    QList<MusicAudioTrack> fetchTracks(qulonglong albumId);

    QList<qulonglong> fetchTrackIds(qulonglong albumId);

    bool updateTracksCount(qulonglong albumId);

    MusicArtist internalArtistFromId(qulonglong artistId);

    MusicAlbum internalAlbumFromId(qulonglong albumId);

    MusicAlbum internalAlbumFromTitleAndArtist(const QString &title, const QString &artist);

    qulonglong internalAlbumIdFromTitleAndArtist(const QString &title, const QString &artist);

    MusicAudioTrack internalTrackFromDatabaseId(qulonglong id);

    qulonglong internalTrackIdFromTitleAlbumTracDiscNumber(const QString &title, const QString &artist, const QString &album,
                                                           int trackNumber, int discNumber);

    qulonglong getDuplicateTrackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &album,
                                                                const QString &albumArtist, const QString &trackPath,
                                                                int trackNumber, int discNumber);

    qulonglong internalTrackIdFromFileName(const QUrl &fileName);

    QList<MusicAudioTrack> internalTracksFromAuthor(const QString &artistName);

    QList<qulonglong> internalAlbumIdsFromAuthor(const QString &artistName);

    void initDatabase();

    void initRequest();

    qulonglong insertAlbum(const QString &title, const QString &albumArtist, const QString &trackArtist,
                           const QString &trackPath, const QUrl &albumArtURI, int tracksCount,
                           AlbumDiscsCount isSingleDiscAlbum, QList<qulonglong> &newAlbumIds,
                           QList<qulonglong> &newArtistsIds);

    bool updateAlbumFromId(qulonglong albumId, const QUrl &albumArtUri, const MusicAudioTrack &currentTrack,
                           QList<qulonglong> &newArtistsIds);

    qulonglong insertArtist(const QString &name, QList<qulonglong> &newArtistsIds);

    qulonglong internalArtistIdFromName(const QString &name);

    qulonglong insertGenre(const QString &name);

    MusicAudioGenre internalGenreFromId(qulonglong genreId);

    void removeTrackInDatabase(qulonglong trackId);

    void updateTrackInDatabase(const MusicAudioTrack &oneTrack, qulonglong albumId,
                               QList<qulonglong> &newArtistsIds);

    void removeAlbumInDatabase(qulonglong albumId);

    void removeArtistInDatabase(qulonglong artistId);

    void reloadExistingDatabase();

    qulonglong insertMusicSource(const QString &name);

    void insertTrackOrigin(const QUrl &fileNameURI, const QDateTime &fileModifiedTime, qulonglong discoverId);

    void updateTrackOrigin(qulonglong trackId, const QUrl &fileName, const QDateTime &fileModifiedTime);

    int computeTrackPriority(qulonglong trackId, const QUrl &fileName);

    qulonglong internalInsertTrack(const MusicAudioTrack &oneModifiedTrack, const QHash<QString, QUrl> &covers,
                                   qulonglong originTrackId, QSet<qulonglong> &modifiedAlbumIds, TrackFileInsertType insertType,
                                   QList<qulonglong> &newAlbumIds, QList<qulonglong> &newArtistsIds, QSet<qulonglong> &modifiedTrackIds);

    MusicAudioTrack buildTrackFromDatabaseRecord(const QSqlRecord &trackRecord) const;

    void internalRemoveTracksList(const QList<QUrl> &removedTracks, QList<qulonglong> &newArtistsIds);

    void internalRemoveTracksList(const QHash<QUrl, QDateTime> &removedTracks, qulonglong sourceId, QList<qulonglong> &newArtistsIds);

    void internalRemoveTracksWithoutMapping(QList<qulonglong> &newArtistsIds);

    QUrl internalAlbumArtUriFromAlbumId(qulonglong albumId);

    bool isValidArtist(qulonglong albumId);

    qulonglong insertComposer(const QString &name);

    MusicArtist internalComposerFromId(qulonglong composerId);

    qulonglong insertLyricist(const QString &name);

    MusicArtist internalLyricistFromId(qulonglong lyricistId);

    qulonglong internalSourceIdFromName(const QString &sourceName);

    QHash<QUrl, QDateTime> internalAllFileNameFromSource(qulonglong sourceId);

    QList<QMap<PropertyType, QVariant>> internalAllGenericPartialData(QSqlQuery &query, int nbFields);

    QList<QMap<PropertyType, QVariant>> internalAllArtistsPartialData();

    QList<QMap<PropertyType, QVariant>> internalAllAlbumsPartialData();

    QList<QMap<PropertyType, QVariant>> internalAllTracksPartialData();

    QList<QMap<PropertyType, QVariant>> internalAllGenresPartialData();

    QList<QMap<PropertyType, QVariant>> internalAllComposersPartialData();

    QList<QMap<PropertyType, QVariant>> internalAllLyricistsPartialData();

    QList<MusicArtist> internalAllPeople(QSqlQuery allPeopleQuery,
                                         QSqlQuery selectCountAlbumsForPeopleQuery);

    std::unique_ptr<DatabaseInterfacePrivate> d;

};



#endif // DATABASEINTERFACE_H
