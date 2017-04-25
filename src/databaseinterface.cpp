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
          mSelectTrackIdFromTitleAlbumArtistQuery(mTracksDatabase), mSelectAllAlbumsQuery(mTracksDatabase),
          mSelectAllAlbumsFromArtistQuery(mTracksDatabase), mSelectAllArtistsQuery(mTracksDatabase),
          mInsertArtistsQuery(mTracksDatabase), mSelectArtistByNameQuery(mTracksDatabase),
          mSelectArtistQuery(mTracksDatabase), mSelectTrackFromFilePathQuery(mTracksDatabase),
          mRemoveTrackQuery(mTracksDatabase), mRemoveAlbumQuery(mTracksDatabase),
          mRemoveArtistQuery(mTracksDatabase), mSelectAllTracksQuery(mTracksDatabase),
          mInsertTrackMapping(mTracksDatabase), mSelectAllTracksFromSourceQuery(mTracksDatabase),
          mInsertMusicSource(mTracksDatabase), mSelectMusicSource(mTracksDatabase),
          mUpdateIsSingleDiscAlbumFromIdQuery(mTracksDatabase), mSelectAllInvalidTracksFromSourceQuery(mTracksDatabase),
          mInitialUpdateTracksValidity(mTracksDatabase), mUpdateTrackMapping(mTracksDatabase),
          mSelectTracksMapping(mTracksDatabase), mSelectTracksMappingPriority(mTracksDatabase)
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

    QSqlQuery mSelectTrackIdFromTitleAlbumArtistQuery;

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

    qulonglong mAlbumId = 1;

    qulonglong mArtistId = 1;

    qulonglong mTrackId = 1;

    qulonglong mDiscoverId = 1;

    bool mInitFinished = false;

};

DatabaseInterface::DatabaseInterface(QObject *parent) : QObject(parent), d(nullptr)
{
}

DatabaseInterface::~DatabaseInterface()
{
    if (d) {
        d->mTracksDatabase.close();
    }

    delete d;
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

    d = new DatabaseInterfacePrivate(tracksDatabase);

    initDatabase();
    initRequest();

    if (!databaseFileName.isEmpty()) {
        reloadExistingDatabase();
    }
}

MusicAlbum DatabaseInterface::albumFromTitle(const QString &title)
{
    auto result = MusicAlbum();

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalAlbumFromTitle(title);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

QList<MusicAudioTrack> DatabaseInterface::allTracks() const
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
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllTracksQuery.lastQuery();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllTracksQuery.boundValues();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllTracksQuery.lastError();

        return result;
    }

    while(d->mSelectAllTracksQuery.next()) {
        auto newTrack = MusicAudioTrack();

        const auto &currentRecord = d->mSelectAllTracksQuery.record();

        newTrack.setDatabaseId(currentRecord.value(0).toULongLong());
        newTrack.setTitle(currentRecord.value(1).toString());
        newTrack.setParentId(currentRecord.value(2).toString());
        newTrack.setArtist(currentRecord.value(3).toString());
        newTrack.setAlbumArtist(currentRecord.value(4).toString());
        newTrack.setResourceURI(currentRecord.value(5).toUrl());
        newTrack.setTrackNumber(currentRecord.value(6).toInt());
        newTrack.setDiscNumber(currentRecord.value(7).toInt());
        newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(currentRecord.value(8).toInt()));
        newTrack.setRating(currentRecord.value(9).toInt());
        newTrack.setValid(true);

        result.push_back(newTrack);
    }

    d->mSelectAllTracksQuery.finish();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

QList<MusicAudioTrack> DatabaseInterface::allTracksFromSource(const QString &musicSource) const
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
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllTracksFromSourceQuery.lastQuery();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllTracksFromSourceQuery.boundValues();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllTracksFromSourceQuery.lastError();

        return result;
    }

    while(d->mSelectAllTracksFromSourceQuery.next()) {
        auto newTrack = MusicAudioTrack();

        const auto &currentRecord = d->mSelectAllTracksFromSourceQuery.record();

        newTrack.setDatabaseId(currentRecord.value(0).toULongLong());
        newTrack.setTitle(currentRecord.value(1).toString());
        newTrack.setParentId(currentRecord.value(2).toString());
        newTrack.setArtist(currentRecord.value(3).toString());
        newTrack.setAlbumArtist(currentRecord.value(4).toString());
        newTrack.setResourceURI(currentRecord.value(5).toUrl());
        newTrack.setTrackNumber(currentRecord.value(6).toInt());
        newTrack.setDiscNumber(currentRecord.value(7).toInt());
        newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(currentRecord.value(8).toInt()));
        newTrack.setAlbumName(currentRecord.value(9).toString());
        newTrack.setRating(currentRecord.value(10).toInt());
        newTrack.setValid(true);

        result.push_back(newTrack);
    }

    d->mSelectAllTracksFromSourceQuery.finish();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

QList<MusicAudioTrack> DatabaseInterface::allInvalidTracksFromSource(const QString &musicSource) const
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
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllInvalidTracksFromSourceQuery.lastQuery();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllInvalidTracksFromSourceQuery.boundValues();
        qDebug() << "DatabaseInterface::allAlbums" << d->mSelectAllInvalidTracksFromSourceQuery.lastError();

        return result;
    }

    while(d->mSelectAllInvalidTracksFromSourceQuery.next()) {
        auto newTrack = MusicAudioTrack();

        const auto &currentRecord = d->mSelectAllInvalidTracksFromSourceQuery.record();

        newTrack.setDatabaseId(currentRecord.value(0).toULongLong());
        newTrack.setTitle(currentRecord.value(1).toString());
        newTrack.setParentId(currentRecord.value(2).toString());
        newTrack.setArtist(currentRecord.value(3).toString());
        newTrack.setAlbumArtist(currentRecord.value(4).toString());
        newTrack.setResourceURI(currentRecord.value(5).toUrl());
        newTrack.setTrackNumber(currentRecord.value(6).toInt());
        newTrack.setDiscNumber(currentRecord.value(7).toInt());
        newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(currentRecord.value(8).toInt()));
        newTrack.setAlbumName(currentRecord.value(9).toString());
        newTrack.setRating(currentRecord.value(10).toInt());
        newTrack.setValid(true);

        result.push_back(newTrack);
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

QList<MusicArtist> DatabaseInterface::allArtists() const
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

QList<MusicAudioTrack> DatabaseInterface::tracksFromAuthor(const QString &artistName) const
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

MusicArtist DatabaseInterface::internalArtistFromId(qulonglong artistId) const
{
    auto result = MusicArtist();

    if (!d || !d->mTracksDatabase.isValid() || !d->mInitFinished) {
        return result;
    }

    d->mSelectArtistQuery.bindValue(QStringLiteral(":artistId"), artistId);

    auto queryResult = d->mSelectArtistQuery.exec();

    if (!queryResult || !d->mSelectArtistQuery.isSelect() || !d->mSelectArtistQuery.isActive()) {
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

qulonglong DatabaseInterface::trackIdFromTitleAlbumArtist(const QString &title, const QString &album, const QString &artist) const
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    result = internalTrackIdFromTitleAlbumArtist(title, album, artist);

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

void DatabaseInterface::insertTracksList(const QList<MusicAudioTrack> &tracks, const QHash<QString, QUrl> &covers, const QString &musicSource)
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    QSet<qulonglong> modifiedAlbumIds;

    for(const auto &oneTrack : tracks) {
        d->mSelectTracksMapping.bindValue(QStringLiteral(":fileName"), oneTrack.resourceURI());

        auto result = d->mSelectTracksMapping.exec();

        if (!result || !d->mSelectTracksMapping.isSelect() || !d->mSelectTracksMapping.isActive()) {
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

        internalInsertTrack(oneTrack, covers, 0, modifiedAlbumIds);
    }

    const auto &constModifiedAlbumIds = modifiedAlbumIds;
    for (auto albumId : constModifiedAlbumIds) {
        Q_EMIT albumModified(internalAlbumFromId(albumId));
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

    QList<MusicAudioTrack> willRemoveTrack;

    for (const auto &removedTrackFileName : removedTracks) {
        d->mSelectTrackFromFilePathQuery.bindValue(QStringLiteral(":filePath"), removedTrackFileName.toString());

        auto result = d->mSelectTrackFromFilePathQuery.exec();

        if (!result || !d->mSelectTrackFromFilePathQuery.isSelect() || !d->mSelectTrackFromFilePathQuery.isActive()) {
            qDebug() << "DatabaseInterface::removeTracksList" << d->mSelectTrackFromFilePathQuery.lastQuery();
            qDebug() << "DatabaseInterface::removeTracksList" << d->mSelectTrackFromFilePathQuery.boundValues();
            qDebug() << "DatabaseInterface::removeTracksList" << d->mSelectTrackFromFilePathQuery.lastError();

            continue;
        }

        while (d->mSelectTrackFromFilePathQuery.next()) {
            MusicAudioTrack removedTrack;

            const auto &currentRecord = d->mSelectTrackFromFilePathQuery.record();

            removedTrack.setDatabaseId(currentRecord.value(0).toULongLong());
            removedTrack.setTitle(currentRecord.value(1).toString());
            removedTrack.setParentId(currentRecord.value(2).toString());
            removedTrack.setArtist(currentRecord.value(3).toString());
            removedTrack.setAlbumArtist(currentRecord.value(4).toString());
            removedTrack.setResourceURI(currentRecord.value(5).toUrl());
            removedTrack.setTrackNumber(currentRecord.value(6).toInt());
            removedTrack.setDiscNumber(currentRecord.value(7).toInt());
            removedTrack.setDuration(QTime::fromMSecsSinceStartOfDay(currentRecord.value(8).toInt()));
            removedTrack.setAlbumName(currentRecord.value(9).toString());
            removedTrack.setRating(currentRecord.value(10).toInt());
            removedTrack.setValid(true);

            willRemoveTrack.push_back(removedTrack);
        }

        d->mSelectTrackFromFilePathQuery.finish();
    }

    QSet<qulonglong> modifiedAlbums;

    for (const auto &oneRemovedTrack : willRemoveTrack) {
        removeTrackInDatabase(oneRemovedTrack.databaseId());
        Q_EMIT trackRemoved(oneRemovedTrack.databaseId());

        const auto &modifiedAlbumId = internalAlbumIdFromTitle(oneRemovedTrack.albumName());
        const auto &allArtistTracks = internalTracksFromAuthor(oneRemovedTrack.artist());
        const auto &removedArtistId = internalArtistIdFromName(oneRemovedTrack.artist());
        const auto &removedArtist = internalArtistFromId(removedArtistId);

        if (updateTracksCount(modifiedAlbumId)) {
            modifiedAlbums.insert(modifiedAlbumId);
        }
        updateIsSingleDiscAlbumFromId(modifiedAlbumId);

        if (allArtistTracks.isEmpty()) {
            removeArtistInDatabase(removedArtistId);
            Q_EMIT artistRemoved(removedArtist);
        }
    }

    for (auto modifiedAlbumId : modifiedAlbums) {
        auto modifiedAlbum = internalAlbumFromId(modifiedAlbumId);

        if (modifiedAlbum.isValid() && !modifiedAlbum.isEmpty()) {
            Q_EMIT albumModified(modifiedAlbum);
        } else {
            removeAlbumInDatabase(modifiedAlbum.databaseId());
            Q_EMIT albumRemoved(modifiedAlbum);
        }
    }

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }
}

void DatabaseInterface::modifyTracksList(const QList<MusicAudioTrack> &modifiedTracks, const QHash<QString, QUrl> &covers)
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    for (const auto &oneModifiedTrack : modifiedTracks) {
        if (oneModifiedTrack.albumArtist().isEmpty()) {
            continue;
        }

        auto originTrackId = internalTrackIdFromFileName(oneModifiedTrack.resourceURI());

        auto originTrack = MusicAudioTrack();

        if (originTrackId != 0) {
            originTrack = internalTrackFromDatabaseId(originTrackId);
        }

        if (originTrack.isValid() && originTrack == oneModifiedTrack) {
            continue;
        }

        auto albumId = insertAlbum(oneModifiedTrack.albumName(), oneModifiedTrack.albumArtist(), covers[oneModifiedTrack.albumName()], 0, true);

        auto otherTrackId = internalTrackIdFromTitleAlbumArtist(oneModifiedTrack.title(), oneModifiedTrack.albumName(), oneModifiedTrack.artist());

        if (originTrack.isValid() || otherTrackId != 0) {
            if (otherTrackId != 0) {
                originTrackId = otherTrackId;
            }

            const auto oldTrack = internalTrackFromDatabaseId(originTrackId);
            if (oldTrack == oneModifiedTrack) {
                updateTrackOrigin(originTrackId, oneModifiedTrack.resourceURI());
                continue;
            }

            removeTrackInDatabase(originTrackId);
        } else {
            originTrackId = d->mTrackId;
        }

        d->mInsertTrackQuery.bindValue(QStringLiteral(":trackId"), originTrackId);
        d->mInsertTrackQuery.bindValue(QStringLiteral(":title"), oneModifiedTrack.title());
        d->mInsertTrackQuery.bindValue(QStringLiteral(":album"), albumId);
        d->mInsertTrackQuery.bindValue(QStringLiteral(":artistId"), insertArtist(oneModifiedTrack.artist()));
        d->mInsertTrackQuery.bindValue(QStringLiteral(":trackNumber"), oneModifiedTrack.trackNumber());
        d->mInsertTrackQuery.bindValue(QStringLiteral(":discNumber"), oneModifiedTrack.discNumber());
        d->mInsertTrackQuery.bindValue(QStringLiteral(":trackDuration"), QVariant::fromValue<qlonglong>(oneModifiedTrack.duration().msecsSinceStartOfDay()));
        d->mInsertTrackQuery.bindValue(QStringLiteral(":trackRating"), oneModifiedTrack.rating());

        auto result = d->mInsertTrackQuery.exec();

        if (result && d->mInsertTrackQuery.isActive()) {
            d->mInsertTrackQuery.finish();

            if (!originTrack.isValid()) {
                ++d->mTrackId;
            }

            updateTrackOrigin(originTrackId, oneModifiedTrack.resourceURI());

            if (originTrack.isValid() || otherTrackId != 0) {
                Q_EMIT trackModified(originTrackId);
                Q_EMIT albumModified(internalAlbumFromId(albumId));
            } else {
                Q_EMIT trackAdded(originTrackId);
            }

            updateIsSingleDiscAlbumFromId(albumId);
            if (updateTracksCount(albumId)) {
                Q_EMIT albumModified(internalAlbumFromId(albumId));
            }
        } else {
            d->mInsertTrackQuery.finish();

            qDebug() << "DatabaseInterface::modifyTracksList" << d->mInsertTrackQuery.lastQuery();
            qDebug() << "DatabaseInterface::modifyTracksList" << d->mInsertTrackQuery.boundValues();
            qDebug() << "DatabaseInterface::modifyTracksList" << d->mInsertTrackQuery.lastError();

            continue;
        }
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

    const auto &listTables = d->mTracksDatabase.tables();

    if (!listTables.contains(QStringLiteral("DiscoverSource"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DiscoverSource` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(55) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError() << createSchemaQuery.lastError().nativeErrorCode();
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
                                                                   "`ArtistID` INTEGER NOT NULL, "
                                                                   "`CoverFileName` VARCHAR(255) NOT NULL, "
                                                                   "`TracksCount` INTEGER NOT NULL, "
                                                                   "`IsSingleDiscAlbum` BOOLEAN NOT NULL, "
                                                                   "`AlbumInternalID` VARCHAR(55), "
                                                                   "UNIQUE (`Title`, `ArtistID`), "
                                                                   "CONSTRAINT fk_albums_artist FOREIGN KEY (`ArtistID`) REFERENCES `Artists`(`ID`))"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();
        }
    }

    if (!listTables.contains(QStringLiteral("Tracks"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Tracks` ("
                                                                   "`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Title` VARCHAR(85) NOT NULL, "
                                                                   "`AlbumID` INTEGER NOT NULL, "
                                                                   "`ArtistID` INTEGER NOT NULL, "
                                                                   "`TrackNumber` INTEGER NOT NULL, "
                                                                   "`DiscNumber` INTEGER, "
                                                                   "`Duration` INTEGER NOT NULL, "
                                                                   "`Rating` INTEGER NOT NULL DEFAULT 0, "
                                                                   "UNIQUE (`Title`, `AlbumID`, `ArtistID`), "
                                                                   "CONSTRAINT fk_tracks_album FOREIGN KEY (`AlbumID`) REFERENCES `Albums`(`ID`), "
                                                                   "CONSTRAINT fk_tracks_artist FOREIGN KEY (`ArtistID`) REFERENCES `Artists`(`ID`))"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();
        }
    } else {
        auto listColumns = d->mTracksDatabase.record(QStringLiteral("Tracks"));

        if (!listColumns.contains(QStringLiteral("Rating"))) {
            QSqlQuery alterSchemaQuery(d->mTracksDatabase);

            const auto &result = alterSchemaQuery.exec(QStringLiteral("ALTER TABLE `Tracks` "
                                                                       "ADD COLUMN RATING INTEGER NOT NULL DEFAULT 0"));

            if (!result) {
                qDebug() << "DatabaseInterface::initDatabase" << alterSchemaQuery.lastError();
            }
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
            qDebug() << "DatabaseInterface::initDatabase" << createTrackIndex.lastError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`AlbumsArtistIndex` ON `Albums` "
                                                                  "(`ArtistID`)"));

        if (!result) {
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
            qDebug() << "DatabaseInterface::initDatabase" << createTrackIndex.lastError();
        }
    }

    {
        QSqlQuery createTrackIndex(d->mTracksDatabase);

        const auto &result = createTrackIndex.exec(QStringLiteral("CREATE INDEX "
                                                                  "IF NOT EXISTS "
                                                                  "`TracksArtistIDAlbumIDFileNameIndex` ON `Tracks` "
                                                                  "(`ArtistID`, `AlbumID`)"));

        if (!result) {
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
                                                   "FROM `Albums` album, `Artists` artist "
                                                   "WHERE "
                                                   "album.`ID` = :albumId AND "
                                                   "artist.`ID` = album.`ArtistID`");

        auto result = d->mSelectAlbumQuery.prepare(selectAlbumQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumQuery.lastError();
        }
    }

    {
        auto selectAllAlbumsText = QStringLiteral("SELECT album.`ID`, "
                                                  "album.`Title`, "
                                                  "album.`AlbumInternalID`, "
                                                  "artist.`Name`, "
                                                  "album.`CoverFileName`, "
                                                  "album.`TracksCount`, "
                                                  "album.`IsSingleDiscAlbum` "
                                                  "FROM `Albums` album, `Artists` artist "
                                                  "WHERE "
                                                  "artist.`ID` = album.`ArtistID` "
                                                  "ORDER BY album.`Title`");

        auto result = d->mSelectAllAlbumsQuery.prepare(selectAllAlbumsText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << selectAllAlbumsText << d->mSelectAllAlbumsQuery.lastError();
            qDebug() << d->mTracksDatabase.lastError();
        }
    }

    {
        auto selectAllArtistsWithFilterText = QStringLiteral("SELECT `ID`, "
                                                            "`Name` "
                                                            "FROM `Artists`");

        auto result = d->mSelectAllArtistsQuery.prepare(selectAllArtistsWithFilterText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << selectAllArtistsWithFilterText << d->mSelectAllArtistsQuery.lastError();
            qDebug() << d->mTracksDatabase.lastError();
        }
    }

    {
        auto selectAllTracksText = QStringLiteral("SELECT tracks.`ID`, "
                                                  "tracks.`Title`, "
                                                  "tracks.`AlbumID`, "
                                                  "artist.`Name`, "
                                                  "artistAlbum.`Name`, "
                                                  "tracksMapping.`FileName`, "
                                                  "tracks.`TrackNumber`, "
                                                  "tracks.`DiscNumber`, "
                                                  "tracks.`Duration`, "
                                                  "tracks.`Rating` "
                                                  "FROM `Tracks` tracks, `Artists` artist, `Artists` artistAlbum, `Albums` album, `TracksMapping` tracksMapping "
                                                  "WHERE "
                                                  "artist.`ID` = tracks.`ArtistID` AND "
                                                  "tracks.`AlbumID` = album.`ID` AND "
                                                  "artistAlbum.`ID` = album.`ArtistID` AND "
                                                  "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                  "tracksMapping.`Priority` = 1");

        auto result = d->mSelectAllTracksQuery.prepare(selectAllTracksText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << selectAllTracksText << d->mSelectAllTracksQuery.lastError();
            qDebug() << d->mTracksDatabase.lastError();
        }
    }

    {
        auto selectAllInvalidTracksFromSourceQueryText = QStringLiteral("SELECT tracks.`ID`, "
                                                                        "tracks.`Title`, "
                                                                        "tracks.`AlbumID`, "
                                                                        "artist.`Name`, "
                                                                        "artistAlbum.`Name`, "
                                                                        "tracksMapping.`FileName`, "
                                                                        "tracks.`TrackNumber`, "
                                                                        "tracks.`DiscNumber`, "
                                                                        "tracks.`Duration`, "
                                                                        "album.`Title`, "
                                                                        "tracks.`Rating` "
                                                                        "FROM `Tracks` tracks, `Artists` artist, `Artists` artistAlbum, "
                                                                        "`Albums` album , `TracksMapping` tracksMapping, `DiscoverSource` source "
                                                                        "WHERE "
                                                                        "artist.`ID` = tracks.`ArtistID` AND "
                                                                        "tracks.`AlbumID` = album.`ID` AND "
                                                                        "artistAlbum.`ID` = album.`ArtistID` AND "
                                                                        "source.`Name` = :source AND "
                                                                        "source.`ID` = tracksMapping.`DiscoverID` AND "
                                                                        "tracksMapping.`TrackValid` = 0 AND "
                                                                        "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                                        "tracksMapping.`Priority` = 1");

        auto result = d->mSelectAllInvalidTracksFromSourceQuery.prepare(selectAllInvalidTracksFromSourceQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << selectAllInvalidTracksFromSourceQueryText << d->mSelectAllInvalidTracksFromSourceQuery.lastError();
            qDebug() << d->mTracksDatabase.lastError();
        }
    }

    {
        auto selectAllTracksFromSourceQueryText = QStringLiteral("SELECT tracks.`ID`, "
                                                                 "tracks.`Title`, "
                                                                 "tracks.`AlbumID`, "
                                                                 "artist.`Name`, "
                                                                 "artistAlbum.`Name`, "
                                                                 "tracksMapping.`FileName`, "
                                                                 "tracks.`TrackNumber`, "
                                                                 "tracks.`DiscNumber`, "
                                                                 "tracks.`Duration`, "
                                                                 "album.`Title`, "
                                                                 "tracks.`Rating` "
                                                                 "FROM `Tracks` tracks, `Artists` artist, `Artists` artistAlbum, "
                                                                 "`Albums` album , `TracksMapping` tracksMapping, `DiscoverSource` source "
                                                                 "WHERE "
                                                                 "artist.`ID` = tracks.`ArtistID` AND "
                                                                 "tracks.`AlbumID` = album.`ID` AND "
                                                                 "artistAlbum.`ID` = album.`ArtistID` AND "
                                                                 "source.`Name` = :source AND "
                                                                 "source.`ID` = tracksMapping.`DiscoverID` AND "
                                                                 "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                                 "tracksMapping.`Priority` = 1");

        auto result = d->mSelectAllTracksFromSourceQuery.prepare(selectAllTracksFromSourceQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << selectAllTracksFromSourceQueryText << d->mSelectAllTracksFromSourceQuery.lastError();
            qDebug() << d->mTracksDatabase.lastError();
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
            qDebug() << "DatabaseInterface::initRequest" << selectArtistByNameText << d->mSelectArtistByNameQuery.lastError();
            qDebug() << d->mTracksDatabase.lastError();
        }
    }

    {
        auto insertArtistsText = QStringLiteral("INSERT INTO `Artists` (`ID`, `Name`) "
                                                             "VALUES (:artistId, :name)");

        auto result = d->mInsertArtistsQuery.prepare(insertArtistsText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << insertArtistsText << d->mInsertArtistsQuery.lastError();
            qDebug() << d->mTracksDatabase.lastError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.`ID`, "
                                                   "tracks.`Title`, "
                                                   "tracks.`AlbumID`, "
                                                   "artist.`Name`, "
                                                   "album.`Title`, "
                                                   "artistAlbum.`Name`, "
                                                   "tracksMapping.`FileName`, "
                                                   "tracks.`TrackNumber`, "
                                                   "tracks.`DiscNumber`, "
                                                   "tracks.`Duration`, "
                                                   "tracks.`Rating`, "
                                                   "album.`CoverFileName` "
                                                   "FROM `Tracks` tracks, `Artists` artist, `Artists` artistAlbum, `Albums` album, `TracksMapping` tracksMapping "
                                                   "WHERE "
                                                   "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                   "tracks.`AlbumID` = :albumId AND "
                                                   "artist.`ID` = tracks.`ArtistID` AND "
                                                   "album.`ID` = :albumId AND "
                                                   "artistAlbum.`ID` = album.`ArtistID` AND "
                                                   "tracksMapping.`Priority` = 1 "
                                                   "ORDER BY tracks.`DiscNumber` ASC, "
                                                   "tracks.`TrackNumber` ASC");

        auto result = d->mSelectTrackQuery.prepare(selectTrackQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackQuery.lastError();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackQuery.lastQuery();
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
                                                         "tracks.`Rating`, "
                                                         "album.`CoverFileName` "
                                                         "FROM `Tracks` tracks, `Artists` artist, `Artists` artistAlbum, `Albums` album, `TracksMapping` tracksMapping "
                                                         "WHERE "
                                                         "tracks.`ID` = :trackId AND "
                                                         "artist.`ID` = tracks.`ArtistID` AND "
                                                         "artistAlbum.`ID` = album.`ArtistID` AND "
                                                         "tracks.`AlbumID` = album.`ID` AND "
                                                         "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                         "tracksMapping.`Priority` = 1");

        auto result = d->mSelectTrackFromIdQuery.prepare(selectTrackFromIdQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackFromIdQuery.lastError();
        }
    }
    {
        auto selectCountAlbumsQueryText = QStringLiteral("SELECT count(*) "
                                                         "FROM `Albums` album, `Artists` artist "
                                                         "WHERE artist.`Name` = :artistName AND "
                                                         "artist.`ID` = album.`ArtistID`");

        const auto result = d->mSelectCountAlbumsForArtistQuery.prepare(selectCountAlbumsQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectCountAlbumsForArtistQuery.lastError();
        }
    }
    {
        auto selectAlbumIdFromTitleQueryText = QStringLiteral("SELECT `ID` FROM `Albums` "
                                                              "WHERE "
                                                              "`Title` = :title");

        auto result = d->mSelectAlbumIdFromTitleQuery.prepare(selectAlbumIdFromTitleQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectAlbumIdFromTitleQuery.lastError();
        }
    }
    {
        auto insertAlbumQueryText = QStringLiteral("INSERT INTO Albums (`ID`, `Title`, `ArtistID`, `CoverFileName`, `TracksCount`, `IsSingleDiscAlbum`) "
                                                   "VALUES (:albumId, :title, :artistId, :coverFileName, :tracksCount, :isSingleDiscAlbum)");

        auto result = d->mInsertAlbumQuery.prepare(insertAlbumQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertAlbumQuery.lastError();
        }
    }

    {
        auto insertTrackMappingQueryText = QStringLiteral("INSERT INTO `TracksMapping` (`FileName`, `DiscoverID`, `Priority`, `TrackValid`) "
                                                   "VALUES (:fileName, :discoverId, :priority, 1)");

        auto result = d->mInsertTrackMapping.prepare(insertTrackMappingQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertTrackMapping.lastError();
        }
    }

    {
        auto initialUpdateTracksValidityQueryText = QStringLiteral("UPDATE `TracksMapping` SET `TrackValid` = 0");

        auto result = d->mInitialUpdateTracksValidity.prepare(initialUpdateTracksValidityQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mInitialUpdateTracksValidity.lastError();
        }
    }

    {
        auto initialUpdateTracksValidityQueryText = QStringLiteral("UPDATE `TracksMapping` SET `TrackValid` = 1, `TrackID` = :trackId, `Priority` = :priority "
                                                                   "WHERE `FileName` = :fileName");

        auto result = d->mUpdateTrackMapping.prepare(initialUpdateTracksValidityQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mUpdateTrackMapping.lastError();
        }
    }

    {
        auto selectTracksMappingQueryText = QStringLiteral("SELECT `TrackID`, `FileName`, `DiscoverID`, `Priority` FROM `TracksMapping` WHERE `FileName` = :fileName");

        auto result = d->mSelectTracksMapping.prepare(selectTracksMappingQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTracksMapping.lastError();
        }
    }

    {
        auto selectTracksMappingPriorityQueryText = QStringLiteral("SELECT count(*) FROM `TracksMapping` WHERE `TrackID` = :trackId AND `FileName` != :fileName");

        auto result = d->mSelectTracksMappingPriority.prepare(selectTracksMappingPriorityQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTracksMappingPriority.lastError();
        }
    }

    {
        auto insertMusicSourceQueryText = QStringLiteral("INSERT OR IGNORE INTO `DiscoverSource` (`ID`, `Name`) "
                                                   "VALUES (:discoverId, :name)");

        auto result = d->mInsertMusicSource.prepare(insertMusicSourceQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertMusicSource.lastError();
        }
    }

    {
        auto selectMusicSourceQueryText = QStringLiteral("SELECT `ID` FROM `DiscoverSource` WHERE `Name` = :name");

        auto result = d->mSelectMusicSource.prepare(selectMusicSourceQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectMusicSource.lastError();
        }
    }

    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.`ID`,  tracksMapping.`FileName` "
                                                   "FROM `Tracks` tracks, `Artists` artist, `TracksMapping` tracksMapping "
                                                   "WHERE "
                                                   "tracks.`Title` = :title AND "
                                                   "tracks.`AlbumID` = :album AND "
                                                   "artist.`Name` = :artist AND "
                                                   "artist.`ID` = tracks.`ArtistID` AND "
                                                   "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                   "tracksMapping.`Priority` = 1");

        auto result = d->mSelectTrackIdFromTitleAlbumIdArtistQuery.prepare(selectTrackQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumIdArtistQuery.lastError();
        }

        auto insertTrackQueryText = QStringLiteral("INSERT INTO `Tracks` (`ID`, `Title`, `AlbumID`, `ArtistID`, `TrackNumber`, `DiscNumber`, `Duration`, `Rating`) "
                                                   "VALUES (:trackId, :title, :album, :artistId, :trackNumber, :discNumber, :trackDuration, :trackRating)");

        result = d->mInsertTrackQuery.prepare(insertTrackQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mInsertTrackQuery.lastError();
        }
    }
    {
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.ID "
                                                   "FROM `Tracks` tracks, `Albums` albums, `Artists` artist "
                                                   "WHERE "
                                                   "tracks.`Title` = :title AND "
                                                   "tracks.`AlbumID` = albums.`ID` AND "
                                                   "albums.`Title` = :album AND "
                                                   "artist.`ID` = tracks.`ArtistID` AND "
                                                   "artist.`Name` = :artist");

        auto result = d->mSelectTrackIdFromTitleAlbumArtistQuery.prepare(selectTrackQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumArtistQuery.lastError();
        }
    }
    {
        auto selectAlbumTrackCountQueryText = QStringLiteral("SELECT `TracksCount` "
                                                             "FROM `Albums`"
                                                             "WHERE "
                                                             "`ID` = :albumId");

        auto result = d->mSelectAlbumTrackCountQuery.prepare(selectAlbumTrackCountQueryText);

        if (!result) {
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
            qDebug() << "DatabaseInterface::initRequest" << d->mUpdateAlbumQuery.lastError();
            qDebug() << "DatabaseInterface::initRequest" << updateAlbumQueryText;
        }
    }

    {
        auto updateIsSingleDiscAlbumFromIdQueryText = QStringLiteral("UPDATE `Albums` "
                                                                     "SET `IsSingleDiscAlbum` = (SELECT COUNT(DISTINCT DiscNumber) = 1 FROM `Tracks` WHERE `AlbumID` = :albumId) "
                                                                     "WHERE "
                                                                     "`ID` = :albumId");

        auto result = d->mUpdateIsSingleDiscAlbumFromIdQuery.prepare(updateIsSingleDiscAlbumFromIdQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mUpdateIsSingleDiscAlbumFromIdQuery.lastError();
            qDebug() << "DatabaseInterface::initRequest" << updateIsSingleDiscAlbumFromIdQueryText;
        }
    }

    {
        auto selectTracksFromArtistQueryText = QStringLiteral("SELECT "
                                                              "tracks.`Title`, "
                                                              "tracks.`ID`, "
                                                              "artist.`Name`, "
                                                              "tracksMapping.`FileName`, "
                                                              "tracks.`TrackNumber`, "
                                                              "tracks.`DiscNumber`, "
                                                              "tracks.`Duration`, "
                                                              "albums.`CoverFileName`, "
                                                              "albums.`Title`, "
                                                              "albumArtist.`Name`, "
                                                              "tracks.`Rating` "
                                                              "FROM `Tracks` tracks, `Albums` albums, `Artists` artist, `Artists` albumArtist, `TracksMapping` tracksMapping "
                                                              "WHERE "
                                                              "artist.`Name` = :artistName AND "
                                                              "tracks.`AlbumID` = albums.`ID` AND "
                                                              "artist.`ID` = tracks.`ArtistID` AND "
                                                              "albumArtist.`ID` = albums.`ArtistID` AND "
                                                              "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                              "tracksMapping.`Priority` = 1 "
                                                              "ORDER BY tracks.`Title` ASC, "
                                                              "albums.`Title` ASC");

        auto result = d->mSelectTracksFromArtist.prepare(selectTracksFromArtistQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTracksFromArtist.lastError();
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
            qDebug() << "DatabaseInterface::initRequest" << selectArtistQueryText << d->mSelectArtistQuery.lastError();
            qDebug() << d->mTracksDatabase.lastError();
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
                                                               "tracks.`Rating` "
                                                               "FROM `Tracks` tracks, `Artists` artist, `Artists` artistAlbum, `Albums` album, `TracksMapping` tracksMapping "
                                                               "WHERE "
                                                               "tracks.`AlbumID` = album.`ID` AND "
                                                               "artist.`ID` = tracks.`ArtistID` AND "
                                                               "artistAlbum.`ID` = album.`ArtistID` AND "
                                                               "tracksMapping.`TrackID` = tracks.`ID` AND "
                                                               "tracksMapping.`FileName` = :filePath AND "
                                                               "tracksMapping.`Priority` = 1");

        auto result = d->mSelectTrackFromFilePathQuery.prepare(selectTrackFromFilePathQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackFromFilePathQuery.lastError();
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackFromFilePathQuery.lastQuery();
        }
    }

    {
        auto removeTrackQueryText = QStringLiteral("DELETE FROM `Tracks` "
                                                   "WHERE "
                                                   "`ID` = :trackId");

        auto result = d->mRemoveTrackQuery.prepare(removeTrackQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveTrackQuery.lastError();
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveTrackQuery.lastQuery();
        }
    }

    {
        auto removeAlbumQueryText = QStringLiteral("DELETE FROM `Albums` "
                                                   "WHERE "
                                                   "`ID` = :albumId");

        auto result = d->mRemoveAlbumQuery.prepare(removeAlbumQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveAlbumQuery.lastError();
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveAlbumQuery.lastQuery();
        }
    }

    {
        auto removeAlbumQueryText = QStringLiteral("DELETE FROM `Artists` "
                                                   "WHERE "
                                                   "`ID` = :artistId");

        auto result = d->mRemoveArtistQuery.prepare(removeAlbumQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveArtistQuery.lastError();
            qDebug() << "DatabaseInterface::initRequest" << d->mRemoveArtistQuery.lastQuery();
        }
    }

    transactionResult = finishTransaction();

    d->mInitFinished = true;
    Q_EMIT requestsInitDone();
}

qulonglong DatabaseInterface::insertAlbum(const QString &title, const QString &albumArtist, const QUrl &albumArtURI, int tracksCount, bool isSingleDiscAlbum)
{
    auto result = qulonglong(0);

    if (title.isEmpty()) {
        return result;
    }

    d->mSelectAlbumIdFromTitleQuery.bindValue(QStringLiteral(":title"), title);
    d->mSelectAlbumIdFromTitleQuery.bindValue(QStringLiteral(":artistId"), insertArtist(albumArtist));

    auto queryResult = d->mSelectAlbumIdFromTitleQuery.exec();

    if (!queryResult || !d->mSelectAlbumIdFromTitleQuery.isSelect() || !d->mSelectAlbumIdFromTitleQuery.isActive()) {
        qDebug() << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleQuery.lastQuery();
        qDebug() << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleQuery.boundValues();
        qDebug() << "DatabaseInterface::insertAlbum" << d->mSelectAlbumIdFromTitleQuery.lastError();

        d->mSelectAlbumIdFromTitleQuery.finish();

        return result;
    }

    if (d->mSelectAlbumIdFromTitleQuery.next()) {
        result = d->mSelectAlbumIdFromTitleQuery.record().value(0).toULongLong();

        d->mSelectAlbumIdFromTitleQuery.finish();

        return result;
    }

    d->mSelectAlbumIdFromTitleQuery.finish();

    d->mInsertAlbumQuery.bindValue(QStringLiteral(":albumId"), d->mAlbumId);
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":title"), title);
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":artistId"), insertArtist(albumArtist));
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":coverFileName"), albumArtURI);
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":tracksCount"), tracksCount);
    d->mInsertAlbumQuery.bindValue(QStringLiteral(":isSingleDiscAlbum"), isSingleDiscAlbum);

    queryResult = d->mInsertAlbumQuery.exec();

    if (!queryResult || !d->mInsertAlbumQuery.isActive()) {
        qDebug() << "DatabaseInterface::insertAlbum" << d->mInsertAlbumQuery.lastQuery();
        qDebug() << "DatabaseInterface::insertAlbum" << d->mInsertAlbumQuery.boundValues();
        qDebug() << "DatabaseInterface::insertAlbum" << d->mInsertAlbumQuery.lastError();

        d->mInsertAlbumQuery.finish();

        return result;
    }

    result = d->mAlbumId;

    ++d->mAlbumId;

    d->mInsertAlbumQuery.finish();

    Q_EMIT albumAdded(internalAlbumFromId(d->mAlbumId - 1));

    return result;
}

void DatabaseInterface::updateIsSingleDiscAlbumFromId(qulonglong albumId) const
{
    d->mUpdateIsSingleDiscAlbumFromIdQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = d->mUpdateIsSingleDiscAlbumFromIdQuery.exec();

    if (!result || !d->mUpdateIsSingleDiscAlbumFromIdQuery.isActive()) {
        qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mUpdateIsSingleDiscAlbumFromIdQuery.lastQuery();
        qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mUpdateIsSingleDiscAlbumFromIdQuery.boundValues();
        qDebug() << "DatabaseInterface::updateIsSingleDiscAlbumFromId" << d->mUpdateIsSingleDiscAlbumFromIdQuery.lastError();

        d->mUpdateIsSingleDiscAlbumFromIdQuery.finish();

        return;
    }

    d->mUpdateIsSingleDiscAlbumFromIdQuery.finish();
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
    d->mUpdateTrackMapping.bindValue(QStringLiteral(":priority"), computeTrackPriority(trackId, fileName) + 1);

    auto queryResult = d->mUpdateTrackMapping.exec();

    if (!queryResult || !d->mUpdateTrackMapping.isActive()) {
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
    auto result = int(0);

    if (!d) {
        return result;
    }

    d->mSelectTracksMappingPriority.bindValue(QStringLiteral(":trackId"), trackId);
    d->mSelectTracksMappingPriority.bindValue(QStringLiteral(":fileName"), fileName);

    auto queryResult = d->mSelectTracksMappingPriority.exec();

    if (!queryResult || !d->mSelectTracksMappingPriority.isSelect() || !d->mSelectTracksMappingPriority.isActive()) {
        qDebug() << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMappingPriority.lastQuery();
        qDebug() << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMappingPriority.boundValues();
        qDebug() << "DatabaseInterface::internalTrackIdFromFileName" << d->mSelectTracksMappingPriority.lastError();

        d->mSelectTracksMappingPriority.finish();

        return result;
    }

    if (d->mSelectTracksMappingPriority.next()) {
        result = d->mSelectTracksMappingPriority.record().value(0).toInt();
    }

    d->mSelectTracksMappingPriority.finish();

    return result;
}

void DatabaseInterface::internalInsertTrack(const MusicAudioTrack &oneTrack, const QHash<QString, QUrl> &covers, int originTrackId, QSet<qulonglong> &modifiedAlbumIds)
{
    if (oneTrack.albumArtist().isEmpty()) {
        return;
    }

    auto albumId = insertAlbum(oneTrack.albumName(), oneTrack.albumArtist(), covers[oneTrack.albumName()], 0, true);

    auto otherTrackId = internalTrackIdFromTitleAlbumArtist(oneTrack.title(), oneTrack.albumName(), oneTrack.artist());
    bool isModifiedTrack = otherTrackId != 0;

    if (isModifiedTrack) {
        originTrackId = otherTrackId;

        removeTrackInDatabase(originTrackId);
    } else {
        originTrackId = d->mTrackId;
    }

    d->mInsertTrackQuery.bindValue(QStringLiteral(":trackId"), originTrackId);
    d->mInsertTrackQuery.bindValue(QStringLiteral(":title"), oneTrack.title());
    d->mInsertTrackQuery.bindValue(QStringLiteral(":album"), albumId);
    d->mInsertTrackQuery.bindValue(QStringLiteral(":artistId"), insertArtist(oneTrack.artist()));
    d->mInsertTrackQuery.bindValue(QStringLiteral(":trackNumber"), oneTrack.trackNumber());
    d->mInsertTrackQuery.bindValue(QStringLiteral(":discNumber"), oneTrack.discNumber());
    d->mInsertTrackQuery.bindValue(QStringLiteral(":trackDuration"), QVariant::fromValue<qlonglong>(oneTrack.duration().msecsSinceStartOfDay()));
    d->mInsertTrackQuery.bindValue(QStringLiteral(":trackRating"), oneTrack.rating());

    auto result = d->mInsertTrackQuery.exec();

    if (result && d->mInsertTrackQuery.isActive()) {
        d->mInsertTrackQuery.finish();

        if (!isModifiedTrack) {
            ++d->mTrackId;
        }

        updateTrackOrigin(originTrackId, oneTrack.resourceURI());

        if (isModifiedTrack) {
            Q_EMIT trackModified(originTrackId);
            modifiedAlbumIds.insert(albumId);
        } else {
            Q_EMIT trackAdded(originTrackId);
        }

        updateIsSingleDiscAlbumFromId(albumId);
        if (updateTracksCount(albumId)) {
            modifiedAlbumIds.insert(albumId);
        }
    } else {
        d->mInsertTrackQuery.finish();

        qDebug() << "DatabaseInterface::modifyTracksList" << d->mInsertTrackQuery.lastQuery();
        qDebug() << "DatabaseInterface::modifyTracksList" << d->mInsertTrackQuery.boundValues();
        qDebug() << "DatabaseInterface::modifyTracksList" << d->mInsertTrackQuery.lastError();
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
    d->mRemoveTrackQuery.bindValue(QStringLiteral(":trackId"), trackId);

    auto result = d->mRemoveTrackQuery.exec();

    if (!result || !d->mRemoveTrackQuery.isActive()) {
        qDebug() << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackQuery.lastQuery();
        qDebug() << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackQuery.boundValues();
        qDebug() << "DatabaseInterface::removeTrackInDatabase" << d->mRemoveTrackQuery.lastError();
    }

    d->mRemoveTrackQuery.finish();
}

void DatabaseInterface::removeAlbumInDatabase(qulonglong albumId)
{
    d->mRemoveAlbumQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = d->mRemoveAlbumQuery.exec();

    if (!result || !d->mRemoveAlbumQuery.isActive()) {
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
        qDebug() << "DatabaseInterface::fetchTracks" << d->mSelectTrackQuery.lastQuery();
        qDebug() << "DatabaseInterface::fetchTracks" << d->mSelectTrackQuery.boundValues();
        qDebug() << "DatabaseInterface::fetchTracks" << d->mSelectTrackQuery.lastError();
    }

    while (d->mSelectTrackQuery.next()) {
        MusicAudioTrack newTrack;

        const auto &currentRecord = d->mSelectTrackQuery.record();

        newTrack.setDatabaseId(currentRecord.value(0).toULongLong());
        newTrack.setTitle(currentRecord.value(1).toString());
        newTrack.setParentId(currentRecord.value(2).toString());
        newTrack.setArtist(currentRecord.value(3).toString());
        newTrack.setAlbumName(currentRecord.value(4).toString());
        newTrack.setAlbumArtist(currentRecord.value(5).toString());
        newTrack.setAlbumCover(currentRecord.value(11).toUrl());
        newTrack.setResourceURI(currentRecord.value(6).toUrl());
        newTrack.setTrackNumber(currentRecord.value(7).toInt());
        newTrack.setDiscNumber(currentRecord.value(8).toInt());
        newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(currentRecord.value(9).toInt()));
        newTrack.setRating(currentRecord.value(10).toInt());
        newTrack.setValid(true);

        allTracks.push_back(newTrack);
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

MusicAlbum DatabaseInterface::internalAlbumFromTitle(const QString &title)
{
    auto result = MusicAlbum();

    d->mSelectAlbumIdFromTitleQuery.bindValue(QStringLiteral(":title"), title);

    auto queryResult = d->mSelectAlbumIdFromTitleQuery.exec();

    if (!queryResult || !d->mSelectAlbumIdFromTitleQuery.isSelect() || !d->mSelectAlbumIdFromTitleQuery.isActive()) {
        qDebug() << "DatabaseInterface::albumFromTitleAndAuthor" << d->mSelectAlbumIdFromTitleQuery.lastQuery();
        qDebug() << "DatabaseInterface::albumFromTitleAndAuthor" << d->mSelectAlbumIdFromTitleQuery.boundValues();
        qDebug() << "DatabaseInterface::albumFromTitleAndAuthor" << d->mSelectAlbumIdFromTitleQuery.lastError();

        d->mSelectAlbumIdFromTitleQuery.finish();

        return result;
    }

    if (!d->mSelectAlbumIdFromTitleQuery.next()) {
        d->mSelectAlbumIdFromTitleQuery.finish();

        return result;
    }

    auto albumId = d->mSelectAlbumIdFromTitleQuery.record().value(0).toULongLong();
    d->mSelectAlbumIdFromTitleQuery.finish();

    result = internalAlbumFromId(albumId);

    return result;
}

qulonglong DatabaseInterface::internalAlbumIdFromTitle(const QString &title)
{
    auto result = qulonglong(0);

    d->mSelectAlbumIdFromTitleQuery.bindValue(QStringLiteral(":title"), title);

    auto queryResult = d->mSelectAlbumIdFromTitleQuery.exec();

    if (!queryResult || !d->mSelectAlbumIdFromTitleQuery.isSelect() || !d->mSelectAlbumIdFromTitleQuery.isActive()) {
        qDebug() << "DatabaseInterface::albumFromTitleAndAuthor" << d->mSelectAlbumIdFromTitleQuery.lastQuery();
        qDebug() << "DatabaseInterface::albumFromTitleAndAuthor" << d->mSelectAlbumIdFromTitleQuery.boundValues();
        qDebug() << "DatabaseInterface::albumFromTitleAndAuthor" << d->mSelectAlbumIdFromTitleQuery.lastError();

        d->mSelectAlbumIdFromTitleQuery.finish();

        return result;
    }

    if (!d->mSelectAlbumIdFromTitleQuery.next()) {
        d->mSelectAlbumIdFromTitleQuery.finish();

        return result;
    }

    result = d->mSelectAlbumIdFromTitleQuery.record().value(0).toULongLong();
    d->mSelectAlbumIdFromTitleQuery.finish();

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

    result.setDatabaseId(currentRecord.value(0).toULongLong());
    result.setAlbumName(internalAlbumDataFromId(currentRecord.value(2).toULongLong(), DatabaseInterface::AlbumData::Title).toString());
    result.setArtist(currentRecord.value(3).toString());
    result.setAlbumArtist(currentRecord.value(4).toString());
    result.setDuration(QTime::fromMSecsSinceStartOfDay(currentRecord.value(8).toLongLong()));
    result.setResourceURI(currentRecord.value(5).toUrl());
    result.setAlbumCover(currentRecord.value(10).toUrl());
    result.setTitle(currentRecord.value(1).toString());
    result.setTrackNumber(currentRecord.value(6).toInt());
    result.setDiscNumber(currentRecord.value(7).toInt());
    result.setRating(currentRecord.value(9).toInt());
    result.setValid(true);

    d->mSelectTrackFromIdQuery.finish();

    return result;
}

qulonglong DatabaseInterface::internalTrackIdFromTitleAlbumArtist(const QString &title, const QString &album, const QString &artist) const
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    d->mSelectTrackIdFromTitleAlbumArtistQuery.bindValue(QStringLiteral(":title"), title);
    d->mSelectTrackIdFromTitleAlbumArtistQuery.bindValue(QStringLiteral(":album"), album);
    d->mSelectTrackIdFromTitleAlbumArtistQuery.bindValue(QStringLiteral(":artist"), artist);

    auto queryResult = d->mSelectTrackIdFromTitleAlbumArtistQuery.exec();

    if (!queryResult || !d->mSelectTrackIdFromTitleAlbumArtistQuery.isSelect() || !d->mSelectTrackIdFromTitleAlbumArtistQuery.isActive()) {
        qDebug() << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleAlbumArtistQuery.lastQuery();
        qDebug() << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleAlbumArtistQuery.boundValues();
        qDebug() << "DatabaseInterface::trackIdFromTitleAlbumArtist" << d->mSelectTrackIdFromTitleAlbumArtistQuery.lastError();

        d->mSelectTrackIdFromTitleAlbumArtistQuery.finish();

        return result;
    }

    if (d->mSelectTrackIdFromTitleAlbumArtistQuery.next()) {
        result = d->mSelectTrackIdFromTitleAlbumArtistQuery.record().value(0).toInt();
    }

    d->mSelectTrackIdFromTitleAlbumArtistQuery.finish();

    return result;
}

qulonglong DatabaseInterface::internalTrackIdFromFileName(const QUrl &fileName) const
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    d->mSelectTracksMapping.bindValue(QStringLiteral(":fileName"), fileName);

    auto queryResult = d->mSelectTracksMapping.exec();

    if (!queryResult || !d->mSelectTracksMapping.isSelect() || !d->mSelectTracksMapping.isActive()) {
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

QVariant DatabaseInterface::internalAlbumDataFromId(qulonglong albumId, DatabaseInterface::AlbumData dataType)
{
    auto result = QVariant();

    auto currentAlbum = internalAlbumFromId(albumId);

    if (!currentAlbum.isValid()) {
        return result;
    }

    switch(dataType)
    {
    case DatabaseInterface::AlbumData::Id:
        result = currentAlbum.id();
        break;
    case DatabaseInterface::AlbumData::Image:
        result = currentAlbum.albumArtURI();
        break;
    case DatabaseInterface::AlbumData::Title:
        result = currentAlbum.title();
        break;
    case DatabaseInterface::AlbumData::Artist:
        result = currentAlbum.artist();
        break;
    case DatabaseInterface::AlbumData::TracksCount:
        result = currentAlbum.tracksCount();
        break;
    }

    return result;
}

QList<MusicAudioTrack> DatabaseInterface::internalTracksFromAuthor(const QString &artistName) const
{
    auto allTracks = QList<MusicAudioTrack>();

    d->mSelectTracksFromArtist.bindValue(QStringLiteral(":artistName"), artistName);

    auto result = d->mSelectTracksFromArtist.exec();

    if (!result || !d->mSelectTracksFromArtist.isSelect() || !d->mSelectTracksFromArtist.isActive()) {
        qDebug() << "DatabaseInterface::tracksFromAuthor" << d->mSelectTracksFromArtist.lastQuery();
        qDebug() << "DatabaseInterface::tracksFromAuthor" << d->mSelectTracksFromArtist.boundValues();
        qDebug() << "DatabaseInterface::tracksFromAuthor" << d->mSelectTracksFromArtist.lastError();

        return allTracks;
    }

    while (d->mSelectTracksFromArtist.next()) {
        MusicAudioTrack newTrack;

        const auto &currentRecord = d->mSelectTracksFromArtist.record();

        newTrack.setDatabaseId(currentRecord.value(1).toULongLong());
        newTrack.setTitle(currentRecord.value(0).toString());
        newTrack.setAlbumName(currentRecord.value(8).toString());
        newTrack.setAlbumArtist(currentRecord.value(9).toString());
        newTrack.setArtist(currentRecord.value(2).toString());
        newTrack.setResourceURI(currentRecord.value(3).toUrl());
        newTrack.setAlbumCover(currentRecord.value(7).toUrl());
        newTrack.setTrackNumber(currentRecord.value(4).toInt());
        newTrack.setDiscNumber(currentRecord.value(5).toInt());
        newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(currentRecord.value(6).toInt()));
        newTrack.setRating(currentRecord.value(10).toInt());
        newTrack.setValid(true);

        allTracks.push_back(newTrack);
    }

    d->mSelectTracksFromArtist.finish();

    return allTracks;
}


#include "moc_databaseinterface.cpp"
