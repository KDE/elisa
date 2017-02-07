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

    DatabaseInterfacePrivate(QSqlDatabase tracksDatabase)
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
          mRemoveArtistQuery(mTracksDatabase)
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

    qulonglong mAlbumId = 1;

    qulonglong mArtistId = 1;

    qulonglong mTrackId = 1;

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

void DatabaseInterface::init(const QString &dbName)
{
    QSqlDatabase tracksDatabase = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), dbName);

    tracksDatabase.setDatabaseName(QStringLiteral("file:memdb1?mode=memory"));
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
}

MusicAlbum DatabaseInterface::albumFromTitle(QString title) const
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

QVector<MusicAlbum> DatabaseInterface::allAlbums() const
{
    auto result = QVector<MusicAlbum>();

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

        newAlbum.setDatabaseId(d->mSelectAllAlbumsQuery.record().value(0).toULongLong());
        newAlbum.setTitle(d->mSelectAllAlbumsQuery.record().value(1).toString());
        newAlbum.setId(d->mSelectAllAlbumsQuery.record().value(2).toString());
        newAlbum.setArtist(d->mSelectAllAlbumsQuery.record().value(3).toString());
        newAlbum.setAlbumArtURI(d->mSelectAllAlbumsQuery.record().value(4).toUrl());
        newAlbum.setTracksCount(d->mSelectAllAlbumsQuery.record().value(5).toInt());
        newAlbum.setIsSingleDiscAlbum(d->mSelectAllAlbumsQuery.record().value(6).toBool());
        newAlbum.setTracks(fetchTracks(newAlbum.databaseId()));
        newAlbum.setTrackIds(fetchTrackIds(newAlbum.databaseId()));
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

QVector<MusicArtist> DatabaseInterface::allArtists() const
{
    auto result = QVector<MusicArtist>();

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return result;
    }

    QString currentFilter(QStringLiteral("%%1%"));

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

        newArtist.setDatabaseId(d->mSelectAllArtistsQuery.record().value(0).toULongLong());
        newArtist.setName(d->mSelectAllArtistsQuery.record().value(1).toString());
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

QVector<MusicAudioTrack> DatabaseInterface::tracksFromAuthor(QString artistName) const
{
    auto allTracks = QVector<MusicAudioTrack>();

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

    result.setDatabaseId(d->mSelectArtistQuery.record().value(0).toULongLong());
    result.setName(d->mSelectArtistQuery.record().value(1).toString());
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

MusicAudioTrack DatabaseInterface::trackFromDatabaseId(qulonglong id) const
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

qulonglong DatabaseInterface::trackIdFromTitleAlbumArtist(QString title, QString album, QString artist) const
{
    auto result = qulonglong(0);

    if (!d) {
        return result;
    }

    auto transactionResult = startTransaction();
    if (!transactionResult) {
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

        transactionResult = finishTransaction();
        if (!transactionResult) {
            return result;
        }
    }

    if (d->mSelectTrackIdFromTitleAlbumArtistQuery.next()) {
        result = d->mSelectTrackIdFromTitleAlbumArtistQuery.record().value(0).toInt();
    }

    d->mSelectTrackIdFromTitleAlbumArtistQuery.finish();

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return result;
    }

    return result;
}

void DatabaseInterface::insertTracksList(QHash<QString, QVector<MusicAudioTrack> > tracks, QHash<QString, QUrl> covers)
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    auto newAddedAlbums = QVector<qulonglong>();
    auto newTracks = QVector<qulonglong>();
    auto maximumAlbumId = qulonglong(0);
    quintptr albumId = 0;

    for (const auto &album : tracks) {
        bool albumIsNew = false;
        bool albumIsModified = false;
        MusicAlbum newAlbum;

        int previousDiscNumber = album[0].discNumber();

        for(const auto &track : album) {
            if (newAlbum.artist().isNull() && !track.albumArtist().isEmpty()) {
                newAlbum.setArtist(track.albumArtist());
            }
            if (!track.albumArtist().isEmpty() && newAlbum.artist() != track.albumArtist()) {
                newAlbum.setArtist(i18nc("Artist name for albums with more than one artist (like compilations", "Various Artists"));
            }

            if (newAlbum.title().isNull()) {
                newAlbum.setTitle(track.albumName());
            }

            if (newAlbum.albumArtURI().isEmpty()) {
                newAlbum.setAlbumArtURI(covers[track.albumName()]);
            }

            if (previousDiscNumber != track.discNumber()) {
                newAlbum.setIsSingleDiscAlbum(false);
            }
        }

        if (newAlbum.artist().isEmpty()) {
            continue;
        }

        newAlbum.setTracksCount(album.size());

        d->mSelectAlbumIdFromTitleQuery.bindValue(QStringLiteral(":title"), newAlbum.title());
        d->mSelectAlbumIdFromTitleQuery.bindValue(QStringLiteral(":artistId"), insertArtist(newAlbum.artist()));

        auto result = d->mSelectAlbumIdFromTitleQuery.exec();

        if (!result || !d->mSelectAlbumIdFromTitleQuery.isSelect() || !d->mSelectAlbumIdFromTitleQuery.isActive()) {
            qDebug() << "DatabaseInterface::insertTracksList" << d->mSelectAlbumIdFromTitleQuery.lastQuery();
            qDebug() << "DatabaseInterface::insertTracksList" << d->mSelectAlbumIdFromTitleQuery.boundValues();
            qDebug() << "DatabaseInterface::insertTracksList" << d->mSelectAlbumIdFromTitleQuery.lastError();

            rollBackTransaction();
            return;
        }

        if (d->mSelectAlbumIdFromTitleQuery.next()) {
            albumId = d->mSelectAlbumIdFromTitleQuery.record().value(0).toULongLong();
            maximumAlbumId = std::max(maximumAlbumId, d->mSelectAlbumIdFromTitleQuery.record().value(0).toULongLong());
            d->mSelectAlbumIdFromTitleQuery.finish();
            albumIsModified = true;
        } else {
            d->mSelectAlbumIdFromTitleQuery.finish();

            d->mInsertAlbumQuery.bindValue(QStringLiteral(":albumId"), d->mAlbumId);
            d->mInsertAlbumQuery.bindValue(QStringLiteral(":title"), newAlbum.title());
            d->mInsertAlbumQuery.bindValue(QStringLiteral(":artistId"), insertArtist(newAlbum.artist()));
            d->mInsertAlbumQuery.bindValue(QStringLiteral(":coverFileName"), newAlbum.albumArtURI());
            d->mInsertAlbumQuery.bindValue(QStringLiteral(":tracksCount"), newAlbum.tracksCount());
            d->mInsertAlbumQuery.bindValue(QStringLiteral(":isSingleDiscAlbum"), newAlbum.isSingleDiscAlbum());

            result = d->mInsertAlbumQuery.exec();

            if (result && d->mInsertAlbumQuery.isActive()) {
                newAddedAlbums.push_back(d->mAlbumId);
                albumIsNew = true;
                ++d->mAlbumId;
            } else {
                qDebug() << "DatabaseInterface::insertTracksList" << "error during album insert";
                qDebug() << "DatabaseInterface::insertTracksList" << d->mInsertAlbumQuery.lastQuery();
                qDebug() << "DatabaseInterface::insertTracksList" << d->mInsertAlbumQuery.boundValues();
                qDebug() << "DatabaseInterface::insertTracksList" << d->mInsertAlbumQuery.lastError();

                d->mInsertAlbumQuery.finish();

                rollBackTransaction();
                return;
            }

            d->mInsertAlbumQuery.finish();

            result = d->mSelectAlbumIdFromTitleQuery.exec();

            if (!result || !d->mSelectAlbumIdFromTitleQuery.isSelect() || !d->mSelectAlbumIdFromTitleQuery.isActive()) {
                qDebug() << "DatabaseInterface::insertTracksList" << d->mSelectAlbumIdFromTitleQuery.lastQuery();
                qDebug() << "DatabaseInterface::insertTracksList" << d->mSelectAlbumIdFromTitleQuery.boundValues();
                qDebug() << "DatabaseInterface::insertTracksList" << d->mSelectAlbumIdFromTitleQuery.lastError();

                d->mSelectAlbumIdFromTitleQuery.finish();

                rollBackTransaction();
                return;
            }

            if (d->mSelectAlbumIdFromTitleQuery.next()) {
                albumId = d->mSelectAlbumIdFromTitleQuery.record().value(0).toULongLong();
                maximumAlbumId = std::max(maximumAlbumId, d->mSelectAlbumIdFromTitleQuery.record().value(0).toULongLong());
            }
            d->mSelectAlbumIdFromTitleQuery.finish();
        }

        for(const auto &track : album) {
            QString artistName = track.artist();

            if (artistName.isEmpty()) {
                artistName = newAlbum.artist();

                continue;
            }

            d->mSelectTrackIdFromTitleAlbumIdArtistQuery.bindValue(QStringLiteral(":title"), track.title());
            d->mSelectTrackIdFromTitleAlbumIdArtistQuery.bindValue(QStringLiteral(":album"), albumId);
            d->mSelectTrackIdFromTitleAlbumIdArtistQuery.bindValue(QStringLiteral(":artist"), artistName);

            result = d->mSelectTrackIdFromTitleAlbumIdArtistQuery.exec();

            if (!result || !d->mSelectTrackIdFromTitleAlbumIdArtistQuery.isSelect() || !d->mSelectTrackIdFromTitleAlbumIdArtistQuery.isActive()) {
                qDebug() << "DatabaseInterface::insertTracksList" << d->mSelectTrackIdFromTitleAlbumIdArtistQuery.lastQuery();
                qDebug() << "DatabaseInterface::insertTracksList" << d->mSelectTrackIdFromTitleAlbumIdArtistQuery.boundValues();
                qDebug() << "DatabaseInterface::insertTracksList" << d->mSelectTrackIdFromTitleAlbumIdArtistQuery.lastError();

                rollBackTransaction();
                return;
            }

            if (d->mSelectTrackIdFromTitleAlbumIdArtistQuery.next()) {
                d->mTrackId = std::max(d->mTrackId, d->mSelectTrackIdFromTitleAlbumIdArtistQuery.record().value(0).toULongLong() + 1);

                d->mSelectTrackIdFromTitleAlbumIdArtistQuery.finish();

                continue;
            } else {
                d->mSelectTrackIdFromTitleAlbumIdArtistQuery.finish();
                d->mInsertTrackQuery.bindValue(QStringLiteral(":trackId"), d->mTrackId);
                d->mInsertTrackQuery.bindValue(QStringLiteral(":title"), track.title());
                d->mInsertTrackQuery.bindValue(QStringLiteral(":album"), albumId);
                d->mInsertTrackQuery.bindValue(QStringLiteral(":artistId"), insertArtist(artistName));
                d->mInsertTrackQuery.bindValue(QStringLiteral(":fileName"), track.resourceURI().toString());
                d->mInsertTrackQuery.bindValue(QStringLiteral(":trackNumber"), track.trackNumber());
                d->mInsertTrackQuery.bindValue(QStringLiteral(":discNumber"), track.discNumber());
                d->mInsertTrackQuery.bindValue(QStringLiteral(":trackDuration"), QVariant::fromValue<qlonglong>(track.duration().msecsSinceStartOfDay()));

                result = d->mInsertTrackQuery.exec();

                if (result && d->mInsertTrackQuery.isActive()) {
                    newTracks.push_back(d->mTrackId);

                    Q_EMIT trackAdded(internalTrackFromDatabaseId(d->mTrackId));

                    ++d->mTrackId;
                } else {
                    qDebug() << "DatabaseInterface::insertTracksList" << d->mInsertTrackQuery.lastQuery();
                    qDebug() << "DatabaseInterface::insertTracksList" << d->mInsertTrackQuery.boundValues();
                    qDebug() << "DatabaseInterface::insertTracksList" << d->mInsertTrackQuery.lastError();

                    rollBackTransaction();
                    return;
                }

                d->mInsertTrackQuery.finish();
            }
        }

        if (albumIsNew) {
            Q_EMIT albumAdded(internalAlbumFromId(albumId));
        }
        if (albumIsModified) {
            Q_EMIT albumModified(internalAlbumFromId(albumId));
        }
    }

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }
}

void DatabaseInterface::removeTracksList(const QList<QUrl> removedTracks)
{
    auto transactionResult = startTransaction();
    if (!transactionResult) {
        return;
    }

    QList<MusicAudioTrack> willRemoveTask;

    for (auto removedTrackFileName : removedTracks) {
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

            removedTrack.setDatabaseId(d->mSelectTrackFromFilePathQuery.record().value(0).toULongLong());
            removedTrack.setTitle(d->mSelectTrackFromFilePathQuery.record().value(1).toString());
            removedTrack.setParentId(d->mSelectTrackFromFilePathQuery.record().value(2).toString());
            removedTrack.setArtist(d->mSelectTrackFromFilePathQuery.record().value(3).toString());
            removedTrack.setAlbumArtist(d->mSelectTrackFromFilePathQuery.record().value(4).toString());
            removedTrack.setResourceURI(d->mSelectTrackFromFilePathQuery.record().value(5).toUrl());
            removedTrack.setTrackNumber(d->mSelectTrackFromFilePathQuery.record().value(6).toInt());
            removedTrack.setDiscNumber(d->mSelectTrackFromFilePathQuery.record().value(7).toInt());
            removedTrack.setDuration(QTime::fromMSecsSinceStartOfDay(d->mSelectTrackFromFilePathQuery.record().value(8).toInt()));
            removedTrack.setAlbumName(d->mSelectTrackFromFilePathQuery.record().value(9).toString());
            removedTrack.setValid(true);

            willRemoveTask.push_back(removedTrack);
        }

        d->mSelectTrackFromFilePathQuery.finish();
    }

    for (auto oneRemovedTrack : willRemoveTask) {
        removeTrackInDatabase(oneRemovedTrack.databaseId());
        Q_EMIT trackRemoved(oneRemovedTrack);

        const auto &modifiedAlbum = internalAlbumFromTitle(oneRemovedTrack.albumName());
        const auto &allArtistTracks = internalTracksFromAuthor(oneRemovedTrack.artist());
        const auto &removedArtistId = internalArtistIdFromName(oneRemovedTrack.artist());
        const auto &removedArtist = internalArtistFromId(removedArtistId);

        if (modifiedAlbum.isValid() && !modifiedAlbum.isEmpty()) {
            Q_EMIT albumModified(internalAlbumFromId(modifiedAlbum.databaseId()));
        }

        if (modifiedAlbum.isValid() && modifiedAlbum.isEmpty()) {
            removeAlbumInDatabase(modifiedAlbum.databaseId());
            Q_EMIT albumRemoved(modifiedAlbum);
        }

        if (allArtistTracks.isEmpty()) {
            removeArtistInDatabase(removedArtistId);
            Q_EMIT artistRemoved(removedArtist);
        }
    }

    transactionResult = finishTransaction();
    if (!transactionResult) {
        return;
    }
}

void DatabaseInterface::databaseArtistAdded(MusicArtist newArtist)
{
    Q_EMIT artistAdded(newArtist);
}

void DatabaseInterface::databaseAlbumAdded(MusicAlbum newAlbum)
{
    Q_EMIT albumAdded(newAlbum);
}

void DatabaseInterface::databaseTrackAdded(MusicAudioTrack newTrack)
{
    Q_EMIT trackAdded(newTrack);
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

    if (!d->mTracksDatabase.tables().contains(QStringLiteral("DiscoverSource"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DiscoverSource` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`UUID` VARCHAR(55) NOT NULL, "
                                                                   "UNIQUE (`UUID`))"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError() << createSchemaQuery.lastError().nativeErrorCode();
        }
    }

    if (!d->mTracksDatabase.tables().contains(QStringLiteral("Artists"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Artists` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Name` VARCHAR(55) NOT NULL, "
                                                                   "UNIQUE (`Name`))"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastQuery();
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();
        }
    }

    if (!d->mTracksDatabase.tables().contains(QStringLiteral("Albums"))) {
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

    if (!d->mTracksDatabase.tables().contains(QStringLiteral("Tracks"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Tracks` ("
                                                                   "`ID` INTEGER PRIMARY KEY NOT NULL, "
                                                                   "`Title` VARCHAR(85) NOT NULL, "
                                                                   "`AlbumID` INTEGER NOT NULL, "
                                                                   "`ArtistID` INTEGER NOT NULL, "
                                                                   "`FileName` VARCHAR(255) NOT NULL, "
                                                                   "`TrackNumber` INTEGER NOT NULL, "
                                                                   "`DiscNumber` INTEGER, "
                                                                   "`Duration` INTEGER NOT NULL, "
                                                                   "UNIQUE (`Title`, `AlbumID`, `ArtistID`), "
                                                                   "CONSTRAINT fk_tracks_album FOREIGN KEY (`AlbumID`) REFERENCES `Albums`(`ID`), "
                                                                   "CONSTRAINT fk_tracks_artist FOREIGN KEY (`ArtistID`) REFERENCES `Artists`(`ID`))"));

        if (!result) {
            qDebug() << "DatabaseInterface::initDatabase" << createSchemaQuery.lastError();
        }
    }

    if (!d->mTracksDatabase.tables().contains(QStringLiteral("TracksMapping"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        const auto &result = createSchemaQuery.exec(QStringLiteral("CREATE TABLE `TracksMapping` ("
                                                                   "`TrackID` INTEGER NOT NULL, "
                                                                   "`DiscoverID` INTEGER NOT NULL, "
                                                                   "PRIMARY KEY (`TrackID`, `DiscoverID`), "
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
                                                                  "`TracksFileNameIndex` ON `Tracks` "
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
                                                                  "(`ArtistID`, `AlbumID`, `FileName`)"));

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
                                                   "artistAlbum.`Name`, "
                                                   "tracks.`FileName`, "
                                                   "tracks.`TrackNumber`, "
                                                   "tracks.`DiscNumber`, "
                                                   "tracks.`Duration` "
                                                   "FROM `Tracks` tracks, `Artists` artist, `Artists` artistAlbum, `Albums` album "
                                                   "WHERE "
                                                   "tracks.`AlbumID` = :albumId AND "
                                                   "artist.`ID` = tracks.`ArtistID` AND "
                                                   "album.`ID` = :albumId AND "
                                                   "artistAlbum.`ID` = album.`ArtistID` "
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
                                                         "tracks.`FileName`, "
                                                         "tracks.`TrackNumber`, "
                                                         "tracks.`DiscNumber`, "
                                                         "tracks.`Duration` "
                                                         "FROM `Tracks` tracks, `Artists` artist, `Artists` artistAlbum, `Albums` album "
                                                         "WHERE "
                                                         "tracks.`ID` = :trackId AND "
                                                         "artist.`ID` = tracks.`ArtistID` AND "
                                                         "artistAlbum.`ID` = album.`ArtistID` AND "
                                                         "tracks.`AlbumID` = album.`ID`");

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
        auto selectTrackQueryText = QStringLiteral("SELECT "
                                                   "tracks.ID "
                                                   "FROM `Tracks` tracks, `Artists` artist "
                                                   "WHERE "
                                                   "tracks.`Title` = :title AND "
                                                   "tracks.`AlbumID` = :album AND "
                                                   "artist.`Name` = :artist AND "
                                                   "artist.`ID` = tracks.`ArtistID`");

        auto result = d->mSelectTrackIdFromTitleAlbumIdArtistQuery.prepare(selectTrackQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mSelectTrackIdFromTitleAlbumIdArtistQuery.lastError();
        }

        auto insertTrackQueryText = QStringLiteral("INSERT INTO `Tracks` (`ID`, `Title`, `AlbumID`, `ArtistID`, `FileName`, `TrackNumber`, `DiscNumber`, `Duration`) "
                                                   "VALUES (:trackId, :title, :album, :artistId, :fileName, :trackNumber, :discNumber, :trackDuration)");

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
                                                   "SET `TracksCount`=:tracksCount "
                                                   "WHERE "
                                                   "`ID` = :albumId");

        auto result = d->mUpdateAlbumQuery.prepare(updateAlbumQueryText);

        if (!result) {
            qDebug() << "DatabaseInterface::initRequest" << d->mUpdateAlbumQuery.lastError();
        }
    }
    {
        auto selectTracksFromArtistQueryText = QStringLiteral("SELECT "
                                                              "tracks.`Title`, "
                                                              "tracks.`ID`, "
                                                              "artist.`Name`, "
                                                              "tracks.`FileName`, "
                                                              "tracks.`TrackNumber`, "
                                                              "tracks.`DiscNumber`, "
                                                              "tracks.`Duration`, "
                                                              "albums.`CoverFileName`, "
                                                              "albums.`Title`, "
                                                              "albumArtist.`Name` "
                                                              "FROM `Tracks` tracks, `Albums` albums, `Artists` artist, `Artists` albumArtist "
                                                              "WHERE "
                                                              "artist.`Name` = :artistName AND "
                                                              "tracks.`AlbumID` = albums.`ID` AND "
                                                              "artist.`ID` = tracks.`ArtistID` AND "
                                                              "albumArtist.`ID` = albums.`ArtistID` "
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
                                                               "tracks.`FileName`, "
                                                               "tracks.`TrackNumber`, "
                                                               "tracks.`DiscNumber`, "
                                                               "tracks.`Duration`, "
                                                               "album.`Title` "
                                                               "FROM `Tracks` tracks, `Artists` artist, `Artists` artistAlbum, `Albums` album "
                                                               "WHERE "
                                                               "tracks.`AlbumID` = album.`ID` AND "
                                                               "artist.`ID` = tracks.`ArtistID` AND "
                                                               "artistAlbum.`ID` = album.`ArtistID` AND "
                                                               "tracks.`FileName` = :filePath");

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

qulonglong DatabaseInterface::insertArtist(QString name)
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

qulonglong DatabaseInterface::internalArtistIdFromName(QString name)
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

QMap<qulonglong, MusicAudioTrack> DatabaseInterface::fetchTracks(qulonglong albumId) const
{
    auto allTracks = QMap<qulonglong, MusicAudioTrack>();

    d->mSelectTrackQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = d->mSelectTrackQuery.exec();

    if (!result || !d->mSelectTrackQuery.isSelect() || !d->mSelectTrackQuery.isActive()) {
        qDebug() << "DatabaseInterface::fetchTracks" << "not select" << d->mSelectTrackQuery.lastQuery();
        qDebug() << "DatabaseInterface::fetchTracks" << "not select" << d->mSelectTrackQuery.boundValues();
        qDebug() << "DatabaseInterface::fetchTracks" << d->mSelectTrackQuery.lastError();
    }

    while (d->mSelectTrackQuery.next()) {
        MusicAudioTrack newTrack;

        newTrack.setDatabaseId(d->mSelectTrackQuery.record().value(0).toULongLong());
        newTrack.setTitle(d->mSelectTrackQuery.record().value(1).toString());
        newTrack.setParentId(d->mSelectTrackQuery.record().value(2).toString());
        newTrack.setArtist(d->mSelectTrackQuery.record().value(3).toString());
        newTrack.setAlbumArtist(d->mSelectTrackQuery.record().value(4).toString());
        newTrack.setResourceURI(d->mSelectTrackQuery.record().value(5).toUrl());
        newTrack.setTrackNumber(d->mSelectTrackQuery.record().value(6).toInt());
        newTrack.setDiscNumber(d->mSelectTrackQuery.record().value(7).toInt());
        newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(d->mSelectTrackQuery.record().value(8).toInt()));
        newTrack.setValid(true);

        allTracks[newTrack.databaseId()] = newTrack;
    }

    d->mSelectTrackQuery.finish();

    updateTracksCount(albumId, allTracks.size());

    return allTracks;
}

QList<qulonglong> DatabaseInterface::fetchTrackIds(qulonglong albumId) const
{
    auto allTrackIds = QList<qulonglong>();

    d->mSelectTrackQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = d->mSelectTrackQuery.exec();

    if (!result || !d->mSelectTrackQuery.isSelect() || !d->mSelectTrackQuery.isActive()) {
        qDebug() << "DatabaseInterface::fetchTrackIds" << d->mSelectTrackQuery.lastQuery();
        qDebug() << "DatabaseInterface::fetchTrackIds" << d->mSelectTrackQuery.boundValues();
        qDebug() << "DatabaseInterface::fetchTrackIds" << d->mSelectTrackQuery.lastError();
    }

    while (d->mSelectTrackQuery.next()) {
        allTrackIds.push_back(d->mSelectTrackQuery.record().value(0).toULongLong());
    }

    d->mSelectTrackQuery.finish();

    return allTrackIds;
}

void DatabaseInterface::updateTracksCount(qulonglong albumId, int tracksCount) const
{
    d->mSelectAlbumTrackCountQuery.bindValue(QStringLiteral(":albumId"), albumId);

    auto result = d->mSelectAlbumTrackCountQuery.exec();

    if (!result || !d->mSelectAlbumTrackCountQuery.isSelect() || !d->mSelectAlbumTrackCountQuery.isActive()) {
        qDebug() << "DatabaseInterface::updateTracksCount" << d->mSelectAlbumTrackCountQuery.lastQuery();
        qDebug() << "DatabaseInterface::updateTracksCount" << d->mSelectAlbumTrackCountQuery.boundValues();
        qDebug() << "DatabaseInterface::updateTracksCount" << d->mSelectAlbumTrackCountQuery.lastError();

        d->mSelectAlbumTrackCountQuery.finish();

        return;
    }

    if (!d->mSelectAlbumTrackCountQuery.next()) {
        d->mSelectAlbumTrackCountQuery.finish();

        return;
    }

    auto oldTracksCount = d->mSelectAlbumTrackCountQuery.record().value(0).toInt();
    d->mSelectAlbumTrackCountQuery.finish();

    if (oldTracksCount != tracksCount) {
        d->mUpdateAlbumQuery.bindValue(QStringLiteral(":tracksCount"), tracksCount);
        d->mUpdateAlbumQuery.bindValue(QStringLiteral(":albumId"), albumId);

        result = d->mUpdateAlbumQuery.exec();

        if (!result || !d->mUpdateAlbumQuery.isActive()) {
            qDebug() << "DatabaseInterface::updateTracksCount" << d->mUpdateAlbumQuery.lastQuery();
            qDebug() << "DatabaseInterface::updateTracksCount" << d->mUpdateAlbumQuery.boundValues();
            qDebug() << "DatabaseInterface::updateTracksCount" << d->mUpdateAlbumQuery.lastError();

            d->mUpdateAlbumQuery.finish();

            return;
        }
        d->mUpdateAlbumQuery.finish();
    }
}

MusicAlbum DatabaseInterface::internalAlbumFromId(qulonglong albumId) const
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

    retrievedAlbum.setDatabaseId(d->mSelectAlbumQuery.record().value(0).toULongLong());
    retrievedAlbum.setTitle(d->mSelectAlbumQuery.record().value(1).toString());
    retrievedAlbum.setId(d->mSelectAlbumQuery.record().value(2).toString());
    retrievedAlbum.setArtist(d->mSelectAlbumQuery.record().value(3).toString());
    retrievedAlbum.setAlbumArtURI(d->mSelectAlbumQuery.record().value(4).toUrl());
    retrievedAlbum.setTracksCount(d->mSelectAlbumQuery.record().value(5).toInt());
    retrievedAlbum.setIsSingleDiscAlbum(d->mSelectAlbumQuery.record().value(6).toBool());
    retrievedAlbum.setTracks(fetchTracks(retrievedAlbum.databaseId()));
    retrievedAlbum.setTrackIds(fetchTrackIds(retrievedAlbum.databaseId()));
    retrievedAlbum.setValid(true);

    d->mSelectAlbumQuery.finish();

    return retrievedAlbum;
}

MusicAlbum DatabaseInterface::internalAlbumFromTitle(QString title) const
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

MusicAudioTrack DatabaseInterface::internalTrackFromDatabaseId(qulonglong id) const
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

    result.setDatabaseId(d->mSelectTrackFromIdQuery.record().value(0).toULongLong());
    result.setAlbumName(internalAlbumDataFromId(d->mSelectTrackFromIdQuery.record().value(2).toULongLong(), DatabaseInterface::AlbumData::Title).toString());
    result.setArtist(d->mSelectTrackFromIdQuery.record().value(3).toString());
    result.setAlbumArtist(d->mSelectTrackFromIdQuery.record().value(4).toString());
    result.setDuration(QTime::fromMSecsSinceStartOfDay(d->mSelectTrackFromIdQuery.record().value(8).toLongLong()));
    result.setResourceURI(d->mSelectTrackFromIdQuery.record().value(5).toUrl());
    result.setAlbumCover(internalAlbumDataFromId(d->mSelectTrackFromIdQuery.record().value(2).toULongLong(), DatabaseInterface::AlbumData::Image).toUrl());
    result.setTitle(d->mSelectTrackFromIdQuery.record().value(1).toString());
    result.setTrackNumber(d->mSelectTrackFromIdQuery.record().value(6).toInt());
    result.setDiscNumber(d->mSelectTrackFromIdQuery.record().value(7).toInt());
    result.setValid(true);

    d->mSelectTrackFromIdQuery.finish();

    return result;
}

QVariant DatabaseInterface::internalAlbumDataFromId(qulonglong albumId, DatabaseInterface::AlbumData dataType) const
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

QVector<MusicAudioTrack> DatabaseInterface::internalTracksFromAuthor(QString artistName) const
{
    auto allTracks = QVector<MusicAudioTrack>();

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

        newTrack.setDatabaseId(d->mSelectTracksFromArtist.record().value(1).toULongLong());
        newTrack.setTitle(d->mSelectTracksFromArtist.record().value(0).toString());
        newTrack.setAlbumName(d->mSelectTracksFromArtist.record().value(8).toString());
        newTrack.setAlbumArtist(d->mSelectTracksFromArtist.record().value(9).toString());
        newTrack.setArtist(d->mSelectTracksFromArtist.record().value(2).toString());
        newTrack.setResourceURI(d->mSelectTracksFromArtist.record().value(3).toUrl());
        newTrack.setAlbumCover(d->mSelectTracksFromArtist.record().value(7).toUrl());
        newTrack.setTrackNumber(d->mSelectTracksFromArtist.record().value(4).toInt());
        newTrack.setDiscNumber(d->mSelectTracksFromArtist.record().value(5).toInt());
        newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(d->mSelectTracksFromArtist.record().value(6).toInt()));
        newTrack.setValid(true);

        allTracks.push_back(newTrack);
    }

    d->mSelectTracksFromArtist.finish();

    return allTracks;
}


#include "moc_databaseinterface.cpp"
