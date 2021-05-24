/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
  */

#include "databaseinterface.h"

#include "databaseLogging.h"

#include <KI18n/KLocalizedString>

#include <QCoreApplication>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QDateTime>
#include <QMutex>
#include <QVariant>
#include <QAtomicInt>
#include <QElapsedTimer>
#include <QDebug>

#include <algorithm>

class DatabaseInterfacePrivate
{
public:

    enum TrackRecordColumns
    {
        TrackId,
        TrackTitle,
        TrackAlbumId,
        TrackArtistName,
        TrackArtistsCount,
        TrackAllArtists,
        TrackAlbumArtistName,
        TrackFileName,
        TrackFileModifiedTime,
        TrackNumber,
        TrackDiscNumber,
        TrackDuration,
        TrackAlbumTitle,
        TrackRating,
        TrackCoverFileName,
        TrackIsSingleDiscAlbum,
        TrackGenreName,
        TrackComposerName,
        TrackLyricistName,
        TrackComment,
        TrackYear,
        TrackChannelsCount,
        TrackBitRate,
        TrackSamplerate,
        TrackHasEmbeddedCover,
        TrackImportDate,
        TrackFirstPlayDate,
        TrackLastPlayDate,
        TrackPlayCounter,
        TrackPlayFrequency,
        TrackEmbeddedCover,
    };

    enum RadioRecordColumns
    {
        RadioId,
        RadioTitle,
        RadioHttpAddress,
        RadioImageAddress,
        RadioRating,
        RadioGenreName,
        RadioComment,
    };

    enum AlbumsRecordColumns
    {
        AlbumsId,
        AlbumsTitle,
        AlbumsSecondaryText,
        AlbumsCoverFileName,
        AlbumsArtistName,
        AlbumsYear,
        AlbumsArtistsCount,
        AlbumsAllArtists,
        AlbumsHighestRating,
        AlbumsAllGenres,
        AlbumsIsSingleDiscAlbum,
        AlbumsEmbeddedCover,
    };

    enum SingleAlbumRecordColumns
    {
        SingleAlbumId,
        SingleAlbumTitle,
        SingleAlbumArtistName,
        SingleAlbumPath,
        SingleAlbumCoverFileName,
        SingleAlbumTracksCount,
        SingleAlbumIsSingleDiscAlbum,
        SingleAlbumArtistsCount,
        SingleAlbumAllArtists,
        SingleAlbumHighestRating,
        SingleAlbumAllGenres,
        SingleAlbumEmbeddedCover,
    };

    DatabaseInterfacePrivate(const QSqlDatabase &tracksDatabase)
        : mTracksDatabase(tracksDatabase), mSelectAlbumQuery(mTracksDatabase),
          mSelectTrackQuery(mTracksDatabase), mSelectAlbumIdFromTitleQuery(mTracksDatabase),
          mInsertAlbumQuery(mTracksDatabase), mSelectTrackIdFromTitleAlbumIdArtistQuery(mTracksDatabase),
          mInsertTrackQuery(mTracksDatabase), mSelectTracksFromArtist(mTracksDatabase),
          mSelectTracksFromGenre(mTracksDatabase),
          mSelectTrackFromIdQuery(mTracksDatabase), mSelectRadioFromIdQuery(mTracksDatabase),
          mSelectCountAlbumsForArtistQuery(mTracksDatabase),
          mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery(mTracksDatabase),
          mSelectAllAlbumsFromArtistQuery(mTracksDatabase), mSelectAllArtistsQuery(mTracksDatabase),
          mInsertArtistsQuery(mTracksDatabase), mSelectArtistByNameQuery(mTracksDatabase),
          mSelectArtistQuery(mTracksDatabase), mUpdateTrackStatistics(mTracksDatabase),
          mRemoveTrackQuery(mTracksDatabase), mRemoveAlbumQuery(mTracksDatabase),
          mRemoveArtistQuery(mTracksDatabase), mSelectAllTracksQuery(mTracksDatabase),
          mSelectAllRadiosQuery(mTracksDatabase),
          mInsertTrackMapping(mTracksDatabase), mUpdateTrackFirstPlayStatistics(mTracksDatabase),
          mInsertMusicSource(mTracksDatabase), mSelectMusicSource(mTracksDatabase),
          mUpdateTrackPriority(mTracksDatabase), mUpdateTrackFileModifiedTime(mTracksDatabase),
          mSelectTracksMapping(mTracksDatabase), mSelectTracksMappingPriority(mTracksDatabase),
          mSelectRadioIdFromHttpAddress(mTracksDatabase),
          mUpdateAlbumArtUriFromAlbumIdQuery(mTracksDatabase), mSelectTracksMappingPriorityByTrackId(mTracksDatabase),
          mSelectAlbumIdsFromArtist(mTracksDatabase), mSelectAllTrackFilesQuery(mTracksDatabase),
          mRemoveTracksMappingFromSource(mTracksDatabase), mRemoveTracksMapping(mTracksDatabase),
          mSelectTracksWithoutMappingQuery(mTracksDatabase), mSelectAlbumIdFromTitleAndArtistQuery(mTracksDatabase),
          mSelectAlbumIdFromTitleWithoutArtistQuery(mTracksDatabase),
          mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery(mTracksDatabase), mSelectAlbumArtUriFromAlbumIdQuery(mTracksDatabase),
          mInsertComposerQuery(mTracksDatabase), mSelectComposerByNameQuery(mTracksDatabase),
          mSelectComposerQuery(mTracksDatabase), mInsertLyricistQuery(mTracksDatabase),
          mSelectLyricistByNameQuery(mTracksDatabase), mSelectLyricistQuery(mTracksDatabase),
          mInsertGenreQuery(mTracksDatabase), mSelectGenreByNameQuery(mTracksDatabase),
          mSelectGenreQuery(mTracksDatabase), mSelectAllTracksShortQuery(mTracksDatabase),
          mSelectAllAlbumsShortQuery(mTracksDatabase), mSelectAllComposersQuery(mTracksDatabase),
          mSelectAllLyricistsQuery(mTracksDatabase), mSelectCountAlbumsForComposerQuery(mTracksDatabase),
          mSelectCountAlbumsForLyricistQuery(mTracksDatabase), mSelectAllGenresQuery(mTracksDatabase),
          mSelectGenreForArtistQuery(mTracksDatabase), mSelectGenreForAlbumQuery(mTracksDatabase),
          mUpdateTrackQuery(mTracksDatabase), mUpdateAlbumArtistQuery(mTracksDatabase),
          mUpdateRadioQuery(mTracksDatabase),
          mUpdateAlbumArtistInTracksQuery(mTracksDatabase), mQueryMaximumTrackIdQuery(mTracksDatabase),
          mQueryMaximumAlbumIdQuery(mTracksDatabase), mQueryMaximumArtistIdQuery(mTracksDatabase),
          mQueryMaximumLyricistIdQuery(mTracksDatabase), mQueryMaximumComposerIdQuery(mTracksDatabase),
          mQueryMaximumGenreIdQuery(mTracksDatabase), mSelectAllArtistsWithGenreFilterQuery(mTracksDatabase),
          mSelectAllAlbumsShortWithGenreArtistFilterQuery(mTracksDatabase), mSelectAllAlbumsShortWithArtistFilterQuery(mTracksDatabase),
          mSelectAllRecentlyPlayedTracksQuery(mTracksDatabase), mSelectAllFrequentlyPlayedTracksQuery(mTracksDatabase),
          mClearTracksDataTable(mTracksDatabase), mClearTracksTable(mTracksDatabase),
          mClearAlbumsTable(mTracksDatabase), mClearArtistsTable(mTracksDatabase),
          mClearComposerTable(mTracksDatabase), mClearGenreTable(mTracksDatabase), mClearLyricistTable(mTracksDatabase),
          mArtistMatchGenreQuery(mTracksDatabase), mSelectTrackIdQuery(mTracksDatabase),
          mInsertRadioQuery(mTracksDatabase), mDeleteRadioQuery(mTracksDatabase),
          mSelectTrackFromIdAndUrlQuery(mTracksDatabase),
          mUpdateDatabaseVersionQuery(mTracksDatabase), mSelectDatabaseVersionQuery(mTracksDatabase)
    {
    }

    QSqlDatabase mTracksDatabase;

    QSqlQuery mSelectAlbumQuery;

    QSqlQuery mSelectTrackQuery;

    QSqlQuery mSelectAlbumIdFromTitleQuery;

    QSqlQuery mInsertAlbumQuery;

    QSqlQuery mSelectTrackIdFromTitleAlbumIdArtistQuery;

    QSqlQuery mInsertTrackQuery;

    QSqlQuery mSelectTracksFromArtist;

    QSqlQuery mSelectTracksFromGenre;

    QSqlQuery mSelectTrackFromIdQuery;

    QSqlQuery mSelectRadioFromIdQuery;

    QSqlQuery mSelectCountAlbumsForArtistQuery;

    QSqlQuery mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery;

    QSqlQuery mSelectAllAlbumsFromArtistQuery;

    QSqlQuery mSelectAllArtistsQuery;

    QSqlQuery mInsertArtistsQuery;

    QSqlQuery mSelectArtistByNameQuery;

    QSqlQuery mSelectArtistQuery;

    QSqlQuery mUpdateTrackStatistics;

    QSqlQuery mRemoveTrackQuery;

    QSqlQuery mRemoveAlbumQuery;

    QSqlQuery mRemoveArtistQuery;

    QSqlQuery mSelectAllTracksQuery;

    QSqlQuery mSelectAllRadiosQuery;

    QSqlQuery mInsertTrackMapping;

    QSqlQuery mUpdateTrackFirstPlayStatistics;

    QSqlQuery mInsertMusicSource;

    QSqlQuery mSelectMusicSource;

    QSqlQuery mUpdateTrackPriority;

    QSqlQuery mUpdateTrackFileModifiedTime;

    QSqlQuery mSelectTracksMapping;

    QSqlQuery mSelectTracksMappingPriority;

    QSqlQuery mSelectRadioIdFromHttpAddress;

    QSqlQuery mUpdateAlbumArtUriFromAlbumIdQuery;

    QSqlQuery mSelectTracksMappingPriorityByTrackId;

    QSqlQuery mSelectAlbumIdsFromArtist;

    QSqlQuery mSelectAllTrackFilesQuery;

    QSqlQuery mRemoveTracksMappingFromSource;

    QSqlQuery mRemoveTracksMapping;

    QSqlQuery mSelectTracksWithoutMappingQuery;

    QSqlQuery mSelectAlbumIdFromTitleAndArtistQuery;

    QSqlQuery mSelectAlbumIdFromTitleWithoutArtistQuery;

    QSqlQuery mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery;

    QSqlQuery mSelectAlbumArtUriFromAlbumIdQuery;

    QSqlQuery mInsertComposerQuery;

    QSqlQuery mSelectComposerByNameQuery;

    QSqlQuery mSelectComposerQuery;

    QSqlQuery mInsertLyricistQuery;

    QSqlQuery mSelectLyricistByNameQuery;

    QSqlQuery mSelectLyricistQuery;

    QSqlQuery mInsertGenreQuery;

    QSqlQuery mSelectGenreByNameQuery;

    QSqlQuery mSelectGenreQuery;

    QSqlQuery mSelectAllTracksShortQuery;

    QSqlQuery mSelectAllAlbumsShortQuery;

    QSqlQuery mSelectAllComposersQuery;

    QSqlQuery mSelectAllLyricistsQuery;

    QSqlQuery mSelectCountAlbumsForComposerQuery;

    QSqlQuery mSelectCountAlbumsForLyricistQuery;

    QSqlQuery mSelectAllGenresQuery;

    QSqlQuery mSelectGenreForArtistQuery;

    QSqlQuery mSelectGenreForAlbumQuery;

    QSqlQuery mUpdateTrackQuery;

    QSqlQuery mUpdateAlbumArtistQuery;

    QSqlQuery mUpdateRadioQuery;

    QSqlQuery mUpdateAlbumArtistInTracksQuery;

    QSqlQuery mQueryMaximumTrackIdQuery;

    QSqlQuery mQueryMaximumAlbumIdQuery;

    QSqlQuery mQueryMaximumArtistIdQuery;

    QSqlQuery mQueryMaximumLyricistIdQuery;

    QSqlQuery mQueryMaximumComposerIdQuery;

    QSqlQuery mQueryMaximumGenreIdQuery;

    QSqlQuery mSelectAllArtistsWithGenreFilterQuery;

    QSqlQuery mSelectAllAlbumsShortWithGenreArtistFilterQuery;

    QSqlQuery mSelectAllAlbumsShortWithArtistFilterQuery;

    QSqlQuery mSelectAllRecentlyPlayedTracksQuery;

    QSqlQuery mSelectAllFrequentlyPlayedTracksQuery;

    QSqlQuery mClearTracksDataTable;

    QSqlQuery mClearTracksTable;

    QSqlQuery mClearAlbumsTable;

    QSqlQuery mClearArtistsTable;

    QSqlQuery mClearComposerTable;

    QSqlQuery mClearGenreTable;

    QSqlQuery mClearLyricistTable;

    QSqlQuery mArtistMatchGenreQuery;

    QSqlQuery mSelectTrackIdQuery;

    QSqlQuery mInsertRadioQuery;

    QSqlQuery mDeleteRadioQuery;

    QSqlQuery mSelectTrackFromIdAndUrlQuery;

    QSqlQuery mUpdateDatabaseVersionQuery;

    QSqlQuery mSelectDatabaseVersionQuery;

    QSet<qulonglong> mModifiedTrackIds;

    QSet<qulonglong> mModifiedAlbumIds;

    QSet<qulonglong> mModifiedArtistIds;

    QSet<qulonglong> mInsertedTracks;

    QSet<qulonglong> mInsertedAlbums;

    QSet<QPair<qulonglong, QString>> mInsertedArtists;

    qulonglong mAlbumId = 1;

    qulonglong mArtistId = 1;

    qulonglong mComposerId = 1;

    qulonglong mLyricistId = 1;

    qulonglong mGenreId = 1;

    qulonglong mTrackId = 1;

    QAtomicInt mStopRequest = 0;

    bool mInitFinished = false;

    bool mIsInBadState = false;

};

DatabaseInterface::DatabaseInterface(QObject *parent) : QObject(parent), d(nullptr)
{
}

DatabaseInterface::~DatabaseInterface()
{
    if (d) {
        d->mTracksDatabase.close();
    }
}

void DatabaseInterface::init(const QString &dbName, const QString &databaseFileName)
{
    QSqlDatabase tracksDatabase = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), dbName);

    if (!databaseFileName.isEmpty()) {
        tracksDatabase.setDatabaseName(QStringLiteral("file:") + databaseFileName);
    } else {
        tracksDatabase.setDatabaseName(QStringLiteral("file:memdb1?mode=memory"));
    }
    tracksDatabase.setConnectOptions(QStringLiteral("foreign_keys = ON;locking_mode = EXCLUSIVE;QSQLITE_OPEN_URI;QSQLITE_BUSY_TIMEOUT=500000"));

    auto result = tracksDatabase.open();
    if (result) {
        qCDebug(orgKdeElisaDatabase) << "database open";
    } else {
        qCDebug(orgKdeElisaDatabase) << "database not open";
    }
    qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::init" << (tracksDatabase.driver()->hasFeature(QSqlDriver::Transactions) ? "yes" : "no");

    tracksDatabase.exec(QStringLiteral("PRAGMA foreign_keys = ON;"));

    d = std::make_unique<DatabaseInterfacePrivate>(tracksDatabase);

    initDatabase();
    initRequest();

    if (!databaseFileName.isEmpty()) {
        reloadExistingDatabase();
    }
}

qulonglong DatabaseInterface::albumIdFromTitleAndArtist(const QString &title, const QString &artist, const QString &albumPath)
{
    auto result = qulonglong{0};

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalAlbumIdFromTitleAndArtist(title, artist, albumPath);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::ListTrackDataType DatabaseInterface::allTracksData()
{
    auto result = DataTypes::ListTrackDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalAllTracksPartialData();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::ListRadioDataType DatabaseInterface::allRadiosData()
{
    auto result = DataTypes::ListRadioDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalAllRadiosPartialData();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::ListTrackDataType DatabaseInterface::recentlyPlayedTracksData(int count)
{
    auto result = DataTypes::ListTrackDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalRecentlyPlayedTracksData(count);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::ListTrackDataType DatabaseInterface::frequentlyPlayedTracksData(int count)
{
    auto result = DataTypes::ListTrackDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalFrequentlyPlayedTracksData(count);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::ListAlbumDataType DatabaseInterface::allAlbumsData()
{
    auto result = DataTypes::ListAlbumDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalAllAlbumsPartialData(d->mSelectAllAlbumsShortQuery);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::ListAlbumDataType DatabaseInterface::allAlbumsDataByGenreAndArtist(const QString &genre, const QString &artist)
{
    auto result = DataTypes::ListAlbumDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    d->mSelectAllAlbumsShortWithGenreArtistFilterQuery.bindValue(QStringLiteral(":artistFilter"), artist);
    d->mSelectAllAlbumsShortWithGenreArtistFilterQuery.bindValue(QStringLiteral(":genreFilter"), genre);

    result = internalAllAlbumsPartialData(d->mSelectAllAlbumsShortWithGenreArtistFilterQuery);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::ListAlbumDataType DatabaseInterface::allAlbumsDataByArtist(const QString &artist)
{
    auto result = DataTypes::ListAlbumDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    d->mSelectAllAlbumsShortWithArtistFilterQuery.bindValue(QStringLiteral(":artistFilter"), artist);

    result = internalAllAlbumsPartialData(d->mSelectAllAlbumsShortWithArtistFilterQuery);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::AlbumDataType DatabaseInterface::albumDataFromDatabaseId(qulonglong id)
{
    auto result = DataTypes::AlbumDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalOneAlbumPartialData(id);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::ListTrackDataType DatabaseInterface::albumData(qulonglong databaseId)
{
    auto result = DataTypes::ListTrackDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalOneAlbumData(databaseId);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::ListArtistDataType DatabaseInterface::allArtistsData()
{
    auto result = DataTypes::ListArtistDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalAllArtistsPartialData(d->mSelectAllArtistsQuery);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::ListArtistDataType DatabaseInterface::allArtistsDataByGenre(const QString &genre)
{
    qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::allArtistsDataByGenre" << genre;

    auto result = DataTypes::ListArtistDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    d->mSelectAllArtistsWithGenreFilterQuery.bindValue(QStringLiteral(":genreFilter"), genre);

    result = internalAllArtistsPartialData(d->mSelectAllArtistsWithGenreFilterQuery);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::ArtistDataType DatabaseInterface::artistDataFromDatabaseId(qulonglong id)
{
    auto result = DataTypes::ArtistDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalOneArtistPartialData(id);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

qulonglong DatabaseInterface::artistIdFromName(const QString &name)
{
    auto result = qulonglong{0};

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalArtistIdFromName(name);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::ListGenreDataType DatabaseInterface::allGenresData()
{
    auto result = DataTypes::ListGenreDataType{};

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalAllGenresPartialData();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

bool DatabaseInterface::internalArtistMatchGenre(qulonglong databaseId, const QString &genre)
{
    auto result = true;

    if (!d) {
        return result;
    }

    d->mArtistMatchGenreQuery.bindValue(QStringLiteral(":databaseId"), databaseId);
    d->mArtistMatchGenreQuery.bindValue(QStringLiteral(":genreFilter"), genre);

    auto queryResult = execQuery(d->mArtistMatchGenreQuery);

    if (!queryResult || !d->mArtistMatchGenreQuery.isSelect() || !d->mArtistMatchGenreQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::artistMatchGenre" << d->mArtistMatchGenreQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::artistMatchGenre" << d->mArtistMatchGenreQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::artistMatchGenre" << d->mArtistMatchGenreQuery.lastError();

        d->mArtistMatchGenreQuery.finish();

        auto transactionResult = finishTransaction();
        if (!transactionResult) {
            return result;
        }

        return result;
    }

    result = d->mArtistMatchGenreQuery.next();

    d->mArtistMatchGenreQuery.finish();

    qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalArtistMatchGenre" << databaseId << (result ? "match" : "does not match");

    return result;
}

DataTypes::ListTrackDataType DatabaseInterface::tracksDataFromAuthor(const QString &ArtistName)
{
    auto allTracks = DataTypes::ListTrackDataType{};

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return allTracks;
    }

    allTracks = internalTracksFromAuthor(ArtistName);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return allTracks;
    }

    return allTracks;
}

DataTypes::ListTrackDataType DatabaseInterface::tracksDataFromGenre(const QString &genre)
{
    auto allTracks = DataTypes::ListTrackDataType{};

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return allTracks;
    }

    allTracks = internalTracksFromGenre(genre);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return allTracks;
    }

    return allTracks;
}
DataTypes::TrackDataType DatabaseInterface::trackDataFromDatabaseId(qulonglong id)
{
    auto result = DataTypes::TrackDataType();

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalOneTrackPartialData(id);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::TrackDataType DatabaseInterface::trackDataFromDatabaseIdAndUrl(qulonglong id, const QUrl &trackUrl)
{
    auto result = DataTypes::TrackDataType();

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalOneTrackPartialDataByIdAndUrl(id, trackUrl);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

DataTypes::TrackDataType DatabaseInterface::radioDataFromDatabaseId(qulonglong id)
{
    auto result = DataTypes::TrackDataType();

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalOneRadioPartialData(id);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

qulonglong DatabaseInterface::trackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &artist, const std::optional<QString> &album,
                                                                     std::optional<int> trackNumber, std::optional<int> discNumber)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalTrackIdFromTitleAlbumTracDiscNumber(title, artist, album, trackNumber, discNumber);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

qulonglong DatabaseInterface::trackIdFromFileName(const QUrl &fileName)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalTrackIdFromFileName(fileName);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

qulonglong DatabaseInterface::radioIdFromFileName(const QUrl &fileName)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalRadioIdFromHttpAddress(fileName.toString());

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

void DatabaseInterface::applicationAboutToQuit()
{
    d->mStopRequest = 1;
}

void DatabaseInterface::askRestoredTracks()
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    auto result = internalAllFileName();

    Q_EMIT restoredTracks(result);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }
}

void DatabaseInterface::trackHasStartedPlaying(const QUrl &fileName, const QDateTime &time)
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    updateTrackStatistics(fileName, time);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }
}

void DatabaseInterface::clearData()
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    auto queryResult = execQuery(d->mClearTracksTable);

    if (!queryResult || !d->mClearTracksTable.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearTracksTable.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearTracksTable.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearTracksTable.lastError();
    }

    d->mClearTracksTable.finish();

    queryResult = execQuery(d->mClearTracksDataTable);

    if (!queryResult || !d->mClearTracksDataTable.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearTracksDataTable.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearTracksDataTable.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearTracksDataTable.lastError();
    }

    d->mClearTracksDataTable.finish();

    queryResult = execQuery(d->mClearAlbumsTable);

    if (!queryResult || !d->mClearAlbumsTable.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearAlbumsTable.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearAlbumsTable.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearAlbumsTable.lastError();
    }

    d->mClearAlbumsTable.finish();

    queryResult = execQuery(d->mClearComposerTable);

    if (!queryResult || !d->mClearComposerTable.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearComposerTable.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearComposerTable.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearComposerTable.lastError();
    }

    d->mClearComposerTable.finish();

    queryResult = execQuery(d->mClearLyricistTable);

    if (!queryResult || !d->mClearLyricistTable.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearLyricistTable.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearLyricistTable.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearLyricistTable.lastError();
    }

    d->mClearLyricistTable.finish();

    queryResult = execQuery(d->mClearGenreTable);

    if (!queryResult || !d->mClearGenreTable.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearGenreTable.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearGenreTable.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearGenreTable.lastError();
    }

    d->mClearGenreTable.finish();

    queryResult = execQuery(d->mClearArtistsTable);

    if (!queryResult || !d->mClearArtistsTable.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearArtistsTable.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearArtistsTable.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::clearData" << d->mClearArtistsTable.lastError();
    }

    d->mClearArtistsTable.finish();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }

    Q_EMIT cleanedDatabase();
}

void DatabaseInterface::initChangesTrackers()
{
    d->mModifiedTrackIds.clear();
    d->mModifiedAlbumIds.clear();
    d->mModifiedArtistIds.clear();
    d->mInsertedTracks.clear();
    d->mInsertedAlbums.clear();
    d->mInsertedArtists.clear();
}

void DatabaseInterface::recordModifiedTrack(qulonglong trackId)
{
    d->mModifiedTrackIds.insert(trackId);
}

void DatabaseInterface::recordModifiedAlbum(qulonglong albumId)
{
    d->mModifiedAlbumIds.insert(albumId);
}

void DatabaseInterface::insertTracksList(const DataTypes::ListTrackDataType &tracks, const QHash<QString, QUrl> &covers)
{
    qCDebug(orgKdeElisaDatabase()) << "DatabaseInterface::insertTracksList" << tracks.count();
    if (d->mStopRequest == 1) {
        Q_EMIT finishInsertingTracksList();
        return;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        Q_EMIT finishInsertingTracksList();
        return;
    }

    initChangesTrackers();

    for(const auto &oneTrack : tracks) {
        switch (oneTrack.elementType())
        {
        case ElisaUtils::Track:
        {
            qCDebug(orgKdeElisaDatabase()) << "DatabaseInterface::insertTracksList" << "insert one track";
            internalInsertOneTrack(oneTrack, covers);
            break;
        }
        case ElisaUtils::Radio:
        {
            qCDebug(orgKdeElisaDatabase()) << "DatabaseInterface::insertTracksList" << "insert one radio";
            internalInsertOneRadio(oneTrack);
            break;
        }
        case ElisaUtils::Album:
        case ElisaUtils::Artist:
        case ElisaUtils::Composer:
        case ElisaUtils::Container:
        case ElisaUtils::FileName:
        case ElisaUtils::Genre:
        case ElisaUtils::Lyricist:
        case ElisaUtils::Unknown:
            qCDebug(orgKdeElisaDatabase()) << "DatabaseInterface::insertTracksList" << "invalid track data";
            break;
        }

        if (d->mStopRequest == 1) {
            transactionResult = finishTransaction();
            if (!transactionResult) {
                Q_EMIT finishInsertingTracksList();
                return;
            }
            Q_EMIT finishInsertingTracksList();
            return;
        }
    }

    if (!d->mInsertedArtists.isEmpty()) {
        DataTypes::ListArtistDataType newArtists;

        for (auto newArtistData : qAsConst(d->mInsertedArtists)) {
            newArtists.push_back({{DataTypes::DatabaseIdRole, newArtistData.first},
                                  {DataTypes::TitleRole, newArtistData.second},
                                  {DataTypes::ElementTypeRole, ElisaUtils::Artist}});
        }
        qCInfo(orgKdeElisaDatabase) << "artistsAdded" << newArtists.size();
        Q_EMIT artistsAdded(newArtists);
    }

    if (!d->mInsertedAlbums.isEmpty()) {
        DataTypes::ListAlbumDataType newAlbums;

        for (auto albumId : qAsConst(d->mInsertedAlbums)) {
            d->mModifiedAlbumIds.remove(albumId);
            newAlbums.push_back(internalOneAlbumPartialData(albumId));
        }

        qCInfo(orgKdeElisaDatabase) << "albumsAdded" << newAlbums.size();
        Q_EMIT albumsAdded(newAlbums);
    }

    for (auto albumId : qAsConst(d->mModifiedAlbumIds)) {
        Q_EMIT albumModified({{DataTypes::DatabaseIdRole, albumId}}, albumId);
    }

    if (!d->mInsertedTracks.isEmpty()) {
        DataTypes::ListTrackDataType newTracks;

        for (auto trackId : qAsConst(d->mInsertedTracks)) {
            newTracks.push_back(internalOneTrackPartialData(trackId));
            d->mModifiedTrackIds.remove(trackId);
        }

        qCInfo(orgKdeElisaDatabase) << "tracksAdded" << newTracks.size();
        Q_EMIT tracksAdded(newTracks);
    }

    for (auto trackId : qAsConst(d->mModifiedTrackIds)) {
        Q_EMIT trackModified(internalOneTrackPartialData(trackId));
    }

    transactionResult = finishTransaction();
    if (!transactionResult) {
        Q_EMIT finishInsertingTracksList();
        return;
    }
    Q_EMIT finishInsertingTracksList();
}

void DatabaseInterface::removeTracksList(const QList<QUrl> &removedTracks)
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        Q_EMIT finishRemovingTracksList();
        return;
    }

    initChangesTrackers();

    internalRemoveTracksList(removedTracks);

    if (!d->mInsertedArtists.isEmpty()) {
        DataTypes::ListArtistDataType newArtists;
        for (auto newArtistData : qAsConst(d->mInsertedArtists)) {
            newArtists.push_back({{DataTypes::DatabaseIdRole, newArtistData.first},
                                  {DataTypes::TitleRole, newArtistData.second},
                                  {DataTypes::ElementTypeRole, ElisaUtils::Artist}});
        }
        Q_EMIT artistsAdded(newArtists);
    }

    transactionResult = finishTransaction();
    if (!transactionResult) {
        Q_EMIT finishRemovingTracksList();
        return;
    }

    Q_EMIT finishRemovingTracksList();
}

bool DatabaseInterface::startTransaction()
{
    auto result = false;

    auto transactionResult = d->mTracksDatabase.transaction();
    if (!transactionResult) {
        qCDebug(orgKdeElisaDatabase) << "transaction failed" << d->mTracksDatabase.lastError() << d->mTracksDatabase.lastError().driverText();

        return result;
    }

    result = true;

    return result;
}

bool DatabaseInterface::finishTransaction()
{
    auto result = false;

    auto transactionResult = d->mTracksDatabase.commit();

    if (!transactionResult) {
        qCDebug(orgKdeElisaDatabase) << "commit failed" << d->mTracksDatabase.lastError() << d->mTracksDatabase.lastError().nativeErrorCode();

        return result;
    }

    result = true;

    return result;
}

bool DatabaseInterface::rollBackTransaction()
{
    auto result = false;

    auto transactionResult = d->mTracksDatabase.rollback();

    if (!transactionResult) {
        qCDebug(orgKdeElisaDatabase) << "commit failed" << d->mTracksDatabase.lastError() << d->mTracksDatabase.lastError().nativeErrorCode();

        return result;
    }

    result = true;

    return result;
}

void DatabaseInterface::initDatabase()
{
    auto listTables = d->mTracksDatabase.tables();

    if (listTables.contains(QLatin1String("DatabaseVersionV2")) ||
        listTables.contains(QLatin1String("DatabaseVersionV3")) ||
        listTables.contains(QLatin1String("DatabaseVersionV4")) ||
        listTables.contains(QLatin1String("DatabaseVersionV6")) ||
        listTables.contains(QLatin1String("DatabaseVersionV7")) ||
        listTables.contains(QLatin1String("DatabaseVersionV8")) ||
        listTables.contains(QLatin1String("DatabaseVersionV10"))) {

        qCDebug(orgKdeElisaDatabase()) << "Old database schema unsupported: delete and start from scratch";
        qCDebug(orgKdeElisaDatabase()) << "list of old tables" << d->mTracksDatabase.tables();

        auto oldTables = QStringList{
                QStringLiteral("DatabaseVersionV2"),
                QStringLiteral("DatabaseVersionV3"),
                QStringLiteral("DatabaseVersionV4"),
                QStringLiteral("DatabaseVersionV5"),
                QStringLiteral("DatabaseVersionV6"),
                QStringLiteral("DatabaseVersionV7"),
                QStringLiteral("DatabaseVersionV8"),
                QStringLiteral("DatabaseVersionV10"),
                QStringLiteral("AlbumsArtists"),
                QStringLiteral("TracksArtists"),
                QStringLiteral("TracksMapping"),
                QStringLiteral("Tracks"),
                QStringLiteral("Composer"),
                QStringLiteral("Genre"),
                QStringLiteral("Lyricist"),
                QStringLiteral("Albums"),
                QStringLiteral("DiscoverSource"),
                QStringLiteral("Artists"),};
        for (const auto &oneTable : oldTables) {
            if (listTables.indexOf(oneTable) == -1) {
                continue;
            }

            QSqlQuery createSchemaQuery(d->mTracksDatabase);

            auto result = createSchemaQuery.exec(QLatin1String("DROP TABLE ") + oneTable);

            if (!result) {
                qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initDatabase" << createSchemaQuery.lastQuery();
                qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();

                Q_EMIT databaseError();
            }
        }
    }

    manageNewDatabaseVersion();
}

void DatabaseInterface::createDatabaseV9()
{
    qCInfo(orgKdeElisaDatabase) << "begin creation of v9 database schema";

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DatabaseVersionV9` (`Version` INTEGER PRIMARY KEY NOT NULL)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DiscoverSource` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(55) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Artists` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(55) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Composer` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(55) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Genre` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(85) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Lyricist` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(55) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Albums` ("
                                                                   "`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Title` VARCHAR(55) NOT NULL, "
                                                                   "`ArtistName` VARCHAR(55), "
                                                                   "`AlbumPath` VARCHAR(255) NOT NULL, "
                                                                   "`CoverFileName` VARCHAR(255) NOT NULL, "
                                                                   "UNIQUE (`Title`, `ArtistName`, `AlbumPath`), "
                                                                   "CONSTRAINT fk_artists FOREIGN KEY (`ArtistName`) REFERENCES `Artists`(`Name`) "
                                                                   "ON DELETE CASCADE)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Tracks` ("
                                                                   "`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Title` VARCHAR(85) NOT NULL, "
                                                                   "`ArtistName` VARCHAR(55), "
                                                                   "`AlbumTitle` VARCHAR(55), "
                                                                   "`AlbumArtistName` VARCHAR(55), "
                                                                   "`AlbumPath` VARCHAR(255), "
                                                                   "`TrackNumber` INTEGER DEFAULT -1, "
                                                                   "`DiscNumber` INTEGER DEFAULT -1, "
                                                                   "`Duration` INTEGER NOT NULL, "
                                                                   "`Rating` INTEGER NOT NULL DEFAULT 0, "
                                                                   "`Genre` VARCHAR(55), "
                                                                   "`Composer` VARCHAR(55), "
                                                                   "`Lyricist` VARCHAR(55), "
                                                                   "`Comment` VARCHAR(255) DEFAULT '', "
                                                                   "`Year` INTEGER DEFAULT 0, "
                                                                   "`Channels` INTEGER DEFAULT -1, "
                                                                   "`BitRate` INTEGER DEFAULT -1, "
                                                                   "`SampleRate` INTEGER DEFAULT -1, "
                                                                   "`HasEmbeddedCover` BOOLEAN NOT NULL, "
                                                                   "`ImportDate` INTEGER NOT NULL, "
                                                                   "`FirstPlayDate` INTEGER, "
                                                                   "`LastPlayDate` INTEGER, "
                                                                   "`PlayCounter` INTEGER NOT NULL, "
                                                                   "UNIQUE ("
                                                                   "`Title`, `AlbumTitle`, `AlbumArtistName`, "
                                                                   "`AlbumPath`, `TrackNumber`, `DiscNumber`"
                                                                   "), "
                                                                   "CONSTRAINT fk_artist FOREIGN KEY (`ArtistName`) REFERENCES `Artists`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_composer FOREIGN KEY (`Composer`) REFERENCES `Composer`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_lyricist FOREIGN KEY (`Lyricist`) REFERENCES `Lyricist`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_genre FOREIGN KEY (`Genre`) REFERENCES `Genre`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_album FOREIGN KEY ("
                                                                   "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"
                                                                   "REFERENCES `Albums`(`Title`, `ArtistName`, `AlbumPath`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `TracksMapping` ("
                                                                   "`TrackID` INTEGER NULL, "
                                                                   "`DiscoverID` INTEGER NOT NULL, "
                                                                   "`FileName` VARCHAR(255) NOT NULL, "
                                                                   "`Priority` INTEGER NOT NULL, "
                                                                   "`FileModifiedTime` DATETIME NOT NULL, "
                                                                   "PRIMARY KEY (`FileName`), "
                                                                   "CONSTRAINT TracksUnique UNIQUE (`TrackID`, `Priority`), "
                                                                   "CONSTRAINT fk_tracksmapping_trackID FOREIGN KEY (`TrackID`) REFERENCES `Tracks`(`ID`) ON DELETE CASCADE, "
                                                                   "CONSTRAINT fk_tracksmapping_discoverID FOREIGN KEY (`DiscoverID`) REFERENCES `DiscoverSource`(`ID`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TitleAlbumsIndex` ON `Albums` "
                                                                  "(`Title`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`ArtistNameAlbumsIndex` ON `Albums` "
                                                                  "(`ArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksAlbumIndex` ON `Tracks` "
                                                                  "(`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`ArtistNameIndex` ON `Tracks` "
                                                                  "(`ArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`AlbumArtistNameIndex` ON `Tracks` "
                                                                  "(`AlbumArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksFileNameIndex` ON `TracksMapping` "
                                                                  "(`FileName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseV9" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    qCInfo(orgKdeElisaDatabase) << "end creation of v9 database schema";
}

void DatabaseInterface::upgradeDatabaseV9()
{
    qCInfo(orgKdeElisaDatabase) << "begin update to v9 of database schema";

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DatabaseVersionV9` (`Version` INTEGER PRIMARY KEY NOT NULL)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery disableForeignKeys(d->mTracksDatabase);

        auto result = disableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=OFF"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << disableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << disableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.transaction();

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `NewAlbums` ("
                                                                   "`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Title` VARCHAR(55) NOT NULL, "
                                                                   "`ArtistName` VARCHAR(55), "
                                                                   "`AlbumPath` VARCHAR(255) NOT NULL, "
                                                                   "`CoverFileName` VARCHAR(255) NOT NULL, "
                                                                   "UNIQUE (`Title`, `ArtistName`, `AlbumPath`), "
                                                                   "CONSTRAINT fk_artists FOREIGN KEY (`ArtistName`) REFERENCES `Artists`(`Name`) "
                                                                   "ON DELETE CASCADE)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery copyDataQuery(d->mTracksDatabase);

        auto result = copyDataQuery.exec(QStringLiteral("INSERT INTO `NewAlbums` "
                                                        "SELECT "
                                                        "album.`ID`, "
                                                        "album.`Title`, "
                                                        "artist.`Name`, "
                                                        "album.`AlbumPath`, "
                                                        "album.`CoverFileName` "
                                                        "FROM "
                                                        "`Albums` album, "
                                                        "`AlbumsArtists` albumArtistMapping, "
                                                        "`Artists` artist "
                                                        "WHERE "
                                                        "album.`ID` = albumArtistMapping.`AlbumID` AND "
                                                        "albumArtistMapping.`ArtistID` = artist.`ID`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << copyDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << copyDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `Albums`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `AlbumsArtists`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("ALTER TABLE `NewAlbums` RENAME TO `Albums`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `NewTracks` ("
                                                                   "`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Title` VARCHAR(85) NOT NULL, "
                                                                   "`ArtistName` VARCHAR(55), "
                                                                   "`AlbumTitle` VARCHAR(55), "
                                                                   "`AlbumArtistName` VARCHAR(55), "
                                                                   "`AlbumPath` VARCHAR(255), "
                                                                   "`TrackNumber` INTEGER DEFAULT -1, "
                                                                   "`DiscNumber` INTEGER DEFAULT -1, "
                                                                   "`Duration` INTEGER NOT NULL, "
                                                                   "`Rating` INTEGER NOT NULL DEFAULT 0, "
                                                                   "`Genre` VARCHAR(55), "
                                                                   "`Composer` VARCHAR(55), "
                                                                   "`Lyricist` VARCHAR(55), "
                                                                   "`Comment` VARCHAR(255) DEFAULT '', "
                                                                   "`Year` INTEGER DEFAULT 0, "
                                                                   "`Channels` INTEGER DEFAULT -1, "
                                                                   "`BitRate` INTEGER DEFAULT -1, "
                                                                   "`SampleRate` INTEGER DEFAULT -1, "
                                                                   "`HasEmbeddedCover` BOOLEAN NOT NULL, "
                                                                   "`ImportDate` INTEGER NOT NULL, "
                                                                   "`FirstPlayDate` INTEGER, "
                                                                   "`LastPlayDate` INTEGER, "
                                                                   "`PlayCounter` INTEGER NOT NULL, "
                                                                   "UNIQUE ("
                                                                   "`Title`, `AlbumTitle`, `AlbumArtistName`, "
                                                                   "`AlbumPath`, `TrackNumber`, `DiscNumber`"
                                                                   "), "
                                                                   "CONSTRAINT fk_artist FOREIGN KEY (`ArtistName`) REFERENCES `Artists`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_composer FOREIGN KEY (`Composer`) REFERENCES `Composer`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_lyricist FOREIGN KEY (`Lyricist`) REFERENCES `Lyricist`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_genre FOREIGN KEY (`Genre`) REFERENCES `Genre`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_album FOREIGN KEY ("
                                                                   "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"
                                                                   "REFERENCES `Albums`(`Title`, `ArtistName`, `AlbumPath`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery copyDataQuery(d->mTracksDatabase);

        auto result = copyDataQuery.exec(QStringLiteral("INSERT INTO `NewTracks` "
                                                        "(`ID`, `Title`, `ArtistName`, "
                                                        "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`, "
                                                        "`TrackNumber`, `DiscNumber`, `Duration`, "
                                                        "`Rating`, `Genre`, `Composer`, "
                                                        "`Lyricist`, `Comment`, `Year`, "
                                                        "`Channels`, `BitRate`, `SampleRate`, "
                                                        "`HasEmbeddedCover`, `ImportDate`, `PlayCounter`) "
                                                        "SELECT "
                                                        "track.`ID`, "
                                                        "track.`Title`, "
                                                        "artist.`Name`, "
                                                        "album.`Title`, "
                                                        "album.`ArtistName`, "
                                                        "album.`AlbumPath`, "
                                                        "track.`TrackNumber`, "
                                                        "track.`DiscNumber`, "
                                                        "track.`Duration`, "
                                                        "track.`Rating`, "
                                                        "genre.`Name`, "
                                                        "composer.`Name`, "
                                                        "lyricist.`Name`, "
                                                        "track.`Comment`, "
                                                        "track.`Year`, "
                                                        "track.`Channels`, "
                                                        "track.`BitRate`, "
                                                        "track.`SampleRate`, "
                                                        "FALSE, "
                                                        "strftime('%s', 'now'), "
                                                        "0 "
                                                        "FROM "
                                                        "`Tracks` track, "
                                                        "`TracksArtists` trackArtistMapping, "
                                                        "`Artists` artist, "
                                                        "`Albums` album "
                                                        "left join "
                                                        "`Genre` genre "
                                                        "on track.`GenreID` = genre.`ID` "
                                                        "left join "
                                                        "`Composer` composer "
                                                        "on track.`ComposerID` = composer.`ID` "
                                                        "left join "
                                                        "`Lyricist` lyricist "
                                                        "on track.`LyricistID` = lyricist.`ID` "
                                                        "WHERE "
                                                        "track.`ID` = trackArtistMapping.`TrackID` AND "
                                                        "trackArtistMapping.`ArtistID` = artist.`ID` AND "
                                                        "track.`AlbumID` = album.`ID`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << copyDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << copyDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `TracksArtists`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("ALTER TABLE `NewTracks` RENAME TO `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.commit();

    {
        QSqlQuery enableForeignKeys(d->mTracksDatabase);

        auto result = enableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=ON"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << enableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV9" << enableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    qCInfo(orgKdeElisaDatabase) << "finished update to v9 of database schema";
}

void DatabaseInterface::upgradeDatabaseV11()
{
    qCInfo(orgKdeElisaDatabase) << "begin update to v11 of database schema";

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DatabaseVersionV11` (`Version` INTEGER PRIMARY KEY NOT NULL)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery disableForeignKeys(d->mTracksDatabase);

        auto result = disableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=OFF"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << disableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << disableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.transaction();

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `TracksData` ("
                                                                   "`DiscoverID` INTEGER NOT NULL, "
                                                                   "`FileName` VARCHAR(255) NOT NULL, "
                                                                   "`FileModifiedTime` DATETIME NOT NULL, "
                                                                   "`ImportDate` INTEGER NOT NULL, "
                                                                   "`FirstPlayDate` INTEGER, "
                                                                   "`LastPlayDate` INTEGER, "
                                                                   "`PlayCounter` INTEGER NOT NULL, "
                                                                   "PRIMARY KEY (`FileName`, `DiscoverID`), "
                                                                   "CONSTRAINT fk_tracksmapping_discoverID FOREIGN KEY (`DiscoverID`) REFERENCES `DiscoverSource`(`ID`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery copyDataQuery(d->mTracksDatabase);

        auto result = copyDataQuery.exec(QStringLiteral("INSERT INTO `TracksData` "
                                                        "SELECT "
                                                        "m.`DiscoverID`, "
                                                        "m.`FileName`, "
                                                        "m.`FileModifiedTime`, "
                                                        "t.`ImportDate`, "
                                                        "t.`FirstPlayDate`, "
                                                        "t.`LastPlayDate`, "
                                                        "t.`PlayCounter` "
                                                        "FROM "
                                                        "`Tracks` t, "
                                                        "`TracksMapping` m "
                                                        "WHERE "
                                                        "t.`ID` = m.`TrackID`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << copyDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << copyDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `NewTracks` ("
                                                            "`ID` INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                            "`DiscoverID` INTEGER NOT NULL, "
                                                            "`FileName` VARCHAR(255) NOT NULL, "
                                                            "`Priority` INTEGER NOT NULL, "
                                                            "`Title` VARCHAR(85) NOT NULL, "
                                                            "`ArtistName` VARCHAR(55), "
                                                            "`AlbumTitle` VARCHAR(55), "
                                                            "`AlbumArtistName` VARCHAR(55), "
                                                            "`AlbumPath` VARCHAR(255), "
                                                            "`TrackNumber` INTEGER, "
                                                            "`DiscNumber` INTEGER, "
                                                            "`Duration` INTEGER NOT NULL, "
                                                            "`Rating` INTEGER NOT NULL DEFAULT 0, "
                                                            "`Genre` VARCHAR(55), "
                                                            "`Composer` VARCHAR(55), "
                                                            "`Lyricist` VARCHAR(55), "
                                                            "`Comment` VARCHAR(255), "
                                                            "`Year` INTEGER, "
                                                            "`Channels` INTEGER, "
                                                            "`BitRate` INTEGER, "
                                                            "`SampleRate` INTEGER, "
                                                            "`HasEmbeddedCover` BOOLEAN NOT NULL, "
                                                            "UNIQUE ("
                                                            "`FileName`"
                                                            "), "
                                                            "UNIQUE ("
                                                            "`Priority`, `Title`, `ArtistName`, "
                                                            "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`"
                                                            "), "
                                                            "CONSTRAINT fk_fileName FOREIGN KEY (`FileName`, `DiscoverID`) "
                                                            "REFERENCES `TracksData`(`FileName`, `DiscoverID`) ON DELETE CASCADE, "
                                                            "CONSTRAINT fk_artist FOREIGN KEY (`ArtistName`) REFERENCES `Artists`(`Name`), "
                                                            "CONSTRAINT fk_tracks_composer FOREIGN KEY (`Composer`) REFERENCES `Composer`(`Name`), "
                                                            "CONSTRAINT fk_tracks_lyricist FOREIGN KEY (`Lyricist`) REFERENCES `Lyricist`(`Name`), "
                                                            "CONSTRAINT fk_tracks_genre FOREIGN KEY (`Genre`) REFERENCES `Genre`(`Name`), "
                                                            "CONSTRAINT fk_tracks_discoverID FOREIGN KEY (`DiscoverID`) REFERENCES `DiscoverSource`(`ID`)"
                                                            "CONSTRAINT fk_tracks_album FOREIGN KEY ("
                                                            "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"
                                                            "REFERENCES `Albums`(`Title`, `ArtistName`, `AlbumPath`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery copyDataQuery(d->mTracksDatabase);

        auto result = copyDataQuery.exec(QStringLiteral("INSERT OR IGNORE INTO `NewTracks` "
                                                        "("
                                                        "`DiscoverID`, "
                                                        "`FileName`, "
                                                        "`Priority`, "
                                                        "`Title`, "
                                                        "`ArtistName`, "
                                                        "`AlbumTitle`, "
                                                        "`AlbumArtistName`, "
                                                        "`AlbumPath`, "
                                                        "`TrackNumber`, "
                                                        "`DiscNumber`, "
                                                        "`Duration`, "
                                                        "`Rating`, "
                                                        "`Genre`, "
                                                        "`Composer`, "
                                                        "`Lyricist`, "
                                                        "`Comment`, "
                                                        "`Year`, "
                                                        "`Channels`, "
                                                        "`BitRate`, "
                                                        "`SampleRate`, "
                                                        "`HasEmbeddedCover`"
                                                        ") "
                                                        "SELECT "
                                                        "m.`DiscoverID`, "
                                                        "m.`FileName`, "
                                                        "m.`Priority`, "
                                                        "t.`Title`, "
                                                        "t.`ArtistName`, "
                                                        "t.`AlbumTitle`, "
                                                        "t.`AlbumArtistName`, "
                                                        "t.`AlbumPath`, "
                                                        "t.`TrackNumber`, "
                                                        "t.`DiscNumber`, "
                                                        "t.`Duration`, "
                                                        "t.`Rating`, "
                                                        "t.`Genre`, "
                                                        "t.`Composer`, "
                                                        "t.`Lyricist`, "
                                                        "t.`Comment`, "
                                                        "t.`Year`, "
                                                        "t.`Channels`, "
                                                        "t.`BitRate`, "
                                                        "t.`SampleRate`, "
                                                        "t.`HasEmbeddedCover` "
                                                        "FROM "
                                                        "`Tracks` t, "
                                                        "`TracksMapping` m "
                                                        "WHERE "
                                                        "t.`ID` = m.`TrackID`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << copyDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << copyDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery updateDataQuery(d->mTracksDatabase);

        auto result = updateDataQuery.exec(QStringLiteral("UPDATE `NewTracks` "
                                                          "SET "
                                                          "`TrackNumber` = NULL "
                                                          "WHERE "
                                                          "`TrackNumber` = -1"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery updateDataQuery(d->mTracksDatabase);

        auto result = updateDataQuery.exec(QStringLiteral("UPDATE `NewTracks` "
                                                          "SET "
                                                          "`Channels` = NULL "
                                                          "WHERE "
                                                          "`Channels` = -1"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery updateDataQuery(d->mTracksDatabase);

        auto result = updateDataQuery.exec(QStringLiteral("UPDATE `NewTracks` "
                                                          "SET "
                                                          "`BitRate` = NULL "
                                                          "WHERE "
                                                          "`BitRate` = -1"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery updateDataQuery(d->mTracksDatabase);

        auto result = updateDataQuery.exec(QStringLiteral("UPDATE `NewTracks` "
                                                          "SET "
                                                          "`SampleRate` = NULL "
                                                          "WHERE "
                                                          "`SampleRate` = -1"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << updateDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `TracksMapping`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("ALTER TABLE `NewTracks` RENAME TO `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.commit();

    {
        QSqlQuery enableForeignKeys(d->mTracksDatabase);

        auto result = enableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=ON"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << enableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << enableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksAlbumIndex` ON `Tracks` "
                                                                  "(`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`ArtistNameIndex` ON `Tracks` "
                                                                  "(`ArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`AlbumArtistNameIndex` ON `Tracks` "
                                                                  "(`AlbumArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksUniqueData` ON `Tracks` "
                                                                  "(`Title`, `ArtistName`, "
                                                                  "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksUniqueDataPriority` ON `Tracks` "
                                                                  "(`Priority`, `Title`, `ArtistName`, "
                                                                  "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksFileNameIndex` ON `Tracks` "
                                                                  "(`FileName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV11" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    qCInfo(orgKdeElisaDatabase) << "finished update to v11 of database schema";
}

void DatabaseInterface::upgradeDatabaseV12()
{
    qCInfo(orgKdeElisaDatabase) << "begin update to v12 of database schema";

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DatabaseVersionV12` (`Version` INTEGER PRIMARY KEY NOT NULL)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery disableForeignKeys(d->mTracksDatabase);

        auto result = disableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=OFF"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << disableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << disableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.transaction();

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `NewTracks` ("
                                                                   "`ID` INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                                   "`FileName` VARCHAR(255) NOT NULL, "
                                                                   "`Priority` INTEGER NOT NULL, "
                                                                   "`Title` VARCHAR(85) NOT NULL, "
                                                                   "`ArtistName` VARCHAR(55), "
                                                                   "`AlbumTitle` VARCHAR(55), "
                                                                   "`AlbumArtistName` VARCHAR(55), "
                                                                   "`AlbumPath` VARCHAR(255), "
                                                                   "`TrackNumber` INTEGER, "
                                                                   "`DiscNumber` INTEGER, "
                                                                   "`Duration` INTEGER NOT NULL, "
                                                                   "`Rating` INTEGER NOT NULL DEFAULT 0, "
                                                                   "`Genre` VARCHAR(55), "
                                                                   "`Composer` VARCHAR(55), "
                                                                   "`Lyricist` VARCHAR(55), "
                                                                   "`Comment` VARCHAR(255), "
                                                                   "`Year` INTEGER, "
                                                                   "`Channels` INTEGER, "
                                                                   "`BitRate` INTEGER, "
                                                                   "`SampleRate` INTEGER, "
                                                                   "`HasEmbeddedCover` BOOLEAN NOT NULL, "
                                                                   "UNIQUE ("
                                                                   "`FileName`"
                                                                   "), "
                                                                   "UNIQUE ("
                                                                   "`Priority`, `Title`, `ArtistName`, "
                                                                   "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`"
                                                                   "), "
                                                                   "CONSTRAINT fk_fileName FOREIGN KEY (`FileName`) "
                                                                   "REFERENCES `TracksData`(`FileName`) ON DELETE CASCADE, "
                                                                   "CONSTRAINT fk_artist FOREIGN KEY (`ArtistName`) REFERENCES `Artists`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_composer FOREIGN KEY (`Composer`) REFERENCES `Composer`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_lyricist FOREIGN KEY (`Lyricist`) REFERENCES `Lyricist`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_genre FOREIGN KEY (`Genre`) REFERENCES `Genre`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_album FOREIGN KEY ("
                                                                   "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"
                                                                   "REFERENCES `Albums`(`Title`, `ArtistName`, `AlbumPath`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `NewTracksData` ("
                                                                   "`FileName` VARCHAR(255) NOT NULL, "
                                                                   "`FileModifiedTime` DATETIME NOT NULL, "
                                                                   "`ImportDate` INTEGER NOT NULL, "
                                                                   "`FirstPlayDate` INTEGER, "
                                                                   "`LastPlayDate` INTEGER, "
                                                                   "`PlayCounter` INTEGER NOT NULL, "
                                                                   "PRIMARY KEY (`FileName`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery copyDataQuery(d->mTracksDatabase);

        auto result = copyDataQuery.exec(QStringLiteral("INSERT INTO `NewTracksData` "
                                                        "SELECT "
                                                        "td.`FileName`, "
                                                        "td.`FileModifiedTime`, "
                                                        "td.`ImportDate`, "
                                                        "td.`FirstPlayDate`, "
                                                        "td.`LastPlayDate`, "
                                                        "td.`PlayCounter` "
                                                        "FROM "
                                                        "`TracksData` td"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << copyDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << copyDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery copyDataQuery(d->mTracksDatabase);

        auto result = copyDataQuery.exec(QStringLiteral("INSERT INTO `NewTracks` "
                                                        "SELECT "
                                                        "t.`ID`, "
                                                        "t.`FileName`, "
                                                        "t.`Priority`, "
                                                        "t.`Title`, "
                                                        "t.`ArtistName`, "
                                                        "t.`AlbumTitle`, "
                                                        "t.`AlbumArtistName`, "
                                                        "t.`AlbumPath`, "
                                                        "t.`TrackNumber`, "
                                                        "t.`DiscNumber`, "
                                                        "t.`Duration`, "
                                                        "t.`Rating`, "
                                                        "t.`Genre`, "
                                                        "t.`Composer`, "
                                                        "t.`Lyricist`, "
                                                        "t.`Comment`, "
                                                        "t.`Year`, "
                                                        "t.`Channels`, "
                                                        "t.`BitRate`, "
                                                        "t.`SampleRate`, "
                                                        "t.`HasEmbeddedCover` "
                                                        "FROM "
                                                        "`Tracks` t"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << copyDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << copyDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `TracksData`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("ALTER TABLE `NewTracksData` RENAME TO `TracksData`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("ALTER TABLE `NewTracks` RENAME TO `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.commit();

    {
        QSqlQuery enableForeignKeys(d->mTracksDatabase);

        auto result = enableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=ON"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << enableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << enableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksAlbumIndex` ON `Tracks` "
                                                                  "(`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`ArtistNameIndex` ON `Tracks` "
                                                                  "(`ArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`AlbumArtistNameIndex` ON `Tracks` "
                                                                  "(`AlbumArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksUniqueData` ON `Tracks` "
                                                                  "(`Title`, `ArtistName`, "
                                                                  "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksUniqueDataPriority` ON `Tracks` "
                                                                  "(`Priority`, `Title`, `ArtistName`, "
                                                                  "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksFileNameIndex` ON `Tracks` "
                                                                  "(`FileName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV12" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    qCInfo(orgKdeElisaDatabase) << "finished update to v12 of database schema";
}

void DatabaseInterface::upgradeDatabaseV13()
{
    qCInfo(orgKdeElisaDatabase) << "begin update to v13 of database schema";

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DatabaseVersionV13` (`Version` INTEGER PRIMARY KEY NOT NULL)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery disableForeignKeys(d->mTracksDatabase);

        auto result = disableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=OFF"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << disableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << disableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.transaction();

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `NewTracks` ("
                                                                   "`ID` INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                                   "`FileName` VARCHAR(255) NOT NULL, "
                                                                   "`Priority` INTEGER NOT NULL, "
                                                                   "`Title` VARCHAR(85) NOT NULL, "
                                                                   "`ArtistName` VARCHAR(55), "
                                                                   "`AlbumTitle` VARCHAR(55), "
                                                                   "`AlbumArtistName` VARCHAR(55), "
                                                                   "`AlbumPath` VARCHAR(255), "
                                                                   "`TrackNumber` INTEGER, "
                                                                   "`DiscNumber` INTEGER, "
                                                                   "`Duration` INTEGER NOT NULL, "
                                                                   "`Rating` INTEGER NOT NULL DEFAULT 0, "
                                                                   "`Genre` VARCHAR(55), "
                                                                   "`Composer` VARCHAR(55), "
                                                                   "`Lyricist` VARCHAR(55), "
                                                                   "`Comment` VARCHAR(255), "
                                                                   "`Year` INTEGER, "
                                                                   "`Channels` INTEGER, "
                                                                   "`BitRate` INTEGER, "
                                                                   "`SampleRate` INTEGER, "
                                                                   "`HasEmbeddedCover` BOOLEAN NOT NULL, "
                                                                   "UNIQUE ("
                                                                   "`FileName`"
                                                                   "), "
                                                                   "UNIQUE ("
                                                                   "`Priority`, `Title`, `ArtistName`, "
                                                                   "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`, "
                                                                   "`TrackNumber`, `DiscNumber`"
                                                                   "), "
                                                                   "CONSTRAINT fk_fileName FOREIGN KEY (`FileName`) "
                                                                   "REFERENCES `TracksData`(`FileName`) ON DELETE CASCADE, "
                                                                   "CONSTRAINT fk_artist FOREIGN KEY (`ArtistName`) REFERENCES `Artists`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_composer FOREIGN KEY (`Composer`) REFERENCES `Composer`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_lyricist FOREIGN KEY (`Lyricist`) REFERENCES `Lyricist`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_genre FOREIGN KEY (`Genre`) REFERENCES `Genre`(`Name`), "
                                                                   "CONSTRAINT fk_tracks_album FOREIGN KEY ("
                                                                   "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"
                                                                   "REFERENCES `Albums`(`Title`, `ArtistName`, `AlbumPath`))"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery copyDataQuery(d->mTracksDatabase);

        auto result = copyDataQuery.exec(QStringLiteral("INSERT INTO `NewTracks` "
                                                        "SELECT "
                                                        "t.`ID`, "
                                                        "t.`FileName`, "
                                                        "t.`Priority`, "
                                                        "t.`Title`, "
                                                        "t.`ArtistName`, "
                                                        "t.`AlbumTitle`, "
                                                        "t.`AlbumArtistName`, "
                                                        "t.`AlbumPath`, "
                                                        "t.`TrackNumber`, "
                                                        "t.`DiscNumber`, "
                                                        "t.`Duration`, "
                                                        "t.`Rating`, "
                                                        "t.`Genre`, "
                                                        "t.`Composer`, "
                                                        "t.`Lyricist`, "
                                                        "t.`Comment`, "
                                                        "t.`Year`, "
                                                        "t.`Channels`, "
                                                        "t.`BitRate`, "
                                                        "t.`SampleRate`, "
                                                        "t.`HasEmbeddedCover` "
                                                        "FROM "
                                                        "`Tracks` t"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << copyDataQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << copyDataQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        auto result = createSchemaQuery.exec(QStringLiteral("ALTER TABLE `NewTracks` RENAME TO `Tracks`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.commit();

    {
        QSqlQuery enableForeignKeys(d->mTracksDatabase);

        auto result = enableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=ON"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << enableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << enableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksAlbumIndex` ON `Tracks` "
                                                                  "(`AlbumTitle`, `AlbumArtistName`, `AlbumPath`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`ArtistNameIndex` ON `Tracks` "
                                                                  "(`ArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`AlbumArtistNameIndex` ON `Tracks` "
                                                                  "(`AlbumArtistName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksUniqueData` ON `Tracks` "
                                                                  "(`Title`, `ArtistName`, "
                                                                  "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`, "
                                                                  "`TrackNumber`, `DiscNumber`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksUniqueDataPriority` ON `Tracks` "
                                                                  "(`Priority`, `Title`, `ArtistName`, "
                                                                  "`AlbumTitle`, `AlbumArtistName`, `AlbumPath`, "
                                                                  "`TrackNumber`, `DiscNumber`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksFileNameIndex` ON `Tracks` "
                                                                  "(`FileName`)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createTrackIndex.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV13" << createTrackIndex.lastError();

            Q_EMIT databaseError();
        }
    }

    qCInfo(orgKdeElisaDatabase) << "finished update to v13 of database schema";
}

void DatabaseInterface::upgradeDatabaseV14()
{
    qCInfo(orgKdeElisaDatabase) << "begin update to v14 of database schema";

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DatabaseVersionV14` (`Version` INTEGER PRIMARY KEY NOT NULL)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV14" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV14" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery disableForeignKeys(d->mTracksDatabase);

        auto result = disableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=OFF"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV14" << disableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV14" << disableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.transaction();

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Radios` ("
                                                                   "`ID` INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                                   "`HttpAddress` VARCHAR(255) NOT NULL, "
                                                                   "`Priority` INTEGER NOT NULL, "
                                                                   "`Title` VARCHAR(85) NOT NULL, "
                                                                   "`Rating` INTEGER NOT NULL DEFAULT 0, "
                                                                   "`Genre` VARCHAR(55), "
                                                                   "`Comment` VARCHAR(255), "
                                                                   "UNIQUE ("
                                                                   "`HttpAddress`"
                                                                   "), "
                                                                   "UNIQUE ("
                                                                   "`Priority`, `Title`, `HttpAddress`"
                                                                   ") "
                                                                   "CONSTRAINT fk_tracks_genre FOREIGN KEY (`Genre`) REFERENCES `Genre`(`Name`))"
                                                                   ));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV14" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV14" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        //Find webradios (french): https://doc.ubuntu-fr.org/liste_radio_france
        //English: https://www.radio.fr/language/english (to get the link play a radio and look for streamUrl in the html elements page).
        const auto &result = createSchemaQuery.exec(QStringLiteral("INSERT INTO `Radios` (`HttpAddress`, `Priority`, `Title`) "
                                        "SELECT 'http://classicrock.stream.ouifm.fr/ouifm3.mp3', 1, 'OuiFM_Classic_Rock' UNION ALL "
                                        "SELECT 'http://rock70s.stream.ouifm.fr/ouifmseventies.mp3', 1, 'OuiFM_70s' UNION ALL "
                                        "SELECT 'http://jazzradio.ice.infomaniak.ch/jazzradio-high.mp3', 2 , 'Jazz_Radio' UNION ALL "
                                        "SELECT 'http://cdn.nrjaudio.fm/audio1/fr/30601/mp3_128.mp3?origine=playerweb', 1, 'Nostalgie' UNION ALL "
                                        "SELECT 'https://scdn.nrjaudio.fm/audio1/fr/30713/aac_64.mp3?origine=playerweb', 1, 'Nostalgie Johnny' UNION ALL "
                                        "SELECT 'http://sc-classrock.1.fm:8200', 1, 'Classic rock replay' UNION ALL "
                                        "SELECT 'http://agnes.torontocast.com:8151/stream', 1, 'Instrumentals Forever' UNION ALL "
                                        "SELECT 'https://stream.laut.fm/jahfari', 1, 'Jahfari'"
                                                                   ));
        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.commit();

    {
        QSqlQuery enableForeignKeys(d->mTracksDatabase);

        auto result = enableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=ON"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV14" << enableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV14" << enableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    qCInfo(orgKdeElisaDatabase) << "finished update to v14 of database schema";
}

void DatabaseInterface::upgradeDatabaseV15()
{
    qCInfo(orgKdeElisaDatabase) << "begin update to v15 of database schema";

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DatabaseVersionV15` (`Version` INTEGER PRIMARY KEY NOT NULL)"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery disableForeignKeys(d->mTracksDatabase);

        auto result = disableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=OFF"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << disableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << disableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.transaction();

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `RadiosNew` ("
                                                                   "`ID` INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                                   "`HttpAddress` VARCHAR(255) NOT NULL, "
                                                                   "`ImageAddress` VARCHAR(255) NOT NULL, "
                                                                   "`Title` VARCHAR(85) NOT NULL, "
                                                                   "`Rating` INTEGER NOT NULL DEFAULT 0, "
                                                                   "`Genre` VARCHAR(55), "
                                                                   "`Comment` VARCHAR(255), "
                                                                   "UNIQUE ("
                                                                   "`HttpAddress`"
                                                                   "), "
                                                                   "UNIQUE ("
                                                                   "`Title`, `HttpAddress`"
                                                                   ") "
                                                                   "CONSTRAINT fk_tracks_genre FOREIGN KEY (`Genre`) REFERENCES `Genre`(`Name`))"
                                                                   ));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("INSERT INTO RadiosNew SELECT ID, HttpAddress, '', Title, Rating, Genre, Comment FROM Radios"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("DROP TABLE `Radios`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("ALTER TABLE `RadiosNew` RENAME TO `Radios`"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << createSchemaQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);
        const auto &result = createSchemaQuery.exec(QStringLiteral("INSERT INTO `Radios` (`HttpAddress`, `ImageAddress`, `Title`) "
                                        "SELECT 'https://chai5she.cdn.dvmr.fr/francemusique-lofi.mp3', 'https://static.radio.fr/images/broadcasts/07/f7/3366/c44.png', 'France Musique'"
                                                                   ));
        if (!result) {
            qCInfo(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << createSchemaQuery.lastQuery();
            qCInfo(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << createSchemaQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    d->mTracksDatabase.commit();

    {
        QSqlQuery enableForeignKeys(d->mTracksDatabase);

        auto result = enableForeignKeys.exec(QStringLiteral(" PRAGMA foreign_keys=ON"));

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << enableForeignKeys.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::upgradeDatabaseV15" << enableForeignKeys.lastError();

            Q_EMIT databaseError();
        }
    }

    qCInfo(orgKdeElisaDatabase) << "finished update to v15 of database schema";
}

void DatabaseInterface::upgradeDatabaseV16()
{

}

void DatabaseInterface::checkDatabaseSchema()
{
    checkAlbumsTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }

    checkArtistsTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }

    checkComposerTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }

    checkGenreTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }

    checkLyricistTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }

    checkTracksTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }

    checkTracksDataTableSchema();
    if (d->mIsInBadState)
    {
        resetDatabase();
        return;
    }
}

void DatabaseInterface::checkAlbumsTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("ID"), QStringLiteral("Title"),
                                  QStringLiteral("ArtistName"), QStringLiteral("AlbumPath"),
                                  QStringLiteral("CoverFileName")};

    genericCheckTable(QStringLiteral("Albums"), fieldsList);
}

void DatabaseInterface::checkArtistsTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("ID"), QStringLiteral("Name")};

    genericCheckTable(QStringLiteral("Artists"), fieldsList);
}

void DatabaseInterface::checkComposerTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("ID"), QStringLiteral("Name")};

    genericCheckTable(QStringLiteral("Composer"), fieldsList);
}

void DatabaseInterface::checkGenreTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("ID"), QStringLiteral("Name")};

    genericCheckTable(QStringLiteral("Genre"), fieldsList);
}

void DatabaseInterface::checkLyricistTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("ID"), QStringLiteral("Name")};

    genericCheckTable(QStringLiteral("Lyricist"), fieldsList);
}

void DatabaseInterface::checkTracksTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("ID"), QStringLiteral("FileName"),
                                  QStringLiteral("Priority"), QStringLiteral("Title"),
                                  QStringLiteral("ArtistName"), QStringLiteral("AlbumTitle"),
                                  QStringLiteral("AlbumArtistName"), QStringLiteral("AlbumPath"),
                                  QStringLiteral("TrackNumber"), QStringLiteral("DiscNumber"),
                                  QStringLiteral("Duration"), QStringLiteral("Rating"),
                                  QStringLiteral("Genre"), QStringLiteral("Composer"),
                                  QStringLiteral("Lyricist"), QStringLiteral("Comment"),
                                  QStringLiteral("Year"), QStringLiteral("Channels"),
                                  QStringLiteral("BitRate"), QStringLiteral("SampleRate"),
                                  QStringLiteral("HasEmbeddedCover")};

    genericCheckTable(QStringLiteral("Tracks"), fieldsList);
}

void DatabaseInterface::checkTracksDataTableSchema()
{
    auto fieldsList = QStringList{QStringLiteral("FileName"), QStringLiteral("FileModifiedTime"),
                                  QStringLiteral("ImportDate"), QStringLiteral("FirstPlayDate"),
                                  QStringLiteral("LastPlayDate"), QStringLiteral("PlayCounter")};

    genericCheckTable(QStringLiteral("TracksData"), fieldsList);
}

void DatabaseInterface::genericCheckTable(const QString &tableName, const QStringList &expectedColumns)
{
    auto columnsList = d->mTracksDatabase.record(tableName);

    if (columnsList.count() != expectedColumns.count()) {
        qCInfo(orgKdeElisaDatabase()) << tableName << "table has wrong number of columns" << columnsList.count() << "expected" << expectedColumns.count();
        d->mIsInBadState = true;
        return;
    }

    for (const auto &oneField : expectedColumns) {
        if (!columnsList.contains(oneField)) {
            qCInfo(orgKdeElisaDatabase()) << tableName << "table has missing column" << oneField;
            d->mIsInBadState = true;
            return;
        }
    }
}

void DatabaseInterface::resetDatabase()
{
    qCInfo(orgKdeElisaDatabase()) << "Full reset of database due to corrupted database";

    auto listTables = d->mTracksDatabase.tables();

    while(!listTables.isEmpty()) {
        for (const auto &oneTable : listTables) {
            QSqlQuery createSchemaQuery(d->mTracksDatabase);

            qCDebug(orgKdeElisaDatabase()) << "dropping table" << oneTable;
            createSchemaQuery.exec(QStringLiteral("DROP TABLE ") + oneTable);
        }

        listTables = d->mTracksDatabase.tables();

        if (listTables == QStringList{QStringLiteral("sqlite_sequence")}) {
            break;
        }
    }

    d->mIsInBadState = false;
}

void DatabaseInterface::manageNewDatabaseVersion()
{
    int versionBegin = 0;

    auto listTables = d->mTracksDatabase.tables();

    if (listTables.contains(QLatin1String("DatabaseVersion"))) {
        manageNewDatabaseVersionInitRequests();

        auto queryResult = execQuery(d->mSelectDatabaseVersionQuery);
        if (!queryResult) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::manageNewDatabaseVersion" << d->mUpdateDatabaseVersionQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::manageNewDatabaseVersion" << d->mUpdateDatabaseVersionQuery.lastError();

            Q_EMIT databaseError();
        }

        if(d->mSelectDatabaseVersionQuery.next()) {
            const auto &currentRecord = d->mSelectDatabaseVersionQuery.record();

            versionBegin = currentRecord.value(0).toInt();
        }
    } else if (listTables.contains(QLatin1String("DatabaseVersionV5")) &&
               !listTables.contains(QLatin1String("DatabaseVersionV9"))) {
        versionBegin = DatabaseInterface::V9;
    } else {
        createDatabaseVersionTable();
        manageNewDatabaseVersionInitRequests();

        if(listTables.contains(QLatin1String("DatabaseVersionV9"))) {
            if (!listTables.contains(QLatin1String("DatabaseVersionV11"))) {
                versionBegin = DatabaseInterface::V11;
            } else if (!listTables.contains(QLatin1String("DatabaseVersionV12"))) {
                versionBegin = DatabaseInterface::V12;
            } else if (!listTables.contains(QLatin1String("DatabaseVersionV13"))) {
                versionBegin = DatabaseInterface::V13;
            } else if (!listTables.contains(QLatin1String("DatabaseVersionV14"))) {
                versionBegin = DatabaseInterface::V14;
            } else {
                versionBegin = DatabaseInterface::V15;
            }
        } else {
            createDatabaseV9();
            versionBegin = DatabaseInterface::V11;
        }
    }

    int version = versionBegin;
    for (; version-1 != DatabaseInterface::V16; version++) {
        callUpgradeFunctionForVersion(static_cast<DatabaseVersion>(version));
    }

    if (version-1 != versionBegin) {
        dropTable(QStringLiteral("DROP TABLE DatabaseVersionV9"));
        dropTable(QStringLiteral("DROP TABLE DatabaseVersionV11"));
        dropTable(QStringLiteral("DROP TABLE DatabaseVersionV12"));
        dropTable(QStringLiteral("DROP TABLE DatabaseVersionV13"));
        dropTable(QStringLiteral("DROP TABLE DatabaseVersionV14"));
    }

    setDatabaseVersionInTable(DatabaseInterface::V16);

    checkDatabaseSchema();
}

void DatabaseInterface::dropTable(const QString &query)
{
    QSqlQuery dropQueryQuery(d->mTracksDatabase);

    const auto &result = dropQueryQuery.exec(query);

    if (!result) {
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::dropTable" << dropQueryQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::dropTable" << dropQueryQuery.lastError();

        Q_EMIT databaseError();
    }
}

void DatabaseInterface::setDatabaseVersionInTable(int version)
{
    d->mUpdateDatabaseVersionQuery.bindValue(QStringLiteral(":version"), version);

    auto queryResult = execQuery(d->mUpdateDatabaseVersionQuery);

    if (!queryResult) {
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::setDatabaseVersionInTable" << d->mUpdateDatabaseVersionQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::setDatabaseVersionInTable" << d->mUpdateDatabaseVersionQuery.lastError();

        Q_EMIT databaseError();
    }
}

void DatabaseInterface::createDatabaseVersionTable()
{
    qCInfo(orgKdeElisaDatabase) << "begin creation of DatabaseVersion table";
    QSqlQuery createSchemaQuery(d->mTracksDatabase);

    const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DatabaseVersion` (`Version` INTEGER PRIMARY KEY NOT NULL default 0)"));

    if (!result) {
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseVersionTable" << createSchemaQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseVersionTable" << createSchemaQuery.lastError();

        Q_EMIT databaseError();
    }

    const auto resultInsert = createSchemaQuery.exec(QStringLiteral("INSERT INTO `DatabaseVersion` VALUES (0)"));
    if (!resultInsert) {
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseVersionTable" << createSchemaQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::createDatabaseVersionTable" << createSchemaQuery.lastError();

        Q_EMIT databaseError();
    }
}

void DatabaseInterface::manageNewDatabaseVersionInitRequests()
{
    {
        auto  initDatabaseVersionQuery = QStringLiteral("UPDATE `DatabaseVersion` set `Version` = :version ");

        auto result = prepareQuery(d->mUpdateDatabaseVersionQuery, initDatabaseVersionQuery);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::manageNewDatabaseVersionInitRequests" << d->mUpdateDatabaseVersionQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::manageNewDatabaseVersionInitRequests" << d->mUpdateDatabaseVersionQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto  selectDatabaseVersionQuery = QStringLiteral("SELECT versionTable.`Version` FROM `DatabaseVersion` versionTable");

        auto result = prepareQuery(d->mSelectDatabaseVersionQuery, selectDatabaseVersionQuery);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::manageNewDatabaseVersionInitRequests" << d->mSelectDatabaseVersionQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::manageNewDatabaseVersionInitRequests" << d->mSelectDatabaseVersionQuery.lastError();

            Q_EMIT databaseError();
        }
    }
}

void DatabaseInterface::callUpgradeFunctionForVersion(DatabaseVersion databaseVersion)
{
    switch(databaseVersion)
    {
    case DatabaseInterface::V9:
        upgradeDatabaseV9();
        break;
    case DatabaseInterface::V11:
        upgradeDatabaseV11();
        break;
    case DatabaseInterface::V12:
        upgradeDatabaseV12();
        break;
    case DatabaseInterface::V13:
        upgradeDatabaseV13();
        break;
    case DatabaseInterface::V14:
        upgradeDatabaseV14();
        break;
    case DatabaseInterface::V15:
        upgradeDatabaseV15();
        break;
    case DatabaseInterface::V16:
        upgradeDatabaseV16();
        break;
    }
}

void DatabaseInterface::internalInsertOneTrack(const DataTypes::TrackDataType &oneTrack, const QHash<QString, QUrl> &covers)
{
    d->mSelectTracksMapping.bindValue(QStringLiteral(":fileName"), oneTrack.resourceURI());

    auto result = execQuery(d->mSelectTracksMapping);

    if (!result || !d->mSelectTracksMapping.isSelect() || !d->mSelectTracksMapping.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertTracksList" << d->mSelectTracksMapping.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertTracksList" << d->mSelectTracksMapping.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertTracksList" << d->mSelectTracksMapping.lastError();

        d->mSelectTracksMapping.finish();

        rollBackTransaction();
        Q_EMIT finishInsertingTracksList();
        return;
    }

    bool isNewTrack = !d->mSelectTracksMapping.next();

    if (isNewTrack) {
        insertTrackOrigin(oneTrack.resourceURI(), oneTrack.fileModificationTime(),
                          QDateTime::currentDateTime());
    } else if (!d->mSelectTracksMapping.record().value(0).isNull() && d->mSelectTracksMapping.record().value(0).toULongLong() != 0) {
        updateTrackOrigin(oneTrack.resourceURI(), oneTrack.fileModificationTime());
    }

    d->mSelectTracksMapping.finish();

    bool isInserted = false;

    const auto insertedTrackId = internalInsertTrack(oneTrack, covers, isInserted);

    if (isInserted && insertedTrackId != 0) {
        d->mInsertedTracks.insert(insertedTrackId);
    }
}

void DatabaseInterface::internalInsertOneRadio(const DataTypes::TrackDataType &oneTrack)
{
    QSqlQuery query = d->mUpdateRadioQuery;

    if (!oneTrack.hasDatabaseId()) {
        query = d->mInsertRadioQuery;
    }

    query.bindValue(QStringLiteral(":httpAddress"), oneTrack.resourceURI());
    query.bindValue(QStringLiteral(":radioId"), oneTrack.databaseId());
    query.bindValue(QStringLiteral(":title"), oneTrack.title());
    query.bindValue(QStringLiteral(":comment"), oneTrack.comment());
    query.bindValue(QStringLiteral(":trackRating"), oneTrack.rating());
    query.bindValue(QStringLiteral(":imageAddress"), oneTrack.albumCover());

    auto result = execQuery(query);

    if (!result || !query.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertTracksList" << query.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertTracksList" << query.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertTracksList" << query.lastError();
    } else {
        if (!oneTrack.hasDatabaseId()) {
            auto radio = internalOneRadioPartialData(internalRadioIdFromHttpAddress(oneTrack.resourceURI().toString()));

            Q_EMIT radioAdded(radio);
        } else {
            auto radio = internalOneRadioPartialData(oneTrack.databaseId());

            Q_EMIT radioModified(radio);
        }
    }

    query.finish();
}

void DatabaseInterface::initRequest()
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    {
        auto selectAlbumQueryText = QStringLiteral("SELECT "
                                                   "album.`ID`, "
                                                   "album.`Title`, "
                                                   "album.`ArtistName`, "
                                                   "album.`AlbumPath`, "
                                                   "album.`CoverFileName`, "
                                                   "("
                                                   "SELECT "
                                                   "COUNT(*) "
                                                   "FROM "
                                                   "`Tracks` tracks3 "
                                                   "WHERE "
                                                   "tracks3.`AlbumTitle` = album.`Title` AND "
                                                   "(tracks3.`AlbumArtistName` = album.`ArtistName` OR "
                                                   "(tracks3.`AlbumArtistName` IS NULL AND "
                                                   "album.`ArtistName` IS NULL"
                                                   ")"
                                                   ") AND "
                                                   "tracks3.`AlbumPath` = album.`AlbumPath` "
                                                   ") as `TracksCount`, "
                                                   "("
                                                   "SELECT "
                                                   "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                   "FROM "
                                                   "`Tracks` tracks2 "
                                                   "WHERE "
                                                   "tracks2.`AlbumTitle` = album.`Title` AND "
                                                   "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                   "(tracks2.`AlbumArtistName` IS NULL AND "
                                                   "album.`ArtistName` IS NULL"
                                                   ")"
                                                   ") AND "
                                                   "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                   ") as `IsSingleDiscAlbum`, "
                                                   "COUNT(DISTINCT tracks.`ArtistName`) as ArtistsCount, "
                                                   "GROUP_CONCAT(tracks.`ArtistName`, ', ') as AllArtists, "
                                                   "MAX(tracks.`Rating`) as HighestRating, "
                                                   "GROUP_CONCAT(genres.`Name`, ', ') as AllGenres, "
                                                   "( "
                                                   "SELECT tracksCover.`FileName` "
                                                   "FROM "
                                                   "`Tracks` tracksCover "
                                                   "WHERE "
                                                   "tracksCover.`HasEmbeddedCover` = 1 AND "
                                                   "tracksCover.`AlbumTitle` = album.`Title` AND "
                                                   "(tracksCover.`AlbumArtistName` = album.`ArtistName` OR "
                                                   "(tracksCover.`AlbumArtistName` IS NULL AND "
                                                   "album.`ArtistName` IS NULL "
                                                   ") "
                                                   ") AND "
                                                   "tracksCover.`AlbumPath` = album.`AlbumPath` "
                                                   ") as EmbeddedCover "
                                                   "FROM "
                                                   "`Albums` album LEFT JOIN "
                                                   "`Tracks` tracks ON "
                                                   "tracks.`AlbumTitle` = album.`Title` AND "
                                                   "("
                                                   "tracks.`AlbumArtistName` = album.`ArtistName` OR "
                                                   "("
                                                   "tracks.`AlbumArtistName` IS NULL AND "
                                                   "album.`ArtistName` IS NULL"
                                                   ")"
                                                   ") AND "
                                                   "tracks.`AlbumPath` = album.`AlbumPath`"
                                                   "LEFT JOIN "
                                                   "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                   "WHERE "
                                                   "album.`ID` = :albumId "
                                                   "GROUP BY album.`ID`");

        auto result = prepareQuery(d->mSelectAlbumQuery, selectAlbumQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllGenresText = QStringLiteral("SELECT "
                                                  "genre.`ID`, "
                                                  "genre.`Name` "
                                                  "FROM `Genre` genre "
                                                  "ORDER BY genre.`Name` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllGenresQuery, selectAllGenresText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllGenresQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllGenresQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllAlbumsText = QStringLiteral("SELECT "
                                                  "album.`ID`, "
                                                  "album.`Title`, "
                                                  "album.`ArtistName` as SecondaryText, "
                                                  "album.`CoverFileName`, "
                                                  "album.`ArtistName`, "
                                                  "GROUP_CONCAT(tracks.`Year`, ', ') as Year, "
                                                  "COUNT(DISTINCT tracks.`ArtistName`) as ArtistsCount, "
                                                  "GROUP_CONCAT(tracks.`ArtistName`, ', ') as AllArtists, "
                                                  "MAX(tracks.`Rating`) as HighestRating, "
                                                  "GROUP_CONCAT(genres.`Name`, ', ') as AllGenres, "
                                                  "("
                                                  "SELECT "
                                                  "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                  "FROM "
                                                  "`Tracks` tracks2 "
                                                  "WHERE "
                                                  "tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks2.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                  ") as `IsSingleDiscAlbum`, "
                                                  "( "
                                                  "SELECT tracksCover.`FileName` "
                                                  "FROM "
                                                  "`Tracks` tracksCover "
                                                  "WHERE "
                                                  "tracksCover.`HasEmbeddedCover` = 1 AND "
                                                  "tracksCover.`AlbumTitle` = album.`Title` AND "
                                                  "(tracksCover.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracksCover.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL "
                                                  ") "
                                                  ") AND "
                                                  "tracksCover.`AlbumPath` = album.`AlbumPath` "
                                                  ") as EmbeddedCover "
                                                  "FROM "
                                                  "`Albums` album, "
                                                  "`Tracks` tracks LEFT JOIN "
                                                  "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                  "WHERE "
                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ") "
                                                  ") AND "
                                                  "tracks.`AlbumPath` = album.`AlbumPath` "
                                                  "GROUP BY album.`ID`, album.`Title`, album.`AlbumPath` "
                                                  "ORDER BY album.`Title` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllAlbumsShortQuery, selectAllAlbumsText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsShortQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsShortQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllAlbumsText = QStringLiteral("SELECT "
                                                  "album.`ID`, "
                                                  "album.`Title`, "
                                                  "album.`ArtistName` as SecondaryText, "
                                                  "album.`CoverFileName`, "
                                                  "album.`ArtistName`, "
                                                  "GROUP_CONCAT(tracks.`Year`, ', ') as Year, "
                                                  "COUNT(DISTINCT tracks.`ArtistName`) as ArtistsCount, "
                                                  "GROUP_CONCAT(tracks.`ArtistName`, ', ') as AllArtists, "
                                                  "MAX(tracks.`Rating`) as HighestRating, "
                                                  "GROUP_CONCAT(genres.`Name`, ', ') as AllGenres, "
                                                  "("
                                                  "SELECT "
                                                  "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                  "FROM "
                                                  "`Tracks` tracks2 "
                                                  "WHERE "
                                                  "tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks2.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                  ") as `IsSingleDiscAlbum`, "
                                                  "( "
                                                  "SELECT tracksCover.`FileName` "
                                                  "FROM "
                                                  "`Tracks` tracksCover "
                                                  "WHERE "
                                                  "tracksCover.`HasEmbeddedCover` = 1 AND "
                                                  "tracksCover.`AlbumTitle` = album.`Title` AND "
                                                  "(tracksCover.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracksCover.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL "
                                                  ") "
                                                  ") AND "
                                                  "tracksCover.`AlbumPath` = album.`AlbumPath` "
                                                  ") as EmbeddedCover "
                                                  "FROM "
                                                  "`Albums` album, "
                                                  "`Tracks` tracks LEFT JOIN "
                                                  "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                  "WHERE "
                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks.`AlbumPath` = album.`AlbumPath` AND "
                                                  "EXISTS ("
                                                  "  SELECT tracks2.`Genre` "
                                                  "  FROM "
                                                  "  `Tracks` tracks2, "
                                                  "  `Genre` genre2 "
                                                  "  WHERE "
                                                  "  tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "  (tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "   (tracks2.`AlbumArtistName` IS NULL AND "
                                                  "    album.`ArtistName` IS NULL"
                                                  "   )"
                                                  "  ) AND "
                                                  "  tracks2.`Genre` = genre2.`Name` AND "
                                                  "  genre2.`Name` = :genreFilter AND "
                                                  "  (tracks2.`ArtistName` = :artistFilter OR tracks2.`AlbumArtistName` = :artistFilter) "
                                                  ") "
                                                  "GROUP BY album.`ID`, album.`Title`, album.`AlbumPath` "
                                                  "ORDER BY album.`Title` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllAlbumsShortWithGenreArtistFilterQuery, selectAllAlbumsText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsShortWithGenreArtistFilterQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsShortWithGenreArtistFilterQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllAlbumsText = QStringLiteral("SELECT "
                                                  "album.`ID`, "
                                                  "album.`Title`, "
                                                  "album.`ArtistName` as SecondaryText, "
                                                  "album.`CoverFileName`, "
                                                  "album.`ArtistName`, "
                                                  "GROUP_CONCAT(tracks.`Year`, ', ') as Year, "
                                                  "COUNT(DISTINCT tracks.`ArtistName`) as ArtistsCount, "
                                                  "GROUP_CONCAT(tracks.`ArtistName`, ', ') as AllArtists, "
                                                  "MAX(tracks.`Rating`) as HighestRating, "
                                                  "GROUP_CONCAT(genres.`Name`, ', ') as AllGenres, "
                                                  "("
                                                  "SELECT "
                                                  "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                  "FROM "
                                                  "`Tracks` tracks2 "
                                                  "WHERE "
                                                  "tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks2.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                  ") as `IsSingleDiscAlbum`, "
                                                  "( "
                                                  "SELECT tracksCover.`FileName` "
                                                  "FROM "
                                                  "`Tracks` tracksCover "
                                                  "WHERE "
                                                  "tracksCover.`HasEmbeddedCover` = 1 AND "
                                                  "tracksCover.`AlbumTitle` = album.`Title` AND "
                                                  "(tracksCover.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracksCover.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL "
                                                  ") "
                                                  ") AND "
                                                  "tracksCover.`AlbumPath` = album.`AlbumPath` "
                                                  ") as EmbeddedCover "
                                                  "FROM "
                                                  "`Albums` album, "
                                                  "`Tracks` tracks LEFT JOIN "
                                                  "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                  "WHERE "
                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks.`AlbumPath` = album.`AlbumPath` AND "
                                                  "EXISTS ("
                                                  "  SELECT tracks2.`Genre` "
                                                  "  FROM "
                                                  "  `Tracks` tracks2 "
                                                  "  WHERE "
                                                  "  tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "  ( "
                                                  "    tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "    ( "
                                                  "      tracks2.`AlbumArtistName` IS NULL AND "
                                                  "      album.`ArtistName` IS NULL "
                                                  "    )"
                                                  "  ) AND "
                                                  "  (tracks2.`ArtistName` = :artistFilter OR tracks2.`AlbumArtistName` = :artistFilter) "
                                                  ") "
                                                  "GROUP BY album.`ID`, album.`Title`, album.`AlbumPath` "
                                                  "ORDER BY album.`Title` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllAlbumsShortWithArtistFilterQuery, selectAllAlbumsText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsShortWithArtistFilterQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsShortWithArtistFilterQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllArtistsWithFilterText = QStringLiteral("SELECT artists.`ID`, "
                                                             "artists.`Name`, "
                                                             "GROUP_CONCAT(genres.`Name`, ', ') as AllGenres "
                                                             "FROM `Artists` artists  LEFT JOIN "
                                                             "`Tracks` tracks ON artists.`Name` = tracks.`ArtistName` LEFT JOIN "
                                                             "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                             "GROUP BY artists.`ID` "
                                                             "ORDER BY artists.`Name` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllArtistsQuery, selectAllArtistsWithFilterText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllArtistsQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllArtistsQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllArtistsWithGenreFilterText = QStringLiteral("SELECT artists.`ID`, "
                                                                  "artists.`Name`, "
                                                                  "GROUP_CONCAT(genres.`Name`, ', ') as AllGenres "
                                                                  "FROM `Artists` artists  LEFT JOIN "
                                                                  "`Tracks` tracks ON (tracks.`ArtistName` = artists.`Name` OR tracks.`AlbumArtistName` = artists.`Name`) LEFT JOIN "
                                                                  "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                                  "WHERE "
                                                                  "EXISTS ("
                                                                  "  SELECT tracks2.`Genre` "
                                                                  "  FROM "
                                                                  "  `Tracks` tracks2, "
                                                                  "  `Genre` genre2 "
                                                                  "  WHERE "
                                                                  "  (tracks2.`ArtistName` = artists.`Name` OR tracks2.`AlbumArtistName` = artists.`Name`) AND "
                                                                  "  tracks2.`Genre` = genre2.`Name` AND "
                                                                  "  genre2.`Name` = :genreFilter "
                                                                  ") "
                                                                  "GROUP BY artists.`ID` "
                                                                  "ORDER BY artists.`Name` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllArtistsWithGenreFilterQuery, selectAllArtistsWithGenreFilterText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllArtistsWithGenreFilterQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllArtistsWithGenreFilterQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto artistMatchGenreText = QStringLiteral("SELECT artists.`ID` "
                                                   "FROM `Artists` artists  LEFT JOIN "
                                                   "`Tracks` tracks ON (tracks.`ArtistName` = artists.`Name` OR tracks.`AlbumArtistName` = artists.`Name`) LEFT JOIN "
                                                   "`Genre` genres ON tracks.`Genre` = genres.`Name` "
                                                   "WHERE "
                                                   "EXISTS ("
                                                   "  SELECT tracks2.`Genre` "
                                                   "  FROM "
                                                   "  `Tracks` tracks2, "
                                                   "  `Genre` genre2 "
                                                   "  WHERE "
                                                   "  (tracks2.`ArtistName` = artists.`Name` OR tracks2.`AlbumArtistName` = artists.`Name`) AND "
                                                   "  tracks2.`Genre` = genre2.`Name` AND "
                                                   "  genre2.`Name` = :genreFilter "
                                                   ") AND "
                                                   "artists.`ID` = :databaseId");

        auto result = prepareQuery(d->mArtistMatchGenreQuery, artistMatchGenreText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mArtistMatchGenreQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mArtistMatchGenreQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllComposersWithFilterText = QStringLiteral("SELECT `ID`, "
                                                               "`Name` "
                                                               "FROM `Artists` "
                                                               "ORDER BY `Name` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllComposersQuery, selectAllComposersWithFilterText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllComposersQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllComposersQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllLyricistsWithFilterText = QStringLiteral("SELECT `ID`, "
                                                               "`Name` "
                                                               "FROM `Lyricist` "
                                                               "ORDER BY `Name` COLLATE NOCASE");

        auto result = prepareQuery(d->mSelectAllLyricistsQuery, selectAllLyricistsWithFilterText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllLyricistsQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllLyricistsQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllTracksText = QStringLiteral("SELECT "
                                                  "tracks.`ID`, "
                                                  "tracks.`Title`, "
                                                  "album.`ID`, "
                                                  "tracks.`ArtistName`, "
                                                  "( "
                                                  "SELECT "
                                                  "COUNT(DISTINCT tracksFromAlbum1.`ArtistName`) "
                                                  "FROM "
                                                  "`Tracks` tracksFromAlbum1 "
                                                  "WHERE "
                                                  "tracksFromAlbum1.`AlbumTitle` = album.`Title` AND "
                                                  "(tracksFromAlbum1.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracksFromAlbum1.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL "
                                                  ") "
                                                  ") AND "
                                                  "tracksFromAlbum1.`AlbumPath` = album.`AlbumPath` "
                                                  ") AS ArtistsCount, "
                                                  "( "
                                                  "SELECT "
                                                  "GROUP_CONCAT(tracksFromAlbum2.`ArtistName`) "
                                                  "FROM "
                                                  "`Tracks` tracksFromAlbum2 "
                                                  "WHERE "
                                                  "tracksFromAlbum2.`AlbumTitle` = album.`Title` AND "
                                                  "(tracksFromAlbum2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracksFromAlbum2.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL "
                                                  ") "
                                                  ") AND "
                                                  "tracksFromAlbum2.`AlbumPath` = album.`AlbumPath` "
                                                  ") AS AllArtists, "
                                                  "tracks.`AlbumArtistName`, "
                                                  "tracksMapping.`FileName`, "
                                                  "tracksMapping.`FileModifiedTime`, "
                                                  "tracks.`TrackNumber`, "
                                                  "tracks.`DiscNumber`, "
                                                  "tracks.`Duration`, "
                                                  "tracks.`AlbumTitle`, "
                                                  "tracks.`Rating`, "
                                                  "album.`CoverFileName`, "
                                                  "("
                                                  "SELECT "
                                                  "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                  "FROM "
                                                  "`Tracks` tracks2 "
                                                  "WHERE "
                                                  "tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks2.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                  ") as `IsSingleDiscAlbum`, "
                                                  "trackGenre.`Name`, "
                                                  "trackComposer.`Name`, "
                                                  "trackLyricist.`Name`, "
                                                  "tracks.`Comment`, "
                                                  "tracks.`Year`, "
                                                  "tracks.`Channels`, "
                                                  "tracks.`BitRate`, "
                                                  "tracks.`SampleRate`, "
                                                  "tracks.`HasEmbeddedCover`, "
                                                  "tracksMapping.`ImportDate`, "
                                                  "tracksMapping.`FirstPlayDate`, "
                                                  "tracksMapping.`LastPlayDate`, "
                                                  "tracksMapping.`PlayCounter`, "
                                                  "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency, "
                                                  "( "
                                                  "SELECT tracksCover.`FileName` "
                                                  "FROM "
                                                  "`Tracks` tracksCover "
                                                  "WHERE "
                                                  "tracksCover.`HasEmbeddedCover` = 1 AND "
                                                  "tracksCover.`AlbumTitle` = album.`Title` AND "
                                                  "(tracksCover.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracksCover.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL "
                                                  ") "
                                                  ") AND "
                                                  "tracksCover.`AlbumPath` = album.`AlbumPath` "
                                                  ") as EmbeddedCover "
                                                  "FROM "
                                                  "`TracksData` tracksMapping "
                                                  "LEFT JOIN "
                                                  "`Tracks` tracks "
                                                  "ON "
                                                  "tracksMapping.`FileName` = tracks.`FileName` "
                                                  "LEFT JOIN "
                                                  "`Albums` album "
                                                  "ON "
                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                  "tracks.`AlbumPath` = album.`AlbumPath` "
                                                  "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                  "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                  "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                  "WHERE "
                                                  "tracks.`Title` IS NULL OR "
                                                  "tracks.`Priority` = ("
                                                  "     SELECT "
                                                  "     MIN(`Priority`) "
                                                  "     FROM "
                                                  "     `Tracks` tracks2 "
                                                  "     WHERE "
                                                  "     tracks.`Title` = tracks2.`Title` AND "
                                                  "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                  "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                  "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                  "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                  ")"
                                                  "");

        auto result = prepareQuery(d->mSelectAllTracksQuery, selectAllTracksText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllTracksQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllTracksQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllRadiosText = QStringLiteral("SELECT "
                                                  "radios.`ID`, "
                                                  "radios.`Title`, "
                                                  "radios.`HttpAddress`, "
                                                  "radios.`ImageAddress`, "
                                                  "radios.`Rating`, "
                                                  "trackGenre.`Name`, "
                                                  "radios.`Comment` "
                                                  "FROM "
                                                  "`Radios` radios "
                                                  "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = radios.`Genre` "
                                                  "");

        auto result = prepareQuery(d->mSelectAllRadiosQuery, selectAllRadiosText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllRadiosQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllRadiosQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllTracksText = QStringLiteral("SELECT "
                                                  "tracks.`ID`, "
                                                  "tracks.`Title`, "
                                                  "album.`ID`, "
                                                  "tracks.`ArtistName`, "
                                                  "( "
                                                  "SELECT "
                                                  "COUNT(DISTINCT tracksFromAlbum1.`ArtistName`) "
                                                  "FROM "
                                                  "`Tracks` tracksFromAlbum1 "
                                                  "WHERE "
                                                  "tracksFromAlbum1.`AlbumTitle` = album.`Title` AND "
                                                  "(tracksFromAlbum1.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracksFromAlbum1.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL "
                                                  ") "
                                                  ") AND "
                                                  "tracksFromAlbum1.`AlbumPath` = album.`AlbumPath` "
                                                  ") AS ArtistsCount, "
                                                  "( "
                                                  "SELECT "
                                                  "GROUP_CONCAT(tracksFromAlbum2.`ArtistName`) "
                                                  "FROM "
                                                  "`Tracks` tracksFromAlbum2 "
                                                  "WHERE "
                                                  "tracksFromAlbum2.`AlbumTitle` = album.`Title` AND "
                                                  "(tracksFromAlbum2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracksFromAlbum2.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL "
                                                  ") "
                                                  ") AND "
                                                  "tracksFromAlbum2.`AlbumPath` = album.`AlbumPath` "
                                                  ") AS AllArtists, "
                                                  "tracks.`AlbumArtistName`, "
                                                  "tracksMapping.`FileName`, "
                                                  "tracksMapping.`FileModifiedTime`, "
                                                  "tracks.`TrackNumber`, "
                                                  "tracks.`DiscNumber`, "
                                                  "tracks.`Duration`, "
                                                  "tracks.`AlbumTitle`, "
                                                  "tracks.`Rating`, "
                                                  "album.`CoverFileName`, "
                                                  "("
                                                  "SELECT "
                                                  "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                  "FROM "
                                                  "`Tracks` tracks2 "
                                                  "WHERE "
                                                  "tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks2.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                  ") as `IsSingleDiscAlbum`, "
                                                  "trackGenre.`Name`, "
                                                  "trackComposer.`Name`, "
                                                  "trackLyricist.`Name`, "
                                                  "tracks.`Comment`, "
                                                  "tracks.`Year`, "
                                                  "tracks.`Channels`, "
                                                  "tracks.`BitRate`, "
                                                  "tracks.`SampleRate`, "
                                                  "tracks.`HasEmbeddedCover`, "
                                                  "tracksMapping.`ImportDate`, "
                                                  "tracksMapping.`FirstPlayDate`, "
                                                  "tracksMapping.`LastPlayDate`, "
                                                  "tracksMapping.`PlayCounter`, "
                                                  "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency, "
                                                  "( "
                                                  "SELECT tracksCover.`FileName` "
                                                  "FROM "
                                                  "`Tracks` tracksCover "
                                                  "WHERE "
                                                  "tracksCover.`HasEmbeddedCover` = 1 AND "
                                                  "tracksCover.`AlbumTitle` = album.`Title` AND "
                                                  "(tracksCover.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracksCover.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL "
                                                  ") "
                                                  ") AND "
                                                  "tracksCover.`AlbumPath` = album.`AlbumPath` "
                                                  ") as EmbeddedCover "
                                                  "FROM "
                                                  "`Tracks` tracks, "
                                                  "`TracksData` tracksMapping "
                                                  "LEFT JOIN "
                                                  "`Albums` album "
                                                  "ON "
                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                  "tracks.`AlbumPath` = album.`AlbumPath` "
                                                  "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                  "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                  "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                  "WHERE "
                                                  "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                  "tracksMapping.`PlayCounter` > 0 AND "
                                                  "tracks.`Priority` = ("
                                                  "     SELECT "
                                                  "     MIN(`Priority`) "
                                                  "     FROM "
                                                  "     `Tracks` tracks2 "
                                                  "     WHERE "
                                                  "     tracks.`Title` = tracks2.`Title` AND "
                                                  "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                  "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                  "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                  "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                  ")"
                                                  "ORDER BY tracksMapping.`LastPlayDate` DESC "
                                                  "LIMIT :maximumResults");

        auto result = prepareQuery(d->mSelectAllRecentlyPlayedTracksQuery, selectAllTracksText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllRecentlyPlayedTracksQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllRecentlyPlayedTracksQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllTracksText = QStringLiteral("SELECT "
                                                  "tracks.`ID`, "
                                                  "tracks.`Title`, "
                                                  "album.`ID`, "
                                                  "tracks.`ArtistName`, "
                                                  "( "
                                                  "SELECT "
                                                  "COUNT(DISTINCT tracksFromAlbum1.`ArtistName`) "
                                                  "FROM "
                                                  "`Tracks` tracksFromAlbum1 "
                                                  "WHERE "
                                                  "tracksFromAlbum1.`AlbumTitle` = album.`Title` AND "
                                                  "(tracksFromAlbum1.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracksFromAlbum1.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL "
                                                  ") "
                                                  ") AND "
                                                  "tracksFromAlbum1.`AlbumPath` = album.`AlbumPath` "
                                                  ") AS ArtistsCount, "
                                                  "( "
                                                  "SELECT "
                                                  "GROUP_CONCAT(tracksFromAlbum2.`ArtistName`) "
                                                  "FROM "
                                                  "`Tracks` tracksFromAlbum2 "
                                                  "WHERE "
                                                  "tracksFromAlbum2.`AlbumTitle` = album.`Title` AND "
                                                  "(tracksFromAlbum2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracksFromAlbum2.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL "
                                                  ") "
                                                  ") AND "
                                                  "tracksFromAlbum2.`AlbumPath` = album.`AlbumPath` "
                                                  ") AS AllArtists, "
                                                  "tracks.`AlbumArtistName`, "
                                                  "tracksMapping.`FileName`, "
                                                  "tracksMapping.`FileModifiedTime`, "
                                                  "tracks.`TrackNumber`, "
                                                  "tracks.`DiscNumber`, "
                                                  "tracks.`Duration`, "
                                                  "tracks.`AlbumTitle`, "
                                                  "tracks.`Rating`, "
                                                  "album.`CoverFileName`, "
                                                  "("
                                                  "SELECT "
                                                  "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                  "FROM "
                                                  "`Tracks` tracks2 "
                                                  "WHERE "
                                                  "tracks2.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracks2.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL"
                                                  ")"
                                                  ") AND "
                                                  "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                  ") as `IsSingleDiscAlbum`, "
                                                  "trackGenre.`Name`, "
                                                  "trackComposer.`Name`, "
                                                  "trackLyricist.`Name`, "
                                                  "tracks.`Comment`, "
                                                  "tracks.`Year`, "
                                                  "tracks.`Channels`, "
                                                  "tracks.`BitRate`, "
                                                  "tracks.`SampleRate`, "
                                                  "tracks.`HasEmbeddedCover`, "
                                                  "tracksMapping.`ImportDate`, "
                                                  "tracksMapping.`FirstPlayDate`, "
                                                  "tracksMapping.`LastPlayDate`, "
                                                  "tracksMapping.`PlayCounter`, "
                                                  "CAST(tracksMapping.`PlayCounter` AS REAL) / ((CAST(strftime('%s','now') as INTEGER) - CAST(tracksMapping.`FirstPlayDate` / 1000 as INTEGER)) / CAST(1000 AS REAL)) as PlayFrequency, "
                                                  "( "
                                                  "SELECT tracksCover.`FileName` "
                                                  "FROM "
                                                  "`Tracks` tracksCover "
                                                  "WHERE "
                                                  "tracksCover.`HasEmbeddedCover` = 1 AND "
                                                  "tracksCover.`AlbumTitle` = album.`Title` AND "
                                                  "(tracksCover.`AlbumArtistName` = album.`ArtistName` OR "
                                                  "(tracksCover.`AlbumArtistName` IS NULL AND "
                                                  "album.`ArtistName` IS NULL "
                                                  ") "
                                                  ") AND "
                                                  "tracksCover.`AlbumPath` = album.`AlbumPath` "
                                                  ") as EmbeddedCover "
                                                  "FROM "
                                                  "`Tracks` tracks, "
                                                  "`TracksData` tracksMapping "
                                                  "LEFT JOIN "
                                                  "`Albums` album "
                                                  "ON "
                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                  "tracks.`AlbumPath` = album.`AlbumPath` "
                                                  "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                  "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                  "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                  "WHERE "
                                                  "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                  "tracksMapping.`PlayCounter` > 0 AND "
                                                  "tracks.`Priority` = ("
                                                  "     SELECT "
                                                  "     MIN(`Priority`) "
                                                  "     FROM "
                                                  "     `Tracks` tracks2 "
                                                  "     WHERE "
                                                  "     tracks.`Title` = tracks2.`Title` AND "
                                                  "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                  "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                  "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                  "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                  ")"
                                                  "ORDER BY CAST(tracksMapping.`PlayCounter` AS REAL) / ((CAST(strftime('%s','now') as INTEGER) - CAST(tracksMapping.`FirstPlayDate` / 1000 as INTEGER)) / CAST(1000 AS REAL)) DESC "
                                                  "LIMIT :maximumResults");

        auto result = prepareQuery(d->mSelectAllFrequentlyPlayedTracksQuery, selectAllTracksText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllFrequentlyPlayedTracksQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllFrequentlyPlayedTracksQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto clearAlbumsTableText = QStringLiteral("DELETE FROM `Albums`");

        auto result = prepareQuery(d->mClearAlbumsTable, clearAlbumsTableText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearAlbumsTable.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearAlbumsTable.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto clearArtistsTableText = QStringLiteral("DELETE FROM `Artists`");

        auto result = prepareQuery(d->mClearArtistsTable, clearArtistsTableText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearArtistsTable.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearArtistsTable.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto clearComposerTableText = QStringLiteral("DELETE FROM `Composer`");

        auto result = prepareQuery(d->mClearComposerTable, clearComposerTableText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearComposerTable.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearComposerTable.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto clearGenreTableText = QStringLiteral("DELETE FROM `Genre`");

        auto result = prepareQuery(d->mClearGenreTable, clearGenreTableText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearGenreTable.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearGenreTable.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto clearLyricistTableText = QStringLiteral("DELETE FROM `Lyricist`");

        auto result = prepareQuery(d->mClearLyricistTable, clearLyricistTableText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearLyricistTable.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearLyricistTable.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto clearTracksDataTableText = QStringLiteral("DELETE FROM `TracksData`");

        auto result = prepareQuery(d->mClearTracksDataTable, clearTracksDataTableText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearTracksDataTable.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearTracksDataTable.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto clearTracksTableText = QStringLiteral("DELETE FROM `Tracks`");

        auto result = prepareQuery(d->mClearTracksTable, clearTracksTableText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearTracksTable.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mClearTracksTable.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllTracksShortText = QStringLiteral("SELECT "
                                                       "tracks.`ID`, "
                                                       "tracks.`Title`, "
                                                       "tracks.`ArtistName`, "
                                                       "tracks.`AlbumTitle`, "
                                                       "( "
                                                       "SELECT "
                                                       "COUNT(DISTINCT tracksFromAlbum1.`ArtistName`) "
                                                       "FROM "
                                                       "`Tracks` tracksFromAlbum1 "
                                                       "WHERE "
                                                       "tracksFromAlbum1.`AlbumTitle` = album.`Title` AND "
                                                       "(tracksFromAlbum1.`AlbumArtistName` = album.`ArtistName` OR "
                                                       "(tracksFromAlbum1.`AlbumArtistName` IS NULL AND "
                                                       "album.`ArtistName` IS NULL "
                                                       ") "
                                                       ") AND "
                                                       "tracksFromAlbum1.`AlbumPath` = album.`AlbumPath` "
                                                       ") AS ArtistsCount, "
                                                       "( "
                                                       "SELECT "
                                                       "GROUP_CONCAT(tracksFromAlbum2.`ArtistName`) "
                                                       "FROM "
                                                       "`Tracks` tracksFromAlbum2 "
                                                       "WHERE "
                                                       "tracksFromAlbum2.`AlbumTitle` = album.`Title` AND "
                                                       "(tracksFromAlbum2.`AlbumArtistName` = album.`ArtistName` OR "
                                                       "(tracksFromAlbum2.`AlbumArtistName` IS NULL AND "
                                                       "album.`ArtistName` IS NULL "
                                                       ") "
                                                       ") AND "
                                                       "tracksFromAlbum2.`AlbumPath` = album.`AlbumPath` "
                                                       ") AS AllArtists, "
                                                       "tracks.`AlbumArtistName`, "
                                                       "tracks.`Duration`, "
                                                       "album.`CoverFileName`, "
                                                       "tracks.`TrackNumber`, "
                                                       "tracks.`DiscNumber`, "
                                                       "tracks.`Rating` "
                                                       "FROM "
                                                       "`Tracks` tracks "
                                                       "LEFT JOIN "
                                                       "`Albums` album "
                                                       "ON "
                                                       "tracks.`AlbumTitle` = album.`Title` AND "
                                                       "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                       "tracks.`AlbumPath` = album.`AlbumPath` "
                                                       "");

        auto result = prepareQuery(d->mSelectAllTracksShortQuery, selectAllTracksShortText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllTracksShortQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllTracksShortQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectArtistByNameText = QStringLiteral("SELECT `ID`, "
                                                     "`Name` "
                                                     "FROM `Artists` "
                                                     "WHERE "
                                                     "`Name` = :name");

        auto result = prepareQuery(d->mSelectArtistByNameQuery, selectArtistByNameText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectArtistByNameQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectArtistByNameQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectComposerByNameText = QStringLiteral("SELECT `ID`, "
                                                       "`Name` "
                                                       "FROM `Composer` "
                                                       "WHERE "
                                                       "`Name` = :name");

        auto result = prepareQuery(d->mSelectComposerByNameQuery, selectComposerByNameText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectComposerByNameQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectComposerByNameQuery.lastError();
        }
    }

    {
        auto selectLyricistByNameText = QStringLiteral("SELECT `ID`, "
                                                       "`Name` "
                                                       "FROM `Lyricist` "
                                                       "WHERE "
                                                       "`Name` = :name");

        auto result = prepareQuery(d->mSelectLyricistByNameQuery, selectLyricistByNameText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectLyricistByNameQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectLyricistByNameQuery.lastError();
        }
    }

    {
        auto selectGenreByNameText = QStringLiteral("SELECT `ID`, "
                                                    "`Name` "
                                                    "FROM `Genre` "
                                                    "WHERE "
                                                    "`Name` = :name");

        auto result = prepareQuery(d->mSelectGenreByNameQuery, selectGenreByNameText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreByNameQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreByNameQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertArtistsText = QStringLiteral("INSERT INTO `Artists` (`ID`, `Name`) "
                                                "VALUES (:artistId, :name)");

        auto result = prepareQuery(d->mInsertArtistsQuery, insertArtistsText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertArtistsQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertArtistsQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertGenreText = QStringLiteral("INSERT INTO `Genre` (`ID`, `Name`) "
                                              "VALUES (:genreId, :name)");

        auto result = prepareQuery(d->mInsertGenreQuery, insertGenreText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertGenreQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertGenreQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertComposerText = QStringLiteral("INSERT INTO `Composer` (`ID`, `Name`) "
                                                 "VALUES (:composerId, :name)");

        auto result = prepareQuery(d->mInsertComposerQuery, insertComposerText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertComposerQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertComposerQuery.lastError();
        }
    }

    {
        auto insertLyricistText = QStringLiteral("INSERT INTO `Lyricist` (`ID`, `Name`) "
                                                 "VALUES (:lyricistId, :name)");

        auto result = prepareQuery(d->mInsertLyricistQuery, insertLyricistText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertLyricistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertLyricistQuery.lastError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.`ID`, "
                                                   "tracks.`Title`, "
                                                   "album.`ID`, "
                                                   "tracks.`ArtistName`, "
                                                   "( "
                                                   "SELECT "
                                                   "COUNT(DISTINCT tracksFromAlbum1.`ArtistName`) "
                                                   "FROM "
                                                   "`Tracks` tracksFromAlbum1 "
                                                   "WHERE "
                                                   "tracksFromAlbum1.`AlbumTitle` = album.`Title` AND "
                                                   "(tracksFromAlbum1.`AlbumArtistName` = album.`ArtistName` OR "
                                                   "(tracksFromAlbum1.`AlbumArtistName` IS NULL AND "
                                                   "album.`ArtistName` IS NULL "
                                                   ") "
                                                   ") AND "
                                                   "tracksFromAlbum1.`AlbumPath` = album.`AlbumPath` "
                                                   ") AS ArtistsCount, "
                                                   "( "
                                                   "SELECT "
                                                   "GROUP_CONCAT(tracksFromAlbum2.`ArtistName`) "
                                                   "FROM "
                                                   "`Tracks` tracksFromAlbum2 "
                                                   "WHERE "
                                                   "tracksFromAlbum2.`AlbumTitle` = album.`Title` AND "
                                                   "(tracksFromAlbum2.`AlbumArtistName` = album.`ArtistName` OR "
                                                   "(tracksFromAlbum2.`AlbumArtistName` IS NULL AND "
                                                   "album.`ArtistName` IS NULL "
                                                   ") "
                                                   ") AND "
                                                   "tracksFromAlbum2.`AlbumPath` = album.`AlbumPath` "
                                                   ") AS AllArtists, "
                                                   "tracks.`AlbumArtistName`, "
                                                   "tracksMapping.`FileName`, "
                                                   "tracksMapping.`FileModifiedTime`, "
                                                   "tracks.`TrackNumber`, "
                                                   "tracks.`DiscNumber`, "
                                                   "tracks.`Duration`, "
                                                   "tracks.`AlbumTitle`, "
                                                   "tracks.`Rating`, "
                                                   "album.`CoverFileName`, "
                                                   "("
                                                   "SELECT "
                                                   "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                   "FROM "
                                                   "`Tracks` tracks2 "
                                                   "WHERE "
                                                   "tracks2.`AlbumTitle` = album.`Title` AND "
                                                   "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                   "(tracks2.`AlbumArtistName` IS NULL AND "
                                                   "album.`ArtistName` IS NULL"
                                                   ")"
                                                   ") AND "
                                                   "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                   ") as `IsSingleDiscAlbum`, "
                                                   "trackGenre.`Name`, "
                                                   "trackComposer.`Name`, "
                                                   "trackLyricist.`Name`, "
                                                   "tracks.`Comment`, "
                                                   "tracks.`Year`, "
                                                   "tracks.`Channels`, "
                                                   "tracks.`BitRate`, "
                                                   "tracks.`SampleRate`, "
                                                   "tracks.`HasEmbeddedCover`, "
                                                   "tracksMapping.`ImportDate`, "
                                                   "tracksMapping.`FirstPlayDate`, "
                                                   "tracksMapping.`LastPlayDate`, "
                                                   "tracksMapping.`PlayCounter`, "
                                                   "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency, "
                                                   "( "
                                                   "SELECT tracksCover.`FileName` "
                                                   "FROM "
                                                   "`Tracks` tracksCover "
                                                   "WHERE "
                                                   "tracksCover.`HasEmbeddedCover` = 1 AND "
                                                   "tracksCover.`AlbumTitle` = album.`Title` AND "
                                                   "(tracksCover.`AlbumArtistName` = album.`ArtistName` OR "
                                                   "(tracksCover.`AlbumArtistName` IS NULL AND "
                                                   "album.`ArtistName` IS NULL "
                                                   ") "
                                                   ") AND "
                                                   "tracksCover.`AlbumPath` = album.`AlbumPath` "
                                                   ") as EmbeddedCover "
                                                   "FROM "
                                                   "`Tracks` tracks, "
                                                   "`TracksData` tracksMapping "
                                                   "LEFT JOIN "
                                                   "`Albums` album "
                                                   "ON "
                                                   "album.`ID` = :albumId AND "
                                                   "tracks.`AlbumTitle` = album.`Title` AND "
                                                   "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                   "tracks.`AlbumPath` = album.`AlbumPath` "
                                                   "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                   "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                   "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                   "WHERE "
                                                   "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                   "album.`ID` = :albumId AND "
                                                   "tracks.`Priority` = ("
                                                   "     SELECT "
                                                   "     MIN(`Priority`) "
                                                   "     FROM "
                                                   "     `Tracks` tracks2 "
                                                   "     WHERE "
                                                   "     tracks.`Title` = tracks2.`Title` AND "
                                                   "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                   "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                   "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                   "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                   ")"
                                                   "ORDER BY tracks.`DiscNumber` ASC, "
                                                   "tracks.`TrackNumber` ASC");

        auto result = prepareQuery(d->mSelectTrackQuery, selectTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.`ID` "
                                                   "FROM "
                                                   "`Tracks` tracks, "
                                                   "`TracksData` tracksMapping "
                                                   "LEFT JOIN "
                                                   "`Albums` album "
                                                   "ON "
                                                   "album.`ID` = :albumId AND "
                                                   "tracks.`AlbumTitle` = album.`Title` AND "
                                                   "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                   "tracks.`AlbumPath` = album.`AlbumPath` "
                                                   "WHERE "
                                                   "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                   "album.`ID` = :albumId AND "
                                                   "tracks.`Priority` = ("
                                                   "     SELECT "
                                                   "     MIN(`Priority`) "
                                                   "     FROM "
                                                   "     `Tracks` tracks2 "
                                                   "     WHERE "
                                                   "     tracks.`Title` = tracks2.`Title` AND "
                                                   "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                   "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                   "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                   "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                   ")"
                                                   "ORDER BY tracks.`DiscNumber` ASC, "
                                                   "tracks.`TrackNumber` ASC");

        auto result = prepareQuery(d->mSelectTrackIdQuery, selectTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTrackFromIdQueryText = QStringLiteral("SELECT "
                                                         "tracks.`Id`, "
                                                         "tracks.`Title`, "
                                                         "album.`ID`, "
                                                         "tracks.`ArtistName`, "
                                                         "( "
                                                         "SELECT "
                                                         "COUNT(DISTINCT tracksFromAlbum1.`ArtistName`) "
                                                         "FROM "
                                                         "`Tracks` tracksFromAlbum1 "
                                                         "WHERE "
                                                         "tracksFromAlbum1.`AlbumTitle` = album.`Title` AND "
                                                         "(tracksFromAlbum1.`AlbumArtistName` = album.`ArtistName` OR "
                                                         "(tracksFromAlbum1.`AlbumArtistName` IS NULL AND "
                                                         "album.`ArtistName` IS NULL "
                                                         ") "
                                                         ") AND "
                                                         "tracksFromAlbum1.`AlbumPath` = album.`AlbumPath` "
                                                         ") AS ArtistsCount, "
                                                         "( "
                                                         "SELECT "
                                                         "GROUP_CONCAT(tracksFromAlbum2.`ArtistName`) "
                                                         "FROM "
                                                         "`Tracks` tracksFromAlbum2 "
                                                         "WHERE "
                                                         "tracksFromAlbum2.`AlbumTitle` = album.`Title` AND "
                                                         "(tracksFromAlbum2.`AlbumArtistName` = album.`ArtistName` OR "
                                                         "(tracksFromAlbum2.`AlbumArtistName` IS NULL AND "
                                                         "album.`ArtistName` IS NULL "
                                                         ") "
                                                         ") AND "
                                                         "tracksFromAlbum2.`AlbumPath` = album.`AlbumPath` "
                                                         ") AS AllArtists, "
                                                         "tracks.`AlbumArtistName`, "
                                                         "tracksMapping.`FileName`, "
                                                         "tracksMapping.`FileModifiedTime`, "
                                                         "tracks.`TrackNumber`, "
                                                         "tracks.`DiscNumber`, "
                                                         "tracks.`Duration`, "
                                                         "tracks.`AlbumTitle`, "
                                                         "tracks.`Rating`, "
                                                         "album.`CoverFileName`, "
                                                         "("
                                                         "SELECT "
                                                         "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                         "FROM "
                                                         "`Tracks` tracks2 "
                                                         "WHERE "
                                                         "tracks2.`AlbumTitle` = album.`Title` AND "
                                                         "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                         "(tracks2.`AlbumArtistName` IS NULL AND "
                                                         "album.`ArtistName` IS NULL"
                                                         ")"
                                                         ") AND "
                                                         "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                         ") as `IsSingleDiscAlbum`, "
                                                         "trackGenre.`Name`, "
                                                         "trackComposer.`Name`, "
                                                         "trackLyricist.`Name`, "
                                                         "tracks.`Comment`, "
                                                         "tracks.`Year`, "
                                                         "tracks.`Channels`, "
                                                         "tracks.`BitRate`, "
                                                         "tracks.`SampleRate`, "
                                                         "tracks.`HasEmbeddedCover`, "
                                                         "tracksMapping.`ImportDate`, "
                                                         "tracksMapping.`FirstPlayDate`, "
                                                         "tracksMapping.`LastPlayDate`, "
                                                         "tracksMapping.`PlayCounter`, "
                                                         "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency, "
                                                         "( "
                                                         "SELECT tracksCover.`FileName` "
                                                         "FROM "
                                                         "`Tracks` tracksCover "
                                                         "WHERE "
                                                         "tracksCover.`HasEmbeddedCover` = 1 AND "
                                                         "tracksCover.`AlbumTitle` = album.`Title` AND "
                                                         "(tracksCover.`AlbumArtistName` = album.`ArtistName` OR "
                                                         "(tracksCover.`AlbumArtistName` IS NULL AND "
                                                         "album.`ArtistName` IS NULL "
                                                         ") "
                                                         ") AND "
                                                         "tracksCover.`AlbumPath` = album.`AlbumPath` "
                                                         ") as EmbeddedCover "
                                                         "FROM "
                                                         "`Tracks` tracks, "
                                                         "`TracksData` tracksMapping "
                                                         "LEFT JOIN "
                                                         "`Albums` album "
                                                         "ON "
                                                         "tracks.`AlbumTitle` = album.`Title` AND "
                                                         "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                         "tracks.`AlbumPath` = album.`AlbumPath` "
                                                         "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                         "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                         "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                         "WHERE "
                                                         "tracks.`ID` = :trackId AND "
                                                         "tracksMapping.`FileName` = tracks.`FileName`"
                                                         "");

        auto result = prepareQuery(d->mSelectTrackFromIdQuery, selectTrackFromIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackFromIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackFromIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTrackFromIdAndUrlQueryText = QStringLiteral("SELECT "
                                                         "tracks.`Id`, "
                                                         "tracks.`Title`, "
                                                         "album.`ID`, "
                                                         "tracks.`ArtistName`, "
                                                         "( "
                                                         "SELECT "
                                                         "COUNT(DISTINCT tracksFromAlbum1.`ArtistName`) "
                                                         "FROM "
                                                         "`Tracks` tracksFromAlbum1 "
                                                         "WHERE "
                                                         "tracksFromAlbum1.`AlbumTitle` = album.`Title` AND "
                                                         "(tracksFromAlbum1.`AlbumArtistName` = album.`ArtistName` OR "
                                                         "(tracksFromAlbum1.`AlbumArtistName` IS NULL AND "
                                                         "album.`ArtistName` IS NULL "
                                                         ") "
                                                         ") AND "
                                                         "tracksFromAlbum1.`AlbumPath` = album.`AlbumPath` "
                                                         ") AS ArtistsCount, "
                                                         "( "
                                                         "SELECT "
                                                         "GROUP_CONCAT(tracksFromAlbum2.`ArtistName`) "
                                                         "FROM "
                                                         "`Tracks` tracksFromAlbum2 "
                                                         "WHERE "
                                                         "tracksFromAlbum2.`AlbumTitle` = album.`Title` AND "
                                                         "(tracksFromAlbum2.`AlbumArtistName` = album.`ArtistName` OR "
                                                         "(tracksFromAlbum2.`AlbumArtistName` IS NULL AND "
                                                         "album.`ArtistName` IS NULL "
                                                         ") "
                                                         ") AND "
                                                         "tracksFromAlbum2.`AlbumPath` = album.`AlbumPath` "
                                                         ") AS AllArtists, "
                                                         "tracks.`AlbumArtistName`, "
                                                         "tracksMapping.`FileName`, "
                                                         "tracksMapping.`FileModifiedTime`, "
                                                         "tracks.`TrackNumber`, "
                                                         "tracks.`DiscNumber`, "
                                                         "tracks.`Duration`, "
                                                         "tracks.`AlbumTitle`, "
                                                         "tracks.`Rating`, "
                                                         "album.`CoverFileName`, "
                                                         "("
                                                         "SELECT "
                                                         "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                         "FROM "
                                                         "`Tracks` tracks2 "
                                                         "WHERE "
                                                         "tracks2.`AlbumTitle` = album.`Title` AND "
                                                         "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                         "(tracks2.`AlbumArtistName` IS NULL AND "
                                                         "album.`ArtistName` IS NULL"
                                                         ")"
                                                         ") AND "
                                                         "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                         ") as `IsSingleDiscAlbum`, "
                                                         "trackGenre.`Name`, "
                                                         "trackComposer.`Name`, "
                                                         "trackLyricist.`Name`, "
                                                         "tracks.`Comment`, "
                                                         "tracks.`Year`, "
                                                         "tracks.`Channels`, "
                                                         "tracks.`BitRate`, "
                                                         "tracks.`SampleRate`, "
                                                         "tracks.`HasEmbeddedCover`, "
                                                         "tracksMapping.`ImportDate`, "
                                                         "tracksMapping.`FirstPlayDate`, "
                                                         "tracksMapping.`LastPlayDate`, "
                                                         "tracksMapping.`PlayCounter`, "
                                                         "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency, "
                                                         "( "
                                                         "SELECT tracksCover.`FileName` "
                                                         "FROM "
                                                         "`Tracks` tracksCover "
                                                         "WHERE "
                                                         "tracksCover.`HasEmbeddedCover` = 1 AND "
                                                         "tracksCover.`AlbumTitle` = album.`Title` AND "
                                                         "(tracksCover.`AlbumArtistName` = album.`ArtistName` OR "
                                                         "(tracksCover.`AlbumArtistName` IS NULL AND "
                                                         "album.`ArtistName` IS NULL "
                                                         ") "
                                                         ") AND "
                                                         "tracksCover.`AlbumPath` = album.`AlbumPath` "
                                                         ") as EmbeddedCover "
                                                         "FROM "
                                                         "`Tracks` tracks, "
                                                         "`TracksData` tracksMapping "
                                                         "LEFT JOIN "
                                                         "`Albums` album "
                                                         "ON "
                                                         "tracks.`AlbumTitle` = album.`Title` AND "
                                                         "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                         "tracks.`AlbumPath` = album.`AlbumPath` "
                                                         "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                         "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                         "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                         "WHERE "
                                                         "tracks.`ID` = :trackId AND "
                                                         "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                         "tracksMapping.`FileName` = :trackUrl "
                                                         "");

        auto result = prepareQuery(d->mSelectTrackFromIdAndUrlQuery, selectTrackFromIdAndUrlQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackFromIdAndUrlQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackFromIdAndUrlQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectRadioFromIdQueryText = QStringLiteral("SELECT "
                                                  "radios.`ID`, "
                                                  "radios.`Title`, "
                                                  "radios.`HttpAddress`, "
                                                  "radios.`ImageAddress`, "
                                                  "radios.`Rating`, "
                                                  "trackGenre.`Name`, "
                                                  "radios.`Comment` "
                                                  "FROM "
                                                  "`Radios` radios "
                                                  "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = radios.`Genre` "
                                                  "WHERE "
                                                  "radios.`ID` = :radioId "
                                                  "");

        auto result = prepareQuery(d->mSelectRadioFromIdQuery, selectRadioFromIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectRadioFromIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectRadioFromIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }
    {
        auto selectCountAlbumsQueryText = QStringLiteral("SELECT count(*) "
                                                         "FROM `Albums` album "
                                                         "WHERE album.`ArtistName` = :artistName ");

        const auto result = prepareQuery(d->mSelectCountAlbumsForArtistQuery, selectCountAlbumsQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectGenreForArtistQueryText = QStringLiteral("SELECT DISTINCT trackGenre.`Name` "
                                                            "FROM "
                                                            "`Tracks` tracks "
                                                            "LEFT JOIN "
                                                            "`Albums` album "
                                                            "ON "
                                                            "tracks.`AlbumTitle` = album.`Title` AND "
                                                            "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                            "tracks.`AlbumPath` = album.`AlbumPath` "
                                                            "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                            "WHERE "
                                                            "album.`ArtistName` = :artistName");

        const auto result = prepareQuery(d->mSelectGenreForArtistQuery, selectGenreForArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreForArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreForArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectGenreForAlbumQueryText = QStringLiteral("SELECT DISTINCT trackGenre.`Name` "
                                                           "FROM "
                                                           "`Tracks` tracks "
                                                           "LEFT JOIN "
                                                           "`Albums` album "
                                                           "ON "
                                                           "tracks.`AlbumTitle` = album.`Title` AND "
                                                           "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                           "tracks.`AlbumPath` = album.`AlbumPath` "
                                                           "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                           "WHERE "
                                                           "album.`ID` = :albumId");

        const auto result = prepareQuery(d->mSelectGenreForAlbumQuery, selectGenreForAlbumQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreForAlbumQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreForAlbumQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectCountAlbumsQueryText = QStringLiteral("SELECT distinct count(album.`ID`) "
                                                         "FROM "
                                                         "`Tracks` tracks, "
                                                         "`Albums` album "
                                                         "LEFT JOIN `Composer` albumComposer ON albumComposer.`Name` = tracks.`Composer` "
                                                         "WHERE "
                                                         "(tracks.`AlbumTitle` = album.`Title` OR tracks.`AlbumTitle` IS NULL ) AND "
                                                         "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                         "(tracks.`AlbumPath` = album.`AlbumPath` OR tracks.`AlbumPath` IS NULL ) AND "
                                                         "albumComposer.`Name` = :artistName");

        const auto result = prepareQuery(d->mSelectCountAlbumsForComposerQuery, selectCountAlbumsQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForComposerQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForComposerQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectCountAlbumsQueryText = QStringLiteral("SELECT distinct count(album.`ID`) "
                                                         "FROM "
                                                         "`Tracks` tracks, "
                                                         "`Albums` album "
                                                         "LEFT JOIN `Lyricist` albumLyricist ON albumLyricist.`Name` = tracks.`Lyricist` "
                                                         "WHERE "
                                                         "(tracks.`AlbumTitle` = album.`Title` OR tracks.`AlbumTitle` IS NULL ) AND "
                                                         "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                         "(tracks.`AlbumPath` = album.`AlbumPath` OR tracks.`AlbumPath` IS NULL ) AND "
                                                         "albumLyricist.`Name` = :artistName");

        const auto result = prepareQuery(d->mSelectCountAlbumsForLyricistQuery, selectCountAlbumsQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForLyricistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForLyricistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAlbumIdFromTitleQueryText = QStringLiteral("SELECT "
                                                              "album.`ID` "
                                                              "FROM "
                                                              "`Albums` album "
                                                              "WHERE "
                                                              "album.`ArtistName` = :artistName AND "
                                                              "album.`Title` = :title");

        auto result = prepareQuery(d->mSelectAlbumIdFromTitleQuery, selectAlbumIdFromTitleQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAlbumIdFromTitleAndArtistQueryText = QStringLiteral("SELECT "
                                                                       "album.`ID` "
                                                                       "FROM "
                                                                       "`Albums` album "
                                                                       "WHERE "
                                                                       "(album.`ArtistName` = :artistName OR :artistName IS NULL OR album.`ArtistName` IS NULL) AND "
                                                                       "album.`Title` = :title AND "
                                                                       "album.`AlbumPath` = :albumPath");

        auto result = prepareQuery(d->mSelectAlbumIdFromTitleAndArtistQuery, selectAlbumIdFromTitleAndArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleAndArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleAndArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAlbumIdFromTitleWithoutArtistQueryText = QStringLiteral("SELECT "
                                                                           "album.`ID` "
                                                                           "FROM "
                                                                           "`Albums` album "
                                                                           "WHERE "
                                                                           "album.`AlbumPath` = :albumPath AND "
                                                                           "album.`Title` = :title AND "
                                                                           "album.`ArtistName` IS NULL");

        auto result = prepareQuery(d->mSelectAlbumIdFromTitleWithoutArtistQuery, selectAlbumIdFromTitleWithoutArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertAlbumQueryText = QStringLiteral("INSERT INTO `Albums` "
                                                   "(`ID`, "
                                                   "`Title`, "
                                                   "`ArtistName`, "
                                                   "`AlbumPath`, "
                                                   "`CoverFileName`) "
                                                   "VALUES "
                                                   "(:albumId, "
                                                   ":title, "
                                                   ":albumArtist, "
                                                   ":albumPath, "
                                                   ":coverFileName)");

        auto result = prepareQuery(d->mInsertAlbumQuery, insertAlbumQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertAlbumQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertAlbumQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertTrackMappingQueryText = QStringLiteral("INSERT INTO "
                                                          "`TracksData` "
                                                          "(`FileName`, "
                                                          "`FileModifiedTime`, "
                                                          "`ImportDate`, "
                                                          "`PlayCounter`) "
                                                          "VALUES (:fileName, :mtime, :importDate, 0)");

        auto result = prepareQuery(d->mInsertTrackMapping, insertTrackMappingQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertTrackMapping.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertTrackMapping.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto initialUpdateTracksValidityQueryText = QStringLiteral("UPDATE `TracksData` "
                                                                   "SET "
                                                                   "`FileModifiedTime` = :mtime "
                                                                   "WHERE `FileName` = :fileName");

        auto result = prepareQuery(d->mUpdateTrackFileModifiedTime, initialUpdateTracksValidityQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackFileModifiedTime.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackFileModifiedTime.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto initialUpdateTracksValidityQueryText = QStringLiteral("UPDATE `Tracks` "
                                                                   "SET "
                                                                   "`Priority` = :priority "
                                                                   "WHERE `FileName` = :fileName");

        auto result = prepareQuery(d->mUpdateTrackPriority, initialUpdateTracksValidityQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackPriority.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackPriority.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto removeTracksMappingFromSourceQueryText = QStringLiteral("DELETE FROM `TracksData` "
                                                                     "WHERE `FileName` = :fileName");

        auto result = prepareQuery(d->mRemoveTracksMappingFromSource, removeTracksMappingFromSourceQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveTracksMappingFromSource.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveTracksMappingFromSource.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto removeTracksMappingQueryText = QStringLiteral("DELETE FROM `TracksData` "
                                                           "WHERE `FileName` = :fileName");

        auto result = prepareQuery(d->mRemoveTracksMapping, removeTracksMappingQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveTracksMapping.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveTracksMapping.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTracksWithoutMappingQueryText = QStringLiteral("SELECT "
                                                                  "tracks.`Id`, "
                                                                  "tracks.`Title`, "
                                                                  "album.`ID`, "
                                                                  "tracks.`ArtistName`, "
                                                                  "( "
                                                                  "SELECT "
                                                                  "COUNT(DISTINCT tracksFromAlbum1.`ArtistName`) "
                                                                  "FROM "
                                                                  "`Tracks` tracksFromAlbum1 "
                                                                  "WHERE "
                                                                  "tracksFromAlbum1.`AlbumTitle` = album.`Title` AND "
                                                                  "(tracksFromAlbum1.`AlbumArtistName` = album.`ArtistName` OR "
                                                                  "(tracksFromAlbum1.`AlbumArtistName` IS NULL AND "
                                                                  "album.`ArtistName` IS NULL "
                                                                  ") "
                                                                  ") AND "
                                                                  "tracksFromAlbum1.`AlbumPath` = album.`AlbumPath` "
                                                                  ") AS ArtistsCount, "
                                                                  "( "
                                                                  "SELECT "
                                                                  "GROUP_CONCAT(tracksFromAlbum2.`ArtistName`) "
                                                                  "FROM "
                                                                  "`Tracks` tracksFromAlbum2 "
                                                                  "WHERE "
                                                                  "tracksFromAlbum2.`AlbumTitle` = album.`Title` AND "
                                                                  "(tracksFromAlbum2.`AlbumArtistName` = album.`ArtistName` OR "
                                                                  "(tracksFromAlbum2.`AlbumArtistName` IS NULL AND "
                                                                  "album.`ArtistName` IS NULL "
                                                                  ") "
                                                                  ") AND "
                                                                  "tracksFromAlbum2.`AlbumPath` = album.`AlbumPath` "
                                                                  ") AS AllArtists, "
                                                                  "tracks.`AlbumArtistName`, "
                                                                  "\"\" as FileName, "
                                                                  "NULL as FileModifiedTime, "
                                                                  "tracks.`TrackNumber`, "
                                                                  "tracks.`DiscNumber`, "
                                                                  "tracks.`Duration`, "
                                                                  "tracks.`AlbumTitle`, "
                                                                  "tracks.`Rating`, "
                                                                  "album.`CoverFileName`, "
                                                                  "("
                                                                  "SELECT "
                                                                  "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                                  "FROM "
                                                                  "`Tracks` tracks2 "
                                                                  "WHERE "
                                                                  "tracks2.`AlbumTitle` = album.`Title` AND "
                                                                  "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                                  "(tracks2.`AlbumArtistName` IS NULL AND "
                                                                  "album.`ArtistName` IS NULL"
                                                                  ")"
                                                                  ") AND "
                                                                  "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                                  ") as `IsSingleDiscAlbum`, "
                                                                  "trackGenre.`Name`, "
                                                                  "trackComposer.`Name`, "
                                                                  "trackLyricist.`Name`, "
                                                                  "tracks.`Comment`, "
                                                                  "tracks.`Year`, "
                                                                  "tracks.`Channels`, "
                                                                  "tracks.`BitRate`, "
                                                                  "tracks.`SampleRate`, "
                                                                  "tracks.`HasEmbeddedCover`, "
                                                                  "tracksMapping.`ImportDate`, "
                                                                  "tracksMapping.`FirstPlayDate`, "
                                                                  "tracksMapping.`LastPlayDate`, "
                                                                  "tracksMapping.`PlayCounter`, "
                                                                  "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency, "
                                                                  "( "
                                                                  "SELECT tracksCover.`FileName` "
                                                                  "FROM "
                                                                  "`Tracks` tracksCover "
                                                                  "WHERE "
                                                                  "tracksCover.`HasEmbeddedCover` = 1 AND "
                                                                  "tracksCover.`AlbumTitle` = album.`Title` AND "
                                                                  "(tracksCover.`AlbumArtistName` = album.`ArtistName` OR "
                                                                  "(tracksCover.`AlbumArtistName` IS NULL AND "
                                                                  "album.`ArtistName` IS NULL "
                                                                  ") "
                                                                  ") AND "
                                                                  "tracksCover.`AlbumPath` = album.`AlbumPath` "
                                                                  ") as EmbeddedCover "
                                                                  "FROM "
                                                                  "`Tracks` tracks, "
                                                                  "`TracksData` tracksMapping "
                                                                  "LEFT JOIN "
                                                                  "`Albums` album "
                                                                  "ON "
                                                                  "tracks.`AlbumTitle` = album.`Title` AND "
                                                                  "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                                  "tracks.`AlbumPath` = album.`AlbumPath` "
                                                                  "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                                  "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                                  "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                                  "WHERE "
                                                                  "tracks.`FileName` = tracksMapping.`FileName` AND "
                                                                  "tracks.`FileName` NOT IN (SELECT tracksMapping2.`FileName` FROM `TracksData` tracksMapping2)");

        auto result = prepareQuery(d->mSelectTracksWithoutMappingQuery, selectTracksWithoutMappingQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksWithoutMappingQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksWithoutMappingQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTracksMappingQueryText = QStringLiteral("SELECT "
                                                           "track.`ID`, "
                                                           "trackData.`FileName`, "
                                                           "track.`Priority`, "
                                                           "trackData.`FileModifiedTime` "
                                                           "FROM "
                                                           "`TracksData` trackData "
                                                           "LEFT JOIN "
                                                           "`Tracks` track "
                                                           "ON "
                                                           "track.`FileName` = trackData.`FileName` "
                                                           "WHERE "
                                                           "trackData.`FileName` = :fileName");

        auto result = prepareQuery(d->mSelectTracksMapping, selectTracksMappingQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksMapping.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksMapping.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectRadioIdFromHttpAddress = QStringLiteral("SELECT "
                                                           "`ID` "
                                                           "FROM "
                                                           "`Radios` "
                                                           "WHERE "
                                                           "`HttpAddress` = :httpAddress");

        auto result = prepareQuery(d->mSelectRadioIdFromHttpAddress, selectRadioIdFromHttpAddress);

        if (!result) {
            qCInfo(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectRadioIdFromHttpAddress.lastQuery();
            qCInfo(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectRadioIdFromHttpAddress.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTracksMappingPriorityQueryText = QStringLiteral("SELECT "
                                                                   "max(tracks.`Priority`) AS Priority "
                                                                   "FROM "
                                                                   "`Tracks` tracks, "
                                                                   "`Albums` albums "
                                                                   "WHERE "
                                                                   "tracks.`Title` = :title AND "
                                                                   "(tracks.`ArtistName` = :trackArtist OR tracks.`ArtistName` IS NULL) AND "
                                                                   "(tracks.`AlbumTitle` = :album OR tracks.`AlbumTitle` IS NULL) AND "
                                                                   "(tracks.`AlbumArtistName` = :albumArtist OR tracks.`AlbumArtistName` IS NULL) AND "
                                                                   "(tracks.`AlbumPath` = :albumPath OR tracks.`AlbumPath` IS NULL)");

        auto result = prepareQuery(d->mSelectTracksMappingPriority, selectTracksMappingPriorityQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksMappingPriority.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksMappingPriority.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTracksMappingPriorityQueryByTrackIdText = QStringLiteral("SELECT "
                                                                            "MAX(track.`Priority`) "
                                                                            "FROM "
                                                                            "`TracksData` trackData, "
                                                                            "`Tracks` track "
                                                                            "WHERE "
                                                                            "track.`ID` = :trackId AND "
                                                                            "trackData.`FileName` = track.`FileName`");

        auto result = prepareQuery(d->mSelectTracksMappingPriorityByTrackId, selectTracksMappingPriorityQueryByTrackIdText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksMappingPriorityByTrackId.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksMappingPriorityByTrackId.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAllTrackFilesFromSourceQueryText = QStringLiteral("SELECT "
                                                                     "tracksMapping.`FileName`, "
                                                                     "tracksMapping.`FileModifiedTime` "
                                                                     "FROM "
                                                                     "`TracksData` tracksMapping");

        auto result = prepareQuery(d->mSelectAllTrackFilesQuery, selectAllTrackFilesFromSourceQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllTrackFilesQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAllTrackFilesQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertMusicSourceQueryText = QStringLiteral("INSERT OR IGNORE INTO `DiscoverSource` (`ID`, `Name`) "
                                                         "VALUES (:discoverId, :name)");

        auto result = prepareQuery(d->mInsertMusicSource, insertMusicSourceQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertMusicSource.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertMusicSource.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectMusicSourceQueryText = QStringLiteral("SELECT `ID` FROM `DiscoverSource` WHERE `Name` = :name");

        auto result = prepareQuery(d->mSelectMusicSource, selectMusicSourceQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectMusicSource.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectMusicSource.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.`ID`,  tracksMapping.`FileName` "
                                                   "FROM "
                                                   "`Tracks` tracks, "
                                                   "`Albums` album, "
                                                   "`TracksData` tracksMapping "
                                                   "WHERE "
                                                   "tracks.`Title` = :title AND "
                                                   "album.`ID` = :album AND "
                                                   "(tracks.`AlbumTitle` = album.`Title` OR tracks.`AlbumTitle` IS NULL ) AND "
                                                   "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                   "(tracks.`AlbumPath` = album.`AlbumPath` OR tracks.`AlbumPath` IS NULL ) AND "
                                                   "tracks.`ArtistName` = :artist AND "
                                                   "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                   "tracks.`Priority` = ("
                                                   "     SELECT "
                                                   "     MIN(`Priority`) "
                                                   "     FROM "
                                                   "     `Tracks` tracks2 "
                                                   "     WHERE "
                                                   "     tracks.`Title` = tracks2.`Title` AND "
                                                   "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                   "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                   "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                   "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                   ")"
                                                   "");

        auto result = prepareQuery(d->mSelectTrackIdFromTitleAlbumIdArtistQuery, selectTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumIdArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumIdArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertTrackQueryText = QStringLiteral("INSERT INTO `Tracks` "
                                                   "("
                                                   "`ID`, "
                                                   "`FileName`, "
                                                   "`Priority`, "
                                                   "`Title`, "
                                                   "`ArtistName`, "
                                                   "`AlbumTitle`, "
                                                   "`AlbumArtistName`, "
                                                   "`AlbumPath`, "
                                                   "`Genre`, "
                                                   "`Composer`, "
                                                   "`Lyricist`, "
                                                   "`Comment`, "
                                                   "`TrackNumber`, "
                                                   "`DiscNumber`, "
                                                   "`Channels`, "
                                                   "`BitRate`, "
                                                   "`SampleRate`, "
                                                   "`Year`,  "
                                                   "`Duration`, "
                                                   "`Rating`, "
                                                   "`HasEmbeddedCover`) "
                                                   "VALUES "
                                                   "("
                                                   ":trackId, "
                                                   ":fileName, "
                                                   ":priority, "
                                                   ":title, "
                                                   ":artistName, "
                                                   ":albumTitle, "
                                                   ":albumArtistName, "
                                                   ":albumPath, "
                                                   ":genre, "
                                                   ":composer, "
                                                   ":lyricist, "
                                                   ":comment, "
                                                   ":trackNumber, "
                                                   ":discNumber, "
                                                   ":channels, "
                                                   ":bitRate, "
                                                   ":sampleRate, "
                                                   ":year, "
                                                   ":trackDuration, "
                                                   ":trackRating, "
                                                   ":hasEmbeddedCover)");

        auto result = prepareQuery(d->mInsertTrackQuery, insertTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertTrackQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertTrackQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto updateTrackQueryText = QStringLiteral("UPDATE `Tracks` "
                                                   "SET "
                                                   "`FileName` = :fileName, "
                                                   "`Title` = :title, "
                                                   "`ArtistName` = :artistName, "
                                                   "`AlbumTitle` = :albumTitle, "
                                                   "`AlbumArtistName` = :albumArtistName, "
                                                   "`AlbumPath` = :albumPath, "
                                                   "`Genre` = :genre, "
                                                   "`Composer` = :composer, "
                                                   "`Lyricist` = :lyricist, "
                                                   "`Comment` = :comment, "
                                                   "`TrackNumber` = :trackNumber, "
                                                   "`DiscNumber` = :discNumber, "
                                                   "`Channels` = :channels, "
                                                   "`BitRate` = :bitRate, "
                                                   "`SampleRate` = :sampleRate, "
                                                   "`Year` = :year, "
                                                   " `Duration` = :trackDuration, "
                                                   "`Rating` = :trackRating "
                                                   "WHERE "
                                                   "`ID` = :trackId");

        auto result = prepareQuery(d->mUpdateTrackQuery, updateTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto insertRadioQueryText = QStringLiteral("INSERT INTO `Radios` "
                                                   "("
                                                   "`Title`, "
                                                   "`httpAddress`, "
                                                   "`Comment`, "
                                                   "`Rating`, "
                                                   "`ImageAddress`) "
                                                   "VALUES "
                                                   "("
                                                   ":title, "
                                                   ":httpAddress, "
                                                   ":comment, "
                                                   ":trackRating,"
                                                   ":imageAddress)");

        auto result = prepareQuery(d->mInsertRadioQuery, insertRadioQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertRadioQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mInsertRadioQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto deleteRadioQueryText = QStringLiteral("DELETE FROM `Radios` "
                                                   "WHERE `ID` = :radioId");

        auto result = prepareQuery(d->mDeleteRadioQuery, deleteRadioQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mDeleteRadioQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mDeleteRadioQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto updateRadioQueryText = QStringLiteral("UPDATE `Radios` "
                                                   "SET "
                                                   "`HttpAddress` = :httpAddress, "
                                                   "`Title` = :title, "
                                                   "`Comment` = :comment, "
                                                   "`Rating` = :trackRating, "
                                                   "`ImageAddress` = :imageAddress "
                                                   "WHERE "
                                                   "`ID` = :radioId");

        auto result = prepareQuery(d->mUpdateRadioQuery, updateRadioQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateRadioQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateRadioQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto updateAlbumArtistQueryText = QStringLiteral("UPDATE `Albums` "
                                                         "SET "
                                                         "`ArtistName` = :artistName "
                                                         "WHERE "
                                                         "`ID` = :albumId");

        auto result = prepareQuery(d->mUpdateAlbumArtistQuery, updateAlbumArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto updateAlbumArtistInTracksQueryText = QStringLiteral("UPDATE `Tracks` "
                                                                 "SET "
                                                                 "`AlbumArtistName` = :artistName "
                                                                 "WHERE "
                                                                 "`AlbumTitle` = :albumTitle AND "
                                                                 "`AlbumPath` = :albumPath AND "
                                                                 "`AlbumArtistName` IS NULL");

        auto result = prepareQuery(d->mUpdateAlbumArtistInTracksQuery, updateAlbumArtistInTracksQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtistInTracksQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtistInTracksQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto queryMaximumTrackIdQueryText = QStringLiteral("SELECT MAX(tracks.`ID`)"
                                                           "FROM "
                                                           "`Tracks` tracks");

        auto result = prepareQuery(d->mQueryMaximumTrackIdQuery, queryMaximumTrackIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumTrackIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumTrackIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto queryMaximumAlbumIdQueryText = QStringLiteral("SELECT MAX(albums.`ID`)"
                                                           "FROM "
                                                           "`Albums` albums");

        auto result = prepareQuery(d->mQueryMaximumAlbumIdQuery, queryMaximumAlbumIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumAlbumIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumAlbumIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto queryMaximumArtistIdQueryText = QStringLiteral("SELECT MAX(artists.`ID`)"
                                                            "FROM "
                                                            "`Artists` artists");

        auto result = prepareQuery(d->mQueryMaximumArtistIdQuery, queryMaximumArtistIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumArtistIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumArtistIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto queryMaximumLyricistIdQueryText = QStringLiteral("SELECT MAX(lyricists.`ID`)"
                                                              "FROM "
                                                              "`Lyricist` lyricists");

        auto result = prepareQuery(d->mQueryMaximumLyricistIdQuery, queryMaximumLyricistIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumLyricistIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumLyricistIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto queryMaximumComposerIdQueryText = QStringLiteral("SELECT MAX(composers.`ID`)"
                                                              "FROM "
                                                              "`Composer` composers");

        auto result = prepareQuery(d->mQueryMaximumComposerIdQuery, queryMaximumComposerIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumComposerIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumComposerIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto queryMaximumGenreIdQueryText = QStringLiteral("SELECT MAX(genres.`ID`)"
                                                           "FROM "
                                                           "`Genre` genres");

        auto result = prepareQuery(d->mQueryMaximumGenreIdQuery, queryMaximumGenreIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumGenreIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mQueryMaximumGenreIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.ID "
                                                   "FROM "
                                                   "`Tracks` tracks "
                                                   "WHERE "
                                                   "tracks.`Title` = :title AND "
                                                   "(tracks.`AlbumTitle` = :album OR (:album IS NULL AND tracks.`AlbumTitle` IS NULL)) AND "
                                                   "(tracks.`TrackNumber` = :trackNumber OR (:trackNumber IS NULL AND tracks.`TrackNumber` IS NULL)) AND "
                                                   "(tracks.`DiscNumber` = :discNumber OR (:discNumber IS NULL AND tracks.`DiscNumber` IS NULL)) AND "
                                                   "tracks.`ArtistName` = :artist");

        auto result = prepareQuery(d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery, selectTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.ID "
                                                   "FROM "
                                                   "`Tracks` tracks, "
                                                   "`Albums` albums "
                                                   "WHERE "
                                                   "tracks.`Title` = :title AND "
                                                   "tracks.`Priority` = :priority AND "
                                                   "(tracks.`ArtistName` = :trackArtist OR tracks.`ArtistName` IS NULL) AND "
                                                   "(tracks.`AlbumTitle` = :album OR tracks.`AlbumTitle` IS NULL) AND "
                                                   "(tracks.`AlbumArtistName` = :albumArtist OR tracks.`AlbumArtistName` IS NULL) AND "
                                                   "(tracks.`AlbumPath` = :albumPath OR tracks.`AlbumPath` IS NULL) AND "
                                                   "(tracks.`TrackNumber` = :trackNumber OR tracks.`TrackNumber` IS NULL) AND "
                                                   "(tracks.`DiscNumber` = :discNumber OR tracks.`DiscNumber` IS NULL) "
                                                   "");

        auto result = prepareQuery(d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery, selectTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAlbumArtUriFromAlbumIdQueryText = QStringLiteral("SELECT `CoverFileName`"
                                                                    "FROM "
                                                                    "`Albums` "
                                                                    "WHERE "
                                                                    "`ID` = :albumId");

        auto result = prepareQuery(d->mSelectAlbumArtUriFromAlbumIdQuery, selectAlbumArtUriFromAlbumIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumArtUriFromAlbumIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumArtUriFromAlbumIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto updateAlbumArtUriFromAlbumIdQueryText = QStringLiteral("UPDATE `Albums` "
                                                                    "SET `CoverFileName` = :coverFileName "
                                                                    "WHERE "
                                                                    "`ID` = :albumId");

        auto result = prepareQuery(d->mUpdateAlbumArtUriFromAlbumIdQuery, updateAlbumArtUriFromAlbumIdQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtUriFromAlbumIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtUriFromAlbumIdQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTracksFromArtistQueryText = QStringLiteral("SELECT "
                                                              "tracks.`ID`, "
                                                              "tracks.`Title`, "
                                                              "album.`ID`, "
                                                              "tracks.`ArtistName`, "
                                                              "( "
                                                              "SELECT "
                                                              "COUNT(DISTINCT tracksFromAlbum1.`ArtistName`) "
                                                              "FROM "
                                                              "`Tracks` tracksFromAlbum1 "
                                                              "WHERE "
                                                              "tracksFromAlbum1.`AlbumTitle` = album.`Title` AND "
                                                              "(tracksFromAlbum1.`AlbumArtistName` = album.`ArtistName` OR "
                                                              "(tracksFromAlbum1.`AlbumArtistName` IS NULL AND "
                                                              "album.`ArtistName` IS NULL "
                                                              ") "
                                                              ") AND "
                                                              "tracksFromAlbum1.`AlbumPath` = album.`AlbumPath` "
                                                              ") AS ArtistsCount, "
                                                              "( "
                                                              "SELECT "
                                                              "GROUP_CONCAT(tracksFromAlbum2.`ArtistName`) "
                                                              "FROM "
                                                              "`Tracks` tracksFromAlbum2 "
                                                              "WHERE "
                                                              "tracksFromAlbum2.`AlbumTitle` = album.`Title` AND "
                                                              "(tracksFromAlbum2.`AlbumArtistName` = album.`ArtistName` OR "
                                                              "(tracksFromAlbum2.`AlbumArtistName` IS NULL AND "
                                                              "album.`ArtistName` IS NULL "
                                                              ") "
                                                              ") AND "
                                                              "tracksFromAlbum2.`AlbumPath` = album.`AlbumPath` "
                                                              ") AS AllArtists, "
                                                              "tracks.`AlbumArtistName`, "
                                                              "tracksMapping.`FileName`, "
                                                              "tracksMapping.`FileModifiedTime`, "
                                                              "tracks.`TrackNumber`, "
                                                              "tracks.`DiscNumber`, "
                                                              "tracks.`Duration`, "
                                                              "tracks.`AlbumTitle`, "
                                                              "tracks.`Rating`, "
                                                              "album.`CoverFileName`, "
                                                              "("
                                                              "SELECT "
                                                              "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                              "FROM "
                                                              "`Tracks` tracks2 "
                                                              "WHERE "
                                                              "tracks2.`AlbumTitle` = album.`Title` AND "
                                                              "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                              "(tracks2.`AlbumArtistName` IS NULL AND "
                                                              "album.`ArtistName` IS NULL"
                                                              ")"
                                                              ") AND "
                                                              "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                              ") as `IsSingleDiscAlbum`, "
                                                              "trackGenre.`Name`, "
                                                              "trackComposer.`Name`, "
                                                              "trackLyricist.`Name`, "
                                                              "tracks.`Comment`, "
                                                              "tracks.`Year`, "
                                                              "tracks.`Channels`, "
                                                              "tracks.`BitRate`, "
                                                              "tracks.`SampleRate`, "
                                                              "tracks.`HasEmbeddedCover`, "
                                                              "tracksMapping.`ImportDate`, "
                                                              "tracksMapping.`FirstPlayDate`, "
                                                              "tracksMapping.`LastPlayDate`, "
                                                              "tracksMapping.`PlayCounter`, "
                                                              "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency, "
                                                              "( "
                                                              "SELECT tracksCover.`FileName` "
                                                              "FROM "
                                                              "`Tracks` tracksCover "
                                                              "WHERE "
                                                              "tracksCover.`HasEmbeddedCover` = 1 AND "
                                                              "tracksCover.`AlbumTitle` = album.`Title` AND "
                                                              "(tracksCover.`AlbumArtistName` = album.`ArtistName` OR "
                                                              "(tracksCover.`AlbumArtistName` IS NULL AND "
                                                              "album.`ArtistName` IS NULL "
                                                              ") "
                                                              ") AND "
                                                              "tracksCover.`AlbumPath` = album.`AlbumPath` "
                                                              ") as EmbeddedCover "
                                                              "FROM "
                                                              "`Tracks` tracks, "
                                                              "`TracksData` tracksMapping "
                                                              "LEFT JOIN "
                                                              "`Albums` album "
                                                              "ON "
                                                              "tracks.`AlbumTitle` = album.`Title` AND "
                                                              "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                              "tracks.`AlbumPath` = album.`AlbumPath` "
                                                              "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                              "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                              "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                              "WHERE "
                                                              "tracks.`ArtistName` = :artistName AND "
                                                              "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                              "tracks.`Priority` = ("
                                                              "     SELECT "
                                                              "     MIN(`Priority`) "
                                                              "     FROM "
                                                              "     `Tracks` tracks2 "
                                                              "     WHERE "
                                                              "     tracks.`Title` = tracks2.`Title` AND "
                                                              "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                              "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                              "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                              "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                              ")"
                                                              "ORDER BY "
                                                              "album.`Title` ASC, "
                                                              "tracks.`DiscNumber` ASC, "
                                                              "tracks.`TrackNumber` ASC, "
                                                              "tracks.`Title` ASC"
                                                              "");

        auto result = prepareQuery(d->mSelectTracksFromArtist, selectTracksFromArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksFromArtist.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksFromArtist.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectTracksFromGenreQueryText = QStringLiteral("SELECT "
                                                             "tracks.`ID`, "
                                                             "tracks.`Title`, "
                                                             "album.`ID`, "
                                                             "tracks.`ArtistName`, "
                                                             "( "
                                                             "SELECT "
                                                             "COUNT(DISTINCT tracksFromAlbum1.`ArtistName`) "
                                                             "FROM "
                                                             "`Tracks` tracksFromAlbum1 "
                                                             "WHERE "
                                                             "tracksFromAlbum1.`AlbumTitle` = album.`Title` AND "
                                                             "(tracksFromAlbum1.`AlbumArtistName` = album.`ArtistName` OR "
                                                             "(tracksFromAlbum1.`AlbumArtistName` IS NULL AND "
                                                             "album.`ArtistName` IS NULL "
                                                             ") "
                                                             ") AND "
                                                             "tracksFromAlbum1.`AlbumPath` = album.`AlbumPath` "
                                                             ") AS ArtistsCount, "
                                                             "( "
                                                             "SELECT "
                                                             "GROUP_CONCAT(tracksFromAlbum2.`ArtistName`) "
                                                             "FROM "
                                                             "`Tracks` tracksFromAlbum2 "
                                                             "WHERE "
                                                             "tracksFromAlbum2.`AlbumTitle` = album.`Title` AND "
                                                             "(tracksFromAlbum2.`AlbumArtistName` = album.`ArtistName` OR "
                                                             "(tracksFromAlbum2.`AlbumArtistName` IS NULL AND "
                                                             "album.`ArtistName` IS NULL "
                                                             ") "
                                                             ") AND "
                                                             "tracksFromAlbum2.`AlbumPath` = album.`AlbumPath` "
                                                             ") AS AllArtists, "
                                                             "tracks.`AlbumArtistName`, "
                                                             "tracksMapping.`FileName`, "
                                                             "tracksMapping.`FileModifiedTime`, "
                                                             "tracks.`TrackNumber`, "
                                                             "tracks.`DiscNumber`, "
                                                             "tracks.`Duration`, "
                                                             "tracks.`AlbumTitle`, "
                                                             "tracks.`Rating`, "
                                                             "album.`CoverFileName`, "
                                                             "("
                                                             "SELECT "
                                                             "COUNT(DISTINCT tracks2.DiscNumber) <= 1 "
                                                             "FROM "
                                                             "`Tracks` tracks2 "
                                                             "WHERE "
                                                             "tracks2.`AlbumTitle` = album.`Title` AND "
                                                             "(tracks2.`AlbumArtistName` = album.`ArtistName` OR "
                                                             "(tracks2.`AlbumArtistName` IS NULL AND "
                                                             "album.`ArtistName` IS NULL"
                                                             ")"
                                                             ") AND "
                                                             "tracks2.`AlbumPath` = album.`AlbumPath` "
                                                             ") as `IsSingleDiscAlbum`, "
                                                             "trackGenre.`Name`, "
                                                             "trackComposer.`Name`, "
                                                             "trackLyricist.`Name`, "
                                                             "tracks.`Comment`, "
                                                             "tracks.`Year`, "
                                                             "tracks.`Channels`, "
                                                             "tracks.`BitRate`, "
                                                             "tracks.`SampleRate`, "
                                                             "tracks.`HasEmbeddedCover`, "
                                                             "tracksMapping.`ImportDate`, "
                                                             "tracksMapping.`FirstPlayDate`, "
                                                             "tracksMapping.`LastPlayDate`, "
                                                             "tracksMapping.`PlayCounter`, "
                                                             "tracksMapping.`PlayCounter` / (strftime('%s', 'now') - tracksMapping.`FirstPlayDate`) as PlayFrequency, "
                                                             "( "
                                                             "SELECT tracksCover.`FileName` "
                                                             "FROM "
                                                             "`Tracks` tracksCover "
                                                             "WHERE "
                                                             "tracksCover.`HasEmbeddedCover` = 1 AND "
                                                             "tracksCover.`AlbumTitle` = album.`Title` AND "
                                                             "(tracksCover.`AlbumArtistName` = album.`ArtistName` OR "
                                                             "(tracksCover.`AlbumArtistName` IS NULL AND "
                                                             "album.`ArtistName` IS NULL "
                                                             ") "
                                                             ") AND "
                                                             "tracksCover.`AlbumPath` = album.`AlbumPath` "
                                                             ") as EmbeddedCover "
                                                             "FROM "
                                                             "`Tracks` tracks, "
                                                             "`TracksData` tracksMapping "
                                                             "LEFT JOIN "
                                                             "`Albums` album "
                                                             "ON "
                                                             "tracks.`AlbumTitle` = album.`Title` AND "
                                                             "(tracks.`AlbumArtistName` = album.`ArtistName` OR tracks.`AlbumArtistName` IS NULL ) AND "
                                                             "tracks.`AlbumPath` = album.`AlbumPath` "
                                                             "LEFT JOIN `Composer` trackComposer ON trackComposer.`Name` = tracks.`Composer` "
                                                             "LEFT JOIN `Lyricist` trackLyricist ON trackLyricist.`Name` = tracks.`Lyricist` "
                                                             "LEFT JOIN `Genre` trackGenre ON trackGenre.`Name` = tracks.`Genre` "
                                                             "WHERE "
                                                             "tracks.`Genre` = :genre AND "
                                                             "tracksMapping.`FileName` = tracks.`FileName` AND "
                                                             "tracks.`Priority` = ("
                                                             "     SELECT "
                                                             "     MIN(`Priority`) "
                                                             "     FROM "
                                                             "     `Tracks` tracks2 "
                                                             "     WHERE "
                                                             "     tracks.`Title` = tracks2.`Title` AND "
                                                             "     (tracks.`ArtistName` IS NULL OR tracks.`ArtistName` = tracks2.`ArtistName`) AND "
                                                             "     (tracks.`AlbumTitle` IS NULL OR tracks.`AlbumTitle` = tracks2.`AlbumTitle`) AND "
                                                             "     (tracks.`AlbumArtistName` IS NULL OR tracks.`AlbumArtistName` = tracks2.`AlbumArtistName`) AND "
                                                             "     (tracks.`AlbumPath` IS NULL OR tracks.`AlbumPath` = tracks2.`AlbumPath`)"
                                                             ")"
                                                             "ORDER BY "
                                                             "album.`Title` ASC, "
                                                             "tracks.`DiscNumber` ASC, "
                                                             "tracks.`TrackNumber` ASC, "
                                                             "tracks.`Title` ASC"
                                                             "");

        auto result = prepareQuery(d->mSelectTracksFromGenre, selectTracksFromGenreQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksFromGenre.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectTracksFromGenre.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectAlbumIdsFromArtistQueryText = QStringLiteral("SELECT "
                                                                "album.`ID` "
                                                                "FROM "
                                                                "`Albums` album "
                                                                "WHERE "
                                                                "album.`ArtistName` = :artistName");

        auto result = prepareQuery(d->mSelectAlbumIdsFromArtist, selectAlbumIdsFromArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdsFromArtist.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectAlbumIdsFromArtist.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectArtistQueryText = QStringLiteral("SELECT `ID`, "
                                                    "`Name` "
                                                    "FROM `Artists` "
                                                    "WHERE "
                                                    "`ID` = :artistId");

        auto result = prepareQuery(d->mSelectArtistQuery, selectArtistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto updateTrackStatisticsQueryText = QStringLiteral("UPDATE `TracksData` "
                                                             "SET "
                                                             "`LastPlayDate` = :playDate, "
                                                             "`PlayCounter` = `PlayCounter` + 1 "
                                                             "WHERE "
                                                             "`FileName` = :fileName");

        auto result = prepareQuery(d->mUpdateTrackStatistics, updateTrackStatisticsQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackStatistics.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackStatistics.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto updateTrackFirstPlayStatisticsQueryText = QStringLiteral("UPDATE `TracksData` "
                                                                      "SET "
                                                                      "`FirstPlayDate` = :playDate "
                                                                      "WHERE "
                                                                      "`FileName` = :fileName AND "
                                                                      "`FirstPlayDate` IS NULL");

        auto result = prepareQuery(d->mUpdateTrackFirstPlayStatistics, updateTrackFirstPlayStatisticsQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackFirstPlayStatistics.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mUpdateTrackFirstPlayStatistics.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectGenreQueryText = QStringLiteral("SELECT `ID`, "
                                                   "`Name` "
                                                   "FROM `Genre` "
                                                   "WHERE "
                                                   "`ID` = :genreId");

        auto result = prepareQuery(d->mSelectGenreQuery, selectGenreQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectGenreQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto selectComposerQueryText = QStringLiteral("SELECT `ID`, "
                                                      "`Name` "
                                                      "FROM `Composer` "
                                                      "WHERE "
                                                      "`ID` = :composerId");

        auto result = prepareQuery(d->mSelectComposerQuery, selectComposerQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectComposerQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectComposerQuery.lastError();
        }
    }

    {
        auto selectLyricistQueryText = QStringLiteral("SELECT `ID`, "
                                                      "`Name` "
                                                      "FROM `Lyricist` "
                                                      "WHERE "
                                                      "`ID` = :lyricistId");

        auto result = prepareQuery(d->mSelectLyricistQuery, selectLyricistQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectLyricistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mSelectLyricistQuery.lastError();
        }
    }

    {
        auto removeTrackQueryText = QStringLiteral("DELETE FROM `Tracks` "
                                                   "WHERE "
                                                   "`ID` = :trackId");

        auto result = prepareQuery(d->mRemoveTrackQuery, removeTrackQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveTrackQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveTrackQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto removeAlbumQueryText = QStringLiteral("DELETE FROM `Albums` "
                                                   "WHERE "
                                                   "`ID` = :albumId");

        auto result = prepareQuery(d->mRemoveAlbumQuery, removeAlbumQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveAlbumQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveAlbumQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    {
        auto removeAlbumQueryText = QStringLiteral("DELETE FROM `Artists` "
                                                   "WHERE "
                                                   "`ID` = :artistId");

        auto result = prepareQuery(d->mRemoveArtistQuery, removeAlbumQueryText);

        if (!result) {
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::initRequest" << d->mRemoveArtistQuery.lastError();

            Q_EMIT databaseError();
        }
    }

    finishTransaction();

    d->mInitFinished = true;
    Q_EMIT requestsInitDone();
}

qulonglong DatabaseInterface::insertAlbum(const QString &title, const QString &albumArtist,
                                          const QString &trackPath, const QUrl &albumArtURI)
{
    auto result = qulonglong(0);

    if (title.isEmpty()) {
        return result;
    }

    d->mSelectAlbumIdFromTitleAndArtistQuery.bindValue(QStringLiteral(":title"), title);
    d->mSelectAlbumIdFromTitleAndArtistQuery.bindValue(QStringLiteral(":albumPath"), trackPath);
    d->mSelectAlbumIdFromTitleAndArtistQuery.bindValue(QStringLiteral(":artistName"), albumArtist);

    auto queryResult = execQuery(d->mSelectAlbumIdFromTitleAndArtistQuery);

    if (!queryResult || !d->mSelectAlbumIdFromTitleAndArtistQuery.isSelect() || !d->mSelectAlbumIdFromTitleAndArtistQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleAndArtistQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleAndArtistQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleAndArtistQuery.lastError();

        d->mSelectAlbumIdFromTitleAndArtistQuery.finish();

        return result;
    }

    if (d->mSelectAlbumIdFromTitleAndArtistQuery.next()) {
        result = d->mSelectAlbumIdFromTitleAndArtistQuery.record().value(0).toULongLong();

        d->mSelectAlbumIdFromTitleAndArtistQuery.finish();

        if (!albumArtist.isEmpty()) {
            const auto similarAlbum = internalOneAlbumPartialData(result);
            updateAlbumArtist(result, title, trackPath, albumArtist);
            if (updateAlbumCover(result, albumArtURI)) {
                recordModifiedAlbum(result);
            }
        }

        return result;
    }

    d->mSelectAlbumIdFromTitleAndArtistQuery.finish();

    d->mInsertAlbumQuery.bindValue(QStringLiteral(":albumId"), d->mAlbumId);
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":title"), title);
    if (!albumArtist.isEmpty()) {
        insertArtist(albumArtist);
        d->mInsertAlbumQuery.bindValue(QStringLiteral(":albumArtist"), albumArtist);
    } else {
        d->mInsertAlbumQuery.bindValue(QStringLiteral(":albumArtist"), {});
    }
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":albumPath"), trackPath);
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":coverFileName"), albumArtURI);

    queryResult = execQuery(d->mInsertAlbumQuery);

    if (!queryResult || !d->mInsertAlbumQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mInsertAlbumQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mInsertAlbumQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertAlbum" << d->mInsertAlbumQuery.lastError();

        d->mInsertAlbumQuery.finish();

        return result;
    }

    result = d->mAlbumId;

    d->mInsertAlbumQuery.finish();

    ++d->mAlbumId;

    d->mInsertedAlbums.insert(result);

    return result;
}

bool DatabaseInterface::updateAlbumFromId(qulonglong albumId, const QUrl &albumArtUri,
                                          const DataTypes::TrackDataType &currentTrack, const QString &albumPath)
{
    auto modifiedAlbum = false;

    modifiedAlbum = updateAlbumCover(albumId, albumArtUri);

    if (!isValidArtist(albumId) && currentTrack.hasAlbum() && (currentTrack.hasAlbumArtist() || currentTrack.hasArtist())) {
        updateAlbumArtist(albumId, currentTrack.album(), albumPath, currentTrack.albumArtist());

        modifiedAlbum = true;
    }

    return modifiedAlbum;
}

qulonglong DatabaseInterface::insertArtist(const QString &name)
{
    auto result = qulonglong(0);

    if (name.isEmpty()) {
        return result;
    }

    d->mSelectArtistByNameQuery.bindValue(QStringLiteral(":name"), name);

    auto queryResult = execQuery(d->mSelectArtistByNameQuery);

    if (!queryResult || !d->mSelectArtistByNameQuery.isSelect() || !d->mSelectArtistByNameQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.lastError();

        d->mSelectArtistByNameQuery.finish();

        return result;
    }

    if (d->mSelectArtistByNameQuery.next()) {
        result = d->mSelectArtistByNameQuery.record().value(0).toULongLong();

        d->mSelectArtistByNameQuery.finish();

        return result;
    }

    d->mSelectArtistByNameQuery.finish();

    d->mInsertArtistsQuery.bindValue(QStringLiteral(":artistId"), d->mArtistId);
    d->mInsertArtistsQuery.bindValue(QStringLiteral(":name"), name);

    queryResult = execQuery(d->mInsertArtistsQuery);

    if (!queryResult || !d->mInsertArtistsQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mInsertArtistsQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mInsertArtistsQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mInsertArtistsQuery.lastError();

        d->mInsertArtistsQuery.finish();

        return result;
    }

    result = d->mArtistId;

    ++d->mArtistId;

    d->mInsertedArtists.insert({result, name});

    d->mInsertArtistsQuery.finish();

    return result;
}

qulonglong DatabaseInterface::insertComposer(const QString &name)
{
    auto result = qulonglong(0);

    if (name.isEmpty()) {
        return result;
    }

    d->mSelectComposerByNameQuery.bindValue(QStringLiteral(":name"), name);

    auto queryResult = execQuery(d->mSelectComposerByNameQuery);

    if (!queryResult || !d->mSelectComposerByNameQuery.isSelect() || !d->mSelectComposerByNameQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertComposer" << d->mSelectComposerByNameQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertComposer" << d->mSelectComposerByNameQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertComposer" << d->mSelectComposerByNameQuery.lastError();

        d->mSelectComposerByNameQuery.finish();

        return result;
    }


    if (d->mSelectComposerByNameQuery.next()) {
        result = d->mSelectComposerByNameQuery.record().value(0).toULongLong();

        d->mSelectComposerByNameQuery.finish();

        return result;
    }

    d->mSelectComposerByNameQuery.finish();

    d->mInsertComposerQuery.bindValue(QStringLiteral(":composerId"), d->mComposerId);
    d->mInsertComposerQuery.bindValue(QStringLiteral(":name"), name);

    queryResult = execQuery(d->mInsertComposerQuery);

    if (!queryResult || !d->mInsertComposerQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertComposer" << d->mInsertComposerQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertComposer" << d->mInsertComposerQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertComposer" << d->mInsertComposerQuery.lastError();

        d->mInsertComposerQuery.finish();

        return result;
    }

    result = d->mComposerId;

    ++d->mComposerId;

    d->mInsertComposerQuery.finish();

    Q_EMIT composersAdded(internalAllComposersPartialData());

    return result;
}

qulonglong DatabaseInterface::insertGenre(const QString &name)
{
    auto result = qulonglong(0);

    if (name.isEmpty()) {
        return result;
    }

    d->mSelectGenreByNameQuery.bindValue(QStringLiteral(":name"), name);

    auto queryResult = execQuery(d->mSelectGenreByNameQuery);

    if (!queryResult || !d->mSelectGenreByNameQuery.isSelect() || !d->mSelectGenreByNameQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertGenre" << d->mSelectGenreByNameQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertGenre" << d->mSelectGenreByNameQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertGenre" << d->mSelectGenreByNameQuery.lastError();

        d->mSelectGenreByNameQuery.finish();

        return result;
    }

    if (d->mSelectGenreByNameQuery.next()) {
        result = d->mSelectGenreByNameQuery.record().value(0).toULongLong();

        d->mSelectGenreByNameQuery.finish();

        return result;
    }

    d->mSelectGenreByNameQuery.finish();

    d->mInsertGenreQuery.bindValue(QStringLiteral(":genreId"), d->mGenreId);
    d->mInsertGenreQuery.bindValue(QStringLiteral(":name"), name);

    queryResult = execQuery(d->mInsertGenreQuery);

    if (!queryResult || !d->mInsertGenreQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertGenre" << d->mInsertGenreQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertGenre" << d->mInsertGenreQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertGenre" << d->mInsertGenreQuery.lastError();

        d->mInsertGenreQuery.finish();

        return result;
    }

    result = d->mGenreId;

    ++d->mGenreId;

    d->mInsertGenreQuery.finish();

    Q_EMIT genresAdded({{{DataTypes::DatabaseIdRole, result},
                         {DataTypes::TitleRole, name},
                         {DataTypes::ElementTypeRole, ElisaUtils::Genre}}});

    return result;
}

void DatabaseInterface::insertTrackOrigin(const QUrl &fileNameURI, const QDateTime &fileModifiedTime,
                                          const QDateTime &importDate)
{
    d->mInsertTrackMapping.bindValue(QStringLiteral(":fileName"), fileNameURI);
    d->mInsertTrackMapping.bindValue(QStringLiteral(":priority"), 1);
    d->mInsertTrackMapping.bindValue(QStringLiteral(":mtime"), fileModifiedTime);
    d->mInsertTrackMapping.bindValue(QStringLiteral(":importDate"), importDate.toMSecsSinceEpoch());

    auto queryResult = execQuery(d->mInsertTrackMapping);

    if (!queryResult || !d->mInsertTrackMapping.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mInsertTrackMapping.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mInsertTrackMapping.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mInsertTrackMapping.lastError();

        d->mInsertTrackMapping.finish();

        return;
    }

    d->mInsertTrackMapping.finish();
}

void DatabaseInterface::updateTrackOrigin(const QUrl &fileName, const QDateTime &fileModifiedTime)
{
    d->mUpdateTrackFileModifiedTime.bindValue(QStringLiteral(":fileName"), fileName);
    d->mUpdateTrackFileModifiedTime.bindValue(QStringLiteral(":mtime"), fileModifiedTime);

    auto queryResult = execQuery(d->mUpdateTrackFileModifiedTime);

    if (!queryResult || !d->mUpdateTrackFileModifiedTime.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackOrigin" << d->mUpdateTrackFileModifiedTime.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackOrigin" << d->mUpdateTrackFileModifiedTime.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackOrigin" << d->mUpdateTrackFileModifiedTime.lastError();

        d->mUpdateTrackFileModifiedTime.finish();

        return;
    }

    d->mUpdateTrackFileModifiedTime.finish();
}

qulonglong DatabaseInterface::internalInsertTrack(const DataTypes::TrackDataType &oneTrack,
                                                  const QHash<QString, QUrl> &covers, bool &isInserted)
{
    qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalInsertTrack trying to insert" << oneTrack;

    qulonglong resultId = 0;

    if (oneTrack.title().isEmpty()) {
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalInsertTrack" << oneTrack << "is not inserted";
    }

    QUrl::FormattingOptions currentOptions = QUrl::PreferLocalFile |
            QUrl::RemoveAuthority | QUrl::RemoveFilename | QUrl::RemoveFragment |
            QUrl::RemovePassword | QUrl::RemovePort | QUrl::RemoveQuery |
            QUrl::RemoveScheme | QUrl::RemoveUserInfo;

    const auto &trackPath = oneTrack.resourceURI().toString(currentOptions);

    auto albumCover = covers[oneTrack.resourceURI().toString()];
    if (albumCover.isEmpty() && !covers.contains(oneTrack.resourceURI().toString())) {
        albumCover = oneTrack.albumCover();
    }

    auto albumId = insertAlbum(oneTrack.album(), (oneTrack.hasAlbumArtist() ? oneTrack.albumArtist() : QString()),
                               trackPath, oneTrack.hasEmbeddedCover() ? QUrl{} : albumCover);

    auto oldAlbumId = albumId;

    auto existingTrackId = internalTrackIdFromFileName(oneTrack.resourceURI());
    bool isModifiedTrack = (existingTrackId != 0);

    if (isModifiedTrack && !oneTrack.title().isEmpty()) {
        resultId = existingTrackId;

        bool albumInfoIsSame = true;

        auto oldTrack = internalTrackFromDatabaseId(existingTrackId);
        qCDebug(orgKdeElisaDatabase()) << "DatabaseInterface::internalInsertTrack" << existingTrackId << oldTrack;
        oldAlbumId = oldTrack.albumId();

        auto isSameTrack = true;
        isSameTrack = isSameTrack && (oldTrack.title() == oneTrack.title());
        isSameTrack = isSameTrack && (oldTrack.hasAlbum() == oneTrack.hasAlbum());
        albumInfoIsSame = albumInfoIsSame && (oldTrack.hasAlbum() == oneTrack.hasAlbum());
        if (isSameTrack && oldTrack.hasAlbum()) {
            isSameTrack = isSameTrack && (oldTrack.album() == oneTrack.album());
            albumInfoIsSame = albumInfoIsSame && (oldTrack.album() == oneTrack.album());
        }
        isSameTrack = isSameTrack && (oldTrack.hasArtist() == oneTrack.hasArtist());
        if (isSameTrack && oldTrack.hasArtist()) {
            isSameTrack = isSameTrack && (oldTrack.artist() == oneTrack.artist());
        }
        isSameTrack = isSameTrack && (oldTrack.hasAlbumArtist() == oneTrack.hasAlbumArtist());
        albumInfoIsSame = albumInfoIsSame && (oldTrack.hasAlbumArtist() == oneTrack.hasAlbumArtist());
        if (isSameTrack && oldTrack.hasAlbumArtist()) {
            isSameTrack = isSameTrack && (oldTrack.albumArtist() == oneTrack.albumArtist());
            albumInfoIsSame = albumInfoIsSame && (oldTrack.albumArtist() == oneTrack.albumArtist());
        }
        isSameTrack = isSameTrack && (oldTrack.hasTrackNumber() == oneTrack.hasTrackNumber());
        if (isSameTrack && oldTrack.hasTrackNumber()) {
            isSameTrack = isSameTrack && (oldTrack.trackNumber() == oneTrack.trackNumber());
        }
        isSameTrack = isSameTrack && (oldTrack.hasDiscNumber() == oneTrack.hasDiscNumber());
        if (isSameTrack && oldTrack.hasDiscNumber()) {
            isSameTrack = isSameTrack && (oldTrack.discNumber() == oneTrack.discNumber());
        }
        isSameTrack = isSameTrack && (oldTrack.duration() == oneTrack.duration());
        isSameTrack = isSameTrack && (oldTrack.rating() == oneTrack.rating());
        isSameTrack = isSameTrack && (oldTrack.resourceURI() == oneTrack.resourceURI());
        isSameTrack = isSameTrack && (oldTrack.hasGenre() == oneTrack.hasGenre());
        if (isSameTrack && oldTrack.hasGenre()) {
            isSameTrack = isSameTrack && (oldTrack.genre() == oneTrack.genre());
        }
        isSameTrack = isSameTrack && (oldTrack.hasComposer() == oneTrack.hasComposer());
        if (isSameTrack && oldTrack.hasComposer()) {
            isSameTrack = isSameTrack && (oldTrack.composer() == oneTrack.composer());
        }
        isSameTrack = isSameTrack && (oldTrack.hasLyricist() == oneTrack.hasLyricist());
        if (isSameTrack && oldTrack.hasLyricist()) {
            isSameTrack = isSameTrack && (oldTrack.lyricist() == oneTrack.lyricist());
        }
        isSameTrack = isSameTrack && (oldTrack.hasComment() == oneTrack.hasComment());
        if (isSameTrack && oldTrack.hasComment()) {
            isSameTrack = isSameTrack && (oldTrack.comment() == oneTrack.comment());
        }
        isSameTrack = isSameTrack && (oldTrack.hasYear() == oneTrack.hasYear());
        if (isSameTrack && oldTrack.hasYear()) {
            isSameTrack = isSameTrack && (oldTrack.year() == oneTrack.year());
        }
        isSameTrack = isSameTrack && (oldTrack.hasChannels() == oneTrack.hasChannels());
        if (isSameTrack && oldTrack.hasChannels()) {
            isSameTrack = isSameTrack && (oldTrack.channels() == oneTrack.channels());
        }
        isSameTrack = isSameTrack && (oldTrack.hasBitRate() == oneTrack.hasBitRate());
        if (isSameTrack && oldTrack.hasBitRate()) {
            isSameTrack = isSameTrack && (oldTrack.bitRate() == oneTrack.bitRate());
        }
        isSameTrack = isSameTrack && (oldTrack.hasSampleRate() == oneTrack.hasSampleRate());
        if (isSameTrack && oldTrack.hasSampleRate()) {
            isSameTrack = isSameTrack && (oldTrack.sampleRate() == oneTrack.sampleRate());
        }

        if (isSameTrack) {
            return resultId;
        }

        auto newTrack = oneTrack;
        newTrack[DataTypes::ColumnsRoles::DatabaseIdRole] = resultId;
        updateTrackInDatabase(newTrack, trackPath);
        updateTrackOrigin(oneTrack.resourceURI(), oneTrack.fileModificationTime());
        auto albumIsModified = updateAlbumFromId(albumId, oneTrack.hasEmbeddedCover() ? QUrl{} : oneTrack.albumCover(), oneTrack, trackPath);

        recordModifiedTrack(existingTrackId);
        if (albumIsModified && albumId != 0) {
            recordModifiedAlbum(albumId);
        }
        if (oldAlbumId != 0) {
            auto tracksCount = fetchTrackIds(oldAlbumId).count();

            if (tracksCount) {
                if (!albumInfoIsSame) {
                    recordModifiedAlbum(oldAlbumId);
                }
            } else {
                removeAlbumInDatabase(oldAlbumId);
                Q_EMIT albumRemoved(oldAlbumId);
            }
        }

        isInserted = false;

        return resultId;
    } else {
        oldAlbumId = 0;
        existingTrackId = d->mTrackId;

        isInserted = true;
    }

    int priority = 1;
    while(true) {
        auto otherTrackId = getDuplicateTrackIdFromTitleAlbumTrackDiscNumber(oneTrack.title(), oneTrack.artist(), oneTrack.album(),
                                                                             oneTrack.albumArtist(), trackPath, oneTrack.trackNumber(),
                                                                             oneTrack.discNumber(), priority);

        if (otherTrackId) {
            ++priority;
        } else {
            break;
        }
    }

    resultId = existingTrackId;

    if (!oneTrack.title().isEmpty()) {
        d->mInsertTrackQuery.bindValue(QStringLiteral(":trackId"), existingTrackId);
        d->mInsertTrackQuery.bindValue(QStringLiteral(":fileName"), oneTrack.resourceURI());
        d->mInsertTrackQuery.bindValue(QStringLiteral(":priority"), priority);
        d->mInsertTrackQuery.bindValue(QStringLiteral(":title"), oneTrack.title());
        if (oneTrack.hasArtist()) {
            insertArtist(oneTrack.artist());
            d->mInsertTrackQuery.bindValue(QStringLiteral(":artistName"), oneTrack.artist());
        } else {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":artistName"), {});
        }
        if (oneTrack.hasAlbum()) {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":albumTitle"), oneTrack.album());
        } else {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":albumTitle"), {});
        }
        if (oneTrack.hasAlbumArtist()) {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":albumArtistName"), oneTrack.albumArtist());
        } else {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":albumArtistName"), {});
        }
        d->mInsertTrackQuery.bindValue(QStringLiteral(":albumPath"), trackPath);
        if (oneTrack.hasTrackNumber()) {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":trackNumber"), oneTrack.trackNumber());
        } else {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":trackNumber"), {});
        }
        if (oneTrack.hasDiscNumber()) {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":discNumber"), oneTrack.discNumber());
        } else {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":discNumber"), {});
        }
        d->mInsertTrackQuery.bindValue(QStringLiteral(":trackDuration"), QVariant::fromValue<qlonglong>(oneTrack.duration().msecsSinceStartOfDay()));
        d->mInsertTrackQuery.bindValue(QStringLiteral(":trackRating"), oneTrack.rating());
        if (insertGenre(oneTrack.genre()) != 0) {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":genre"), oneTrack.genre());
        } else {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":genre"), {});
        }
        if (insertComposer(oneTrack.composer()) != 0) {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":composer"), oneTrack.composer());
        } else {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":composer"), {});
        }
        if (insertLyricist(oneTrack.lyricist()) != 0) {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":lyricist"), oneTrack.lyricist());
        } else {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":lyricist"), {});
        }
        if (oneTrack.hasComment()) {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":comment"), oneTrack.comment());
        } else {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":comment"), {});
        }
        if (oneTrack.hasYear()) {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":year"), oneTrack.year());
        } else {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":year"), {});
        }
        if (oneTrack.hasChannels()) {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":channels"), oneTrack.channels());
        } else {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":channels"), {});
        }
        if (oneTrack.hasBitRate()) {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":bitRate"), oneTrack.bitRate());
        } else {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":bitRate"), {});
        }
        if (oneTrack.hasSampleRate()) {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":sampleRate"), oneTrack.sampleRate());
        } else {
            d->mInsertTrackQuery.bindValue(QStringLiteral(":sampleRate"), {});
        }
        d->mInsertTrackQuery.bindValue(QStringLiteral(":hasEmbeddedCover"), oneTrack.hasEmbeddedCover());

        auto result = execQuery(d->mInsertTrackQuery);
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalInsertTrack" << oneTrack << "is inserted";

        if (result && d->mInsertTrackQuery.isActive()) {
            d->mInsertTrackQuery.finish();

            if (!isModifiedTrack) {
                ++d->mTrackId;
            }

            updateTrackOrigin(oneTrack.resourceURI(), oneTrack.fileModificationTime());

            if (isModifiedTrack) {
                recordModifiedTrack(existingTrackId);
                if (albumId != 0) {
                    recordModifiedAlbum(albumId);
                }
                if (oldAlbumId != 0) {
                    recordModifiedAlbum(oldAlbumId);
                }
            }

            if (albumId != 0) {
                if (updateAlbumFromId(albumId, covers[oneTrack.resourceURI().toString()], oneTrack, trackPath)) {
                    auto modifiedTracks = fetchTrackIds(albumId);
                    for (auto oneModifiedTrack : modifiedTracks) {
                        if (oneModifiedTrack != resultId) {
                            recordModifiedTrack(oneModifiedTrack);
                        }
                    }
                }
                recordModifiedAlbum(albumId);
            }
        } else {
            d->mInsertTrackQuery.finish();

            Q_EMIT databaseError();

            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalInsertTrack" << oneTrack << oneTrack.resourceURI();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalInsertTrack" << d->mInsertTrackQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalInsertTrack" << d->mInsertTrackQuery.boundValues();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalInsertTrack" << d->mInsertTrackQuery.lastError();
        }
    } else {
        if (!isModifiedTrack) {
            ++d->mTrackId;
        }

        updateTrackOrigin(oneTrack.resourceURI(), oneTrack.fileModificationTime());
    }

    return resultId;
}

DataTypes::TrackDataType DatabaseInterface::buildTrackDataFromDatabaseRecord(const QSqlRecord &trackRecord) const
{
    DataTypes::TrackDataType result;

    result[DataTypes::TrackDataType::key_type::DatabaseIdRole] = trackRecord.value(DatabaseInterfacePrivate::TrackId);
    result[DataTypes::TrackDataType::key_type::TitleRole] = trackRecord.value(DatabaseInterfacePrivate::TrackTitle);
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackAlbumTitle).isNull()) {
        result[DataTypes::TrackDataType::key_type::AlbumRole] = trackRecord.value(DatabaseInterfacePrivate::TrackAlbumTitle);
        result[DataTypes::TrackDataType::key_type::AlbumIdRole] = trackRecord.value(DatabaseInterfacePrivate::TrackAlbumId);
    }
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackArtistName).isNull()) {
        result[DataTypes::TrackDataType::key_type::ArtistRole] = trackRecord.value(DatabaseInterfacePrivate::TrackArtistName);
    }

    if (!trackRecord.value(DatabaseInterfacePrivate::TrackAlbumArtistName).isNull()) {
        result[DataTypes::TrackDataType::key_type::IsValidAlbumArtistRole] = true;
        result[DataTypes::TrackDataType::key_type::AlbumArtistRole] = trackRecord.value(DatabaseInterfacePrivate::TrackAlbumArtistName);
    } else {
        result[DataTypes::TrackDataType::key_type::IsValidAlbumArtistRole] = false;
        if (trackRecord.value(DatabaseInterfacePrivate::TrackArtistsCount).toInt() == 1) {
            result[DataTypes::TrackDataType::key_type::AlbumArtistRole] = trackRecord.value(DatabaseInterfacePrivate::TrackArtistName);
        } else if (trackRecord.value(DatabaseInterfacePrivate::TrackArtistsCount).toInt() > 1) {
            result[DataTypes::TrackDataType::key_type::AlbumArtistRole] = i18n("Various Artists");
        }
    }

    result[DataTypes::TrackDataType::key_type::ResourceRole] = trackRecord.value(DatabaseInterfacePrivate::TrackFileName);
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackNumber).isNull()) {
        result[DataTypes::TrackDataType::key_type::TrackNumberRole] = trackRecord.value(DatabaseInterfacePrivate::TrackNumber);
    }
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackDiscNumber).isNull()) {
        result[DataTypes::TrackDataType::key_type::DiscNumberRole] = trackRecord.value(DatabaseInterfacePrivate::TrackDiscNumber);
    }
    result[DataTypes::TrackDataType::key_type::DurationRole] = QTime::fromMSecsSinceStartOfDay(trackRecord.value(DatabaseInterfacePrivate::TrackDuration).toInt());
    result[DataTypes::TrackDataType::key_type::RatingRole] = trackRecord.value(DatabaseInterfacePrivate::TrackRating);
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackCoverFileName).toString().isEmpty()) {
        result[DataTypes::TrackDataType::key_type::ImageUrlRole] = QUrl(trackRecord.value(DatabaseInterfacePrivate::TrackCoverFileName).toString());
    } else if (!trackRecord.value(DatabaseInterfacePrivate::TrackEmbeddedCover).toString().isEmpty()) {
        result[DataTypes::TrackDataType::key_type::ImageUrlRole] = QVariant{QLatin1String("image://cover/") + trackRecord.value(DatabaseInterfacePrivate::TrackEmbeddedCover).toUrl().toLocalFile()};
    }
    result[DataTypes::TrackDataType::key_type::IsSingleDiscAlbumRole] = trackRecord.value(DatabaseInterfacePrivate::TrackIsSingleDiscAlbum);
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackGenreName).isNull()) {
        result[DataTypes::TrackDataType::key_type::GenreRole] = trackRecord.value(DatabaseInterfacePrivate::TrackGenreName);
    }
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackComposerName).isNull()) {
        result[DataTypes::TrackDataType::key_type::ComposerRole] = trackRecord.value(DatabaseInterfacePrivate::TrackComposerName);
    }
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackLyricistName).isNull()) {
        result[DataTypes::TrackDataType::key_type::LyricistRole] = trackRecord.value(DatabaseInterfacePrivate::TrackLyricistName);
    }
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackComment).isNull()) {
        result[DataTypes::TrackDataType::key_type::CommentRole] = trackRecord.value(DatabaseInterfacePrivate::TrackComment);
    }
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackYear).isNull()) {
        result[DataTypes::TrackDataType::key_type::YearRole] = trackRecord.value(DatabaseInterfacePrivate::TrackYear);
    }
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackChannelsCount).isNull()) {
        result[DataTypes::TrackDataType::key_type::ChannelsRole] = trackRecord.value(DatabaseInterfacePrivate::TrackChannelsCount);
    }
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackBitRate).isNull()) {
        result[DataTypes::TrackDataType::key_type::BitRateRole] = trackRecord.value(DatabaseInterfacePrivate::TrackBitRate);
    }
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackSamplerate).isNull()) {
        result[DataTypes::TrackDataType::key_type::SampleRateRole] = trackRecord.value(DatabaseInterfacePrivate::TrackSamplerate);
    }
    result[DataTypes::TrackDataType::key_type::HasEmbeddedCover] = trackRecord.value(DatabaseInterfacePrivate::TrackHasEmbeddedCover);
    result[DataTypes::TrackDataType::key_type::FileModificationTime] = trackRecord.value(DatabaseInterfacePrivate::TrackFileModifiedTime);
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackFirstPlayDate).isNull()) {
        result[DataTypes::TrackDataType::key_type::FirstPlayDate] = trackRecord.value(DatabaseInterfacePrivate::TrackFirstPlayDate);
    }
    if (!trackRecord.value(DatabaseInterfacePrivate::TrackLastPlayDate).isNull()) {
        result[DataTypes::TrackDataType::key_type::LastPlayDate] = trackRecord.value(DatabaseInterfacePrivate::TrackLastPlayDate);
    }
    result[DataTypes::TrackDataType::key_type::PlayCounter] = trackRecord.value(DatabaseInterfacePrivate::TrackPlayCounter);
    result[DataTypes::TrackDataType::key_type::PlayFrequency] = trackRecord.value(DatabaseInterfacePrivate::TrackPlayFrequency);
    result[DataTypes::TrackDataType::key_type::ElementTypeRole] = QVariant::fromValue(ElisaUtils::Track);

    return result;
}

DataTypes::TrackDataType DatabaseInterface::buildRadioDataFromDatabaseRecord(const QSqlRecord &trackRecord) const
{
    DataTypes::TrackDataType result;

    result[DataTypes::TrackDataType::key_type::DatabaseIdRole] = trackRecord.value(DatabaseInterfacePrivate::RadioId);
    result[DataTypes::TrackDataType::key_type::TitleRole] = trackRecord.value(DatabaseInterfacePrivate::RadioTitle);
    result[DataTypes::TrackDataType::key_type::AlbumRole] = i18n("Radios");
    result[DataTypes::TrackDataType::key_type::ResourceRole] = trackRecord.value(DatabaseInterfacePrivate::RadioHttpAddress);
    result[DataTypes::TrackDataType::key_type::ImageUrlRole] = trackRecord.value(DatabaseInterfacePrivate::RadioImageAddress);
    result[DataTypes::TrackDataType::key_type::RatingRole] = trackRecord.value(DatabaseInterfacePrivate::RadioRating);
    if (!trackRecord.value(DatabaseInterfacePrivate::RadioGenreName).isNull()) {
        result[DataTypes::TrackDataType::key_type::GenreRole] = trackRecord.value(DatabaseInterfacePrivate::RadioGenreName);
    }
    result[DataTypes::TrackDataType::key_type::CommentRole] = trackRecord.value(DatabaseInterfacePrivate::RadioComment);
    result[DataTypes::TrackDataType::key_type::ElementTypeRole] = ElisaUtils::Radio;

    return result;
}

void DatabaseInterface::internalRemoveTracksList(const QList<QUrl> &removedTracks)
{
    QSet<qulonglong> modifiedAlbums;

    QUrl::FormattingOptions currentOptions = QUrl::PreferLocalFile |
            QUrl::RemoveAuthority | QUrl::RemoveFilename | QUrl::RemoveFragment |
            QUrl::RemovePassword | QUrl::RemovePort | QUrl::RemoveQuery |
            QUrl::RemoveScheme | QUrl::RemoveUserInfo;

    for (const auto &removedTrackFileName : removedTracks) {
        auto removedTrackId = internalTrackIdFromFileName(removedTrackFileName);

        Q_EMIT trackRemoved(removedTrackId);

        auto oneRemovedTrack = internalTrackFromDatabaseId(removedTrackId);

        removeTrackInDatabase(removedTrackId);

        const auto &trackPath = oneRemovedTrack.resourceURI().toString(currentOptions);
        const auto &modifiedAlbumId = internalAlbumIdFromTitleAndArtist(oneRemovedTrack.album(), oneRemovedTrack.albumArtist(), trackPath);
        const auto &allTracksFromArtist = internalTracksFromAuthor(oneRemovedTrack.artist());
        const auto &allAlbumsFromArtist = internalAlbumIdsFromAuthor(oneRemovedTrack.artist());
        const auto &removedArtistId = internalArtistIdFromName(oneRemovedTrack.artist());

        if (modifiedAlbumId) {
            recordModifiedAlbum(modifiedAlbumId);
            modifiedAlbums.insert(modifiedAlbumId);
        }

        if (removedArtistId != 0 && allTracksFromArtist.isEmpty() && allAlbumsFromArtist.isEmpty()) {
            removeArtistInDatabase(removedArtistId);
            Q_EMIT artistRemoved(removedArtistId);
        }

        d->mRemoveTracksMapping.bindValue(QStringLiteral(":fileName"), removedTrackFileName.toString());

        auto result = execQuery(d->mRemoveTracksMapping);

        if (!result || !d->mRemoveTracksMapping.isActive()) {
            Q_EMIT databaseError();

            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalRemoveTracksList" << d->mRemoveTracksMapping.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalRemoveTracksList" << d->mRemoveTracksMapping.boundValues();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalRemoveTracksList" << d->mRemoveTracksMapping.lastError();

            continue;
        }

        d->mRemoveTracksMapping.finish();
    }

    for (auto modifiedAlbumId : modifiedAlbums) {
        const auto &modifiedAlbumData = internalOneAlbumPartialData(modifiedAlbumId);

        auto tracksCount = fetchTrackIds(modifiedAlbumId).count();

        if (!modifiedAlbumData.isEmpty() && tracksCount) {
            auto modifiedAlbum = internalOneAlbumData(modifiedAlbumId);
            if (updateAlbumFromId(modifiedAlbumId, modifiedAlbum.at(0).albumCover(), modifiedAlbum.at(0), modifiedAlbum.at(0).resourceURI().toString(currentOptions))) {
                for (const auto &oneTrack : modifiedAlbum) {
                    recordModifiedTrack(oneTrack.databaseId());
                }
            }

            Q_EMIT albumModified({{DataTypes::DatabaseIdRole, modifiedAlbumId}}, modifiedAlbumId);
        } else {
            removeAlbumInDatabase(modifiedAlbumId);
            Q_EMIT albumRemoved(modifiedAlbumId);

            const auto &allTracksFromArtist = internalTracksFromAuthor(modifiedAlbumData[DataTypes::AlbumDataType::key_type::ArtistRole].toString());
            const auto &allAlbumsFromArtist = internalAlbumIdsFromAuthor(modifiedAlbumData[DataTypes::AlbumDataType::key_type::ArtistRole].toString());
            const auto &removedArtistId = internalArtistIdFromName(modifiedAlbumData[DataTypes::AlbumDataType::key_type::ArtistRole].toString());

            if (removedArtistId != 0 && allTracksFromArtist.isEmpty() && allAlbumsFromArtist.isEmpty()) {
                removeArtistInDatabase(removedArtistId);
                Q_EMIT artistRemoved(removedArtistId);
            }
        }
    }
}

QUrl DatabaseInterface::internalAlbumArtUriFromAlbumId(qulonglong albumId)
{
    auto result = QUrl();

    d->mSelectAlbumArtUriFromAlbumIdQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto queryResult = execQuery(d->mSelectAlbumArtUriFromAlbumIdQuery);

    if (!queryResult || !d->mSelectAlbumArtUriFromAlbumIdQuery.isSelect() || !d->mSelectAlbumArtUriFromAlbumIdQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectAlbumArtUriFromAlbumIdQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectAlbumArtUriFromAlbumIdQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectAlbumArtUriFromAlbumIdQuery.lastError();

        d->mSelectAlbumArtUriFromAlbumIdQuery.finish();

        return result;
    }

    if (!d->mSelectAlbumArtUriFromAlbumIdQuery.next()) {
        d->mSelectAlbumArtUriFromAlbumIdQuery.finish();

        return result;
    }

    result = d->mSelectAlbumArtUriFromAlbumIdQuery.record().value(0).toUrl();

    d->mSelectAlbumArtUriFromAlbumIdQuery.finish();

    return result;
}

bool DatabaseInterface::isValidArtist(qulonglong albumId)
{
    auto result = false;

    d->mSelectAlbumQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto queryResult = execQuery(d->mSelectAlbumQuery);

    if (!queryResult || !d->mSelectAlbumQuery.isSelect() || !d->mSelectAlbumQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumFromId" << d->mSelectAlbumQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumFromId" << d->mSelectAlbumQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumFromId" << d->mSelectAlbumQuery.lastError();

        d->mSelectAlbumQuery.finish();

        return result;
    }

    if (!d->mSelectAlbumQuery.next()) {
        d->mSelectAlbumQuery.finish();

        return result;
    }

    const auto &currentRecord = d->mSelectAlbumQuery.record();

    result = !currentRecord.value(2).toString().isEmpty();

    return result;
}

bool DatabaseInterface::internalGenericPartialData(QSqlQuery &query)
{
    auto result = false;

    auto queryResult = execQuery(query);

    if (!queryResult || !query.isSelect() || !query.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAllGenericPartialData" << query.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAllGenericPartialData" << query.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAllGenericPartialData" << query.lastError();

        query.finish();

        auto transactionResult = finishTransaction();
        if (!transactionResult) {
            return result;
        }

        return result;
    }

    result = true;

    return result;
}

qulonglong DatabaseInterface::insertLyricist(const QString &name)
{
    auto result = qulonglong(0);

    if (name.isEmpty()) {
        return result;
    }

    d->mSelectLyricistByNameQuery.bindValue(QStringLiteral(":name"), name);

    auto queryResult = execQuery(d->mSelectLyricistByNameQuery);

    if (!queryResult || !d->mSelectLyricistByNameQuery.isSelect() || !d->mSelectLyricistByNameQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertLyricist" << d->mSelectLyricistByNameQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertLyricist" << d->mSelectLyricistByNameQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertLyricist" << d->mSelectLyricistByNameQuery.lastError();

        d->mSelectLyricistByNameQuery.finish();

        return result;
    }

    if (d->mSelectLyricistByNameQuery.next()) {
        result = d->mSelectLyricistByNameQuery.record().value(0).toULongLong();

        d->mSelectLyricistByNameQuery.finish();

        return result;
    }

    d->mSelectLyricistByNameQuery.finish();

    d->mInsertLyricistQuery.bindValue(QStringLiteral(":lyricistId"), d->mLyricistId);
    d->mInsertLyricistQuery.bindValue(QStringLiteral(":name"), name);

    queryResult = execQuery(d->mInsertLyricistQuery);

    if (!queryResult || !d->mInsertLyricistQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertLyricist" << d->mInsertLyricistQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertLyricist" << d->mInsertLyricistQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertLyricist" << d->mInsertLyricistQuery.lastError();

        d->mInsertLyricistQuery.finish();

        return result;
    }

    result = d->mLyricistId;

    ++d->mLyricistId;

    d->mInsertLyricistQuery.finish();

    Q_EMIT lyricistsAdded(internalAllLyricistsPartialData());

    return result;
}

QHash<QUrl, QDateTime> DatabaseInterface::internalAllFileName()
{
    auto allFileNames = QHash<QUrl, QDateTime>{};

    auto queryResult = execQuery(d->mSelectAllTrackFilesQuery);

    if (!queryResult || !d->mSelectAllTrackFilesQuery.isSelect() || !d->mSelectAllTrackFilesQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertMusicSource" << d->mSelectAllTrackFilesQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertMusicSource" << d->mSelectAllTrackFilesQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertMusicSource" << d->mSelectAllTrackFilesQuery.lastError();

        d->mSelectAllTrackFilesQuery.finish();

        return allFileNames;
    }

    while(d->mSelectAllTrackFilesQuery.next()) {
        auto fileName = d->mSelectAllTrackFilesQuery.record().value(0).toUrl();
        auto fileModificationTime = d->mSelectAllTrackFilesQuery.record().value(1).toDateTime();

        allFileNames[fileName] = fileModificationTime;
    }

    d->mSelectAllTrackFilesQuery.finish();

    return allFileNames;
}

qulonglong DatabaseInterface::internalArtistIdFromName(const QString &name)
{
    auto result = qulonglong(0);

    if (name.isEmpty()) {
        return result;
    }

    d->mSelectArtistByNameQuery.bindValue(QStringLiteral(":name"), name);

    auto queryResult = execQuery(d->mSelectArtistByNameQuery);

    if (!queryResult || !d->mSelectArtistByNameQuery.isSelect() || !d->mSelectArtistByNameQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.lastError();

        d->mSelectArtistByNameQuery.finish();

        return result;
    }

    if (!d->mSelectArtistByNameQuery.next()) {
        d->mSelectArtistByNameQuery.finish();

        return result;
    }

    result = d->mSelectArtistByNameQuery.record().value(0).toULongLong();

    d->mSelectArtistByNameQuery.finish();

    return result;
}

void DatabaseInterface::removeTrackInDatabase(qulonglong trackId)
{
    d->mRemoveTrackQuery.bindValue(QStringLiteral(":trackId"), trackId);

    auto result = execQuery(d->mRemoveTrackQuery);

    if (!result || !d->mRemoveTrackQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackQuery.lastError();
    }

    d->mRemoveTrackQuery.finish();
}

void DatabaseInterface::updateTrackInDatabase(const DataTypes::TrackDataType &oneTrack, const QString &albumPath)
{
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":fileName"), oneTrack.resourceURI());
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":trackId"), oneTrack.databaseId());
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":title"), oneTrack.title());
    if (oneTrack.hasArtist()) {
        insertArtist(oneTrack.artist());
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":artistName"), oneTrack.artist());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":artistName"), {});
    }
    if (oneTrack.hasAlbum()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":albumTitle"), oneTrack.album());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":albumTitle"), {});
    }
    if (oneTrack.hasAlbumArtist()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":albumArtistName"), oneTrack.albumArtist());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":albumArtistName"), {});
    }
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":albumPath"), albumPath);
    if (oneTrack.hasTrackNumber()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":trackNumber"), oneTrack.trackNumber());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":trackNumber"), {});
    }
    if (oneTrack.hasDiscNumber()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":discNumber"), oneTrack.discNumber());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":discNumber"), {});
    }
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":trackDuration"), QVariant::fromValue<qlonglong>(oneTrack.duration().msecsSinceStartOfDay()));
    d->mUpdateTrackQuery.bindValue(QStringLiteral(":trackRating"), oneTrack.rating());
    if (oneTrack.hasGenre()) {
        if (insertGenre(oneTrack.genre()) != 0) {
            d->mUpdateTrackQuery.bindValue(QStringLiteral(":genre"), oneTrack.genre());
        } else {
            d->mUpdateTrackQuery.bindValue(QStringLiteral(":genre"), {});
        }
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":genre"), {});
    }
    if (oneTrack.hasComposer()) {
        if (insertComposer(oneTrack.composer()) != 0) {
            d->mUpdateTrackQuery.bindValue(QStringLiteral(":composer"), oneTrack.composer());
        } else {
            d->mUpdateTrackQuery.bindValue(QStringLiteral(":composer"), {});
        }
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":composer"), {});
    }
    if (oneTrack.hasLyricist()) {
        if (insertLyricist(oneTrack.lyricist()) != 0) {
            d->mUpdateTrackQuery.bindValue(QStringLiteral(":lyricist"), oneTrack.lyricist());
        } else {
            d->mUpdateTrackQuery.bindValue(QStringLiteral(":lyricist"), {});
        }
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":lyricist"), {});
    }
    if (oneTrack.hasYear()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":comment"), oneTrack.comment());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":comment"), {});
    }
    if (oneTrack.hasYear()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":year"), oneTrack.year());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":year"), {});
    }
    if (oneTrack.hasChannels()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":channels"), oneTrack.channels());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":channels"), {});
    }
    if (oneTrack.hasBitRate()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":bitRate"), oneTrack.bitRate());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":bitRate"), {});
    }
    if (oneTrack.hasSampleRate()) {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":sampleRate"), oneTrack.sampleRate());
    } else {
        d->mUpdateTrackQuery.bindValue(QStringLiteral(":sampleRate"), {});
    }

    auto result = execQuery(d->mUpdateTrackQuery);

    if (!result || !d->mUpdateTrackQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackInDatabase" << d->mUpdateTrackQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackInDatabase" << d->mUpdateTrackQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackInDatabase" << d->mUpdateTrackQuery.lastError();
    }

    d->mUpdateTrackQuery.finish();
}

void DatabaseInterface::removeRadio(qulonglong radioId)
{
    QSqlQuery query = d->mDeleteRadioQuery;

    query.bindValue(QStringLiteral(":radioId"), radioId);

    auto result = execQuery(query);

    if (!result || !query.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackInDatabase" << query.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackInDatabase" << query.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackInDatabase" << query.lastError();
    }else{
        Q_EMIT radioRemoved(radioId);
    }

    query.finish();
}

void DatabaseInterface::removeAlbumInDatabase(qulonglong albumId)
{
    d->mRemoveAlbumQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = execQuery(d->mRemoveAlbumQuery);

    if (!result || !d->mRemoveAlbumQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeAlbumInDatabase" << d->mRemoveAlbumQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeAlbumInDatabase" << d->mRemoveAlbumQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeAlbumInDatabase" << d->mRemoveAlbumQuery.lastError();
    }

    d->mRemoveAlbumQuery.finish();
}

void DatabaseInterface::removeArtistInDatabase(qulonglong artistId)
{
    d->mRemoveArtistQuery.bindValue(QStringLiteral(":artistId"), artistId);

    auto result = execQuery(d->mRemoveArtistQuery);

    if (!result || !d->mRemoveArtistQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeArtistInDatabase" << d->mRemoveArtistQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeArtistInDatabase" << d->mRemoveArtistQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::removeArtistInDatabase" << d->mRemoveArtistQuery.lastError();
    }

    d->mRemoveArtistQuery.finish();
}

void DatabaseInterface::reloadExistingDatabase()
{
    qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::reloadExistingDatabase";

    d->mArtistId = genericInitialId(d->mQueryMaximumArtistIdQuery);
    d->mComposerId = genericInitialId(d->mQueryMaximumComposerIdQuery);
    d->mLyricistId = genericInitialId(d->mQueryMaximumLyricistIdQuery);
    d->mAlbumId = genericInitialId(d->mQueryMaximumAlbumIdQuery);
    d->mTrackId = genericInitialId(d->mQueryMaximumTrackIdQuery);
    d->mGenreId = genericInitialId(d->mQueryMaximumGenreIdQuery);
}

qulonglong DatabaseInterface::genericInitialId(QSqlQuery &request)
{
    auto result = qulonglong(0);

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    auto queryResult = execQuery(request);

    if (!queryResult || !request.isSelect() || !request.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertMusicSource" << request.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertMusicSource" << request.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::insertMusicSource" << request.lastError();

        request.finish();

        transactionResult = finishTransaction();
        if (!transactionResult) {
            return result;
        }

        return result;
    }

    if (request.next()) {
        result = request.record().value(0).toULongLong() + 1;

        request.finish();
    }

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

QList<qulonglong> DatabaseInterface::fetchTrackIds(qulonglong albumId)
{
    auto allTracks = QList<qulonglong>();

    d->mSelectTrackIdQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = execQuery(d->mSelectTrackIdQuery);

    if (!result || !d->mSelectTrackIdQuery.isSelect() || !d->mSelectTrackIdQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::fetchTrackIds" << d->mSelectTrackIdQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::fetchTrackIds" << d->mSelectTrackIdQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::fetchTrackIds" << d->mSelectTrackIdQuery.lastError();
    }

    while (d->mSelectTrackIdQuery.next()) {
        const auto &currentRecord = d->mSelectTrackIdQuery.record();

        allTracks.push_back(currentRecord.value(0).toULongLong());
    }

    d->mSelectTrackIdQuery.finish();

    return allTracks;
}

qulonglong DatabaseInterface::internalAlbumIdFromTitleAndArtist(const QString &title, const QString &artist, const QString &albumPath)
{
    auto result = qulonglong(0);

    d->mSelectAlbumIdFromTitleQuery.bindValue(QStringLiteral(":title"), title);
    d->mSelectAlbumIdFromTitleQuery.bindValue(QStringLiteral(":artistName"), artist);

    auto queryResult = execQuery(d->mSelectAlbumIdFromTitleQuery);

    if (!queryResult || !d->mSelectAlbumIdFromTitleQuery.isSelect() || !d->mSelectAlbumIdFromTitleQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleQuery.lastError();

        d->mSelectAlbumIdFromTitleQuery.finish();

        return result;
    }

    if (d->mSelectAlbumIdFromTitleQuery.next()) {
        result = d->mSelectAlbumIdFromTitleQuery.record().value(0).toULongLong();
    }

    d->mSelectAlbumIdFromTitleQuery.finish();

    if (result == 0) {
        d->mSelectAlbumIdFromTitleWithoutArtistQuery.bindValue(QStringLiteral(":title"), title);
        d->mSelectAlbumIdFromTitleWithoutArtistQuery.bindValue(QStringLiteral(":albumPath"), albumPath);

        queryResult = execQuery(d->mSelectAlbumIdFromTitleWithoutArtistQuery);

        if (!queryResult || !d->mSelectAlbumIdFromTitleWithoutArtistQuery.isSelect() || !d->mSelectAlbumIdFromTitleWithoutArtistQuery.isActive()) {
            Q_EMIT databaseError();

            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.boundValues();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastError();

            d->mSelectAlbumIdFromTitleWithoutArtistQuery.finish();

            return result;
        }

        if (d->mSelectAlbumIdFromTitleWithoutArtistQuery.next()) {
            result = d->mSelectAlbumIdFromTitleWithoutArtistQuery.record().value(0).toULongLong();
        }

        d->mSelectAlbumIdFromTitleWithoutArtistQuery.finish();
    }

    return result;
}

DataTypes::TrackDataType DatabaseInterface::internalTrackFromDatabaseId(qulonglong id)
{
    auto result = DataTypes::TrackDataType();

    if (result.isValid()) {
        return result;
    }

    if (!d || !d->mTracksDatabase.isValid() || !d->mInitFinished) {
        return result;
    }

    d->mSelectTrackFromIdQuery.bindValue(QStringLiteral(":trackId"), id);

    auto queryResult = execQuery(d->mSelectTrackFromIdQuery);

    if (!queryResult || !d->mSelectTrackFromIdQuery.isSelect() || !d->mSelectTrackFromIdQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackFromDatabaseId" << d->mSelectTrackFromIdQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackFromDatabaseId" << d->mSelectTrackFromIdQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackFromDatabaseId" << d->mSelectTrackFromIdQuery.lastError();

        d->mSelectTrackFromIdQuery.finish();

        return result;
    }

    if (!d->mSelectTrackFromIdQuery.next()) {
        d->mSelectTrackFromIdQuery.finish();

        return result;
    }

    const auto &currentRecord = d->mSelectTrackFromIdQuery.record();

    result = buildTrackDataFromDatabaseRecord(currentRecord);

    d->mSelectTrackFromIdQuery.finish();

    return result;
}

qulonglong DatabaseInterface::internalTrackIdFromTitleAlbumTracDiscNumber(const QString &title, const QString &artist, const std::optional<QString> &album,
                                                                          std::optional<int> trackNumber, std::optional<int> discNumber)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":title"), title);
    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":artist"), artist);
    if (album.has_value()) {
        d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":album"), album.value());
    } else {
        d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":album"), {});
    }
    if (trackNumber.has_value()) {
        d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":trackNumber"), trackNumber.value());
    } else {
        d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":trackNumber"), {});
    }
    if (discNumber.has_value()) {
        d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":discNumber"), discNumber.value());
    } else {
        d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":discNumber"), {});
    }

    auto queryResult = execQuery(d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery);

    if (!queryResult || !d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.isSelect() || !d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.lastError();

        d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.finish();

        return result;
    }

    if (d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.next()) {
        result = d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.record().value(0).toULongLong();
    }

    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.finish();

    return result;
}

qulonglong DatabaseInterface::getDuplicateTrackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &trackArtist,
                                                                               const QString &album, const QString &albumArtist,
                                                                               const QString &trackPath, int trackNumber,
                                                                               int discNumber, int priority)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":title"), title);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":trackArtist"), trackArtist);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":album"), album);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":albumPath"), trackPath);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":albumArtist"), albumArtist);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":trackNumber"), trackNumber);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":discNumber"), discNumber);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":priority"), priority);

    auto queryResult = execQuery(d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery);

    if (!queryResult || !d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.isSelect() || !d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.lastError();

        d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.finish();

        return result;
    }

    if (d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.next()) {
        result = d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.record().value(0).toULongLong();
    }

    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.finish();

    return result;
}

qulonglong DatabaseInterface::internalTrackIdFromFileName(const QUrl &fileName)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    d->mSelectTracksMapping.bindValue(QStringLiteral(":fileName"), fileName.toString());

    auto queryResult = execQuery(d->mSelectTracksMapping);

    if (!queryResult || !d->mSelectTracksMapping.isSelect() || !d->mSelectTracksMapping.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMapping.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMapping.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMapping.lastError();

        d->mSelectTracksMapping.finish();

        return result;
    }

    if (d->mSelectTracksMapping.next()) {
        const auto &currentRecordValue = d->mSelectTracksMapping.record().value(0);
        if (currentRecordValue.isValid()) {
            result = currentRecordValue.toULongLong();
        }
    }

    d->mSelectTracksMapping.finish();

    return result;
}

qulonglong DatabaseInterface::internalRadioIdFromHttpAddress(const QString &httpAddress)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    d->mSelectRadioIdFromHttpAddress.bindValue(QStringLiteral(":httpAddress"), httpAddress);

    auto queryResult = execQuery(d->mSelectRadioIdFromHttpAddress);

    if (!queryResult || !d->mSelectRadioIdFromHttpAddress.isSelect() || !d->mSelectRadioIdFromHttpAddress.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectRadioIdFromHttpAddress.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectRadioIdFromHttpAddress.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectRadioIdFromHttpAddress.lastError();

        d->mSelectRadioIdFromHttpAddress.finish();

        return result;
    }

    if (d->mSelectRadioIdFromHttpAddress.next()) {
        const auto &currentRecordValue = d->mSelectRadioIdFromHttpAddress.record().value(0);
        if (currentRecordValue.isValid()) {
            result = currentRecordValue.toULongLong();
        }
    }

    d->mSelectRadioIdFromHttpAddress.finish();

    return result;
}

DataTypes::ListTrackDataType DatabaseInterface::internalTracksFromAuthor(const QString &ArtistName)
{
    auto allTracks = DataTypes::ListTrackDataType{};

    d->mSelectTracksFromArtist.bindValue(QStringLiteral(":artistName"), ArtistName);

    auto result = execQuery(d->mSelectTracksFromArtist);

    if (!result || !d->mSelectTracksFromArtist.isSelect() || !d->mSelectTracksFromArtist.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromAuthor" << d->mSelectTracksFromArtist.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromAuthor" << d->mSelectTracksFromArtist.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromAuthor" << d->mSelectTracksFromArtist.lastError();

        return allTracks;
    }

    while (d->mSelectTracksFromArtist.next()) {
        const auto &currentRecord = d->mSelectTracksFromArtist.record();

        allTracks.push_back(buildTrackDataFromDatabaseRecord(currentRecord));
    }

    d->mSelectTracksFromArtist.finish();

    return allTracks;
}

DataTypes::ListTrackDataType DatabaseInterface::internalTracksFromGenre(const QString &genre)
{
    auto allTracks = DataTypes::ListTrackDataType{};

    d->mSelectTracksFromGenre.bindValue(QStringLiteral(":genre"), genre);

    auto result = execQuery(d->mSelectTracksFromGenre);

    if (!result || !d->mSelectTracksFromGenre.isSelect() || !d->mSelectTracksFromGenre.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromGenre" << d->mSelectTracksFromGenre.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromGenre" << d->mSelectTracksFromGenre.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromGenre" << d->mSelectTracksFromGenre.lastError();

        return allTracks;
    }

    while (d->mSelectTracksFromGenre.next()) {
        const auto &currentRecord = d->mSelectTracksFromGenre.record();

        allTracks.push_back(buildTrackDataFromDatabaseRecord(currentRecord));
    }

    d->mSelectTracksFromGenre.finish();

    return allTracks;
}


QList<qulonglong> DatabaseInterface::internalAlbumIdsFromAuthor(const QString &ArtistName)
{
    auto allAlbumIds = QList<qulonglong>();

    d->mSelectAlbumIdsFromArtist.bindValue(QStringLiteral(":artistName"), ArtistName);

    auto result = execQuery(d->mSelectAlbumIdsFromArtist);

    if (!result || !d->mSelectAlbumIdsFromArtist.isSelect() || !d->mSelectAlbumIdsFromArtist.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromAuthor" << d->mSelectAlbumIdsFromArtist.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromAuthor" << d->mSelectAlbumIdsFromArtist.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::tracksFromAuthor" << d->mSelectAlbumIdsFromArtist.lastError();

        return allAlbumIds;
    }

    while (d->mSelectAlbumIdsFromArtist.next()) {
        const auto &currentRecord = d->mSelectAlbumIdsFromArtist.record();

        allAlbumIds.push_back(currentRecord.value(0).toULongLong());
    }

    d->mSelectAlbumIdsFromArtist.finish();

    return allAlbumIds;
}

DataTypes::ListArtistDataType DatabaseInterface::internalAllArtistsPartialData(QSqlQuery &artistsQuery)
{
    auto result = DataTypes::ListArtistDataType{};

    if (!internalGenericPartialData(artistsQuery)) {
        return result;
    }

    while(artistsQuery.next()) {
        auto newData = DataTypes::ArtistDataType{};

        const auto &currentRecord = artistsQuery.record();

        newData[DataTypes::DatabaseIdRole] = currentRecord.value(0);
        newData[DataTypes::TitleRole] = currentRecord.value(1);
        newData[DataTypes::GenreRole] = QVariant::fromValue(currentRecord.value(2).toString().split(QStringLiteral(", ")));
        newData[DataTypes::ElementTypeRole] = ElisaUtils::Artist;

        result.push_back(newData);
    }

    artistsQuery.finish();

    return result;
}

DataTypes::ListAlbumDataType DatabaseInterface::internalAllAlbumsPartialData(QSqlQuery &query)
{
    auto result = DataTypes::ListAlbumDataType{};

    if (!internalGenericPartialData(query)) {
        return result;
    }

    while(query.next()) {
        auto newData = DataTypes::AlbumDataType{};

        const auto &currentRecord = query.record();

        newData[DataTypes::DatabaseIdRole] = currentRecord.value(DatabaseInterfacePrivate::AlbumsId);
        newData[DataTypes::TitleRole] = currentRecord.value(DatabaseInterfacePrivate::AlbumsTitle);
        if (!currentRecord.value(DatabaseInterfacePrivate::AlbumsCoverFileName).toString().isEmpty()) {
            newData[DataTypes::ImageUrlRole] = currentRecord.value(DatabaseInterfacePrivate::AlbumsCoverFileName);
        } else if (!currentRecord.value(DatabaseInterfacePrivate::AlbumsEmbeddedCover).toString().isEmpty()) {
            newData[DataTypes::ImageUrlRole] = QVariant{QLatin1String("image://cover/") + currentRecord.value(DatabaseInterfacePrivate::AlbumsEmbeddedCover).toUrl().toLocalFile()};
        }
        auto allArtists = currentRecord.value(DatabaseInterfacePrivate::AlbumsAllArtists).toString().split(QStringLiteral(", "));
        allArtists.removeDuplicates();
        newData[DataTypes::AllArtistsRole] = QVariant::fromValue(allArtists);
        if (!currentRecord.value(DatabaseInterfacePrivate::AlbumsArtistName).isNull()) {
            newData[DataTypes::IsValidAlbumArtistRole] = true;
            newData[DataTypes::SecondaryTextRole] = currentRecord.value(DatabaseInterfacePrivate::AlbumsArtistName);
        } else {
            newData[DataTypes::IsValidAlbumArtistRole] = false;
            if (currentRecord.value(DatabaseInterfacePrivate::AlbumsArtistsCount).toInt() == 1) {
                newData[DataTypes::SecondaryTextRole] = allArtists.first();
            } else if (currentRecord.value(DatabaseInterfacePrivate::AlbumsArtistsCount).toInt() > 1) {
                newData[DataTypes::SecondaryTextRole] = i18n("Various Artists");
            }
        }
        newData[DataTypes::ArtistRole] = newData[DataTypes::SecondaryTextRole];
        newData[DataTypes::HighestTrackRating] = currentRecord.value(DatabaseInterfacePrivate::AlbumsHighestRating);
        newData[DataTypes::IsSingleDiscAlbumRole] = currentRecord.value(DatabaseInterfacePrivate::AlbumsIsSingleDiscAlbum);
        newData[DataTypes::GenreRole] = QVariant::fromValue(currentRecord.value(DatabaseInterfacePrivate::AlbumsAllGenres).toString().split(QStringLiteral(", ")));
        auto allYears = currentRecord.value(DatabaseInterfacePrivate::AlbumsYear).toString().split(QStringLiteral(", "));
        allYears.removeDuplicates();
        if (allYears.size() == 1) {
            newData[DataTypes::YearRole] = allYears.at(0).toInt();
        } else {
            newData[DataTypes::YearRole] = 0;
        }
        newData[DataTypes::ElementTypeRole] = ElisaUtils::Album;

        result.push_back(newData);
    }

    query.finish();

    return result;
}

DataTypes::ListTrackDataType DatabaseInterface::internalOneAlbumData(qulonglong databaseId)
{
    DataTypes::ListTrackDataType result;

    d->mSelectTrackQuery.bindValue(QStringLiteral(":albumId"), databaseId);

    auto queryResult = execQuery(d->mSelectTrackQuery);

    if (!queryResult || !d->mSelectTrackQuery.isSelect() || !d->mSelectTrackQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::albumData" << d->mSelectTrackQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::albumData" << d->mSelectTrackQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::albumData" << d->mSelectTrackQuery.lastError();
    }

    while (d->mSelectTrackQuery.next()) {
        const auto &currentRecord = d->mSelectTrackQuery.record();

        result.push_back(buildTrackDataFromDatabaseRecord(currentRecord));
    }

    d->mSelectTrackQuery.finish();

    return result;
}

DataTypes::AlbumDataType DatabaseInterface::internalOneAlbumPartialData(qulonglong databaseId)
{
    auto result = DataTypes::AlbumDataType{};

    d->mSelectAlbumQuery.bindValue(QStringLiteral(":albumId"), databaseId);

    if (!internalGenericPartialData(d->mSelectAlbumQuery)) {
        return result;
    }

    if (d->mSelectAlbumQuery.next()) {
        const auto &currentRecord = d->mSelectAlbumQuery.record();

        result[DataTypes::DatabaseIdRole] = currentRecord.value(DatabaseInterfacePrivate::SingleAlbumId);
        result[DataTypes::TitleRole] = currentRecord.value(DatabaseInterfacePrivate::SingleAlbumTitle);
        if (!currentRecord.value(DatabaseInterfacePrivate::SingleAlbumCoverFileName).toString().isEmpty()) {
            result[DataTypes::ImageUrlRole] = currentRecord.value(DatabaseInterfacePrivate::SingleAlbumCoverFileName);
        } else if (!currentRecord.value(DatabaseInterfacePrivate::SingleAlbumEmbeddedCover).toString().isEmpty()) {
            result[DataTypes::ImageUrlRole] = QVariant{QLatin1String("image://cover/") + currentRecord.value(DatabaseInterfacePrivate::SingleAlbumEmbeddedCover).toUrl().toLocalFile()};
        }

        auto allArtists = currentRecord.value(DatabaseInterfacePrivate::SingleAlbumAllArtists).toString().split(QStringLiteral(", "));
        allArtists.removeDuplicates();
        result[DataTypes::AllArtistsRole] = QVariant::fromValue(allArtists);

        if (!currentRecord.value(DatabaseInterfacePrivate::SingleAlbumArtistName).isNull()) {
            result[DataTypes::IsValidAlbumArtistRole] = true;
            result[DataTypes::SecondaryTextRole] = currentRecord.value(DatabaseInterfacePrivate::SingleAlbumArtistName);
        } else {
            result[DataTypes::IsValidAlbumArtistRole] = false;
            if (currentRecord.value(DatabaseInterfacePrivate::SingleAlbumArtistsCount).toInt() == 1) {
                result[DataTypes::SecondaryTextRole] = allArtists.first();
            } else if (currentRecord.value(DatabaseInterfacePrivate::SingleAlbumArtistsCount).toInt() > 1) {
                result[DataTypes::SecondaryTextRole] = i18n("Various Artists");
            }
        }
        result[DataTypes::ArtistRole] = result[DataTypes::SecondaryTextRole];
        result[DataTypes::HighestTrackRating] = currentRecord.value(DatabaseInterfacePrivate::SingleAlbumHighestRating);
        result[DataTypes::IsSingleDiscAlbumRole] = currentRecord.value(DatabaseInterfacePrivate::SingleAlbumIsSingleDiscAlbum);
        result[DataTypes::GenreRole] = QVariant::fromValue(currentRecord.value(DatabaseInterfacePrivate::SingleAlbumAllGenres).toString().split(QStringLiteral(", ")));
        result[DataTypes::ElementTypeRole] = ElisaUtils::Album;

    }

    d->mSelectAlbumQuery.finish();

    return result;
}

DataTypes::ArtistDataType DatabaseInterface::internalOneArtistPartialData(qulonglong databaseId)
{
    auto result = DataTypes::ArtistDataType{};

    d->mSelectArtistQuery.bindValue(QStringLiteral(":artistId"), databaseId);

    if (!internalGenericPartialData(d->mSelectArtistQuery)) {
        return result;
    }

    if (d->mSelectArtistQuery.next()) {
        const auto &currentRecord = d->mSelectArtistQuery.record();

        result[DataTypes::DatabaseIdRole] = currentRecord.value(0);
        result[DataTypes::TitleRole] = currentRecord.value(1);
        result[DataTypes::GenreRole] = QVariant::fromValue(currentRecord.value(2).toString().split(QStringLiteral(", ")));
        result[DataTypes::ElementTypeRole] = ElisaUtils::Artist;
    }

    d->mSelectArtistQuery.finish();

    return result;
}

DataTypes::ListTrackDataType DatabaseInterface::internalAllTracksPartialData()
{
    auto result = DataTypes::ListTrackDataType{};

    if (!internalGenericPartialData(d->mSelectAllTracksQuery)) {
        return result;
    }

    while(d->mSelectAllTracksQuery.next()) {
        const auto &currentRecord = d->mSelectAllTracksQuery.record();

        auto newData = buildTrackDataFromDatabaseRecord(currentRecord);

        result.push_back(newData);
    }

    d->mSelectAllTracksQuery.finish();

    return result;
}

DataTypes::ListRadioDataType DatabaseInterface::internalAllRadiosPartialData()
{
    auto result = DataTypes::ListRadioDataType{};

    if (!internalGenericPartialData(d->mSelectAllRadiosQuery)) {
        return result;
    }

    while(d->mSelectAllRadiosQuery.next()) {
        const auto &currentRecord = d->mSelectAllRadiosQuery.record();

        auto newData = buildRadioDataFromDatabaseRecord(currentRecord);

        result.push_back(newData);
    }

    d->mSelectAllRadiosQuery.finish();

    return result;
}

DataTypes::ListTrackDataType DatabaseInterface::internalRecentlyPlayedTracksData(int count)
{
    auto result = DataTypes::ListTrackDataType{};

    d->mSelectAllRecentlyPlayedTracksQuery.bindValue(QStringLiteral(":maximumResults"), count);

    if (!internalGenericPartialData(d->mSelectAllRecentlyPlayedTracksQuery)) {
        return result;
    }

    while(d->mSelectAllRecentlyPlayedTracksQuery.next()) {
        const auto &currentRecord = d->mSelectAllRecentlyPlayedTracksQuery.record();

        auto newData = buildTrackDataFromDatabaseRecord(currentRecord);

        result.push_back(newData);
    }

    d->mSelectAllRecentlyPlayedTracksQuery.finish();

    return result;
}

DataTypes::ListTrackDataType DatabaseInterface::internalFrequentlyPlayedTracksData(int count)
{
    auto result = DataTypes::ListTrackDataType{};

    d->mSelectAllFrequentlyPlayedTracksQuery.bindValue(QStringLiteral(":maximumResults"), count);

    if (!internalGenericPartialData(d->mSelectAllFrequentlyPlayedTracksQuery)) {
        return result;
    }

    while(d->mSelectAllFrequentlyPlayedTracksQuery.next()) {
        const auto &currentRecord = d->mSelectAllFrequentlyPlayedTracksQuery.record();

        auto newData = buildTrackDataFromDatabaseRecord(currentRecord);

        result.push_back(newData);
    }

    d->mSelectAllFrequentlyPlayedTracksQuery.finish();

    return result;
}

DataTypes::TrackDataType DatabaseInterface::internalOneTrackPartialData(qulonglong databaseId)
{
    auto result = DataTypes::TrackDataType{};

    d->mSelectTrackFromIdQuery.bindValue(QStringLiteral(":trackId"), databaseId);

    if (!internalGenericPartialData(d->mSelectTrackFromIdQuery)) {
        return result;
    }

    if (d->mSelectTrackFromIdQuery.next()) {
        const auto &currentRecord = d->mSelectTrackFromIdQuery.record();

        result = buildTrackDataFromDatabaseRecord(currentRecord);
    }

    d->mSelectTrackFromIdQuery.finish();

    return result;
}

DataTypes::TrackDataType DatabaseInterface::internalOneTrackPartialDataByIdAndUrl(qulonglong databaseId, const QUrl &trackUrl)
{
    auto result = DataTypes::TrackDataType{};

    d->mSelectTrackFromIdAndUrlQuery.bindValue(QStringLiteral(":trackId"), databaseId);
    d->mSelectTrackFromIdAndUrlQuery.bindValue(QStringLiteral(":trackUrl"), trackUrl);

    if (!internalGenericPartialData(d->mSelectTrackFromIdAndUrlQuery)) {
        return result;
    }

    if (d->mSelectTrackFromIdAndUrlQuery.next()) {
        const auto &currentRecord = d->mSelectTrackFromIdAndUrlQuery.record();

        result = buildTrackDataFromDatabaseRecord(currentRecord);
    }

    d->mSelectTrackFromIdAndUrlQuery.finish();

    return result;
}

DataTypes::TrackDataType DatabaseInterface::internalOneRadioPartialData(qulonglong databaseId)
{
    auto result = DataTypes::TrackDataType{};

    d->mSelectRadioFromIdQuery.bindValue(QStringLiteral(":radioId"), databaseId);

    if (!internalGenericPartialData(d->mSelectRadioFromIdQuery)) {
        return result;
    }

    if (d->mSelectRadioFromIdQuery.next()) {
        const auto &currentRecord = d->mSelectRadioFromIdQuery.record();

        result = buildRadioDataFromDatabaseRecord(currentRecord);
    }

    d->mSelectRadioFromIdQuery.finish();

    return result;
}

DataTypes::ListGenreDataType DatabaseInterface::internalAllGenresPartialData()
{
    DataTypes::ListGenreDataType result;

    if (!internalGenericPartialData(d->mSelectAllGenresQuery)) {
        return result;
    }

    while(d->mSelectAllGenresQuery.next()) {
        auto newData = DataTypes::GenreDataType{};

        const auto &currentRecord = d->mSelectAllGenresQuery.record();

        newData[DataTypes::DatabaseIdRole] = currentRecord.value(0);
        newData[DataTypes::TitleRole] = currentRecord.value(1);
        newData[DataTypes::ElementTypeRole] = ElisaUtils::Genre;

        result.push_back(newData);
    }

    d->mSelectAllGenresQuery.finish();

    return result;
}

DataTypes::ListArtistDataType DatabaseInterface::internalAllComposersPartialData()
{
    DataTypes::ListArtistDataType result;

    if (!internalGenericPartialData(d->mSelectAllComposersQuery)) {
        return result;
    }

    while(d->mSelectAllComposersQuery.next()) {
        auto newData = DataTypes::ArtistDataType{};

        const auto &currentRecord = d->mSelectAllComposersQuery.record();

        newData[DataTypes::DatabaseIdRole] = currentRecord.value(0);
        newData[DataTypes::TitleRole] = currentRecord.value(1);
        newData[DataTypes::ElementTypeRole] = ElisaUtils::Composer;

        result.push_back(newData);
    }

    d->mSelectAllComposersQuery.finish();

    return result;
}

DataTypes::ListArtistDataType DatabaseInterface::internalAllLyricistsPartialData()
{
    DataTypes::ListArtistDataType result;

    if (!internalGenericPartialData(d->mSelectAllLyricistsQuery)) {
        return result;
    }

    while(d->mSelectAllLyricistsQuery.next()) {
        auto newData = DataTypes::ArtistDataType{};

        const auto &currentRecord = d->mSelectAllLyricistsQuery.record();

        newData[DataTypes::DatabaseIdRole] = currentRecord.value(0);
        newData[DataTypes::TitleRole] = currentRecord.value(1);
        newData[DataTypes::ElementTypeRole] = ElisaUtils::Lyricist;

        result.push_back(newData);
    }

    d->mSelectAllLyricistsQuery.finish();

    return result;
}

bool DatabaseInterface::prepareQuery(QSqlQuery &query, const QString &queryText) const
{
    query.setForwardOnly(true);
    return query.prepare(queryText);
}

bool DatabaseInterface::execQuery(QSqlQuery &query)
{
#if !defined NDEBUG
    auto timer = QElapsedTimer{};
    timer.start();
#endif

    auto result = query.exec();

#if !defined NDEBUG
    if (timer.nsecsElapsed() > 10000000) {
        qCDebug(orgKdeElisaDatabase) << "[[" << timer.nsecsElapsed() << "]]" << query.lastQuery();
    }
#endif

    return result;
}

void DatabaseInterface::updateAlbumArtist(qulonglong albumId, const QString &title,
                                          const QString &albumPath,
                                          const QString &artistName)
{
    d->mUpdateAlbumArtistQuery.bindValue(QStringLiteral(":albumId"), albumId);
    insertArtist(artistName);
    d->mUpdateAlbumArtistQuery.bindValue(QStringLiteral(":artistName"), artistName);

    auto queryResult = execQuery(d->mUpdateAlbumArtistQuery);

    if (!queryResult || !d->mUpdateAlbumArtistQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumArtist" << d->mUpdateAlbumArtistQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumArtist" << d->mUpdateAlbumArtistQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumArtist" << d->mUpdateAlbumArtistQuery.lastError();

        d->mUpdateAlbumArtistQuery.finish();

        return;
    }

    d->mUpdateAlbumArtistQuery.finish();

    d->mUpdateAlbumArtistInTracksQuery.bindValue(QStringLiteral(":albumTitle"), title);
    d->mUpdateAlbumArtistInTracksQuery.bindValue(QStringLiteral(":albumPath"), albumPath);
    d->mUpdateAlbumArtistInTracksQuery.bindValue(QStringLiteral(":artistName"), artistName);

    queryResult = execQuery(d->mUpdateAlbumArtistInTracksQuery);

    if (!queryResult || !d->mUpdateAlbumArtistInTracksQuery.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumArtist" << d->mUpdateAlbumArtistInTracksQuery.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumArtist" << d->mUpdateAlbumArtistInTracksQuery.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumArtist" << d->mUpdateAlbumArtistInTracksQuery.lastError();

        d->mUpdateAlbumArtistInTracksQuery.finish();

        return;
    }

    d->mUpdateAlbumArtistInTracksQuery.finish();
}

bool DatabaseInterface::updateAlbumCover(qulonglong albumId, const QUrl &albumArtUri)
{
    bool modifiedAlbum = false;

    auto storedAlbumArtUri = internalAlbumArtUriFromAlbumId(albumId);

    if (albumArtUri.isValid() && (!storedAlbumArtUri.isValid() || storedAlbumArtUri != albumArtUri)) {
        d->mUpdateAlbumArtUriFromAlbumIdQuery.bindValue(QStringLiteral(":albumId"), albumId);
        d->mUpdateAlbumArtUriFromAlbumIdQuery.bindValue(QStringLiteral(":coverFileName"), albumArtUri);

        auto result = execQuery(d->mUpdateAlbumArtUriFromAlbumIdQuery);

        if (!result || !d->mUpdateAlbumArtUriFromAlbumIdQuery.isActive()) {
            Q_EMIT databaseError();

            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumCover" << d->mUpdateAlbumArtUriFromAlbumIdQuery.lastQuery();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumCover" << d->mUpdateAlbumArtUriFromAlbumIdQuery.boundValues();
            qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateAlbumCover" << d->mUpdateAlbumArtUriFromAlbumIdQuery.lastError();

            d->mUpdateAlbumArtUriFromAlbumIdQuery.finish();

            return modifiedAlbum;
        }

        d->mUpdateAlbumArtUriFromAlbumIdQuery.finish();

        modifiedAlbum = true;
    }

    return modifiedAlbum;
}

void DatabaseInterface::updateTrackStatistics(const QUrl &fileName, const QDateTime &time)
{
    d->mUpdateTrackStatistics.bindValue(QStringLiteral(":fileName"), fileName);
    d->mUpdateTrackStatistics.bindValue(QStringLiteral(":playDate"), time.toMSecsSinceEpoch());

    auto queryResult = execQuery(d->mUpdateTrackStatistics);

    if (!queryResult || !d->mUpdateTrackStatistics.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackStatistics" << d->mUpdateTrackStatistics.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackStatistics" << d->mUpdateTrackStatistics.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackStatistics" << d->mUpdateTrackStatistics.lastError();

        d->mUpdateTrackStatistics.finish();

        return;
    }

    d->mUpdateTrackStatistics.finish();

    d->mUpdateTrackFirstPlayStatistics.bindValue(QStringLiteral(":fileName"), fileName);
    d->mUpdateTrackFirstPlayStatistics.bindValue(QStringLiteral(":playDate"), time.toMSecsSinceEpoch());

    queryResult = execQuery(d->mUpdateTrackFirstPlayStatistics);

    if (!queryResult || !d->mUpdateTrackFirstPlayStatistics.isActive()) {
        Q_EMIT databaseError();

        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackStatistics" << d->mUpdateTrackFirstPlayStatistics.lastQuery();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackStatistics" << d->mUpdateTrackFirstPlayStatistics.boundValues();
        qCDebug(orgKdeElisaDatabase) << "DatabaseInterface::updateTrackStatistics" << d->mUpdateTrackFirstPlayStatistics.lastError();

        d->mUpdateTrackFirstPlayStatistics.finish();

        return;
    }

    d->mUpdateTrackFirstPlayStatistics.finish();
}


#include "moc_databaseinterface.cpp"
