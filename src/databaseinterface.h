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

public:

    enum DatabaseVersion {
        V9 = 9,
        V11 = 11,
        V12 = 12,
        V13 = 13,
        V14 = 14,
        V15 = 15,
        V16 = 16, //Does not exist yet, for testing purpose only.
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

    void artistsAdded(const DataTypes::ListArtistDataType &newArtists);

    void composersAdded(const DataTypes::ListArtistDataType &newComposers);

    void lyricistsAdded(const DataTypes::ListArtistDataType &newLyricists);

    void albumsAdded(const DataTypes::ListAlbumDataType &newAlbums);

    void tracksAdded(const DataTypes::ListTrackDataType &allTracks);

    void genresAdded(const DataTypes::ListGenreDataType &allGenres);

    void artistRemoved(qulonglong removedArtistId);

    void albumRemoved(qulonglong removedAlbumId);

    void trackRemoved(qulonglong id);

    void albumModified(const DataTypes::AlbumDataType &modifiedAlbum, qulonglong modifiedAlbumId);

    void trackModified(const DataTypes::TrackDataType &modifiedTrack);

    void requestsInitDone();

    void databaseError();

    void restoredTracks(const QHash<QUrl, QDateTime> &allFiles);

    void cleanedDatabase();

    void finishInsertingTracksList();

    void finishRemovingTracksList();

    void radioAdded(const DataTypes::TrackDataType &radio);

    void radioModified(const DataTypes::TrackDataType &radio);

    void radioRemoved(qulonglong radioId);

public Q_SLOTS:

    void insertTracksList(const DataTypes::ListTrackDataType &tracks, const QHash<QString, QUrl> &covers);

    void removeTracksList(const QList<QUrl> &removedTracks);

    void askRestoredTracks();

    void trackHasStartedPlaying(const QUrl &fileName, const QDateTime &time);

    void clearData();

    void removeRadio(qulonglong radioId);

private:

    enum class TrackFileInsertType {
        NewTrackFileInsert,
        ModifiedTrackFileInsert,
    };

    void initChangesTrackers();

    void recordModifiedTrack(qulonglong trackId);

    void recordModifiedAlbum(qulonglong albumId);

    bool startTransaction();

    bool finishTransaction();

    bool rollBackTransaction();

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

    void initDatabase();

    void initRequest();

    qulonglong insertAlbum(const QString &title, const QString &albumArtist,
                           const QString &trackPath, const QUrl &albumArtURI);

    bool updateAlbumFromId(qulonglong albumId, const QUrl &albumArtUri,
                           const DataTypes::TrackDataType &currentTrack, const QString &albumPath);

    qulonglong insertArtist(const QString &name);

    qulonglong internalArtistIdFromName(const QString &name);

    qulonglong insertGenre(const QString &name);

    void removeTrackInDatabase(qulonglong trackId);

    void updateTrackInDatabase(const DataTypes::TrackDataType &oneTrack, const QString &albumPath);

    void removeAlbumInDatabase(qulonglong albumId);

    void removeArtistInDatabase(qulonglong artistId);

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

    qulonglong insertComposer(const QString &name);

    qulonglong insertLyricist(const QString &name);

    QHash<QUrl, QDateTime> internalAllFileName();

    bool internalGenericPartialData(QSqlQuery &query);

    DataTypes::ListArtistDataType internalAllArtistsPartialData(QSqlQuery &artistsQuery);

    DataTypes::ListAlbumDataType internalAllAlbumsPartialData(QSqlQuery &query);

    DataTypes::ListTrackDataType internalOneAlbumData(qulonglong databaseId);

    DataTypes::AlbumDataType internalOneAlbumPartialData(qulonglong databaseId);

    DataTypes::ArtistDataType internalOneArtistPartialData(qulonglong databaseId);

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

    bool prepareQuery(QSqlQuery &query, const QString &queryText) const;

    bool execQuery(QSqlQuery &query);

    void updateAlbumArtist(qulonglong albumId, const QString &title, const QString &albumPath,
                           const QString &artistName);

    bool updateAlbumCover(qulonglong albumId, const QUrl &albumArtUri);

    void updateTrackStatistics(const QUrl &fileName, const QDateTime &time);

    void createDatabaseV9();

    void upgradeDatabaseV9();

    void upgradeDatabaseV11();

    void upgradeDatabaseV12();

    void upgradeDatabaseV13();

    void upgradeDatabaseV14();

    void upgradeDatabaseV15();

    void upgradeDatabaseV16();

    void checkDatabaseSchema();

    void checkAlbumsTableSchema();

    void checkArtistsTableSchema();

    void checkComposerTableSchema();

    void checkGenreTableSchema();

    void checkLyricistTableSchema();

    void checkTracksTableSchema();

    void checkTracksDataTableSchema();

    void genericCheckTable(const QString &tableName, const QStringList &expectedColumns);

    void resetDatabase();

    void manageNewDatabaseVersion();

    void dropTable(const QString &table);

    void setDatabaseVersionInTable(int version);

    void createDatabaseVersionTable();

    void manageNewDatabaseVersionInitRequests();

    void callUpgradeFunctionForVersion(DatabaseVersion databaseVersion);

    void internalInsertOneTrack(const DataTypes::TrackDataType &oneTrack, const QHash<QString, QUrl> &covers);

    void internalInsertOneRadio(const DataTypes::TrackDataType &oneTrack);

    std::unique_ptr<DatabaseInterfacePrivate> d;

};

#endif // DATABASEINTERFACE_H
