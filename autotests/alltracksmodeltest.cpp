/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "databasetestdata.h"

#include "musicalbum.h"
#include "musicaudiotrack.h"
#include "databaseinterface.h"
#include "models/alltracksmodel.h"
#include "qabstractitemmodeltester.h"

#include <QObject>
#include <QUrl>
#include <QString>
#include <QHash>
#include <QVector>
#include <QThread>
#include <QMetaObject>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

#include <QDebug>

#include <QtTest>

class AllTracksModelTests: public QObject, public DatabaseTestData
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>");
        qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
        qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
    }

    void removeOneTrack()
    {
        DatabaseInterface musicDb;
        AllTracksModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &tracksModel, &AllTracksModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &tracksModel, &AllTracksModel::trackModified);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &tracksModel, &AllTracksModel::trackRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &AllTracksModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &AllTracksModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 22);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                    QStringLiteral("album1"), 1, 1);

        auto firstTrack = musicDb.trackDataFromDatabaseId(trackId);

        musicDb.removeTracksList({firstTrack[DatabaseInterface::ResourceRole].toUrl()});

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 21);
    }

    void removeOneAlbum()
    {
        DatabaseInterface musicDb;
        AllTracksModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &tracksModel, &AllTracksModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &tracksModel, &AllTracksModel::trackModified);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &tracksModel, &AllTracksModel::trackRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &AllTracksModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &AllTracksModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 22);

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 1, 1);
        auto firstTrack = musicDb.trackDataFromDatabaseId(firstTrackId);
        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                          QStringLiteral("album3"), 2, 1);
        auto secondTrack = musicDb.trackDataFromDatabaseId(secondTrackId);
        auto thirdTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 3, 1);
        auto thirdTrack = musicDb.trackDataFromDatabaseId(thirdTrackId);

        musicDb.removeTracksList({firstTrack[DatabaseInterface::ResourceRole].toUrl(),
                                  secondTrack[DatabaseInterface::ResourceRole].toUrl(),
                                  thirdTrack[DatabaseInterface::ResourceRole].toUrl()});

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 3);
        QCOMPARE(endRemoveRowsSpy.count(), 3);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 19);
    }

    void addOneTrack()
    {
        DatabaseInterface musicDb;
        AllTracksModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &tracksModel, &AllTracksModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &tracksModel, &AllTracksModel::trackModified);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &tracksModel, &AllTracksModel::trackRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &AllTracksModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &AllTracksModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 22);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track23"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 23, 1, QTime::fromMSecsSinceStartOfDay(23),
        {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("file://image$23"))}, 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        musicDb.insertTracksList(newTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 5);

        QCOMPARE(tracksModel.rowCount(), 23);
    }

    void addOneAlbum()
    {
        DatabaseInterface musicDb;
        AllTracksModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &tracksModel, &AllTracksModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &tracksModel, &AllTracksModel::trackModified);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &tracksModel, &AllTracksModel::trackRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &AllTracksModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &AllTracksModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto newFiles = QList<QUrl>();
        const auto &constNewTracks = mNewTracks;
        for (const auto &oneTrack : constNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 22);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track23"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 23, 1, QTime::fromMSecsSinceStartOfDay(23),
        {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("file://image$23"))}, 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newCover = QUrl::fromLocalFile(QStringLiteral("album5"));
        auto newCovers = QHash<QString, QUrl>();
        newCovers[QStringLiteral("album5")] = newCover;

        auto newFiles2 = QList<QUrl>();
        for (const auto &oneTrack : newTracks) {
            newFiles2.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 5);

        QCOMPARE(tracksModel.rowCount(), 23);
    }

    void modifyOneTrack()
    {
        DatabaseInterface musicDb;
        AllTracksModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &tracksModel, &AllTracksModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &tracksModel, &AllTracksModel::trackModified);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &tracksModel, &AllTracksModel::trackRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &AllTracksModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &AllTracksModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 22);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
                1, 1, QTime::fromMSecsSinceStartOfDay(1), {QUrl::fromLocalFile(QStringLiteral("/$1"))},
                QDateTime::fromMSecsSinceEpoch(1),
        {QUrl::fromLocalFile(QStringLiteral("file://image$1"))}, 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        musicDb.insertTracksList(newTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 1);

        QCOMPARE(tracksModel.rowCount(), 22);

        const auto &dataChangedSignal =  dataChangedSpy.constFirst();

        QCOMPARE(dataChangedSignal.size(), 3);

        auto changedIndex = dataChangedSignal.constFirst().toModelIndex();

        QCOMPARE(tracksModel.data(changedIndex, DatabaseInterface::ColumnsRoles::RatingRole).isValid(), true);
        QCOMPARE(tracksModel.data(changedIndex, DatabaseInterface::ColumnsRoles::RatingRole).toInt(), 5);
    }

    void addEmptyTracksList()
    {
        AllTracksModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        auto newTracks = QList<MusicAudioTrack>();

        QSignalSpy beginInsertRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &AllTracksModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &AllTracksModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        //tracksModel.tracksAdded(newTracks);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 0);
    }

    void addTracksListTwice()
    {
        DatabaseInterface musicDb;
        AllTracksModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &tracksModel, &AllTracksModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &tracksModel, &AllTracksModel::trackModified);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &tracksModel, &AllTracksModel::trackRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &AllTracksModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &AllTracksModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &AllTracksModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 22);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track23"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 23, 1, QTime::fromMSecsSinceStartOfDay(23),
        {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("file://image$23"))}, 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};

        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        musicDb.insertTracksList(newTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 5);

        QCOMPARE(tracksModel.rowCount(), 23);
    }
};

QTEST_GUILESS_MAIN(AllTracksModelTests)


#include "alltracksmodeltest.moc"
