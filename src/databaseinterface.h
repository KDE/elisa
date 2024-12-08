/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "datatypes.h"

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QHash>
#include <QList>
#include <QUrl>
#include <QDateTime>

#include <memory>
#include <optional>

class DatabaseInterfacePrivate;
class QSqlRecord;
class QSqlQuery;

class ELISALIB_EXPORT DatabaseInterface : public QObject
{
    Q_OBJECT

    QML_ELEMENT

    QML_UNCREATABLE("")

public:

    enum DatabaseVersion {
        V9 = 9,
        V11 = 11,
        V12 = 12,
        V13 = 13,
        V14 = 14,
        V15 = 15,
        V16 = 16,
        V17 = 17,
    };

    explicit DatabaseInterface(QObject *parent = nullptr);

    ~DatabaseInterface() override;

    Q_INVOKABLE void init(const QString &dbName, const QString &databaseFileName = {});

    qulonglong albumIdFromTitleAndArtist(const QString &title, const QString &artist, const QString &albumPath);

    DataTypes::ListTrackDataType allTracksData();

    DataTypes::ListRadioDataType allRadiosData();

    DataTypes::ListTrackDataType recentlyPlayedTracksData(int count);

    DataTypes::ListTrackDataType frequentlyPlayedTracksData(int count);

    DataTypes::ListAlbumDataType allAlbumsData();

    DataTypes::ListAlbumDataType allAlbumsDataByGenreAndArtist(const QString &genre, const QString &artist);

    DataTypes::ListAlbumDataType allAlbumsDataByArtist(const QString &artist);

    DataTypes::AlbumDataType albumDataFromDatabaseId(qulonglong id);

    DataTypes::ListTrackDataType albumData(qulonglong databaseId);

    DataTypes::ListArtistDataType allArtistsData();

    DataTypes::ListArtistDataType allArtistsDataByGenre(const QString &genre);

    DataTypes::ArtistDataType artistDataFromDatabaseId(qulonglong id);

    qulonglong artistIdFromName(const QString &name);

    DataTypes::ListGenreDataType allGenresData();

    bool internalArtistMatchGenre(qulonglong databaseId, const QString &genre);

    DataTypes::ListTrackDataType tracksDataFromAuthor(const QString &artistName);

    DataTypes::ListTrackDataType tracksDataFromGenre(const QString &genre);

    DataTypes::TrackDataType trackDataFromDatabaseId(qulonglong id);

    DataTypes::TrackDataType trackDataFromDatabaseIdAndUrl(qulonglong id, const QUrl &trackUrl);

    DataTypes::TrackDataType radioDataFromDatabaseId(qulonglong id);

    qulonglong trackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &artist, const std::optional<QString> &album, std::optional<int> trackNumber, std::optional<int> discNumber);

    qulonglong trackIdFromFileName(const QUrl &fileName);

    qulonglong radioIdFromFileName(const QUrl &fileName);

    void applicationAboutToQuit();

Q_SIGNALS:

    void tracksAdded(const DataTypes::ListTrackDataType &allTracks);
    void radioAdded(const DataTypes::TrackDataType &radio);
    void albumsAdded(const DataTypes::ListAlbumDataType &newAlbums);
    void artistsAdded(const DataTypes::ListArtistDataType &newArtists);
    void genresAdded(const DataTypes::ListGenreDataType &NewGenres);
    void composersAdded(const DataTypes::ListArtistDataType &newComposers);
    void lyricistsAdded(const DataTypes::ListArtistDataType &newLyricists);

    void trackRemoved(qulonglong removedTrackId);
    void radioRemoved(qulonglong radioId);
    void albumRemoved(qulonglong removedAlbumId);
    void artistRemoved(qulonglong removedArtistId);
    void genreRemoved(qulonglong removedGenreId);
    void composerRemoved(qulonglong removedArtistId);
    void lyricistRemoved(qulonglong removedLyricistId);

    void trackModified(const DataTypes::TrackDataType &modifiedTrack);
    void radioModified(const DataTypes::TrackDataType &radio);
    void albumModified(const DataTypes::AlbumDataType &modifiedAlbum, qulonglong modifiedAlbumId);

    void requestsInitDone();

    void databaseError();

    void restoredTracks(const QHash<QUrl, QDateTime> &allFiles);

    void cleanedDatabase();

    void finishInsertingTracksList();

    void finishRemovingTracksList();

public Q_SLOTS:

    void insertTracksList(const DataTypes::ListTrackDataType &tracks, const QHash<QString, QUrl> &covers);

    void removeTracksList(const QList<QUrl> &removedTracks);

    void askRestoredTracks();

    void trackHasStartedPlaying(const QUrl &fileName, const QDateTime &time);

    void trackHasFinishedPlaying(const QUrl &fileName, const QDateTime &time);

    void clearData();

    void removeRadio(qulonglong radioId);

private:

    enum class DatabaseState {
        GoodState,
        BadState,
    };

    /********* Init and upgrade methods *********/

    void initConnection(const QString &connectionName, const QString &databaseFileName);

    bool initDatabase();

    void createDatabaseV9();

    void upgradeDatabaseV9();

    void upgradeDatabaseV11();

    void upgradeDatabaseV12();

    void upgradeDatabaseV13();

    void upgradeDatabaseV14();

    void upgradeDatabaseV15();

    void upgradeDatabaseV16();

    void upgradeDatabaseV17();

    [[nodiscard]] DatabaseState checkDatabaseSchema() const;

    [[nodiscard]] DatabaseState checkTable(const QString &tableName, const QStringList &expectedColumns) const;

    bool resetDatabase();

    DatabaseVersion currentDatabaseVersion();

    bool upgradeDatabaseToLatestVersion();

    void dropTable(const QString &table);

    void setDatabaseVersionInTable(int version);

    void createDatabaseVersionTable();

    void initDatabaseVersionQueries();

    void callUpgradeFunctionForVersion(DatabaseVersion databaseVersion);

    /********* Data query methods *********/

    bool startTransaction();

    bool finishTransaction();

    bool rollBackTransaction();

    bool prepareQuery(QSqlQuery &query, const QString &queryText) const;

    bool execQuery(QSqlQuery &query);

    void initDataQueries();

    void initChangesTrackers();

    void emitTrackerChanges();

    void recordModifiedTrack(qulonglong trackId);

    void recordModifiedAlbum(qulonglong albumId);

    QList<qulonglong> fetchTrackIds(qulonglong albumId);

    qulonglong internalAlbumIdFromTitleAndArtist(const QString &title, const QString &artist, const QString &albumPath);

    DataTypes::TrackDataType internalTrackFromDatabaseId(qulonglong id);

    qulonglong internalTrackIdFromTitleAlbumTracDiscNumber(const QString &title, const QString &artist, const std::optional<QString> &album,
                                                           std::optional<int> trackNumber, std::optional<int> discNumber);

    qulonglong getDuplicateTrackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &trackArtist, const QString &album,
                                                                const QString &albumArtist, const QString &trackPath, int trackNumber,
                                                                int discNumber, int priority);

    qulonglong internalTrackIdFromFileName(const QUrl &fileName);

    qulonglong internalRadioIdFromHttpAddress(const QString &httpAddress);

    DataTypes::ListTrackDataType internalTracksFromAuthor(const QString &artistName);

    DataTypes::ListTrackDataType internalTracksFromGenre(const QString &genre);

    QList<qulonglong> internalAlbumIdsFromAuthor(const QString &artistName);

    qulonglong insertAlbum(const QString &title, const QString &albumArtist,
                           const QString &trackPath, const QUrl &albumArtURI);

    bool updateAlbumFromId(qulonglong albumId, const QUrl &albumArtUri,
                           const DataTypes::TrackDataType &currentTrack, const QString &albumPath);

    qulonglong insertArtist(const QString &name);
    qulonglong insertGenre(const QString &name);
    qulonglong insertComposer(const QString &name);
    qulonglong insertLyricist(const QString &name);

    /**
     * @returns the database ID, or 0 if the item could not be found
     */
    qulonglong internalGenericIdFromName(QSqlQuery &query);
    qulonglong internalArtistIdFromName(const QString &name);
    qulonglong internalGenreIdFromName(const QString &name);
    qulonglong internalComposerIdFromName(const QString &name);
    qulonglong internalLyricistIdFromName(const QString &name);

    void updateTrackInDatabase(const DataTypes::TrackDataType &oneTrack, const QString &albumPath);

    void removeTrackInDatabase(qulonglong trackId);
    void removeAlbumInDatabase(qulonglong albumId);
    void removeArtistInDatabase(qulonglong artistId);
    void removeGenreInDatabase(qulonglong genreId);
    void removeComposerInDatabase(qulonglong composerId);
    void removeLyricistInDatabase(qulonglong lyricistId);

    void reloadExistingDatabase();

    qulonglong genericInitialId(QSqlQuery &request);

    void insertTrackOrigin(const QUrl &fileNameURI, const QDateTime &fileModifiedTime, const QDateTime &importDate);

    void updateTrackOrigin(const QUrl &fileName, const QDateTime &fileModifiedTime);

    qulonglong internalInsertTrack(const DataTypes::TrackDataType &oneModifiedTrack,
                                   const QHash<QString, QUrl> &covers, bool &isInserted);

    [[nodiscard]] DataTypes::TrackDataType buildTrackDataFromDatabaseRecord(const QSqlRecord &trackRecord) const;

    [[nodiscard]] DataTypes::TrackDataType buildRadioDataFromDatabaseRecord(const QSqlRecord &trackRecord) const;

    void internalRemoveTracksList(const QList<QUrl> &removedTracks);

    void internalRemoveTracksList(const QHash<QUrl, QDateTime> &removedTracks, qulonglong sourceId);

    QUrl internalAlbumArtUriFromAlbumId(qulonglong albumId);

    bool isValidArtist(qulonglong albumId);

    QHash<QUrl, QDateTime> internalAllFileName();

    bool internalGenericPartialData(QSqlQuery &query);

    DataTypes::ListArtistDataType internalAllArtistsPartialData(QSqlQuery &artistsQuery);

    DataTypes::ListAlbumDataType internalAllAlbumsPartialData(QSqlQuery &query);

    DataTypes::ListTrackDataType internalOneAlbumData(qulonglong databaseId);

    DataTypes::AlbumDataType internalOneAlbumPartialData(qulonglong databaseId);
    DataTypes::ArtistDataType internalOneArtistPartialData(qulonglong databaseId);
    DataTypes::GenreDataType internalOneGenrePartialData(qulonglong databaseId);
    DataTypes::ArtistDataType internalOneLyricistPartialData(qulonglong databaseId);
    DataTypes::ArtistDataType internalOneComposerPartialData(qulonglong databaseId);

    DataTypes::ListTrackDataType internalAllTracksPartialData();

    DataTypes::ListRadioDataType internalAllRadiosPartialData();

    DataTypes::ListTrackDataType internalRecentlyPlayedTracksData(int count);

    DataTypes::ListTrackDataType internalFrequentlyPlayedTracksData(int count);

    DataTypes::TrackDataType internalOneTrackPartialData(qulonglong databaseId);

    DataTypes::TrackDataType internalOneTrackPartialDataByIdAndUrl(qulonglong databaseId, const QUrl &trackUrl);

    DataTypes::TrackDataType internalOneRadioPartialData(qulonglong databaseId);

    DataTypes::ListGenreDataType internalAllGenresPartialData();

    DataTypes::ListArtistDataType internalAllComposersPartialData();

    DataTypes::ListArtistDataType internalAllLyricistsPartialData();

    void updateAlbumArtist(qulonglong albumId, const QString &title, const QString &albumPath,
                           const QString &artistName);

    bool updateAlbumCover(qulonglong albumId, const QUrl &albumArtUri);

    QVariantList internalGetLatestFourCoversForArtist(const QString& artistName);

    void updateTrackStartedStatistics(const QUrl &fileName, const QDateTime &time);

    void updateTrackFinishedStatistics(const QUrl &fileName, const QDateTime &time);

    void internalInsertOneTrack(const DataTypes::TrackDataType &oneTrack, const QHash<QString, QUrl> &covers);

    void internalInsertOneRadio(const DataTypes::TrackDataType &oneTrack);

    /**
     * Execute a query that checks whether there is at least 1 matching row
     */
    bool execHasRowQuery(QSqlQuery &query);

    bool artistHasTracks(qulonglong artistId);
    bool genreHasTracks(qulonglong genreId);
    bool composerHasTracks(qulonglong composerId);
    bool lyricistHasTracks(qulonglong lyricistId);

    void pruneCollections();
    void pruneArtists();
    void pruneGenres();
    void pruneComposers();
    void pruneLyricists();

    std::unique_ptr<DatabaseInterfacePrivate> d;

};

#endif // DATABASEINTERFACE_H
