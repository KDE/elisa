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

#include "databaseinterface.h"

#include <KI18n/KLocalizedString>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QMutex>
#include <QVariant>
#include <QAtomicInt>
#include <QDebug>

#include <algorithm>

class DatabaseInterfacePrivate
{
public:

    DatabaseInterfacePrivate(const QSqlDatabase &tracksDatabase)
        : mTracksDatabase(tracksDatabase), mSelectAlbumQuery(mTracksDatabase),
          mSelectTrackQuery(mTracksDatabase), mSelectAlbumIdFromTitleQuery(mTracksDatabase),
          mInsertAlbumQuery(mTracksDatabase), mSelectTrackIdFromTitleAlbumIdArtistQuery(mTracksDatabase),
          mInsertTrackQuery(mTracksDatabase), mSelectAlbumTrackCountQuery(mTracksDatabase),
          mUpdateAlbumQuery(mTracksDatabase), mSelectTracksFromArtist(mTracksDatabase),
          mSelectTrackFromIdQuery(mTracksDatabase), mSelectCountAlbumsForArtistQuery(mTracksDatabase),
          mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery(mTracksDatabase), mSelectAllAlbumsQuery(mTracksDatabase),
          mSelectAllAlbumsFromArtistQuery(mTracksDatabase), mSelectAllArtistsQuery(mTracksDatabase),
          mInsertArtistsQuery(mTracksDatabase), mSelectArtistByNameQuery(mTracksDatabase),
          mSelectArtistQuery(mTracksDatabase), mSelectTrackFromFilePathQuery(mTracksDatabase),
          mRemoveTrackQuery(mTracksDatabase), mRemoveAlbumQuery(mTracksDatabase),
          mRemoveArtistQuery(mTracksDatabase), mSelectAllTracksQuery(mTracksDatabase),
          mInsertTrackMapping(mTracksDatabase), mSelectAllTracksFromSourceQuery(mTracksDatabase),
          mInsertMusicSource(mTracksDatabase), mSelectMusicSource(mTracksDatabase),
          mUpdateIsSingleDiscAlbumFromIdQuery(mTracksDatabase), mSelectAllInvalidTracksFromSourceQuery(mTracksDatabase),
          mInitialUpdateTracksValidity(mTracksDatabase), mUpdateTrackMapping(mTracksDatabase),
          mSelectTracksMapping(mTracksDatabase), mSelectTracksMappingPriority(mTracksDatabase),
          mUpdateAlbumArtUriFromAlbumIdQuery(mTracksDatabase), mSelectTracksMappingPriorityByTrackId(mTracksDatabase),
          mSelectAllTrackFilesFromSourceQuery(mTracksDatabase), mFindInvalidTrackFilesQuery(mTracksDatabase),
          mSelectAlbumIdsFromArtist(mTracksDatabase), mRemoveTracksMappingFromSource(mTracksDatabase),
          mRemoveTracksMapping(mTracksDatabase), mSelectTracksWithoutMappingQuery(mTracksDatabase),
          mSelectAlbumIdFromTitleAndArtistQuery(mTracksDatabase), mSelectAlbumIdFromTitleWithoutArtistQuery(mTracksDatabase),
          mInsertAlbumArtistQuery(mTracksDatabase), mInsertTrackArtistQuery(mTracksDatabase),
          mRemoveTrackArtistQuery(mTracksDatabase), mRemoveAlbumArtistQuery(mTracksDatabase),
          mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery(mTracksDatabase)
    {
    }

    QSqlDatabase mTracksDatabase;

    QSqlQuery mSelectAlbumQuery;

    QSqlQuery mSelectTrackQuery;

    QSqlQuery mSelectAlbumIdFromTitleQuery;

    QSqlQuery mInsertAlbumQuery;

    QSqlQuery mSelectTrackIdFromTitleAlbumIdArtistQuery;

    QSqlQuery mInsertTrackQuery;

    QSqlQuery mSelectAlbumTrackCountQuery;

    QSqlQuery mUpdateAlbumQuery;

    QSqlQuery mSelectTracksFromArtist;

    QSqlQuery mSelectTrackFromIdQuery;

    QSqlQuery mSelectCountAlbumsForArtistQuery;

    QSqlQuery mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery;

    QSqlQuery mSelectAllAlbumsQuery;

    QSqlQuery mSelectAllAlbumsFromArtistQuery;

    QSqlQuery mSelectAllArtistsQuery;

    QSqlQuery mInsertArtistsQuery;

    QSqlQuery mSelectArtistByNameQuery;

    QSqlQuery mSelectArtistQuery;

    QSqlQuery mSelectTrackFromFilePathQuery;

    QSqlQuery mRemoveTrackQuery;

    QSqlQuery mRemoveAlbumQuery;

    QSqlQuery mRemoveArtistQuery;

    QSqlQuery mSelectAllTracksQuery;

    QSqlQuery mInsertTrackMapping;

    QSqlQuery mSelectAllTracksFromSourceQuery;

    QSqlQuery mInsertMusicSource;

    QSqlQuery mSelectMusicSource;

    QSqlQuery mUpdateIsSingleDiscAlbumFromIdQuery;

    QSqlQuery mSelectAllInvalidTracksFromSourceQuery;

    QSqlQuery mInitialUpdateTracksValidity;

    QSqlQuery mUpdateTrackMapping;

    QSqlQuery mSelectTracksMapping;

    QSqlQuery mSelectTracksMappingPriority;

    QSqlQuery mUpdateAlbumArtUriFromAlbumIdQuery;

    QSqlQuery mSelectTracksMappingPriorityByTrackId;

    QSqlQuery mSelectAllTrackFilesFromSourceQuery;

    QSqlQuery mFindInvalidTrackFilesQuery;

    QSqlQuery mSelectAlbumIdsFromArtist;

    QSqlQuery mRemoveTracksMappingFromSource;

    QSqlQuery mRemoveTracksMapping;

    QSqlQuery mSelectTracksWithoutMappingQuery;

    QSqlQuery mSelectAlbumIdFromTitleAndArtistQuery;

    QSqlQuery mSelectAlbumIdFromTitleWithoutArtistQuery;

    QSqlQuery mInsertAlbumArtistQuery;

    QSqlQuery mInsertTrackArtistQuery;

    QSqlQuery mRemoveTrackArtistQuery;

    QSqlQuery mRemoveAlbumArtistQuery;

    QSqlQuery mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery;

    qulonglong mAlbumId = 1;

    qulonglong mArtistId = 1;

    qulonglong mTrackId = 1;

    qulonglong mDiscoverId = 1;

    bool mInitFinished = false;

    QAtomicInt mStopRequest = 0;

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
        qDebug() << "database open";
    } else {
        qDebug() << "database not open";
    }
    qDebug() << "DatabaseInterface::init" << (tracksDatabase.driver()->hasFeature(QSqlDriver::Transactions) ? "yes" : "no");

    d = std::make_unique<DatabaseInterfacePrivate>(tracksDatabase);

    initDatabase();
    initRequest();

    if (!databaseFileName.isEmpty()) {
        reloadExistingDatabase();
    }
}

MusicAlbum DatabaseInterface::albumFromTitleAndArtist(const QString &title, const QString &artist)
{
    auto result = MusicAlbum();

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalAlbumFromTitleAndArtist(title, artist);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

QList<MusicAudioTrack> DatabaseInterface::allTracks()
{
    auto result = QList<MusicAudioTrack>();

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    auto queryResult = d->mSelectAllTracksQuery.exec();

    if (!queryResult || !d->mSelectAllTracksQuery.isSelect() || !d->mSelectAllTracksQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllTracksQuery.lastQuery();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllTracksQuery.boundValues();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllTracksQuery.lastError();

        return result;
    }

    while(d->mSelectAllTracksQuery.next()) {
        const auto &currentRecord = d->mSelectAllTracksQuery.record();

        result.push_back(buildTrackFromDatabaseRecord(currentRecord));
    }

    d->mSelectAllTracksQuery.finish();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

QList<MusicAudioTrack> DatabaseInterface::allTracksFromSource(const QString &musicSource)
{
    auto result = QList<MusicAudioTrack>();

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    d->mSelectAllTracksFromSourceQuery.bindValue(QStringLiteral(":source"), musicSource);

    auto queryResult = d->mSelectAllTracksFromSourceQuery.exec();

    if (!queryResult || !d->mSelectAllTracksFromSourceQuery.isSelect() || !d->mSelectAllTracksFromSourceQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllTracksFromSourceQuery.lastQuery();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllTracksFromSourceQuery.boundValues();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllTracksFromSourceQuery.lastError();

        d->mSelectAllTracksFromSourceQuery.finish();

        transactionResult = finishTransaction();
        if (!transactionResult) {
            return result;
        }

        return result;
    }

    while(d->mSelectAllTracksFromSourceQuery.next()) {
        const auto &currentRecord = d->mSelectAllTracksFromSourceQuery.record();

        result.push_back(buildTrackFromDatabaseRecord(currentRecord));
    }

    d->mSelectAllTracksFromSourceQuery.finish();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

QList<MusicAudioTrack> DatabaseInterface::allInvalidTracksFromSource(const QString &musicSource)
{
    auto result = QList<MusicAudioTrack>();

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    d->mSelectAllInvalidTracksFromSourceQuery.bindValue(QStringLiteral(":source"), musicSource);

    auto queryResult = d->mSelectAllInvalidTracksFromSourceQuery.exec();

    if (!queryResult || !d->mSelectAllInvalidTracksFromSourceQuery.isSelect() || !d->mSelectAllInvalidTracksFromSourceQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllInvalidTracksFromSourceQuery.lastQuery();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllInvalidTracksFromSourceQuery.boundValues();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllInvalidTracksFromSourceQuery.lastError();

        return result;
    }

    while(d->mSelectAllInvalidTracksFromSourceQuery.next()) {
        const auto &currentRecord = d->mSelectAllInvalidTracksFromSourceQuery.record();

        result.push_back(buildTrackFromDatabaseRecord(currentRecord));
    }

    d->mSelectAllInvalidTracksFromSourceQuery.finish();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

QList<MusicAlbum> DatabaseInterface::allAlbums()
{
    auto result = QList<MusicAlbum>();

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    auto queryResult = d->mSelectAllAlbumsQuery.exec();

    if (!queryResult || !d->mSelectAllAlbumsQuery.isSelect() || !d->mSelectAllAlbumsQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllAlbumsQuery.lastQuery();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllAlbumsQuery.boundValues();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllAlbumsQuery.lastError();

        return result;
    }

    while(d->mSelectAllAlbumsQuery.next()) {
        auto newAlbum = MusicAlbum();

        const auto &currentRecord = d->mSelectAllAlbumsQuery.record();

        newAlbum.setDatabaseId(currentRecord.value(0).toULongLong());
        newAlbum.setTitle(currentRecord.value(1).toString());
        newAlbum.setId(currentRecord.value(2).toString());
        newAlbum.setArtist(currentRecord.value(3).toString());
        newAlbum.setAlbumArtURI(currentRecord.value(4).toUrl());
        newAlbum.setTracksCount(currentRecord.value(5).toInt());
        newAlbum.setIsSingleDiscAlbum(currentRecord.value(6).toBool());
        newAlbum.setTracks(fetchTracks(newAlbum.databaseId()));
        newAlbum.setValid(true);

        result.push_back(newAlbum);
    }

    d->mSelectAllAlbumsQuery.finish();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

QList<MusicArtist> DatabaseInterface::allArtists()
{
    auto result = QList<MusicArtist>();

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    auto queryResult = d->mSelectAllArtistsQuery.exec();

    if (!queryResult || !d->mSelectAllArtistsQuery.isSelect() || !d->mSelectAllArtistsQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::allArtists" << d->mSelectAllArtistsQuery.lastQuery();
        qDebug() << "DatabaseInterface::allArtists" << d->mSelectAllArtistsQuery.boundValues();
        qDebug() << "DatabaseInterface::allArtists" << d->mSelectAllArtistsQuery.lastError();

        d->mSelectAllArtistsQuery.finish();

        transactionResult = finishTransaction();
        if (!transactionResult) {
            return result;
        }

        return result;
    }

    while(d->mSelectAllArtistsQuery.next()) {
        auto newArtist = MusicArtist();

        const auto &currentRecord = d->mSelectAllArtistsQuery.record();

        newArtist.setDatabaseId(currentRecord.value(0).toULongLong());
        newArtist.setName(currentRecord.value(1).toString());
        newArtist.setValid(true);

        d->mSelectCountAlbumsForArtistQuery.bindValue(QStringLiteral(":artistName"), newArtist.name());

        auto queryResult = d->mSelectCountAlbumsForArtistQuery.exec();

        if (!queryResult || !d->mSelectCountAlbumsForArtistQuery.isSelect() || !d->mSelectCountAlbumsForArtistQuery.isActive() || !d->mSelectCountAlbumsForArtistQuery.next()) {
            Q_EMIT databaseError();

            qDebug() << "DatabaseInterface::allArtists" << d->mSelectCountAlbumsForArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::allArtists" << d->mSelectCountAlbumsForArtistQuery.boundValues();
            qDebug() << "DatabaseInterface::allArtists" << d->mSelectCountAlbumsForArtistQuery.lastError();

            d->mSelectCountAlbumsForArtistQuery.finish();

            transactionResult = finishTransaction();
            if (!transactionResult) {
                return result;
            }

            return result;
        }

        newArtist.setAlbumsCount(d->mSelectCountAlbumsForArtistQuery.record().value(0).toInt());

        d->mSelectCountAlbumsForArtistQuery.finish();

        result.push_back(newArtist);
    }

    d->mSelectAllArtistsQuery.finish();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

QList<MusicAudioTrack> DatabaseInterface::tracksFromAuthor(const QString &artistName)
{
    auto allTracks = QList<MusicAudioTrack>();

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return allTracks;
    }

    allTracks = internalTracksFromAuthor(artistName);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return allTracks;
    }

    return allTracks;
}

MusicArtist DatabaseInterface::internalArtistFromId(qulonglong artistId)
{
    auto result = MusicArtist();

    if (!d || !d->mTracksDatabase.isValid() || !d->mInitFinished) {
        return result;
    }

    d->mSelectArtistQuery.bindValue(QStringLiteral(":artistId"), artistId);

    auto queryResult = d->mSelectArtistQuery.exec();

    if (!queryResult || !d->mSelectArtistQuery.isSelect() || !d->mSelectArtistQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::internalArtistFromId" << d->mSelectArtistQuery.lastQuery();
        qDebug() << "DatabaseInterface::internalArtistFromId" << d->mSelectArtistQuery.boundValues();
        qDebug() << "DatabaseInterface::internalArtistFromId" << d->mSelectArtistQuery.lastError();

        d->mSelectArtistQuery.finish();

        return result;
    }

    if (!d->mSelectArtistQuery.next()) {
        d->mSelectArtistQuery.finish();

        return result;
    }

    const auto &currentRecord = d->mSelectArtistQuery.record();

    result.setDatabaseId(currentRecord.value(0).toULongLong());
    result.setName(currentRecord.value(1).toString());
    result.setValid(true);

    d->mSelectArtistQuery.finish();

    d->mSelectCountAlbumsForArtistQuery.bindValue(QStringLiteral(":artistName"), result.name());

    queryResult = d->mSelectCountAlbumsForArtistQuery.exec();

    if (!queryResult || !d->mSelectCountAlbumsForArtistQuery.isSelect() || !d->mSelectCountAlbumsForArtistQuery.isActive() || !d->mSelectCountAlbumsForArtistQuery.next()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::internalArtistFromId" << d->mSelectCountAlbumsForArtistQuery.lastQuery();
        qDebug() << "DatabaseInterface::internalArtistFromId" << d->mSelectCountAlbumsForArtistQuery.boundValues();
        qDebug() << "DatabaseInterface::internalArtistFromId" << d->mSelectCountAlbumsForArtistQuery.lastError();

        d->mSelectCountAlbumsForArtistQuery.finish();

        return result;
    }

    result.setAlbumsCount(d->mSelectCountAlbumsForArtistQuery.record().value(0).toInt());

    d->mSelectCountAlbumsForArtistQuery.finish();

    return result;
}

MusicAudioTrack DatabaseInterface::trackFromDatabaseId(qulonglong id)
{
    auto result = MusicAudioTrack();

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalTrackFromDatabaseId(id);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

qulonglong DatabaseInterface::trackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &artist, const QString &album,
                                                                   int trackNumber, int discNumber)
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

void DatabaseInterface::applicationAboutToQuit()
{
    d->mStopRequest = 1;
}

void DatabaseInterface::removeAllTracksFromSource(const QString &sourceName)
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    d->mSelectMusicSource.bindValue(QStringLiteral(":name"), sourceName);

    auto queryResult = d->mSelectMusicSource.exec();

    if (!queryResult || !d->mSelectMusicSource.isSelect() || !d->mSelectMusicSource.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::insertMusicSource" << d->mSelectMusicSource.lastQuery();
        qDebug() << "DatabaseInterface::insertMusicSource" << d->mSelectMusicSource.boundValues();
        qDebug() << "DatabaseInterface::insertMusicSource" << d->mSelectMusicSource.lastError();

        d->mSelectMusicSource.finish();

        transactionResult = finishTransaction();
        if (!transactionResult) {
            return;
        }

        return;
    }

    if (!d->mSelectMusicSource.next()) {
        transactionResult = finishTransaction();
        if (!transactionResult) {
            return;
        }

        return;
    }

    qulonglong sourceId = d->mSelectMusicSource.record().value(0).toULongLong();

    d->mSelectMusicSource.finish();

    d->mSelectAllTrackFilesFromSourceQuery.bindValue(QStringLiteral(":discoverId"), sourceId);

    queryResult = d->mSelectAllTrackFilesFromSourceQuery.exec();

    if (!queryResult || !d->mSelectAllTrackFilesFromSourceQuery.isSelect() || !d->mSelectAllTrackFilesFromSourceQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::insertMusicSource" << d->mSelectAllTrackFilesFromSourceQuery.lastQuery();
        qDebug() << "DatabaseInterface::insertMusicSource" << d->mSelectAllTrackFilesFromSourceQuery.boundValues();
        qDebug() << "DatabaseInterface::insertMusicSource" << d->mSelectAllTrackFilesFromSourceQuery.lastError();

        d->mSelectAllTrackFilesFromSourceQuery.finish();

        transactionResult = finishTransaction();
        if (!transactionResult) {
            return;
        }

        return;
    }

    QList<QUrl> allFileNames;

    while(d->mSelectAllTrackFilesFromSourceQuery.next()) {
        auto fileName = d->mSelectAllTrackFilesFromSourceQuery.record().value(0).toUrl();

        allFileNames.push_back(fileName);
    }

    d->mSelectAllTrackFilesFromSourceQuery.finish();

    internalRemoveTracksList(allFileNames, sourceId);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }
}

void DatabaseInterface::cleanInvalidTracks()
{
    if (d->mStopRequest == 1) {
        return;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    auto queryResult = d->mFindInvalidTrackFilesQuery.exec();

    if (!queryResult || !d->mFindInvalidTrackFilesQuery.isSelect() || !d->mFindInvalidTrackFilesQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::insertMusicSource" << d->mFindInvalidTrackFilesQuery.lastQuery();
        qDebug() << "DatabaseInterface::insertMusicSource" << d->mFindInvalidTrackFilesQuery.boundValues();
        qDebug() << "DatabaseInterface::insertMusicSource" << d->mFindInvalidTrackFilesQuery.lastError();

        d->mFindInvalidTrackFilesQuery.finish();

        transactionResult = finishTransaction();
        if (!transactionResult) {
            return;
        }

        return;
    }

    QList<QUrl> allFileNames;
    auto sourceId = qulonglong();

    while(d->mFindInvalidTrackFilesQuery.next()) {
        auto fileName = d->mFindInvalidTrackFilesQuery.record().value(0).toUrl();
        sourceId = d->mFindInvalidTrackFilesQuery.record().value(1).toULongLong();
        allFileNames.push_back(fileName);
    }

    d->mFindInvalidTrackFilesQuery.finish();

    internalRemoveTracksList(allFileNames, sourceId);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }
}

void DatabaseInterface::insertTracksList(const QList<MusicAudioTrack> &tracks, const QHash<QString, QUrl> &covers, const QString &musicSource)
{
    if (d->mStopRequest == 1) {
        return;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    QSet<qulonglong> modifiedAlbumIds;
    QList<qulonglong> insertedTracks;

    for(const auto &oneTrack : tracks) {
        d->mSelectTracksMapping.bindValue(QStringLiteral(":fileName"), oneTrack.resourceURI());

        auto result = d->mSelectTracksMapping.exec();

        if (!result || !d->mSelectTracksMapping.isSelect() || !d->mSelectTracksMapping.isActive()) {
            Q_EMIT databaseError();

            qDebug() << "DatabaseInterface::insertTracksList" << d->mSelectTracksMapping.lastQuery();
            qDebug() << "DatabaseInterface::insertTracksList" << d->mSelectTracksMapping.boundValues();
            qDebug() << "DatabaseInterface::insertTracksList" << d->mSelectTracksMapping.lastError();

            d->mSelectTracksMapping.finish();

            rollBackTransaction();
            return;
        }

        bool isNewTrack = !d->mSelectTracksMapping.next();

        if (isNewTrack) {
            insertTrackOrigin(oneTrack.resourceURI(), insertMusicSource(musicSource));
        } else {
            updateTrackOrigin(d->mSelectTracksMapping.record().value(0).toULongLong(), oneTrack.resourceURI());
        }

        d->mSelectTracksMapping.finish();

        const auto insertedTrackId = internalInsertTrack(oneTrack, covers, 0, modifiedAlbumIds,
                                                         (isNewTrack ? TrackFileInsertType::NewTrackFileInsert : TrackFileInsertType::ModifiedTrackFileInsert));

        if (isNewTrack && insertedTrackId != 0) {
            insertedTracks.push_back(insertedTrackId);
        }

        if (d->mStopRequest == 1) {
            transactionResult = finishTransaction();
            if (!transactionResult) {
                return;
            }
            return;
        }
    }

    const auto &constModifiedAlbumIds = modifiedAlbumIds;
    for (auto albumId : constModifiedAlbumIds) {
        Q_EMIT albumModified(internalAlbumFromId(albumId), albumId);
    }

    QList<MusicAudioTrack> newTracks;
    for (auto trackId : qAsConst(insertedTracks)) {
        newTracks.push_back(internalTrackFromDatabaseId(trackId));
    }

    if (!newTracks.isEmpty()) {
        Q_EMIT tracksAdded(newTracks);
    }

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }
}

void DatabaseInterface::removeTracksList(const QList<QUrl> &removedTracks)
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    internalRemoveTracksList(removedTracks);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }
}

void DatabaseInterface::modifyTracksList(const QList<MusicAudioTrack> &modifiedTracks, const QHash<QString, QUrl> &covers,
                                         const QString &musicSource)
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    QSet<qulonglong> modifiedAlbumIds;

    for (const auto &oneModifiedTrack : modifiedTracks) {
        if (oneModifiedTrack.albumArtist().isEmpty()) {
            continue;
        }

        bool modifyExistingTrack = internalTrackFromDatabaseId(oneModifiedTrack.databaseId()).isValid() ||
                (internalTrackIdFromFileName(oneModifiedTrack.resourceURI()) != 0);

        auto originTrackId = oneModifiedTrack.databaseId();
        if (!originTrackId) {
            originTrackId = internalTrackIdFromFileName(oneModifiedTrack.resourceURI());
        }

        if (!modifyExistingTrack) {
            insertTrackOrigin(oneModifiedTrack.resourceURI(), insertMusicSource(musicSource));
        } else {
            updateTrackOrigin(originTrackId, oneModifiedTrack.resourceURI());
        }

        internalInsertTrack(oneModifiedTrack, covers, (modifyExistingTrack ? originTrackId : 0),
                            modifiedAlbumIds,
                            (modifyExistingTrack ? TrackFileInsertType::ModifiedTrackFileInsert : TrackFileInsertType::NewTrackFileInsert));
    }

    const auto &constModifiedAlbumIds = modifiedAlbumIds;
    for (auto albumId : constModifiedAlbumIds) {
        Q_EMIT albumModified(internalAlbumFromId(albumId), albumId);
    }

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }
}

bool DatabaseInterface::startTransaction() const
{
    auto result = false;

    auto transactionResult = d->mTracksDatabase.transaction();
    if (!transactionResult) {
        qDebug() << "transaction failed" << d->mTracksDatabase.lastError() << d->mTracksDatabase.lastError().driverText();

        return result;
    }

    result = true;

    return result;
}

bool DatabaseInterface::finishTransaction() const
{
    auto result = false;

    auto transactionResult = d->mTracksDatabase.commit();

    if (!transactionResult) {
        qDebug() << "commit failed" << d->mTracksDatabase.lastError() << d->mTracksDatabase.lastError().nativeErrorCode();

        return result;
    }

    result = true;

    return result;
}

bool DatabaseInterface::rollBackTransaction() const
{
    auto result = false;

    auto transactionResult = d->mTracksDatabase.rollback();

    if (!transactionResult) {
        qDebug() << "commit failed" << d->mTracksDatabase.lastError() << d->mTracksDatabase.lastError().nativeErrorCode();

        return result;
    }

    result = true;

    return result;
}

void DatabaseInterface::initDatabase() const
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    auto listTables = d->mTracksDatabase.tables();

    if (!listTables.contains(QStringLiteral("DatabaseVersionV1"))) {
        for (const auto &oneTable : listTables) {
            QSqlQuery createSchemaQuery(d->mTracksDatabase);

            auto result = createSchemaQuery.exec(QStringLiteral("DROP TABLE ") + oneTable);

            if (!result) {
                qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastQuery();
                qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();
            }
        }

        listTables = d->mTracksDatabase.tables();
    }

    if (!listTables.contains(QStringLiteral("DatabaseVersionV1"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DatabaseVersionV1` (`Version` INTEGER PRIMARY KEY NOT NULL)"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();
        }
    }

    if (!listTables.contains(QStringLiteral("DiscoverSource"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DiscoverSource` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(55) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();
        }
    }

    if (!listTables.contains(QStringLiteral("Artists"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Artists` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(55) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();
        }
    }

    if (!listTables.contains(QStringLiteral("Albums"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Albums` ("
                                                                   "`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Title` VARCHAR(55) NOT NULL, "
                                                                   "`CoverFileName` VARCHAR(255) NOT NULL, "
                                                                   "`TracksCount` INTEGER NOT NULL, "
                                                                   "`IsSingleDiscAlbum` BOOLEAN NOT NULL, "
                                                                   "`AlbumInternalID` VARCHAR(55))"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();
        }
    }

    if (!listTables.contains(QStringLiteral("AlbumsArtists"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `AlbumsArtists` ("
                                                                   "`AlbumID` INTEGER NOT NULL, "
                                                                   "`ArtistID` INTEGER NOT NULL, "
                                                                   "CONSTRAINT pk_albumsartists PRIMARY KEY (`AlbumID`, `ArtistID`), "
                                                                   "CONSTRAINT fk_albums FOREIGN KEY (`AlbumID`) REFERENCES `Albums`(`ID`) "
                                                                   "ON DELETE CASCADE, "
                                                                   "CONSTRAINT fk_artists FOREIGN KEY (`ArtistID`) REFERENCES `Artists`(`ID`) "
                                                                   "ON DELETE CASCADE)"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();
        }
    }

    if (!listTables.contains(QStringLiteral("Tracks"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Tracks` ("
                                                                   "`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Title` VARCHAR(85) NOT NULL, "
                                                                   "`AlbumID` INTEGER NOT NULL, "
                                                                   "`TrackNumber` INTEGER NOT NULL, "
                                                                   "`DiscNumber` INTEGER DEFAULT -1, "
                                                                   "`Duration` INTEGER NOT NULL, "
                                                                   "`Rating` INTEGER NOT NULL DEFAULT 0, "
                                                                   "UNIQUE (`Title`, `AlbumID`, `TrackNumber`, `DiscNumber`), "
                                                                   "CONSTRAINT fk_tracks_album FOREIGN KEY (`AlbumID`) REFERENCES `Albums`(`ID`))"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();
        }
    }

    if (!listTables.contains(QStringLiteral("TracksArtists"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `TracksArtists` ("
                                                                   "`TrackID` INTEGER NOT NULL, "
                                                                   "`ArtistID` INTEGER NOT NULL, "
                                                                   "CONSTRAINT pk_tracksartists PRIMARY KEY (`TrackID`, `ArtistID`), "
                                                                   "CONSTRAINT fk_tracks FOREIGN KEY (`TrackID`) REFERENCES `Tracks`(`ID`) "
                                                                   "ON DELETE CASCADE, "
                                                                   "CONSTRAINT fk_artists FOREIGN KEY (`ArtistID`) REFERENCES `Artists`(`ID`) "
                                                                   "ON DELETE CASCADE)"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();
        }
    }

    if (!listTables.contains(QStringLiteral("TracksMapping"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `TracksMapping` ("
                                                                   "`TrackID` INTEGER NULL, "
                                                                   "`DiscoverID` INTEGER NOT NULL, "
                                                                   "`FileName` VARCHAR(255) NOT NULL, "
                                                                   "`Priority` INTEGER NOT NULL, "
                                                                   "`TrackValid` BOOLEAN NOT NULL, "
                                                                   "PRIMARY KEY (`FileName`), "
                                                                   "CONSTRAINT TracksUnique UNIQUE (`TrackID`, `Priority`), "
                                                                   "CONSTRAINT fk_tracksmapping_trackID FOREIGN KEY (`TrackID`) REFERENCES `Tracks`(`ID`), "
                                                                   "CONSTRAINT fk_tracksmapping_discoverID FOREIGN KEY (`DiscoverID`) REFERENCES `DiscoverSource`(`ID`))"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksAlbumIndex` ON `Tracks` "
                                                                  "(`AlbumID`)"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createTrackIndex.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createTrackIndex.lastError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`AlbumsArtistsArtistIndex` ON `AlbumsArtists` "
                                                                  "(`ArtistID`)"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createTrackIndex.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createTrackIndex.lastError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`AlbumsArtistsAlbumIndex` ON `AlbumsArtists` "
                                                                  "(`AlbumID`)"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createTrackIndex.lastError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksArtistsArtistIndex` ON `TracksArtists` "
                                                                  "(`ArtistID`)"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createTrackIndex.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createTrackIndex.lastError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksArtistsTrackIndex` ON `TracksArtists` "
                                                                  "(`TrackID`)"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createTrackIndex.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createTrackIndex.lastError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksFileNameIndex` ON `TracksMapping` "
                                                                  "(`FileName`)"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createTrackIndex.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createTrackIndex.lastError();
        }
    }

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }
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
                                                   "album.`AlbumInternalID`, "
                                                   "artist.`Name`, "
                                                   "album.`CoverFileName`, "
                                                   "album.`TracksCount`, "
                                                   "album.`IsSingleDiscAlbum` "
                                                   "FROM `Albums` album "
                                                   "LEFT JOIN `AlbumsArtists` albumArtist "
                                                   "ON "
                                                   "albumArtist.`AlbumID` = album.`ID` "
                                                   "LEFT JOIN `Artists` artist "
                                                   "ON "
                                                   "albumArtist.`ArtistID` = artist.`ID` "
                                                   "WHERE "
                                                   "album.`ID` = :albumId");

        auto result = d->mSelectAlbumQuery.prepare(selectAlbumQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumQuery.lastError();
        }
    }

    {
        auto selectAllAlbumsText = QStringLiteral("SELECT "
                                                  "album.`ID`, "
                                                  "album.`Title`, "
                                                  "album.`AlbumInternalID`, "
                                                  "artist.`Name`, "
                                                  "album.`CoverFileName`, "
                                                  "album.`TracksCount`, "
                                                  "album.`IsSingleDiscAlbum` "
                                                  "FROM `Albums` album "
                                                  "LEFT JOIN `AlbumsArtists` albumArtist "
                                                  "ON "
                                                  "albumArtist.`AlbumID` = album.`ID` "
                                                  "LEFT JOIN `Artists` artist "
                                                  "ON "
                                                  "albumArtist.`ArtistID` = artist.`ID` "
                                                  "ORDER BY album.`Title`");

        auto result = d->mSelectAllAlbumsQuery.prepare(selectAllAlbumsText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAllAlbumsQuery.lastError();
        }
    }

    {
        auto selectAllArtistsWithFilterText = QStringLiteral("SELECT `ID`, "
                                                             "`Name` "
                                                             "FROM `Artists`");

        auto result = d->mSelectAllArtistsQuery.prepare(selectAllArtistsWithFilterText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAllArtistsQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAllArtistsQuery.lastError();
        }
    }

    {
        auto selectAllTracksText = QStringLiteral("SELECT "
                                                  "tracks.`ID`, "
                                                  "tracks.`Title`, "
                                                  "tracks.`AlbumID`, "
                                                  "artist.`Name`, "
                                                  "artistAlbum.`Name`, "
                                                  "tracksMapping.`FileName`, "
                                                  "tracks.`TrackNumber`, "
                                                  "tracks.`DiscNumber`, "
                                                  "tracks.`Duration`, "
                                                  "album.`Title`, "
                                                  "tracks.`Rating`, "
                                                  "album.`CoverFileName`, "
                                                  "album.`IsSingleDiscAlbum` "
                                                  "FROM "
                                                  "`Tracks` tracks, `Artists` artist, `TracksArtists` trackArtist, "
                                                  "`Albums` album, `TracksMapping` tracksMapping "
                                                  "LEFT JOIN `AlbumsArtists` artistAlbumMapping ON artistAlbumMapping.`AlbumID` = album.`ID` "
                                                  "LEFT JOIN `Artists` artistAlbum ON artistAlbum.`ID` = artistAlbumMapping.`ArtistID` "
                                                  "WHERE "
                                                  "tracks.`ID` = trackArtist.`TrackID` AND "
                                                  "artist.`ID` = trackArtist.`ArtistID` AND "
                                                  "tracks.`AlbumID` = album.`ID` AND "
                                                  "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                  "tracksMapping.`Priority` = (SELECT MIN(`Priority`) FROM `TracksMapping` WHERE `TrackID` = tracks.`ID`)");

        auto result = d->mSelectAllTracksQuery.prepare(selectAllTracksText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAllTracksQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAllTracksQuery.lastError();
        }
    }

    {
        auto selectAllInvalidTracksFromSourceQueryText = QStringLiteral("SELECT "
                                                                        "tracks.`ID`, "
                                                                        "tracks.`Title`, "
                                                                        "tracks.`AlbumID`, "
                                                                        "artist.`Name`, "
                                                                        "artistAlbum.`Name`, "
                                                                        "tracksMapping.`FileName`, "
                                                                        "tracks.`TrackNumber`, "
                                                                        "tracks.`DiscNumber`, "
                                                                        "tracks.`Duration`, "
                                                                        "album.`Title`, "
                                                                        "tracks.`Rating`, "
                                                                        "album.`CoverFileName`, "
                                                                        "album.`IsSingleDiscAlbum` "
                                                                        "FROM "
                                                                        "`Tracks` tracks, `Artists` artist, `TracksArtists` trackArtist, "
                                                                        "`Albums` album, `TracksMapping` tracksMapping, `DiscoverSource` source "
                                                                        "LEFT JOIN `AlbumsArtists` artistAlbumMapping ON artistAlbumMapping.`AlbumID` = album.`ID` "
                                                                        "LEFT JOIN `Artists` artistAlbum ON artistAlbum.`ID` = artistAlbumMapping.`ArtistID` "
                                                                        "WHERE "
                                                                        "tracks.`ID` = trackArtist.`TrackID` AND "
                                                                        "artist.`ID` = trackArtist.`ArtistID` AND "
                                                                        "tracks.`AlbumID` = album.`ID` AND "
                                                                        "source.`Name` = :source AND "
                                                                        "source.`ID` = tracksMapping.`DiscoverID` AND "
                                                                        "tracksMapping.`TrackValid` = 0 AND "
                                                                        "tracksMapping.`TrackID` = tracks.`ID`");

        auto result = d->mSelectAllInvalidTracksFromSourceQuery.prepare(selectAllInvalidTracksFromSourceQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAllInvalidTracksFromSourceQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAllInvalidTracksFromSourceQuery.lastError();
        }
    }

    {
        auto selectAllTracksFromSourceQueryText = QStringLiteral("SELECT "
                                                                 "tracks.`ID`, "
                                                                 "tracks.`Title`, "
                                                                 "tracks.`AlbumID`, "
                                                                 "artist.`Name`, "
                                                                 "artistAlbum.`Name`, "
                                                                 "tracksMapping.`FileName`, "
                                                                 "tracks.`TrackNumber`, "
                                                                 "tracks.`DiscNumber`, "
                                                                 "tracks.`Duration`, "
                                                                 "album.`Title`, "
                                                                 "tracks.`Rating`, "
                                                                 "album.`CoverFileName`, "
                                                                 "album.`IsSingleDiscAlbum` "
                                                                 "FROM "
                                                                 "`Tracks` tracks, `Artists` artist, `TracksArtists` trackArtist, "
                                                                 "`Albums` album, `TracksMapping` tracksMapping, `DiscoverSource` source "
                                                                 "LEFT JOIN `AlbumsArtists` artistAlbumMapping ON artistAlbumMapping.`AlbumID` = album.`ID` "
                                                                 "LEFT JOIN `Artists` artistAlbum ON artistAlbum.`ID` = artistAlbumMapping.`ArtistID` "
                                                                 "WHERE "
                                                                 "tracks.`ID` = trackArtist.`TrackID` AND "
                                                                 "artist.`ID` = trackArtist.`ArtistID` AND "
                                                                 "tracks.`AlbumID` = album.`ID` AND "
                                                                 "source.`Name` = :source AND "
                                                                 "source.`ID` = tracksMapping.`DiscoverID` AND "
                                                                 "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                                 "tracksMapping.`Priority` = (SELECT MIN(`Priority`) FROM `TracksMapping` WHERE `TrackID` = tracks.`ID`)");

        auto result = d->mSelectAllTracksFromSourceQuery.prepare(selectAllTracksFromSourceQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAllTracksFromSourceQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAllTracksFromSourceQuery.lastError();
        }
    }

    {
        auto selectArtistByNameText = QStringLiteral("SELECT `ID`, "
                                                     "`Name` "
                                                     "FROM `Artists` "
                                                     "WHERE "
                                                     "`Name` = :name");

        auto result = d->mSelectArtistByNameQuery.prepare(selectArtistByNameText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectArtistByNameQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectArtistByNameQuery.lastError();
        }
    }

    {
        auto insertArtistsText = QStringLiteral("INSERT INTO `Artists` (`ID`, `Name`) "
                                                "VALUES (:artistId, :name)");

        auto result = d->mInsertArtistsQuery.prepare(insertArtistsText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertArtistsQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertArtistsQuery.lastError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.`ID`, "
                                                   "tracks.`Title`, "
                                                   "tracks.`AlbumID`, "
                                                   "artist.`Name`, "
                                                   "artistAlbum.`Name`, "
                                                   "tracksMapping.`FileName`, "
                                                   "tracks.`TrackNumber`, "
                                                   "tracks.`DiscNumber`, "
                                                   "tracks.`Duration`, "
                                                   "album.`Title`, "
                                                   "tracks.`Rating`, "
                                                   "album.`CoverFileName`, "
                                                   "album.`IsSingleDiscAlbum` "
                                                   "FROM "
                                                   "`Tracks` tracks, `Artists` artist, `TracksArtists` trackArtist, "
                                                   "`Albums` album, `TracksMapping` tracksMapping "
                                                   "LEFT JOIN `AlbumsArtists` artistAlbumMapping ON artistAlbumMapping.`AlbumID` = album.`ID` "
                                                   "LEFT JOIN `Artists` artistAlbum ON artistAlbum.`ID` = artistAlbumMapping.`ArtistID` "
                                                   "WHERE "
                                                   "tracks.`ID` = trackArtist.`TrackID` AND "
                                                   "artist.`ID` = trackArtist.`ArtistID` AND "
                                                   "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                   "tracks.`AlbumID` = :albumId AND "
                                                   "album.`ID` = :albumId AND "
                                                   "tracksMapping.`Priority` = (SELECT MIN(`Priority`) FROM `TracksMapping` WHERE `TrackID` = tracks.`ID`) "
                                                   "ORDER BY tracks.`DiscNumber` ASC, "
                                                   "tracks.`TrackNumber` ASC");

        auto result = d->mSelectTrackQuery.prepare(selectTrackQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackQuery.lastError();
        }
    }
    {
        auto selectTrackFromIdQueryText = QStringLiteral("SELECT "
                                                         "tracks.`Id`, "
                                                         "tracks.`Title`, "
                                                         "tracks.`AlbumID`, "
                                                         "artist.`Name`, "
                                                         "artistAlbum.`Name`, "
                                                         "tracksMapping.`FileName`, "
                                                         "tracks.`TrackNumber`, "
                                                         "tracks.`DiscNumber`, "
                                                         "tracks.`Duration`, "
                                                         "album.`Title`, "
                                                         "tracks.`Rating`, "
                                                         "album.`CoverFileName`, "
                                                         "album.`IsSingleDiscAlbum` "
                                                         "FROM "
                                                         "`Tracks` tracks, `Artists` artist, `TracksArtists` trackArtist, "
                                                         "`Albums` album, `TracksMapping` tracksMapping "
                                                         "LEFT JOIN `AlbumsArtists` artistAlbumMapping ON artistAlbumMapping.`AlbumID` = album.`ID` "
                                                         "LEFT JOIN `Artists` artistAlbum ON artistAlbum.`ID` = artistAlbumMapping.`ArtistID` "
                                                         "WHERE "
                                                         "tracks.`ID` = trackArtist.`TrackID` AND "
                                                         "artist.`ID` = trackArtist.`ArtistID` AND "
                                                         "tracks.`ID` = :trackId AND "
                                                         "tracks.`AlbumID` = album.`ID` AND "
                                                         "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                         "tracksMapping.`Priority` = (SELECT MIN(`Priority`) FROM `TracksMapping` WHERE `TrackID` = tracks.`ID`)");

        auto result = d->mSelectTrackFromIdQuery.prepare(selectTrackFromIdQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackFromIdQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackFromIdQuery.lastError();
        }
    }
    {
        auto selectCountAlbumsQueryText = QStringLiteral("SELECT count(*) "
                                                         "FROM `Albums` album, `Artists` artist, `AlbumsArtists` albumArtist "
                                                         "WHERE artist.`Name` = :artistName AND "
                                                         "album.`ID` = albumArtist.`AlbumID` AND "
                                                         "artist.`ID` = albumArtist.`ArtistID`");

        const auto result = d->mSelectCountAlbumsForArtistQuery.prepare(selectCountAlbumsQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForArtistQuery.lastError();
        }
    }
    {
        auto selectAlbumIdFromTitleQueryText = QStringLiteral("SELECT "
                                                              "album.`ID` "
                                                              "FROM "
                                                              "`Albums` album, `Artists` artist, `AlbumsArtists` albumArtist "
                                                              "WHERE "
                                                              "artist.`Name` = :artistName AND "
                                                              "album.`ID` = albumArtist.`AlbumID` AND "
                                                              "artist.`ID` = albumArtist.`ArtistID` AND "
                                                              "album.`Title` = :title");

        auto result = d->mSelectAlbumIdFromTitleQuery.prepare(selectAlbumIdFromTitleQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleQuery.lastError();
        }
    }

    {
        auto selectAlbumIdFromTitleAndArtistQueryText = QStringLiteral("SELECT "
                                                                       "album.`ID` "
                                                                       "FROM "
                                                                       "`Albums` album, "
                                                                       "`AlbumsArtists` albumArtist "
                                                                       "WHERE "
                                                                       "album.`ID` = albumArtist.`AlbumID` AND "
                                                                       "album.`Title` = :title AND "
                                                                       "albumArtist.`ArtistID` = :artistId");

        auto result = d->mSelectAlbumIdFromTitleAndArtistQuery.prepare(selectAlbumIdFromTitleAndArtistQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleAndArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleAndArtistQuery.lastError();
        }
    }

    {
        auto selectAlbumIdFromTitleWithoutArtistQueryText = QStringLiteral("SELECT "
                                                                           "album.`ID` "
                                                                           "FROM "
                                                                           "`Albums` album "
                                                                           "WHERE "
                                                                           "album.`Title` = :title AND "
                                                                           "NOT EXISTS ("
                                                                           "SELECT "
                                                                           "albumArtist.`AlbumID` "
                                                                           "FROM "
                                                                           "`AlbumsArtists` albumArtist "
                                                                           "WHERE "
                                                                           "albumArtist.`AlbumID` = album.`ID`"
                                                                           ")");

        auto result = d->mSelectAlbumIdFromTitleWithoutArtistQuery.prepare(selectAlbumIdFromTitleWithoutArtistQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastError();
        }
    }

    {
        auto insertAlbumQueryText = QStringLiteral("INSERT INTO Albums (`ID`, `Title`, `CoverFileName`, `TracksCount`, `IsSingleDiscAlbum`) "
                                                   "VALUES (:albumId, :title, :coverFileName, :tracksCount, :isSingleDiscAlbum)");

        auto result = d->mInsertAlbumQuery.prepare(insertAlbumQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertAlbumQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertAlbumQuery.lastError();
        }
    }

    {
        auto insertAlbumArtistQueryText = QStringLiteral("INSERT INTO `AlbumsArtists` (`AlbumID`, `ArtistID`) "
                                                         "VALUES (:albumId, :artistId)");

        auto result = d->mInsertAlbumArtistQuery.prepare(insertAlbumArtistQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertAlbumArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertAlbumArtistQuery.lastError();
        }
    }

    {
        auto insertTrackArtistQueryText = QStringLiteral("INSERT INTO `TracksArtists` (`TrackID`, `ArtistID`) "
                                                         "VALUES (:trackId, :artistId)");

        auto result = d->mInsertTrackArtistQuery.prepare(insertTrackArtistQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertTrackArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertTrackArtistQuery.lastError();
        }
    }

    {
        auto insertTrackMappingQueryText = QStringLiteral("INSERT INTO `TracksMapping` (`FileName`, `DiscoverID`, `Priority`, `TrackValid`) "
                                                          "VALUES (:fileName, :discoverId, :priority, 1)");

        auto result = d->mInsertTrackMapping.prepare(insertTrackMappingQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertTrackMapping.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertTrackMapping.lastError();
        }
    }

    {
        auto initialUpdateTracksValidityQueryText = QStringLiteral("UPDATE `TracksMapping` SET `TrackValid` = 0");

        auto result = d->mInitialUpdateTracksValidity.prepare(initialUpdateTracksValidityQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mInitialUpdateTracksValidity.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mInitialUpdateTracksValidity.lastError();
        }
    }

    {
        auto initialUpdateTracksValidityQueryText = QStringLiteral("UPDATE `TracksMapping` SET `TrackValid` = 1, `TrackID` = :trackId, `Priority` = :priority "
                                                                   "WHERE `FileName` = :fileName");

        auto result = d->mUpdateTrackMapping.prepare(initialUpdateTracksValidityQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mUpdateTrackMapping.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mUpdateTrackMapping.lastError();
        }
    }

    {
        auto removeTracksMappingFromSourceQueryText = QStringLiteral("DELETE FROM `TracksMapping` "
                                                                     "WHERE `FileName` = :fileName AND `DiscoverID` = :sourceId");

        auto result = d->mRemoveTracksMappingFromSource.prepare(removeTracksMappingFromSourceQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveTracksMappingFromSource.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveTracksMappingFromSource.lastError();
        }
    }

    {
        auto removeTracksMappingQueryText = QStringLiteral("DELETE FROM `TracksMapping` "
                                                           "WHERE `FileName` = :fileName");

        auto result = d->mRemoveTracksMapping.prepare(removeTracksMappingQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveTracksMapping.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveTracksMapping.lastError();
        }
    }

    {
        auto selectTracksWithoutMappingQueryText = QStringLiteral("SELECT "
                                                                  "tracks.`Id`, "
                                                                  "tracks.`Title`, "
                                                                  "tracks.`AlbumID`, "
                                                                  "artist.`Name`, "
                                                                  "artistAlbum.`Name`, "
                                                                  "\"\" as FileName, "
                                                                  "tracks.`TrackNumber`, "
                                                                  "tracks.`DiscNumber`, "
                                                                  "tracks.`Duration`, "
                                                                  "album.`Title`, "
                                                                  "tracks.`Rating`, "
                                                                  "album.`CoverFileName`, "
                                                                  "album.`IsSingleDiscAlbum` "
                                                                  "FROM "
                                                                  "`Tracks` tracks, "
                                                                  "`Artists` artist, "
                                                                  "`TracksArtists` trackArtist, "
                                                                  "`Albums` album "
                                                                  "LEFT JOIN `AlbumsArtists` artistAlbumMapping ON artistAlbumMapping.`AlbumID` = album.`ID` "
                                                                  "LEFT JOIN `Artists` artistAlbum ON artistAlbum.`ID` = artistAlbumMapping.`ArtistID` "
                                                                  "WHERE "
                                                                  "tracks.`ID` = trackArtist.`TrackID` AND "
                                                                  "artist.`ID` = trackArtist.`ArtistID` AND "
                                                                  "tracks.`AlbumID` = album.`ID` AND "
                                                                  "tracks.`ID` NOT IN (SELECT tracksMapping2.`TrackID` FROM `TracksMapping` tracksMapping2)");

        auto result = d->mSelectTracksWithoutMappingQuery.prepare(selectTracksWithoutMappingQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTracksWithoutMappingQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTracksWithoutMappingQuery.lastError();
        }
    }

    {
        auto selectTracksMappingQueryText = QStringLiteral("SELECT "
                                                           "`TrackID`, "
                                                           "`FileName`, "
                                                           "`DiscoverID`, "
                                                           "`Priority` "
                                                           "FROM "
                                                           "`TracksMapping` "
                                                           "WHERE "
                                                           "`FileName` = :fileName");

        auto result = d->mSelectTracksMapping.prepare(selectTracksMappingQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTracksMapping.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTracksMapping.lastError();
        }
    }

    {
        auto selectTracksMappingPriorityQueryText = QStringLiteral("SELECT "
                                                                   "`Priority` "
                                                                   "FROM "
                                                                   "`TracksMapping` "
                                                                   "WHERE "
                                                                   "`TrackID` = :trackId AND "
                                                                   "`FileName` = :fileName");

        auto result = d->mSelectTracksMappingPriority.prepare(selectTracksMappingPriorityQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTracksMappingPriority.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTracksMappingPriority.lastError();
        }
    }

    {
        auto selectTracksMappingPriorityQueryByTrackIdText = QStringLiteral("SELECT "
                                                                            "MAX(`Priority`) "
                                                                            "FROM "
                                                                            "`TracksMapping` "
                                                                            "WHERE "
                                                                            "`TrackID` = :trackId");

        auto result = d->mSelectTracksMappingPriorityByTrackId.prepare(selectTracksMappingPriorityQueryByTrackIdText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTracksMappingPriorityByTrackId.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTracksMappingPriorityByTrackId.lastError();
        }
    }

    {
        auto selectAllTrackFilesFromSourceQueryText = QStringLiteral("SELECT "
                                                                     "tracksMapping.`FileName` "
                                                                     "FROM "
                                                                     "`TracksMapping` tracksMapping "
                                                                     "WHERE "
                                                                     "tracksMapping.`DiscoverID` = :discoverId");

        auto result = d->mSelectAllTrackFilesFromSourceQuery.prepare(selectAllTrackFilesFromSourceQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAllTrackFilesFromSourceQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAllTrackFilesFromSourceQuery.lastError();
        }
    }

    {
        auto findInvalidTrackFilesText = QStringLiteral("SELECT "
                                                        "tracksMapping.`FileName`, "
                                                        "tracksMapping.`DiscoverID` "
                                                        "FROM "
                                                        "`TracksMapping` tracksMapping "
                                                        "WHERE "
                                                        "tracksMapping.`TrackValid` = 0");

        auto result = d->mFindInvalidTrackFilesQuery.prepare(findInvalidTrackFilesText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mFindInvalidTrackFilesQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mFindInvalidTrackFilesQuery.lastError();
        }
    }

    {
        auto insertMusicSourceQueryText = QStringLiteral("INSERT OR IGNORE INTO `DiscoverSource` (`ID`, `Name`) "
                                                         "VALUES (:discoverId, :name)");

        auto result = d->mInsertMusicSource.prepare(insertMusicSourceQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertMusicSource.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertMusicSource.lastError();
        }
    }

    {
        auto selectMusicSourceQueryText = QStringLiteral("SELECT `ID` FROM `DiscoverSource` WHERE `Name` = :name");

        auto result = d->mSelectMusicSource.prepare(selectMusicSourceQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectMusicSource.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectMusicSource.lastError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.`ID`,  tracksMapping.`FileName` "
                                                   "FROM "
                                                   "`Tracks` tracks, `Artists` artist, `TracksArtists` trackArtist, "
                                                   "`TracksMapping` tracksMapping "
                                                   "WHERE "
                                                   "tracks.`Title` = :title AND "
                                                   "tracks.`AlbumID` = :album AND "
                                                   "artist.`Name` = :artist AND "
                                                   "tracks.`ID` = trackArtist.`TrackID` AND "
                                                   "artist.`ID` = trackArtist.`ArtistID` AND "
                                                   "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                   "tracksMapping.`Priority` = (SELECT MIN(`Priority`) FROM `TracksMapping` WHERE `TrackID` = tracks.`ID`)");

        auto result = d->mSelectTrackIdFromTitleAlbumIdArtistQuery.prepare(selectTrackQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumIdArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumIdArtistQuery.lastError();
        }

        auto insertTrackQueryText = QStringLiteral("INSERT INTO `Tracks` (`ID`, `Title`, `AlbumID`, `TrackNumber`, `DiscNumber`, `Duration`, `Rating`) "
                                                   "VALUES (:trackId, :title, :album, :trackNumber, :discNumber, :trackDuration, :trackRating)");

        result = d->mInsertTrackQuery.prepare(insertTrackQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertTrackQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertTrackQuery.lastError();
        }
    }
    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.ID "
                                                   "FROM "
                                                   "`Tracks` tracks, "
                                                   "`Albums` albums, "
                                                   "`TracksArtists` trackArtist, "
                                                   "`Artists` artist "
                                                   "WHERE "
                                                   "tracks.`Title` = :title AND "
                                                   "tracks.`AlbumID` = albums.`ID` AND "
                                                   "albums.`Title` = :album AND "
                                                   "tracks.`TrackNumber` = :trackNumber AND "
                                                   "tracks.`DiscNumber` = :discNumber AND "
                                                   "trackArtist.`TrackID` = tracks.`ID` AND "
                                                   "trackArtist.`ArtistID` = artist.`ID` AND "
                                                   "artist.`Name` = :artist");

        auto result = d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.prepare(selectTrackQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.lastError();
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
                                                   "tracks.`AlbumID` = albums.`ID` AND "
                                                   "albums.`Title` = :album AND "
                                                   "tracks.`TrackNumber` = :trackNumber AND "
                                                   "tracks.`DiscNumber` = :discNumber AND "
                                                   "( "
                                                   "( NOT EXISTS (SELECT albumArtistMapping.`AlbumID` "
                                                   "FROM "
                                                   "`AlbumsArtists` albumArtistMapping "
                                                   "WHERE "
                                                   "albumArtistMapping.`AlbumID` = albums.`ID`) "
                                                   ") OR "
                                                   "( EXISTS (SELECT albumArtistMapping.`AlbumID` "
                                                   "FROM "
                                                   "`AlbumsArtists` albumArtistMapping, "
                                                   "`Artists` albumArtist "
                                                   "WHERE "
                                                   "albumArtist.`Name` = :albumArtist AND "
                                                   "albumArtist.`ID` = albumArtistMapping.`ArtistID` AND "
                                                   "albumArtistMapping.`AlbumID` = albums.`ID`) "
                                                   ") "
                                                   ")");

        auto result = d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.prepare(selectTrackQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.lastError();
        }
    }

    {
        auto selectAlbumTrackCountQueryText = QStringLiteral("SELECT `TracksCount` "
                                                             "FROM `Albums`"
                                                             "WHERE "
                                                             "`ID` = :albumId");

        auto result = d->mSelectAlbumTrackCountQuery.prepare(selectAlbumTrackCountQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumTrackCountQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumTrackCountQuery.lastError();
        }
    }
    {
        auto updateAlbumQueryText = QStringLiteral("UPDATE `Albums` "
                                                   "SET `TracksCount` = (SELECT COUNT(*) FROM `Tracks` WHERE `AlbumID` = :albumId) "
                                                   "WHERE "
                                                   "`ID` = :albumId");

        auto result = d->mUpdateAlbumQuery.prepare(updateAlbumQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mUpdateAlbumQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mUpdateAlbumQuery.lastError();
        }
    }

    {
        auto updateIsSingleDiscAlbumFromIdQueryText = QStringLiteral("UPDATE `Albums` "
                                                                     "SET `IsSingleDiscAlbum` = (SELECT COUNT(DISTINCT DiscNumber) = 1 FROM `Tracks` WHERE `AlbumID` = :albumId) "
                                                                     "WHERE "
                                                                     "`ID` = :albumId");

        auto result = d->mUpdateIsSingleDiscAlbumFromIdQuery.prepare(updateIsSingleDiscAlbumFromIdQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mUpdateIsSingleDiscAlbumFromIdQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mUpdateIsSingleDiscAlbumFromIdQuery.lastError();
        }
    }

    {
        auto updateAlbumArtUriFromAlbumIdQueryText = QStringLiteral("UPDATE `Albums` "
                                                                    "SET `CoverFileName` = :coverFileName "
                                                                    "WHERE "
                                                                    "`ID` = :albumId");

        auto result = d->mUpdateAlbumArtUriFromAlbumIdQuery.prepare(updateAlbumArtUriFromAlbumIdQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtUriFromAlbumIdQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mUpdateAlbumArtUriFromAlbumIdQuery.lastError();
        }
    }

    {
        auto selectTracksFromArtistQueryText = QStringLiteral("SELECT "
                                                              "tracks.`ID`, "
                                                              "tracks.`Title`, "
                                                              "tracks.`AlbumID`, "
                                                              "artist.`Name`, "
                                                              "artistAlbum.`Name`, "
                                                              "tracksMapping.`FileName`, "
                                                              "tracks.`TrackNumber`, "
                                                              "tracks.`DiscNumber`, "
                                                              "tracks.`Duration`, "
                                                              "album.`Title`, "
                                                              "tracks.`Rating`, "
                                                              "album.`CoverFileName`, "
                                                              "album.`IsSingleDiscAlbum` "
                                                              "FROM `Tracks` tracks, `Albums` album, `Artists` artist, `TracksArtists` trackArtist, "
                                                              "`TracksMapping` tracksMapping "
                                                              "LEFT JOIN `AlbumsArtists` artistAlbumMapping ON artistAlbumMapping.`AlbumID` = album.`ID` "
                                                              "LEFT JOIN `Artists` artistAlbum ON artistAlbum.`ID` = artistAlbumMapping.`ArtistID` "
                                                              "WHERE "
                                                              "artist.`Name` = :artistName AND "
                                                              "tracks.`AlbumID` = album.`ID` AND "
                                                              "artist.`ID` = trackArtist.`ArtistID` AND "
                                                              "tracks.`ID` = trackArtist.`TrackID` AND "
                                                              "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                              "tracksMapping.`Priority` = (SELECT MIN(`Priority`) FROM `TracksMapping` WHERE `TrackID` = tracks.`ID`) "
                                                              "ORDER BY tracks.`Title` ASC, "
                                                              "album.`Title` ASC");

        auto result = d->mSelectTracksFromArtist.prepare(selectTracksFromArtistQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTracksFromArtist.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTracksFromArtist.lastError();
        }
    }

    {
        auto selectAlbumIdsFromArtistQueryText = QStringLiteral("SELECT "
                                                                "album.`ID` "
                                                                "FROM "
                                                                "`Albums` album, "
                                                                "`Artists` artist,"
                                                                "`AlbumsArtists` albumArtist "
                                                                "WHERE "
                                                                "album.`ID` = albumArtist.`AlbumID` AND "
                                                                "artist.`ID` = albumArtist.`ArtistID` AND "
                                                                "artist.`Name` = :artistName");

        auto result = d->mSelectAlbumIdsFromArtist.prepare(selectAlbumIdsFromArtistQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumIdsFromArtist.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumIdsFromArtist.lastError();
        }
    }

    {
        auto selectArtistQueryText = QStringLiteral("SELECT `ID`, "
                                                    "`Name` "
                                                    "FROM `Artists` "
                                                    "WHERE "
                                                    "`ID` = :artistId");

        auto result = d->mSelectArtistQuery.prepare(selectArtistQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectArtistQuery.lastError();
        }
    }

    {
        auto selectTrackFromFilePathQueryText = QStringLiteral("SELECT "
                                                               "tracks.`ID`, "
                                                               "tracks.`Title`, "
                                                               "tracks.`AlbumID`, "
                                                               "artist.`Name`, "
                                                               "artistAlbum.`Name`, "
                                                               "tracksMapping.`FileName`, "
                                                               "tracks.`TrackNumber`, "
                                                               "tracks.`DiscNumber`, "
                                                               "tracks.`Duration`, "
                                                               "album.`Title`, "
                                                               "tracks.`Rating`, "
                                                               "album.`CoverFileName`, "
                                                               "album.`IsSingleDiscAlbum` "
                                                               "FROM `Tracks` tracks, `Artists` artist, `Albums` album, `TracksArtists` trackArtist, "
                                                               "`TracksMapping` tracksMapping "
                                                               "LEFT JOIN `AlbumsArtists` artistAlbumMapping ON artistAlbumMapping.`AlbumID` = album.`ID` "
                                                               "LEFT JOIN `Artists` artistAlbum ON artistAlbum.`ID` = artistAlbumMapping.`ArtistID` "
                                                               "WHERE "
                                                               "tracks.`AlbumID` = album.`ID` AND "
                                                               "artist.`ID` = trackArtist.`ArtistID` AND "
                                                               "tracks.`ID` = trackArtist.`TrackID` AND "
                                                               "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                               "tracksMapping.`FileName` = :filePath AND "
                                                               "tracksMapping.`Priority` = (SELECT MIN(`Priority`) FROM `TracksMapping` WHERE `TrackID` = tracks.`ID`)");

        auto result = d->mSelectTrackFromFilePathQuery.prepare(selectTrackFromFilePathQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackFromFilePathQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackFromFilePathQuery.lastError();
        }
    }

    {
        auto removeTrackQueryText = QStringLiteral("DELETE FROM `Tracks` "
                                                   "WHERE "
                                                   "`ID` = :trackId");

        auto result = d->mRemoveTrackQuery.prepare(removeTrackQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveTrackQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveTrackQuery.lastError();
        }
    }

    {
        auto removeTrackArtistQueryText = QStringLiteral("DELETE FROM `TracksArtists` "
                                                         "WHERE "
                                                         "`TrackID` = :trackId");

        auto result = d->mRemoveTrackArtistQuery.prepare(removeTrackArtistQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveTrackArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveTrackArtistQuery.lastError();
        }
    }

    {
        auto removeAlbumQueryText = QStringLiteral("DELETE FROM `Albums` "
                                                   "WHERE "
                                                   "`ID` = :albumId");

        auto result = d->mRemoveAlbumQuery.prepare(removeAlbumQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveAlbumQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveAlbumQuery.lastError();
        }
    }

    {
        auto removeAlbumArtistQueryText = QStringLiteral("DELETE FROM `AlbumsArtists` "
                                                         "WHERE "
                                                         "`AlbumID` = :albumId");

        auto result = d->mRemoveAlbumArtistQuery.prepare(removeAlbumArtistQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveAlbumArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveAlbumArtistQuery.lastError();
        }
    }

    {
        auto removeAlbumQueryText = QStringLiteral("DELETE FROM `Artists` "
                                                   "WHERE "
                                                   "`ID` = :artistId");

        auto result = d->mRemoveArtistQuery.prepare(removeAlbumQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveArtistQuery.lastError();
        }
    }

    transactionResult = finishTransaction();

    d->mInitFinished = true;
    Q_EMIT requestsInitDone();
}

qulonglong DatabaseInterface::insertAlbum(const QString &title, const QString &albumArtist, const QString &trackArtist,
                                          const QUrl &albumArtURI, int tracksCount, bool isSingleDiscAlbum)
{
    auto result = qulonglong(0);

    if (title.isEmpty()) {
        return result;
    }

    if (!albumArtist.isEmpty() || !trackArtist.isEmpty()) {
        d->mSelectAlbumIdFromTitleAndArtistQuery.bindValue(QStringLiteral(":title"), title);
        if (!albumArtist.isEmpty()) {
            d->mSelectAlbumIdFromTitleAndArtistQuery.bindValue(QStringLiteral(":artistId"), insertArtist(albumArtist));
        } else {
            d->mSelectAlbumIdFromTitleAndArtistQuery.bindValue(QStringLiteral(":artistId"), insertArtist(trackArtist));
        }

        auto queryResult = d->mSelectAlbumIdFromTitleAndArtistQuery.exec();

        if (!queryResult || !d->mSelectAlbumIdFromTitleAndArtistQuery.isSelect() || !d->mSelectAlbumIdFromTitleAndArtistQuery.isActive()) {
            Q_EMIT databaseError();

            qDebug() << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleAndArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleAndArtistQuery.boundValues();
            qDebug() << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleAndArtistQuery.lastError();

            d->mSelectAlbumIdFromTitleAndArtistQuery.finish();

            return result;
        }

        if (d->mSelectAlbumIdFromTitleAndArtistQuery.next()) {
            result = d->mSelectAlbumIdFromTitleAndArtistQuery.record().value(0).toULongLong();

            d->mSelectAlbumIdFromTitleAndArtistQuery.finish();

            return result;
        }

        d->mSelectAlbumIdFromTitleAndArtistQuery.finish();
    }

    if (result == 0) {
        d->mSelectAlbumIdFromTitleWithoutArtistQuery.bindValue(QStringLiteral(":title"), title);

        auto queryResult = d->mSelectAlbumIdFromTitleWithoutArtistQuery.exec();

        if (!queryResult || !d->mSelectAlbumIdFromTitleWithoutArtistQuery.isSelect() || !d->mSelectAlbumIdFromTitleWithoutArtistQuery.isActive()) {
            Q_EMIT databaseError();

            qDebug() << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.boundValues();
            qDebug() << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastError();

            d->mSelectAlbumIdFromTitleWithoutArtistQuery.finish();

            return result;
        }

        if (d->mSelectAlbumIdFromTitleWithoutArtistQuery.next()) {
            result = d->mSelectAlbumIdFromTitleWithoutArtistQuery.record().value(0).toULongLong();

            d->mSelectAlbumIdFromTitleWithoutArtistQuery.finish();

            return result;
        }

        d->mSelectAlbumIdFromTitleWithoutArtistQuery.finish();
    }

    d->mInsertAlbumQuery.bindValue(QStringLiteral(":albumId"), d->mAlbumId);
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":title"), title);
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":coverFileName"), albumArtURI);
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":tracksCount"), tracksCount);
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":isSingleDiscAlbum"), isSingleDiscAlbum);

    auto queryResult = d->mInsertAlbumQuery.exec();

    if (!queryResult || !d->mInsertAlbumQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::insertAlbum" << d->mInsertAlbumQuery.lastQuery();
        qDebug() << "DatabaseInterface::insertAlbum" << d->mInsertAlbumQuery.boundValues();
        qDebug() << "DatabaseInterface::insertAlbum" << d->mInsertAlbumQuery.lastError();

        d->mInsertAlbumQuery.finish();

        return result;
    }

    result = d->mAlbumId;

    d->mInsertAlbumQuery.finish();

    if (!albumArtist.isEmpty()) {
        d->mInsertAlbumArtistQuery.bindValue(QStringLiteral(":albumId"), d->mAlbumId);
        d->mInsertAlbumArtistQuery.bindValue(QStringLiteral(":artistId"), insertArtist(albumArtist));

        queryResult = d->mInsertAlbumArtistQuery.exec();

        if (!queryResult || !d->mInsertAlbumArtistQuery.isActive()) {
            Q_EMIT databaseError();

            qDebug() << "DatabaseInterface::insertAlbum" << d->mInsertAlbumArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::insertAlbum" << d->mInsertAlbumArtistQuery.boundValues();
            qDebug() << "DatabaseInterface::insertAlbum" << d->mInsertAlbumArtistQuery.lastError();

            d->mInsertAlbumArtistQuery.finish();

            return result;
        }

        d->mInsertAlbumArtistQuery.finish();
    }

    ++d->mAlbumId;

    Q_EMIT albumAdded(internalAlbumFromId(d->mAlbumId - 1));

    return result;
}

bool DatabaseInterface::updateAlbumFromId(qulonglong albumId, const QUrl &albumArtUri, const MusicAudioTrack &currentTrack)
{
    auto modifiedAlbum = false;

    d->mUpdateIsSingleDiscAlbumFromIdQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = d->mUpdateIsSingleDiscAlbumFromIdQuery.exec();

    if (!result || !d->mUpdateIsSingleDiscAlbumFromIdQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mUpdateIsSingleDiscAlbumFromIdQuery.lastQuery();
        qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mUpdateIsSingleDiscAlbumFromIdQuery.boundValues();
        qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mUpdateIsSingleDiscAlbumFromIdQuery.lastError();

        d->mUpdateIsSingleDiscAlbumFromIdQuery.finish();

        return modifiedAlbum;
    }

    modifiedAlbum = (d->mUpdateIsSingleDiscAlbumFromIdQuery.numRowsAffected() != 0);
    d->mUpdateIsSingleDiscAlbumFromIdQuery.finish();

    if (!albumArtUri.isValid()) {
        return modifiedAlbum;
    }

    const auto &album = internalAlbumFromId(albumId);

    if (!album.albumArtURI().isValid() || album.albumArtURI() == albumArtUri) {
        d->mUpdateAlbumArtUriFromAlbumIdQuery.bindValue(QStringLiteral(":albumId"), albumId);
        d->mUpdateAlbumArtUriFromAlbumIdQuery.bindValue(QStringLiteral(":coverFileName"), albumArtUri);

        result = d->mUpdateAlbumArtUriFromAlbumIdQuery.exec();

        if (!result || !d->mUpdateAlbumArtUriFromAlbumIdQuery.isActive()) {
            Q_EMIT databaseError();

            qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mUpdateAlbumArtUriFromAlbumIdQuery.lastQuery();
            qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mUpdateAlbumArtUriFromAlbumIdQuery.boundValues();
            qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mUpdateAlbumArtUriFromAlbumIdQuery.lastError();

            d->mUpdateAlbumArtUriFromAlbumIdQuery.finish();

            return modifiedAlbum;
        }

        d->mUpdateAlbumArtUriFromAlbumIdQuery.finish();

        modifiedAlbum = true;
    }

    if (!album.isValidArtist() && album.canUpdateArtist(currentTrack)) {
        d->mRemoveAlbumArtistQuery.bindValue(QStringLiteral(":albumId"), albumId);

        result = d->mRemoveAlbumArtistQuery.exec();

        if (!result || !d->mRemoveAlbumArtistQuery.isActive()) {
            Q_EMIT databaseError();

            qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mRemoveAlbumArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mRemoveAlbumArtistQuery.boundValues();
            qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mRemoveAlbumArtistQuery.lastError();

            d->mRemoveAlbumArtistQuery.finish();

            return modifiedAlbum;
        }

        d->mRemoveAlbumArtistQuery.finish();

        d->mInsertAlbumArtistQuery.bindValue(QStringLiteral(":albumId"), albumId);
        d->mInsertAlbumArtistQuery.bindValue(QStringLiteral(":artistId"), insertArtist(currentTrack.albumArtist()));

        result = d->mInsertAlbumArtistQuery.exec();

        if (!result || !d->mInsertAlbumArtistQuery.isActive()) {
            Q_EMIT databaseError();

            qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mInsertAlbumArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mInsertAlbumArtistQuery.boundValues();
            qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mInsertAlbumArtistQuery.lastError();

            d->mInsertAlbumArtistQuery.finish();

            return modifiedAlbum;
        }

        d->mInsertAlbumArtistQuery.finish();

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

    auto queryResult = d->mSelectArtistByNameQuery.exec();

    if (!queryResult || !d->mSelectArtistByNameQuery.isSelect() || !d->mSelectArtistByNameQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.lastQuery();
        qDebug() << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.boundValues();
        qDebug() << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.lastError();

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

    queryResult = d->mInsertArtistsQuery.exec();

    if (!queryResult || !d->mInsertArtistsQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::insertArtist" << d->mInsertArtistsQuery.lastQuery();
        qDebug() << "DatabaseInterface::insertArtist" << d->mInsertArtistsQuery.boundValues();
        qDebug() << "DatabaseInterface::insertArtist" << d->mInsertArtistsQuery.lastError();

        d->mInsertArtistsQuery.finish();

        return result;
    }

    result = d->mArtistId;

    ++d->mArtistId;

    d->mInsertArtistsQuery.finish();

    Q_EMIT artistAdded(internalArtistFromId(d->mArtistId - 1));

    return result;
}

void DatabaseInterface::insertTrackOrigin(const QUrl &fileNameURI, qulonglong discoverId)
{
    d->mInsertTrackMapping.bindValue(QStringLiteral(":discoverId"), discoverId);
    d->mInsertTrackMapping.bindValue(QStringLiteral(":fileName"), fileNameURI);
    d->mInsertTrackMapping.bindValue(QStringLiteral(":priority"), 1);

    auto queryResult = d->mInsertTrackMapping.exec();

    if (!queryResult || !d->mInsertTrackMapping.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::insertArtist" << d->mInsertTrackMapping.lastQuery();
        qDebug() << "DatabaseInterface::insertArtist" << d->mInsertTrackMapping.boundValues();
        qDebug() << "DatabaseInterface::insertArtist" << d->mInsertTrackMapping.lastError();

        d->mInsertTrackMapping.finish();

        return;
    }

    d->mInsertTrackMapping.finish();
}

void DatabaseInterface::updateTrackOrigin(qulonglong trackId, const QUrl &fileName)
{
    d->mUpdateTrackMapping.bindValue(QStringLiteral(":trackId"), trackId);
    d->mUpdateTrackMapping.bindValue(QStringLiteral(":fileName"), fileName);
    d->mUpdateTrackMapping.bindValue(QStringLiteral(":priority"), computeTrackPriority(trackId, fileName));

    auto queryResult = d->mUpdateTrackMapping.exec();

    if (!queryResult || !d->mUpdateTrackMapping.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::updateTrackOrigin" << d->mUpdateTrackMapping.lastQuery();
        qDebug() << "DatabaseInterface::updateTrackOrigin" << d->mUpdateTrackMapping.boundValues();
        qDebug() << "DatabaseInterface::updateTrackOrigin" << d->mUpdateTrackMapping.lastError();

        d->mUpdateTrackMapping.finish();

        return;
    }

    d->mInsertTrackMapping.finish();
}

int DatabaseInterface::computeTrackPriority(qulonglong trackId, const QUrl &fileName)
{
    auto result = int(1);

    if (!d) {
        return result;
    }

    d->mSelectTracksMappingPriority.bindValue(QStringLiteral(":trackId"), trackId);
    d->mSelectTracksMappingPriority.bindValue(QStringLiteral(":fileName"), fileName);

    auto queryResult = d->mSelectTracksMappingPriority.exec();

    if (!queryResult || !d->mSelectTracksMappingPriority.isSelect() || !d->mSelectTracksMappingPriority.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMappingPriority.lastQuery();
        qDebug() << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMappingPriority.boundValues();
        qDebug() << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMappingPriority.lastError();

        d->mSelectTracksMappingPriority.finish();

        return result;
    }

    if (d->mSelectTracksMappingPriority.next()) {
        result = d->mSelectTracksMappingPriority.record().value(0).toInt();

        d->mSelectTracksMappingPriority.finish();

        return result;
    }

    d->mSelectTracksMappingPriority.finish();

    d->mSelectTracksMappingPriorityByTrackId.bindValue(QStringLiteral(":trackId"), trackId);

    queryResult = d->mSelectTracksMappingPriorityByTrackId.exec();

    if (!queryResult || !d->mSelectTracksMappingPriorityByTrackId.isSelect() || !d->mSelectTracksMappingPriorityByTrackId.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMappingPriorityByTrackId.lastQuery();
        qDebug() << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMappingPriorityByTrackId.boundValues();
        qDebug() << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMappingPriorityByTrackId.lastError();

        d->mSelectTracksMappingPriorityByTrackId.finish();

        return result;
    }

    if (d->mSelectTracksMappingPriorityByTrackId.next()) {
        result = d->mSelectTracksMappingPriorityByTrackId.record().value(0).toInt() + 1;
    }

    d->mSelectTracksMappingPriorityByTrackId.finish();

    return result;
}

qulonglong DatabaseInterface::internalInsertTrack(const MusicAudioTrack &oneTrack, const QHash<QString, QUrl> &covers,
                                                  int originTrackId, QSet<qulonglong> &modifiedAlbumIds, TrackFileInsertType insertType)
{
    qulonglong resultId = 0;

    if (oneTrack.albumArtist().isEmpty()) {
        return resultId;
    }

    auto albumId = insertAlbum(oneTrack.albumName(), (oneTrack.isValidAlbumArtist() ? oneTrack.albumArtist() : QString()),
                               oneTrack.artist(), covers[oneTrack.resourceURI().toString()], 0, true);

    if (albumId == 0) {
        return resultId;
    }

    auto otherTrackId = getDuplicateTrackIdFromTitleAlbumTracDiscNumber(oneTrack.title(), oneTrack.albumName(), oneTrack.albumArtist(),
                                                                        oneTrack.trackNumber(), oneTrack.discNumber());
    bool isModifiedTrack = (otherTrackId != 0) || (insertType == TrackFileInsertType::ModifiedTrackFileInsert);
    bool isSameTrack = false;

    qulonglong oldAlbumId = 0;

    if (isModifiedTrack) {
        if (otherTrackId == 0) {
            otherTrackId = internalTrackIdFromFileName(oneTrack.resourceURI());
        }

        originTrackId = otherTrackId;

        const auto &oldTrack = internalTrackFromDatabaseId(originTrackId);

        isSameTrack = (oldTrack.title() == oneTrack.title());
        isSameTrack = isSameTrack && (oldTrack.albumName() == oneTrack.albumName());
        isSameTrack = isSameTrack && (oldTrack.artist() == oneTrack.artist());
        isSameTrack = isSameTrack && (oldTrack.trackNumber() == oneTrack.trackNumber());
        isSameTrack = isSameTrack && (oldTrack.discNumber() == oneTrack.discNumber());
        isSameTrack = isSameTrack && (oldTrack.duration() == oneTrack.duration());
        isSameTrack = isSameTrack && (oldTrack.rating() == oneTrack.rating());
        isSameTrack = isSameTrack && (oldTrack.resourceURI() == oneTrack.resourceURI());

        oldAlbumId = internalAlbumIdFromTitleAndArtist(oldTrack.albumName(), oldTrack.albumArtist());

        if (!isSameTrack) {
            removeTrackInDatabase(originTrackId);
        }
    } else {
        originTrackId = d->mTrackId;
    }

    resultId = originTrackId;

    if (!isSameTrack) {
        d->mInsertTrackQuery.bindValue(QStringLiteral(":trackId"), originTrackId);
        d->mInsertTrackQuery.bindValue(QStringLiteral(":title"), oneTrack.title());
        d->mInsertTrackQuery.bindValue(QStringLiteral(":album"), albumId);
        d->mInsertTrackQuery.bindValue(QStringLiteral(":trackNumber"), oneTrack.trackNumber());
        d->mInsertTrackQuery.bindValue(QStringLiteral(":discNumber"), oneTrack.discNumber());
        d->mInsertTrackQuery.bindValue(QStringLiteral(":trackDuration"), QVariant::fromValue<qlonglong>(oneTrack.duration().msecsSinceStartOfDay()));
        d->mInsertTrackQuery.bindValue(QStringLiteral(":trackRating"), oneTrack.rating());

        auto result = d->mInsertTrackQuery.exec();

        if (result && d->mInsertTrackQuery.isActive()) {
            d->mInsertTrackQuery.finish();

            d->mInsertTrackArtistQuery.bindValue(QStringLiteral(":trackId"), originTrackId);
            d->mInsertTrackArtistQuery.bindValue(QStringLiteral(":artistId"), insertArtist(oneTrack.artist()));

            result = d->mInsertTrackArtistQuery.exec();

            if (!result || !d->mInsertTrackArtistQuery.isActive()) {
                Q_EMIT databaseError();

                qDebug() << "DatabaseInterface::internalInsertTrack" << d->mInsertTrackArtistQuery.lastQuery();
                qDebug() << "DatabaseInterface::internalInsertTrack" << d->mInsertTrackArtistQuery.boundValues();
                qDebug() << "DatabaseInterface::internalInsertTrack" << d->mInsertTrackArtistQuery.lastError();

                d->mInsertTrackArtistQuery.finish();

                return resultId;
            }

            d->mInsertTrackArtistQuery.finish();


            if (!isModifiedTrack) {
                ++d->mTrackId;
            }

            updateTrackOrigin(originTrackId, oneTrack.resourceURI());

            if (isModifiedTrack) {
                Q_EMIT trackModified(internalTrackFromDatabaseId(originTrackId));
                modifiedAlbumIds.insert(albumId);
                if (oldAlbumId != 0) {
                    modifiedAlbumIds.insert(oldAlbumId);
                }
            } else {
                Q_EMIT trackAdded(originTrackId);
            }

            if (updateAlbumFromId(albumId, covers[oneTrack.resourceURI().toString()], oneTrack)) {
                modifiedAlbumIds.insert(albumId);
            }

            if (updateTracksCount(albumId)) {
                modifiedAlbumIds.insert(albumId);
            }
        } else {
            d->mInsertTrackQuery.finish();

            Q_EMIT databaseError();

            qDebug() << "DatabaseInterface::internalInsertTrack" << oneTrack << oneTrack.resourceURI();
            qDebug() << "DatabaseInterface::internalInsertTrack" << d->mInsertTrackQuery.lastQuery();
            qDebug() << "DatabaseInterface::internalInsertTrack" << d->mInsertTrackQuery.boundValues();
            qDebug() << "DatabaseInterface::internalInsertTrack" << d->mInsertTrackQuery.lastError();
        }
    }

    return resultId;
}

MusicAudioTrack DatabaseInterface::buildTrackFromDatabaseRecord(const QSqlRecord &trackRecord) const
{
    auto result = MusicAudioTrack();

    result.setDatabaseId(trackRecord.value(0).toULongLong());
    result.setTitle(trackRecord.value(1).toString());
    result.setParentId(trackRecord.value(2).toString());
    result.setArtist(trackRecord.value(3).toString());

    if (trackRecord.value(4).isValid()) {
        result.setAlbumArtist(trackRecord.value(4).toString());
    }

    result.setResourceURI(trackRecord.value(5).toUrl());
    result.setTrackNumber(trackRecord.value(6).toInt());
    result.setDiscNumber(trackRecord.value(7).toInt());
    result.setDuration(QTime::fromMSecsSinceStartOfDay(trackRecord.value(8).toInt()));
    result.setAlbumName(trackRecord.value(9).toString());
    result.setRating(trackRecord.value(10).toInt());
    result.setAlbumCover(trackRecord.value(11).toUrl());
    result.setIsSingleDiscAlbum(trackRecord.value(12).toBool());
    result.setValid(true);

    return result;
}

void DatabaseInterface::internalRemoveTracksList(const QList<QUrl> &removedTracks)
{
    for (const auto &removedTrackFileName : removedTracks) {
        d->mRemoveTracksMapping.bindValue(QStringLiteral(":fileName"), removedTrackFileName.toString());

        auto result = d->mRemoveTracksMapping.exec();

        if (!result || !d->mRemoveTracksMapping.isActive()) {
            Q_EMIT databaseError();

            qDebug() << "DatabaseInterface::internalRemoveTracksList" << d->mRemoveTracksMapping.lastQuery();
            qDebug() << "DatabaseInterface::internalRemoveTracksList" << d->mRemoveTracksMapping.boundValues();
            qDebug() << "DatabaseInterface::internalRemoveTracksList" << d->mRemoveTracksMapping.lastError();

            continue;
        }

        d->mRemoveTracksMapping.finish();
    }

    internalRemoveTracksWithoutMapping();
}

void DatabaseInterface::internalRemoveTracksList(const QList<QUrl> &removedTracks, qulonglong sourceId)
{
    for (const auto &removedTrackFileName : removedTracks) {
        d->mRemoveTracksMappingFromSource.bindValue(QStringLiteral(":fileName"), removedTrackFileName.toString());
        d->mRemoveTracksMappingFromSource.bindValue(QStringLiteral(":sourceId"), sourceId);

        auto result = d->mRemoveTracksMappingFromSource.exec();

        if (!result || !d->mRemoveTracksMappingFromSource.isActive()) {
            Q_EMIT databaseError();

            qDebug() << "DatabaseInterface::removeTracksList" << d->mRemoveTracksMappingFromSource.lastQuery();
            qDebug() << "DatabaseInterface::removeTracksList" << d->mRemoveTracksMappingFromSource.boundValues();
            qDebug() << "DatabaseInterface::removeTracksList" << d->mRemoveTracksMappingFromSource.lastError();

            continue;
        }

        d->mRemoveTracksMappingFromSource.finish();
    }

    internalRemoveTracksWithoutMapping();
}

void DatabaseInterface::internalRemoveTracksWithoutMapping()
{
    auto queryResult = d->mSelectTracksWithoutMappingQuery.exec();

    if (!queryResult || !d->mSelectTracksWithoutMappingQuery.isSelect() || !d->mSelectTracksWithoutMappingQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::insertArtist" << d->mSelectTracksWithoutMappingQuery.lastQuery();
        qDebug() << "DatabaseInterface::insertArtist" << d->mSelectTracksWithoutMappingQuery.boundValues();
        qDebug() << "DatabaseInterface::insertArtist" << d->mSelectTracksWithoutMappingQuery.lastError();

        d->mSelectTracksWithoutMappingQuery.finish();

        return;
    }

    QList<MusicAudioTrack> willRemoveTrack;

    while (d->mSelectTracksWithoutMappingQuery.next()) {
        const auto &currentRecord = d->mSelectTracksWithoutMappingQuery.record();

        willRemoveTrack.push_back(buildTrackFromDatabaseRecord(currentRecord));
    }

    d->mSelectTracksWithoutMappingQuery.finish();

    QSet<qulonglong> modifiedAlbums;

    for (const auto &oneRemovedTrack : willRemoveTrack) {
        removeTrackInDatabase(oneRemovedTrack.databaseId());

        Q_EMIT trackRemoved(oneRemovedTrack.databaseId());

        const auto &modifiedAlbumId = internalAlbumIdFromTitleAndArtist(oneRemovedTrack.albumName(), oneRemovedTrack.albumArtist());
        const auto &allTracksFromArtist = internalTracksFromAuthor(oneRemovedTrack.artist());
        const auto &allAlbumsFromArtist = internalAlbumIdsFromAuthor(oneRemovedTrack.artist());
        const auto &removedArtistId = internalArtistIdFromName(oneRemovedTrack.artist());
        const auto &removedArtist = internalArtistFromId(removedArtistId);

        if (updateTracksCount(modifiedAlbumId)) {
            modifiedAlbums.insert(modifiedAlbumId);
        }
        updateAlbumFromId(modifiedAlbumId, oneRemovedTrack.albumCover(), oneRemovedTrack);

        if (allTracksFromArtist.isEmpty() && allAlbumsFromArtist.isEmpty()) {
            removeArtistInDatabase(removedArtistId);
            Q_EMIT artistRemoved(removedArtist);
        }
    }

    for (auto modifiedAlbumId : modifiedAlbums) {
        auto modifiedAlbum = internalAlbumFromId(modifiedAlbumId);

        if (modifiedAlbum.isValid() && !modifiedAlbum.isEmpty()) {
            Q_EMIT albumModified(modifiedAlbum, modifiedAlbumId);
        } else {
            removeAlbumInDatabase(modifiedAlbum.databaseId());
            Q_EMIT albumRemoved(modifiedAlbum, modifiedAlbumId);

            const auto &allTracksFromArtist = internalTracksFromAuthor(modifiedAlbum.artist());
            const auto &allAlbumsFromArtist = internalAlbumIdsFromAuthor(modifiedAlbum.artist());
            const auto &removedArtistId = internalArtistIdFromName(modifiedAlbum.artist());
            const auto &removedArtist = internalArtistFromId(removedArtistId);

            if (allTracksFromArtist.isEmpty() && allAlbumsFromArtist.isEmpty()) {
                removeArtistInDatabase(removedArtistId);
                Q_EMIT artistRemoved(removedArtist);
            }
        }
    }
}

qulonglong DatabaseInterface::internalArtistIdFromName(const QString &name)
{
    auto result = qulonglong(0);

    if (name.isEmpty()) {
        return result;
    }

    d->mSelectArtistByNameQuery.bindValue(QStringLiteral(":name"), name);

    auto queryResult = d->mSelectArtistByNameQuery.exec();

    if (!queryResult || !d->mSelectArtistByNameQuery.isSelect() || !d->mSelectArtistByNameQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.lastQuery();
        qDebug() << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.boundValues();
        qDebug() << "DatabaseInterface::insertArtist" << d->mSelectArtistByNameQuery.lastError();

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
    d->mRemoveTrackArtistQuery.bindValue(QStringLiteral(":trackId"), trackId);

    auto result = d->mRemoveTrackArtistQuery.exec();

    if (!result || !d->mRemoveTrackArtistQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackArtistQuery.lastQuery();
        qDebug() << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackArtistQuery.boundValues();
        qDebug() << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackArtistQuery.lastError();
    }

    d->mRemoveTrackArtistQuery.finish();

    d->mRemoveTrackQuery.bindValue(QStringLiteral(":trackId"), trackId);

    result = d->mRemoveTrackQuery.exec();

    if (!result || !d->mRemoveTrackQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackQuery.lastQuery();
        qDebug() << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackQuery.boundValues();
        qDebug() << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackQuery.lastError();
    }

    d->mRemoveTrackQuery.finish();
}

void DatabaseInterface::removeAlbumInDatabase(qulonglong albumId)
{
    d->mRemoveAlbumArtistQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = d->mRemoveAlbumArtistQuery.exec();

    if (!result || !d->mRemoveAlbumArtistQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::removeAlbumInDatabase" << d->mRemoveAlbumArtistQuery.lastQuery();
        qDebug() << "DatabaseInterface::removeAlbumInDatabase" << d->mRemoveAlbumArtistQuery.boundValues();
        qDebug() << "DatabaseInterface::removeAlbumInDatabase" << d->mRemoveAlbumArtistQuery.lastError();
    }

    d->mRemoveAlbumArtistQuery.finish();

    d->mRemoveAlbumQuery.bindValue(QStringLiteral(":albumId"), albumId);

    result = d->mRemoveAlbumQuery.exec();

    if (!result || !d->mRemoveAlbumQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::removeAlbumInDatabase" << d->mRemoveAlbumQuery.lastQuery();
        qDebug() << "DatabaseInterface::removeAlbumInDatabase" << d->mRemoveAlbumQuery.boundValues();
        qDebug() << "DatabaseInterface::removeAlbumInDatabase" << d->mRemoveAlbumQuery.lastError();
    }

    d->mRemoveAlbumQuery.finish();
}

void DatabaseInterface::removeArtistInDatabase(qulonglong artistId)
{
    d->mRemoveArtistQuery.bindValue(QStringLiteral(":artistId"), artistId);

    auto result = d->mRemoveArtistQuery.exec();

    if (!result || !d->mRemoveArtistQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::removeArtistInDatabase" << d->mRemoveArtistQuery.lastQuery();
        qDebug() << "DatabaseInterface::removeArtistInDatabase" << d->mRemoveArtistQuery.boundValues();
        qDebug() << "DatabaseInterface::removeArtistInDatabase" << d->mRemoveArtistQuery.lastError();
    }

    d->mRemoveArtistQuery.finish();
}

void DatabaseInterface::reloadExistingDatabase()
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    d->mInitialUpdateTracksValidity.exec();
    qDebug() << "DatabaseInterface::reloadExistingDatabase";

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }

    const auto restoredArtists = allArtists();
    for (const auto &oneArtist : restoredArtists) {
        d->mArtistId = std::max(d->mArtistId, oneArtist.databaseId());
        Q_EMIT artistAdded(oneArtist);
    }
    ++d->mArtistId;

    const auto restoredAlbums = allAlbums();
    for (const auto &oneAlbum : restoredAlbums) {
        d->mAlbumId = std::max(d->mAlbumId, oneAlbum.databaseId());
        Q_EMIT albumAdded(oneAlbum);
    }
    ++d->mAlbumId;

    const auto restoredTracks = allTracks();
    Q_EMIT tracksAdded(restoredTracks);
    for (const auto &oneTrack : restoredTracks) {
        d->mTrackId = std::max(d->mTrackId, oneTrack.databaseId());
        Q_EMIT trackAdded(oneTrack.databaseId());
    }
    ++d->mTrackId;
}

qulonglong DatabaseInterface::insertMusicSource(const QString &name)
{
    qulonglong result = 0;

    d->mSelectMusicSource.bindValue(QStringLiteral(":name"), name);

    auto queryResult = d->mSelectMusicSource.exec();

    if (!queryResult || !d->mSelectMusicSource.isSelect() || !d->mSelectMusicSource.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::insertMusicSource" << d->mSelectMusicSource.lastQuery();
        qDebug() << "DatabaseInterface::insertMusicSource" << d->mSelectMusicSource.boundValues();
        qDebug() << "DatabaseInterface::insertMusicSource" << d->mSelectMusicSource.lastError();

        d->mSelectMusicSource.finish();

        return result;
    }

    if (d->mSelectMusicSource.next()) {
        result = d->mSelectMusicSource.record().value(0).toULongLong();

        d->mSelectMusicSource.finish();

        return result;
    }

    d->mSelectMusicSource.finish();

    d->mInsertMusicSource.bindValue(QStringLiteral(":discoverId"), d->mDiscoverId);
    d->mInsertMusicSource.bindValue(QStringLiteral(":name"), name);

    queryResult = d->mInsertMusicSource.exec();

    if (!queryResult || !d->mInsertMusicSource.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::insertMusicSource" << d->mInsertMusicSource.lastQuery();
        qDebug() << "DatabaseInterface::insertMusicSource" << d->mInsertMusicSource.boundValues();
        qDebug() << "DatabaseInterface::insertMusicSource" << d->mInsertMusicSource.lastError();

        d->mInsertMusicSource.finish();

        return d->mDiscoverId;
    }

    d->mInsertMusicSource.finish();

    ++d->mDiscoverId;

    return d->mDiscoverId - 1;
}

QList<MusicAudioTrack> DatabaseInterface::fetchTracks(qulonglong albumId)
{
    auto allTracks = QList<MusicAudioTrack>();

    d->mSelectTrackQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = d->mSelectTrackQuery.exec();

    if (!result || !d->mSelectTrackQuery.isSelect() || !d->mSelectTrackQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::fetchTracks" << d->mSelectTrackQuery.lastQuery();
        qDebug() << "DatabaseInterface::fetchTracks" << d->mSelectTrackQuery.boundValues();
        qDebug() << "DatabaseInterface::fetchTracks" << d->mSelectTrackQuery.lastError();
    }

    while (d->mSelectTrackQuery.next()) {
        const auto &currentRecord = d->mSelectTrackQuery.record();

        allTracks.push_back(buildTrackFromDatabaseRecord(currentRecord));
    }

    d->mSelectTrackQuery.finish();

    updateTracksCount(albumId);

    return allTracks;
}

bool DatabaseInterface::updateTracksCount(qulonglong albumId)
{
    bool isModified = false;

    d->mSelectAlbumTrackCountQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = d->mSelectAlbumTrackCountQuery.exec();

    if (!result || !d->mSelectAlbumTrackCountQuery.isSelect() || !d->mSelectAlbumTrackCountQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::updateTracksCount" << d->mSelectAlbumTrackCountQuery.lastQuery();
        qDebug() << "DatabaseInterface::updateTracksCount" << d->mSelectAlbumTrackCountQuery.boundValues();
        qDebug() << "DatabaseInterface::updateTracksCount" << d->mSelectAlbumTrackCountQuery.lastError();

        d->mSelectAlbumTrackCountQuery.finish();

        return isModified;
    }

    if (!d->mSelectAlbumTrackCountQuery.next()) {
        d->mSelectAlbumTrackCountQuery.finish();

        return isModified;
    }

    auto oldTracksCount = d->mSelectAlbumTrackCountQuery.record().value(0).toInt();

    d->mUpdateAlbumQuery.bindValue(QStringLiteral(":albumId"), albumId);

    result = d->mUpdateAlbumQuery.exec();

    if (!result || !d->mUpdateAlbumQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::updateTracksCount" << d->mUpdateAlbumQuery.lastQuery();
        qDebug() << "DatabaseInterface::updateTracksCount" << d->mUpdateAlbumQuery.boundValues();
        qDebug() << "DatabaseInterface::updateTracksCount" << d->mUpdateAlbumQuery.lastError();

        d->mUpdateAlbumQuery.finish();

        return isModified;
    }

    d->mUpdateAlbumQuery.finish();

    d->mSelectAlbumTrackCountQuery.bindValue(QStringLiteral(":albumId"), albumId);

    result = d->mSelectAlbumTrackCountQuery.exec();

    if (!result || !d->mSelectAlbumTrackCountQuery.isSelect() || !d->mSelectAlbumTrackCountQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::updateTracksCount" << d->mSelectAlbumTrackCountQuery.lastQuery();
        qDebug() << "DatabaseInterface::updateTracksCount" << d->mSelectAlbumTrackCountQuery.boundValues();
        qDebug() << "DatabaseInterface::updateTracksCount" << d->mSelectAlbumTrackCountQuery.lastError();

        d->mSelectAlbumTrackCountQuery.finish();

        return isModified;
    }

    if (!d->mSelectAlbumTrackCountQuery.next()) {
        d->mSelectAlbumTrackCountQuery.finish();

        return isModified;
    }

    auto newTracksCount = d->mSelectAlbumTrackCountQuery.record().value(0).toInt();

    isModified = (newTracksCount != oldTracksCount);

    return isModified;
}

MusicAlbum DatabaseInterface::internalAlbumFromId(qulonglong albumId)
{
    auto retrievedAlbum = MusicAlbum();

    d->mSelectAlbumQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = d->mSelectAlbumQuery.exec();

    if (!result || !d->mSelectAlbumQuery.isSelect() || !d->mSelectAlbumQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::internalAlbumFromId" << d->mSelectAlbumQuery.lastQuery();
        qDebug() << "DatabaseInterface::internalAlbumFromId" << d->mSelectAlbumQuery.boundValues();
        qDebug() << "DatabaseInterface::internalAlbumFromId" << d->mSelectAlbumQuery.lastError();

        d->mSelectAlbumQuery.finish();

        return retrievedAlbum;
    }

    if (!d->mSelectAlbumQuery.next()) {
        d->mSelectAlbumQuery.finish();

        return retrievedAlbum;
    }

    const auto &currentRecord = d->mSelectAlbumQuery.record();

    retrievedAlbum.setDatabaseId(currentRecord.value(0).toULongLong());
    retrievedAlbum.setTitle(currentRecord.value(1).toString());
    retrievedAlbum.setId(currentRecord.value(2).toString());
    retrievedAlbum.setArtist(currentRecord.value(3).toString());
    retrievedAlbum.setAlbumArtURI(currentRecord.value(4).toUrl());
    retrievedAlbum.setTracksCount(currentRecord.value(5).toInt());
    retrievedAlbum.setIsSingleDiscAlbum(currentRecord.value(6).toBool());
    retrievedAlbum.setTracks(fetchTracks(albumId));
    retrievedAlbum.setValid(true);

    d->mSelectAlbumQuery.finish();

    return retrievedAlbum;
}

MusicAlbum DatabaseInterface::internalAlbumFromTitleAndArtist(const QString &title, const QString &artist)
{
    auto result = MusicAlbum();

    auto albumId = internalAlbumIdFromTitleAndArtist(title, artist);

    if (albumId == 0) {
        return result;
    }

    result = internalAlbumFromId(albumId);

    return result;
}

qulonglong DatabaseInterface::internalAlbumIdFromTitleAndArtist(const QString &title, const QString &artist)
{
    auto result = qulonglong(0);

    d->mSelectAlbumIdFromTitleQuery.bindValue(QStringLiteral(":title"), title);
    d->mSelectAlbumIdFromTitleQuery.bindValue(QStringLiteral(":artistName"), artist);

    auto queryResult = d->mSelectAlbumIdFromTitleQuery.exec();

    if (!queryResult || !d->mSelectAlbumIdFromTitleQuery.isSelect() || !d->mSelectAlbumIdFromTitleQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleQuery.lastQuery();
        qDebug() << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleQuery.boundValues();
        qDebug() << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleQuery.lastError();

        d->mSelectAlbumIdFromTitleQuery.finish();

        return result;
    }

    if (d->mSelectAlbumIdFromTitleQuery.next()) {
        result = d->mSelectAlbumIdFromTitleQuery.record().value(0).toULongLong();
    }

    d->mSelectAlbumIdFromTitleQuery.finish();

    if (result == 0) {
        d->mSelectAlbumIdFromTitleWithoutArtistQuery.bindValue(QStringLiteral(":title"), title);

        auto queryResult = d->mSelectAlbumIdFromTitleWithoutArtistQuery.exec();

        if (!queryResult || !d->mSelectAlbumIdFromTitleWithoutArtistQuery.isSelect() || !d->mSelectAlbumIdFromTitleWithoutArtistQuery.isActive()) {
            Q_EMIT databaseError();

            qDebug() << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastQuery();
            qDebug() << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.boundValues();
            qDebug() << "DatabaseInterface::internalAlbumIdFromTitleAndArtist" << d->mSelectAlbumIdFromTitleWithoutArtistQuery.lastError();

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

MusicAudioTrack DatabaseInterface::internalTrackFromDatabaseId(qulonglong id)
{
    auto result = MusicAudioTrack();

    if (!d || !d->mTracksDatabase.isValid() || !d->mInitFinished) {
        return result;
    }

    d->mSelectTrackFromIdQuery.bindValue(QStringLiteral(":trackId"), id);

    auto queryResult = d->mSelectTrackFromIdQuery.exec();

    if (!queryResult || !d->mSelectTrackFromIdQuery.isSelect() || !d->mSelectTrackFromIdQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::internalTrackFromDatabaseId" << d->mSelectAlbumQuery.lastQuery();
        qDebug() << "DatabaseInterface::internalTrackFromDatabaseId" << d->mSelectAlbumQuery.boundValues();
        qDebug() << "DatabaseInterface::internalTrackFromDatabaseId" << d->mSelectAlbumQuery.lastError();

        d->mSelectTrackFromIdQuery.finish();

        return result;
    }

    if (!d->mSelectTrackFromIdQuery.next()) {
        d->mSelectTrackFromIdQuery.finish();

        return result;
    }

    const auto &currentRecord = d->mSelectTrackFromIdQuery.record();

    result = buildTrackFromDatabaseRecord(currentRecord);

    d->mSelectTrackFromIdQuery.finish();

    return result;
}

qulonglong DatabaseInterface::internalTrackIdFromTitleAlbumTracDiscNumber(const QString &title, const QString &artist, const QString &album,
                                                                          int trackNumber, int discNumber)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":title"), title);
    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":artist"), artist);
    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":album"), album);
    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":trackNumber"), trackNumber);
    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":discNumber"), discNumber);

    auto queryResult = d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.exec();

    if (!queryResult || !d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.isSelect() || !d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.lastQuery();
        qDebug() << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.boundValues();
        qDebug() << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.lastError();

        d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.finish();

        return result;
    }

    if (d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.next()) {
        result = d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.record().value(0).toInt();
    }

    d->mSelectTrackIdFromTitleArtistAlbumTrackDiscNumberQuery.finish();

    return result;
}

qulonglong DatabaseInterface::getDuplicateTrackIdFromTitleAlbumTracDiscNumber(const QString &title, const QString &album,
                                                                              const QString &albumArtist, int trackNumber, int discNumber)
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":title"), title);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":album"), album);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":albumArtist"), albumArtist);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":trackNumber"), trackNumber);
    d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.bindValue(QStringLiteral(":discNumber"), discNumber);

    auto queryResult = d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.exec();

    if (!queryResult || !d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.isSelect() || !d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.lastQuery();
        qDebug() << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.boundValues();
        qDebug() << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.lastError();

        d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.finish();

        return result;
    }

    if (d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.next()) {
        result = d->mSelectTrackIdFromTitleAlbumTrackDiscNumberQuery.record().value(0).toInt();
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

    d->mSelectTracksMapping.bindValue(QStringLiteral(":fileName"), fileName);

    auto queryResult = d->mSelectTracksMapping.exec();

    if (!queryResult || !d->mSelectTracksMapping.isSelect() || !d->mSelectTracksMapping.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMapping.lastQuery();
        qDebug() << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMapping.boundValues();
        qDebug() << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMapping.lastError();

        d->mSelectTracksMapping.finish();

        return result;
    }

    if (d->mSelectTracksMapping.next()) {
        const auto &currentRecordValue = d->mSelectTracksMapping.record().value(0);
        if (currentRecordValue.isValid()) {
            result = currentRecordValue.toInt();
        }
    }

    d->mSelectTracksMapping.finish();

    return result;
}

QList<MusicAudioTrack> DatabaseInterface::internalTracksFromAuthor(const QString &artistName)
{
    auto allTracks = QList<MusicAudioTrack>();

    d->mSelectTracksFromArtist.bindValue(QStringLiteral(":artistName"), artistName);

    auto result = d->mSelectTracksFromArtist.exec();

    if (!result || !d->mSelectTracksFromArtist.isSelect() || !d->mSelectTracksFromArtist.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::tracksFromAuthor" << d->mSelectTracksFromArtist.lastQuery();
        qDebug() << "DatabaseInterface::tracksFromAuthor" << d->mSelectTracksFromArtist.boundValues();
        qDebug() << "DatabaseInterface::tracksFromAuthor" << d->mSelectTracksFromArtist.lastError();

        return allTracks;
    }

    while (d->mSelectTracksFromArtist.next()) {
        const auto &currentRecord = d->mSelectTracksFromArtist.record();

        allTracks.push_back(buildTrackFromDatabaseRecord(currentRecord));
    }

    d->mSelectTracksFromArtist.finish();

    return allTracks;
}

QList<qulonglong> DatabaseInterface::internalAlbumIdsFromAuthor(const QString &artistName)
{
    auto allAlbumIds = QList<qulonglong>();

    d->mSelectAlbumIdsFromArtist.bindValue(QStringLiteral(":artistName"), artistName);

    auto result = d->mSelectAlbumIdsFromArtist.exec();

    if (!result || !d->mSelectAlbumIdsFromArtist.isSelect() || !d->mSelectAlbumIdsFromArtist.isActive()) {
        Q_EMIT databaseError();

        qDebug() << "DatabaseInterface::tracksFromAuthor" << d->mSelectAlbumIdsFromArtist.lastQuery();
        qDebug() << "DatabaseInterface::tracksFromAuthor" << d->mSelectAlbumIdsFromArtist.boundValues();
        qDebug() << "DatabaseInterface::tracksFromAuthor" << d->mSelectAlbumIdsFromArtist.lastError();

        return allAlbumIds;
    }

    while (d->mSelectAlbumIdsFromArtist.next()) {
        const auto &currentRecord = d->mSelectAlbumIdsFromArtist.record();

        allAlbumIds.push_back(currentRecord.value(0).toULongLong());
    }

    d->mSelectAlbumIdsFromArtist.finish();

    return allAlbumIds;
}


#include "moc_databaseinterface.cpp"
