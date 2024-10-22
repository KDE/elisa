/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "models/trackmetadatamodel.h"
#include "models/trackmetadataproxymodel.h"

#include "databasetestdata.h"

#include <QAbstractItemModelTester>
#include <QDebug>
#include <QTemporaryFile>

#include <QSignalSpy>
#include <QTest>

constexpr auto trackRowCount = 19;

class TrackMetadataModelTests: public QObject, public DatabaseTestData
{
    Q_OBJECT

public:

    explicit TrackMetadataModelTests(QObject *aParent = nullptr) : QObject(aParent)
    {
    }

private Q_SLOTS:

    void initTestCase()
    {
    }

    void loadOneTrackData()
    {
        TrackMetadataModel myModel;
        TrackMetadataProxyModel proxyModel;
        proxyModel.setSourceModel(&myModel);
        QAbstractItemModelTester testModel(&myModel);
        QAbstractItemModelTester testProxyModel(&proxyModel);

        QSignalSpy beginResetSpy(&myModel, &TrackMetadataModel::modelAboutToBeReset);
        QSignalSpy endResetSpy(&myModel, &TrackMetadataModel::modelReset);
        QSignalSpy beginInsertRowsSpy(&myModel, &TrackMetadataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&myModel, &TrackMetadataModel::rowsInserted);
        QSignalSpy dataChangedSpy(&myModel, &TrackMetadataModel::dataChanged);
        QSignalSpy beginRemovedRowsSpy(&myModel, &TrackMetadataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemovedRowsSpy(&myModel, &TrackMetadataModel::rowsRemoved);

        QCOMPARE(beginResetSpy.count(), 0);
        QCOMPARE(endResetSpy.count(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(beginRemovedRowsSpy.count(), 0);
        QCOMPARE(endRemovedRowsSpy.count(), 0);
        QCOMPARE(myModel.rowCount(), 0);
        QCOMPARE(proxyModel.rowCount(), 0);

        auto trackData = TrackMetadataModel::TrackDataType{{DataTypes::DatabaseIdRole, 1},
                                                           {DataTypes::TitleRole, QStringLiteral("title")},
                                                           {DataTypes::AlbumRole, QStringLiteral("album")}};
        myModel.trackData(trackData);

        QCOMPARE(beginResetSpy.count(), 1);
        QCOMPARE(endResetSpy.count(), 1);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(beginRemovedRowsSpy.count(), 0);
        QCOMPARE(endRemovedRowsSpy.count(), 0);
        QCOMPARE(myModel.rowCount(), trackRowCount);
        QCOMPARE(proxyModel.rowCount(), 2);

        proxyModel.setShowTagsWithNoData(true);
        QCOMPARE(proxyModel.rowCount(), trackRowCount);
    }

    void modifyTrackInDatabase()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addOneTrackWithoutAlbumArtist" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        TrackMetadataModel myModel;
        TrackMetadataProxyModel proxyModel;
        proxyModel.setSourceModel(&myModel);
        QAbstractItemModelTester testModel(&myModel);
        QAbstractItemModelTester testProxyModel(&proxyModel);

        QSignalSpy beginResetSpy(&myModel, &TrackMetadataModel::modelAboutToBeReset);
        QSignalSpy endResetSpy(&myModel, &TrackMetadataModel::modelReset);
        QSignalSpy beginInsertRowsSpy(&myModel, &TrackMetadataModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&myModel, &TrackMetadataModel::rowsInserted);
        QSignalSpy dataChangedSpy(&myModel, &TrackMetadataModel::dataChanged);
        QSignalSpy beginRemovedRowsSpy(&myModel, &TrackMetadataModel::rowsAboutToBeRemoved);
        QSignalSpy endRemovedRowsSpy(&myModel, &TrackMetadataModel::rowsRemoved);

        QCOMPARE(beginResetSpy.count(), 0);
        QCOMPARE(endResetSpy.count(), 0);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(beginRemovedRowsSpy.count(), 0);
        QCOMPARE(endRemovedRowsSpy.count(), 0);
        QCOMPARE(myModel.rowCount(), 0);
        QCOMPARE(proxyModel.rowCount(), 0);

        myModel.setDatabase(&musicDb);

        auto trackId = musicDb.trackIdFromFileName(QUrl::fromLocalFile(QStringLiteral("/$1")));

        myModel.initializeByIdAndUrl(ElisaUtils::Track, trackId, QUrl::fromLocalFile(QStringLiteral("/$1")));

        QCOMPARE(beginResetSpy.count(), 1);
        QCOMPARE(endResetSpy.count(), 1);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(beginRemovedRowsSpy.count(), 0);
        QCOMPARE(endRemovedRowsSpy.count(), 0);
        QCOMPARE(myModel.rowCount(), trackRowCount);
        QCOMPARE(proxyModel.rowCount(), 12);

        proxyModel.setShowTagsWithNoData(true);
        QCOMPARE(proxyModel.rowCount(), trackRowCount);
        proxyModel.setShowTagsWithNoData(false);

        musicDb.trackHasStartedPlaying(QUrl::fromLocalFile(QStringLiteral("/$2")), QDateTime::currentDateTime());

        QCOMPARE(beginResetSpy.count(), 1);
        QCOMPARE(endResetSpy.count(), 1);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(beginRemovedRowsSpy.count(), 0);
        QCOMPARE(endRemovedRowsSpy.count(), 0);
        QCOMPARE(myModel.rowCount(), trackRowCount);
        QCOMPARE(proxyModel.rowCount(), 12);

        proxyModel.setShowTagsWithNoData(true);
        QCOMPARE(proxyModel.rowCount(), trackRowCount);
    }
};

QTEST_GUILESS_MAIN(TrackMetadataModelTests)


#include "trackmetadatamodeltest.moc"
