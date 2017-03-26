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

class DatabaseMetadataFetcher : public QObject
{
    Q_OBJECT

public:

    explicit DatabaseMetadataFetcher(const QList<MusicAudioTrack> &newTracks,
                                     QHash<QString, QUrl> newCovers,
                                     QObject *parent = nullptr) : QObject(parent), mNewTracks(newTracks), mNewCovers(newCovers)
    {
    }

Q_SIGNALS:

    void modifyTracksList(const QList<MusicAudioTrack> &modifiedTracks, const QHash<QString, QUrl> &covers);

public Q_SLOTS:

    void newTrackFile(MusicAudioTrack newTrack)
    {
        for (const auto &oneTrack : mNewTracks) {
            if (oneTrack.resourceURI() == newTrack.resourceURI()) {
                if (oneTrack != newTrack) {
                    Q_EMIT modifyTracksList({oneTrack}, mNewCovers);
                }
                break;
            }
        }
    }

private:

    QList<MusicAudioTrack> mNewTracks;

    QHash<QString, QUrl> mNewCovers;

};

class DatabaseInterfaceTests: public QObject
{
    Q_OBJECT

private:

    QList<MusicAudioTrack> mNewTracks;
    QHash<QString, QUrl> mNewCovers;

private Q_SLOTS:

    void initTestCase()
    {
        mNewTracks = {
            {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 1, 1, QTime::fromMSecsSinceStartOfDay(1), {QUrl::fromLocalFile(QStringLiteral("/$1"))},
        {QUrl::fromLocalFile(QStringLiteral("album1"))}, 1},
            {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 2, 2, QTime::fromMSecsSinceStartOfDay(2), {QUrl::fromLocalFile(QStringLiteral("/$2"))},
        {QUrl::fromLocalFile(QStringLiteral("album1"))}, 2},
            {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 3, 3, QTime::fromMSecsSinceStartOfDay(3), {QUrl::fromLocalFile(QStringLiteral("/$3"))},
        {QUrl::fromLocalFile(QStringLiteral("album1"))}, 3},
            {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist4"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 4, 4, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("/$4"))},
        {QUrl::fromLocalFile(QStringLiteral("album1"))}, 4},
            {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist4"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 4, 4, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("/$4Bis"))},
        {QUrl::fromLocalFile(QStringLiteral("album1"))}, 4},
            {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, 1, QTime::fromMSecsSinceStartOfDay(5), {QUrl::fromLocalFile(QStringLiteral("/$5"))},
        {QUrl::fromLocalFile(QStringLiteral("album2"))}, 5},
            {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 2, 1, QTime::fromMSecsSinceStartOfDay(6), {QUrl::fromLocalFile(QStringLiteral("/$6"))},
        {QUrl::fromLocalFile(QStringLiteral("album2"))}, 1},
            {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 3, 1, QTime::fromMSecsSinceStartOfDay(7), {QUrl::fromLocalFile(QStringLiteral("/$7"))},
        {QUrl::fromLocalFile(QStringLiteral("album2"))}, 2},
            {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 4, 1, QTime::fromMSecsSinceStartOfDay(8), {QUrl::fromLocalFile(QStringLiteral("/$8"))},
        {QUrl::fromLocalFile(QStringLiteral("album2"))}, 3},
            {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 5, 1, QTime::fromMSecsSinceStartOfDay(9), {QUrl::fromLocalFile(QStringLiteral("/$9"))},
        {QUrl::fromLocalFile(QStringLiteral("album2"))}, 4},
            {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist1 and artist2"), QStringLiteral("album2"), QStringLiteral("artist1"), 6, 1, QTime::fromMSecsSinceStartOfDay(10), {QUrl::fromLocalFile(QStringLiteral("/$10"))},
        {QUrl::fromLocalFile(QStringLiteral("album2"))}, 5},
            {true, QStringLiteral("$11"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 1, 1, QTime::fromMSecsSinceStartOfDay(11), {QUrl::fromLocalFile(QStringLiteral("/$11"))},
        {QUrl::fromLocalFile(QStringLiteral("album3"))}, 1},
            {true, QStringLiteral("$12"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 2, 1, QTime::fromMSecsSinceStartOfDay(12), {QUrl::fromLocalFile(QStringLiteral("/$12"))},
        {QUrl::fromLocalFile(QStringLiteral("album3"))}, 2},
            {true, QStringLiteral("$13"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 3, 1, QTime::fromMSecsSinceStartOfDay(13), {QUrl::fromLocalFile(QStringLiteral("/$13"))},
        {QUrl::fromLocalFile(QStringLiteral("album3"))}, 3},
            {true, QStringLiteral("$14"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral(""), 1, 1, QTime::fromMSecsSinceStartOfDay(14), {QUrl::fromLocalFile(QStringLiteral("/$14"))},
        {QUrl::fromLocalFile(QStringLiteral("album4"))}, 4},
            {true, QStringLiteral("$15"), QStringLiteral("0"), QStringLiteral("track2"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral(""), 2, 1, QTime::fromMSecsSinceStartOfDay(15), {QUrl::fromLocalFile(QStringLiteral("/$15"))},
        {QUrl::fromLocalFile(QStringLiteral("album4"))}, 5},
            {true, QStringLiteral("$16"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral(""), 3, 1, QTime::fromMSecsSinceStartOfDay(16), {QUrl::fromLocalFile(QStringLiteral("/$16"))},
        {QUrl::fromLocalFile(QStringLiteral("album4"))}, 1},
            {true, QStringLiteral("$17"), QStringLiteral("0"), QStringLiteral("track4"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral(""), 4, 1, QTime::fromMSecsSinceStartOfDay(17), {QUrl::fromLocalFile(QStringLiteral("/$17"))},
        {QUrl::fromLocalFile(QStringLiteral("album4"))}, 2},
            {true, QStringLiteral("$18"), QStringLiteral("0"), QStringLiteral("track5"),
                QStringLiteral("artist2"), QStringLiteral("album4"), QStringLiteral(""), 5, 1, QTime::fromMSecsSinceStartOfDay(18), {QUrl::fromLocalFile(QStringLiteral("/$18"))},
        {QUrl::fromLocalFile(QStringLiteral("album4"))}, 3}
        };

        mNewCovers[QStringLiteral("album1")] = QUrl::fromLocalFile(QStringLiteral("album1"));
        mNewCovers[QStringLiteral("album2")] = QUrl::fromLocalFile(QStringLiteral("album2"));
        mNewCovers[QStringLiteral("album3")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        mNewCovers[QStringLiteral("album4")] = QUrl::fromLocalFile(QStringLiteral("album4"));

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
        musicDb.allAlbums();
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

        DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);

        connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
        connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

        musicDb.init(QStringLiteral("testDb"));

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);

        auto firstAlbum = musicDb.albumFromTitle(QStringLiteral("album1"));

        QCOMPARE(firstAlbum.isValid(), true);
        QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

        auto firstAlbumInvalid = musicDb.albumFromTitle(QStringLiteral("album1Invalid"));

        QCOMPARE(firstAlbumInvalid.isValid(), false);
    }

    void addTwiceSameTracksWidthDatabaseFile()
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

        QTemporaryFile myTempDatabase;
        myTempDatabase.open();

        {
            DatabaseInterface musicDb;

            DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);

            connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
            connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

            musicDb.init(QStringLiteral("testDb1"), myTempDatabase.fileName());

            musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 3);

            auto firstAlbum = musicDb.albumFromTitle(QStringLiteral("album1"));

            QCOMPARE(firstAlbum.isValid(), true);
            QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

            auto firstAlbumInvalid = musicDb.albumFromTitle(QStringLiteral("album1Invalid"));

            QCOMPARE(firstAlbumInvalid.isValid(), false);
        }

        {
            DatabaseInterface musicDb;

            DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);

            connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
            connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

            musicDb.init(QStringLiteral("testDb2"), myTempDatabase.fileName());

            musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 3);

            auto firstAlbum = musicDb.albumFromTitle(QStringLiteral("album1"));

            QCOMPARE(firstAlbum.isValid(), true);
            QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

            auto firstAlbumInvalid = musicDb.albumFromTitle(QStringLiteral("album1Invalid"));

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
            allNewTracks.push_back({true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                                    QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 6, 1, QTime::fromMSecsSinceStartOfDay(19),
                                    {QUrl::fromLocalFile(QStringLiteral("/$19"))}, {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5});

            DatabaseMetadataFetcher metadataFetcher(allNewTracks, mNewCovers);

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);

            connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
            connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

            musicDb.init(QStringLiteral("testDb1"), myTempDatabase.fileName());

            musicDb.insertTracksList(allNewTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 3);

            auto firstAlbum = musicDb.albumFromTitle(QStringLiteral("album1"));

            QCOMPARE(firstAlbum.isValid(), true);
            QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

            auto firstAlbumInvalid = musicDb.albumFromTitle(QStringLiteral("album1Invalid"));

            QCOMPARE(firstAlbumInvalid.isValid(), false);

            auto fourthAlbum = musicDb.albumFromTitle(QStringLiteral("album3"));

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
            allNewTracks.push_back({true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                                    QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 6, 1, QTime::fromMSecsSinceStartOfDay(19),
                                    {QUrl::fromLocalFile(QStringLiteral("/$19"))}, {QUrl::fromLocalFile(QStringLiteral("album3"))}, 3});

            DatabaseMetadataFetcher metadataFetcher(allNewTracks, mNewCovers);

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);

            connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
            connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

            musicDb.init(QStringLiteral("testDb2"), myTempDatabase.fileName());

            auto newFiles = QList<QUrl>();
            for (const auto &oneTrack : allNewTracks) {
                newFiles.push_back(oneTrack.resourceURI());
            }

            musicDb.insertTracksList(allNewTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 3);

            auto firstAlbum = musicDb.albumFromTitle(QStringLiteral("album1"));

            QCOMPARE(firstAlbum.isValid(), true);
            QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

            auto firstAlbumInvalid = musicDb.albumFromTitle(QStringLiteral("album1Invalid"));

            QCOMPARE(firstAlbumInvalid.isValid(), false);

            auto fourthAlbum = musicDb.albumFromTitle(QStringLiteral("album3"));

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

        DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);

        connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
        connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

        musicDb.init(QStringLiteral("testDb"));

        auto newFiles = QList<QUrl>();
        for (const auto &oneTrack : mNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);

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
        auto firstTrackRating = firstTrack.rating();

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

        DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

        connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
        connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

        musicDb.init(QStringLiteral("testDbVariousArtistAlbum"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);

        auto newFiles = QList<QUrl>();
        for (const auto &oneTrack : mNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));
        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);

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

        auto invalidTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("invalidArtist1"));
        QCOMPARE(invalidTrackId, decltype(invalidTrackId)(0));

        auto firstTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));
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

        auto secondTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track2"), QStringLiteral("album1"), QStringLiteral("artist2"));
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

        auto thirdTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist3"));
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

        auto fourthTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track4"), QStringLiteral("album1"), QStringLiteral("artist4"));
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

        DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

        connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
        connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

        musicDb.init(QStringLiteral("testDbVariousArtistAlbum"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);

        auto newFiles = QList<QUrl>();
        for (const auto &oneTrack : mNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 6, 1, QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$19"))}, 5};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newFiles2 = QList<QUrl>();
        for (const auto &oneTrack : newTracks) {
            newFiles2.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(newTracks, mNewCovers, QStringLiteral("autoTest2"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDbArtistAddedSpy.count(), 7);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 14);

        auto allTracks = musicDb.allTracksFromSource(QStringLiteral("autoTest"));

        auto allTracks2 = musicDb.allTracksFromSource(QStringLiteral("autoTest2"));

        QCOMPARE(allTracks.count(), 13);
        QCOMPARE(allTracks2.count(), 1);
    }

    void simpleAccessorAndVariousArtistAlbumWithFile()
    {
        QTemporaryFile myDatabaseFile;
        myDatabaseFile.open();

        {
            DatabaseInterface musicDb;

            DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

            connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
            connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

            QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistAdded);
            QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumAdded);
            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);

            QCOMPARE(musicDb.allAlbums().count(), 0);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);

            musicDb.init(QStringLiteral("testDbVariousArtistAlbum1"), myDatabaseFile.fileName());

            QCOMPARE(musicDb.allAlbums().count(), 0);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);

            auto newFiles = QList<QUrl>();
            for (const auto &oneTrack : mNewTracks) {
                newFiles.push_back(oneTrack.resourceURI());
            }

            musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 3);
            QCOMPARE(musicDbArtistAddedSpy.count(), 6);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
            QCOMPARE(musicDbTrackAddedSpy.count(), 13);

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

            auto invalidTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("invalidArtist1"));
            QCOMPARE(invalidTrackId, decltype(invalidTrackId)(0));

            auto firstTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));
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

            auto secondTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track2"), QStringLiteral("album1"), QStringLiteral("artist2"));
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

            auto thirdTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist3"));
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

            auto fourthTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track4"), QStringLiteral("album1"), QStringLiteral("artist4"));
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

            DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

            connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
            connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

            QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistAdded);
            QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumAdded);
            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);

            QCOMPARE(musicDb.allAlbums().count(), 0);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);

            musicDb.init(QStringLiteral("testDbVariousArtistAlbum2"), myDatabaseFile.fileName());

            QCOMPARE(musicDb.allAlbums().count(), 3);
            QCOMPARE(musicDbArtistAddedSpy.count(), 6);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
            QCOMPARE(musicDbTrackAddedSpy.count(), 13);

            auto newFiles = QList<QUrl>();
            for (const auto &oneTrack : mNewTracks) {
                newFiles.push_back(oneTrack.resourceURI());
            }

            musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 3);
            QCOMPARE(musicDbArtistAddedSpy.count(), 6);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
            QCOMPARE(musicDbTrackAddedSpy.count(), 13);

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

            auto invalidTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("invalidArtist1"));
            QCOMPARE(invalidTrackId, decltype(invalidTrackId)(0));

            auto firstTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));
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

            auto secondTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track2"), QStringLiteral("album1"), QStringLiteral("artist2"));
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

            auto thirdTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist3"));
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

            auto fourthTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track4"), QStringLiteral("album1"), QStringLiteral("artist4"));
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

            auto newTrack = MusicAudioTrack{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                    QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 6, 1, QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))},
            {QUrl::fromLocalFile(QStringLiteral("file://image$19"))}, 5};
            auto newTracks = QList<MusicAudioTrack>();
            newTracks.push_back(newTrack);

            auto newFiles2 = QList<QUrl>();
            for (const auto &oneTrack : newTracks) {
                newFiles2.push_back(oneTrack.resourceURI());
            }

            musicDb.insertTracksList(newTracks, mNewCovers, QStringLiteral("autoTest"));

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbums().count(), 3);
            QCOMPARE(musicDbArtistAddedSpy.count(), 7);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
            QCOMPARE(musicDbTrackAddedSpy.count(), 14);
        }
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

        DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

        connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
        connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

        musicDb.init(QStringLiteral("testDbVariousArtistAlbum"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);

        auto newFiles = QList<QUrl>();
        for (const auto &oneTrack : mNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);

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

        DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

        connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
        connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);

        auto newFiles = QList<QUrl>();
        for (const auto &oneTrack : mNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDb.allArtists().count(), 6);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 14);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);

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

        auto trackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));

        auto firstTrack = musicDb.trackFromDatabaseId(trackId);

        musicDb.removeTracksList({firstTrack.resourceURI()});

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDb.allArtists().count(), 6);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 1);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 15);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);

        auto removedTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));
        QCOMPARE(removedTrackId, qulonglong(0));
    }

    void removeOneAlbum()
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

        DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

        connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
        connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);

        auto newFiles = QList<QUrl>();
        for (const auto &oneTrack : mNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDb.allArtists().count(), 6);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 14);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);

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

        auto firstTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track1"), QStringLiteral("album1"), QStringLiteral("artist1"));
        auto firstTrack = musicDb.trackFromDatabaseId(firstTrackId);
        auto secondTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track2"), QStringLiteral("album1"), QStringLiteral("artist2"));
        auto secondTrack = musicDb.trackFromDatabaseId(secondTrackId);
        auto thirdTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist3"));
        auto thirdTrack = musicDb.trackFromDatabaseId(thirdTrackId);
        auto fourthTrackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track4"), QStringLiteral("album1"), QStringLiteral("artist4"));
        auto fourthTrack = musicDb.trackFromDatabaseId(fourthTrackId);

        musicDb.removeTracksList({firstTrack.resourceURI(), secondTrack.resourceURI(), thirdTrack.resourceURI(), fourthTrack.resourceURI()});

        QCOMPARE(musicDb.allAlbums().count(), 2);
        QCOMPARE(musicDb.allArtists().count(), 4);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 2);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 1);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 4);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 18);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);

        auto removedAlbum = musicDb.albumFromTitle(QStringLiteral("album1"));

        QCOMPARE(removedAlbum.isValid(), false);
    }

    void removeOneArtist()
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

        DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

        connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
        connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);

        auto newFiles = QList<QUrl>();
        for (const auto &oneTrack : mNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDb.allArtists().count(), 6);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 14);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);

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

        auto trackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track4"), QStringLiteral("album1"), QStringLiteral("artist4"));
        auto track = musicDb.trackFromDatabaseId(trackId);

        musicDb.removeTracksList({track.resourceURI()});

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDb.allArtists().count(), 5);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 1);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 15);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
    }
    void addOneTrack()
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

        DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

        connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
        connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);

        auto newFiles = QList<QUrl>();
        for (const auto &oneTrack : mNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDb.allArtists().count(), 6);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 14);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"), 6, 1, QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$19"))}, 5};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        musicDb.insertTracksList(newTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDb.allArtists().count(), 6);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 14);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 15);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
    }


    void modifyOneTrack()
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

        DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

        connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
        connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);

        auto newFiles = QList<QUrl>();
        for (const auto &oneTrack : mNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDb.allArtists().count(), 6);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 14);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);

        auto modifiedTrack = MusicAudioTrack{true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
                QStringLiteral("artist3"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 5, 3,
                QTime::fromMSecsSinceStartOfDay(3), {QUrl::fromLocalFile(QStringLiteral("/$3"))}, {QUrl::fromLocalFile(QStringLiteral("file://image$3"))}, 5};

        musicDb.modifyTracksList({modifiedTrack}, mNewCovers);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDb.allArtists().count(), 6);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 15);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 2);

        auto trackId = musicDb.trackIdFromTitleAlbumArtist(QStringLiteral("track3"), QStringLiteral("album1"), QStringLiteral("artist3"));
        QCOMPARE(trackId != 0, true);
        auto track = musicDb.trackFromDatabaseId(trackId);

        QCOMPARE(track.isValid(), true);
        QCOMPARE(track.trackNumber(), 5);
    }

    void addOneAlbum()
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

        DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

        connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
        connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);

        auto newFiles = QList<QUrl>();
        for (const auto &oneTrack : mNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDb.allArtists().count(), 6);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 14);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track1"),
                QStringLiteral("artist2"), QStringLiteral("album5"), QStringLiteral("artist2"), 1, 1, QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$19"))}, 5};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newCover = QUrl::fromLocalFile(QStringLiteral("album5"));
        auto newCovers = QHash<QString, QUrl>();
        newCovers[QStringLiteral("album5")] = newCover;

        musicDb.insertTracksList(newTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 4);
        QCOMPARE(musicDb.allArtists().count(), 6);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 4);
        QCOMPARE(musicDbTrackAddedSpy.count(), 14);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 15);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
    }

    void addOneArtist()
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

        DatabaseMetadataFetcher metadataFetcher(mNewTracks, mNewCovers);

        connect(&musicDb, &DatabaseInterface::newTrackFile, &metadataFetcher, &DatabaseMetadataFetcher::newTrackFile, Qt::QueuedConnection);
        connect(&metadataFetcher, &DatabaseMetadataFetcher::modifyTracksList, &musicDb, &DatabaseInterface::modifyTracksList, Qt::QueuedConnection);

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::trackAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbArtistModifiedSpy(&musicDb, &DatabaseInterface::artistModified);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);

        QCOMPARE(musicDb.allAlbums().count(), 0);
        QCOMPARE(musicDb.allArtists().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);

        auto newFiles = QList<QUrl>();
        for (const auto &oneTrack : mNewTracks) {
            newFiles.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDb.allArtists().count(), 6);
        QCOMPARE(musicDbArtistAddedSpy.count(), 6);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 13);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 14);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);

        auto newTrack = MusicAudioTrack{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("Various Artists"), 6, 1, QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))},
        {QUrl::fromLocalFile(QStringLiteral("file://image$19"))}, 5};
        auto newTracks = QList<MusicAudioTrack>();
        newTracks.push_back(newTrack);

        auto newFiles2 = QList<QUrl>();
        for (const auto &oneTrack : newTracks) {
            newFiles2.push_back(oneTrack.resourceURI());
        }

        musicDb.insertTracksList(newTracks, mNewCovers, QStringLiteral("autoTest"));

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbums().count(), 3);
        QCOMPARE(musicDb.allArtists().count(), 7);
        QCOMPARE(musicDbArtistAddedSpy.count(), 7);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 3);
        QCOMPARE(musicDbTrackAddedSpy.count(), 14);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbArtistModifiedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 15);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
    }
};

QTEST_MAIN(DatabaseInterfaceTests)


#include "databaseinterfacetest.moc"
