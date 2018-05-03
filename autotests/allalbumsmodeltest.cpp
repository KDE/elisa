/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "musicalbum.h"
#include "musicaudiotrack.h"
#include "databaseinterface.h"
#include "models/allalbumsmodel.h"
#include "modeltest.h"

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

class AllAlbumsModelTests: public QObject
{
    Q_OBJECT

private:

    QList<MusicAudioTrack> mNewTracks = {
        {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
         QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
         1, 1, QTime::fromMSecsSinceStartOfDay(1), {QUrl::fromLocalFile(QStringLiteral("/$1"))}, QDateTime::fromMSecsSinceEpoch(1),
         {QUrl::fromLocalFile(QStringLiteral("file://image$1"))}, 1, false,
         QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
         QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
         2, 2, QTime::fromMSecsSinceStartOfDay(2), {QUrl::fromLocalFile(QStringLiteral("/$2"))}, QDateTime::fromMSecsSinceEpoch(2),
         {QUrl::fromLocalFile(QStringLiteral("file://image$2"))}, 2, false,
         QStringLiteral("genre2"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
         QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
         3, 3, QTime::fromMSecsSinceStartOfDay(3), {QUrl::fromLocalFile(QStringLiteral("/$3"))}, QDateTime::fromMSecsSinceEpoch(3),
         {QUrl::fromLocalFile(QStringLiteral("file://image$3"))}, 3, false,
         QStringLiteral("genre3"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
         QStringLiteral("artist4"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
         4, 4, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("/$4"))}, QDateTime::fromMSecsSinceEpoch(4),
         {QUrl::fromLocalFile(QStringLiteral("file://image$4"))}, 4, false,
         QStringLiteral("genre4"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
         QStringLiteral("artist4"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
         4, 4, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("/$4Bis"))}, QDateTime::fromMSecsSinceEpoch(4),
         {QUrl::fromLocalFile(QStringLiteral("file://image$4"))}, 4, false,
         QStringLiteral("genre4"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"),
         1, 1, QTime::fromMSecsSinceStartOfDay(5), {QUrl::fromLocalFile(QStringLiteral("/$5"))}, QDateTime::fromMSecsSinceEpoch(5),
         {QUrl::fromLocalFile(QStringLiteral("file://image$5"))}, 4, true,
         QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"),
         2, 1, QTime::fromMSecsSinceStartOfDay(6), {QUrl::fromLocalFile(QStringLiteral("/$6"))}, QDateTime::fromMSecsSinceEpoch(6),
         {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 1, true,
         QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"),
         3, 1, QTime::fromMSecsSinceStartOfDay(7), {QUrl::fromLocalFile(QStringLiteral("/$7"))}, QDateTime::fromMSecsSinceEpoch(7),
         {QUrl::fromLocalFile(QStringLiteral("file://image$7"))}, 5, true,
         QStringLiteral("genre2"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"),
         4, 1, QTime::fromMSecsSinceStartOfDay(8), {QUrl::fromLocalFile(QStringLiteral("/$8"))}, QDateTime::fromMSecsSinceEpoch(8),
         {QUrl::fromLocalFile(QStringLiteral("file://image$8"))}, 2, true,
         QStringLiteral("genre2"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"),
         5, 1, QTime::fromMSecsSinceStartOfDay(9), {QUrl::fromLocalFile(QStringLiteral("/$9"))}, QDateTime::fromMSecsSinceEpoch(9),
         {QUrl::fromLocalFile(QStringLiteral("file://image$9"))}, 3, true,
         QStringLiteral("genre2"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
         QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), QStringLiteral("artist1"),
         6, 1, QTime::fromMSecsSinceStartOfDay(10), {QUrl::fromLocalFile(QStringLiteral("/$10"))}, QDateTime::fromMSecsSinceEpoch(10),
         {QUrl::fromLocalFile(QStringLiteral("file://image$10"))}, 5, true,
         QStringLiteral("genre2"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$11"), QStringLiteral("0"), QStringLiteral("track1"),
         QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
         1, 1, QTime::fromMSecsSinceStartOfDay(11), {QUrl::fromLocalFile(QStringLiteral("/$11"))}, QDateTime::fromMSecsSinceEpoch(11),
         {QUrl::fromLocalFile(QStringLiteral("file://image$11"))}, 1, true,
         QStringLiteral("genre3"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$12"), QStringLiteral("0"), QStringLiteral("track2"),
         QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
         2, 1, QTime::fromMSecsSinceStartOfDay(12), {QUrl::fromLocalFile(QStringLiteral("/$12"))}, QDateTime::fromMSecsSinceEpoch(12),
         {QUrl::fromLocalFile(QStringLiteral("file://image$12"))}, 2, true,
         QStringLiteral("genre3"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$13"), QStringLiteral("0"), QStringLiteral("track3"),
         QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
         3, 1, QTime::fromMSecsSinceStartOfDay(13), {QUrl::fromLocalFile(QStringLiteral("/$13"))}, QDateTime::fromMSecsSinceEpoch(13),
         {QUrl::fromLocalFile(QStringLiteral("file://image$13"))}, 3, true,
         QStringLiteral("genre3"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$14"), QStringLiteral("0"), QStringLiteral("track1"),
         QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"),
         1, 1, QTime::fromMSecsSinceStartOfDay(14), {QUrl::fromLocalFile(QStringLiteral("/$14"))}, QDateTime::fromMSecsSinceEpoch(14),
         {QUrl::fromLocalFile(QStringLiteral("file://image$14"))}, 4, true,
         QStringLiteral("genre4"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$15"), QStringLiteral("0"), QStringLiteral("track2"),
         QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"),
         2, 1, QTime::fromMSecsSinceStartOfDay(15), {QUrl::fromLocalFile(QStringLiteral("/$15"))}, QDateTime::fromMSecsSinceEpoch(15),
         {QUrl::fromLocalFile(QStringLiteral("file://image$15"))}, 5, true,
         QStringLiteral("genre4"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$16"), QStringLiteral("0"), QStringLiteral("track3"),
         QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"),
         3, 1, QTime::fromMSecsSinceStartOfDay(16), {QUrl::fromLocalFile(QStringLiteral("/$16"))}, QDateTime::fromMSecsSinceEpoch(16),
         {QUrl::fromLocalFile(QStringLiteral("file://image$16"))}, 1, true,
         QStringLiteral("genre4"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$17"), QStringLiteral("0"), QStringLiteral("track4"),
         QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"),
         4, 1, QTime::fromMSecsSinceStartOfDay(17), {QUrl::fromLocalFile(QStringLiteral("/$17"))}, QDateTime::fromMSecsSinceEpoch(17),
         {QUrl::fromLocalFile(QStringLiteral("file://image$17"))}, 2, true,
         QStringLiteral("genre4"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$18"), QStringLiteral("0"), QStringLiteral("track5"),
         QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"),
         5, 1, QTime::fromMSecsSinceStartOfDay(18), {QUrl::fromLocalFile(QStringLiteral("/$18"))}, QDateTime::fromMSecsSinceEpoch(18),
         {QUrl::fromLocalFile(QStringLiteral("file://image$18"))}, 3, true,
         QStringLiteral("genre3"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track1"),
         QStringLiteral("artist7"), QStringLiteral("album3"), QStringLiteral("artist7"),
         1, 1, QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))}, QDateTime::fromMSecsSinceEpoch(19),
         {QUrl::fromLocalFile(QStringLiteral("file://image$19"))}, 1, true,
         QStringLiteral("genre2"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track2"),
         QStringLiteral("artist7"), QStringLiteral("album3"), QStringLiteral("artist7"),
         2, 1, QTime::fromMSecsSinceStartOfDay(20), {QUrl::fromLocalFile(QStringLiteral("/$20"))}, QDateTime::fromMSecsSinceEpoch(20),
         {QUrl::fromLocalFile(QStringLiteral("file://image$20"))}, 2, true,
         QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$21"), QStringLiteral("0"), QStringLiteral("track3"),
         QStringLiteral("artist7"), QStringLiteral("album3"), QStringLiteral("artist7"),
         3, 1, QTime::fromMSecsSinceStartOfDay(21), {QUrl::fromLocalFile(QStringLiteral("/$21"))}, QDateTime::fromMSecsSinceEpoch(21),
         {QUrl::fromLocalFile(QStringLiteral("file://image$21"))}, 3, true,
         QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
        {true, QStringLiteral("$22"), QStringLiteral("0"), QStringLiteral("track9"),
         QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist7"),
         9, 1, QTime::fromMSecsSinceStartOfDay(22), {QUrl::fromLocalFile(QStringLiteral("/$22"))}, QDateTime::fromMSecsSinceEpoch(22),
         {QUrl::fromLocalFile(QStringLiteral("file://image$22"))}, 9, true,
         QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1")},
    };

    QHash<QString, QUrl> mNewCovers = {
        {QStringLiteral("album1"), QUrl::fromLocalFile(QStringLiteral("album1"))},
        {QStringLiteral("album2"), QUrl::fromLocalFile(QStringLiteral("album2"))},
        {QStringLiteral("album3"), QUrl::fromLocalFile(QStringLiteral("album3"))},
        {QStringLiteral("album4"), QUrl::fromLocalFile(QStringLiteral("album4"))}
    };

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>");
        qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
        qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
        qRegisterMetaType<MusicArtist>("MusicArtist");
    }

    void removeOneTrack()
    {
        DatabaseInterface musicDb;
        AllAlbumsModel albumsModel;
        ModelTest testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::albumsAdded,
                &albumsModel, &AllAlbumsModel::albumsAdded);
        connect(&musicDb, &DatabaseInterface::albumModified,
                &albumsModel, &AllAlbumsModel::albumModified);
        connect(&musicDb, &DatabaseInterface::albumRemoved,
                &albumsModel, &AllAlbumsModel::albumRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &AllAlbumsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &AllAlbumsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &AllAlbumsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &AllAlbumsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &AllAlbumsModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(dataChangedSpy.wait(150), false);

        QCOMPARE(albumsModel.rowCount(), 5);
        QCOMPARE(beginInsertRowsSpy.count(), 5);
        QCOMPARE(endInsertRowsSpy.count(), 5);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);

        auto firstTrack = musicDb.trackFromDatabaseId(trackId);

        QCOMPARE(firstTrack.isValid(), true);

        musicDb.removeTracksList({firstTrack.resourceURI()});

        QCOMPARE(dataChangedSpy.wait(150), true);

        QCOMPARE(albumsModel.rowCount(), 5);
        QCOMPARE(beginInsertRowsSpy.count(), 5);
        QCOMPARE(endInsertRowsSpy.count(), 5);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 1);
    }

    void removeOneAlbum()
    {
        DatabaseInterface musicDb;
        AllAlbumsModel albumsModel;
        ModelTest testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::albumsAdded,
                &albumsModel, &AllAlbumsModel::albumsAdded);
        connect(&musicDb, &DatabaseInterface::albumModified,
                &albumsModel, &AllAlbumsModel::albumModified);
        connect(&musicDb, &DatabaseInterface::albumRemoved,
                &albumsModel, &AllAlbumsModel::albumRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &AllAlbumsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &AllAlbumsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &AllAlbumsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &AllAlbumsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &AllAlbumsModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        while(beginInsertRowsSpy.count() < 4) {
            QCOMPARE(beginInsertRowsSpy.wait(150), true);
        }
        while(endInsertRowsSpy.count() < 4) {
            QCOMPARE(endInsertRowsSpy.wait(150), true);
        }

        QCOMPARE(beginInsertRowsSpy.count(), 5);
        QCOMPARE(endInsertRowsSpy.count(), 5);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 1, 1);
        auto firstTrack = musicDb.trackFromDatabaseId(firstTrackId);
        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                          QStringLiteral("album3"), 2, 1);
        auto secondTrack = musicDb.trackFromDatabaseId(secondTrackId);
        auto thirdTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 3, 1);
        auto thirdTrack = musicDb.trackFromDatabaseId(thirdTrackId);

        musicDb.removeTracksList({firstTrack.resourceURI(), secondTrack.resourceURI(), thirdTrack.resourceURI()});

        while (beginRemoveRowsSpy.count() < 1) {
            QCOMPARE(beginRemoveRowsSpy.wait(150), true);
        }
        while (endRemoveRowsSpy.count() < 1) {
            QCOMPARE(endRemoveRowsSpy.wait(150), true);
        }

        QCOMPARE(beginInsertRowsSpy.count(), 5);
        QCOMPARE(endInsertRowsSpy.count(), 5);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
    }

    void addOneTrack()
    {
        DatabaseInterface musicDb;
        AllAlbumsModel albumsModel;
        ModelTest testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::albumsAdded,
                &albumsModel, &AllAlbumsModel::albumsAdded);
        connect(&musicDb, &DatabaseInterface::albumModified,
                &albumsModel, &AllAlbumsModel::albumModified);
        connect(&musicDb, &DatabaseInterface::albumRemoved,
                &albumsModel, &AllAlbumsModel::albumRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &AllAlbumsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &AllAlbumsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &AllAlbumsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &AllAlbumsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &AllAlbumsModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        while(beginInsertRowsSpy.count() < 5) {
            QCOMPARE(beginInsertRowsSpy.wait(150), true);
        }
        while(endInsertRowsSpy.count() < 5) {
            QCOMPARE(endInsertRowsSpy.wait(150), true);
        }

        QCOMPARE(beginInsertRowsSpy.count(), 5);
        QCOMPARE(endInsertRowsSpy.count(), 5);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track23"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 23, 1, QTime::fromMSecsSinceStartOfDay(23),
        {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("file://image$23"))}, 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1")};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        musicDb.insertTracksList(newTracks, mNewCovers, QStringLiteral("autoTest"));

        while(dataChangedSpy.count() < 1) {
            QCOMPARE(dataChangedSpy.wait(150), true);
        }

        QCOMPARE(beginInsertRowsSpy.count(), 5);
        QCOMPARE(endInsertRowsSpy.count(), 5);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 1);
    }

    void addOneAlbum()
    {
        DatabaseInterface musicDb;
        AllAlbumsModel albumsModel;
        ModelTest testModel(&albumsModel);

        connect(&musicDb, &DatabaseInterface::albumsAdded,
                &albumsModel, &AllAlbumsModel::albumsAdded);
        connect(&musicDb, &DatabaseInterface::albumModified,
                &albumsModel, &AllAlbumsModel::albumModified);
        connect(&musicDb, &DatabaseInterface::albumRemoved,
                &albumsModel, &AllAlbumsModel::albumRemoved);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy beginInsertRowsSpy(&albumsModel, &AllAlbumsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&albumsModel, &AllAlbumsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&albumsModel, &AllAlbumsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&albumsModel, &AllAlbumsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&albumsModel, &AllAlbumsModel::dataChanged);

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

        while(beginInsertRowsSpy.count() < 5) {
            QCOMPARE(beginInsertRowsSpy.wait(150), true);
        }
        while(endInsertRowsSpy.count() < 5) {
            QCOMPARE(endInsertRowsSpy.wait(150), true);
        }

        QCOMPARE(beginInsertRowsSpy.count(), 5);
        QCOMPARE(endInsertRowsSpy.count(), 5);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album5"), QStringLiteral("artist2"), 1, 1,
                QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))},
                QDateTime::fromMSecsSinceEpoch(19),
        {QUrl::fromLocalFile(QStringLiteral("file://image$19"))}, 5, true,
        {}, QStringLiteral("composer1"), QStringLiteral("lyricist1")};
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

        while(beginInsertRowsSpy.count() < 6) {
            QCOMPARE(beginInsertRowsSpy.wait(150), true);
        }
        while(endInsertRowsSpy.count() < 6) {
            QCOMPARE(endInsertRowsSpy.wait(150), true);
        }

        QCOMPARE(beginInsertRowsSpy.count(), 6);
        QCOMPARE(endInsertRowsSpy.count(), 6);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
    }
};

QTEST_GUILESS_MAIN(AllAlbumsModelTests)


#include "allalbumsmodeltest.moc"
