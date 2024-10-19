/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "databasetestdata.h"

#include "databaseinterface.h"
#include "datatypes.h"

#include "config-upnp-qt.h"

#include <QObject>
#include <QUrl>
#include <QString>
#include <QHash>
#include <QList>
#include <QThread>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTemporaryFile>

#include <QDebug>

#include <QTest>
#include <QSignalSpy>

#include <algorithm>

using namespace Qt::Literals::StringLiterals;

class DatabaseInterfaceTests: public QObject, public DatabaseTestData
{
    Q_OBJECT

public:

    explicit DatabaseInterfaceTests(QObject *aParent = nullptr) : QObject(aParent)
    {
    }

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QList<qlonglong>>("QList<qlonglong>");
        qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
        qRegisterMetaType<QHash<QUrl,QDateTime>>("QHash<QUrl,QDateTime>");
        qRegisterMetaType<DataTypes::ListTrackDataType>("ListTrackDataType");
        qRegisterMetaType<DataTypes::ListAlbumDataType>("ListAlbumDataType");
        qRegisterMetaType<DataTypes::ListArtistDataType>("ListArtistDataType");
        qRegisterMetaType<DataTypes::ListGenreDataType>("ListGenreDataType");
        qRegisterMetaType<DataTypes::TrackDataType>("TrackDataType");
        qRegisterMetaType<DataTypes::AlbumDataType>("AlbumDataType");
        qRegisterMetaType<DataTypes::ArtistDataType>("ArtistDataType");
        qRegisterMetaType<DataTypes::GenreDataType>("GenreDataType");
        qRegisterMetaType<DataTypes::ListRadioDataType>("ListRadioDataType");
    }

    void avoidCrashInTrackIdFromTitleAlbumArtist()
    {
        DatabaseInterface musicDb;
        musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album3"), 1, 1);
    }

    void avoidCrashInAllArtists()
    {
        DatabaseInterface musicDb;
        musicDb.allArtistsData();
    }

    void avoidCrashInallAlbumsData()
    {
        DatabaseInterface musicDb;
        musicDb.allAlbumsData();
    }

    void avoidCrashInAllRadios()
    {
        DatabaseInterface musicDb;
        musicDb.allRadiosData();
    }

    void verifyNumberOfRadioStations()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();
        qDebug() << "verifyNumberOfRadioStations" << databaseFile.fileName();
        DatabaseInterface musicDb;
        musicDb.init(QStringLiteral("testDb"),databaseFile.fileName());
        QSignalSpy musicDbRadioAddedSpy(&musicDb,&DatabaseInterface::radioAdded);
        QCOMPARE(musicDb.allRadiosData().count(), 18);
    }

    void addOneRadio()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();
        qDebug() << "addOneRadio" << databaseFile.fileName();
        DatabaseInterface musicDb;
        musicDb.init(QStringLiteral("testDb"),databaseFile.fileName());
        QSignalSpy musicDbRadioAddedSpy(&musicDb,&DatabaseInterface::radioAdded);
        int initialCount = musicDb.allRadiosData().count();

        auto newRadio = DataTypes::TrackDataType();
        newRadio[DataTypes::CommentRole]=QStringLiteral("Test Comment");
        newRadio[DataTypes::ResourceRole]=QUrl::fromEncoded("http://ice2.somafm.com/defcon-128-aac");
        newRadio[DataTypes::TitleRole]=QStringLiteral("Test Radio");
        newRadio[DataTypes::RatingRole]=9;
        newRadio[DataTypes::ImageUrlRole]=QUrl::fromEncoded("https://somafm.com/img3/defcon400.png");
        newRadio[DataTypes::ElementTypeRole]=ElisaUtils::Radio;
        auto newRadios = DataTypes::ListRadioDataType();

        newRadios.push_back(newRadio);
        //Covers are ignored for radios.
        musicDb.insertTracksList(newRadios,mNewCovers);
        musicDbRadioAddedSpy.wait(300);

        QCOMPARE(musicDb.allRadiosData().count(),initialCount + 1);
        QCOMPARE(musicDbRadioAddedSpy.count(),1);
        databaseFile.close();
    }

    void modifyOneRadio(){
        QTemporaryFile databaseFile;
        databaseFile.open();
        qDebug() << "modifyOneRadio" << databaseFile.fileName();
        DatabaseInterface musicDb;
        musicDb.init(QStringLiteral("testDb"),databaseFile.fileName());
        QSignalSpy musicDbRadioAddedSpy(&musicDb,&DatabaseInterface::radioAdded);
        QSignalSpy musicDbRadioModifiedSpy(&musicDb,&DatabaseInterface::radioModified);
        int initialCount = musicDb.allRadiosData().count();

        auto newRadio = DataTypes::TrackDataType();
        newRadio[DataTypes::CommentRole]=QStringLiteral("Test Comment");
        newRadio[DataTypes::ResourceRole]=QUrl::fromEncoded("http://ice2.somafm.com/defcon-128-aac");
        newRadio[DataTypes::TitleRole]=QStringLiteral("Test Radio");
        newRadio[DataTypes::RatingRole]=9;
        newRadio[DataTypes::ImageUrlRole]=QUrl::fromEncoded("https://somafm.com/img3/defcon400.png");
        newRadio[DataTypes::ElementTypeRole]=ElisaUtils::Radio;
        auto newRadios = DataTypes::ListRadioDataType();

        newRadios.push_back(newRadio);
        //Covers are ignored for radios.
        musicDb.insertTracksList(newRadios,mNewCovers);
        musicDbRadioAddedSpy.wait(300);

        QCOMPARE(musicDb.allRadiosData().count(),initialCount + 1);
        QCOMPARE(musicDbRadioAddedSpy.count(),1);

        newRadios.clear();

        auto radioId = musicDb.radioIdFromFileName(QUrl::fromEncoded("http://ice2.somafm.com/defcon-128-aac"));
        auto modifiedRadio = musicDb.radioDataFromDatabaseId(radioId);
        modifiedRadio[DataTypes::CommentRole]=QStringLiteral("News Test Comment");
        newRadios.push_back(modifiedRadio);
        musicDb.insertTracksList(newRadios,mNewCovers);

        QCOMPARE(musicDb.allRadiosData().count(),initialCount + 1);
        QCOMPARE(musicDbRadioAddedSpy.count(),1);
        QCOMPARE(musicDbRadioModifiedSpy.count(),1);
        databaseFile.close();
    }

    void addOneTrackWithoutAlbumArtist()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addOneTrackWithoutAlbumArtist" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), {},
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto track = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                                                  QStringLiteral("album3"), 6, 1));

        QCOMPARE(track.isValid(), true);
        QCOMPARE(track.title(), QStringLiteral("track6"));
        QCOMPARE(track.artist(), QStringLiteral("artist2"));
        QCOMPARE(track.album(), QStringLiteral("album3"));
        QVERIFY(!track.albumArtist().isEmpty());
        QCOMPARE(track.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(track.trackNumber(), 6);
        QCOMPARE(track.discNumber(), 1);
        QCOMPARE(track.duration(), QTime::fromMSecsSinceStartOfDay(23));
        QCOMPARE(track.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$23")));
        QCOMPARE(track.rating(), 5);
        QCOMPARE(track.genre(), QStringLiteral("genre1"));
        QCOMPARE(track.composer(), QStringLiteral("composer1"));
        QCOMPARE(track.lyricist(), QStringLiteral("lyricist1"));

        auto albumId = musicDb.albumIdFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist2"), QStringLiteral("/"));
        QVERIFY(albumId != 0);
        auto album = musicDb.albumDataFromDatabaseId(albumId);
        auto albumData = musicDb.albumData(albumId);

        QCOMPARE(album.isValid(), true);
        QCOMPARE(albumData.count(), 1);
        QCOMPARE(album.title(), QStringLiteral("album3"));
        QCOMPARE(album.artist(), QStringLiteral("artist2"));
        QCOMPARE(album[DataTypes::ColumnsRoles::SecondaryTextRole], QStringLiteral("artist2"));
        QVERIFY(album.isValidArtist());
        QCOMPARE(album.albumArtURI(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(album.isSingleDiscAlbum(), true);
    }

    void addAndRemoveOneTrackWithoutAlbum()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addAndRemoveOneTrackWithoutAlbum" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType {true, QStringLiteral("$24"), QStringLiteral("0"), QStringLiteral("track10"),
                QStringLiteral("artist8"), {}, QStringLiteral("artist8"),
                9, 1, QTime::fromMSecsSinceStartOfDay(24), {QUrl::fromLocalFile(QStringLiteral("/$24"))}, QDateTime::fromMSecsSinceEpoch(24),
        {}, 9, true, QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};

        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$24")] = QUrl::fromLocalFile(QStringLiteral("album4"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto trackId = musicDb.trackIdFromFileName(QUrl::fromLocalFile(QStringLiteral("/$24")));
        QVERIFY(trackId != 0);
        auto track = musicDb.trackDataFromDatabaseId(trackId);

        QCOMPARE(track.isValid(), true);
        QCOMPARE(track.title(), QStringLiteral("track10"));
        QCOMPARE(track.artist(), QStringLiteral("artist8"));
        QCOMPARE(track.album(), QString());
        QVERIFY(!track.albumArtist().isEmpty());
        QCOMPARE(track.albumCover(), QUrl());
        QCOMPARE(track.trackNumber(), 9);
        QCOMPARE(track.discNumber(), 1);
        QCOMPARE(track.duration(), QTime::fromMSecsSinceStartOfDay(24));
        QCOMPARE(track.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$24")));
        QCOMPARE(track.rating(), 9);
        QCOMPARE(track.genre(), QStringLiteral("genre1"));
        QCOMPARE(track.composer(), QStringLiteral("composer1"));
        QCOMPARE(track.lyricist(), QStringLiteral("lyricist1"));

        musicDb.removeTracksList({track.resourceURI()});

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void addAndRemoveOneTrackWithoutTrackNumber()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType {true, QStringLiteral("$26"), QStringLiteral("0"), QStringLiteral("track12"),
                QStringLiteral("artist8"), QStringLiteral("album4"), QStringLiteral("artist8"),
                -1, 1, QTime::fromMSecsSinceStartOfDay(26), {QUrl::fromLocalFile(QStringLiteral("/$26"))}, QDateTime::fromMSecsSinceEpoch(26),
                QUrl::fromLocalFile(QStringLiteral("file://image$26")), 9, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        newTrack.remove(DataTypes::TrackNumberRole);
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$26")] = QUrl::fromLocalFile(QStringLiteral("album4"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto track = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromFileName(QUrl::fromLocalFile(QStringLiteral("/$26"))));

        QCOMPARE(track.isValid(), true);
        QCOMPARE(track.title(), QStringLiteral("track12"));
        QCOMPARE(track.artist(), QStringLiteral("artist8"));
        QCOMPARE(track.album(), QStringLiteral("album4"));
        QCOMPARE(track.albumArtist(), QStringLiteral("artist8"));
        QCOMPARE(track.albumCover(), QUrl::fromLocalFile(QStringLiteral("album4")));
        QCOMPARE(track.hasTrackNumber(), false);
        QCOMPARE(track.discNumber(), 1);
        QCOMPARE(track.duration(), QTime::fromMSecsSinceStartOfDay(26));
        QCOMPARE(track.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$26")));
        QCOMPARE(track.rating(), 9);
        QCOMPARE(track.genre(), QStringLiteral("genre1"));
        QCOMPARE(track.composer(), QStringLiteral("composer1"));
        QCOMPARE(track.lyricist(), QStringLiteral("lyricist1"));

        musicDb.removeTracksList({track.resourceURI()});

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 1);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void addAndRemoveOneTrackWithoutArtist()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType {true, QStringLiteral("$26"), QStringLiteral("0"), QStringLiteral("track11"),
        {}, QStringLiteral("album4"), {},
                9, 1, QTime::fromMSecsSinceStartOfDay(26), {QUrl::fromLocalFile(QStringLiteral("/$26"))}, QDateTime::fromMSecsSinceEpoch(26),
                QUrl::fromLocalFile(QStringLiteral("file://image$26")), 9, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$26")] = QUrl::fromLocalFile(QStringLiteral("album4"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto track = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromFileName(QUrl::fromLocalFile(QStringLiteral("/$26"))));

        QCOMPARE(track.isValid(), true);
        QCOMPARE(track.title(), QStringLiteral("track11"));
        QCOMPARE(track.artist(), QString());
        QCOMPARE(track.album(), QStringLiteral("album4"));
        QVERIFY(track.albumArtist().isEmpty());
        QCOMPARE(track.albumCover(), QUrl::fromLocalFile(QStringLiteral("album4")));
        QCOMPARE(track.trackNumber(), 9);
        QCOMPARE(track.discNumber(), 1);
        QCOMPARE(track.duration(), QTime::fromMSecsSinceStartOfDay(26));
        QCOMPARE(track.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$26")));
        QCOMPARE(track.rating(), 9);
        QCOMPARE(track.genre(), QStringLiteral("genre1"));
        QCOMPARE(track.composer(), QStringLiteral("composer1"));
        QCOMPARE(track.lyricist(), QStringLiteral("lyricist1"));

        musicDb.removeTracksList({track.resourceURI()});

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 1);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void addOneTrackWithoutAlbumArtistAndAnotherTrackWith()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType {true, QStringLiteral("$26"), QStringLiteral("0"), QStringLiteral("track11"),
        {}, QStringLiteral("album4"), {},
                9, 1, QTime::fromMSecsSinceStartOfDay(26), {QUrl::fromLocalFile(QStringLiteral("/$26"))}, QDateTime::fromMSecsSinceEpoch(26),
                QUrl::fromLocalFile(QStringLiteral("file://image$26")), 9, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$26")] = QUrl::fromLocalFile(QStringLiteral("album4"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto track = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromFileName(QUrl::fromLocalFile(QStringLiteral("/$26"))));

        QCOMPARE(track.isValid(), true);
        QCOMPARE(track.title(), QStringLiteral("track11"));
        QCOMPARE(track.artist(), QString());
        QCOMPARE(track.album(), QStringLiteral("album4"));
        QCOMPARE(track.albumArtist(), QString());
        QVERIFY(track.albumArtist().isEmpty());
        QCOMPARE(track.albumCover(), QUrl::fromLocalFile(QStringLiteral("album4")));
        QCOMPARE(track.trackNumber(), 9);
        QCOMPARE(track.discNumber(), 1);
        QCOMPARE(track.duration(), QTime::fromMSecsSinceStartOfDay(26));
        QCOMPARE(track.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$26")));
        QCOMPARE(track.rating(), 9);
        QCOMPARE(track.genre(), QStringLiteral("genre1"));
        QCOMPARE(track.composer(), QStringLiteral("composer1"));
        QCOMPARE(track.lyricist(), QStringLiteral("lyricist1"));

        newTrack = DataTypes::TrackDataType {true, QStringLiteral("$27"), QStringLiteral("0"), QStringLiteral("track12"),
                QStringLiteral("artist1"), QStringLiteral("album4"), QStringLiteral("artist2"),
                10, 1, QTime::fromMSecsSinceStartOfDay(27), {QUrl::fromLocalFile(QStringLiteral("/autre/$27"))}, QDateTime::fromMSecsSinceEpoch(27),
                QUrl::fromLocalFile(QStringLiteral("file://image$27")), 10, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///autre/$27")] = QUrl::fromLocalFile(QStringLiteral("album4"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 2);
        QCOMPARE(musicDb.allArtistsData().count(), 2);
        QCOMPARE(musicDb.allTracksData().count(), 2);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 2);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType();

        newTracks = {{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                      QStringLiteral("artist2"), QStringLiteral("album3"), {}, 6, 1, QTime::fromMSecsSinceStartOfDay(19),
                      {QUrl::fromLocalFile(QStringLiteral("/$19"))},
                      QDateTime::fromMSecsSinceEpoch(23),
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                      QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), true},
                     {true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track7"),
                      QStringLiteral("artist3"), QStringLiteral("album3"), {}, 7, 1, QTime::fromMSecsSinceStartOfDay(20),
                      {QUrl::fromLocalFile(QStringLiteral("/$20"))},
                      QDateTime::fromMSecsSinceEpoch(23),
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                      QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("file:///$20")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 2);
        QCOMPARE(musicDb.allTracksData().count(), 2);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrack = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                                                       QStringLiteral("album3"), 6, 1));

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrack.title(), QStringLiteral("track6"));
        QCOMPARE(firstTrack.artist(), QStringLiteral("artist2"));
        QCOMPARE(firstTrack.album(), QStringLiteral("album3"));
        QVERIFY(!firstTrack.albumArtist().isEmpty());
        QCOMPARE(firstTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstTrack.trackNumber(), 6);
        QCOMPARE(firstTrack.discNumber(), 1);
        QCOMPARE(firstTrack.duration(), QTime::fromMSecsSinceStartOfDay(19));
        QCOMPARE(firstTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$19")));
        QCOMPARE(firstTrack.rating(), 5);
        QCOMPARE(firstTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(firstTrack.composer(), QStringLiteral("composer1"));
        QCOMPARE(firstTrack.lyricist(), QStringLiteral("lyricist1"));
        QCOMPARE(firstTrack.hasEmbeddedCover(), true);

        auto secondTrack = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track7"), QStringLiteral("artist3"),
                                                                                                        QStringLiteral("album3"), 7, 1));

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrack.title(), QStringLiteral("track7"));
        QCOMPARE(secondTrack.artist(), QStringLiteral("artist3"));
        QCOMPARE(secondTrack.album(), QStringLiteral("album3"));
        QVERIFY(!secondTrack.albumArtist().isEmpty());
        QCOMPARE(secondTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondTrack.trackNumber(), 7);
        QCOMPARE(secondTrack.discNumber(), 1);
        QCOMPARE(secondTrack.duration(), QTime::fromMSecsSinceStartOfDay(20));
        QCOMPARE(secondTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$20")));
        QCOMPARE(secondTrack.rating(), 5);
        QCOMPARE(secondTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(secondTrack.composer(), QStringLiteral("composer1"));
        QCOMPARE(secondTrack.lyricist(), QStringLiteral("lyricist1"));
        QCOMPARE(secondTrack.hasEmbeddedCover(), false);

        auto albumId = musicDb.albumIdFromTitleAndArtist(QStringLiteral("album3"), {}, QStringLiteral("/"));
        auto album = musicDb.albumDataFromDatabaseId(albumId);
        auto albumData = musicDb.albumData(albumId);

        QCOMPARE(album.isValid(), true);
        QCOMPARE(albumData.count(), 2);
        QCOMPARE(album.title(), QStringLiteral("album3"));
        QVERIFY(album.isValidArtist());
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType();

        newTracks = {{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                      QStringLiteral("artist2"), QStringLiteral("album3"), {}, 6, 1,
                      QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))},
                      QDateTime::fromMSecsSinceEpoch(19),
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                      QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
                     {true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track7"),
                      QStringLiteral("artist2"), QStringLiteral("album3"), {}, 7, 1,
                      QTime::fromMSecsSinceStartOfDay(20), {QUrl::fromLocalFile(QStringLiteral("/$20"))},
                      QDateTime::fromMSecsSinceEpoch(20),
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 4, true,
                      QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
                     {true, QStringLiteral("$21"), QStringLiteral("0"), QStringLiteral("track8"),
                      QStringLiteral("artist2"), QStringLiteral("album3"), {}, 8, 1,
                      QTime::fromMSecsSinceStartOfDay(21), {QUrl::fromLocalFile(QStringLiteral("/$21"))},
                      QDateTime::fromMSecsSinceEpoch(21),
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 3, true,
                      QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$19")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("file:///$20")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("file:///$21")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 3);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrack = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                                                       QStringLiteral("album3"), 6, 1));

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrack.title(), QStringLiteral("track6"));
        QCOMPARE(firstTrack.artist(), QStringLiteral("artist2"));
        QCOMPARE(firstTrack.album(), QStringLiteral("album3"));
        QVERIFY(!firstTrack.albumArtist().isEmpty());
        QCOMPARE(firstTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstTrack.trackNumber(), 6);
        QCOMPARE(firstTrack.discNumber(), 1);
        QCOMPARE(firstTrack.duration(), QTime::fromMSecsSinceStartOfDay(19));
        QCOMPARE(firstTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$19")));
        QCOMPARE(firstTrack.rating(), 5);
        QCOMPARE(firstTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(firstTrack.composer(), QStringLiteral("composer1"));
        QCOMPARE(firstTrack.lyricist(), QStringLiteral("lyricist1"));
        QCOMPARE(firstTrack.isSingleDiscAlbum(), true);

        auto secondTrack = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track7"), QStringLiteral("artist2"),
                                                                                                        QStringLiteral("album3"), 7, 1));

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrack.title(), QStringLiteral("track7"));
        QCOMPARE(secondTrack.artist(), QStringLiteral("artist2"));
        QCOMPARE(secondTrack.album(), QStringLiteral("album3"));
        QVERIFY(!secondTrack.albumArtist().isEmpty());
        QCOMPARE(secondTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondTrack.trackNumber(), 7);
        QCOMPARE(secondTrack.discNumber(), 1);
        QCOMPARE(secondTrack.duration(), QTime::fromMSecsSinceStartOfDay(20));
        QCOMPARE(secondTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$20")));
        QCOMPARE(secondTrack.rating(), 4);
        QCOMPARE(secondTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(secondTrack.composer(), QStringLiteral("composer1"));
        QCOMPARE(secondTrack.lyricist(), QStringLiteral("lyricist1"));
        QCOMPARE(secondTrack.isSingleDiscAlbum(), true);
        QCOMPARE(secondTrack.hasEmbeddedCover(), false);

        auto thirdTrack = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track8"), QStringLiteral("artist2"),
                                                                                                       QStringLiteral("album3"), 8, 1));

        QCOMPARE(thirdTrack.isValid(), true);
        QCOMPARE(thirdTrack.title(), QStringLiteral("track8"));
        QCOMPARE(thirdTrack.artist(), QStringLiteral("artist2"));
        QCOMPARE(thirdTrack.album(), QStringLiteral("album3"));
        QVERIFY(!thirdTrack.albumArtist().isEmpty());
        QCOMPARE(thirdTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(thirdTrack.trackNumber(), 8);
        QCOMPARE(thirdTrack.discNumber(), 1);
        QCOMPARE(thirdTrack.duration(), QTime::fromMSecsSinceStartOfDay(21));
        QCOMPARE(thirdTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$21")));
        QCOMPARE(thirdTrack.rating(), 3);
        QCOMPARE(thirdTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(thirdTrack.composer(), QStringLiteral("composer1"));
        QCOMPARE(thirdTrack.lyricist(), QStringLiteral("lyricist1"));
        QCOMPARE(thirdTrack.isSingleDiscAlbum(), true);

        auto albumId = musicDb.albumIdFromTitleAndArtist(QStringLiteral("album3"), {}, QStringLiteral("/"));
        auto album = musicDb.albumDataFromDatabaseId(albumId);
        auto albumData = musicDb.albumData(albumId);

        QCOMPARE(album.isValid(), true);
        QCOMPARE(albumData.count(), 3);
        QCOMPARE(album.title(), QStringLiteral("album3"));
        QVERIFY(album.isValidArtist());
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType();

        newTracks = {{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                      QStringLiteral("artist2"), QStringLiteral("album3"), {}, 6, 1,
                      QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))},
                      QDateTime::fromMSecsSinceEpoch(19),
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                      QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), true},
                     {true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track7"),
                      QStringLiteral("artist3"), QStringLiteral("album3"), {QStringLiteral("artist4")}, 7, 1,
                      QTime::fromMSecsSinceStartOfDay(20), {QUrl::fromLocalFile(QStringLiteral("/$20"))},
                      QDateTime::fromMSecsSinceEpoch(20),
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                      QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$19")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("file:///$20")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 3);
        QCOMPARE(musicDb.allTracksData().count(), 2);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrack = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                                                       QStringLiteral("album3"), 6, 1));

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrack.title(), QStringLiteral("track6"));
        QCOMPARE(firstTrack.artist(), QStringLiteral("artist2"));
        QCOMPARE(firstTrack.album(), QStringLiteral("album3"));
        QCOMPARE(firstTrack.albumArtist(), QStringLiteral("artist4"));
        QCOMPARE(firstTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstTrack.trackNumber(), 6);
        QCOMPARE(firstTrack.discNumber(), 1);
        QCOMPARE(firstTrack.duration(), QTime::fromMSecsSinceStartOfDay(19));
        QCOMPARE(firstTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$19")));
        QCOMPARE(firstTrack.rating(), 5);
        QCOMPARE(firstTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(firstTrack.composer(), QStringLiteral("composer1"));
        QCOMPARE(firstTrack.lyricist(), QStringLiteral("lyricist1"));
        QCOMPARE(firstTrack.isSingleDiscAlbum(), true);
        QCOMPARE(firstTrack.hasEmbeddedCover(), true);

        auto secondTrack = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track7"), QStringLiteral("artist3"),
                                                                                                        QStringLiteral("album3"), 7, 1));

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrack.title(), QStringLiteral("track7"));
        QCOMPARE(secondTrack.artist(), QStringLiteral("artist3"));
        QCOMPARE(secondTrack.album(), QStringLiteral("album3"));
        QCOMPARE(secondTrack.albumArtist(), QStringLiteral("artist4"));
        QCOMPARE(secondTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondTrack.trackNumber(), 7);
        QCOMPARE(secondTrack.discNumber(), 1);
        QCOMPARE(secondTrack.duration(), QTime::fromMSecsSinceStartOfDay(20));
        QCOMPARE(secondTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$20")));
        QCOMPARE(secondTrack.rating(), 5);
        QCOMPARE(secondTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(secondTrack.composer(), QStringLiteral("composer1"));
        QCOMPARE(secondTrack.lyricist(), QStringLiteral("lyricist1"));
        QCOMPARE(secondTrack.isSingleDiscAlbum(), true);
        QCOMPARE(secondTrack.hasEmbeddedCover(), false);

        auto albumId = musicDb.albumIdFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist4"), QStringLiteral("/"));
        auto album = musicDb.albumDataFromDatabaseId(albumId);
        auto albumData = musicDb.albumData(albumId);

        QCOMPARE(album.isValid(), true);
        QCOMPARE(albumData.count(), 2);
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
        QSignalSpy musicDbErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDbErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDbErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDbErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDbErrorSpy.count(), 0);

        auto firstAlbum = musicDb.albumDataFromDatabaseId(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/")));

        QCOMPARE(firstAlbum.isValid(), true);
        QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));
        QCOMPARE(musicDbErrorSpy.count(), 0);

        auto firstAlbumInvalid = musicDb.albumDataFromDatabaseId(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1Invalid"), QStringLiteral("Invalid Artist"), QStringLiteral("/")));

        QCOMPARE(firstAlbumInvalid.isValid(), false);
        QCOMPARE(musicDbErrorSpy.count(), 0);
    }

    void addTwiceSameTracksWithDatabaseFile()
    {
        QTemporaryFile myTempDatabase;
        myTempDatabase.open();

        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

            musicDb.init(QStringLiteral("testDb1"), myTempDatabase.fileName());

            musicDb.insertTracksList(mNewTracks, mNewCovers);

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbumsData().count(), 5);

            auto firstAlbum = musicDb.albumDataFromDatabaseId(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/")));

            QCOMPARE(firstAlbum.isValid(), true);
            QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

            auto firstAlbumInvalid = musicDb.albumDataFromDatabaseId(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1Invalid"), QStringLiteral("Invalid Artist"), QStringLiteral("/")));

            QCOMPARE(firstAlbumInvalid.isValid(), false);
        }

        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

            musicDb.init(QStringLiteral("testDb2"), myTempDatabase.fileName());

            musicDb.insertTracksList(mNewTracks, mNewCovers);

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbumsData().count(), 5);

            auto firstAlbum = musicDb.albumDataFromDatabaseId(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/")));

            QCOMPARE(firstAlbum.isValid(), true);
            QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

            auto firstAlbumInvalid = musicDb.albumDataFromDatabaseId(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1Invalid"), QStringLiteral("Invalid Artist"), QStringLiteral("/")));

            QCOMPARE(firstAlbumInvalid.isValid(), false);
        }
    }

    void restoreModifiedTracksWidthDatabaseFile()
    {
        QTemporaryFile myTempDatabase;
        myTempDatabase.open();

        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
            QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
            QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
            QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
            QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
            QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
            QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
            QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

            QCOMPARE(musicDb.allAlbumsData().count(), 0);
            QCOMPARE(musicDb.allArtistsData().count(), 0);
            QCOMPARE(musicDb.allTracksData().count(), 0);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);
            QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
            QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
            QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
            QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
            QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
            QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

            auto allNewTracks = mNewTracks;
            allNewTracks.push_back({true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                                    QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                                    6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                                    QDateTime::fromMSecsSinceEpoch(23),
                                    {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                                    QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false});

            musicDb.init(QStringLiteral("testDb1"), myTempDatabase.fileName());

            QCOMPARE(musicDb.allAlbumsData().count(), 0);
            QCOMPARE(musicDb.allArtistsData().count(), 0);
            QCOMPARE(musicDb.allTracksData().count(), 0);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);
            QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
            QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
            QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
            QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
            QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
            QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

            qDebug() << "restoreModifiedTracksWidthDatabaseFile" << myTempDatabase.fileName();

            auto newCovers = mNewCovers;
            newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

            musicDb.insertTracksList(allNewTracks, newCovers);

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbumsData().count(), 5);
            QCOMPARE(musicDb.allArtistsData().count(), 7);
            QCOMPARE(musicDb.allTracksData().count(), 23);
            QCOMPARE(musicDbArtistAddedSpy.count(), 1);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
            QCOMPARE(musicDbTrackAddedSpy.count(), 1);
            QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
            QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
            QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
            QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
            QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
            QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

            auto firstAlbum = musicDb.albumDataFromDatabaseId(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/")));

            QCOMPARE(firstAlbum.isValid(), true);
            QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

            auto firstAlbumInvalid = musicDb.albumDataFromDatabaseId(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1Invalid"), QStringLiteral("Invalid Artist"), QStringLiteral("/")));

            QCOMPARE(firstAlbumInvalid.isValid(), false);

            auto fourthAlbumId = musicDb.albumIdFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist2"), QStringLiteral("/"));
            auto fourthAlbum = musicDb.albumDataFromDatabaseId(fourthAlbumId);
            auto fourthAlbumData = musicDb.albumData(fourthAlbumId);

            QCOMPARE(fourthAlbum.isValid(), true);
            QCOMPARE(fourthAlbum.title(), QStringLiteral("album3"));

            QCOMPARE(fourthAlbumData.count(), 4);

            const auto &oneTrack = fourthAlbumData.at(3);

            QCOMPARE(oneTrack.isValid(), true);
            QCOMPARE(oneTrack.rating(), 5);
        }

        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
            QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
            QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
            QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
            QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
            QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
            QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
            QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

            QCOMPARE(musicDb.allAlbumsData().count(), 0);
            QCOMPARE(musicDb.allArtistsData().count(), 0);
            QCOMPARE(musicDb.allTracksData().count(), 0);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);
            QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
            QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
            QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
            QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
            QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
            QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

            auto allNewTracks = mNewTracks;
            allNewTracks.push_back({true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                                    QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                                    6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                                    QDateTime::fromMSecsSinceEpoch(23),
                                    {QUrl::fromLocalFile(QStringLiteral("album3"))}, 3, true,
                                    QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false});

            musicDb.init(QStringLiteral("testDb2"), myTempDatabase.fileName());

            qDebug() << "restoreModifiedTracksWidthDatabaseFile" << myTempDatabase.fileName();

            QCOMPARE(musicDb.allAlbumsData().count(), 5);
            QCOMPARE(musicDb.allArtistsData().count(), 7);
            QCOMPARE(musicDb.allTracksData().count(), 23);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);
            QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
            QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
            QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
            QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
            QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
            QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

            auto newCovers = mNewCovers;
            newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

            musicDb.insertTracksList(allNewTracks, newCovers);

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbumsData().count(), 5);
            QCOMPARE(musicDb.allArtistsData().count(), 7);
            QCOMPARE(musicDb.allTracksData().count(), 23);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);
            QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
            QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
            QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
            QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
            QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
            QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

            QCOMPARE(musicDb.allAlbumsData().count(), 5);

            auto firstAlbum = musicDb.albumDataFromDatabaseId(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Various Artists"), QStringLiteral("/")));

            QCOMPARE(firstAlbum.isValid(), true);
            QCOMPARE(firstAlbum.title(), QStringLiteral("album1"));

            auto firstAlbumInvalid = musicDb.albumDataFromDatabaseId(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1Invalid"), QStringLiteral("Invalid Artist"), QStringLiteral("/")));

            QCOMPARE(firstAlbumInvalid.isValid(), false);

            auto fourthAlbumId = musicDb.albumIdFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist2"), QStringLiteral("/"));
            auto fourthAlbum = musicDb.albumDataFromDatabaseId(fourthAlbumId);
            auto fourthAlbumData = musicDb.albumData(fourthAlbumId);

            QCOMPARE(fourthAlbum.isValid(), true);
            QCOMPARE(fourthAlbum.title(), QStringLiteral("album3"));

            QCOMPARE(fourthAlbumData.count(), 4);

            const auto &oneTrack = fourthAlbumData.at(3);

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

        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);

        auto allAlbums = musicDb.allAlbumsData();

        auto firstAlbumData = musicDb.albumData(allAlbums[0].databaseId());
        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = firstAlbumData.count();
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

        auto firstTrack = musicDb.trackDataFromDatabaseId(trackId);

        auto firstTrackTitle = firstTrack.title();
        auto firstTrackArtist = firstTrack.artist();
        auto firstTrackAlbumArtist = firstTrack.albumArtist();
        auto firstTrackAlbum = firstTrack.album();
        auto firstTrackImage = firstTrack.albumCover();
        auto firstTrackDuration = firstTrack.duration();
        auto firstTrackMilliSecondsDuration = firstTrack.duration().msecsSinceStartOfDay();
        auto firstTrackTrackNumber = firstTrack.trackNumber();
        auto firstTrackDiscNumber = firstTrack.discNumber();
        const auto &firstTrackResource = firstTrack.resourceURI();
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

        auto secondAlbumData = musicDb.albumData(allAlbums[1].databaseId());
        auto secondAlbumTitle = allAlbums[1].title();
        auto secondAlbumArtist = allAlbums[1].artist();
        auto secondAlbumImage = allAlbums[1].albumArtURI();
        auto secondAlbumTracksCount = secondAlbumData.count();
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

        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);
        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);

        auto allAlbums = musicDb.allAlbumsData();

        auto firstAlbumData = musicDb.albumData(allAlbums[0].databaseId());
        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = firstAlbumData.count();
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
        auto firstTrack = musicDb.trackDataFromDatabaseId(firstTrackId);

        auto firstTrackTitle = firstTrack.title();
        auto firstTrackArtist = firstTrack.artist();
        auto firstTrackAlbumArtist = firstTrack.albumArtist();
        auto firstTrackAlbum = firstTrack.album();
        auto firstTrackImage = firstTrack.albumCover();
        auto firstTrackDuration = firstTrack.duration();
        auto firstTrackMilliSecondsDuration = firstTrack.duration().msecsSinceStartOfDay();
        auto firstTrackTrackNumber = firstTrack.trackNumber();
        auto firstTrackDiscNumber = firstTrack.discNumber();
        const auto &firstTrackResource = firstTrack.resourceURI();
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
        auto secondTrack = musicDb.trackDataFromDatabaseId(secondTrackId);

        auto secondTrackTitle = secondTrack.title();
        auto secondTrackArtist = secondTrack.artist();
        auto secondTrackAlbumArtist = secondTrack.albumArtist();
        auto secondTrackAlbum = secondTrack.album();
        auto seconfTrackImage = secondTrack.albumCover();
        auto secondTrackDuration = secondTrack.duration();
        auto secondTrackMilliSecondsDuration = secondTrack.duration().msecsSinceStartOfDay();
        auto secondTrackTrackNumber = secondTrack.trackNumber();
        auto secondTrackDiscNumber = secondTrack.discNumber();
        const auto &secondTrackResource = secondTrack.resourceURI();
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
        auto thirdTrack = musicDb.trackDataFromDatabaseId(thirdTrackId);

        auto thirdTrackTitle = thirdTrack.title();
        auto thirdTrackArtist = thirdTrack.artist();
        auto thirdTrackAlbumArtist = thirdTrack.albumArtist();
        auto thirdTrackAlbum = thirdTrack.album();
        auto thirdTrackImage = thirdTrack.albumCover();
        auto thirdTrackDuration = thirdTrack.duration();
        auto thirdTrackMilliSecondsDuration = thirdTrack.duration().msecsSinceStartOfDay();
        auto thirdTrackTrackNumber = thirdTrack.trackNumber();
        auto thirdTrackDiscNumber = thirdTrack.discNumber();
        const auto &thirdTrackResource = thirdTrack.resourceURI();
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
        auto fourthTrack = musicDb.trackDataFromDatabaseId(fourthTrackId);

        auto fourthTrackTitle = fourthTrack.title();
        auto fourthTrackArtist = fourthTrack.artist();
        auto fourthTrackAlbumArtist = fourthTrack.albumArtist();
        auto fourthTrackAlbum = fourthTrack.album();
        auto fourthTrackImage = fourthTrack.albumCover();
        auto fourthTrackDuration = fourthTrack.duration();
        auto fourthTrackMilliSecondsDuration = fourthTrack.duration().msecsSinceStartOfDay();
        auto fourthTrackTrackNumber = fourthTrack.trackNumber();
        auto fourthTrackDiscNumber = fourthTrack.discNumber();
        const auto &fourthTrackResource = fourthTrack.resourceURI();
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

        auto secondAlbumData = musicDb.albumData(allAlbums[1].databaseId());
        auto secondAlbumTitle = allAlbums[1].title();
        auto secondAlbumArtist = allAlbums[1].artist();
        auto secondAlbumImage = allAlbums[1].albumArtURI();
        auto secondAlbumTracksCount = secondAlbumData.count();
        auto secondAlbumIsSingleDiscAlbum = allAlbums[1].isSingleDiscAlbum();

        QCOMPARE(secondAlbumTitle, QStringLiteral("album2"));
        QCOMPARE(secondAlbumArtist, QStringLiteral("artist1"));
        QCOMPARE(secondAlbumImage.isValid(), true);
        QCOMPARE(secondAlbumImage, QUrl::fromLocalFile(QStringLiteral("album2")));
        QCOMPARE(secondAlbumTracksCount, 6);
        QCOMPARE(secondAlbumIsSingleDiscAlbum, true);
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

            QCOMPARE(musicDb.allAlbumsData().count(), 0);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);

            musicDb.init(QStringLiteral("testDbVariousArtistAlbum1"), myDatabaseFile.fileName());

            QCOMPARE(musicDb.allAlbumsData().count(), 0);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);

            musicDb.insertTracksList(mNewTracks, mNewCovers);

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbumsData().count(), 5);
            QCOMPARE(musicDb.allArtistsData().count(), 7);
            QCOMPARE(musicDb.allTracksData().count(), 22);
            QCOMPARE(musicDbArtistAddedSpy.count(), 1);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
            QCOMPARE(musicDbTrackAddedSpy.count(), 1);

            auto allAlbums = musicDb.allAlbumsData();

            auto firstAlbumData = musicDb.albumData(allAlbums[0].databaseId());
            auto firstAlbumTitle = allAlbums[0].title();
            auto firstAlbumArtist = allAlbums[0].artist();
            auto firstAlbumImage = allAlbums[0].albumArtURI();
            auto firstAlbumTracksCount = firstAlbumData.count();
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
            auto firstTrack = musicDb.trackDataFromDatabaseId(firstTrackId);

            auto firstTrackTitle = firstTrack.title();
            auto firstTrackArtist = firstTrack.artist();
            auto firstTrackAlbumArtist = firstTrack.albumArtist();
            auto firstTrackAlbum = firstTrack.album();
            auto firstTrackImage = firstTrack.albumCover();
            auto firstTrackDuration = firstTrack.duration();
            auto firstTrackMilliSecondsDuration = firstTrack.duration().msecsSinceStartOfDay();
            auto firstTrackTrackNumber = firstTrack.trackNumber();
            auto firstTrackDiscNumber = firstTrack.discNumber();
            const auto &firstTrackResource = firstTrack.resourceURI();
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
            auto secondTrack = musicDb.trackDataFromDatabaseId(secondTrackId);

            auto secondTrackTitle = secondTrack.title();
            auto secondTrackArtist = secondTrack.artist();
            auto secondTrackAlbumArtist = secondTrack.albumArtist();
            auto secondTrackAlbum = secondTrack.album();
            auto seconfTrackImage = secondTrack.albumCover();
            auto secondTrackDuration = secondTrack.duration();
            auto secondTrackMilliSecondsDuration = secondTrack.duration().msecsSinceStartOfDay();
            auto secondTrackTrackNumber = secondTrack.trackNumber();
            auto secondTrackDiscNumber = secondTrack.discNumber();
            const auto &secondTrackResource = secondTrack.resourceURI();
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
            auto thirdTrack = musicDb.trackDataFromDatabaseId(thirdTrackId);

            auto thirdTrackTitle = thirdTrack.title();
            auto thirdTrackArtist = thirdTrack.artist();
            auto thirdTrackAlbumArtist = thirdTrack.albumArtist();
            auto thirdTrackAlbum = thirdTrack.album();
            auto thirdTrackImage = thirdTrack.albumCover();
            auto thirdTrackDuration = thirdTrack.duration();
            auto thirdTrackMilliSecondsDuration = thirdTrack.duration().msecsSinceStartOfDay();
            auto thirdTrackTrackNumber = thirdTrack.trackNumber();
            auto thirdTrackDiscNumber = thirdTrack.discNumber();
            const auto &thirdTrackResource = thirdTrack.resourceURI();
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
            auto fourthTrack = musicDb.trackDataFromDatabaseId(fourthTrackId);

            auto fourthTrackTitle = fourthTrack.title();
            auto fourthTrackArtist = fourthTrack.artist();
            auto fourthTrackAlbumArtist = fourthTrack.albumArtist();
            auto fourthTrackAlbum = fourthTrack.album();
            auto fourthTrackImage = fourthTrack.albumCover();
            auto fourthTrackDuration = fourthTrack.duration();
            auto fourthTrackMilliSecondsDuration = fourthTrack.duration().msecsSinceStartOfDay();
            auto fourthTrackTrackNumber = fourthTrack.trackNumber();
            auto fourthTrackDiscNumber = fourthTrack.discNumber();
            const auto &fourthTrackResource = fourthTrack.resourceURI();
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

            auto secondAlbumData = musicDb.albumData(allAlbums[1].databaseId());
            auto secondAlbumTitle = allAlbums[1].title();
            auto secondAlbumArtist = allAlbums[1].artist();
            auto secondAlbumImage = allAlbums[1].albumArtURI();
            auto secondAlbumTracksCount = secondAlbumData.count();
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

            QCOMPARE(musicDb.allAlbumsData().count(), 0);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);

            musicDb.init(QStringLiteral("testDbVariousArtistAlbum2"), myDatabaseFile.fileName());

            QCOMPARE(musicDb.allAlbumsData().count(), 5);
            QCOMPARE(musicDb.allArtistsData().count(), 7);
            QCOMPARE(musicDb.allTracksData().count(), 22);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);

            musicDb.insertTracksList(mNewTracks, mNewCovers);

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbumsData().count(), 5);
            QCOMPARE(musicDb.allArtistsData().count(), 7);
            QCOMPARE(musicDb.allTracksData().count(), 22);
            QCOMPARE(musicDbArtistAddedSpy.count(), 0);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 0);

            auto allAlbums = musicDb.allAlbumsData();

            auto firstAlbumData = musicDb.albumData(allAlbums[0].databaseId());
            auto firstAlbumTitle = allAlbums[0].title();
            auto firstAlbumArtist = allAlbums[0].artist();
            auto firstAlbumImage = allAlbums[0].albumArtURI();
            auto firstAlbumTracksCount = firstAlbumData.count();
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
            auto firstTrack = musicDb.trackDataFromDatabaseId(firstTrackId);

            auto firstTrackTitle = firstTrack.title();
            auto firstTrackArtist = firstTrack.artist();
            auto firstTrackAlbumArtist = firstTrack.albumArtist();
            auto firstTrackAlbum = firstTrack.album();
            auto firstTrackImage = firstTrack.albumCover();
            auto firstTrackDuration = firstTrack.duration();
            auto firstTrackMilliSecondsDuration = firstTrack.duration().msecsSinceStartOfDay();
            auto firstTrackTrackNumber = firstTrack.trackNumber();
            auto firstTrackDiscNumber = firstTrack.discNumber();
            const auto &firstTrackResource = firstTrack.resourceURI();
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
            auto secondTrack = musicDb.trackDataFromDatabaseId(secondTrackId);

            auto secondTrackTitle = secondTrack.title();
            auto secondTrackArtist = secondTrack.artist();
            auto secondTrackAlbumArtist = secondTrack.albumArtist();
            auto secondTrackAlbum = secondTrack.album();
            auto seconfTrackImage = secondTrack.albumCover();
            auto secondTrackDuration = secondTrack.duration();
            auto secondTrackMilliSecondsDuration = secondTrack.duration().msecsSinceStartOfDay();
            auto secondTrackTrackNumber = secondTrack.trackNumber();
            auto secondTrackDiscNumber = secondTrack.discNumber();
            const auto &secondTrackResource = secondTrack.resourceURI();
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
            auto thirdTrack = musicDb.trackDataFromDatabaseId(thirdTrackId);

            auto thirdTrackTitle = thirdTrack.title();
            auto thirdTrackArtist = thirdTrack.artist();
            auto thirdTrackAlbumArtist = thirdTrack.albumArtist();
            auto thirdTrackAlbum = thirdTrack.album();
            auto thirdTrackImage = thirdTrack.albumCover();
            auto thirdTrackDuration = thirdTrack.duration();
            auto thirdTrackMilliSecondsDuration = thirdTrack.duration().msecsSinceStartOfDay();
            auto thirdTrackTrackNumber = thirdTrack.trackNumber();
            auto thirdTrackDiscNumber = thirdTrack.discNumber();
            const auto &thirdTrackResource = thirdTrack.resourceURI();
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
            auto fourthTrack = musicDb.trackDataFromDatabaseId(fourthTrackId);

            auto fourthTrackTitle = fourthTrack.title();
            auto fourthTrackArtist = fourthTrack.artist();
            auto fourthTrackAlbumArtist = fourthTrack.albumArtist();
            auto fourthTrackAlbum = fourthTrack.album();
            auto fourthTrackImage = fourthTrack.albumCover();
            auto fourthTrackDuration = fourthTrack.duration();
            auto fourthTrackMilliSecondsDuration = fourthTrack.duration().msecsSinceStartOfDay();
            auto fourthTrackTrackNumber = fourthTrack.trackNumber();
            auto fourthTrackDiscNumber = fourthTrack.discNumber();
            const auto &fourthTrackResource = fourthTrack.resourceURI();
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

            auto secondAlbumData = musicDb.albumData(allAlbums[1].databaseId());
            auto secondAlbumTitle = allAlbums[1].title();
            auto secondAlbumArtist = allAlbums[1].artist();
            auto secondAlbumImage = allAlbums[1].albumArtURI();
            auto secondAlbumTracksCount = secondAlbumData.count();
            auto secondAlbumIsSingleDiscAlbum = allAlbums[1].isSingleDiscAlbum();

            QCOMPARE(secondAlbumTitle, QStringLiteral("album2"));
            QCOMPARE(secondAlbumArtist, QStringLiteral("artist1"));
            QCOMPARE(secondAlbumImage.isValid(), true);
            QCOMPARE(secondAlbumImage, QUrl::fromLocalFile(QStringLiteral("album2")));
            QCOMPARE(secondAlbumTracksCount, 6);
            QCOMPARE(secondAlbumIsSingleDiscAlbum, true);

            const DataTypes::TrackDataType newTrack = {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                    QStringLiteral("artist8"), QStringLiteral("album3"), QStringLiteral("artist2"),
                    6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                    QDateTime::fromMSecsSinceEpoch(23),
            {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                    QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};

            auto newCovers = mNewCovers;
            newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("image$19"));

            musicDb.insertTracksList({newTrack}, newCovers);

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDb.allAlbumsData().count(), 5);
            QCOMPARE(musicDb.allArtistsData().count(), 8);
            QCOMPARE(musicDb.allTracksData().count(), 23);
            QCOMPARE(musicDbArtistAddedSpy.count(), 1);
            QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
            QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        }
    }

    void testTracksFromAuthor() {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDbVariousArtistAlbum"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);

        auto allTracks = musicDb.tracksDataFromAuthor(QStringLiteral("artist1"));

        QCOMPARE(allTracks.size(), 7);
        QCOMPARE(allTracks[0].albumArtist(), QStringLiteral("Various Artists"));
        QCOMPARE(allTracks[1].albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allTracks[2].albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allTracks[3].albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allTracks[4].albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allTracks[5].albumArtist(), QStringLiteral("artist1"));
        QCOMPARE(allTracks[6].albumArtist(), QStringLiteral("artist1"));
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbums = musicDb.allAlbumsData();

        auto firstAlbumData = musicDb.albumData(allAlbums[0].databaseId());
        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = firstAlbumData.count();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album1"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("Various Artists"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstAlbumTracksCount, 4);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, false);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                    QStringLiteral("album1"), 1, 1);

        QVERIFY2(trackId != 0, "trackId should be different from 0");

        auto firstTrack = musicDb.trackDataFromDatabaseId(trackId);

        QCOMPARE(firstTrack.isValid(), true);

        musicDb.removeTracksList({firstTrack.resourceURI()});

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 21);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbumsV2 = musicDb.allAlbumsData();
        const auto &firstAlbum = allAlbumsV2[0];

        QCOMPARE(musicDb.allAlbumsData().isEmpty(), false);
        QCOMPARE(firstAlbum.isValid(), true);

        auto firstAlbumDataV2 = musicDb.albumData(firstAlbum.databaseId());
        auto firstAlbumTitleV2 = firstAlbum.title();
        auto firstAlbumArtistV2 = firstAlbum.artist();
        auto firstAlbumImageV2 = firstAlbum.albumArtURI();
        auto firstAlbumTracksCountV2 = firstAlbumDataV2.count();
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbums = musicDb.allAlbumsData();

        auto firstAlbumData = musicDb.albumData(allAlbums[0].databaseId());
        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = firstAlbumData.count();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album1"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("Various Artists"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstAlbumTracksCount, 4);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, false);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                    QStringLiteral("album1"), 1, 1);

        QCOMPARE(trackId != 0, true);

        auto firstTrack = musicDb.trackDataFromDatabaseId(trackId);

        QCOMPARE(firstTrack.isValid(), true);

        musicDb.removeTracksList({firstTrack.resourceURI()});

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 21);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbumsV2 = musicDb.allAlbumsData();
        const auto &firstAlbum = allAlbumsV2[0];

        QCOMPARE(musicDb.allAlbumsData().isEmpty(), false);
        QCOMPARE(firstAlbum.isValid(), true);

        auto firstAlbumDataV2 = musicDb.albumData(firstAlbum.databaseId());
        auto firstAlbumTitleV2 = firstAlbum.title();
        auto firstAlbumArtistV2 = firstAlbum.artist();
        auto firstAlbumImageV2 = firstAlbum.albumArtURI();
        auto firstAlbumTracksCountV2 = firstAlbumDataV2.count();
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

        firstTrack[DataTypes::DatabaseIdRole] = 0;

        musicDb.insertTracksList({firstTrack}, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 2);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbums3 = musicDb.allAlbumsData();

        auto firstAlbumData3 = musicDb.albumData(allAlbums3[0].databaseId());
        auto firstAlbumTitle3 = allAlbums3[0].title();
        auto firstAlbumArtist3 = allAlbums3[0].artist();
        auto firstAlbumImage3 = allAlbums3[0].albumArtURI();
        auto firstAlbumTracksCount3 = firstAlbumData3.count();
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbums = musicDb.allAlbumsData();

        auto firstAlbumData = musicDb.albumData(allAlbums[0].databaseId());
        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = firstAlbumData.count();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album1"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("Various Artists"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstAlbumTracksCount, 4);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, false);

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                         QStringLiteral("album2"), 1, 1);
        auto firstTrack = musicDb.trackDataFromDatabaseId(firstTrackId);
        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track2"), QStringLiteral("artist1"),
                                                                          QStringLiteral("album2"), 2, 1);
        auto secondTrack = musicDb.trackDataFromDatabaseId(secondTrackId);
        auto thirdTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist1"),
                                                                         QStringLiteral("album2"), 3, 1);
        auto thirdTrack = musicDb.trackDataFromDatabaseId(thirdTrackId);
        auto fourthTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track4"), QStringLiteral("artist1"),
                                                                          QStringLiteral("album2"), 4, 1);
        auto fourthTrack = musicDb.trackDataFromDatabaseId(fourthTrackId);
        auto fithTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track5"), QStringLiteral("artist1"),
                                                                        QStringLiteral("album2"), 5, 1);
        auto fithTrack = musicDb.trackDataFromDatabaseId(fithTrackId);
        auto sixthTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist1 and artist2"),
                                                                         QStringLiteral("album2"), 6, 1);
        auto sixthTrack = musicDb.trackDataFromDatabaseId(sixthTrackId);

        musicDb.removeTracksList({firstTrack.resourceURI(), secondTrack.resourceURI(), thirdTrack.resourceURI(),
                                  fourthTrack.resourceURI(), fithTrack.resourceURI(), sixthTrack.resourceURI()});

        QCOMPARE(musicDb.allAlbumsData().count(), 4);
        QCOMPARE(musicDb.allArtistsData().count(), 6);
        QCOMPARE(musicDb.allTracksData().count(), 16);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 1);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 6);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto removedAlbum = musicDb.albumDataFromDatabaseId(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album2"), QStringLiteral("artist1"), QStringLiteral("/")));

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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbums = musicDb.allAlbumsData();

        auto firstAlbumData = musicDb.albumData(allAlbums[0].databaseId());
        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = firstAlbumData.count();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album1"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("Various Artists"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album1")));
        QCOMPARE(firstAlbumTracksCount, 4);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, false);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"),
                                                                    QStringLiteral("album1"), 3, 3);
        auto track = musicDb.trackDataFromDatabaseId(trackId);

        musicDb.removeTracksList({track.resourceURI()});

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 6);
        QCOMPARE(musicDb.allTracksData().count(), 21);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 1);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 23);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void addTwoTracksSameAlbumSameTitle()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addTwoTracksSameAlbumSameTitle" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 6, 1);
        auto firstTrack = musicDb.trackDataFromDatabaseId(firstTrackId);

        auto firstTrackTitle = firstTrack.title();
        auto firstTrackArtist = firstTrack.artist();
        auto firstTrackAlbumArtist = firstTrack.albumArtist();
        auto firstTrackAlbum = firstTrack.album();
        auto firstTrackImage = firstTrack.albumCover();
        auto firstTrackDuration = firstTrack.duration();
        auto firstTrackMilliSecondsDuration = firstTrack.duration().msecsSinceStartOfDay();
        auto firstTrackTrackNumber = firstTrack.trackNumber();
        auto firstTrackDiscNumber = firstTrack.discNumber();
        const auto &firstTrackResource = firstTrack.resourceURI();
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

        auto allAlbums = musicDb.allAlbumsData();

        auto firstAlbumData = musicDb.albumData(allAlbums[0].databaseId());
        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = firstAlbumData.count();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album3"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("artist2"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstAlbumTracksCount, 1);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, true);

        auto newTrack2 = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist9"), QStringLiteral("album3"), QStringLiteral("artist2"), 6, 1,
                QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("image$19")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks2 = DataTypes::ListTrackDataType();
        newTracks2.push_back(newTrack2);

        auto newCovers2 = mNewCovers;
        newCovers2[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("image$19"));

        musicDb.insertTracksList(newTracks2, newCovers2);

        musicDbTrackAddedSpy.wait(50);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 2);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist9"),
                                                                          QStringLiteral("album3"), 6, 1);
        auto secondTrack = musicDb.trackDataFromDatabaseId(secondTrackId);

        auto secondTrackTitle = secondTrack.title();
        auto secondTrackArtist = secondTrack.artist();
        auto secondTrackAlbumArtist = secondTrack.albumArtist();
        auto secondTrackAlbum = secondTrack.album();
        auto secondTrackImage = secondTrack.albumCover();
        auto secondTrackDuration = secondTrack.duration();
        auto secondTrackMilliSecondsDuration = secondTrack.duration().msecsSinceStartOfDay();
        auto secondTrackTrackNumber = secondTrack.trackNumber();
        auto secondTrackDiscNumber = secondTrack.discNumber();
        const auto &secondTrackResource = secondTrack.resourceURI();
        auto secondTrackRating = secondTrack.rating();
        auto secondIsSingleDiscAlbum = secondTrack.isSingleDiscAlbum();

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrackTitle, QStringLiteral("track6"));
        QCOMPARE(secondTrackArtist, QStringLiteral("artist9"));
        QCOMPARE(secondTrackAlbumArtist, QStringLiteral("artist2"));
        QCOMPARE(secondTrackAlbum, QStringLiteral("album3"));
        QCOMPARE(secondTrackImage.isValid(), true);
        QCOMPARE(secondTrackImage, QUrl::fromLocalFile(QStringLiteral("image$19")));
        QCOMPARE(secondTrackDuration, QTime::fromMSecsSinceStartOfDay(23));
        QCOMPARE(secondTrackMilliSecondsDuration, 23);
        QCOMPARE(secondTrackTrackNumber, 6);
        QCOMPARE(secondTrackDiscNumber, 1);
        QCOMPARE(secondTrackResource.isValid(), true);
        QCOMPARE(secondTrackResource, QUrl::fromLocalFile(QStringLiteral("/$23")));
        QCOMPARE(secondTrackRating, 5);
        QCOMPARE(secondIsSingleDiscAlbum, true);

        auto allAlbums2 = musicDb.allAlbumsData();

        auto firstAlbumData2 = musicDb.albumData(allAlbums2[0].databaseId());
        auto firstAlbumTitle2 = allAlbums2[0].title();
        auto firstAlbumArtist2 = allAlbums2[0].artist();
        auto firstAlbumImage2 = allAlbums2[0].albumArtURI();
        auto firstAlbumTracksCount2 = firstAlbumData2.count();
        auto firstAlbumIsSingleDiscAlbum2 = allAlbums2[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle2, QStringLiteral("album3"));
        QCOMPARE(firstAlbumArtist2, QStringLiteral("artist2"));
        QCOMPARE(firstAlbumImage2.isValid(), true);
        QCOMPARE(firstAlbumImage2, QUrl::fromLocalFile(QStringLiteral("image$19")));
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 6, 1);
        auto firstTrack = musicDb.trackDataFromDatabaseId(firstTrackId);

        auto firstTrackTitle = firstTrack.title();
        auto firstTrackArtist = firstTrack.artist();
        auto firstTrackAlbumArtist = firstTrack.albumArtist();
        auto firstTrackAlbum = firstTrack.album();
        auto firstTrackImage = firstTrack.albumCover();
        auto firstTrackDuration = firstTrack.duration();
        auto firstTrackMilliSecondsDuration = firstTrack.duration().msecsSinceStartOfDay();
        auto firstTrackTrackNumber = firstTrack.trackNumber();
        auto firstTrackDiscNumber = firstTrack.discNumber();
        const auto &firstTrackResource = firstTrack.resourceURI();
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

        auto allAlbums = musicDb.allAlbumsData();

        QCOMPARE(allAlbums.size(), 1);

        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = musicDb.albumData(allAlbums[0].databaseId()).count();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album3"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("artist2"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstAlbumTracksCount, 1);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, true);

        auto newTrack2 = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist9"), QStringLiteral("album3"), QStringLiteral("artist9"), 6, 1,
                QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks2 = DataTypes::ListTrackDataType();
        newTracks2.push_back(newTrack2);

        auto newCovers2 = mNewCovers;
        newCovers2[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks2, newCovers2);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 2);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 2);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 1);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist9"),
                                                                          QStringLiteral("album3"), 6, 1);
        auto secondTrack = musicDb.trackDataFromDatabaseId(secondTrackId);

        auto secondTrackTitle = secondTrack.title();
        auto secondTrackArtist = secondTrack.artist();
        auto secondTrackAlbumArtist = secondTrack.albumArtist();
        auto secondTrackAlbum = secondTrack.album();
        auto secondTrackImage = secondTrack.albumCover();
        auto secondTrackDuration = secondTrack.duration();
        auto secondTrackMilliSecondsDuration = secondTrack.duration().msecsSinceStartOfDay();
        auto secondTrackTrackNumber = secondTrack.trackNumber();
        auto secondTrackDiscNumber = secondTrack.discNumber();
        const auto &secondTrackResource = secondTrack.resourceURI();
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

        auto allAlbums2 = musicDb.allAlbumsData();

        auto secondAlbumTitle = allAlbums2[0].title();
        auto secondAlbumArtist = allAlbums2[0].artist();
        auto secondAlbumImage = allAlbums2[0].albumArtURI();
        auto secondAlbumTracksCount = musicDb.albumData(allAlbums2[0].databaseId()).count();
        auto secondAlbumIsSingleDiscAlbum = allAlbums2[0].isSingleDiscAlbum();

        QCOMPARE(secondAlbumTitle, QStringLiteral("album3"));
        QCOMPARE(secondAlbumArtist, QStringLiteral("artist9"));
        QCOMPARE(secondAlbumImage.isValid(), true);
        QCOMPARE(secondAlbumImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondAlbumTracksCount, 1);
        QCOMPARE(secondAlbumIsSingleDiscAlbum, true);
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 6, 1);
        auto firstTrack = musicDb.trackDataFromDatabaseId(firstTrackId);

        auto firstTrackTitle = firstTrack.title();
        auto firstTrackArtist = firstTrack.artist();
        auto firstTrackAlbumArtist = firstTrack.albumArtist();
        auto firstTrackAlbum = firstTrack.album();
        auto firstTrackImage = firstTrack.albumCover();
        auto firstTrackDuration = firstTrack.duration();
        auto firstTrackMilliSecondsDuration = firstTrack.duration().msecsSinceStartOfDay();
        auto firstTrackTrackNumber = firstTrack.trackNumber();
        auto firstTrackDiscNumber = firstTrack.discNumber();
        const auto &firstTrackResource = firstTrack.resourceURI();
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

        auto allAlbums = musicDb.allAlbumsData();

        QCOMPARE(allAlbums.size(), 1);

        auto firstAlbumData = musicDb.albumData(allAlbums[0].databaseId());
        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = firstAlbumData.count();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album3"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("artist2"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstAlbumTracksCount, 1);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, true);

        auto newTrack2 = DataTypes::TrackDataType{true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist9"), QStringLiteral("album3"), QStringLiteral("artist9"), 6, 1, QTime::fromMSecsSinceStartOfDay(20),
        {QUrl::fromLocalFile(QStringLiteral("/$20"))},
                QDateTime::fromMSecsSinceEpoch(20),
        {QUrl::fromLocalFile(QStringLiteral("file://image$20"))}, 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks2 = DataTypes::ListTrackDataType();
        newTracks2.push_back(newTrack2);

        auto newCovers2 = mNewCovers;
        newCovers2[QStringLiteral("file:///$20")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks2, newCovers2);

        musicDbTrackAddedSpy.wait(50);

        QCOMPARE(musicDb.allAlbumsData().count(), 2);
        QCOMPARE(musicDb.allArtistsData().count(), 2);
        QCOMPARE(musicDb.allTracksData().count(), 2);
        QCOMPARE(musicDbArtistAddedSpy.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 2);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist9"),
                                                                          QStringLiteral("album3"), 6, 1);
        auto secondTrack = musicDb.trackDataFromDatabaseId(secondTrackId);

        auto secondTrackTitle = secondTrack.title();
        auto secondTrackArtist = secondTrack.artist();
        auto secondTrackAlbumArtist = secondTrack.albumArtist();
        auto secondTrackAlbum = secondTrack.album();
        auto secondTrackImage = secondTrack.albumCover();
        auto secondTrackDuration = secondTrack.duration();
        auto secondTrackMilliSecondsDuration = secondTrack.duration().msecsSinceStartOfDay();
        auto secondTrackTrackNumber = secondTrack.trackNumber();
        auto secondTrackDiscNumber = secondTrack.discNumber();
        const auto &secondTrackResource = secondTrack.resourceURI();
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

        auto allAlbums2 = musicDb.allAlbumsData();

        auto firstAlbumData2 = musicDb.albumData(allAlbums2[0].databaseId());
        auto firstAlbumTitle2 = allAlbums2[0].title();
        auto firstAlbumArtist2 = allAlbums2[0].artist();
        auto firstAlbumImage2 = allAlbums2[0].albumArtURI();
        auto firstAlbumTracksCount2 = firstAlbumData2.count();
        auto firstAlbumIsSingleDiscAlbum2 = allAlbums2[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle2, QStringLiteral("album3"));
        QCOMPARE(firstAlbumArtist2, QStringLiteral("artist2"));
        QCOMPARE(firstAlbumImage2.isValid(), true);
        QCOMPARE(firstAlbumImage2, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstAlbumTracksCount2, 1);
        QCOMPARE(firstAlbumIsSingleDiscAlbum2, true);

        auto secondAlbumData = musicDb.albumData(allAlbums2[1].databaseId());
        auto secondAlbumTitle = allAlbums2[1].title();
        auto secondAlbumArtist = allAlbums2[1].artist();
        auto secondAlbumImage = allAlbums2[1].albumArtURI();
        auto secondAlbumTracksCount = secondAlbumData.count();
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album7"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl();

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 6);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 23);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 2);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                    QStringLiteral("album7"), 6, 1);
        const auto &firstTrack = musicDb.trackDataFromDatabaseId(trackId);

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrack.albumCover(), QUrl());

        auto newTrack2 = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track7"),
                QStringLiteral("artist2"), QStringLiteral("album7"), QStringLiteral("artist2"), 7, 1,
                QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks2 = DataTypes::ListTrackDataType();
        newTracks2.push_back(newTrack2);

        auto newCovers2 = mNewCovers;
        newCovers2[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album7"));

        musicDb.insertTracksList(newTracks2, newCovers2);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 6);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 23);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 2);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track7"), QStringLiteral("artist2"),
                                                                          QStringLiteral("album7"), 7, 1);
        const auto &secondTrack = musicDb.trackDataFromDatabaseId(secondTrackId);

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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto modifiedTrack = DataTypes::TrackDataType{true,
                                                      QStringLiteral("$3"),
                                                      QStringLiteral("0"),
                                                      QStringLiteral("track3"),
                                                      QStringLiteral("artist3"),
                                                      QStringLiteral("album1"),
                                                      QStringLiteral("Various Artists"),
                                                      5,
                                                      3,
                                                      QTime::fromMSecsSinceStartOfDay(3),
                                                      {QUrl::fromLocalFile(QStringLiteral("/$3"))},
                                                      QDateTime::fromMSecsSinceEpoch(23),
                                                      {QUrl::fromLocalFile(QStringLiteral("file://image$3"))},
                                                      5,
                                                      true,
                                                      QStringLiteral("genre1"),
                                                      QStringLiteral("composer1"),
                                                      QStringLiteral("lyricist2"),
                                                      false};

        musicDb.insertTracksList({modifiedTrack}, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto trackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track3"), QStringLiteral("artist3"),
                                                                    QStringLiteral("album1"), 5, 3);
        QCOMPARE(trackId != 0, true);
        auto track = musicDb.trackDataFromDatabaseId(trackId);

        QCOMPARE(track.isValid(), true);
        QCOMPARE(track.trackNumber(), 5);
        QCOMPARE(track.lyricist(), u"lyricist2"_s);
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album7"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCover = QUrl::fromLocalFile(QStringLiteral("file://image$19"));
        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = newCover;

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 6);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 23);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 2);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        const auto &newAlbum = musicDb.albumDataFromDatabaseId(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album7"), QStringLiteral("artist2"), QStringLiteral("/")));
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        const DataTypes::TrackDataType newTrack = {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album1")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album1"));

        musicDb.insertTracksList({newTrack}, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 8);
        QCOMPARE(musicDb.allTracksData().count(), 23);
        QCOMPARE(musicDbArtistAddedSpy.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void reloadDatabaseWithAllTracks()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "reloadDatabaseWithAllTracks" << databaseFile.fileName();

        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
            QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

            musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

            musicDb.insertTracksList(mNewTracks, mNewCovers);

            musicDbTrackAddedSpy.wait(300);

            QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
        }

        DatabaseInterface musicDb2;

        QSignalSpy musicDbArtistAddedSpy2(&musicDb2, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy2(&musicDb2, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy2(&musicDb2, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy2(&musicDb2, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy2(&musicDb2, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy2(&musicDb2, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy2(&musicDb2, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy2(&musicDb2, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy2(&musicDb2, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb2.allAlbumsData().count(), 0);
        QCOMPARE(musicDb2.allArtistsData().count(), 0);
        QCOMPARE(musicDb2.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy2.count(), 0);

        musicDb2.init(QStringLiteral("testDb2"), databaseFile.fileName());

        musicDbTrackAddedSpy2.wait(300);

        QCOMPARE(musicDb2.allAlbumsData().count(), 5);
        QCOMPARE(musicDb2.allArtistsData().count(), 7);
        QCOMPARE(musicDb2.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy2.count(), 0);

        musicDb2.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy2.wait(300);

        QCOMPARE(musicDb2.allAlbumsData().count(), 5);
        QCOMPARE(musicDb2.allArtistsData().count(), 7);
        QCOMPARE(musicDb2.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy2.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy2.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy2.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy2.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy2.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy2.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy2.count(), 0);
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        DataTypes::ListTrackDataType newTracks = {
            {true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track1"),
             QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("artist6"),
             1, 1, QTime::fromMSecsSinceStartOfDay(1), {QUrl::fromLocalFile(QStringLiteral("/$20"))},
             QDateTime::fromMSecsSinceEpoch(20),
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 1, false,
             QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
            {true, QStringLiteral("$21"), QStringLiteral("0"), QStringLiteral("track2"),
             QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("artist6"),
             2, 1, QTime::fromMSecsSinceStartOfDay(2), {QUrl::fromLocalFile(QStringLiteral("/$21"))},
             QDateTime::fromMSecsSinceEpoch(21),
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 2, false,
             QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
            {true, QStringLiteral("$22"), QStringLiteral("0"), QStringLiteral("track3"),
             QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("artist6"),
             3, 1, QTime::fromMSecsSinceStartOfDay(3), {QUrl::fromLocalFile(QStringLiteral("/$22"))},
             QDateTime::fromMSecsSinceEpoch(22),
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 3, false,
             QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
            {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track4"),
             QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("artist6"),
             4, 1, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
             QDateTime::fromMSecsSinceEpoch(23),
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 4, false,
             QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
            {true, QStringLiteral("$24"), QStringLiteral("0"), QStringLiteral("track5"),
             QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("artist6"),
             5, 1, QTime::fromMSecsSinceStartOfDay(4), {QUrl::fromLocalFile(QStringLiteral("/$24"))},
             QDateTime::fromMSecsSinceEpoch(24),
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 5, false,
             QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
            {true, QStringLiteral("$25"), QStringLiteral("0"), QStringLiteral("track6"),
             QStringLiteral("artist6"), QStringLiteral("album1"), QStringLiteral("artist6"),
             6, 1, QTime::fromMSecsSinceStartOfDay(5), {QUrl::fromLocalFile(QStringLiteral("/$25"))},
             QDateTime::fromMSecsSinceEpoch(25),
             {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 6, true,
             QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
            {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
             QStringLiteral("artist2"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
             2, 2, QTime::fromMSecsSinceStartOfDay(2), {QUrl::fromLocalFile(QStringLiteral("/$26"))},
             QDateTime::fromMSecsSinceEpoch(26),
             {QUrl::fromLocalFile(QStringLiteral("file://image$2"))}, 2, false,
             QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
        };

        musicDb.insertTracksList(newTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(100);

        QCOMPARE(musicDb.allAlbumsData().count(), 2);
        QCOMPARE(musicDb.allArtistsData().count(), 3);
        QCOMPARE(musicDb.allTracksData().count(), 7);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstAlbum = musicDb.albumDataFromDatabaseId(musicDb.albumIdFromTitleAndArtist(QStringLiteral("album1"), QStringLiteral("Invalid Artist"), QStringLiteral("/")));
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType();

        newTracks = {{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                      QStringLiteral("artist2"), QStringLiteral("album3"), {},
                      6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                      QDateTime::fromMSecsSinceEpoch(23),
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                      QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), true},};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrack = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                                                       QStringLiteral("album3"), 6, 1));

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrack.title(), QStringLiteral("track6"));
        QCOMPARE(firstTrack.artist(), QStringLiteral("artist2"));
        QCOMPARE(firstTrack.album(), QStringLiteral("album3"));
        QVERIFY(!firstTrack.albumArtist().isEmpty());
        QCOMPARE(firstTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstTrack.trackNumber(), 6);
        QCOMPARE(firstTrack.discNumber(), 1);
        QCOMPARE(firstTrack.duration(), QTime::fromMSecsSinceStartOfDay(23));
        QCOMPARE(firstTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$23")));
        QCOMPARE(firstTrack.rating(), 5);
        QCOMPARE(firstTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(firstTrack.composer(), QStringLiteral("composer1"));
        QCOMPARE(firstTrack.lyricist(), QStringLiteral("lyricist1"));
        QCOMPARE(firstTrack.hasEmbeddedCover(), true);

        auto newTracks2 = DataTypes::ListTrackDataType();

        newTracks2 = {{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                       QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                       6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                       QDateTime::fromMSecsSinceEpoch(23),
                       {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                       QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},};

        auto newCovers2 = mNewCovers;
        newCovers2[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks2, newCovers2);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType();

        newTracks = {{true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track7"),
                      QStringLiteral("artist3"), QStringLiteral("album3"), {QStringLiteral("artist4")}, 7, 1,
                      QTime::fromMSecsSinceStartOfDay(20), {QUrl::fromLocalFile(QStringLiteral("/$20"))},
                      QDateTime::fromMSecsSinceEpoch(20),
                      {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                      QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("file:///$20")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 2);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks2 = DataTypes::ListTrackDataType();

        newTracks2 = {{true, QStringLiteral("$19"), QStringLiteral("0"), QStringLiteral("track6"),
                       QStringLiteral("artist4"), QStringLiteral("album3"), {}, 6, 1,
                       QTime::fromMSecsSinceStartOfDay(19), {QUrl::fromLocalFile(QStringLiteral("/$19"))},
                       QDateTime::fromMSecsSinceEpoch(19),
                       {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                       QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), true}};

        musicDb.insertTracksList(newTracks2, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 2);
        QCOMPARE(musicDb.allTracksData().count(), 2);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrack = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist4"),
                                                                                                       QStringLiteral("album3"), 6, 1));

        QCOMPARE(firstTrack.isValid(), true);
        QCOMPARE(firstTrack.title(), QStringLiteral("track6"));
        QCOMPARE(firstTrack.artist(), QStringLiteral("artist4"));
        QCOMPARE(firstTrack.album(), QStringLiteral("album3"));
        QVERIFY(!firstTrack.albumArtist().isEmpty());
        QCOMPARE(firstTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstTrack.trackNumber(), 6);
        QCOMPARE(firstTrack.discNumber(), 1);
        QCOMPARE(firstTrack.duration(), QTime::fromMSecsSinceStartOfDay(19));
        QCOMPARE(firstTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$19")));
        QCOMPARE(firstTrack.rating(), 5);
        QCOMPARE(firstTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(firstTrack.composer(), QStringLiteral("composer1"));
        QCOMPARE(firstTrack.lyricist(), QStringLiteral("lyricist1"));
        QCOMPARE(firstTrack.hasEmbeddedCover(), true);

        auto secondTrack = musicDb.trackDataFromDatabaseId(musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track7"), QStringLiteral("artist3"),
                                                                                                        QStringLiteral("album3"), 7, 1));

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrack.title(), QStringLiteral("track7"));
        QCOMPARE(secondTrack.artist(), QStringLiteral("artist3"));
        QCOMPARE(secondTrack.album(), QStringLiteral("album3"));
        QCOMPARE(secondTrack.albumArtist(), QStringLiteral("artist4"));
        QCOMPARE(secondTrack.albumCover(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondTrack.trackNumber(), 7);
        QCOMPARE(secondTrack.discNumber(), 1);
        QCOMPARE(secondTrack.duration(), QTime::fromMSecsSinceStartOfDay(20));
        QCOMPARE(secondTrack.resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$20")));
        QCOMPARE(secondTrack.rating(), 5);
        QCOMPARE(secondTrack.genre(), QStringLiteral("genre1"));
        QCOMPARE(secondTrack.composer(), QStringLiteral("composer1"));
        QCOMPARE(secondTrack.lyricist(), QStringLiteral("lyricist1"));
        QCOMPARE(secondTrack.hasEmbeddedCover(), false);

        auto albumId = musicDb.albumIdFromTitleAndArtist(QStringLiteral("album3"), QStringLiteral("artist4"), QStringLiteral("/"));
        auto album = musicDb.albumDataFromDatabaseId(albumId);
        auto albumData = musicDb.albumData(albumId);

        QCOMPARE(album.isValid(), true);
        QCOMPARE(albumData.count(), 2);
        QCOMPARE(album.title(), QStringLiteral("album3"));
        QCOMPARE(album.artist(), QStringLiteral("artist4"));
        QCOMPARE(album.isValidArtist(), true);
        QCOMPARE(album.albumArtURI(), QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(album.isSingleDiscAlbum(), true);
    }

    void addTowAlbumsWithDifferentPathsAndSameName()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addTowAlbumsWithDifferentPathsAndSameName" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType();

        newTracks = {{true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track7"),
                      QStringLiteral("artist1"), QStringLiteral("album7"), {}, 7, 1,
                      QTime::fromMSecsSinceStartOfDay(20), {QUrl::fromLocalFile(QStringLiteral("/album7/$20"))},
                      QDateTime::fromMSecsSinceEpoch(20),
                      {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true,
                      QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
                     {true, QStringLiteral("$21"), QStringLiteral("0"), QStringLiteral("track8"),
                      QStringLiteral("artist2"), QStringLiteral("album7"), {}, 8, 1,
                      QTime::fromMSecsSinceStartOfDay(21), {QUrl::fromLocalFile(QStringLiteral("/album7/$21"))},
                      QDateTime::fromMSecsSinceEpoch(21),
                      {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true,
                      QStringLiteral("genre2"), QStringLiteral("composer2"), QStringLiteral("lyricist2"), false},
                     {true, QStringLiteral("$22"), QStringLiteral("0"), QStringLiteral("track9"),
                      QStringLiteral("artist3"), QStringLiteral("album7"), {}, 9, 1,
                      QTime::fromMSecsSinceStartOfDay(22), {QUrl::fromLocalFile(QStringLiteral("/album8/$22"))},
                      QDateTime::fromMSecsSinceEpoch(22),
                      {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true,
                      QStringLiteral("genre3"), QStringLiteral("composer3"), QStringLiteral("lyricist3"), false},
                     {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track10"),
                      QStringLiteral("artist4"), QStringLiteral("album7"), {}, 10, 1,
                      QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/album8/$23"))},
                      QDateTime::fromMSecsSinceEpoch(23),
                      {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true,
                      QStringLiteral("genre4"), QStringLiteral("composer4"), QStringLiteral("lyricist4"), false},};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///album7/$20")] = QUrl::fromLocalFile(QStringLiteral("album7"));
        newCovers[QStringLiteral("file:///album7/$21")] = QUrl::fromLocalFile(QStringLiteral("album7"));
        newCovers[QStringLiteral("file:///album8/$22")] = QUrl::fromLocalFile(QStringLiteral("album8"));
        newCovers[QStringLiteral("file:///album8/$23")] = QUrl::fromLocalFile(QStringLiteral("album8"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 2);
        QCOMPARE(musicDb.allArtistsData().count(), 4);
        QCOMPARE(musicDb.allTracksData().count(), 4);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        QCOMPARE(musicDb.allAlbumsData().size(), 2);
        QCOMPARE(musicDb.allArtistsData().size(), 4);
        QCOMPARE(musicDb.allTracksData().size(), 4);

        auto allAlbums = musicDb.allAlbumsData();
        auto firstAlbum = allAlbums[0];
        auto firstAlbumData = musicDb.albumData(firstAlbum.databaseId());
        QCOMPARE(firstAlbumData.count(), 2);
        auto secondAlbum = allAlbums[1];
        auto secondAlbumData = musicDb.albumData(secondAlbum.databaseId());
        QCOMPARE(secondAlbumData.count(), 2);
    }

    void addTwoAlbumsWithDifferentPathsAndSameTracks()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addTowAlbumsWithDifferentPathsAndSameName" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType();

        newTracks = {{true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track7"),
                      QStringLiteral("artist1"), QStringLiteral("album7"), {}, 7, 1,
                      QTime::fromMSecsSinceStartOfDay(20), {QUrl::fromLocalFile(QStringLiteral("/album7/$20"))},
                      QDateTime::fromMSecsSinceEpoch(20),
                      {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true,
                      QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
                     {true, QStringLiteral("$21"), QStringLiteral("0"), QStringLiteral("track8"),
                      QStringLiteral("artist2"), QStringLiteral("album7"), {}, 8, 1,
                      QTime::fromMSecsSinceStartOfDay(21), {QUrl::fromLocalFile(QStringLiteral("/album7/$21"))},
                      QDateTime::fromMSecsSinceEpoch(21),
                      {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true,
                      QStringLiteral("genre2"), QStringLiteral("composer2"), QStringLiteral("lyricist2"), false},
                     {true, QStringLiteral("$22"), QStringLiteral("0"), QStringLiteral("track9"),
                      QStringLiteral("artist3"), QStringLiteral("album7"), {}, 9, 1,
                      QTime::fromMSecsSinceStartOfDay(22), {QUrl::fromLocalFile(QStringLiteral("/album7/$22"))},
                      QDateTime::fromMSecsSinceEpoch(22),
                      {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true,
                      QStringLiteral("genre3"), QStringLiteral("composer3"), QStringLiteral("lyricist3"), false},
                     {true, QStringLiteral("$20"), QStringLiteral("0"), QStringLiteral("track7"),
                      QStringLiteral("artist1"), QStringLiteral("album7"), {}, 7, 1,
                      QTime::fromMSecsSinceStartOfDay(20), {QUrl::fromLocalFile(QStringLiteral("/album8/$20"))},
                      QDateTime::fromMSecsSinceEpoch(20),
                      {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true,
                      QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
                     {true, QStringLiteral("$21"), QStringLiteral("0"), QStringLiteral("track8"),
                      QStringLiteral("artist2"), QStringLiteral("album7"), {}, 8, 1,
                      QTime::fromMSecsSinceStartOfDay(21), {QUrl::fromLocalFile(QStringLiteral("/album8/$21"))},
                      QDateTime::fromMSecsSinceEpoch(21),
                      {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true,
                      QStringLiteral("genre2"), QStringLiteral("composer2"), QStringLiteral("lyricist2"), false},
                     {true, QStringLiteral("$22"), QStringLiteral("0"), QStringLiteral("track9"),
                      QStringLiteral("artist3"), QStringLiteral("album7"), {}, 9, 1,
                      QTime::fromMSecsSinceStartOfDay(22), {QUrl::fromLocalFile(QStringLiteral("/album8/$22"))},
                      QDateTime::fromMSecsSinceEpoch(22),
                      {QUrl::fromLocalFile(QStringLiteral("album7"))}, 5, true,
                      QStringLiteral("genre3"), QStringLiteral("composer3"), QStringLiteral("lyricist3"), false},};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///album7/$20")] = QUrl::fromLocalFile(QStringLiteral("album7"));
        newCovers[QStringLiteral("file:///album7/$21")] = QUrl::fromLocalFile(QStringLiteral("album7"));
        newCovers[QStringLiteral("file:///album7/$22")] = QUrl::fromLocalFile(QStringLiteral("album7"));
        newCovers[QStringLiteral("file:///album8/$20")] = QUrl::fromLocalFile(QStringLiteral("album8"));
        newCovers[QStringLiteral("file:///album8/$21")] = QUrl::fromLocalFile(QStringLiteral("album8"));
        newCovers[QStringLiteral("file:///album8/$22")] = QUrl::fromLocalFile(QStringLiteral("album8"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 2);
        QCOMPARE(musicDb.allArtistsData().count(), 3);
        QCOMPARE(musicDb.allTracksData().count(), 6);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allAlbums = musicDb.allAlbumsData();
        auto firstAlbum = allAlbums[0];
        auto firstAlbumData = musicDb.albumData(firstAlbum.databaseId());
        QCOMPARE(firstAlbumData.count(), 3);
        auto secondAlbum = allAlbums[1];
        auto secondAlbumData = musicDb.albumData(secondAlbum.databaseId());
        QCOMPARE(secondAlbumData.count(), 3);
    }

    void addTwoTracksFromSameAlbumButDifferentDiscs()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addTwoTracksFromSameAlbumButDifferentDiscs" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
        {QUrl::fromLocalFile(QStringLiteral("album3"))}, 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 6, 1);
        auto firstTrack = musicDb.trackDataFromDatabaseId(firstTrackId);

        auto firstTrackTitle = firstTrack.title();
        auto firstTrackArtist = firstTrack.artist();
        auto firstTrackAlbumArtist = firstTrack.albumArtist();
        auto firstTrackAlbum = firstTrack.album();
        auto firstTrackImage = firstTrack.albumCover();
        auto firstTrackDuration = firstTrack.duration();
        auto firstTrackMilliSecondsDuration = firstTrack.duration().msecsSinceStartOfDay();
        auto firstTrackTrackNumber = firstTrack.trackNumber();
        auto firstTrackDiscNumber = firstTrack.discNumber();
        const auto &firstTrackResource = firstTrack.resourceURI();
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

        auto allAlbums = musicDb.allAlbumsData();

        QCOMPARE(allAlbums.size(), 1);

        auto firstAlbumData = musicDb.albumData(allAlbums[0].databaseId());
        auto firstAlbumTitle = allAlbums[0].title();
        auto firstAlbumArtist = allAlbums[0].artist();
        auto firstAlbumImage = allAlbums[0].albumArtURI();
        auto firstAlbumTracksCount = firstAlbumData.count();
        auto firstAlbumIsSingleDiscAlbum = allAlbums[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle, QStringLiteral("album3"));
        QCOMPARE(firstAlbumArtist, QStringLiteral("artist2"));
        QCOMPARE(firstAlbumImage.isValid(), true);
        QCOMPARE(firstAlbumImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstAlbumTracksCount, 1);
        QCOMPARE(firstAlbumIsSingleDiscAlbum, true);

        auto newTrack2 = DataTypes::TrackDataType{true, QStringLiteral("$25"), QStringLiteral("0"), QStringLiteral("track8"),
                QStringLiteral("artist9"), QStringLiteral("album3"), QStringLiteral("artist2"), 8, 2, QTime::fromMSecsSinceStartOfDay(25),
        {QUrl::fromLocalFile(QStringLiteral("/$25"))},
                QDateTime::fromMSecsSinceEpoch(25),
        {QUrl::fromLocalFile(QStringLiteral("file://image$25"))}, 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks2 = DataTypes::ListTrackDataType();
        newTracks2.push_back(newTrack2);

        auto newCovers2 = mNewCovers;
        newCovers2[QStringLiteral("file:///$25")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks2, newCovers2);

        musicDbTrackAddedSpy.wait(50);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 2);
        QCOMPARE(musicDb.allTracksData().count(), 2);
        QCOMPARE(musicDbArtistAddedSpy.count(), 2);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 1);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track8"), QStringLiteral("artist9"),
                                                                          QStringLiteral("album3"), 8, 2);
        auto secondTrack = musicDb.trackDataFromDatabaseId(secondTrackId);

        auto secondTrackTitle = secondTrack.title();
        auto secondTrackArtist = secondTrack.artist();
        auto secondTrackAlbumArtist = secondTrack.albumArtist();
        auto secondTrackAlbum = secondTrack.album();
        auto secondTrackImage = secondTrack.albumCover();
        auto secondTrackDuration = secondTrack.duration();
        auto secondTrackMilliSecondsDuration = secondTrack.duration().msecsSinceStartOfDay();
        auto secondTrackTrackNumber = secondTrack.trackNumber();
        auto secondTrackDiscNumber = secondTrack.discNumber();
        const auto &secondTrackResource = secondTrack.resourceURI();
        auto secondTrackRating = secondTrack.rating();
        auto secondIsSingleDiscAlbum = secondTrack.isSingleDiscAlbum();

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrackTitle, QStringLiteral("track8"));
        QCOMPARE(secondTrackArtist, QStringLiteral("artist9"));
        QCOMPARE(secondTrackAlbumArtist, QStringLiteral("artist2"));
        QCOMPARE(secondTrackAlbum, QStringLiteral("album3"));
        QCOMPARE(secondTrackImage.isValid(), true);
        QCOMPARE(secondTrackImage, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(secondTrackDuration, QTime::fromMSecsSinceStartOfDay(25));
        QCOMPARE(secondTrackMilliSecondsDuration, 25);
        QCOMPARE(secondTrackTrackNumber, 8);
        QCOMPARE(secondTrackDiscNumber, 2);
        QCOMPARE(secondTrackResource.isValid(), true);
        QCOMPARE(secondTrackResource, QUrl::fromLocalFile(QStringLiteral("/$25")));
        QCOMPARE(secondTrackRating, 5);
        QCOMPARE(secondIsSingleDiscAlbum, false);

        auto allAlbums2 = musicDb.allAlbumsData();

        auto firstAlbumData2 = musicDb.albumData(allAlbums2[0].databaseId());
        auto firstAlbumTitle2 = allAlbums2[0].title();
        auto firstAlbumArtist2 = allAlbums2[0].artist();
        auto firstAlbumImage2 = allAlbums2[0].albumArtURI();
        auto firstAlbumTracksCount2 = firstAlbumData2.count();
        auto firstAlbumIsSingleDiscAlbum2 = allAlbums2[0].isSingleDiscAlbum();

        QCOMPARE(firstAlbumTitle2, QStringLiteral("album3"));
        QCOMPARE(firstAlbumArtist2, QStringLiteral("artist2"));
        QCOMPARE(firstAlbumImage2.isValid(), true);
        QCOMPARE(firstAlbumImage2, QUrl::fromLocalFile(QStringLiteral("album3")));
        QCOMPARE(firstAlbumTracksCount2, 2);
        QCOMPARE(firstAlbumIsSingleDiscAlbum2, false);
    }

    void addTwoIdenticalInvalidTracks()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), {},
        {}, {}, {}, {}, {}, {}, {QUrl::fromLocalFile(QStringLiteral("file:///$23"))},
                QDateTime::fromMSecsSinceEpoch(23), {}, {}, {}, {}, {}, {}, false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("file:///$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 23);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 2);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }


    void checkRestoredTracks()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);
        QSignalSpy musicDbRestoredTracksSpy(&musicDb, &DatabaseInterface::restoredTracks);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
        QCOMPARE(musicDbRestoredTracksSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
        QCOMPARE(musicDbRestoredTracksSpy.count(), 0);

        musicDb.askRestoredTracks();

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
        QCOMPARE(musicDbRestoredTracksSpy.count(), 1);

        const auto &firstSignal = musicDbRestoredTracksSpy.at(0);
        QCOMPARE(firstSignal.count(), 1);

        const auto &restoredTracks = firstSignal.at(0).value<QHash<QUrl,QDateTime>>();
        QCOMPARE(restoredTracks.count(), 23);
    }

    void addOneTrackWithParticularPath()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/test{{test}}/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};
        auto newTracks = DataTypes::ListTrackDataType();
        newTracks.push_back(newTrack);

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("/test{{test}}/$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void readRecentlyPlayedTracksData()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.trackHasStartedPlaying(QUrl::fromLocalFile(QStringLiteral("/$9")), QDateTime::fromSecsSinceEpoch(1534689));
        musicDb.trackHasStartedPlaying(QUrl::fromLocalFile(QStringLiteral("/$17")), QDateTime::fromSecsSinceEpoch(1534692));
        musicDb.trackHasStartedPlaying(QUrl::fromLocalFile(QStringLiteral("/$16")), QDateTime::fromSecsSinceEpoch(1534759));
        musicDb.trackHasStartedPlaying(QUrl::fromLocalFile(QStringLiteral("/$8")), QDateTime::fromSecsSinceEpoch(1534869));
        musicDb.trackHasStartedPlaying(QUrl::fromLocalFile(QStringLiteral("/$2")), QDateTime::fromSecsSinceEpoch(1535189));
        musicDb.trackHasStartedPlaying(QUrl::fromLocalFile(QStringLiteral("/$11")), QDateTime::fromSecsSinceEpoch(1535389));
        musicDb.trackHasStartedPlaying(QUrl::fromLocalFile(QStringLiteral("/$18")), QDateTime::fromSecsSinceEpoch(1535689));
        musicDb.trackHasStartedPlaying(QUrl::fromLocalFile(QStringLiteral("/$7")), QDateTime::fromSecsSinceEpoch(1536189));
        musicDb.trackHasStartedPlaying(QUrl::fromLocalFile(QStringLiteral("/$5")), QDateTime::fromSecsSinceEpoch(1536689));
        musicDb.trackHasStartedPlaying(QUrl::fromLocalFile(QStringLiteral("/$13")), QDateTime::fromSecsSinceEpoch(1537689));

        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$9")), QDateTime::fromSecsSinceEpoch(1534689));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$17")), QDateTime::fromSecsSinceEpoch(1534692));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$16")), QDateTime::fromSecsSinceEpoch(1534759));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$8")), QDateTime::fromSecsSinceEpoch(1534869));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$2")), QDateTime::fromSecsSinceEpoch(1535189));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$11")), QDateTime::fromSecsSinceEpoch(1535389));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$18")), QDateTime::fromSecsSinceEpoch(1535689));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$7")), QDateTime::fromSecsSinceEpoch(1536189));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$5")), QDateTime::fromSecsSinceEpoch(1536689));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$13")), QDateTime::fromSecsSinceEpoch(1537689));

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto recentlyPlayedTracksData = musicDb.recentlyPlayedTracksData(10);

        QCOMPARE(recentlyPlayedTracksData.count(), 10);
        QCOMPARE(recentlyPlayedTracksData[0].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$13")));
        QCOMPARE(recentlyPlayedTracksData[1].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$5")));
        QCOMPARE(recentlyPlayedTracksData[2].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$7")));
        QCOMPARE(recentlyPlayedTracksData[3].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$18")));
        QCOMPARE(recentlyPlayedTracksData[4].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$11")));
        QCOMPARE(recentlyPlayedTracksData[5].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$2")));
        QCOMPARE(recentlyPlayedTracksData[6].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$8")));
        QCOMPARE(recentlyPlayedTracksData[7].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$16")));
        QCOMPARE(recentlyPlayedTracksData[8].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$17")));
        QCOMPARE(recentlyPlayedTracksData[9].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$9")));
    }

    void readFrequentlyPlayedTracksData()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$9")), QDateTime::fromSecsSinceEpoch(1553279650));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$17")), QDateTime::fromSecsSinceEpoch(1553288650));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$16")), QDateTime::fromSecsSinceEpoch(1553287650));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$8")), QDateTime::fromSecsSinceEpoch(1553286650));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$2")), QDateTime::fromSecsSinceEpoch(1553285650));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$11")), QDateTime::fromSecsSinceEpoch(1553284650));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$18")), QDateTime::fromSecsSinceEpoch(1553283650));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$7")), QDateTime::fromSecsSinceEpoch(1553282650));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$5")), QDateTime::fromSecsSinceEpoch(1553281650));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$13")), QDateTime::fromSecsSinceEpoch(1553280650));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$9")), QDateTime::fromSecsSinceEpoch(1553289660));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$2")), QDateTime::fromSecsSinceEpoch(1553289680));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$18")), QDateTime::fromSecsSinceEpoch(1553289700));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$17")), QDateTime::fromSecsSinceEpoch(1553289720));
        musicDb.trackHasFinishedPlaying(QUrl::fromLocalFile(QStringLiteral("/$7")), QDateTime::fromSecsSinceEpoch(1553289740));

        auto frequentlyPlayedTracksData = musicDb.frequentlyPlayedTracksData(5);

        QCOMPARE(frequentlyPlayedTracksData.count(), 5);
        QCOMPARE(frequentlyPlayedTracksData[0].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$2")));
        QCOMPARE(frequentlyPlayedTracksData[1].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$7")));
        QCOMPARE(frequentlyPlayedTracksData[2].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$9")));
        QCOMPARE(frequentlyPlayedTracksData[3].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$17")));
        QCOMPARE(frequentlyPlayedTracksData[4].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/$18")));
    }

    void readAllGenresData()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allGenresData = musicDb.allGenresData();

        QCOMPARE(allGenresData.count(), 4);
        QCOMPARE(allGenresData[0].title(), QStringLiteral("genre1"));
        QCOMPARE(allGenresData[1].title(), QStringLiteral("genre2"));
        QCOMPARE(allGenresData[2].title(), QStringLiteral("genre3"));
        QCOMPARE(allGenresData[3].title(), QStringLiteral("genre4"));
    }

    void clearDataTest()
    {
        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"));

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);
        QSignalSpy musicDbCleanedDatabaseSpy(&musicDb, &DatabaseInterface::cleanedDatabase);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
        QCOMPARE(musicDbCleanedDatabaseSpy.count(), 0);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
        QCOMPARE(musicDbCleanedDatabaseSpy.count(), 0);

        musicDb.clearData();

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
        QCOMPARE(musicDbCleanedDatabaseSpy.count(), 1);
    }

    void upgradeFromStableVersion()
    {
        auto dbTestFile = QString{QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH) + QStringLiteral("/elisaDatabase.v0.3.db")};
        auto dbOriginFile = QString{QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/elisaDatabase.v0.3.db")};
        QVERIFY(QDir().mkpath(QStringLiteral(LOCAL_FILE_TESTS_WORKING_PATH)));
        QFile::remove(dbTestFile);
        QFile::copy(dbOriginFile, dbTestFile);

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), dbTestFile);

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDb.allArtistsData().count(), 7);
        QCOMPARE(musicDb.allTracksData().count(), 22);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void addMultipleDifferentTracksWithSameTitle()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "addMultipleDifferentTracksWithSameTitle" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType{
        {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/test/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
        {true, QStringLiteral("$24"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                7, 1, QTime::fromMSecsSinceStartOfDay(24), {QUrl::fromLocalFile(QStringLiteral("/test/$24"))},
                QDateTime::fromMSecsSinceEpoch(24),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
        {true, QStringLiteral("$25"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                8, 1, QTime::fromMSecsSinceStartOfDay(25), {QUrl::fromLocalFile(QStringLiteral("/test/$25"))},
                QDateTime::fromMSecsSinceEpoch(25),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("/test/$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$24")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$25")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 3);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
    }

    void enqueueTracksWithMissingMetadata()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "enqueueTracksWithMissingMetadata" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("enqueueTracksWithMissingMetadata"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto fullTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/test/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};

        fullTrack[DataTypes::BitRateRole] = 154;
        fullTrack[DataTypes::ChannelsRole] = 2;
        fullTrack[DataTypes::SampleRateRole] = 48000;

        auto newTrack = DataTypes::TrackDataType{};

        newTrack[DataTypes::IdRole] = QStringLiteral("$29");
        newTrack[DataTypes::ParentIdRole] = QStringLiteral("0");
        newTrack[DataTypes::TitleRole] = QStringLiteral("track19");
        newTrack[DataTypes::ArtistRole] = QStringLiteral("artist2");
        newTrack[DataTypes::DurationRole] = QTime::fromMSecsSinceStartOfDay(29);
        newTrack[DataTypes::ResourceRole] = QUrl::fromLocalFile(QStringLiteral("/$29"));
        newTrack[DataTypes::FileModificationTime] = QDateTime::fromMSecsSinceEpoch(29);
        newTrack[DataTypes::ImageUrlRole] = QUrl::fromLocalFile(QStringLiteral("/withoutAlbum"));
        newTrack[DataTypes::RatingRole] = 9;
        newTrack[DataTypes::IsSingleDiscAlbumRole] = true;
        newTrack[DataTypes::GenreRole] = QStringLiteral("genre1");
        newTrack[DataTypes::ComposerRole] = QStringLiteral("composer1");
        newTrack[DataTypes::LyricistRole] = QStringLiteral("lyricist1");
        newTrack[DataTypes::HasEmbeddedCover] = false;
        newTrack[DataTypes::ElementTypeRole] = ElisaUtils::Track;

        auto temp = {fullTrack, newTrack};
        musicDb.insertTracksList(temp, mNewCovers);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 2);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto firstTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track6"), QStringLiteral("artist2"),
                                                                         QStringLiteral("album3"), 6, 1);
        auto firstTrack = musicDb.trackDataFromDatabaseId(firstTrackId);

        auto firstTrackTitle = firstTrack.title();
        auto firstTrackArtist = firstTrack.artist();
        auto firstTrackAlbumArtist = firstTrack.albumArtist();
        auto firstTrackAlbum = firstTrack.album();
        auto firstTrackImage = firstTrack.albumCover();
        auto firstTrackDuration = firstTrack.duration();
        auto firstTrackMilliSecondsDuration = firstTrack.duration().msecsSinceStartOfDay();
        auto firstTrackTrackNumber = firstTrack.trackNumber();
        auto firstTrackDiscNumber = firstTrack.discNumber();
        const auto &firstTrackResource = firstTrack.resourceURI();
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
        QCOMPARE(firstTrackResource, QUrl::fromLocalFile(QStringLiteral("/test/$23")));
        QCOMPARE(firstTrackRating, 5);
        QCOMPARE(firstIsSingleDiscAlbum, true);

        auto secondTrackId = musicDb.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track19"), QStringLiteral("artist2"), {}, {}, {});
        auto secondTrack = musicDb.trackDataFromDatabaseId(secondTrackId);

        auto secondTrackTitle = secondTrack.title();
        auto secondTrackArtist = secondTrack.artist();
        auto secondTrackAlbumArtist = secondTrack.albumArtist();
        auto secondTrackAlbum = secondTrack.album();
        auto secondTrackImage = secondTrack.albumCover();
        auto secondTrackDuration = secondTrack.duration();
        auto secondTrackMilliSecondsDuration = secondTrack.duration().msecsSinceStartOfDay();
        auto secondTrackTrackNumber = secondTrack.trackNumber();
        auto secondTrackDiscNumber = secondTrack.discNumber();
        auto secondTrackChannels = secondTrack.channels();
        auto secondTrackBitRate = secondTrack.bitRate();
        auto secondTrackSampleRate = secondTrack.sampleRate();
        const auto &secondTrackResource = secondTrack.resourceURI();
        auto secondTrackRating = secondTrack.rating();
        auto secondIsSingleDiscAlbum = secondTrack.isSingleDiscAlbum();

        QCOMPARE(secondTrack.isValid(), true);
        QCOMPARE(secondTrackTitle, QStringLiteral("track19"));
        QCOMPARE(secondTrackArtist, QStringLiteral("artist2"));
        QCOMPARE(secondTrackAlbumArtist, QString());
        QCOMPARE(secondTrackAlbum, QString());
        QCOMPARE(secondTrackImage.isValid(), false);
        QCOMPARE(secondTrackDuration, QTime::fromMSecsSinceStartOfDay(29));
        QCOMPARE(secondTrackMilliSecondsDuration, 29);
        QCOMPARE(secondTrackTrackNumber, 0);
        QCOMPARE(secondTrackDiscNumber, 0);
        QCOMPARE(secondTrackChannels, 0);
        QCOMPARE(secondTrackBitRate, 0);
        QCOMPARE(secondTrackSampleRate, 0);
        QCOMPARE(secondTrackResource.isValid(), true);
        QCOMPARE(secondTrackResource, QUrl::fromLocalFile(QStringLiteral("/$29")));
        QCOMPARE(secondTrackRating, 9);
        QCOMPARE(secondIsSingleDiscAlbum, true);
    }

    void testCompilationWithoutAlbumArtist()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "testCompilationWithoutAlbumArtist" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType{
        {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), {},
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/test/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
        {true, QStringLiteral("$24"), QStringLiteral("0"), QStringLiteral("track7"),
                QStringLiteral("artist3"), QStringLiteral("album3"), {},
                7, 1, QTime::fromMSecsSinceStartOfDay(24), {QUrl::fromLocalFile(QStringLiteral("/test/$24"))},
                QDateTime::fromMSecsSinceEpoch(24),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
        {true, QStringLiteral("$25"), QStringLiteral("0"), QStringLiteral("track8"),
                QStringLiteral("artist4"), QStringLiteral("album3"), {},
                8, 1, QTime::fromMSecsSinceStartOfDay(25), {QUrl::fromLocalFile(QStringLiteral("/test/$25"))},
                QDateTime::fromMSecsSinceEpoch(25),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("/test/$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$24")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$25")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 3);
        QCOMPARE(musicDb.allTracksData().count(), 3);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        const auto newAlbumSignal = musicDbAlbumAddedSpy.at(0);
        QCOMPARE(newAlbumSignal.size(), 1);
        const auto newAlbums = newAlbumSignal.at(0).value<DataTypes::ListAlbumDataType>();
        QCOMPARE(newAlbums.size(), 1);
        const auto newAlbum = newAlbums.at(0);
        QCOMPARE(newAlbum.title(), QStringLiteral("album3"));
        QCOMPARE(newAlbum.artist(), QStringLiteral("Various Artists"));
        QCOMPARE(newAlbum[DataTypes::ColumnsRoles::SecondaryTextRole], QStringLiteral("Various Artists"));

        const auto oneAlbum = musicDb.allAlbumsData().at(0);
        QCOMPARE(oneAlbum.title(), QStringLiteral("album3"));
        QCOMPARE(oneAlbum.artist(), QStringLiteral("Various Artists"));
        QCOMPARE(oneAlbum[DataTypes::ColumnsRoles::SecondaryTextRole], QStringLiteral("Various Artists"));
    }

    void testAlbumFromGenreAndArtistWithoutAlbumArtist()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "testAlbumFromGenreAndArtistWithoutAlbumArtist" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType{
        {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), {},
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/test/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("/test/$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$24")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$25")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto albumFromGenreAndArtist = musicDb.allAlbumsDataByGenreAndArtist(QStringLiteral("genre1"), QStringLiteral("artist2"));

        QCOMPARE(albumFromGenreAndArtist.size(), 1);
    }

    void testTracksWithoutMetadata()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "testTracksWithoutMetadata" << databaseFile.fileName();

        DatabaseInterface musicDb;

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        QSignalSpy musicDbArtistAddedSpy(&musicDb, &DatabaseInterface::artistsAdded);
        QSignalSpy musicDbAlbumAddedSpy(&musicDb, &DatabaseInterface::albumsAdded);
        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbArtistRemovedSpy(&musicDb, &DatabaseInterface::artistRemoved);
        QSignalSpy musicDbAlbumRemovedSpy(&musicDb, &DatabaseInterface::albumRemoved);
        QSignalSpy musicDbTrackRemovedSpy(&musicDb, &DatabaseInterface::trackRemoved);
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType {
        {true, {}, {}, {}, {}, {}, {}, 0, 0, {}, {QUrl::fromLocalFile(QStringLiteral("/test/$23"))}, QDateTime::fromMSecsSinceEpoch(23), {}, 0, true, {}, {}, {}, false},
        {true, {}, {}, {}, {}, {}, {}, 0, 0, {}, {QUrl::fromLocalFile(QStringLiteral("/test/$24"))}, QDateTime::fromMSecsSinceEpoch(24), {}, 0, true, {}, {}, {}, false},
        {true, {}, {}, {}, {}, {}, {}, 0, 0, {}, {QUrl::fromLocalFile(QStringLiteral("/test/$25"))}, QDateTime::fromMSecsSinceEpoch(25), {}, 0, true, {}, {}, {}, false}};

        auto newCovers = mNewCovers;

        musicDb.insertTracksList(newTracks, newCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 3);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto allTracks = musicDb.allTracksData();
        QVERIFY(allTracks[0].title().isEmpty());
        QCOMPARE(allTracks[0].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/test/$23")));
        QCOMPARE(allTracks[0].fileModificationTime(), QDateTime::fromMSecsSinceEpoch(23));
        QVERIFY(allTracks[1].title().isEmpty());
        QCOMPARE(allTracks[1].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/test/$24")));
        QCOMPARE(allTracks[1].fileModificationTime(), QDateTime::fromMSecsSinceEpoch(24));
        QVERIFY(allTracks[2].title().isEmpty());
        QCOMPARE(allTracks[2].resourceURI(), QUrl::fromLocalFile(QStringLiteral("/test/$25")));
        QCOMPARE(allTracks[2].fileModificationTime(), QDateTime::fromMSecsSinceEpoch(25));
    }

    void removeTrackNumber()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "removeTrackNumber" << databaseFile.fileName();

        DatabaseInterface musicDb;

        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbErrorSpy(&musicDb, &DatabaseInterface::databaseError);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDbErrorSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType{
        {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), {},
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/test/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("/test/$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$24")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$25")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        QCOMPARE(musicDb.allAlbumsData().count(), 6);
        QCOMPARE(musicDbErrorSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);

        auto trackIdFirstVersion = musicDb.trackIdFromFileName(QUrl::fromLocalFile(QStringLiteral("/test/$23")));
        auto firstTrackDataVersion = musicDb.trackDataFromDatabaseId(trackIdFirstVersion);

        auto newTrackData = firstTrackDataVersion;
        newTrackData.remove(DataTypes::TrackNumberRole);

        musicDb.insertTracksList({newTrackData}, newCovers);

        QCOMPARE(musicDb.allAlbumsData().count(), 6);
        QCOMPARE(musicDbErrorSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);

        auto trackIdSecondVersion = musicDb.trackIdFromFileName(QUrl::fromLocalFile(QStringLiteral("/test/$23")));
        QCOMPARE(trackIdFirstVersion, trackIdSecondVersion);

        auto secondTrackDataVersion = musicDb.trackDataFromDatabaseId(trackIdSecondVersion);

        auto modifiedTrack = musicDbTrackModifiedSpy.at(0).at(0).value<DataTypes::TrackDataType>();

        auto trackIdIterator = secondTrackDataVersion.find(DataTypes::TrackNumberRole);
        QCOMPARE(trackIdIterator, secondTrackDataVersion.end());

        QCOMPARE(secondTrackDataVersion, modifiedTrack);
    }

    void removeDiscNumber()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "removeTrackNumber" << databaseFile.fileName();

        DatabaseInterface musicDb;

        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbErrorSpy(&musicDb, &DatabaseInterface::databaseError);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDbErrorSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType{
        {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), {},
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/test/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("/test/$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$24")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$25")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        QCOMPARE(musicDb.allAlbumsData().count(), 6);
        QCOMPARE(musicDbErrorSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);

        auto trackIdFirstVersion = musicDb.trackIdFromFileName(QUrl::fromLocalFile(QStringLiteral("/test/$23")));
        auto firstTrackDataVersion = musicDb.trackDataFromDatabaseId(trackIdFirstVersion);

        auto newTrackData = firstTrackDataVersion;
        newTrackData.remove(DataTypes::DiscNumberRole);

        musicDb.insertTracksList({newTrackData}, newCovers);

        QCOMPARE(musicDb.allAlbumsData().count(), 6);
        QCOMPARE(musicDbErrorSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);

        auto trackIdSecondVersion = musicDb.trackIdFromFileName(QUrl::fromLocalFile(QStringLiteral("/test/$23")));
        QCOMPARE(trackIdFirstVersion, trackIdSecondVersion);

        auto secondTrackDataVersion = musicDb.trackDataFromDatabaseId(trackIdSecondVersion);

        auto modifiedTrack = musicDbTrackModifiedSpy.at(0).at(0).value<DataTypes::TrackDataType>();

        auto trackIdIterator = secondTrackDataVersion.find(DataTypes::DiscNumberRole);
        QCOMPARE(trackIdIterator, secondTrackDataVersion.end());

        QCOMPARE(secondTrackDataVersion, modifiedTrack);

        qDebug() << trackIdFirstVersion << secondTrackDataVersion;
    }

    void removeYear()
    {
        QTemporaryFile databaseFile;
        databaseFile.open();

        qDebug() << "removeYear" << databaseFile.fileName();

        DatabaseInterface musicDb;

        QSignalSpy musicDbTrackAddedSpy(&musicDb, &DatabaseInterface::tracksAdded);
        QSignalSpy musicDbErrorSpy(&musicDb, &DatabaseInterface::databaseError);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);

        musicDb.init(QStringLiteral("testDb"), databaseFile.fileName());

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 5);
        QCOMPARE(musicDbErrorSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);

        auto newTracks = DataTypes::ListTrackDataType{
        {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), {},
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/test/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false}};

        newTracks.last()[DataTypes::YearRole] = 2020;

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("/test/$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$24")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$25")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        QCOMPARE(musicDb.allAlbumsData().count(), 6);
        QCOMPARE(musicDbErrorSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);

        auto trackIdFirstVersion = musicDb.trackIdFromFileName(QUrl::fromLocalFile(QStringLiteral("/test/$23")));
        auto firstTrackDataVersion = musicDb.trackDataFromDatabaseId(trackIdFirstVersion);

        auto newTrackData = firstTrackDataVersion;
        newTrackData.remove(DataTypes::YearRole);

        musicDb.insertTracksList({newTrackData}, newCovers);

        QCOMPARE(musicDb.allAlbumsData().count(), 6);
        QCOMPARE(musicDbErrorSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);

        auto trackIdSecondVersion = musicDb.trackIdFromFileName(QUrl::fromLocalFile(QStringLiteral("/test/$23")));
        QCOMPARE(trackIdFirstVersion, trackIdSecondVersion);

        auto secondTrackDataVersion = musicDb.trackDataFromDatabaseId(trackIdSecondVersion);

        auto modifiedTrack = musicDbTrackModifiedSpy.at(0).at(0).value<DataTypes::TrackDataType>();

        auto trackIdIterator = secondTrackDataVersion.find(DataTypes::YearRole);
        QCOMPARE(trackIdIterator, secondTrackDataVersion.end());

        QCOMPARE(secondTrackDataVersion, modifiedTrack);
    }

    void modifyOneTrackWithEmbeddedCover()
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
        QSignalSpy musicDbAlbumModifiedSpy(&musicDb, &DatabaseInterface::albumModified);
        QSignalSpy musicDbTrackModifiedSpy(&musicDb, &DatabaseInterface::trackModified);
        QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

        QCOMPARE(musicDb.allAlbumsData().count(), 0);
        QCOMPARE(musicDb.allArtistsData().count(), 0);
        QCOMPARE(musicDb.allTracksData().count(), 0);
        QCOMPARE(musicDbArtistAddedSpy.count(), 0);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 0);
        QCOMPARE(musicDbTrackAddedSpy.count(), 0);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto fullTrack = DataTypes::TrackDataType{true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), QStringLiteral("artist2"),
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/test/$23"))},
                QDateTime::fromMSecsSinceEpoch(23), {}, 5, true,
                QStringLiteral("genre1"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), true};

        fullTrack[DataTypes::BitRateRole] = 154;
        fullTrack[DataTypes::ChannelsRole] = 2;
        fullTrack[DataTypes::SampleRateRole] = 48000;

        musicDb.insertTracksList({fullTrack}, {});

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 0);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto trackIdFirstVersion = musicDb.trackIdFromFileName(QUrl::fromLocalFile(QStringLiteral("/test/$23")));
        auto firstTrackDataVersion = musicDb.trackDataFromDatabaseId(trackIdFirstVersion);

        QVERIFY(firstTrackDataVersion.hasEmbeddedCover());
        QCOMPARE(firstTrackDataVersion[DataTypes::ImageUrlRole].toString(), QStringLiteral("image://cover//test/$23"));

        firstTrackDataVersion[DataTypes::TrackNumberRole] = 12;
        firstTrackDataVersion[DataTypes::FileModificationTime] = QDateTime::fromMSecsSinceEpoch(32);

        musicDb.insertTracksList({firstTrackDataVersion}, {});

        musicDbTrackAddedSpy.wait(300);

        QCOMPARE(musicDb.allAlbumsData().count(), 1);
        QCOMPARE(musicDb.allArtistsData().count(), 1);
        QCOMPARE(musicDb.allTracksData().count(), 1);
        QCOMPARE(musicDbArtistAddedSpy.count(), 1);
        QCOMPARE(musicDbAlbumAddedSpy.count(), 1);
        QCOMPARE(musicDbTrackAddedSpy.count(), 1);
        QCOMPARE(musicDbArtistRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumRemovedSpy.count(), 0);
        QCOMPARE(musicDbTrackRemovedSpy.count(), 0);
        QCOMPARE(musicDbAlbumModifiedSpy.count(), 0);
        QCOMPARE(musicDbTrackModifiedSpy.count(), 1);
        QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);

        auto modifiedTrackIdFirstVersion = musicDb.trackIdFromFileName(QUrl::fromLocalFile(QStringLiteral("/test/$23")));
        auto modifiedTrackDataVersion = musicDb.trackDataFromDatabaseId(modifiedTrackIdFirstVersion);

        QVERIFY(modifiedTrackDataVersion.hasEmbeddedCover());
        QCOMPARE(modifiedTrackDataVersion[DataTypes::ImageUrlRole].toString(), QStringLiteral("image://cover//test/$23"));

        auto modifiedTrack = musicDbTrackModifiedSpy.at(0).at(0).value<DataTypes::TrackDataType>();

        QVERIFY(modifiedTrack.hasEmbeddedCover());
        QCOMPARE(modifiedTrack[DataTypes::ImageUrlRole].toString(), QStringLiteral("image://cover//test/$23"));
    }

    void testInvalidDatabase()
    {
        const auto dbName = QStringLiteral("testDb");
        QString databaseFileName;

        {
            // NOTE: a QTemporaryFile cannot be deleted but `resetDatabase()` attempts to delete the database file,
            // hence why we use a QFile in this test. Remember to delete the file when the test has finished!
            QTemporaryFile tempFile;
            tempFile.open();
            databaseFileName = tempFile.fileName();
            tempFile.setAutoRemove(false);
        }

        QFile databaseFile(databaseFileName);

        // Make a valid database
        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

            musicDb.init(dbName, databaseFileName);

            QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
        }

        // Invalidate the database
        {
            auto invalidDatabase = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), dbName);
            invalidDatabase.setDatabaseName(databaseFileName);
            QCOMPARE(invalidDatabase.open(), true);

            QCOMPARE(invalidDatabase.transaction(), true);

            auto dropQuery = QSqlQuery(invalidDatabase);
            QCOMPARE(dropQuery.prepare(QStringLiteral("ALTER TABLE `Albums` DROP COLUMN `CoverFileName`")), true);
            QCOMPARE(dropQuery.exec(), true);
            dropQuery.finish();

            QCOMPARE(invalidDatabase.commit(), true);

            invalidDatabase.close();
        }

        // This one should detect the error & fix it
        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

            musicDb.init(dbName, databaseFileName);

            QCOMPARE(musicDbDatabaseErrorSpy.count(), 1);
        }

        // Check database has been fixed
        {
            DatabaseInterface musicDb;

            QSignalSpy musicDbDatabaseErrorSpy(&musicDb, &DatabaseInterface::databaseError);

            musicDb.init(dbName, databaseFileName);

            QCOMPARE(musicDbDatabaseErrorSpy.count(), 0);
        }

        databaseFile.remove();
    }
};

QTEST_GUILESS_MAIN(DatabaseInterfaceTests)


#include "databaseinterfacetest.moc"
