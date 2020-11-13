/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "databasetestdata.h"

#include "databaseinterface.h"
#include "datatypes.h"
#include "models/datamodel.h"

#include <QObject>
#include <QUrl>
#include <QString>
#include <QHash>
#include <QVector>
#include <QThread>
#include <QStandardPaths>
#include <QAbstractItemModelTester>

#include <QDebug>

#include <QtTest>

class DataModelTests: public QObject, public DatabaseTestData
{
    Q_OBJECT

public:

    explicit DataModelTests(QObject *aParent = nullptr) : QObject(aParent)
    {
    }

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
        qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
    }

    void removeOneTrack()
    {
        DatabaseInterface musicDb;
        DataModel albumsModel;
        QAbstractItemModelTester testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &albumsModel, &DataModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &albumsModel, &DataModel::trackRemoved);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &albumsModel, &DataModel::trackModified);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &DataModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.initialize(nullptr, nullptr, ElisaUtils::Track, ElisaUtils::FilterById, {}, {},
                               musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/")), {});

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.tracksAdded(musicDb.albumData(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/"))));

        QCOMPARE(albumsModel.rowCount(), 4);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);

        auto firstTrack = musicDb.trackDataFromDatabaseId(trackId);

        musicDb.removeTracksList({firstTrack[DataTypes::ResourceRole].toUrl()});

        QCOMPARE(albumsModel.rowCount(), 3);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
    }

    void removeOneAlbum()
    {
        DatabaseInterface musicDb;
        DataModel albumsModel;
        QAbstractItemModelTester testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &albumsModel, &DataModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &albumsModel, &DataModel::trackRemoved);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &albumsModel, &DataModel::trackModified);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &DataModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.initialize(nullptr, nullptr, ElisaUtils::Track, ElisaUtils::FilterById, {}, {},
                               musicDb.albumIdFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist2"), QStringLiteral("/")), {});

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.tracksAdded(musicDb.albumData(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist2"), QStringLiteral("/"))));

        QCOMPARE(albumsModel.rowCount(), 3);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
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

        musicDb.removeTracksList({firstTrack[DataTypes::ResourceRole].toUrl(),
                                  secondTrack[DataTypes::ResourceRole].toUrl(),
                                  thirdTrack[DataTypes::ResourceRole].toUrl()});

        QCOMPARE(albumsModel.rowCount(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 3);
        QCOMPARE(endRemoveRowsSpy.count(), 3);
        QCOMPARE(dataChangedSpy.count(), 0);
    }

    void addOneTrack()
    {
        DatabaseInterface musicDb;
        DataModel albumsModel;
        QAbstractItemModelTester testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &albumsModel, &DataModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &albumsModel, &DataModel::trackRemoved);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &albumsModel, &DataModel::trackModified);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &DataModel::dataChanged);

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

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.initialize(nullptr, nullptr, ElisaUtils::Track, ElisaUtils::FilterById, {}, {},
                               musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/")), {});

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.tracksAdded(musicDb.albumData(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/"))));

        QCOMPARE(albumsModel.rowCount(), 4);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 6, 1,
                QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album1")), 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newFiles2 = QList<QUrl>();
        for (const auto &oneTrack : newTracks) {
            newFiles2.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(newTracks, mNewCovers);

        QCOMPARE(albumsModel.rowCount(), 5);
        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(beginInsertRowsSpy.at(1).at(1).toInt(), 4);
        QCOMPARE(beginInsertRowsSpy.at(1).at(2).toInt(), 4);

        QCOMPARE(albumsModel.data(albumsModel.index(4, 0), DataTypes::ColumnsRoles::TitleRole).toString(), QStringLiteral("track6"));

        auto secondNewTrack = DataTypes::TrackDataType{true, QStringLiteral("$24"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 5, 1,
                QTime::fromMSecsSinceStartOfDay(24), {QUrl::fromLocalFile(QStringLiteral("/$24"))},
                QDateTime::fromMSecsSinceEpoch(24),
                QUrl::fromLocalFile(QStringLiteral("album1")), 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto secondNewTracks = DataTypes::ListTrackDataType();
        secondNewTracks.push_back(secondNewTrack);

        auto newFiles3 = QList<QUrl>();
        for (const auto &oneTrack : secondNewTracks) {
            newFiles3.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(secondNewTracks, mNewCovers);

        QCOMPARE(albumsModel.rowCount(), 6);
        QCOMPARE(beginInsertRowsSpy.count(), 3);
        QCOMPARE(endInsertRowsSpy.count(), 3);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(beginInsertRowsSpy.at(2).at(1).toInt(), 4);
        QCOMPARE(beginInsertRowsSpy.at(2).at(2).toInt(), 4);

        QCOMPARE(albumsModel.data(albumsModel.index(4, 0), DataTypes::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
    }

    void modifyOneTrack()
    {
        DatabaseInterface musicDb;
        DataModel albumsModel;
        QAbstractItemModelTester testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &albumsModel, &DataModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &albumsModel, &DataModel::trackRemoved);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &albumsModel, &DataModel::trackModified);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &DataModel::dataChanged);

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

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.initialize(nullptr, nullptr, ElisaUtils::Track, ElisaUtils::FilterById, {}, {},
                               musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/")), {});

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.tracksAdded(musicDb.albumData(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/"))));

        QCOMPARE(albumsModel.rowCount(), 4);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto modifiedTrack = DataTypes::TrackDataType{
                true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 5, 3,
                QTime::fromMSecsSinceStartOfDay(3), {QUrl::fromLocalFile(QStringLiteral("/$3"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album1")), 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};

        musicDb.insertTracksList({modifiedTrack}, mNewCovers);

        QCOMPARE(albumsModel.rowCount(), 4);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 1);

        QCOMPARE(dataChangedSpy.at(0).at(0).toModelIndex(), albumsModel.index(2, 0));
        QCOMPARE(dataChangedSpy.at(0).at(1).toModelIndex(), albumsModel.index(2, 0));

        QCOMPARE(albumsModel.data(albumsModel.index(2, 0), DataTypes::ColumnsRoles::TrackNumberRole).toInt(), 5);
    }


    void removeOneTrackAllTracks()
    {
        DatabaseInterface musicDb;
        DataModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &tracksModel, &DataModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &tracksModel, &DataModel::trackModified);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &tracksModel, &DataModel::trackRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&tracksModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &DataModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        tracksModel.initialize(nullptr, nullptr, ElisaUtils::Track,  ElisaUtils::NoFilter, {}, {}, 0, {});

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 23);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                    QStringLiteral("album1"), 1, 1);

        auto firstTrack = musicDb.trackDataFromDatabaseId(trackId);

        musicDb.removeTracksList({firstTrack[DataTypes::ResourceRole].toUrl()});

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 22);
    }

    void removeOneAlbumAllTracks()
    {
        DatabaseInterface musicDb;
        DataModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &tracksModel, &DataModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &tracksModel, &DataModel::trackModified);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &tracksModel, &DataModel::trackRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&tracksModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &DataModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        tracksModel.initialize(nullptr, nullptr, ElisaUtils::Track, ElisaUtils::NoFilter, {}, {}, 0, {});

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 23);

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 1, 1);
        auto firstTrack = musicDb.trackDataFromDatabaseId(firstTrackId);
        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                          QStringLiteral("album3"), 2, 1);
        auto secondTrack = musicDb.trackDataFromDatabaseId(secondTrackId);
        auto thirdTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 3, 1);
        auto thirdTrack = musicDb.trackDataFromDatabaseId(thirdTrackId);

        musicDb.removeTracksList({firstTrack[DataTypes::ResourceRole].toUrl(),
                                  secondTrack[DataTypes::ResourceRole].toUrl(),
                                  thirdTrack[DataTypes::ResourceRole].toUrl()});

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 3);
        QCOMPARE(endRemoveRowsSpy.count(), 3);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 20);
    }

    void addOneTrackAllTracks()
    {
        DatabaseInterface musicDb;
        DataModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &tracksModel, &DataModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &tracksModel, &DataModel::trackModified);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &tracksModel, &DataModel::trackRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&tracksModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &DataModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        tracksModel.initialize(nullptr, nullptr, ElisaUtils::Track, ElisaUtils::NoFilter, {}, {}, 0, {});

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 23);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track23"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 23, 1, QTime::fromMSecsSinceStartOfDay(23),
        {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("file://image$23"))}, 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        musicDb.insertTracksList(newTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 24);
    }

    void addOneAlbumAllTracks()
    {
        DatabaseInterface musicDb;
        DataModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &tracksModel, &DataModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &tracksModel, &DataModel::trackModified);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &tracksModel, &DataModel::trackRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&tracksModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &DataModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        tracksModel.initialize(nullptr, nullptr, ElisaUtils::Track, ElisaUtils::NoFilter, {}, {}, 0, {});

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

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 23);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track23"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 23, 1, QTime::fromMSecsSinceStartOfDay(23),
        {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("file://image$23"))}, 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCover = QUrl::fromLocalFile(QStringLiteral("album5"));
        auto newCovers = QHash<QString, QUrl>();
        newCovers[QStringLiteral("album5")] = newCover;

        auto newFiles2 = QList<QUrl>();
        for (const auto &oneTrack : newTracks) {
            newFiles2.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(newTracks, newCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 24);
    }

    void modifyOneTrackAllTracks()
    {
        DatabaseInterface musicDb;
        DataModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &tracksModel, &DataModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &tracksModel, &DataModel::trackModified);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &tracksModel, &DataModel::trackRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&tracksModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &DataModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        tracksModel.initialize(nullptr, nullptr, ElisaUtils::Track, ElisaUtils::NoFilter, {}, {}, 0, {});

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 23);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
                1, 1, QTime::fromMSecsSinceStartOfDay(1), {QUrl::fromLocalFile(QStringLiteral("/$1"))},
                QDateTime::fromMSecsSinceEpoch(1),
        {QUrl::fromLocalFile(QStringLiteral("file://image$1"))}, 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        musicDb.insertTracksList(newTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 1);

        QCOMPARE(tracksModel.rowCount(), 23);

        const auto &dataChangedSignal =  dataChangedSpy.constFirst();

        QCOMPARE(dataChangedSignal.size(), 3);

        auto changedIndex = dataChangedSignal.constFirst().toModelIndex();

        QCOMPARE(tracksModel.data(changedIndex, DataTypes::ColumnsRoles::RatingRole).isValid(), true);
        QCOMPARE(tracksModel.data(changedIndex, DataTypes::ColumnsRoles::RatingRole).toInt(), 5);
    }

    void addEmptyTracksListAllTracks()
    {
        DataModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        auto newTracks = DataModel::ListTrackDataType{};

        QSignalSpy beginInsertRowsSpy(&tracksModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &DataModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        tracksModel.tracksAdded(newTracks);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 0);
    }

    void addTracksListTwiceAllTracks()
    {
        DatabaseInterface musicDb;
        DataModel tracksModel;
        QAbstractItemModelTester testModel(&tracksModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &tracksModel, &DataModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &tracksModel, &DataModel::trackModified);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &tracksModel, &DataModel::trackRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&tracksModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&tracksModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&tracksModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&tracksModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&tracksModel, &DataModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        tracksModel.initialize(nullptr, nullptr, ElisaUtils::Track, ElisaUtils::NoFilter, {}, {}, 0, {});

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 23);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track23"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 23, 1, QTime::fromMSecsSinceStartOfDay(23),
        {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("file://image$23"))}, 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};

        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        musicDb.insertTracksList(newTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(tracksModel.rowCount(), 24);
    }

    void removeOneTrackAllAlbums()
    {
        DatabaseInterface musicDb;
        DataModel albumsModel;
        QAbstractItemModelTester testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::albumsAdded,
                &albumsModel, &DataModel::albumsAdded);
        connect(&musicDb, &DatabaseInterface::albumModified,
                &albumsModel, &DataModel::albumModified);
        connect(&musicDb, &DatabaseInterface::albumRemoved,
                &albumsModel, &DataModel::albumRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &DataModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.initialize(nullptr, nullptr, ElisaUtils::Album, ElisaUtils::NoFilter, {}, {}, 0, {});

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(albumsModel.rowCount(), 5);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);

        auto firstTrack = musicDb.trackDataFromDatabaseId(trackId);

        QVERIFY(!firstTrack.isEmpty());

        musicDb.removeTracksList({firstTrack[DataTypes::ResourceRole].toUrl()});

        QCOMPARE(albumsModel.rowCount(), 5);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 1);
    }

    void removeOneAlbumAllAlbums()
    {
        DatabaseInterface musicDb;
        DataModel albumsModel;
        QAbstractItemModelTester testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::albumsAdded,
                &albumsModel, &DataModel::albumsAdded);
        connect(&musicDb, &DatabaseInterface::albumModified,
                &albumsModel, &DataModel::albumModified);
        connect(&musicDb, &DatabaseInterface::albumRemoved,
                &albumsModel, &DataModel::albumRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &DataModel::dataChanged);

        QCOMPARE(albumsModel.rowCount(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.initialize(nullptr, nullptr, ElisaUtils::Album, ElisaUtils::NoFilter, {}, {}, 0, {});

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(albumsModel.rowCount(), 5);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
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

        musicDb.removeTracksList({firstTrack[DataTypes::ResourceRole].toUrl(),
                                  secondTrack[DataTypes::ResourceRole].toUrl(),
                                  thirdTrack[DataTypes::ResourceRole].toUrl()});

        QCOMPARE(albumsModel.rowCount(), 4);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
    }

    void addOneTrackAllAlbums()
    {
        DatabaseInterface musicDb;
        DataModel albumsModel;
        QAbstractItemModelTester testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::albumsAdded,
                &albumsModel, &DataModel::albumsAdded);
        connect(&musicDb, &DatabaseInterface::albumModified,
                &albumsModel, &DataModel::albumModified);
        connect(&musicDb, &DatabaseInterface::albumRemoved,
                &albumsModel, &DataModel::albumRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &DataModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.initialize(nullptr, nullptr, ElisaUtils::Album, ElisaUtils::NoFilter, {}, {}, 0, {});

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track23"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 23, 1, QTime::fromMSecsSinceStartOfDay(23),
        {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("file://image$23"))}, 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        musicDb.insertTracksList(newTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 1);
    }

    void addOneAlbumAllAlbums()
    {
        DatabaseInterface musicDb;
        DataModel albumsModel;
        QAbstractItemModelTester testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::albumsAdded,
                &albumsModel, &DataModel::albumsAdded);
        connect(&musicDb, &DatabaseInterface::albumModified,
                &albumsModel, &DataModel::albumModified);
        connect(&musicDb, &DatabaseInterface::albumRemoved,
                &albumsModel, &DataModel::albumRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &DataModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.initialize(nullptr, nullptr, ElisaUtils::Album, ElisaUtils::NoFilter, {}, {}, 0, {});

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

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album5"), QStringLiteral("artist2"), 1, 1,
                QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("file://image$23"))}, 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCover = QUrl::fromLocalFile(QStringLiteral("album5"));
        auto newCovers = QHash<QString, QUrl>();
        newCovers[QStringLiteral("album5")] = newCover;

        musicDb.insertTracksList(newTracks, newCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
    }

    void removeOneArtistAllArtists()
    {
        DatabaseInterface musicDb;
        DataModel artistsModel;
        QAbstractItemModelTester testModel(&artistsModel);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&artistsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&artistsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&artistsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&artistsModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&artistsModel, &DataModel::dataChanged);

        QCOMPARE(artistsModel.rowCount(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        artistsModel.initialize(nullptr, &musicDb, ElisaUtils::Artist, ElisaUtils::NoFilter, {}, {}, 0, {});

        QCOMPARE(artistsModel.rowCount(), 0);
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

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        beginInsertRowsSpy.wait();

        QCOMPARE(artistsModel.rowCount(), 7);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), 6, 1);

        auto firstTrack = musicDb.trackDataFromDatabaseId(trackId);

        musicDb.removeTracksList({firstTrack[DataTypes::ResourceRole].toUrl()});

        beginRemoveRowsSpy.wait();

        QCOMPARE(artistsModel.rowCount(), 6);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
    }

    void addOneArtistAllArtists()
    {
        DatabaseInterface musicDb;
        DataModel artistsModel;
        QAbstractItemModelTester testModel(&artistsModel);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&artistsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&artistsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&artistsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&artistsModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&artistsModel, &DataModel::dataChanged);

        QCOMPARE(artistsModel.rowCount(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        artistsModel.initialize(nullptr, &musicDb, ElisaUtils::Artist, ElisaUtils::NoFilter, {}, {}, 0, {});

        QCOMPARE(artistsModel.rowCount(), 0);
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

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        beginInsertRowsSpy.wait();

        QCOMPARE(artistsModel.rowCount(), 7);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track23"),
                QStringLiteral("artist6"), QStringLiteral("album4"), QStringLiteral("Various Artists"), 23, 1, QTime::fromMSecsSinceStartOfDay(23),
        {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("file://image$23"))}, 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newFiles2 = QList<QUrl>();
        for (const auto &oneTrack : newTracks) {
            newFiles2.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(newTracks, mNewCovers);

        beginInsertRowsSpy.wait();

        QCOMPARE(artistsModel.rowCount(), 8);
        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
    }

    void displayAlbumsFromOneArtist()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "displayAlbumsFromOneArtist" << databaseFile.fileName();

        DatabaseInterface musicDb;
        DataModel artistsModel;
        QAbstractItemModelTester testModel(&artistsModel);

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy beginInsertRowsSpy(&artistsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&artistsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&artistsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&artistsModel, &DataModel::rowsRemoved);
        QSignalSpy modelAboutToBeResetSpy(&artistsModel, &DataModel::modelAboutToBeReset);
        QSignalSpy modelResetSpy(&artistsModel, &DataModel::modelReset);
        QSignalSpy dataChangedSpy(&artistsModel, &DataModel::dataChanged);

        QCOMPARE(artistsModel.rowCount(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(modelAboutToBeResetSpy.count(), 0);
        QCOMPARE(modelResetSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        artistsModel.initialize(nullptr, &musicDb, ElisaUtils::Album, ElisaUtils::FilterByArtist, {}, QStringLiteral("artist1"), 0, {});

        QCOMPARE(artistsModel.rowCount(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(modelAboutToBeResetSpy.count(), 0);
        QCOMPARE(modelResetSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto newFiles = QList<QUrl>();
        const auto &constNewTracks = mNewTracks;
        for (const auto &oneTrack : constNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        beginInsertRowsSpy.wait();

        QCOMPARE(artistsModel.rowCount(), 1);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(modelAboutToBeResetSpy.count(), 0);
        QCOMPARE(modelResetSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
    }

    void displayArtistsFromOneGenre()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "displayArtistsFromOneGenre" << databaseFile.fileName();

        DatabaseInterface musicDb;
        DataModel artistsModel;
        QAbstractItemModelTester testModel(&artistsModel);

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy beginInsertRowsSpy(&artistsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&artistsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&artistsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&artistsModel, &DataModel::rowsRemoved);
        QSignalSpy modelAboutToBeResetSpy(&artistsModel, &DataModel::modelAboutToBeReset);
        QSignalSpy modelResetSpy(&artistsModel, &DataModel::modelReset);
        QSignalSpy dataChangedSpy(&artistsModel, &DataModel::dataChanged);

        QCOMPARE(artistsModel.rowCount(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(modelAboutToBeResetSpy.count(), 0);
        QCOMPARE(modelResetSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        artistsModel.initialize(nullptr, &musicDb, ElisaUtils::Artist, ElisaUtils::FilterByGenre, QStringLiteral("genre1"), {}, 0, {});

        QCOMPARE(artistsModel.rowCount(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(modelAboutToBeResetSpy.count(), 0);
        QCOMPARE(modelResetSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto newFiles = QList<QUrl>();
        const auto &constNewTracks = mNewTracks;
        for (const auto &oneTrack : constNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        beginInsertRowsSpy.wait();

        QCOMPARE(artistsModel.rowCount(), 4);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(modelAboutToBeResetSpy.count(), 0);
        QCOMPARE(modelResetSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
    }

    void displayAlbumsFromOneArtistAndGenre()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "displayAlbumsFromOneArtistAndGenre" << databaseFile.fileName();

        DatabaseInterface musicDb;
        DataModel artistsModel;
        QAbstractItemModelTester testModel(&artistsModel);

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy beginInsertRowsSpy(&artistsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&artistsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&artistsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&artistsModel, &DataModel::rowsRemoved);
        QSignalSpy modelAboutToBeResetSpy(&artistsModel, &DataModel::modelAboutToBeReset);
        QSignalSpy modelResetSpy(&artistsModel, &DataModel::modelReset);
        QSignalSpy dataChangedSpy(&artistsModel, &DataModel::dataChanged);

        QCOMPARE(artistsModel.rowCount(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(modelAboutToBeResetSpy.count(), 0);
        QCOMPARE(modelResetSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        artistsModel.initialize(nullptr, &musicDb, ElisaUtils::Album, ElisaUtils::FilterByGenreAndArtist,
                                QStringLiteral("genre3"), QStringLiteral("artist2"), 0, {});

        QCOMPARE(artistsModel.rowCount(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(modelAboutToBeResetSpy.count(), 0);
        QCOMPARE(modelResetSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto newFiles = QList<QUrl>();
        const auto &constNewTracks = mNewTracks;
        for (const auto &oneTrack : constNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        beginInsertRowsSpy.wait();

        QCOMPARE(artistsModel.rowCount(), 2);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(modelAboutToBeResetSpy.count(), 0);
        QCOMPARE(modelResetSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
    }

    void displayTracksFromOneAlbum()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "displayTracksFromOneAlbum" << databaseFile.fileName();

        DatabaseInterface musicDb;
        DataModel artistsModel;
        QAbstractItemModelTester testModel(&artistsModel);

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy beginInsertRowsSpy(&artistsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&artistsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&artistsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&artistsModel, &DataModel::rowsRemoved);
        QSignalSpy modelAboutToBeResetSpy(&artistsModel, &DataModel::modelAboutToBeReset);
        QSignalSpy modelResetSpy(&artistsModel, &DataModel::modelReset);
        QSignalSpy dataChangedSpy(&artistsModel, &DataModel::dataChanged);

        QCOMPARE(artistsModel.rowCount(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(modelAboutToBeResetSpy.count(), 0);
        QCOMPARE(modelResetSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(artistsModel.rowCount(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(modelAboutToBeResetSpy.count(), 0);
        QCOMPARE(modelResetSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto albumId = musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"),
                                                         QStringLiteral("Various Artists"),
                                                         QStringLiteral("/"));

        QVERIFY(albumId != 0);

        artistsModel.initialize(nullptr, &musicDb, ElisaUtils::Track, ElisaUtils::FilterById, {}, {}, albumId, {});

        beginInsertRowsSpy.wait();

        QCOMPARE(artistsModel.rowCount(), 4);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(modelAboutToBeResetSpy.count(), 0);
        QCOMPARE(modelResetSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType();

        newTracks = {{true, QStringLiteral("$31"), QStringLiteral("0"), QStringLiteral("track12"),
                      QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
                      12, 1, QTime::fromMSecsSinceStartOfDay(31),
                      {QUrl::fromLocalFile(QStringLiteral("/$31"))},
                      QDateTime::fromMSecsSinceEpoch(31),
                      {QUrl::fromLocalFile(QStringLiteral("album1"))}, 5, true,
                      QStringLiteral("genre4"), QStringLiteral("composer4"), QStringLiteral("lyricist4"), true},
                     {true, QStringLiteral("$32"), QStringLiteral("0"), QStringLiteral("track13"),
                      QStringLiteral("artist3"), QStringLiteral("album3"), QStringLiteral("artist3"),
                      13, 1, QTime::fromMSecsSinceStartOfDay(32),
                      {QUrl::fromLocalFile(QStringLiteral("/$32"))},
                      QDateTime::fromMSecsSinceEpoch(32),
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 4, true,
                      QStringLiteral("genre3"), QStringLiteral("composer3"), QStringLiteral("lyricist3"), false}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$31")] = QUrl::fromLocalFile(QStringLiteral("album1"));
        newCovers[QStringLiteral("file:///$32")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        beginInsertRowsSpy.wait();

        QCOMPARE(artistsModel.rowCount(), 5);
        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(modelAboutToBeResetSpy.count(), 0);
        QCOMPARE(modelResetSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
    }

    void addOneTrackWrongOrder()
    {
        DatabaseInterface musicDb;
        DataModel albumsModel;
        QAbstractItemModelTester testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::tracksAdded,
                &albumsModel, &DataModel::tracksAdded);
        connect(&musicDb, &DatabaseInterface::trackRemoved,
                &albumsModel, &DataModel::trackRemoved);
        connect(&musicDb, &DatabaseInterface::trackModified,
                &albumsModel, &DataModel::trackModified);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &DataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &DataModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &DataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &DataModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &DataModel::dataChanged);

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

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.initialize(nullptr, nullptr, ElisaUtils::Track, ElisaUtils::FilterById, {}, {},
                               musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/")), {});

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        albumsModel.tracksAdded(musicDb.albumData(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/"))));

        QCOMPARE(albumsModel.rowCount(), 4);
        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 2, 1,
                QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album1")), 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newFiles2 = QList<QUrl>();
        for (const auto &oneTrack : newTracks) {
            newFiles2.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(newTracks, mNewCovers);

        QCOMPARE(albumsModel.rowCount(), 5);
        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        QCOMPARE(beginInsertRowsSpy.at(1).at(1).toInt(), 2);
        QCOMPARE(beginInsertRowsSpy.at(1).at(2).toInt(), 2);
    }
};

QTEST_GUILESS_MAIN(DataModelTests)


#include "datamodeltest.moc"
