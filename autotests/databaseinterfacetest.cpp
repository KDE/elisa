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
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTemporaryFile>

#include <QDebug>

#include <QtTest>

#include <algorithm>

class DatabaseInterfaceTests: public QObject
{
    Q_OBJECT

private:

    QList<MusicAudioTrack> mNewTracks = {
        {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
         QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
         1, 1, QTime::fromMSecsSinceStartOfDay(1), {QUrl::fromLocalFile(QStringLiteral("/$1"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$1"))}, 1, false,
         QStringLiteral("genre1"), QStringLiteral("composer1")},
        {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
         QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
         2, 2, QTime::fromMSecsSinceStartOfDay(2), {QUrl::fromLocalFile(QStringLiteral("/$2"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$2"))}, 2, false,
         QStringLiteral("genre2"), QStringLiteral("composer1")},
        {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
         QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
         3, 3, QTime::fromMSecsSinceStartOfDay(3), {QUrl::fromLocalFile(QStringLiteral("/$3"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$3"))}, 3, false,
         QStringLiteral("genre3"), QStringLiteral("composer1")},
        {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
         QStringLiteral("artist4"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
         4, 4, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("/$4"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$4"))}, 4, false,
         QStringLiteral("genre4"), QStringLiteral("composer1")},
        {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
         QStringLiteral("artist4"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
         4, 4, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("/$4Bis"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$4"))}, 4, false,
         QStringLiteral("genre4"), QStringLiteral("composer1")},
        {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"),
         1, 1, QTime::fromMSecsSinceStartOfDay(5), {QUrl::fromLocalFile(QStringLiteral("/$5"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$5"))}, 4, true,
         QStringLiteral("genre1"), QStringLiteral("composer1")},
        {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"),
         2, 1, QTime::fromMSecsSinceStartOfDay(6), {QUrl::fromLocalFile(QStringLiteral("/$6"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 1, true,
         QStringLiteral("genre1"), QStringLiteral("composer1")},
        {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"),
         3, 1, QTime::fromMSecsSinceStartOfDay(7), {QUrl::fromLocalFile(QStringLiteral("/$7"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$7"))}, 5, true,
         QStringLiteral("genre2"), QStringLiteral("composer1")},
        {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"),
         4, 1, QTime::fromMSecsSinceStartOfDay(8), {QUrl::fromLocalFile(QStringLiteral("/$8"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$8"))}, 2, true,
         QStringLiteral("genre2"), QStringLiteral("composer1")},
        {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"),
         5, 1, QTime::fromMSecsSinceStartOfDay(9), {QUrl::fromLocalFile(QStringLiteral("/$9"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$9"))}, 3, true,
         QStringLiteral("genre2"), QStringLiteral("composer1")},
        {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
         QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), QStringLiteral("artist1"),
         6, 1, QTime::fromMSecsSinceStartOfDay(10), {QUrl::fromLocalFile(QStringLiteral("/$10"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$10"))}, 5, true,
         QStringLiteral("genre2"), QStringLiteral("composer1")},
        {true, QStringLiteral("$11"), QStringLiteral("0"), QStringLiteral("track1"),
         QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
         1, 1, QTime::fromMSecsSinceStartOfDay(11), {QUrl::fromLocalFile(QStringLiteral("/$11"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$11"))}, 1, true,
         QStringLiteral("genre3"), QStringLiteral("composer1")},
        {true, QStringLiteral("$12"), QStringLiteral("0"), QStringLiteral("track2"),
         QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
         2, 1, QTime::fromMSecsSinceStartOfDay(12), {QUrl::fromLocalFile(QStringLiteral("/$12"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$12"))}, 2, true,
         QStringLiteral("genre3"), QStringLiteral("composer1")},
        {true, QStringLiteral("$13"), QStringLiteral("0"), QStringLiteral("track3"),
         QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
         3, 1, QTime::fromMSecsSinceStartOfDay(13), {QUrl::fromLocalFile(QStringLiteral("/$13"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$13"))}, 3, true,
         QStringLiteral("genre3"), QStringLiteral("composer1")},
        {true, QStringLiteral("$14"), QStringLiteral("0"), QStringLiteral("track1"),
         QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"),
         1, 1, QTime::fromMSecsSinceStartOfDay(14), {QUrl::fromLocalFile(QStringLiteral("/$14"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$14"))}, 4, true,
         QStringLiteral("genre4"), QStringLiteral("composer1")},
        {true, QStringLiteral("$15"), QStringLiteral("0"), QStringLiteral("track2"),
         QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"),
         2, 1, QTime::fromMSecsSinceStartOfDay(15), {QUrl::fromLocalFile(QStringLiteral("/$15"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$15"))}, 5, true,
         QStringLiteral("genre4"), QStringLiteral("composer1")},
        {true, QStringLiteral("$16"), QStringLiteral("0"), QStringLiteral("track3"),
         QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"),
         3, 1, QTime::fromMSecsSinceStartOfDay(16), {QUrl::fromLocalFile(QStringLiteral("/$16"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$16"))}, 1, true,
         QStringLiteral("genre4"), QStringLiteral("composer1")},
        {true, QStringLiteral("$17"), QStringLiteral("0"), QStringLiteral("track4"),
         QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"),
         4, 1, QTime::fromMSecsSinceStartOfDay(17), {QUrl::fromLocalFile(QStringLiteral("/$17"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$17"))}, 2, true,
         QStringLiteral("genre4"), QStringLiteral("composer1")},
        {true, QStringLiteral("$18"), QStringLiteral("0"), QStringLiteral("track5"),
         QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral("artist2"),
         5, 1, QTime::fromMSecsSinceStartOfDay(18), {QUrl::fromLocalFile(QStringLiteral("/$18"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$18"))}, 3, true,
         QStringLiteral("genre3"), QStringLiteral("composer1")},
        {true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track1"),
         QStringLiteral("artist7"), QStringLiteral("album3"), QStringLiteral("artist7"),
         1, 1, QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$19"))}, 1, true,
         QStringLiteral("genre2"), QStringLiteral("composer1")},
        {true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track2"),
         QStringLiteral("artist7"), QStringLiteral("album3"), QStringLiteral("artist7"),
         2, 1, QTime::fromMSecsSinceStartOfDay(20), {QUrl::fromLocalFile(QStringLiteral("/$20"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$20"))}, 2, true,
         QStringLiteral("genre1"), QStringLiteral("composer1")},
        {true, QStringLiteral("$21"), QStringLiteral("0"), QStringLiteral("track3"),
         QStringLiteral("artist7"), QStringLiteral("album3"), QStringLiteral("artist7"),
         3, 1, QTime::fromMSecsSinceStartOfDay(21), {QUrl::fromLocalFile(QStringLiteral("/$21"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$21"))}, 3, true,
         QStringLiteral("genre1"), QStringLiteral("composer1")},
        {true, QStringLiteral("$22"), QStringLiteral("0"), QStringLiteral("track9"),
         QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist7"),
         9, 1, QTime::fromMSecsSinceStartOfDay(22), {QUrl::fromLocalFile(QStringLiteral("/$22"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$22"))}, 9, true,
         QStringLiteral("genre1"), QStringLiteral("composer1")},
    };

    QHash<QString, QUrl> mNewCovers = {
        {QStringLiteral("file:///$1"), QUrl::fromLocalFile(QStringLiteral("album1"))},
        {QStringLiteral("file:///$2"), QUrl::fromLocalFile(QStringLiteral("album1"))},
        {QStringLiteral("file:///$3"), QUrl::fromLocalFile(QStringLiteral("album1"))},
        {QStringLiteral("file:///$4"), QUrl::fromLocalFile(QStringLiteral("album1"))},
        {QStringLiteral("file:///$4Bis"), QUrl::fromLocalFile(QStringLiteral("album1"))},
        {QStringLiteral("file:///$5"), QUrl::fromLocalFile(QStringLiteral("album2"))},
        {QStringLiteral("file:///$6"), QUrl::fromLocalFile(QStringLiteral("album2"))},
        {QStringLiteral("file:///$7"), QUrl::fromLocalFile(QStringLiteral("album2"))},
        {QStringLiteral("file:///$8"), QUrl::fromLocalFile(QStringLiteral("album2"))},
        {QStringLiteral("file:///$9"), QUrl::fromLocalFile(QStringLiteral("album2"))},
        {QStringLiteral("file:///$0"), QUrl::fromLocalFile(QStringLiteral("album2"))},
        {QStringLiteral("file:///$11"), QUrl::fromLocalFile(QStringLiteral("album3"))},
        {QStringLiteral("file:///$12"), QUrl::fromLocalFile(QStringLiteral("album3"))},
        {QStringLiteral("file:///$13"), QUrl::fromLocalFile(QStringLiteral("album3"))},
    };

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QList<MusicAudioTrack>>("QList<MusicAudioTrack>");
        qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
        qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
        qRegisterMetaType<MusicArtist>("MusicArtist");
    }

    void avoidCrashInTrackIdFromTitleAlbumArtist()
    {
        DatabaseInterface musicDb;
        musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album3"), 1, 1);
    }

    void avoidCrashInAllArtists()
    {
        DatabaseInterface musicDb;
        musicDb.allArtists();
    }

    void avoidCrashInAllAlbums()
    {
        DatabaseInterface musicDb;
        musicDb.allAlbums();
    }

    void addOneTrackWithoutAlbumArtist()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), {},
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 1);
        QCOMPARE(musicDb.allArtists().count(), 1);
        QCOMPARE(musicDb.allTracks().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto track = musicDb.trackFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                                              QStringLiteral("album3"), 6, 1));

        QCOMPARE(track.isValid(), true);
        QCOMPARE(track.title(), QStringLiteral("track6"));
        QCOMPARE(track.artist(), QStringLiteral("artist2"));
        QCOMPARE(track.albumName(), QStringLiteral("album3"));
        QCOMPARE(track.albumArtist(), QStringLiteral("artist2"));
        QCOMPARE(track.isValidAlbumArtist(), false);
        QCOMPARE(track.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(track.trackNumber(), 6);
        QCOMPARE(track.discNumber(), 1);
        QCOMPARE(track.duration(), QTime::fromMSecsSinceStartOfDay(23));
        QCOMPARE(track.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$23")));
        QCOMPARE(track.rating(), 5);
        QCOMPARE(track.genre(), QStringLiteral("genre1"));
        QCOMPARE(track.composer(), QStringLiteral("composer1"));

        auto album = musicDb.albumFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist2"));

        QCOMPARE(album.isValid(), true);
        QCOMPARE(album.tracksCount(), 1);
        QCOMPARE(album.title(), QStringLiteral("album3"));
        QCOMPARE(album.artist(), QStringLiteral("artist2"));
        QCOMPARE(album.isValidArtist(), false);
        QCOMPARE(album.albumArtURI(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(album.isSingleDiscAlbum(), true);
    }

    void addTwoTracksWithoutAlbumArtist()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addTwoTracksWithoutAlbumArtist" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = QList<MusicAudioTrack>();

        newTracks = {{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                      QStringLiteral("artist2"), QStringLiteral("album3"), {}, 6, 1, QTime::fromMSecsSinceStartOfDay(19),
                      {QUrl::fromLocalFile(QStringLiteral("/$19"))},
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")},
                     {true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track7"),
                      QStringLiteral("artist3"), QStringLiteral("album3"), {}, 7, 1, QTime::fromMSecsSinceStartOfDay(20),
                      {QUrl::fromLocalFile(QStringLiteral("/$20"))},
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("file:///$20")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 1);
        QCOMPARE(musicDb.allArtists().count(), 2);
        QCOMPARE(musicDb.allTracks().count(), 2);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrack = musicDb.trackFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                                                   QStringLiteral("album3"), 6, 1));

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrack.title(), QStringLiteral("track6"));
        QCOMPARE(firstTrack.artist(), QStringLiteral("artist2"));
        QCOMPARE(firstTrack.albumName(), QStringLiteral("album3"));
        QCOMPARE(firstTrack.albumArtist(), QStringLiteral("artist2"));
        QCOMPARE(firstTrack.isValidAlbumArtist(), false);
        QCOMPARE(firstTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstTrack.trackNumber(), 6);
        QCOMPARE(firstTrack.discNumber(), 1);
        QCOMPARE(firstTrack.duration(), QTime::fromMSecsSinceStartOfDay(19));
        QCOMPARE(firstTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$19")));
        QCOMPARE(firstTrack.rating(), 5);
        QCOMPARE(firstTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(firstTrack.composer(), QStringLiteral("composer1"));

        auto secondTrack = musicDb.trackFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track7"), QStringLiteral("artist3"),
                                                                                                    QStringLiteral("album3"), 7, 1));

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrack.title(), QStringLiteral("track7"));
        QCOMPARE(secondTrack.artist(), QStringLiteral("artist3"));
        QCOMPARE(secondTrack.albumName(), QStringLiteral("album3"));
        QCOMPARE(secondTrack.albumArtist(), QStringLiteral("artist3"));
        QCOMPARE(secondTrack.isValidAlbumArtist(), false);
        QCOMPARE(secondTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondTrack.trackNumber(), 7);
        QCOMPARE(secondTrack.discNumber(), 1);
        QCOMPARE(secondTrack.duration(), QTime::fromMSecsSinceStartOfDay(20));
        QCOMPARE(secondTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$20")));
        QCOMPARE(secondTrack.rating(), 5);
        QCOMPARE(secondTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(secondTrack.composer(), QStringLiteral("composer1"));

        auto album = musicDb.albumFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist2"));

        QCOMPARE(album.isValid(), true);
        QCOMPARE(album.tracksCount(), 2);
        QCOMPARE(album.title(), QStringLiteral("album3"));
        QCOMPARE(album.artist(), QStringLiteral("Various Artists"));
        QCOMPARE(album.isValidArtist(), false);
        QCOMPARE(album.albumArtURI(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(album.isSingleDiscAlbum(), true);
    }

    void addThreeTracksWithoutAlbumArtistButSameArtist()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = QList<MusicAudioTrack>();

        newTracks = {{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                      QStringLiteral("artist2"), QStringLiteral("album3"), {}, 6, 1,
                      QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))},
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")},
                     {true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track7"),
                      QStringLiteral("artist2"), QStringLiteral("album3"), {}, 7, 1,
                      QTime::fromMSecsSinceStartOfDay(20), {QUrl::fromLocalFile(QStringLiteral("/$20"))},
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 4, true, QStringLiteral("genre1"), QStringLiteral("composer1")},
                     {true, QStringLiteral("$21"), QStringLiteral("0"), QStringLiteral("track8"),
                      QStringLiteral("artist2"), QStringLiteral("album3"), {}, 8, 1,
                      QTime::fromMSecsSinceStartOfDay(21), {QUrl::fromLocalFile(QStringLiteral("/$21"))},
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 3, true, QStringLiteral("genre1"), QStringLiteral("composer1")}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("file:///$20")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("file:///$21")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 1);
        QCOMPARE(musicDb.allArtists().count(), 1);
        QCOMPARE(musicDb.allTracks().count(), 3);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrack = musicDb.trackFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                                                   QStringLiteral("album3"), 6, 1));

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrack.title(), QStringLiteral("track6"));
        QCOMPARE(firstTrack.artist(), QStringLiteral("artist2"));
        QCOMPARE(firstTrack.albumName(), QStringLiteral("album3"));
        QCOMPARE(firstTrack.albumArtist(), QStringLiteral("artist2"));
        QCOMPARE(firstTrack.isValidAlbumArtist(), false);
        QCOMPARE(firstTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstTrack.trackNumber(), 6);
        QCOMPARE(firstTrack.discNumber(), 1);
        QCOMPARE(firstTrack.duration(), QTime::fromMSecsSinceStartOfDay(19));
        QCOMPARE(firstTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$19")));
        QCOMPARE(firstTrack.rating(), 5);
        QCOMPARE(firstTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(firstTrack.composer(), QStringLiteral("composer1"));

        auto secondTrack = musicDb.trackFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track7"), QStringLiteral("artist2"),
                                                                                                    QStringLiteral("album3"), 7, 1));

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrack.title(), QStringLiteral("track7"));
        QCOMPARE(secondTrack.artist(), QStringLiteral("artist2"));
        QCOMPARE(secondTrack.albumName(), QStringLiteral("album3"));
        QCOMPARE(secondTrack.albumArtist(), QStringLiteral("artist2"));
        QCOMPARE(secondTrack.isValidAlbumArtist(), false);
        QCOMPARE(secondTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondTrack.trackNumber(), 7);
        QCOMPARE(secondTrack.discNumber(), 1);
        QCOMPARE(secondTrack.duration(), QTime::fromMSecsSinceStartOfDay(20));
        QCOMPARE(secondTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$20")));
        QCOMPARE(secondTrack.rating(), 4);
        QCOMPARE(secondTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(secondTrack.composer(), QStringLiteral("composer1"));

        auto thirdTrack = musicDb.trackFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track8"), QStringLiteral("artist2"),
                                                                                                   QStringLiteral("album3"), 8, 1));

        QCOMPARE(thirdTrack.isValid(), true);
        QCOMPARE(thirdTrack.title(), QStringLiteral("track8"));
        QCOMPARE(thirdTrack.artist(), QStringLiteral("artist2"));
        QCOMPARE(thirdTrack.albumName(), QStringLiteral("album3"));
        QCOMPARE(thirdTrack.albumArtist(), QStringLiteral("artist2"));
        QCOMPARE(thirdTrack.isValidAlbumArtist(), false);
        QCOMPARE(thirdTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(thirdTrack.trackNumber(), 8);
        QCOMPARE(thirdTrack.discNumber(), 1);
        QCOMPARE(thirdTrack.duration(), QTime::fromMSecsSinceStartOfDay(21));
        QCOMPARE(thirdTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$21")));
        QCOMPARE(thirdTrack.rating(), 3);
        QCOMPARE(thirdTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(thirdTrack.composer(), QStringLiteral("composer1"));

        auto album = musicDb.albumFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist2"));

        QCOMPARE(album.isValid(), true);
        QCOMPARE(album.tracksCount(), 3);
        QCOMPARE(album.title(), QStringLiteral("album3"));
        QCOMPARE(album.artist(), QStringLiteral("artist2"));
        QCOMPARE(album.isValidArtist(), false);
        QCOMPARE(album.albumArtURI(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(album.isSingleDiscAlbum(), true);
    }

    void addTwoTracksWithPartialAlbumArtist()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addTwoTracksWithPartialAlbumArtist" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = QList<MusicAudioTrack>();

        newTracks = {{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                      QStringLiteral("artist2"), QStringLiteral("album3"), {}, 6, 1,
                      QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))},
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")},
                     {true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track7"),
                      QStringLiteral("artist3"), QStringLiteral("album3"), {QStringLiteral("artist4")}, 7, 1,
                      QTime::fromMSecsSinceStartOfDay(20), {QUrl::fromLocalFile(QStringLiteral("/$20"))},
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("file:///$20")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 1);
        QCOMPARE(musicDb.allArtists().count(), 3);
        QCOMPARE(musicDb.allTracks().count(), 2);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrack = musicDb.trackFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                                                   QStringLiteral("album3"), 6, 1));

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrack.title(), QStringLiteral("track6"));
        QCOMPARE(firstTrack.artist(), QStringLiteral("artist2"));
        QCOMPARE(firstTrack.albumName(), QStringLiteral("album3"));
        QCOMPARE(firstTrack.albumArtist(), QStringLiteral("artist4"));
        QCOMPARE(firstTrack.isValidAlbumArtist(), true);
        QCOMPARE(firstTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstTrack.trackNumber(), 6);
        QCOMPARE(firstTrack.discNumber(), 1);
        QCOMPARE(firstTrack.duration(), QTime::fromMSecsSinceStartOfDay(19));
        QCOMPARE(firstTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$19")));
        QCOMPARE(firstTrack.rating(), 5);
        QCOMPARE(firstTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(firstTrack.composer(), QStringLiteral("composer1"));

        auto secondTrack = musicDb.trackFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track7"), QStringLiteral("artist3"),
                                                                                                    QStringLiteral("album3"), 7, 1));

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrack.title(), QStringLiteral("track7"));
        QCOMPARE(secondTrack.artist(), QStringLiteral("artist3"));
        QCOMPARE(secondTrack.albumName(), QStringLiteral("album3"));
        QCOMPARE(secondTrack.albumArtist(), QStringLiteral("artist4"));
        QCOMPARE(secondTrack.isValidAlbumArtist(), true);
        QCOMPARE(secondTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondTrack.trackNumber(), 7);
        QCOMPARE(secondTrack.discNumber(), 1);
        QCOMPARE(secondTrack.duration(), QTime::fromMSecsSinceStartOfDay(20));
        QCOMPARE(secondTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$20")));
        QCOMPARE(secondTrack.rating(), 5);
        QCOMPARE(secondTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(secondTrack.composer(), QStringLiteral("composer1"));

        auto album = musicDb.albumFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist4"));

        QCOMPARE(album.isValid(), true);
        QCOMPARE(album.tracksCount(), 2);
        QCOMPARE(album.title(), QStringLiteral("album3"));
        QCOMPARE(album.artist(), QStringLiteral("artist4"));
        QCOMPARE(album.isValidArtist(), true);
        QCOMPARE(album.albumArtURI(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(album.isSingleDiscAlbum(), true);
    }

    void addMultipleTimeSameTracks()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addMultipleTimeSameTracks" << databaseFile.fileName();

        DatabaseInterface musicDb;

        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);

        auto firstAlbum = musicDb.albumFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"));

        QCOMPARE(firstAlbum.isValid(), true);
        QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

        auto firstAlbumInvalid = musicDb.albumFromTitleAndArtist(QStringLiteral("album1Invalid"), QStringLiteral("Invalid Artist"));

        QCOMPARE(firstAlbumInvalid.isValid(), false);
    }

    void addTwiceSameTracksWithDatabaseFile()
    {
        QTemporaryFile myTempDatabase;
        myTempDatabase.open();

        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

            musicDb.init(QStringLiteral("testDb1"), myTempDatabase.fileName());

            musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 5);

            auto firstAlbum = musicDb.albumFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"));

            QCOMPARE(firstAlbum.isValid(), true);
            QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

            auto firstAlbumInvalid = musicDb.albumFromTitleAndArtist(QStringLiteral("album1Invalid"), QStringLiteral("Invalid Artist"));

            QCOMPARE(firstAlbumInvalid.isValid(), false);
        }

        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

            musicDb.init(QStringLiteral("testDb2"), myTempDatabase.fileName());

            musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 5);

            auto firstAlbum = musicDb.albumFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"));

            QCOMPARE(firstAlbum.isValid(), true);
            QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

            auto firstAlbumInvalid = musicDb.albumFromTitleAndArtist(QStringLiteral("album1Invalid"), QStringLiteral("Invalid Artist"));

            QCOMPARE(firstAlbumInvalid.isValid(), false);
        }
    }

    void restoreModifiedTracksWidthDatabaseFile()
    {
        QTemporaryFile myTempDatabase;
        myTempDatabase.open();

        {
            DatabaseInterface musicDb;

            auto allNewTracks = mNewTracks;
            allNewTracks.push_back({true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                                    QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                                    6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                                    {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")});

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

            musicDb.init(QStringLiteral("testDb1"), myTempDatabase.fileName());

            auto newCovers = mNewCovers;
            newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

            musicDb.insertTracksList(allNewTracks, newCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 5);

            auto firstAlbum = musicDb.albumFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"));

            QCOMPARE(firstAlbum.isValid(), true);
            QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

            auto firstAlbumInvalid = musicDb.albumFromTitleAndArtist(QStringLiteral("album1Invalid"), QStringLiteral("Invalid Artist"));

            QCOMPARE(firstAlbumInvalid.isValid(), false);

            auto fourthAlbum = musicDb.albumFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist2"));

            QCOMPARE(fourthAlbum.isValid(), true);
            QCOMPARE(fourthAlbum.title(), QStringLiteral("album3"));

            QCOMPARE(fourthAlbum.tracksCount(), 4);

            auto oneTrack = fourthAlbum.trackFromIndex(3);

            QCOMPARE(oneTrack.isValid(), true);
            QCOMPARE(oneTrack.rating(), 5);
        }

        {
            DatabaseInterface musicDb;

            auto allNewTracks = mNewTracks;
            allNewTracks.push_back({true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                                    QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                                    6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                                    {QUrl::fromLocalFile(QStringLiteral("album3"))}, 3, true, QStringLiteral("genre1"), QStringLiteral("composer1")});

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

            musicDb.init(QStringLiteral("testDb2"), myTempDatabase.fileName());

            auto newCovers = mNewCovers;
            newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

            musicDb.insertTracksList(allNewTracks, newCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 5);

            auto firstAlbum = musicDb.albumFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"));

            QCOMPARE(firstAlbum.isValid(), true);
            QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

            auto firstAlbumInvalid = musicDb.albumFromTitleAndArtist(QStringLiteral("album1Invalid"), QStringLiteral("Invalid Artist"));

            QCOMPARE(firstAlbumInvalid.isValid(), false);

            auto fourthAlbum = musicDb.albumFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist2"));

            QCOMPARE(fourthAlbum.isValid(), true);
            QCOMPARE(fourthAlbum.title(), QStringLiteral("album3"));

            QCOMPARE(fourthAlbum.tracksCount(), 4);

            auto oneTrack = fourthAlbum.trackFromIndex(3);

            QCOMPARE(oneTrack.isValid(), true);
            QCOMPARE(oneTrack.title(), QStringLiteral("track6"));
            QCOMPARE(oneTrack.rating(), 3);
        }
    }


    void simpleAccessor()
    {
        DatabaseInterface musicDb;

        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

        musicDb.init(QStringLiteral("testDb"));

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);

        auto allAlbums = musicDb.allAlbums();

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

        auto invalidTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 1);
        QCOMPARE(invalidTrackId, decltype(invalidTrackId)(0));

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);

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
        auto firstTrackRating = firstTrack.rating();
        auto firstIsSingleDiscAlbum = firstTrack.isSingleDiscAlbum();

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
        QCOMPARE(firstTrackResource, QUrl::fromLocalFile(QStringLiteral("/$1")));
        QCOMPARE(firstTrackRating, 1);
        QCOMPARE(firstIsSingleDiscAlbum, false);

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

    void simpleAccessorAndVariousArtistAlbum()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDbVariousArtistAlbum"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);

        auto allAlbums = musicDb.allAlbums();

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

        auto invalidTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 1);
        QCOMPARE(invalidTrackId, decltype(invalidTrackId)(0));

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
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
        auto firstTrackRating = firstTrack.rating();
        auto firstIsSingleDiscAlbum = firstTrack.isSingleDiscAlbum();

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
        QCOMPARE(firstTrackResource, QUrl::fromLocalFile(QStringLiteral("/$1")));
        QCOMPARE(firstTrackRating, 1);
        QCOMPARE(firstIsSingleDiscAlbum, false);

        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                          QStringLiteral("album1"), 2, 2);
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
        auto secondTrackRating = secondTrack.rating();
        auto secondIsSingleDiscAlbum = secondTrack.isSingleDiscAlbum();

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
        QCOMPARE(secondTrackResource, QUrl::fromLocalFile(QStringLiteral("/$2")));
        QCOMPARE(secondTrackRating, 2);
        QCOMPARE(secondIsSingleDiscAlbum, false);

        auto thirdTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"),
                                                                         QStringLiteral("album1"), 3, 3);
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
        auto thirdTrackRating = thirdTrack.rating();
        auto thirdIsSingleDiscAlbum = thirdTrack.isSingleDiscAlbum();

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
        QCOMPARE(thirdTrackResource, QUrl::fromLocalFile(QStringLiteral("/$3")));
        QCOMPARE(thirdTrackRating, 3);
        QCOMPARE(thirdIsSingleDiscAlbum, false);

        auto fourthTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist4"),
                                                                          QStringLiteral("album1"), 4, 4);
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
        auto fourthTrackRating = thirdTrack.rating();
        auto fourthIsSingleDiscAlbum = fourthTrack.isSingleDiscAlbum();

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
        QCOMPARE(fourthTrackResource, QUrl::fromLocalFile(QStringLiteral("/$4")));
        QCOMPARE(fourthTrackRating, 3);
        QCOMPARE(fourthIsSingleDiscAlbum, false);

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

    void fetchAllTracksFromSource()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDbVariousArtistAlbum"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist9"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newFiles2 = QList<QUrl>();
        for (const auto &oneTrack : newTracks) {
            newFiles2.push_back(oneTrack.resourceURI());
        }

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("image$19"));

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest2"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 8);
        QCOMPARE(musicDb.allTracks().count(), 23);
        QCOMPARE(musicDbArtistAddedSpy.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);

        auto allTracks = musicDb.allTracksFromSource(QStringLiteral("autoTest"));

        auto allTracks2 = musicDb.allTracksFromSource(QStringLiteral("autoTest2"));

        QCOMPARE(allTracks.count(), 22);
        QCOMPARE(allTracks2.count(), 1);
    }

    void simpleAccessorAndVariousArtistAlbumWithFile()
    {
        QTemporaryFile myDatabaseFile;
        myDatabaseFile.open();

        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
            QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

            QCOMPARE(musicDb.allAlbums().count(), 0);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);

            musicDb.init(QStringLiteral("testDbVariousArtistAlbum1"), myDatabaseFile.fileName());

            QCOMPARE(musicDb.allAlbums().count(), 0);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);

            musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 5);
            QCOMPARE(musicDb.allArtists().count(), 7);
            QCOMPARE(musicDb.allTracks().count(), 22);
            QCOMPARE(musicDbArtistAddedSpy.count(), 1);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
            QCOMPARE(musicDbTrackAddedSpy.count(), 1);

            auto allAlbums = musicDb.allAlbums();

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

            auto invalidTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 2, 1);
            QCOMPARE(invalidTrackId, decltype(invalidTrackId)(0));

            auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1);
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
            auto firstTrackRating = firstTrack.rating();
            auto firstIsSingleDiscAlbum = firstTrack.isSingleDiscAlbum();

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
            QCOMPARE(firstTrackResource, QUrl::fromLocalFile(QStringLiteral("/$1")));
            QCOMPARE(firstTrackRating, 1);
            QCOMPARE(firstIsSingleDiscAlbum, false);

            auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                              QStringLiteral("album1"), 2, 2);
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
            auto secondTrackRating = secondTrack.rating();
            auto secondIsSingleDiscAlbum = secondTrack.isSingleDiscAlbum();

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
            QCOMPARE(secondTrackResource, QUrl::fromLocalFile(QStringLiteral("/$2")));
            QCOMPARE(secondTrackRating, 2);
            QCOMPARE(secondIsSingleDiscAlbum, false);

            auto thirdTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"),
                                                                             QStringLiteral("album1"), 3, 3);
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
            auto thirdTrackRating = thirdTrack.rating();
            auto thirdIsSingleDiscAlbum = thirdTrack.isSingleDiscAlbum();

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
            QCOMPARE(thirdTrackResource, QUrl::fromLocalFile(QStringLiteral("/$3")));
            QCOMPARE(thirdTrackRating, 3);
            QCOMPARE(thirdIsSingleDiscAlbum, false);

            auto fourthTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist4"),
                                                                              QStringLiteral("album1"), 4, 4);
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
            auto fourthTrackRating = thirdTrack.rating();
            auto fourthIsSingleDiscAlbum = fourthTrack.isSingleDiscAlbum();

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
            QCOMPARE(fourthTrackResource, QUrl::fromLocalFile(QStringLiteral("/$4")));
            QCOMPARE(fourthTrackRating, 3);
            QCOMPARE(fourthIsSingleDiscAlbum, false);

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

        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
            QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

            QCOMPARE(musicDb.allAlbums().count(), 0);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);

            musicDb.init(QStringLiteral("testDbVariousArtistAlbum2"), myDatabaseFile.fileName());

            QCOMPARE(musicDb.allAlbums().count(), 5);
            QCOMPARE(musicDb.allArtists().count(), 7);
            QCOMPARE(musicDb.allTracks().count(), 22);
            QCOMPARE(musicDbArtistAddedSpy.count(), 1);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
            QCOMPARE(musicDbTrackAddedSpy.count(), 1);

            musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 5);
            QCOMPARE(musicDb.allArtists().count(), 7);
            QCOMPARE(musicDb.allTracks().count(), 22);
            QCOMPARE(musicDbArtistAddedSpy.count(), 1);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
            QCOMPARE(musicDbTrackAddedSpy.count(), 1);

            auto allAlbums = musicDb.allAlbums();

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

            auto invalidTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                               QStringLiteral("album1"), 2, 1);
            QCOMPARE(invalidTrackId, decltype(invalidTrackId)(0));

            auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                             QStringLiteral("album1"), 1, 1);
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
            auto firstTrackRating = firstTrack.rating();
            auto firstIsSingleDiscAlbum = firstTrack.isSingleDiscAlbum();

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
            QCOMPARE(firstTrackResource, QUrl::fromLocalFile(QStringLiteral("/$1")));
            QCOMPARE(firstTrackRating, 1);
            QCOMPARE(firstIsSingleDiscAlbum, false);

            auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist2"),
                                                                              QStringLiteral("album1"), 2, 2);
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
            auto secondTrackRating = secondTrack.rating();
            auto secondIsSingleDiscAlbum = secondTrack.isSingleDiscAlbum();

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
            QCOMPARE(secondTrackResource, QUrl::fromLocalFile(QStringLiteral("/$2")));
            QCOMPARE(secondTrackRating, 2);
            QCOMPARE(secondIsSingleDiscAlbum, false);

            auto thirdTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"),
                                                                             QStringLiteral("album1"), 3, 3);
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
            auto thirdTrackRating = thirdTrack.rating();
            auto thirdIsSingleDiscAlbum = thirdTrack.isSingleDiscAlbum();

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
            QCOMPARE(thirdTrackResource, QUrl::fromLocalFile(QStringLiteral("/$3")));
            QCOMPARE(thirdTrackRating, 3);
            QCOMPARE(thirdIsSingleDiscAlbum, false);

            auto fourthTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist4"),
                                                                              QStringLiteral("album1"), 4, 4);
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
            auto fourthTrackRating = thirdTrack.rating();
            auto fourthIsSingleDiscAlbum = fourthTrack.isSingleDiscAlbum();

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
            QCOMPARE(fourthTrackResource, QUrl::fromLocalFile(QStringLiteral("/$4")));
            QCOMPARE(fourthTrackRating, 3);
            QCOMPARE(fourthIsSingleDiscAlbum, false);

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

            auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                    QStringLiteral("artist8"), QStringLiteral("album3"), QStringLiteral("artist2"),
                    6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                    {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
            auto newTracks = QList<MusicAudioTrack>();
            newTracks.push_back(newTrack);

            auto newFiles2 = QList<QUrl>();
            for (const auto &oneTrack : newTracks) {
                newFiles2.push_back(oneTrack.resourceURI());
            }

            auto newCovers = mNewCovers;
            newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("image$19"));

            musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 5);
            QCOMPARE(musicDb.allArtists().count(), 8);
            QCOMPARE(musicDb.allTracks().count(), 23);
            QCOMPARE(musicDbArtistAddedSpy.count(), 2);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
            QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        }
    }

    void testTracksFromAuthor() {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDbVariousArtistAlbum"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);

        auto allTracks = musicDb.tracksFromAuthor(QStringLiteral("artist1"));

        QCOMPARE(allTracks.size(), 6);
        QCOMPARE(allTracks[0].albumArtist(), QStringLiteral("Various Artists"));
        QCOMPARE(allTracks[1].albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allTracks[2].albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allTracks[3].albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allTracks[4].albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allTracks[5].albumArtist(), QStringLiteral("artist1"));
    }

    void removeOneTrack()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "removeOneTrack" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbums = musicDb.allAlbums();

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

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                    QStringLiteral("album1"), 1, 1);

        auto firstTrack = musicDb.trackFromDatabaseId(trackId);

        musicDb.removeTracksList({firstTrack.resourceURI()});

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 21);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 1);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbumsV2 = musicDb.allAlbums();
        const auto &firstAlbum = allAlbumsV2[0];

        QCOMPARE(musicDb.allAlbums().isEmpty(), false);
        QCOMPARE(firstAlbum.isValid(), true);

        auto firstAlbumTitleV2 = firstAlbum.title();
        auto firstAlbumArtistV2 = firstAlbum.artist();
        auto firstAlbumImageV2 = firstAlbum.albumArtURI();
        auto firstAlbumTracksCountV2 = firstAlbum.tracksCount();
        auto firstAlbumIsSingleDiscAlbumV2 = firstAlbum.isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitleV2, QStringLiteral("album1"));
        QCOMPARE(firstAlbumArtistV2, QStringLiteral("Various Artists"));
        QCOMPARE(firstAlbumImageV2.isValid(), true);
        QCOMPARE(firstAlbumImageV2, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstAlbumTracksCountV2, 3);
        QCOMPARE(firstAlbumIsSingleDiscAlbumV2, false);

        auto removedTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                           QStringLiteral("album1"), 1, 1);
        QCOMPARE(removedTrackId, qulonglong(0));
    }

    void removeOneTrackAndModifyIt()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "removeOneTrackAndModifyIt" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbums = musicDb.allAlbums();

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

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                    QStringLiteral("album1"), 1, 1);

        auto firstTrack = musicDb.trackFromDatabaseId(trackId);

        musicDb.removeTracksList({firstTrack.resourceURI()});

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 21);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 1);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbumsV2 = musicDb.allAlbums();
        const auto &firstAlbum = allAlbumsV2[0];

        QCOMPARE(musicDb.allAlbums().isEmpty(), false);
        QCOMPARE(firstAlbum.isValid(), true);

        auto firstAlbumTitleV2 = firstAlbum.title();
        auto firstAlbumArtistV2 = firstAlbum.artist();
        auto firstAlbumImageV2 = firstAlbum.albumArtURI();
        auto firstAlbumTracksCountV2 = firstAlbum.tracksCount();
        auto firstAlbumIsSingleDiscAlbumV2 = firstAlbum.isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitleV2, QStringLiteral("album1"));
        QCOMPARE(firstAlbumArtistV2, QStringLiteral("Various Artists"));
        QCOMPARE(firstAlbumImageV2.isValid(), true);
        QCOMPARE(firstAlbumImageV2, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstAlbumTracksCountV2, 3);
        QCOMPARE(firstAlbumIsSingleDiscAlbumV2, false);

        auto removedTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                           QStringLiteral("album1"), 1, 1);
        QCOMPARE(removedTrackId, qulonglong(0));

        firstTrack.setDatabaseId(0);

        musicDb.modifyTracksList({firstTrack}, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 1);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 2);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbums3 = musicDb.allAlbums();

        auto firstAlbumTitle3 = allAlbums3[0].title();
        auto firstAlbumArtist3 = allAlbums3[0].artist();
        auto firstAlbumImage3 = allAlbums3[0].albumArtURI();
        auto firstAlbumTracksCount3 = allAlbums3[0].tracksCount();
        auto firstAlbumIsSingleDiscAlbum3 = allAlbums3[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle3, QStringLiteral("album1"));
        QCOMPARE(firstAlbumArtist3, QStringLiteral("Various Artists"));
        QCOMPARE(firstAlbumImage3.isValid(), true);
        QCOMPARE(firstAlbumImage3, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstAlbumTracksCount3, 4);
        QCOMPARE(firstAlbumIsSingleDiscAlbum3, false);

        auto trackId3 = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                    QStringLiteral("album1"), 1, 1);

        QCOMPARE(trackId3 != 0, true);
    }

    void removeOneAlbum()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "removeOneAlbum" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbums = musicDb.allAlbums();

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

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                         QStringLiteral("album2"), 1, 1);
        auto firstTrack = musicDb.trackFromDatabaseId(firstTrackId);
        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"),
                                                                          QStringLiteral("album2"), 2, 1);
        auto secondTrack = musicDb.trackFromDatabaseId(secondTrackId);
        auto thirdTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"),
                                                                         QStringLiteral("album2"), 3, 1);
        auto thirdTrack = musicDb.trackFromDatabaseId(thirdTrackId);
        auto fourthTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"),
                                                                          QStringLiteral("album2"), 4, 1);
        auto fourthTrack = musicDb.trackFromDatabaseId(fourthTrackId);
        auto fithTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"),
                                                                        QStringLiteral("album2"), 5, 1);
        auto fithTrack = musicDb.trackFromDatabaseId(fithTrackId);
        auto sixthTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"),
                                                                         QStringLiteral("album2"), 6, 1);
        auto sixthTrack = musicDb.trackFromDatabaseId(sixthTrackId);

        musicDb.removeTracksList({firstTrack.resourceURI(), secondTrack.resourceURI(), thirdTrack.resourceURI(),
                                  fourthTrack.resourceURI(), fithTrack.resourceURI(), sixthTrack.resourceURI()});

        QCOMPARE(musicDb.allAlbums().count(), 4);
        QCOMPARE(musicDb.allArtists().count(), 6);
        QCOMPARE(musicDb.allTracks().count(), 16);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 1);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 6);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto removedAlbum = musicDb.albumFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("Invalid Artist"));

        QCOMPARE(removedAlbum.isValid(), false);
    }

    void removeOneArtist()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbums = musicDb.allAlbums();

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

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"),
                                                                    QStringLiteral("album1"), 3, 3);
        auto track = musicDb.trackFromDatabaseId(trackId);

        musicDb.removeTracksList({track.resourceURI()});

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 6);
        QCOMPARE(musicDb.allTracks().count(), 21);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 1);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void addOneTrack()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("image$19"));

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 23);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void addTwoTracksSameAlbumSameTitle()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 1);
        QCOMPARE(musicDb.allArtists().count(), 1);
        QCOMPARE(musicDb.allTracks().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 6, 1);
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
        auto firstTrackRating = firstTrack.rating();
        auto firstIsSingleDiscAlbum = firstTrack.isSingleDiscAlbum();

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrackTitle, QStringLiteral("track6"));
        QCOMPARE(firstTrackArtist, QStringLiteral("artist2"));
        QCOMPARE(firstTrackAlbumArtist, QStringLiteral("artist2"));
        QCOMPARE(firstTrackAlbum, QStringLiteral("album3"));
        QCOMPARE(firstTrackImage.isValid(), true);
        QCOMPARE(firstTrackImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstTrackDuration, QTime::fromMSecsSinceStartOfDay(23));
        QCOMPARE(firstTrackMilliSecondsDuration, 23);
        QCOMPARE(firstTrackTrackNumber, 6);
        QCOMPARE(firstTrackDiscNumber, 1);
        QCOMPARE(firstTrackResource.isValid(), true);
        QCOMPARE(firstTrackResource, QUrl::fromLocalFile(QStringLiteral("/$23")));
        QCOMPARE(firstTrackRating, 5);
        QCOMPARE(firstIsSingleDiscAlbum, true);

        auto allAlbums = musicDb.allAlbums();

        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = allAlbums[0].tracksCount();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album3"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("artist2"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstAlbumTracksCount, 1);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, true);

        auto newTrack2 = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist9"), QStringLiteral("album3"), QStringLiteral("artist2"), 6, 1,
                QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$23"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks2 = QList<MusicAudioTrack>();
        newTracks2.push_back(newTrack2);

        auto newCovers2 = mNewCovers;
        newCovers2[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("image$19"));

        musicDb.insertTracksList(newTracks2, newCovers2, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(50);

        QCOMPARE(musicDb.allAlbums().count(), 1);
        QCOMPARE(musicDb.allArtists().count(), 2);
        QCOMPARE(musicDb.allTracks().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist9"),
                                                                         QStringLiteral("album3"), 6, 1);
        auto secondTrack = musicDb.trackFromDatabaseId(secondTrackId);

        auto secondTrackTitle = secondTrack.title();
        auto secondTrackArtist = secondTrack.artist();
        auto secondTrackAlbumArtist = secondTrack.albumArtist();
        auto secondTrackAlbum = secondTrack.albumName();
        auto secondTrackImage = secondTrack.albumCover();
        auto secondTrackDuration = secondTrack.duration();
        auto secondTrackMilliSecondsDuration = secondTrack.duration().msecsSinceStartOfDay();
        auto secondTrackTrackNumber = secondTrack.trackNumber();
        auto secondTrackDiscNumber = secondTrack.discNumber();
        auto secondTrackResource = secondTrack.resourceURI();
        auto secondTrackRating = secondTrack.rating();
        auto secondIsSingleDiscAlbum = secondTrack.isSingleDiscAlbum();

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrackTitle, QStringLiteral("track6"));
        QCOMPARE(secondTrackArtist, QStringLiteral("artist9"));
        QCOMPARE(secondTrackAlbumArtist, QStringLiteral("artist2"));
        QCOMPARE(secondTrackAlbum, QStringLiteral("album3"));
        QCOMPARE(secondTrackImage.isValid(), true);
        QCOMPARE(secondTrackImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondTrackDuration, QTime::fromMSecsSinceStartOfDay(23));
        QCOMPARE(secondTrackMilliSecondsDuration, 23);
        QCOMPARE(secondTrackTrackNumber, 6);
        QCOMPARE(secondTrackDiscNumber, 1);
        QCOMPARE(secondTrackResource.isValid(), true);
        QCOMPARE(secondTrackResource, QUrl::fromLocalFile(QStringLiteral("/$23")));
        QCOMPARE(secondTrackRating, 5);
        QCOMPARE(secondIsSingleDiscAlbum, true);

        auto allAlbums2 = musicDb.allAlbums();

        auto firstAlbumTitle2 = allAlbums2[0].title();
        auto firstAlbumArtist2 = allAlbums2[0].artist();
        auto firstAlbumImage2 = allAlbums2[0].albumArtURI();
        auto firstAlbumTracksCount2 = allAlbums2[0].tracksCount();
        auto firstAlbumIsSingleDiscAlbum2 = allAlbums2[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle2, QStringLiteral("album3"));
        QCOMPARE(firstAlbumArtist2, QStringLiteral("artist2"));
        QCOMPARE(firstAlbumImage2.isValid(), true);
        QCOMPARE(firstAlbumImage2, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstAlbumTracksCount2, 1);
        QCOMPARE(firstAlbumIsSingleDiscAlbum2, true);
    }

    void addTwoTracksSameFileWithAlbumSameName()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addTwoTracksSameFileWithAlbumSameName" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 1);
        QCOMPARE(musicDb.allArtists().count(), 1);
        QCOMPARE(musicDb.allTracks().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 6, 1);
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
        auto firstTrackRating = firstTrack.rating();
        auto firstIsSingleDiscAlbum = firstTrack.isSingleDiscAlbum();

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrackTitle, QStringLiteral("track6"));
        QCOMPARE(firstTrackArtist, QStringLiteral("artist2"));
        QCOMPARE(firstTrackAlbumArtist, QStringLiteral("artist2"));
        QCOMPARE(firstTrackAlbum, QStringLiteral("album3"));
        QCOMPARE(firstTrackImage.isValid(), true);
        QCOMPARE(firstTrackImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstTrackDuration, QTime::fromMSecsSinceStartOfDay(23));
        QCOMPARE(firstTrackMilliSecondsDuration, 23);
        QCOMPARE(firstTrackTrackNumber, 6);
        QCOMPARE(firstTrackDiscNumber, 1);
        QCOMPARE(firstTrackResource.isValid(), true);
        QCOMPARE(firstTrackResource, QUrl::fromLocalFile(QStringLiteral("/$23")));
        QCOMPARE(firstTrackRating, 5);
        QCOMPARE(firstIsSingleDiscAlbum, true);

        auto allAlbums = musicDb.allAlbums();

        QCOMPARE(allAlbums.size(), 1);

        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = allAlbums[0].tracksCount();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album3"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("artist2"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstAlbumTracksCount, 1);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, true);

        auto newTrack2 = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist9"), QStringLiteral("album3"), QStringLiteral("artist9"), 6, 1,
                QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$23"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks2 = QList<MusicAudioTrack>();
        newTracks2.push_back(newTrack2);

        auto newCovers2 = mNewCovers;
        newCovers2[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks2, newCovers2, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(50);

        QCOMPARE(musicDb.allAlbums().count(), 2);
        QCOMPARE(musicDb.allArtists().count(), 2);
        QCOMPARE(musicDb.allTracks().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 2);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist9"),
                                                                         QStringLiteral("album3"), 6, 1);
        auto secondTrack = musicDb.trackFromDatabaseId(secondTrackId);

        auto secondTrackTitle = secondTrack.title();
        auto secondTrackArtist = secondTrack.artist();
        auto secondTrackAlbumArtist = secondTrack.albumArtist();
        auto secondTrackAlbum = secondTrack.albumName();
        auto secondTrackImage = secondTrack.albumCover();
        auto secondTrackDuration = secondTrack.duration();
        auto secondTrackMilliSecondsDuration = secondTrack.duration().msecsSinceStartOfDay();
        auto secondTrackTrackNumber = secondTrack.trackNumber();
        auto secondTrackDiscNumber = secondTrack.discNumber();
        auto secondTrackResource = secondTrack.resourceURI();
        auto secondTrackRating = secondTrack.rating();
        auto secondIsSingleDiscAlbum = secondTrack.isSingleDiscAlbum();

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrackTitle, QStringLiteral("track6"));
        QCOMPARE(secondTrackArtist, QStringLiteral("artist9"));
        QCOMPARE(secondTrackAlbumArtist, QStringLiteral("artist9"));
        QCOMPARE(secondTrackAlbum, QStringLiteral("album3"));
        QCOMPARE(secondTrackImage.isValid(), true);
        QCOMPARE(secondTrackImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondTrackDuration, QTime::fromMSecsSinceStartOfDay(23));
        QCOMPARE(secondTrackMilliSecondsDuration, 23);
        QCOMPARE(secondTrackTrackNumber, 6);
        QCOMPARE(secondTrackDiscNumber, 1);
        QCOMPARE(secondTrackResource.isValid(), true);
        QCOMPARE(secondTrackResource, QUrl::fromLocalFile(QStringLiteral("/$23")));
        QCOMPARE(secondTrackRating, 5);
        QCOMPARE(secondIsSingleDiscAlbum, true);

        auto allAlbums2 = musicDb.allAlbums();

        auto firstAlbumTitle2 = allAlbums2[0].title();
        auto firstAlbumArtist2 = allAlbums2[0].artist();
        auto firstAlbumImage2 = allAlbums2[0].albumArtURI();
        auto firstAlbumTracksCount2 = allAlbums2[0].tracksCount();
        auto firstAlbumIsSingleDiscAlbum2 = allAlbums2[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle2, QStringLiteral("album3"));
        QCOMPARE(firstAlbumArtist2, QStringLiteral("artist2"));
        QCOMPARE(firstAlbumImage2.isValid(), true);
        QCOMPARE(firstAlbumImage2, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstAlbumTracksCount2, 0);
        QCOMPARE(firstAlbumIsSingleDiscAlbum2, true);

        auto secondAlbumTitle = allAlbums2[1].title();
        auto secondAlbumArtist = allAlbums2[1].artist();
        auto secondAlbumImage = allAlbums2[1].albumArtURI();
        auto secondAlbumTracksCount = allAlbums2[1].tracksCount();
        auto secondAlbumIsSingleDiscAlbum = allAlbums2[1].isSingleDiscAlbum();

        QCOMPARE(secondAlbumTitle, QStringLiteral("album3"));
        QCOMPARE(secondAlbumArtist, QStringLiteral("artist9"));
        QCOMPARE(secondAlbumImage.isValid(), true);
        QCOMPARE(secondAlbumImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondAlbumTracksCount, 1);
        QCOMPARE(secondAlbumIsSingleDiscAlbum, true);

        auto modifiedAlbumIds = QList<qulonglong>{musicDbAlbumModifiedSpy.at(0).at(1).toULongLong(),};

        std::sort(modifiedAlbumIds.begin(), modifiedAlbumIds.end());
        QCOMPARE(modifiedAlbumIds.at(0), qulonglong(2));
    }

    void addTwoTracksWithAlbumSameName()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addTwoTracksWithAlbumSameName" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 1);
        QCOMPARE(musicDb.allArtists().count(), 1);
        QCOMPARE(musicDb.allTracks().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 6, 1);
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
        auto firstTrackRating = firstTrack.rating();
        auto firstIsSingleDiscAlbum = firstTrack.isSingleDiscAlbum();

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrackTitle, QStringLiteral("track6"));
        QCOMPARE(firstTrackArtist, QStringLiteral("artist2"));
        QCOMPARE(firstTrackAlbumArtist, QStringLiteral("artist2"));
        QCOMPARE(firstTrackAlbum, QStringLiteral("album3"));
        QCOMPARE(firstTrackImage.isValid(), true);
        QCOMPARE(firstTrackImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstTrackDuration, QTime::fromMSecsSinceStartOfDay(23));
        QCOMPARE(firstTrackMilliSecondsDuration, 23);
        QCOMPARE(firstTrackTrackNumber, 6);
        QCOMPARE(firstTrackDiscNumber, 1);
        QCOMPARE(firstTrackResource.isValid(), true);
        QCOMPARE(firstTrackResource, QUrl::fromLocalFile(QStringLiteral("/$23")));
        QCOMPARE(firstTrackRating, 5);
        QCOMPARE(firstIsSingleDiscAlbum, true);

        auto allAlbums = musicDb.allAlbums();

        QCOMPARE(allAlbums.size(), 1);

        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = allAlbums[0].tracksCount();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album3"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("artist2"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstAlbumTracksCount, 1);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, true);

        auto newTrack2 = MusicAudioTrack{true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist9"), QStringLiteral("album3"), QStringLiteral("artist9"), 6, 1, QTime::fromMSecsSinceStartOfDay(20),
        {QUrl::fromLocalFile(QStringLiteral("/$20"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$20"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks2 = QList<MusicAudioTrack>();
        newTracks2.push_back(newTrack2);

        auto newCovers2 = mNewCovers;
        newCovers2[QStringLiteral("file:///$20")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks2, newCovers2, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(50);

        QCOMPARE(musicDb.allAlbums().count(), 2);
        QCOMPARE(musicDb.allArtists().count(), 2);
        QCOMPARE(musicDb.allTracks().count(), 2);
        QCOMPARE(musicDbArtistAddedSpy.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 2);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist9"),
                                                                         QStringLiteral("album3"), 6, 1);
        auto secondTrack = musicDb.trackFromDatabaseId(secondTrackId);

        auto secondTrackTitle = secondTrack.title();
        auto secondTrackArtist = secondTrack.artist();
        auto secondTrackAlbumArtist = secondTrack.albumArtist();
        auto secondTrackAlbum = secondTrack.albumName();
        auto secondTrackImage = secondTrack.albumCover();
        auto secondTrackDuration = secondTrack.duration();
        auto secondTrackMilliSecondsDuration = secondTrack.duration().msecsSinceStartOfDay();
        auto secondTrackTrackNumber = secondTrack.trackNumber();
        auto secondTrackDiscNumber = secondTrack.discNumber();
        auto secondTrackResource = secondTrack.resourceURI();
        auto secondTrackRating = secondTrack.rating();
        auto secondIsSingleDiscAlbum = secondTrack.isSingleDiscAlbum();

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrackTitle, QStringLiteral("track6"));
        QCOMPARE(secondTrackArtist, QStringLiteral("artist9"));
        QCOMPARE(secondTrackAlbumArtist, QStringLiteral("artist9"));
        QCOMPARE(secondTrackAlbum, QStringLiteral("album3"));
        QCOMPARE(secondTrackImage.isValid(), true);
        QCOMPARE(secondTrackImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondTrackDuration, QTime::fromMSecsSinceStartOfDay(20));
        QCOMPARE(secondTrackMilliSecondsDuration, 20);
        QCOMPARE(secondTrackTrackNumber, 6);
        QCOMPARE(secondTrackDiscNumber, 1);
        QCOMPARE(secondTrackResource.isValid(), true);
        QCOMPARE(secondTrackResource, QUrl::fromLocalFile(QStringLiteral("/$20")));
        QCOMPARE(secondTrackRating, 5);
        QCOMPARE(secondIsSingleDiscAlbum, true);

        auto allAlbums2 = musicDb.allAlbums();

        auto firstAlbumTitle2 = allAlbums2[0].title();
        auto firstAlbumArtist2 = allAlbums2[0].artist();
        auto firstAlbumImage2 = allAlbums2[0].albumArtURI();
        auto firstAlbumTracksCount2 = allAlbums2[0].tracksCount();
        auto firstAlbumIsSingleDiscAlbum2 = allAlbums2[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle2, QStringLiteral("album3"));
        QCOMPARE(firstAlbumArtist2, QStringLiteral("artist2"));
        QCOMPARE(firstAlbumImage2.isValid(), true);
        QCOMPARE(firstAlbumImage2, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstAlbumTracksCount2, 1);
        QCOMPARE(firstAlbumIsSingleDiscAlbum2, true);

        auto secondAlbumTitle = allAlbums2[1].title();
        auto secondAlbumArtist = allAlbums2[1].artist();
        auto secondAlbumImage = allAlbums2[1].albumArtURI();
        auto secondAlbumTracksCount = allAlbums2[1].tracksCount();
        auto secondAlbumIsSingleDiscAlbum = allAlbums2[1].isSingleDiscAlbum();

        QCOMPARE(secondAlbumTitle, QStringLiteral("album3"));
        QCOMPARE(secondAlbumArtist, QStringLiteral("artist9"));
        QCOMPARE(secondAlbumImage.isValid(), true);
        QCOMPARE(secondAlbumImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondAlbumTracksCount, 1);
        QCOMPARE(secondAlbumIsSingleDiscAlbum, true);
    }

    void addTwoTracksInAlbumWithoutCover()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album7"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl();

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 6);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 23);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 2);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                    QStringLiteral("album7"), 6, 1);
        const auto &firstTrack = musicDb.trackFromDatabaseId(trackId);

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrack.albumCover(), QUrl());

        auto newTrack2 = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track7"),
                QStringLiteral("artist2"), QStringLiteral("album7"), QStringLiteral("artist2"), 7, 1,
                QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
        {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks2 = QList<MusicAudioTrack>();
        newTracks2.push_back(newTrack2);

        auto newCovers2 = mNewCovers;
        newCovers2[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album7"));

        musicDb.insertTracksList(newTracks2, newCovers2, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 6);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 23);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 2);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 2);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track7"), QStringLiteral("artist2"),
                                                                          QStringLiteral("album7"), 7, 1);
        const auto &secondTrack = musicDb.trackFromDatabaseId(secondTrackId);

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album7")));
    }

    void modifyOneTrack()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "modifyOneTrack" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto modifiedTrack = MusicAudioTrack{true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 5, 3,
                QTime::fromMSecsSinceStartOfDay(3), {QUrl::fromLocalFile(QStringLiteral("/$3"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};

        musicDb.modifyTracksList({modifiedTrack}, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 2);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"),
                                                                    QStringLiteral("album1"), 5, 3);
        QCOMPARE(trackId != 0, true);
        auto track = musicDb.trackFromDatabaseId(trackId);

        QCOMPARE(track.isValid(), true);
        QCOMPARE(track.trackNumber(), 5);
    }

    void addOneAlbum()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album7"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newCover = QUrl::fromLocalFile(QStringLiteral("file://image$19"));
        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = newCover;

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 6);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 23);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 2);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        const auto &newAlbum = musicDb.albumFromTitleAndArtist(QStringLiteral("album7"), QStringLiteral("artist2"));
        QCOMPARE(newAlbum.albumArtURI(), QUrl::fromLocalFile(QStringLiteral("file://image$19")));
    }

    void addOneArtist()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newFiles2 = QList<QUrl>();
        for (const auto &oneTrack : newTracks) {
            newFiles2.push_back(oneTrack.resourceURI());
        }

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("file://image$19"));

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 8);
        QCOMPARE(musicDb.allTracks().count(), 23);
        QCOMPARE(musicDbArtistAddedSpy.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void reloadDatabase()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "reloadDatabase" << databaseFile.fileName();

        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

            musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

            musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);
        }

        DatabaseInterface musicDb2;

        QSignalSpy musicDbArtistAddedSpy2(&musicDb2, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy2(&musicDb2, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy2(&musicDb2, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy2(&musicDb2, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy2(&musicDb2, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy2(&musicDb2, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy2(&musicDb2, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy2(&musicDb2, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy2(&musicDb2, &DatabaseInterface::trackModified);

        QCOMPARE(musicDb2.allAlbums().count(), 0);
        QCOMPARE(musicDb2.allArtists().count(), 0);
        QCOMPARE(musicDb2.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 0);

        musicDb2.init(QStringLiteral("testDb2"), databaseFile.fileName());

        musicDbTrackAddedSpy2.wait(300);

        QCOMPARE(musicDb2.allAlbums().count(), 5);
        QCOMPARE(musicDb2.allArtists().count(), 7);
        QCOMPARE(musicDb2.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 0);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newFiles2 = QList<QUrl>();
        for (const auto &oneTrack : newTracks) {
            newFiles2.push_back(oneTrack.resourceURI());
        }

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("file://image$19"));

        musicDb2.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        QCOMPARE(musicDb2.allAlbums().count(), 5);
        QCOMPARE(musicDb2.allArtists().count(), 8);
        QCOMPARE(musicDb2.allTracks().count(), 23);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 0);

        auto invalidTracks = musicDb2.allInvalidTracksFromSource(QStringLiteral("autoTest"));

        QCOMPARE(invalidTracks.size(), 23);

        auto tracksUrlsToRemove = QList<QUrl>();

        for (const auto &oneTrack : invalidTracks) {
            tracksUrlsToRemove.push_back(oneTrack.resourceURI());
        }

        musicDb2.removeTracksList(tracksUrlsToRemove);

        QCOMPARE(musicDb2.allAlbums().count(), 1);
        QCOMPARE(musicDb2.allArtists().count(), 2);
        QCOMPARE(musicDb2.allTracks().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 6);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 4);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 22);
        QCOMPARE(musicDbArtistModifiedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 2);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 0);
    }

    void reloadDatabaseWithAllTracks()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "reloadDatabaseWithAllTracks" << databaseFile.fileName();

        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

            musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

            musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);
        }

        DatabaseInterface musicDb2;

        QSignalSpy musicDbArtistAddedSpy2(&musicDb2, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy2(&musicDb2, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy2(&musicDb2, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy2(&musicDb2, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy2(&musicDb2, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy2(&musicDb2, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy2(&musicDb2, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy2(&musicDb2, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy2(&musicDb2, &DatabaseInterface::trackModified);

        QCOMPARE(musicDb2.allAlbums().count(), 0);
        QCOMPARE(musicDb2.allArtists().count(), 0);
        QCOMPARE(musicDb2.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 0);

        musicDb2.init(QStringLiteral("testDb2"), databaseFile.fileName());

        musicDbTrackAddedSpy2.wait(300);

        QCOMPARE(musicDb2.allAlbums().count(), 5);
        QCOMPARE(musicDb2.allArtists().count(), 7);
        QCOMPARE(musicDb2.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 0);

        musicDb2.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(musicDb2.allAlbums().count(), 5);
        QCOMPARE(musicDb2.allArtists().count(), 7);
        QCOMPARE(musicDb2.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 1);

        auto invalidTracks = musicDb2.allInvalidTracksFromSource(QStringLiteral("autoTest"));

        QCOMPARE(invalidTracks.size(), 0);

        auto tracksUrlsToRemove = QList<QUrl>();

        for (const auto &oneTrack : invalidTracks) {
            tracksUrlsToRemove.push_back(oneTrack.resourceURI());
        }

        musicDb2.removeTracksList(tracksUrlsToRemove);

        QCOMPARE(musicDb2.allAlbums().count(), 5);
        QCOMPARE(musicDb2.allArtists().count(), 7);
        QCOMPARE(musicDb2.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 1);
    }

    void reloadDatabaseAndCleanInvalidTracks()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "reloadDatabaseAndCleanInvalidTracks" << databaseFile.fileName();

        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

            musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

            musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);
        }

        DatabaseInterface musicDb2;

        QSignalSpy musicDbArtistAddedSpy2(&musicDb2, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy2(&musicDb2, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy2(&musicDb2, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy2(&musicDb2, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy2(&musicDb2, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy2(&musicDb2, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy2(&musicDb2, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy2(&musicDb2, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy2(&musicDb2, &DatabaseInterface::trackModified);

        QCOMPARE(musicDb2.allAlbums().count(), 0);
        QCOMPARE(musicDb2.allArtists().count(), 0);
        QCOMPARE(musicDb2.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 0);

        musicDb2.init(QStringLiteral("testDb2"), databaseFile.fileName());

        musicDbTrackAddedSpy2.wait(300);

        QCOMPARE(musicDb2.allAlbums().count(), 5);
        QCOMPARE(musicDb2.allArtists().count(), 7);
        QCOMPARE(musicDb2.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 0);

        musicDb2.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(musicDb2.allAlbums().count(), 5);
        QCOMPARE(musicDb2.allArtists().count(), 7);
        QCOMPARE(musicDb2.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 1);

        musicDb2.cleanInvalidTracks();

        QCOMPARE(musicDb2.allAlbums().count(), 5);
        QCOMPARE(musicDb2.allArtists().count(), 7);
        QCOMPARE(musicDb2.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 1);
    }

    void testRemovalOfTracksFromInvalidSource()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.removeAllTracksFromSource(QStringLiteral("autoTestNotValid"));

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void testRemovalOfTracksFromValidSource()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "testRemovalOfTracksFromValidSource" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 5);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDb.allTracks().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        QList<MusicAudioTrack> secondNewTracks = {
            {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track1"),
             QStringLiteral("artist6"), QStringLiteral("album7"), QStringLiteral("artist6"),
             1, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 1, false, QStringLiteral("genre1"), QStringLiteral("composer1")},
            {true, QStringLiteral("$24"), QStringLiteral("0"), QStringLiteral("track2"),
             QStringLiteral("artist6"), QStringLiteral("album7"), QStringLiteral("artist6"),
             2, 1, QTime::fromMSecsSinceStartOfDay(24), {QUrl::fromLocalFile(QStringLiteral("/$24"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 2, false, QStringLiteral("genre1"), QStringLiteral("composer1")},
            {true, QStringLiteral("$25"), QStringLiteral("0"), QStringLiteral("track3"),
             QStringLiteral("artist6"), QStringLiteral("album7"), QStringLiteral("artist6"),
             3, 1, QTime::fromMSecsSinceStartOfDay(25), {QUrl::fromLocalFile(QStringLiteral("/$25"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 3, false, QStringLiteral("genre1"), QStringLiteral("composer1")},
            {true, QStringLiteral("$26"), QStringLiteral("0"), QStringLiteral("track4"),
             QStringLiteral("artist6"), QStringLiteral("album7"), QStringLiteral("artist6"),
             4, 1, QTime::fromMSecsSinceStartOfDay(26), {QUrl::fromLocalFile(QStringLiteral("/$26"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 4, false, QStringLiteral("genre1"), QStringLiteral("composer1")},
            {true, QStringLiteral("$27"), QStringLiteral("0"), QStringLiteral("track5"),
             QStringLiteral("artist6"), QStringLiteral("album7"), QStringLiteral("artist6"),
             5, 1, QTime::fromMSecsSinceStartOfDay(27), {QUrl::fromLocalFile(QStringLiteral("/$27"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 5, false, QStringLiteral("genre1"), QStringLiteral("composer1")},
            {true, QStringLiteral("$28"), QStringLiteral("0"), QStringLiteral("track6"),
             QStringLiteral("artist6"), QStringLiteral("album7"), QStringLiteral("artist6"),
             6, 1, QTime::fromMSecsSinceStartOfDay(28), {QUrl::fromLocalFile(QStringLiteral("/$28"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 6, true, QStringLiteral("genre1"), QStringLiteral("composer1")},
            {true, QStringLiteral("$29"), QStringLiteral("0"), QStringLiteral("track2"),
             QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
             2, 2, QTime::fromMSecsSinceStartOfDay(29), {QUrl::fromLocalFile(QStringLiteral("/$29"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$2"))}, 2, false, QStringLiteral("genre1"), QStringLiteral("composer1")},
        };

        musicDb.insertTracksList(secondNewTracks, mNewCovers, QStringLiteral("autoTestSource2"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 6);
        QCOMPARE(musicDb.allArtists().count(), 8);
        QCOMPARE(musicDb.allTracks().count(), 28);
        QCOMPARE(musicDbArtistAddedSpy.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 2);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 2);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.removeAllTracksFromSource(QStringLiteral("autoTest"));

        QCOMPARE(musicDb.allAlbums().count(), 2);
        QCOMPARE(musicDb.allArtists().count(), 3);
        QCOMPARE(musicDb.allTracks().count(), 7);
        QCOMPARE(musicDbArtistAddedSpy.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 2);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 5);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 4);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 21);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 2);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 2);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void testAddAlbumsSameName()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "testAddAlbumsSameName" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        QList<MusicAudioTrack> newTracks = {
            {true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track1"),
             QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("artist6"),
             1, 1, QTime::fromMSecsSinceStartOfDay(1), {QUrl::fromLocalFile(QStringLiteral("/$20"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 1, false, QStringLiteral("genre1"), QStringLiteral("composer1")},
            {true, QStringLiteral("$21"), QStringLiteral("0"), QStringLiteral("track2"),
             QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("artist6"),
             2, 1, QTime::fromMSecsSinceStartOfDay(2), {QUrl::fromLocalFile(QStringLiteral("/$21"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 2, false, QStringLiteral("genre1"), QStringLiteral("composer1")},
            {true, QStringLiteral("$22"), QStringLiteral("0"), QStringLiteral("track3"),
             QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("artist6"),
             3, 1, QTime::fromMSecsSinceStartOfDay(3), {QUrl::fromLocalFile(QStringLiteral("/$22"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 3, false, QStringLiteral("genre1"), QStringLiteral("composer1")},
            {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track4"),
             QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("artist6"),
             4, 1, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 4, false, QStringLiteral("genre1"), QStringLiteral("composer1")},
            {true, QStringLiteral("$24"), QStringLiteral("0"), QStringLiteral("track5"),
             QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("artist6"),
             5, 1, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("/$24"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 5, false, QStringLiteral("genre1"), QStringLiteral("composer1")},
            {true, QStringLiteral("$25"), QStringLiteral("0"), QStringLiteral("track6"),
             QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("artist6"),
             6, 1, QTime::fromMSecsSinceStartOfDay(5), {QUrl::fromLocalFile(QStringLiteral("/$25"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 6, true, QStringLiteral("genre1"), QStringLiteral("composer1")},
            {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
             QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
             2, 2, QTime::fromMSecsSinceStartOfDay(2), {QUrl::fromLocalFile(QStringLiteral("/$26"))},
             {QUrl::fromLocalFile(QStringLiteral("file://image$2"))}, 2, false, QStringLiteral("genre1"), QStringLiteral("composer1")},
        };

        musicDb.insertTracksList(newTracks, mNewCovers, QStringLiteral("autoTestSource2"));

        musicDbTrackAddedSpy.wait(100);

        QCOMPARE(musicDb.allAlbums().count(), 2);
        QCOMPARE(musicDb.allArtists().count(), 3);
        QCOMPARE(musicDb.allTracks().count(), 7);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstAlbum = musicDb.albumFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Invalid Artist"));
    }

    void addTwoIdenticalTracksWithPartialAlbumArtist()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addTwoTracksWithoutAlbumArtist" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = QList<MusicAudioTrack>();

        newTracks = {{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                      QStringLiteral("artist2"), QStringLiteral("album3"), {},
                      6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")},};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 1);
        QCOMPARE(musicDb.allArtists().count(), 1);
        QCOMPARE(musicDb.allTracks().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrack = musicDb.trackFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                                                   QStringLiteral("album3"), 6, 1));

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrack.title(), QStringLiteral("track6"));
        QCOMPARE(firstTrack.artist(), QStringLiteral("artist2"));
        QCOMPARE(firstTrack.albumName(), QStringLiteral("album3"));
        QCOMPARE(firstTrack.albumArtist(), QStringLiteral("artist2"));
        QCOMPARE(firstTrack.isValidAlbumArtist(), false);
        QCOMPARE(firstTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstTrack.trackNumber(), 6);
        QCOMPARE(firstTrack.discNumber(), 1);
        QCOMPARE(firstTrack.duration(), QTime::fromMSecsSinceStartOfDay(23));
        QCOMPARE(firstTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$23")));
        QCOMPARE(firstTrack.rating(), 5);
        QCOMPARE(firstTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(firstTrack.composer(), QStringLiteral("composer1"));

        auto newTracks2 = QList<MusicAudioTrack>();

        newTracks2 = {{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                       QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                       6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                       {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")},};

        auto newCovers2 = mNewCovers;
        newCovers2[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks2, newCovers2, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 1);
        QCOMPARE(musicDb.allArtists().count(), 1);
        QCOMPARE(musicDb.allTracks().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void addTwoTracksWithPartialAlbumArtist2()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addTwoTracksWithPartialAlbumArtist" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDb.allTracks().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = QList<MusicAudioTrack>();

        newTracks = {{true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track7"),
                      QStringLiteral("artist3"), QStringLiteral("album3"), {QStringLiteral("artist4")}, 7, 1,
                      QTime::fromMSecsSinceStartOfDay(20), {QUrl::fromLocalFile(QStringLiteral("/$20"))},
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("file:///$20")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 1);
        QCOMPARE(musicDb.allArtists().count(), 2);
        QCOMPARE(musicDb.allTracks().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks2 = QList<MusicAudioTrack>();

        newTracks2 = {{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                      QStringLiteral("artist4"), QStringLiteral("album3"), {}, 6, 1,
                       QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))},
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true, QStringLiteral("genre1"), QStringLiteral("composer1")}};

        musicDb.insertTracksList(newTracks2, newCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 1);
        QCOMPARE(musicDb.allArtists().count(), 2);
        QCOMPARE(musicDb.allTracks().count(), 2);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrack = musicDb.trackFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist4"),
                                                                                                   QStringLiteral("album3"), 6, 1));

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrack.title(), QStringLiteral("track6"));
        QCOMPARE(firstTrack.artist(), QStringLiteral("artist4"));
        QCOMPARE(firstTrack.albumName(), QStringLiteral("album3"));
        QCOMPARE(firstTrack.albumArtist(), QStringLiteral("artist4"));
        QCOMPARE(firstTrack.isValidAlbumArtist(), true);
        QCOMPARE(firstTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstTrack.trackNumber(), 6);
        QCOMPARE(firstTrack.discNumber(), 1);
        QCOMPARE(firstTrack.duration(), QTime::fromMSecsSinceStartOfDay(19));
        QCOMPARE(firstTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$19")));
        QCOMPARE(firstTrack.rating(), 5);
        QCOMPARE(firstTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(firstTrack.composer(), QStringLiteral("composer1"));

        auto secondTrack = musicDb.trackFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track7"), QStringLiteral("artist3"),
                                                                                                    QStringLiteral("album3"), 7, 1));

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrack.title(), QStringLiteral("track7"));
        QCOMPARE(secondTrack.artist(), QStringLiteral("artist3"));
        QCOMPARE(secondTrack.albumName(), QStringLiteral("album3"));
        QCOMPARE(secondTrack.albumArtist(), QStringLiteral("artist4"));
        QCOMPARE(secondTrack.isValidAlbumArtist(), true);
        QCOMPARE(secondTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondTrack.trackNumber(), 7);
        QCOMPARE(secondTrack.discNumber(), 1);
        QCOMPARE(secondTrack.duration(), QTime::fromMSecsSinceStartOfDay(20));
        QCOMPARE(secondTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$20")));
        QCOMPARE(secondTrack.rating(), 5);
        QCOMPARE(secondTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(secondTrack.composer(), QStringLiteral("composer1"));

        auto album = musicDb.albumFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist4"));

        QCOMPARE(album.isValid(), true);
        QCOMPARE(album.tracksCount(), 2);
        QCOMPARE(album.title(), QStringLiteral("album3"));
        QCOMPARE(album.artist(), QStringLiteral("artist4"));
        QCOMPARE(album.isValidArtist(), true);
        QCOMPARE(album.albumArtURI(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(album.isSingleDiscAlbum(), true);
    }
};

QTEST_GUILESS_MAIN(DatabaseInterfaceTests)


#include "databaseinterfacetest.moc"
