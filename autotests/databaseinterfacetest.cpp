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

private:

    QHash<QString, QVector<MusicAudioTrack>> mNewTracks;
    QHash<QString, QUrl> mNewCovers;

private Q_SLOTS:

    void initTestCase()
    {
        mNewTracks[QStringLiteral("album1")] = {
            {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 1, 1, QTime::fromMSecsSinceStartOfDay(1), {QUrl::fromLocalFile(QStringLiteral("$1"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$1"))}},
            {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 2, 2, QTime::fromMSecsSinceStartOfDay(2), {QUrl::fromLocalFile(QStringLiteral("$2"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$2"))}},
            {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 3, 3, QTime::fromMSecsSinceStartOfDay(3), {QUrl::fromLocalFile(QStringLiteral("$3"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$3"))}},
            {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist4"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 4, 4, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("$4"))},
                {QUrl::fromLocalFile(QStringLiteral("file://image$4"))}},
        };

        mNewTracks[QStringLiteral("album2")] = {
            {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, 1, QTime::fromMSecsSinceStartOfDay(5), {QUrl::fromLocalFile(QStringLiteral("$5"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$5"))}},
            {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 2, 1, QTime::fromMSecsSinceStartOfDay(6), {QUrl::fromLocalFile(QStringLiteral("$6"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}},
            {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 3, 1, QTime::fromMSecsSinceStartOfDay(7), {QUrl::fromLocalFile(QStringLiteral("$7"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$7"))}},
            {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 4, 1, QTime::fromMSecsSinceStartOfDay(8), {QUrl::fromLocalFile(QStringLiteral("$8"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$8"))}},
            {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 5, 1, QTime::fromMSecsSinceStartOfDay(9), {QUrl::fromLocalFile(QStringLiteral("$9"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$9"))}},
            {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), QStringLiteral("artist1"), 6, 1, QTime::fromMSecsSinceStartOfDay(10), {QUrl::fromLocalFile(QStringLiteral("$10"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$10"))}}
        };

        mNewTracks[QStringLiteral("album3")] = {
            {true, QStringLiteral("$11"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 1, 1, QTime::fromMSecsSinceStartOfDay(11), {QUrl::fromLocalFile(QStringLiteral("$11"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$11"))}},
            {true, QStringLiteral("$12"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 2, 1, QTime::fromMSecsSinceStartOfDay(12), {QUrl::fromLocalFile(QStringLiteral("$12"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$12"))}},
            {true, QStringLiteral("$13"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 3, 1, QTime::fromMSecsSinceStartOfDay(13), {QUrl::fromLocalFile(QStringLiteral("$13"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$13"))}}
        };

        mNewTracks[QStringLiteral("album4")] = {
            {true, QStringLiteral("$14"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 1, 1, QTime::fromMSecsSinceStartOfDay(14), {QUrl::fromLocalFile(QStringLiteral("$14"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$14"))}},
            {true, QStringLiteral("$15"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 2, 1, QTime::fromMSecsSinceStartOfDay(15), {QUrl::fromLocalFile(QStringLiteral("$15"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$15"))}},
            {true, QStringLiteral("$16"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 3, 1, QTime::fromMSecsSinceStartOfDay(16), {QUrl::fromLocalFile(QStringLiteral("$16"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$16"))}},
            {true, QStringLiteral("$17"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 4, 1, QTime::fromMSecsSinceStartOfDay(17), {QUrl::fromLocalFile(QStringLiteral("$17"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$17"))}},
            {true, QStringLiteral("$18"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"), 5, 1, QTime::fromMSecsSinceStartOfDay(18), {QUrl::fromLocalFile(QStringLiteral("$18"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$18"))}}
        };

        mNewCovers[QStringLiteral("album1")] = QUrl::fromLocalFile(QStringLiteral("album1"));
        mNewCovers[QStringLiteral("album2")] = QUrl::fromLocalFile(QStringLiteral("album2"));
        mNewCovers[QStringLiteral("album3")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        mNewCovers[QStringLiteral("album4")] = QUrl::fromLocalFile(QStringLiteral("album4"));

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

    void avoidCrashInAllArtists()
    {
        DatabaseInterface musicDb;
        musicDb.allArtists();
    }

    void avoidCrashInAllAlbums()
    {
        DatabaseInterface musicDb;
        musicDb.allAlbums({});
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

        musicDb.init(QStringLiteral("testDb"));

        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(musicDb.allAlbums({}).count(), 4);

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

        musicDb.init(QStringLiteral("testDb"));

        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(musicDb.allAlbums({}).count(), 4);

        auto allAlbums = musicDb.allAlbums({});

        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = allAlbums[0].tracksCount();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album1"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("Various Artists"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstAlbumTracksCount, 4);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, false);

        auto invalidTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("invalidArtist1"));
        QCOMPARE(invalidTrackId, decltype(invalidTrackId)(0));

        auto trackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));

        auto firstTrack = musicDb.trackFromDatabaseId(trackId);

        auto firstTrackTitle = firstTrack.title();
        auto firstTrackArtist = firstTrack.artist();
        auto firstTrackAlbumArtist = firstTrack.albumArtist();
        auto firstTrackAlbum = firstTrack.albumName();
        auto firstTrackImage = firstTrack.albumCover();
        auto firstTrackDuration = firstTrack.duration();
        auto firstTrackMilliSecondsDuration = firstTrack.duration().msecsSinceStartOfDay();
        auto firstTrackTrackNumber = firstTrack.trackNumber();
        auto firstTrackDiscNumber = firstTrack.discNumber();
        auto firstTrackResource = firstTrack.resourceURI();

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrackTitle, QStringLiteral("track1"));
        QCOMPARE(firstTrackArtist, QStringLiteral("artist1"));
        QCOMPARE(firstTrackAlbumArtist, QStringLiteral("Various Artists"));
        QCOMPARE(firstTrackAlbum, QStringLiteral("album1"));
        QCOMPARE(firstTrackImage.isValid(), true);
        QCOMPARE(firstTrackImage, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstTrackDuration, QTime::fromMSecsSinceStartOfDay(1));
        QCOMPARE(firstTrackMilliSecondsDuration, 1);
        QCOMPARE(firstTrackTrackNumber, 1);
        QCOMPARE(firstTrackDiscNumber, 1);
        QCOMPARE(firstTrackResource.isValid(), true);
        QCOMPARE(firstTrackResource, QUrl::fromLocalFile(QStringLiteral("$1")));

        auto secondAlbumTitle = allAlbums[1].title();
        auto secondAlbumArtist = allAlbums[1].artist();
        auto secondAlbumImage = allAlbums[1].albumArtURI();
        auto secondAlbumTracksCount = allAlbums[1].tracksCount();
        auto secondAlbumIsSingleDiscAlbum = allAlbums[1].isSingleDiscAlbum();

        QCOMPARE(secondAlbumTitle, QStringLiteral("album2"));
        QCOMPARE(secondAlbumArtist, QStringLiteral("artist1"));
        QCOMPARE(secondAlbumImage.isValid(), true);
        QCOMPARE(secondAlbumImage, QUrl::fromLocalFile(QStringLiteral("album2")));
        QCOMPARE(secondAlbumTracksCount, 6);
        QCOMPARE(secondAlbumIsSingleDiscAlbum, true);
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

        musicDb.init(QStringLiteral("testDb"));

        clientDb.init(QStringLiteral("clientDb"));

        QSignalSpy musicDbChangedSpy(&musicDb, &DatabaseInterface::databaseChanged);

        connect(&musicDb, &DatabaseInterface::databaseChanged,
                &clientDb, &DatabaseInterface::databaseHasChanged);

        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(musicDb.allAlbums({}).count(), 4);
        QCOMPARE(clientDb.allAlbums({}).count(), 4);
        QCOMPARE(musicDbChangedSpy.count(), 4);

        auto allAlbums = musicDb.allAlbums({});

        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = allAlbums[0].tracksCount();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album1"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("Various Artists"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstAlbumTracksCount, 4);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, false);

        auto invalidTrackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("invalidArtist1"));
        QCOMPARE(invalidTrackId, decltype(invalidTrackId)(0));

        auto trackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));
        auto firstTrack = musicDb.trackFromDatabaseId(trackId);

        auto firstTrackTitle = firstTrack.title();
        auto firstTrackArtist = firstTrack.artist();
        auto firstTrackAlbumArtist = firstTrack.albumArtist();
        auto firstTrackAlbum = firstTrack.albumName();
        auto firstTrackImage = firstTrack.albumCover();
        auto firstTrackDuration = firstTrack.duration();
        auto firstTrackMilliSecondsDuration = firstTrack.duration().msecsSinceStartOfDay();
        auto firstTrackTrackNumber = firstTrack.trackNumber();
        auto firstTrackDiscNumber = firstTrack.discNumber();
        auto firstTrackResource = firstTrack.resourceURI();

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrackTitle, QStringLiteral("track1"));
        QCOMPARE(firstTrackArtist, QStringLiteral("artist1"));
        QCOMPARE(firstTrackAlbumArtist, QStringLiteral("Various Artists"));
        QCOMPARE(firstTrackAlbum, QStringLiteral("album1"));
        QCOMPARE(firstTrackImage.isValid(), true);
        QCOMPARE(firstTrackImage, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstTrackDuration, QTime::fromMSecsSinceStartOfDay(1));
        QCOMPARE(firstTrackMilliSecondsDuration, 1);
        QCOMPARE(firstTrackTrackNumber, 1);
        QCOMPARE(firstTrackDiscNumber, 1);
        QCOMPARE(firstTrackResource.isValid(), true);
        QCOMPARE(firstTrackResource, QUrl::fromLocalFile(QStringLiteral("$1")));

        auto secondAlbumTitle = allAlbums[1].title();
        auto secondAlbumArtist = allAlbums[1].artist();
        auto secondAlbumImage = allAlbums[1].albumArtURI();
        auto secondAlbumTracksCount = allAlbums[1].tracksCount();
        auto secondAlbumIsSingleDiscAlbum = allAlbums[1].isSingleDiscAlbum();

        QCOMPARE(secondAlbumTitle, QStringLiteral("album2"));
        QCOMPARE(secondAlbumArtist, QStringLiteral("artist1"));
        QCOMPARE(secondAlbumImage.isValid(), true);
        QCOMPARE(secondAlbumImage, QUrl::fromLocalFile(QStringLiteral("album2")));
        QCOMPARE(secondAlbumTracksCount, 6);
        QCOMPARE(secondAlbumIsSingleDiscAlbum, true);
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

        musicDb.init(QStringLiteral("testDbVariousArtistAlbum"));

        clientDb.init(QStringLiteral("clientDbVariousArtistAlbum"));

        QSignalSpy musicDbChangedSpy(&musicDb, &DatabaseInterface::databaseChanged);

        connect(&musicDb, &DatabaseInterface::databaseChanged,
                &clientDb, &DatabaseInterface::databaseHasChanged);

        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(musicDb.allAlbums({}).count(), 4);
        QCOMPARE(clientDb.allAlbums({}).count(), 4);
        QCOMPARE(musicDbChangedSpy.count(), 4);

        auto allAlbums = musicDb.allAlbums({});

        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = allAlbums[0].tracksCount();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album1"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("Various Artists"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstAlbumTracksCount, 4);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, false);

        auto invalidTrackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("invalidArtist1"));
        QCOMPARE(invalidTrackId, decltype(invalidTrackId)(0));

        auto firstTrackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));
        auto firstTrack = musicDb.trackFromDatabaseId(firstTrackId);

        auto firstTrackTitle = firstTrack.title();
        auto firstTrackArtist = firstTrack.artist();
        auto firstTrackAlbumArtist = firstTrack.albumArtist();
        auto firstTrackAlbum = firstTrack.albumName();
        auto firstTrackImage = firstTrack.albumCover();
        auto firstTrackDuration = firstTrack.duration();
        auto firstTrackMilliSecondsDuration = firstTrack.duration().msecsSinceStartOfDay();
        auto firstTrackTrackNumber = firstTrack.trackNumber();
        auto firstTrackDiscNumber = firstTrack.discNumber();
        auto firstTrackResource = firstTrack.resourceURI();

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrackTitle, QStringLiteral("track1"));
        QCOMPARE(firstTrackArtist, QStringLiteral("artist1"));
        QCOMPARE(firstTrackAlbumArtist, QStringLiteral("Various Artists"));
        QCOMPARE(firstTrackAlbum, QStringLiteral("album1"));
        QCOMPARE(firstTrackImage.isValid(), true);
        QCOMPARE(firstTrackImage, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstTrackDuration, QTime::fromMSecsSinceStartOfDay(1));
        QCOMPARE(firstTrackMilliSecondsDuration, 1);
        QCOMPARE(firstTrackTrackNumber, 1);
        QCOMPARE(firstTrackDiscNumber, 1);
        QCOMPARE(firstTrackResource.isValid(), true);
        QCOMPARE(firstTrackResource, QUrl::fromLocalFile(QStringLiteral("$1")));

        auto secondTrackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track2"), QStringLiteral("album1"), QStringLiteral("artist2"));
        auto secondTrack = musicDb.trackFromDatabaseId(secondTrackId);

        auto secondTrackTitle = secondTrack.title();
        auto secondTrackArtist = secondTrack.artist();
        auto secondTrackAlbumArtist = secondTrack.albumArtist();
        auto secondTrackAlbum = secondTrack.albumName();
        auto seconfTrackImage = secondTrack.albumCover();
        auto secondTrackDuration = secondTrack.duration();
        auto secondTrackMilliSecondsDuration = secondTrack.duration().msecsSinceStartOfDay();
        auto secondTrackTrackNumber = secondTrack.trackNumber();
        auto secondTrackDiscNumber = secondTrack.discNumber();
        auto secondTrackResource = secondTrack.resourceURI();

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrackTitle, QStringLiteral("track2"));
        QCOMPARE(secondTrackArtist, QStringLiteral("artist2"));
        QCOMPARE(secondTrackAlbumArtist, QStringLiteral("Various Artists"));
        QCOMPARE(secondTrackAlbum, QStringLiteral("album1"));
        QCOMPARE(seconfTrackImage.isValid(), true);
        QCOMPARE(seconfTrackImage, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(secondTrackDuration, QTime::fromMSecsSinceStartOfDay(2));
        QCOMPARE(secondTrackMilliSecondsDuration, 2);
        QCOMPARE(secondTrackTrackNumber, 2);
        QCOMPARE(secondTrackDiscNumber, 2);
        QCOMPARE(secondTrackResource.isValid(), true);
        QCOMPARE(secondTrackResource, QUrl::fromLocalFile(QStringLiteral("$2")));

        auto thirdTrackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist3"));
        auto thirdTrack = musicDb.trackFromDatabaseId(thirdTrackId);

        auto thirdTrackTitle = thirdTrack.title();
        auto thirdTrackArtist = thirdTrack.artist();
        auto thirdTrackAlbumArtist = thirdTrack.albumArtist();
        auto thirdTrackAlbum = thirdTrack.albumName();
        auto thirdTrackImage = thirdTrack.albumCover();
        auto thirdTrackDuration = thirdTrack.duration();
        auto thirdTrackMilliSecondsDuration = thirdTrack.duration().msecsSinceStartOfDay();
        auto thirdTrackTrackNumber = thirdTrack.trackNumber();
        auto thirdTrackDiscNumber = thirdTrack.discNumber();
        auto thirdTrackResource = thirdTrack.resourceURI();

        QCOMPARE(thirdTrack.isValid(), true);
        QCOMPARE(thirdTrackTitle, QStringLiteral("track3"));
        QCOMPARE(thirdTrackArtist, QStringLiteral("artist3"));
        QCOMPARE(thirdTrackAlbumArtist, QStringLiteral("Various Artists"));
        QCOMPARE(thirdTrackAlbum, QStringLiteral("album1"));
        QCOMPARE(thirdTrackImage.isValid(), true);
        QCOMPARE(thirdTrackImage, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(thirdTrackDuration, QTime::fromMSecsSinceStartOfDay(3));
        QCOMPARE(thirdTrackMilliSecondsDuration, 3);
        QCOMPARE(thirdTrackTrackNumber, 3);
        QCOMPARE(thirdTrackDiscNumber, 3);
        QCOMPARE(thirdTrackResource.isValid(), true);
        QCOMPARE(thirdTrackResource, QUrl::fromLocalFile(QStringLiteral("$3")));

        auto fourthTrackId = clientDb.trackIdFromTitleAlbumArtist(QStringLiteral("track4"), QStringLiteral("album1"), QStringLiteral("artist4"));
        auto fourthTrack = musicDb.trackFromDatabaseId(fourthTrackId);

        auto fourthTrackTitle = fourthTrack.title();
        auto fourthTrackArtist = fourthTrack.artist();
        auto fourthTrackAlbumArtist = fourthTrack.albumArtist();
        auto fourthTrackAlbum = fourthTrack.albumName();
        auto fourthTrackImage = fourthTrack.albumCover();
        auto fourthTrackDuration = fourthTrack.duration();
        auto fourthTrackMilliSecondsDuration = fourthTrack.duration().msecsSinceStartOfDay();
        auto fourthTrackTrackNumber = fourthTrack.trackNumber();
        auto fourthTrackDiscNumber = fourthTrack.discNumber();
        auto fourthTrackResource = fourthTrack.resourceURI();

        QCOMPARE(fourthTrack.isValid(), true);
        QCOMPARE(fourthTrackTitle, QStringLiteral("track4"));
        QCOMPARE(fourthTrackArtist, QStringLiteral("artist4"));
        QCOMPARE(fourthTrackAlbumArtist, QStringLiteral("Various Artists"));
        QCOMPARE(fourthTrackAlbum, QStringLiteral("album1"));
        QCOMPARE(fourthTrackImage.isValid(), true);
        QCOMPARE(fourthTrackImage, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(fourthTrackDuration, QTime::fromMSecsSinceStartOfDay(4));
        QCOMPARE(fourthTrackMilliSecondsDuration, 4);
        QCOMPARE(fourthTrackTrackNumber, 4);
        QCOMPARE(fourthTrackDiscNumber, 4);
        QCOMPARE(fourthTrackResource.isValid(), true);
        QCOMPARE(fourthTrackResource, QUrl::fromLocalFile(QStringLiteral("$4")));

        auto secondAlbumTitle = allAlbums[1].title();
        auto secondAlbumArtist = allAlbums[1].artist();
        auto secondAlbumImage = allAlbums[1].albumArtURI();
        auto secondAlbumTracksCount = allAlbums[1].tracksCount();
        auto secondAlbumIsSingleDiscAlbum = allAlbums[1].isSingleDiscAlbum();

        QCOMPARE(secondAlbumTitle, QStringLiteral("album2"));
        QCOMPARE(secondAlbumArtist, QStringLiteral("artist1"));
        QCOMPARE(secondAlbumImage.isValid(), true);
        QCOMPARE(secondAlbumImage, QUrl::fromLocalFile(QStringLiteral("album2")));
        QCOMPARE(secondAlbumTracksCount, 6);
        QCOMPARE(secondAlbumIsSingleDiscAlbum, true);
    }

    void testTracksFromAuthor() {
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

        musicDb.init(QStringLiteral("testDbVariousArtistAlbum"));

        clientDb.init(QStringLiteral("clientDbVariousArtistAlbum"));

        QSignalSpy musicDbChangedSpy(&musicDb, &DatabaseInterface::databaseChanged);

        connect(&musicDb, &DatabaseInterface::databaseChanged,
                &clientDb, &DatabaseInterface::databaseHasChanged);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(musicDb.allAlbums({}).count(), 4);
        QCOMPARE(clientDb.allAlbums({}).count(), 4);
        QCOMPARE(musicDbChangedSpy.count(), 1);

        auto allTracks = clientDb.tracksFromAuthor(QStringLiteral("artist1"));

        QCOMPARE(allTracks.size(), 6);
        QCOMPARE(allTracks[0].albumArtist(), QStringLiteral("Various Artists"));
        QCOMPARE(allTracks[1].albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allTracks[2].albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allTracks[3].albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allTracks[4].albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allTracks[5].albumArtist(), QStringLiteral("artist1"));
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

        QMetaObject::invokeMethod(&musicDbThread1, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));
        QMetaObject::invokeMethod(&musicDbThread1, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));
        QMetaObject::invokeMethod(&musicDbThread1, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));
        QMetaObject::invokeMethod(&musicDbThread1, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));

        QMetaObject::invokeMethod(&musicDbThread2, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));
        QMetaObject::invokeMethod(&musicDbThread2, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));
        QMetaObject::invokeMethod(&musicDbThread2, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));
        QMetaObject::invokeMethod(&musicDbThread2, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));

        QMetaObject::invokeMethod(&musicDbThread3, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));
        QMetaObject::invokeMethod(&musicDbThread3, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));
        QMetaObject::invokeMethod(&musicDbThread3, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));
        QMetaObject::invokeMethod(&musicDbThread3, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));

        QMetaObject::invokeMethod(&musicDbThread4, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));
        QMetaObject::invokeMethod(&musicDbThread4, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));
        QMetaObject::invokeMethod(&musicDbThread4, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));
        QMetaObject::invokeMethod(&musicDbThread4, "insertTracksList",
                                  QArgument<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>", mNewTracks),
                                  QArgument<QHash<QString, QUrl>>("QHash<QString,QUrl>", mNewCovers));

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

        QCOMPARE(musicDb.allAlbums({}).count(), 4);
    }

    void testAlbumsFromAuthor() {
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

        musicDb.init(QStringLiteral("testDbVariousArtistAlbum"));

        clientDb.init(QStringLiteral("clientDbVariousArtistAlbum"));

        QSignalSpy musicDbChangedSpy(&musicDb, &DatabaseInterface::databaseChanged);

        connect(&musicDb, &DatabaseInterface::databaseChanged,
                &clientDb, &DatabaseInterface::databaseHasChanged);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(musicDb.allAlbums({}).count(), 4);
        QCOMPARE(clientDb.allAlbums({}).count(), 4);
        QCOMPARE(musicDbChangedSpy.count(), 1);

        auto allAlbums = clientDb.allAlbumsFromArtist(QStringLiteral("artist1"));

        QCOMPARE(allAlbums.size(), 2);

        QCOMPARE(allAlbums[0].artist(), QStringLiteral("Various Artists"));
        QCOMPARE(allAlbums[0].tracksCount(), 4);
        QCOMPARE(allAlbums[0].trackFromIndex(0).albumArtist(), QStringLiteral("Various Artists"));
        QCOMPARE(allAlbums[0].trackFromIndex(1).albumArtist(), QStringLiteral("Various Artists"));
        QCOMPARE(allAlbums[0].trackFromIndex(2).albumArtist(), QStringLiteral("Various Artists"));
        QCOMPARE(allAlbums[0].trackFromIndex(3).albumArtist(), QStringLiteral("Various Artists"));
        QCOMPARE(allAlbums[1].artist(), QStringLiteral("artist1"));
        QCOMPARE(allAlbums[1].tracksCount(), 6);
        QCOMPARE(allAlbums[1].trackFromIndex(0).albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allAlbums[1].trackFromIndex(1).albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allAlbums[1].trackFromIndex(2).albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allAlbums[1].trackFromIndex(3).albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allAlbums[1].trackFromIndex(4).albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allAlbums[1].trackFromIndex(5).albumArtist(), QStringLiteral("artist1"));
    }
};

QTEST_MAIN(DatabaseInterfaceTests)


#include "databaseinterfacetest.moc"
