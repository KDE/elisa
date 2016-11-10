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

#include <QUrl>
#include <QString>
#include <QHash>
#include <QVector>
#include <QThread>
#include <QMetaObject>

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
        DatabaseInterface musicDb;
        auto newTracks = QHash<QString, QVector<MusicAudioTrack>>();
        auto newCovers = QHash<QString, QUrl>();

        musicDb.init(QStringLiteral("testDb"));
        musicDb.initDatabase();
        musicDb.initRequest();

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
        DatabaseInterface musicDb;
        auto newTracks = QHash<QString, QVector<MusicAudioTrack>>();
        auto newCovers = QHash<QString, QUrl>();

        musicDb.init(QStringLiteral("testDb"));
        musicDb.initDatabase();
        musicDb.initRequest();

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

        auto firstAlbumTitle = musicDb.albumDataFromIndex(0, DatabaseInterface::AlbumData::Title);
        auto firstAlbumArtist = musicDb.albumDataFromIndex(0, DatabaseInterface::AlbumData::Artist);
        auto firstAlbumImage = musicDb.albumDataFromIndex(0, DatabaseInterface::AlbumData::Image);
        auto firstAlbumTracksCount = musicDb.albumDataFromIndex(0, DatabaseInterface::AlbumData::TracksCount);
        auto firstAlbumId = musicDb.albumDataFromIndex(0, DatabaseInterface::AlbumData::Id);

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

        auto firstInvalidAlbumTitle = musicDb.albumDataFromIndex(-1, DatabaseInterface::AlbumData::Title);
        QCOMPARE(firstInvalidAlbumTitle.isValid(), false);

        auto secondInvalidAlbumTitle = musicDb.albumDataFromIndex(4, DatabaseInterface::AlbumData::Title);
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
        DatabaseInterface musicDb;
        DatabaseInterface clientDb;

        auto newTracks = QHash<QString, QVector<MusicAudioTrack>>();
        auto newCovers = QHash<QString, QUrl>();

        musicDb.init(QStringLiteral("testDb"));
        musicDb.initDatabase();
        musicDb.initRequest();

        clientDb.init(QStringLiteral("clientDb"));
        clientDb.initDatabase();
        clientDb.initRequest();

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

        auto firstAlbumTitle = musicDb.albumDataFromIndex(0, DatabaseInterface::AlbumData::Title);
        auto firstAlbumArtist = musicDb.albumDataFromIndex(0, DatabaseInterface::AlbumData::Artist);
        auto firstAlbumImage = musicDb.albumDataFromIndex(0, DatabaseInterface::AlbumData::Image);
        auto firstAlbumTracksCount = musicDb.albumDataFromIndex(0, DatabaseInterface::AlbumData::TracksCount);
        auto firstAlbumId = musicDb.albumDataFromIndex(0, DatabaseInterface::AlbumData::Id);

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

        auto firstInvalidAlbumTitle = clientDb.albumDataFromIndex(-1, DatabaseInterface::AlbumData::Title);
        QCOMPARE(firstInvalidAlbumTitle.isValid(), false);

        auto secondInvalidAlbumTitle = clientDb.albumDataFromIndex(4, DatabaseInterface::AlbumData::Title);
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

    void addMultipleTimeSameTracksMultiThread()
    {
#if 0
        DatabaseInterface musicDb;
        musicDb.init(QStringLiteral("testDb"));
        musicDb.initDatabase();
        musicDb.initRequest();

        DatabaseInterface musicDbThread1;
        QThread thread1;
        musicDbThread1.moveToThread(&thread1);
        thread1.start();
        QMetaObject::invokeMethod(&musicDbThread1, "init", Qt::QueuedConnection, Q_ARG(QString, QStringLiteral("testDb1")));
        QMetaObject::invokeMethod(&musicDbThread1, "initRequest", Qt::QueuedConnection);

        DatabaseInterface musicDbThread2;
        QThread thread2;
        musicDbThread2.moveToThread(&thread2);
        thread2.start();
        QMetaObject::invokeMethod(&musicDbThread2, "init", Qt::QueuedConnection, Q_ARG(QString, QStringLiteral("testDb2")));
        QMetaObject::invokeMethod(&musicDbThread2, "initRequest", Qt::QueuedConnection);

        DatabaseInterface musicDbThread3;
        QThread thread3;
        musicDbThread3.moveToThread(&thread3);
        thread3.start();
        QMetaObject::invokeMethod(&musicDbThread3, "init", Qt::QueuedConnection, Q_ARG(QString, QStringLiteral("testDb3")));
        QMetaObject::invokeMethod(&musicDbThread3, "initRequest", Qt::QueuedConnection);

        DatabaseInterface musicDbThread4;
        QThread thread4;
        musicDbThread4.moveToThread(&thread4);
        thread4.start();
        QMetaObject::invokeMethod(&musicDbThread4, "init", Qt::QueuedConnection, Q_ARG(QString, QStringLiteral("testDb4")));
        QMetaObject::invokeMethod(&musicDbThread4, "initRequest", Qt::QueuedConnection);

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

        qDebug() << musicDb.albumCount();
        QVERIFY(musicDb.albumCount() == 4);
#endif
    }
};

QTEST_MAIN(DatabaseInterfaceTests)


#include "databaseinterfacetest.moc"
