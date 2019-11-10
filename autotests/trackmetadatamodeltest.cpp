/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "models/trackmetadatamodel.h"

#include "databasetestdata.h"

#include <QAbstractItemModelTester>
#include <QDebug>

#include <QtTest>

class TrackMetadataModelTests: public QObject, public DatabaseTestData
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
    }

    void loadOneTrackData()
    {
        TrackMetadataModel myModel;
        QAbstractItemModelTester testModel(&myModel);

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
        QCOMPARE(myModel.rowCount(), 2);
    }

    void modifyTrackInDatabase()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addOneTrackWithoutAlbumArtist" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        musicDb.insertTracksList(MusicAudioTrack::tracksToListData(mNewTracks), mNewCovers);

        TrackMetadataModel myModel;
        QAbstractItemModelTester testModel(&myModel);

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
        QCOMPARE(myModel.rowCount(), 12);

        musicDb.trackHasStartedPlaying(QUrl::fromLocalFile(QStringLiteral("/$2")), QDateTime::currentDateTime());

        QCOMPARE(beginResetSpy.count(), 1);
        QCOMPARE(endResetSpy.count(), 1);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(beginRemovedRowsSpy.count(), 0);
        QCOMPARE(endRemovedRowsSpy.count(), 0);
        QCOMPARE(myModel.rowCount(), 12);

        musicDb.trackHasStartedPlaying(QUrl::fromLocalFile(QStringLiteral("/$1")), QDateTime::currentDateTime());

        QCOMPARE(beginResetSpy.count(), 2);
        QCOMPARE(endResetSpy.count(), 2);
        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(beginRemovedRowsSpy.count(), 0);
        QCOMPARE(endRemovedRowsSpy.count(), 0);
        QCOMPARE(myModel.rowCount(), 13);
    }
};

QTEST_GUILESS_MAIN(TrackMetadataModelTests)


#include "trackmetadatamodeltest.moc"
