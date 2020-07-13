/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "databasetestdata.h"

#include "musicaudiotrack.h"
#include "databaseinterface.h"
#include "models/albummodel.h"
#include <QAbstractItemModelTester>

#include <QObject>
#include <QUrl>
#include <QString>
#include <QHash>
#include <QVector>
#include <QThread>
#include <QStandardPaths>


#include <QtTest>

class AlbumModelTests: public QObject, public DatabaseTestData
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QList<MusicAudioTrack>>("QList<MusicAudioTrack>");
        qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
        qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
    }

    void removeOneTrack()
    {
        DatabaseInterface musicDb;
        AlbumModel albumsModel;
        QAbstractItemModelTester testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &albumsModel, &AlbumModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &albumsModel, &AlbumModel::trackRemoved);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &albumsModel, &AlbumModel::trackModified);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &AlbumModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &AlbumModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &AlbumModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &AlbumModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &AlbumModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.initialize(QStringLiteral("album1"), QStringLiteral("Various Artists"), nullptr);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.tracksAdded(musicDb.albumData(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"))));

        QCOMPARE(beginInsertRowsSpy.count(), 4);
        QCOMPARE(endInsertRowsSpy.count(), 4);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);

        auto firstTrack = musicDb.trackDataFromDatabaseId(trackId);

        musicDb.removeTracksList({firstTrack[DatabaseInterface::ResourceRole].toUrl()});

        QCOMPARE(beginInsertRowsSpy.count(), 4);
        QCOMPARE(endInsertRowsSpy.count(), 4);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
    }

    void removeOneAlbum()
    {
        DatabaseInterface musicDb;
        AlbumModel albumsModel;
        QAbstractItemModelTester testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &albumsModel, &AlbumModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &albumsModel, &AlbumModel::trackRemoved);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &albumsModel, &AlbumModel::trackModified);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &AlbumModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &AlbumModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &AlbumModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &AlbumModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &AlbumModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.initialize(QStringLiteral("album3"), QStringLiteral("artist2"), nullptr);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.tracksAdded(musicDb.albumData(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist2"))));

        QCOMPARE(beginInsertRowsSpy.count(), 3);
        QCOMPARE(endInsertRowsSpy.count(), 3);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

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

        QCOMPARE(beginInsertRowsSpy.count(), 3);
        QCOMPARE(endInsertRowsSpy.count(), 3);
        QCOMPARE(beginRemoveRowsSpy.count(), 3);
        QCOMPARE(endRemoveRowsSpy.count(), 3);
        QCOMPARE(dataChangedSpy.count(), 0);
    }

    void addOneTrack()
    {
        DatabaseInterface musicDb;
        AlbumModel albumsModel;
        QAbstractItemModelTester testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &albumsModel, &AlbumModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &albumsModel, &AlbumModel::trackRemoved);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &albumsModel, &AlbumModel::trackModified);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &AlbumModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &AlbumModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &AlbumModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &AlbumModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &AlbumModel::dataChanged);

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

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.initialize(QStringLiteral("album1"), QStringLiteral("Various Artists"), nullptr);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.tracksAdded(musicDb.albumData(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"))));

        QCOMPARE(beginInsertRowsSpy.count(), 4);
        QCOMPARE(endInsertRowsSpy.count(), 4);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 6, 1,
                QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album1")), 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newFiles2 = QList<QUrl>();
        for (const auto &oneTrack : newTracks) {
            newFiles2.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(newTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 5);
        QCOMPARE(endInsertRowsSpy.count(), 5);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 4);

        QCOMPARE(beginInsertRowsSpy.at(4).at(1).toInt(), 4);
        QCOMPARE(beginInsertRowsSpy.at(4).at(2).toInt(), 4);

        QCOMPARE(albumsModel.data(albumsModel.index(4, 0), DatabaseInterface::ColumnsRoles::TitleRole).toString(), QStringLiteral("track6"));

        auto secondNewTrack = MusicAudioTrack{true, QStringLiteral("$24"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 5, 1,
                QTime::fromMSecsSinceStartOfDay(24), {QUrl::fromLocalFile(QStringLiteral("/$24"))},
                QDateTime::fromMSecsSinceEpoch(24),
                QUrl::fromLocalFile(QStringLiteral("album1")), 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto secondNewTracks = QList<MusicAudioTrack>();
        secondNewTracks.push_back(secondNewTrack);

        auto newFiles3 = QList<QUrl>();
        for (const auto &oneTrack : secondNewTracks) {
            newFiles3.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(secondNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 6);
        QCOMPARE(endInsertRowsSpy.count(), 6);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 9);

        QCOMPARE(beginInsertRowsSpy.at(5).at(1).toInt(), 4);
        QCOMPARE(beginInsertRowsSpy.at(5).at(2).toInt(), 4);

        QCOMPARE(albumsModel.data(albumsModel.index(4, 0), DatabaseInterface::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
    }

    void modifyOneTrack()
    {
        DatabaseInterface musicDb;
        AlbumModel albumsModel;
        QAbstractItemModelTester testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &albumsModel, &AlbumModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &albumsModel, &AlbumModel::trackRemoved);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &albumsModel, &AlbumModel::trackModified);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &AlbumModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &AlbumModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &AlbumModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &AlbumModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &AlbumModel::dataChanged);

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

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.initialize(QStringLiteral("album1"), QStringLiteral("Various Artists"), nullptr);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.tracksAdded(musicDb.albumData(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"))));

        QCOMPARE(beginInsertRowsSpy.count(), 4);
        QCOMPARE(endInsertRowsSpy.count(), 4);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto modifiedTrack = MusicAudioTrack{
                true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 5, 3,
                QTime::fromMSecsSinceStartOfDay(3), {QUrl::fromLocalFile(QStringLiteral("/$3"))},
                QDateTime::fromMSecsSinceEpoch(23),
        QUrl::fromLocalFile(QStringLiteral("album1")), 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};

        musicDb.modifyTracksList({modifiedTrack}, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(beginInsertRowsSpy.count(), 4);
        QCOMPARE(endInsertRowsSpy.count(), 4);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 1);

        QCOMPARE(dataChangedSpy.at(0).at(0).toModelIndex(), albumsModel.index(2, 0));
        QCOMPARE(dataChangedSpy.at(0).at(1).toModelIndex(), albumsModel.index(2, 0));

        QCOMPARE(albumsModel.data(albumsModel.index(2, 0), DatabaseInterface::ColumnsRoles::TrackNumberRole).toInt(), 5);
    }
};

QTEST_GUILESS_MAIN(AlbumModelTests)


#include "albummodeltest.moc"
