/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
#include "musicalbum.h"
#include "musicaudiotrack.h"

#include <QObject>
#include <QUrl>
#include <QString>
#include <QHash>
#include <QVector>
#include <QThread>
#include <QMetaObject>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include <QDebug>

#include <QtTest/QtTest>

class DatabaseInterfaceTests: public QObject
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

    void avoidCrashInTrackIdFromTitleAlbumArtist()
    {
        DatabaseInterface musicDb;
        musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album3"), QStringLiteral("artist2"));
    }

    void addMultipleTimeSameTracks()
    {
        auto configDirectory = QDir(QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::AppDataLocation));
        auto rootDirectory = QDir::root();
        rootDirectory.mkpath(configDirectory.path());
        auto fileName = configDirectory.filePath(QStringLiteral("elisaMusicDatabase.sqlite"));
        QFile dbFile(fileName);
        auto dbExists = dbFile.exists();

        if (dbExists) {
            QCOMPARE(dbFile.remove(), true);
        }

        DatabaseInterface musicDb;
        auto newTracks = QHash<QString, QVector<MusicAudioTrack>>();
        auto newCovers = QHash<QString, QUrl>();

        musicDb.init(QStringLiteral("testDb"));

        newTracks[QStringLiteral("album1")] = {
            {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$1"))}},
            {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$2"))}},
            {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$3"))}},
            {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$4"))}},
        };

        newTracks[QStringLiteral("album2")] = {
            {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$5"))}},
            {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$6"))}},
            {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$7"))}},
            {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$8"))}},
            {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$9"))}},
            {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$10"))}}
        };

        newTracks[QStringLiteral("album3")] = {
            {true, QStringLiteral("$11"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$11"))}},
            {true, QStringLiteral("$12"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$12"))}},
            {true, QStringLiteral("$13"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$13"))}}
        };

        newTracks[QStringLiteral("album4")] = {
            {true, QStringLiteral("$14"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$14"))}},
            {true, QStringLiteral("$15"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$15"))}},
            {true, QStringLiteral("$16"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$16"))}},
            {true, QStringLiteral("$17"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$17"))}},
            {true, QStringLiteral("$18"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$18"))}}
        };

        newCovers[QStringLiteral("album1")] = QUrl::fromLocalFile(QStringLiteral("album1"));
        newCovers[QStringLiteral("album2")] = QUrl::fromLocalFile(QStringLiteral("album2"));
        newCovers[QStringLiteral("album3")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("album4")] = QUrl::fromLocalFile(QStringLiteral("album4"));

        musicDb.insertTracksList(newTracks, newCovers);
        musicDb.insertTracksList(newTracks, newCovers);
        musicDb.insertTracksList(newTracks, newCovers);
        musicDb.insertTracksList(newTracks, newCovers);

        QCOMPARE(musicDb.albumCount({}), 4);

        auto firstAlbum = musicDb.albumFromTitleAndAuthor(QStringLiteral("album1"), QStringLiteral("artist1"));

        QCOMPARE(firstAlbum.isValid(), true);
        QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

        auto firstAlbumInvalid = musicDb.albumFromTitleAndAuthor(QStringLiteral("album1Invalid"), QStringLiteral("artist1"));

        QCOMPARE(firstAlbumInvalid.isValid(), false);
    }

    void simpleAccessor()
    {
        auto configDirectory = QDir(QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::AppDataLocation));
        auto rootDirectory = QDir::root();
        rootDirectory.mkpath(configDirectory.path());
        auto fileName = configDirectory.filePath(QStringLiteral("elisaMusicDatabase.sqlite"));
        QFile dbFile(fileName);
        auto dbExists = dbFile.exists();

        if (dbExists) {
            QCOMPARE(dbFile.remove(), true);
        }

        DatabaseInterface musicDb;
        auto newTracks = QHash<QString, QVector<MusicAudioTrack>>();
        auto newCovers = QHash<QString, QUrl>();

        musicDb.init(QStringLiteral("testDb"));

        newTracks[QStringLiteral("album1")] = {
            {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, QTime::fromMSecsSinceStartOfDay(1), {QUrl::fromLocalFile(QStringLiteral("$1"))}},
            {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, QTime::fromMSecsSinceStartOfDay(2), {QUrl::fromLocalFile(QStringLiteral("$2"))}},
            {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, QTime::fromMSecsSinceStartOfDay(3), {QUrl::fromLocalFile(QStringLiteral("$3"))}},
            {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("$4"))}},
        };

        newTracks[QStringLiteral("album2")] = {
            {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, QTime::fromMSecsSinceStartOfDay(5), {QUrl::fromLocalFile(QStringLiteral("$5"))}},
            {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, QTime::fromMSecsSinceStartOfDay(6), {QUrl::fromLocalFile(QStringLiteral("$6"))}},
            {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, QTime::fromMSecsSinceStartOfDay(7), {QUrl::fromLocalFile(QStringLiteral("$7"))}},
            {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, QTime::fromMSecsSinceStartOfDay(8), {QUrl::fromLocalFile(QStringLiteral("$8"))}},
            {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, QTime::fromMSecsSinceStartOfDay(9), {QUrl::fromLocalFile(QStringLiteral("$9"))}},
            {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, QTime::fromMSecsSinceStartOfDay(10), {QUrl::fromLocalFile(QStringLiteral("$10"))}}
        };

        newTracks[QStringLiteral("album3")] = {
            {true, QStringLiteral("$11"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 1, QTime::fromMSecsSinceStartOfDay(11), {QUrl::fromLocalFile(QStringLiteral("$11"))}},
            {true, QStringLiteral("$12"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 1, QTime::fromMSecsSinceStartOfDay(12), {QUrl::fromLocalFile(QStringLiteral("$12"))}},
            {true, QStringLiteral("$13"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 1, QTime::fromMSecsSinceStartOfDay(13), {QUrl::fromLocalFile(QStringLiteral("$13"))}}
        };

        newTracks[QStringLiteral("album4")] = {
            {true, QStringLiteral("$14"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, QTime::fromMSecsSinceStartOfDay(14), {QUrl::fromLocalFile(QStringLiteral("$14"))}},
            {true, QStringLiteral("$15"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, QTime::fromMSecsSinceStartOfDay(15), {QUrl::fromLocalFile(QStringLiteral("$15"))}},
            {true, QStringLiteral("$16"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, QTime::fromMSecsSinceStartOfDay(16), {QUrl::fromLocalFile(QStringLiteral("$16"))}},
            {true, QStringLiteral("$17"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, QTime::fromMSecsSinceStartOfDay(17), {QUrl::fromLocalFile(QStringLiteral("$17"))}},
            {true, QStringLiteral("$18"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, QTime::fromMSecsSinceStartOfDay(18), {QUrl::fromLocalFile(QStringLiteral("$18"))}}
        };

        newCovers[QStringLiteral("album1")] = QUrl::fromLocalFile(QStringLiteral("album1"));
        newCovers[QStringLiteral("album2")] = QUrl::fromLocalFile(QStringLiteral("album2"));
        newCovers[QStringLiteral("album3")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("album4")] = QUrl::fromLocalFile(QStringLiteral("album4"));

        musicDb.insertTracksList(newTracks, newCovers);
        musicDb.insertTracksList(newTracks, newCovers);
        musicDb.insertTracksList(newTracks, newCovers);
        musicDb.insertTracksList(newTracks, newCovers);

        QCOMPARE(musicDb.albumCount({}), 4);

        auto firstAlbumTitle = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::Title);
        auto firstAlbumArtist = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::Artist);
        auto firstAlbumImage = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::Image);
        auto firstAlbumTracksCount = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::TracksCount);
        auto firstAlbumId = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::Id);

        QCOMPARE(firstAlbumTitle.isValid(), true);
        QCOMPARE(firstAlbumTitle.toString(), QStringLiteral("album1"));
        QCOMPARE(firstAlbumArtist.isValid(), true);
        QCOMPARE(firstAlbumArtist.toString(), QStringLiteral("artist1"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage.toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstAlbumTracksCount.isValid(), true);
        QCOMPARE(firstAlbumTracksCount.toInt(), 4);
        QCOMPARE(firstAlbumId.isValid(), true);
        QCOMPARE(firstAlbumId.toInt(), 0);

        auto firstInvalidAlbumTitle = musicDb.albumDataFromIndex({}, -1, DatabaseInterface::AlbumData::Title);
        QCOMPARE(firstInvalidAlbumTitle.isValid(), false);

        auto secondInvalidAlbumTitle = musicDb.albumDataFromIndex({}, 4, DatabaseInterface::AlbumData::Title);
        QCOMPARE(secondInvalidAlbumTitle.isValid(), false);

        auto invalidTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("invalidArtist1"));
        QCOMPARE(invalidTrackId, decltype(invalidTrackId)(0));

        auto trackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));

        auto firstTrackTitle = musicDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::Title);
        auto firstTrackArtist = musicDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::Artist);
        auto firstTrackAlbum = musicDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::Album);
        auto firstTrackImage = musicDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::Image);
        auto firstTrackDuration = musicDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::Duration);
        auto firstTrackMilliSecondsDuration = musicDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::MilliSecondsDuration);
        auto firstTrackTrackNumber = musicDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::TrackNumber);
        auto firstTrackResource = musicDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::Resource);

        QCOMPARE(firstTrackTitle.isValid(), true);
        QCOMPARE(firstTrackTitle.toString(), QStringLiteral("track1"));
        QCOMPARE(firstTrackArtist.isValid(), true);
        QCOMPARE(firstTrackArtist.toString(), QStringLiteral("artist1"));
        QCOMPARE(firstTrackAlbum.isValid(), true);
        QCOMPARE(firstTrackAlbum.toString(), QStringLiteral("album1"));
        QCOMPARE(firstTrackImage.isValid(), true);
        QCOMPARE(firstTrackImage.toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstTrackDuration.isValid(), true);
        QCOMPARE(firstTrackDuration.toTime(), QTime::fromMSecsSinceStartOfDay(1));
        QCOMPARE(firstTrackMilliSecondsDuration.isValid(), true);
        QCOMPARE(firstTrackMilliSecondsDuration.toInt(), 1);
        QCOMPARE(firstTrackTrackNumber.isValid(), true);
        QCOMPARE(firstTrackTrackNumber.toInt(), 1);
        QCOMPARE(firstTrackResource.isValid(), true);
        QCOMPARE(firstTrackResource.toUrl(), QUrl::fromLocalFile(QStringLiteral("$1")));

        auto firstInvalidTrackTitle = musicDb.trackDataFromDatabaseId(-1, DatabaseInterface::TrackData::Title);
        QCOMPARE(firstInvalidTrackTitle.isValid(), false);

        auto secondInvalidTrackTitle = musicDb.trackDataFromDatabaseId(19, DatabaseInterface::TrackData::Title);
        QCOMPARE(secondInvalidTrackTitle.isValid(), false);
    }

    void simpleAccessorWithClientDatabase()
    {
        auto configDirectory = QDir(QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::AppDataLocation));
        auto rootDirectory = QDir::root();
        rootDirectory.mkpath(configDirectory.path());
        auto fileName = configDirectory.filePath(QStringLiteral("elisaMusicDatabase.sqlite"));
        QFile dbFile(fileName);
        auto dbExists = dbFile.exists();

        if (dbExists) {
            QCOMPARE(dbFile.remove(), true);
        }

        DatabaseInterface musicDb;
        DatabaseInterface clientDb;

        auto newTracks = QHash<QString, QVector<MusicAudioTrack>>();
        auto newCovers = QHash<QString, QUrl>();

        musicDb.init(QStringLiteral("testDb"));

        clientDb.init(QStringLiteral("clientDb"));

        QSignalSpy musicDbChangedSpy(&musicDb, &DatabaseInterface::databaseChanged);

        connect(&musicDb, &DatabaseInterface::databaseChanged,
                &clientDb, &DatabaseInterface::databaseHasChanged);

        newTracks[QStringLiteral("album1")] = {
            {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, QTime::fromMSecsSinceStartOfDay(1), {QUrl::fromLocalFile(QStringLiteral("$1"))}},
            {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, QTime::fromMSecsSinceStartOfDay(2), {QUrl::fromLocalFile(QStringLiteral("$2"))}},
            {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, QTime::fromMSecsSinceStartOfDay(3), {QUrl::fromLocalFile(QStringLiteral("$3"))}},
            {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("$4"))}},
        };

        newTracks[QStringLiteral("album2")] = {
            {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, QTime::fromMSecsSinceStartOfDay(5), {QUrl::fromLocalFile(QStringLiteral("$5"))}},
            {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, QTime::fromMSecsSinceStartOfDay(6), {QUrl::fromLocalFile(QStringLiteral("$6"))}},
            {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, QTime::fromMSecsSinceStartOfDay(7), {QUrl::fromLocalFile(QStringLiteral("$7"))}},
            {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, QTime::fromMSecsSinceStartOfDay(8), {QUrl::fromLocalFile(QStringLiteral("$8"))}},
            {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, QTime::fromMSecsSinceStartOfDay(9), {QUrl::fromLocalFile(QStringLiteral("$9"))}},
            {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, QTime::fromMSecsSinceStartOfDay(10), {QUrl::fromLocalFile(QStringLiteral("$10"))}}
        };

        newTracks[QStringLiteral("album3")] = {
            {true, QStringLiteral("$11"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 1, QTime::fromMSecsSinceStartOfDay(11), {QUrl::fromLocalFile(QStringLiteral("$11"))}},
            {true, QStringLiteral("$12"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 1, QTime::fromMSecsSinceStartOfDay(12), {QUrl::fromLocalFile(QStringLiteral("$12"))}},
            {true, QStringLiteral("$13"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 1, QTime::fromMSecsSinceStartOfDay(13), {QUrl::fromLocalFile(QStringLiteral("$13"))}}
        };

        newTracks[QStringLiteral("album4")] = {
            {true, QStringLiteral("$14"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, QTime::fromMSecsSinceStartOfDay(14), {QUrl::fromLocalFile(QStringLiteral("$14"))}},
            {true, QStringLiteral("$15"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, QTime::fromMSecsSinceStartOfDay(15), {QUrl::fromLocalFile(QStringLiteral("$15"))}},
            {true, QStringLiteral("$16"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, QTime::fromMSecsSinceStartOfDay(16), {QUrl::fromLocalFile(QStringLiteral("$16"))}},
            {true, QStringLiteral("$17"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, QTime::fromMSecsSinceStartOfDay(17), {QUrl::fromLocalFile(QStringLiteral("$17"))}},
            {true, QStringLiteral("$18"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, QTime::fromMSecsSinceStartOfDay(18), {QUrl::fromLocalFile(QStringLiteral("$18"))}}
        };

        newCovers[QStringLiteral("album1")] = QUrl::fromLocalFile(QStringLiteral("album1"));
        newCovers[QStringLiteral("album2")] = QUrl::fromLocalFile(QStringLiteral("album2"));
        newCovers[QStringLiteral("album3")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("album4")] = QUrl::fromLocalFile(QStringLiteral("album4"));

        musicDb.insertTracksList(newTracks, newCovers);
        musicDb.insertTracksList(newTracks, newCovers);
        musicDb.insertTracksList(newTracks, newCovers);
        musicDb.insertTracksList(newTracks, newCovers);

        QCOMPARE(musicDb.albumCount({}), 4);
        QCOMPARE(clientDb.albumCount({}), 4);
        QCOMPARE(musicDbChangedSpy.count(), 4);

        auto firstAlbumTitle = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::Title);
        auto firstAlbumArtist = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::Artist);
        auto firstAlbumImage = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::Image);
        auto firstAlbumTracksCount = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::TracksCount);
        auto firstAlbumId = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::Id);

        QCOMPARE(firstAlbumTitle.isValid(), true);
        QCOMPARE(firstAlbumTitle.toString(), QStringLiteral("album1"));
        QCOMPARE(firstAlbumArtist.isValid(), true);
        QCOMPARE(firstAlbumArtist.toString(), QStringLiteral("artist1"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage.toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstAlbumTracksCount.isValid(), true);
        QCOMPARE(firstAlbumTracksCount.toInt(), 4);
        QCOMPARE(firstAlbumId.isValid(), true);
        QCOMPARE(firstAlbumId.toInt(), 0);

        auto firstInvalidAlbumTitle = clientDb.albumDataFromIndex({}, -1, DatabaseInterface::AlbumData::Title);
        QCOMPARE(firstInvalidAlbumTitle.isValid(), false);

        auto secondInvalidAlbumTitle = clientDb.albumDataFromIndex({}, 4, DatabaseInterface::AlbumData::Title);
        QCOMPARE(secondInvalidAlbumTitle.isValid(), false);

        auto invalidTrackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("invalidArtist1"));
        QCOMPARE(invalidTrackId, decltype(invalidTrackId)(0));

        auto trackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));

        auto firstTrackTitle = clientDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::Title);
        auto firstTrackArtist = clientDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::Artist);
        auto firstTrackAlbum = clientDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::Album);
        auto firstTrackImage = clientDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::Image);
        auto firstTrackDuration = clientDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::Duration);
        auto firstTrackMilliSecondsDuration = clientDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::MilliSecondsDuration);
        auto firstTrackTrackNumber = clientDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::TrackNumber);
        auto firstTrackResource = clientDb.trackDataFromDatabaseId(trackId, DatabaseInterface::TrackData::Resource);

        QCOMPARE(firstTrackTitle.isValid(), true);
        QCOMPARE(firstTrackTitle.toString(), QStringLiteral("track1"));
        QCOMPARE(firstTrackArtist.isValid(), true);
        QCOMPARE(firstTrackArtist.toString(), QStringLiteral("artist1"));
        QCOMPARE(firstTrackAlbum.isValid(), true);
        QCOMPARE(firstTrackAlbum.toString(), QStringLiteral("album1"));
        QCOMPARE(firstTrackImage.isValid(), true);
        QCOMPARE(firstTrackImage.toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstTrackDuration.isValid(), true);
        QCOMPARE(firstTrackDuration.toTime(), QTime::fromMSecsSinceStartOfDay(1));
        QCOMPARE(firstTrackMilliSecondsDuration.isValid(), true);
        QCOMPARE(firstTrackMilliSecondsDuration.toInt(), 1);
        QCOMPARE(firstTrackTrackNumber.isValid(), true);
        QCOMPARE(firstTrackTrackNumber.toInt(), 1);
        QCOMPARE(firstTrackResource.isValid(), true);
        QCOMPARE(firstTrackResource.toUrl(), QUrl::fromLocalFile(QStringLiteral("$1")));

        auto firstInvalidTrackTitle = clientDb.trackDataFromDatabaseId(-1, DatabaseInterface::TrackData::Title);
        QCOMPARE(firstInvalidTrackTitle.isValid(), false);

        auto secondInvalidTrackTitle = clientDb.trackDataFromDatabaseId(19, DatabaseInterface::TrackData::Title);
        QCOMPARE(secondInvalidTrackTitle.isValid(), false);
    }

    void simpleAccessorWithClientDatabaseAndVariousArtistAlbum()
    {
        auto configDirectory = QDir(QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::AppDataLocation));
        auto rootDirectory = QDir::root();
        rootDirectory.mkpath(configDirectory.path());
        auto fileName = configDirectory.filePath(QStringLiteral("elisaMusicDatabase.sqlite"));
        QFile dbFile(fileName);
        auto dbExists = dbFile.exists();

        if (dbExists) {
            QCOMPARE(dbFile.remove(), true);
        }

        DatabaseInterface musicDb;
        DatabaseInterface clientDb;

        auto newTracks = QHash<QString, QVector<MusicAudioTrack>>();
        auto newCovers = QHash<QString, QUrl>();

        musicDb.init(QStringLiteral("testDbVariousArtistAlbum"));

        clientDb.init(QStringLiteral("clientDbVariousArtistAlbum"));

        QSignalSpy musicDbChangedSpy(&musicDb, &DatabaseInterface::databaseChanged);

        connect(&musicDb, &DatabaseInterface::databaseChanged,
                &clientDb, &DatabaseInterface::databaseHasChanged);

        newTracks[QStringLiteral("album1")] = {
            {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, QTime::fromMSecsSinceStartOfDay(1), {QUrl::fromLocalFile(QStringLiteral("$1"))}},
            {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album1"), 2, QTime::fromMSecsSinceStartOfDay(2), {QUrl::fromLocalFile(QStringLiteral("$2"))}},
            {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist3"), QStringLiteral("album1"), 3, QTime::fromMSecsSinceStartOfDay(3), {QUrl::fromLocalFile(QStringLiteral("$3"))}},
            {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist4"), QStringLiteral("album1"), 4, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("$4"))}},
        };

        newTracks[QStringLiteral("album2")] = {
            {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, QTime::fromMSecsSinceStartOfDay(5), {QUrl::fromLocalFile(QStringLiteral("$5"))}},
            {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 2, QTime::fromMSecsSinceStartOfDay(6), {QUrl::fromLocalFile(QStringLiteral("$6"))}},
            {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 3, QTime::fromMSecsSinceStartOfDay(7), {QUrl::fromLocalFile(QStringLiteral("$7"))}},
            {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 4, QTime::fromMSecsSinceStartOfDay(8), {QUrl::fromLocalFile(QStringLiteral("$8"))}},
            {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 5, QTime::fromMSecsSinceStartOfDay(9), {QUrl::fromLocalFile(QStringLiteral("$9"))}},
            {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 6, QTime::fromMSecsSinceStartOfDay(10), {QUrl::fromLocalFile(QStringLiteral("$10"))}}
        };

        newTracks[QStringLiteral("album3")] = {
            {true, QStringLiteral("$11"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 1, QTime::fromMSecsSinceStartOfDay(11), {QUrl::fromLocalFile(QStringLiteral("$11"))}},
            {true, QStringLiteral("$12"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 2, QTime::fromMSecsSinceStartOfDay(12), {QUrl::fromLocalFile(QStringLiteral("$12"))}},
            {true, QStringLiteral("$13"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 3, QTime::fromMSecsSinceStartOfDay(13), {QUrl::fromLocalFile(QStringLiteral("$13"))}}
        };

        newTracks[QStringLiteral("album4")] = {
            {true, QStringLiteral("$14"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, QTime::fromMSecsSinceStartOfDay(14), {QUrl::fromLocalFile(QStringLiteral("$14"))}},
            {true, QStringLiteral("$15"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 2, QTime::fromMSecsSinceStartOfDay(15), {QUrl::fromLocalFile(QStringLiteral("$15"))}},
            {true, QStringLiteral("$16"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 3, QTime::fromMSecsSinceStartOfDay(16), {QUrl::fromLocalFile(QStringLiteral("$16"))}},
            {true, QStringLiteral("$17"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 4, QTime::fromMSecsSinceStartOfDay(17), {QUrl::fromLocalFile(QStringLiteral("$17"))}},
            {true, QStringLiteral("$18"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 5, QTime::fromMSecsSinceStartOfDay(18), {QUrl::fromLocalFile(QStringLiteral("$18"))}}
        };

        newCovers[QStringLiteral("album1")] = QUrl::fromLocalFile(QStringLiteral("album1"));
        newCovers[QStringLiteral("album2")] = QUrl::fromLocalFile(QStringLiteral("album2"));
        newCovers[QStringLiteral("album3")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("album4")] = QUrl::fromLocalFile(QStringLiteral("album4"));

        musicDb.insertTracksList(newTracks, newCovers);
        musicDb.insertTracksList(newTracks, newCovers);
        musicDb.insertTracksList(newTracks, newCovers);
        musicDb.insertTracksList(newTracks, newCovers);

        QCOMPARE(musicDb.albumCount({}), 4);
        QCOMPARE(clientDb.albumCount({}), 4);
        QCOMPARE(musicDbChangedSpy.count(), 4);

        auto firstAlbumTitle = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::Title);
        auto firstAlbumArtist = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::Artist);
        auto firstAlbumImage = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::Image);
        auto firstAlbumTracksCount = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::TracksCount);
        auto firstAlbumId = musicDb.albumDataFromIndex({}, 0, DatabaseInterface::AlbumData::Id);

        QCOMPARE(firstAlbumTitle.isValid(), true);
        QCOMPARE(firstAlbumTitle.toString(), QStringLiteral("album1"));
        QCOMPARE(firstAlbumArtist.isValid(), true);
        QCOMPARE(firstAlbumArtist.toString(), QStringLiteral("Various Artists"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage.toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstAlbumTracksCount.isValid(), true);
        QCOMPARE(firstAlbumTracksCount.toInt(), 4);
        QCOMPARE(firstAlbumId.isValid(), true);
        QCOMPARE(firstAlbumId.toInt(), 0);

        auto firstInvalidAlbumTitle = clientDb.albumDataFromIndex({}, -1, DatabaseInterface::AlbumData::Title);
        QCOMPARE(firstInvalidAlbumTitle.isValid(), false);

        auto secondInvalidAlbumTitle = clientDb.albumDataFromIndex({}, 4, DatabaseInterface::AlbumData::Title);
        QCOMPARE(secondInvalidAlbumTitle.isValid(), false);

        auto invalidTrackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("invalidArtist1"));
        QCOMPARE(invalidTrackId, decltype(invalidTrackId)(0));

        auto firstTrackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));

        auto firstTrackTitle = clientDb.trackDataFromDatabaseId(firstTrackId, DatabaseInterface::TrackData::Title);
        auto firstTrackArtist = clientDb.trackDataFromDatabaseId(firstTrackId, DatabaseInterface::TrackData::Artist);
        auto firstTrackAlbum = clientDb.trackDataFromDatabaseId(firstTrackId, DatabaseInterface::TrackData::Album);
        auto firstTrackImage = clientDb.trackDataFromDatabaseId(firstTrackId, DatabaseInterface::TrackData::Image);
        auto firstTrackDuration = clientDb.trackDataFromDatabaseId(firstTrackId, DatabaseInterface::TrackData::Duration);
        auto firstTrackMilliSecondsDuration = clientDb.trackDataFromDatabaseId(firstTrackId, DatabaseInterface::TrackData::MilliSecondsDuration);
        auto firstTrackTrackNumber = clientDb.trackDataFromDatabaseId(firstTrackId, DatabaseInterface::TrackData::TrackNumber);
        auto firstTrackResource = clientDb.trackDataFromDatabaseId(firstTrackId, DatabaseInterface::TrackData::Resource);

        QCOMPARE(firstTrackTitle.isValid(), true);
        QCOMPARE(firstTrackTitle.toString(), QStringLiteral("track1"));
        QCOMPARE(firstTrackArtist.isValid(), true);
        QCOMPARE(firstTrackArtist.toString(), QStringLiteral("artist1"));
        QCOMPARE(firstTrackAlbum.isValid(), true);
        QCOMPARE(firstTrackAlbum.toString(), QStringLiteral("album1"));
        QCOMPARE(firstTrackImage.isValid(), true);
        QCOMPARE(firstTrackImage.toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstTrackDuration.isValid(), true);
        QCOMPARE(firstTrackDuration.toTime(), QTime::fromMSecsSinceStartOfDay(1));
        QCOMPARE(firstTrackMilliSecondsDuration.isValid(), true);
        QCOMPARE(firstTrackMilliSecondsDuration.toInt(), 1);
        QCOMPARE(firstTrackTrackNumber.isValid(), true);
        QCOMPARE(firstTrackTrackNumber.toInt(), 1);
        QCOMPARE(firstTrackResource.isValid(), true);
        QCOMPARE(firstTrackResource.toUrl(), QUrl::fromLocalFile(QStringLiteral("$1")));

        auto secondTrackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track2"), QStringLiteral("album1"), QStringLiteral("artist2"));

        auto secondTrackTitle = clientDb.trackDataFromDatabaseId(secondTrackId, DatabaseInterface::TrackData::Title);
        auto secondTrackArtist = clientDb.trackDataFromDatabaseId(secondTrackId, DatabaseInterface::TrackData::Artist);
        auto secondTrackAlbum = clientDb.trackDataFromDatabaseId(secondTrackId, DatabaseInterface::TrackData::Album);
        auto secondTrackImage = clientDb.trackDataFromDatabaseId(secondTrackId, DatabaseInterface::TrackData::Image);
        auto secondTrackDuration = clientDb.trackDataFromDatabaseId(secondTrackId, DatabaseInterface::TrackData::Duration);
        auto secondTrackMilliSecondsDuration = clientDb.trackDataFromDatabaseId(secondTrackId, DatabaseInterface::TrackData::MilliSecondsDuration);
        auto secondTrackTrackNumber = clientDb.trackDataFromDatabaseId(secondTrackId, DatabaseInterface::TrackData::TrackNumber);
        auto secondTrackResource = clientDb.trackDataFromDatabaseId(secondTrackId, DatabaseInterface::TrackData::Resource);

        QCOMPARE(secondTrackTitle.isValid(), true);
        QCOMPARE(secondTrackTitle.toString(), QStringLiteral("track2"));
        QCOMPARE(secondTrackArtist.isValid(), true);
        QCOMPARE(secondTrackArtist.toString(), QStringLiteral("artist2"));
        QCOMPARE(secondTrackAlbum.isValid(), true);
        QCOMPARE(secondTrackAlbum.toString(), QStringLiteral("album1"));
        QCOMPARE(secondTrackImage.isValid(), true);
        QCOMPARE(secondTrackImage.toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(secondTrackDuration.isValid(), true);
        QCOMPARE(secondTrackDuration.toTime(), QTime::fromMSecsSinceStartOfDay(2));
        QCOMPARE(secondTrackMilliSecondsDuration.isValid(), true);
        QCOMPARE(secondTrackMilliSecondsDuration.toInt(), 2);
        QCOMPARE(secondTrackTrackNumber.isValid(), true);
        QCOMPARE(secondTrackTrackNumber.toInt(), 2);
        QCOMPARE(secondTrackResource.isValid(), true);
        QCOMPARE(secondTrackResource.toUrl(), QUrl::fromLocalFile(QStringLiteral("$2")));

        auto thirdTrackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist3"));

        auto thirdTrackTitle = clientDb.trackDataFromDatabaseId(thirdTrackId, DatabaseInterface::TrackData::Title);
        auto thirdTrackArtist = clientDb.trackDataFromDatabaseId(thirdTrackId, DatabaseInterface::TrackData::Artist);
        auto thirdTrackAlbum = clientDb.trackDataFromDatabaseId(thirdTrackId, DatabaseInterface::TrackData::Album);
        auto thirdTrackImage = clientDb.trackDataFromDatabaseId(thirdTrackId, DatabaseInterface::TrackData::Image);
        auto thirdTrackDuration = clientDb.trackDataFromDatabaseId(thirdTrackId, DatabaseInterface::TrackData::Duration);
        auto thirdTrackMilliSecondsDuration = clientDb.trackDataFromDatabaseId(thirdTrackId, DatabaseInterface::TrackData::MilliSecondsDuration);
        auto thirdTrackTrackNumber = clientDb.trackDataFromDatabaseId(thirdTrackId, DatabaseInterface::TrackData::TrackNumber);
        auto thirdTrackResource = clientDb.trackDataFromDatabaseId(thirdTrackId, DatabaseInterface::TrackData::Resource);

        QCOMPARE(thirdTrackTitle.isValid(), true);
        QCOMPARE(thirdTrackTitle.toString(), QStringLiteral("track3"));
        QCOMPARE(thirdTrackArtist.isValid(), true);
        QCOMPARE(thirdTrackArtist.toString(), QStringLiteral("artist3"));
        QCOMPARE(thirdTrackAlbum.isValid(), true);
        QCOMPARE(thirdTrackAlbum.toString(), QStringLiteral("album1"));
        QCOMPARE(thirdTrackImage.isValid(), true);
        QCOMPARE(thirdTrackImage.toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(thirdTrackDuration.isValid(), true);
        QCOMPARE(thirdTrackDuration.toTime(), QTime::fromMSecsSinceStartOfDay(3));
        QCOMPARE(thirdTrackMilliSecondsDuration.isValid(), true);
        QCOMPARE(thirdTrackMilliSecondsDuration.toInt(), 3);
        QCOMPARE(thirdTrackTrackNumber.isValid(), true);
        QCOMPARE(thirdTrackTrackNumber.toInt(), 3);
        QCOMPARE(thirdTrackResource.isValid(), true);
        QCOMPARE(thirdTrackResource.toUrl(), QUrl::fromLocalFile(QStringLiteral("$3")));

        auto fourthTrackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track4"), QStringLiteral("album1"), QStringLiteral("artist4"));

        auto fourthTrackTitle = clientDb.trackDataFromDatabaseId(fourthTrackId, DatabaseInterface::TrackData::Title);
        auto fourthTrackArtist = clientDb.trackDataFromDatabaseId(fourthTrackId, DatabaseInterface::TrackData::Artist);
        auto fourthTrackAlbum = clientDb.trackDataFromDatabaseId(fourthTrackId, DatabaseInterface::TrackData::Album);
        auto fourthTrackImage = clientDb.trackDataFromDatabaseId(fourthTrackId, DatabaseInterface::TrackData::Image);
        auto fourthTrackDuration = clientDb.trackDataFromDatabaseId(fourthTrackId, DatabaseInterface::TrackData::Duration);
        auto fourthTrackMilliSecondsDuration = clientDb.trackDataFromDatabaseId(fourthTrackId, DatabaseInterface::TrackData::MilliSecondsDuration);
        auto fourthTrackTrackNumber = clientDb.trackDataFromDatabaseId(fourthTrackId, DatabaseInterface::TrackData::TrackNumber);
        auto fourthTrackResource = clientDb.trackDataFromDatabaseId(fourthTrackId, DatabaseInterface::TrackData::Resource);

        QCOMPARE(fourthTrackTitle.isValid(), true);
        QCOMPARE(fourthTrackTitle.toString(), QStringLiteral("track4"));
        QCOMPARE(fourthTrackArtist.isValid(), true);
        QCOMPARE(fourthTrackArtist.toString(), QStringLiteral("artist4"));
        QCOMPARE(fourthTrackAlbum.isValid(), true);
        QCOMPARE(fourthTrackAlbum.toString(), QStringLiteral("album1"));
        QCOMPARE(fourthTrackImage.isValid(), true);
        QCOMPARE(fourthTrackImage.toUrl(), QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(fourthTrackDuration.isValid(), true);
        QCOMPARE(fourthTrackDuration.toTime(), QTime::fromMSecsSinceStartOfDay(4));
        QCOMPARE(fourthTrackMilliSecondsDuration.isValid(), true);
        QCOMPARE(fourthTrackMilliSecondsDuration.toInt(), 4);
        QCOMPARE(fourthTrackTrackNumber.isValid(), true);
        QCOMPARE(fourthTrackTrackNumber.toInt(), 4);
        QCOMPARE(fourthTrackResource.isValid(), true);
        QCOMPARE(fourthTrackResource.toUrl(), QUrl::fromLocalFile(QStringLiteral("$4")));

        auto firstInvalidTrackTitle = clientDb.trackDataFromDatabaseId(-1, DatabaseInterface::TrackData::Title);
        QCOMPARE(firstInvalidTrackTitle.isValid(), false);

        auto secondInvalidTrackTitle = clientDb.trackDataFromDatabaseId(19, DatabaseInterface::TrackData::Title);
        QCOMPARE(secondInvalidTrackTitle.isValid(), false);
    }

    void addMultipleTimeSameTracksMultiThread()
    {
        QSKIP("multithreaded SQLite support is complicated. Disable the test.");

        auto configDirectory = QDir(QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::AppDataLocation));
        auto rootDirectory = QDir::root();
        rootDirectory.mkpath(configDirectory.path());
        auto fileName = configDirectory.filePath(QStringLiteral("elisaMusicDatabase.sqlite"));
        QFile dbFile(fileName);
        auto dbExists = dbFile.exists();

        if (dbExists) {
            QCOMPARE(dbFile.remove(), true);
        }

        DatabaseInterface musicDb;
        musicDb.init(QStringLiteral("testDb"));

        DatabaseInterface musicDbThread1;
        QSignalSpy musicDbThread1Spy(&musicDbThread1, &DatabaseInterface::requestsInitDone);
        QSignalSpy musicDbdatabaseChanged1Spy(&musicDbThread1, &DatabaseInterface::databaseChanged);
        QThread thread1;
        QSignalSpy thread1FinishedSpy(&thread1, &QThread::finished);
        musicDbThread1.moveToThread(&thread1);
        thread1.start();
        QMetaObject::invokeMethod(&musicDbThread1, "init", Qt::QueuedConnection, Q_ARG(QString, QStringLiteral("testDb1")));

        musicDbThread1Spy.wait();

        DatabaseInterface musicDbThread2;
        QSignalSpy musicDbThread2Spy(&musicDbThread2, &DatabaseInterface::requestsInitDone);
        QSignalSpy musicDbdatabaseChanged2Spy(&musicDbThread2, &DatabaseInterface::databaseChanged);
        QThread thread2;
        QSignalSpy thread2FinishedSpy(&thread2, &QThread::finished);
        musicDbThread2.moveToThread(&thread2);
        thread2.start();
        QMetaObject::invokeMethod(&musicDbThread2, "init", Qt::QueuedConnection, Q_ARG(QString, QStringLiteral("testDb2")));

        musicDbThread2Spy.wait();

        DatabaseInterface musicDbThread3;
        QSignalSpy musicDbThread3Spy(&musicDbThread3, &DatabaseInterface::requestsInitDone);
        QSignalSpy musicDbdatabaseChanged3Spy(&musicDbThread3, &DatabaseInterface::databaseChanged);
        QThread thread3;
        QSignalSpy thread3FinishedSpy(&thread3, &QThread::finished);
        musicDbThread3.moveToThread(&thread3);
        thread3.start();
        QMetaObject::invokeMethod(&musicDbThread3, "init", Qt::QueuedConnection, Q_ARG(QString, QStringLiteral("testDb3")));

        musicDbThread3Spy.wait();

        DatabaseInterface musicDbThread4;
        QSignalSpy musicDbThread4Spy(&musicDbThread4, &DatabaseInterface::requestsInitDone);
        QSignalSpy musicDbdatabaseChanged4Spy(&musicDbThread4, &DatabaseInterface::databaseChanged);
        QThread thread4;
        QSignalSpy thread4FinishedSpy(&thread4, &QThread::finished);
        musicDbThread4.moveToThread(&thread4);
        thread4.start();
        QMetaObject::invokeMethod(&musicDbThread4, "init", Qt::QueuedConnection, Q_ARG(QString, QStringLiteral("testDb4")));

        musicDbThread4Spy.wait();

        auto newTracks = QHash<QString, QVector<MusicAudioTrack>>();
        auto newCovers = QHash<QString, QUrl>();

        newTracks[QStringLiteral("album1")] = {
            {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$1"))}},
            {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$2"))}},
            {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$3"))}},
            {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist1"), QStringLiteral("album1"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$4"))}},
        };

        newTracks[QStringLiteral("album2")] = {
            {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$5"))}},
            {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$6"))}},
            {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$7"))}},
            {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$8"))}},
            {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$9"))}},
            {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist1"), QStringLiteral("album2"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$10"))}}
        };

        newTracks[QStringLiteral("album3")] = {
            {true, QStringLiteral("$11"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$11"))}},
            {true, QStringLiteral("$12"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$12"))}},
            {true, QStringLiteral("$13"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album3"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$13"))}}
        };

        newTracks[QStringLiteral("album4")] = {
            {true, QStringLiteral("$14"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$14"))}},
            {true, QStringLiteral("$15"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$15"))}},
            {true, QStringLiteral("$16"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$16"))}},
            {true, QStringLiteral("$17"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$17"))}},
            {true, QStringLiteral("$18"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist2"), QStringLiteral("album4"), 1, {}, {QUrl::fromLocalFile(QStringLiteral("$18"))}}
        };

        newCovers[QStringLiteral("album1")] = QUrl::fromLocalFile(QStringLiteral("album1"));
        newCovers[QStringLiteral("album2")] = QUrl::fromLocalFile(QStringLiteral("album2"));
        newCovers[QStringLiteral("album3")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("album4")] = QUrl::fromLocalFile(QStringLiteral("album4"));

        QMetaObject::invokeMethod(&musicDbThread1, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));
        QMetaObject::invokeMethod(&musicDbThread1, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));
        QMetaObject::invokeMethod(&musicDbThread1, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));
        QMetaObject::invokeMethod(&musicDbThread1, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));

        QMetaObject::invokeMethod(&musicDbThread2, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));
        QMetaObject::invokeMethod(&musicDbThread2, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));
        QMetaObject::invokeMethod(&musicDbThread2, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));
        QMetaObject::invokeMethod(&musicDbThread2, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));

        QMetaObject::invokeMethod(&musicDbThread3, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));
        QMetaObject::invokeMethod(&musicDbThread3, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));
        QMetaObject::invokeMethod(&musicDbThread3, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));
        QMetaObject::invokeMethod(&musicDbThread3, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));

        QMetaObject::invokeMethod(&musicDbThread4, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));
        QMetaObject::invokeMethod(&musicDbThread4, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));
        QMetaObject::invokeMethod(&musicDbThread4, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));
        QMetaObject::invokeMethod(&musicDbThread4, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", newTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", newCovers));

        while(musicDbdatabaseChanged1Spy.count() < 4) {
            QCOMPARE(musicDbdatabaseChanged1Spy.wait(200), true);
        }
        qDebug() << "thread 1 finished";

        while(musicDbdatabaseChanged2Spy.count() < 4) {
            QCOMPARE(musicDbdatabaseChanged2Spy.wait(200), true);
        }
        qDebug() << "thread 2 finished";

        while(musicDbdatabaseChanged3Spy.count() < 4) {
            QCOMPARE(musicDbdatabaseChanged3Spy.wait(200), true);
        }
        qDebug() << "thread 3 finished";

        while(musicDbdatabaseChanged4Spy.count() < 4) {
            QCOMPARE(musicDbdatabaseChanged4Spy.wait(200), true);
        }
        qDebug() << "thread 4 finished";

        thread1.quit();
        thread2.quit();
        thread3.quit();
        thread4.quit();

        if (thread1.isRunning()) {
            QCOMPARE(thread1FinishedSpy.wait(200), true);
        }
        if (thread2.isRunning()) {
            QCOMPARE(thread2FinishedSpy.wait(200), true);
        }
        if (thread3.isRunning()) {
            QCOMPARE(thread3FinishedSpy.wait(200), true);
        }
        if (thread4.isRunning()) {
            QCOMPARE(thread4FinishedSpy.wait(200), true);
        }

        QCOMPARE(musicDb.albumCount({}), 4);
    }
};

QTEST_MAIN(DatabaseInterfaceTests)


#include "databaseinterfacetest.moc"
