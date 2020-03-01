/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "databasetestdata.h"

#include "trackslistener.h"
#include "mediaplaylist.h"
#include "databaseinterface.h"
#include "datatypes.h"

#include "config-upnp-qt.h"

#include <QObject>
#include <QUrl>
#include <QString>
#include <QHash>
#include <QVector>
#include <QThread>
#include <QStandardPaths>


#include <QtTest>

class TracksListenerTests: public QObject, public DatabaseTestData
{
    Q_OBJECT

public:

    TracksListenerTests(QObject *parent = nullptr) : QObject(parent)
    {
    }

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
        qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
        qRegisterMetaType<QList<QUrl>>("QList<QUrl>");
        qRegisterMetaType<DataTypes::ListTrackDataType>("ListTrackDataType");
        qRegisterMetaType<DataTypes::ListAlbumDataType>("ListAlbumDataType");
        qRegisterMetaType<DataTypes::ListArtistDataType>("ListArtistDataType");
        qRegisterMetaType<DataTypes::ListGenreDataType>("ListGenreDataType");
        qRegisterMetaType<DataTypes::TrackDataType>("TrackDataType");
        qRegisterMetaType<DataTypes::AlbumDataType>("AlbumDataType");
        qRegisterMetaType<DataTypes::ArtistDataType>("ArtistDataType");
        qRegisterMetaType<DataTypes::GenreDataType>("GenreDataType");
        qRegisterMetaType<ElisaUtils::PlayListEntryType>("PlayListEntryType");
        qRegisterMetaType<TracksListener::TrackDataType>("TracksListener::TrackDataType");
        qRegisterMetaType<TracksListener::ListTrackDataType>("TracksListener::ListTrackDataType");
    }

    void testTrackRemoval()
    {
        MediaPlayList myPlayList;
        DatabaseInterface myDatabaseContent;
        TracksListener myListener(&myDatabaseContent);

        QSignalSpy trackHasChangedSpy(&myListener, &TracksListener::trackHasChanged);
        QSignalSpy trackHasBeenRemovedSpy(&myListener, &TracksListener::trackHasBeenRemoved);
        QSignalSpy tracksListAddedSpy(&myListener, &TracksListener::tracksListAdded);

        myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

        connect(&myDatabaseContent, &DatabaseInterface::trackRemoved, &myListener, &TracksListener::trackRemoved);
        connect(&myDatabaseContent, &DatabaseInterface::tracksAdded, &myListener, &TracksListener::tracksAdded);
        connect(&myDatabaseContent, &DatabaseInterface::trackModified, &myListener, &TracksListener::trackModified);
        connect(&myListener, &TracksListener::trackHasChanged, &myPlayList, &MediaPlayList::trackChanged);
        connect(&myListener, &TracksListener::trackHasBeenRemoved, &myPlayList, &MediaPlayList::trackRemoved);
        connect(&myListener, &TracksListener::tracksListAdded, &myPlayList, &MediaPlayList::tracksListAdded);
        connect(&myPlayList, &MediaPlayList::newEntryInList, &myListener, &TracksListener::newEntryInList);
        connect(&myPlayList, &MediaPlayList::newTrackByNameInList, &myListener, &TracksListener::trackByNameInList);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        myPlayList.enqueueOneEntry(DataTypes::EntryData{{{DataTypes::ElementTypeRole, ElisaUtils::Artist}}, QStringLiteral("artist1"), {}});

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 1);

        QCOMPARE(myPlayList.rowCount(), 6);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
        QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
        QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
        QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
        QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
        QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
        QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
        QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
        QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
        QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
        QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
        QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
        QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 5);
        QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);

        myDatabaseContent.removeTracksList({QUrl::fromLocalFile(QStringLiteral("/$1"))});

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 1);
        QCOMPARE(tracksListAddedSpy.count(), 1);

        QCOMPARE(myPlayList.rowCount(), 6);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), false);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), -1);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 0);
        QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
        QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(1, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track2"));
        QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
        QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
        QCOMPARE(myPlayList.data(myPlayList.index(2, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track3"));
        QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
        QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 3);
        QCOMPARE(myPlayList.data(myPlayList.index(3, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track4"));
        QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
        QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 4);
        QCOMPARE(myPlayList.data(myPlayList.index(4, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track5"));
        QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album2"));
        QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 5);
        QCOMPARE(myPlayList.data(myPlayList.index(5, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);
    }

    void testInsertTrackIdAndRemoval()
    {
        MediaPlayList myPlayList;
        DatabaseInterface myDatabaseContent;
        TracksListener myListener(&myDatabaseContent);

        QSignalSpy trackHasChangedSpy(&myListener, &TracksListener::trackHasChanged);
        QSignalSpy trackHasBeenRemovedSpy(&myListener, &TracksListener::trackHasBeenRemoved);
        QSignalSpy tracksListAddedSpy(&myListener, &TracksListener::tracksListAdded);

        myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

        connect(&myDatabaseContent, &DatabaseInterface::trackRemoved, &myListener, &TracksListener::trackRemoved);
        connect(&myDatabaseContent, &DatabaseInterface::tracksAdded, &myListener, &TracksListener::tracksAdded);
        connect(&myDatabaseContent, &DatabaseInterface::trackModified, &myListener, &TracksListener::trackModified);
        connect(&myListener, &TracksListener::trackHasChanged, &myPlayList, &MediaPlayList::trackChanged);
        connect(&myListener, &TracksListener::trackHasBeenRemoved, &myPlayList, &MediaPlayList::trackRemoved);
        connect(&myListener, &TracksListener::tracksListAdded, &myPlayList, &MediaPlayList::tracksListAdded);
        connect(&myPlayList, &MediaPlayList::newEntryInList, &myListener, &TracksListener::newEntryInList);
        connect(&myPlayList, &MediaPlayList::newTrackByNameInList, &myListener, &TracksListener::trackByNameInList);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        auto trackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                              QStringLiteral("album1"), 1, 1);

        QCOMPARE(trackId != 0, true);

        myPlayList.enqueueOneEntry(DataTypes::EntryData{{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                                         {DataTypes::DatabaseIdRole, trackId}}, {}, {}});

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.rowCount(), 1);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);

        myDatabaseContent.removeTracksList({QUrl::fromLocalFile(QStringLiteral("/$1"))});

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 1);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.rowCount(), 1);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), false);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), -1);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 0);
    }

    void testInsertTrackByNameAndRemoval()
    {
        MediaPlayList myPlayList;
        DatabaseInterface myDatabaseContent;
        TracksListener myListener(&myDatabaseContent);

        QSignalSpy trackHasChangedSpy(&myListener, &TracksListener::trackHasChanged);
        QSignalSpy trackHasBeenRemovedSpy(&myListener, &TracksListener::trackHasBeenRemoved);
        QSignalSpy tracksListAddedSpy(&myListener, &TracksListener::tracksListAdded);

        myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

        connect(&myDatabaseContent, &DatabaseInterface::trackRemoved, &myListener, &TracksListener::trackRemoved);
        connect(&myDatabaseContent, &DatabaseInterface::tracksAdded, &myListener, &TracksListener::tracksAdded);
        connect(&myDatabaseContent, &DatabaseInterface::trackModified, &myListener, &TracksListener::trackModified);
        connect(&myListener, &TracksListener::trackHasChanged, &myPlayList, &MediaPlayList::trackChanged);
        connect(&myListener, &TracksListener::trackHasBeenRemoved, &myPlayList, &MediaPlayList::trackRemoved);
        connect(&myListener, &TracksListener::tracksListAdded, &myPlayList, &MediaPlayList::tracksListAdded);
        connect(&myPlayList, &MediaPlayList::newEntryInList, &myListener, &TracksListener::newEntryInList);
        connect(&myPlayList, &MediaPlayList::newTrackByNameInList, &myListener, &TracksListener::trackByNameInList);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        myPlayList.enqueueOneEntry({{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                     {DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1)}},
                                    QStringLiteral("track1"), {}});

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.rowCount(), 1);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);

        myDatabaseContent.removeTracksList({QUrl::fromLocalFile(QStringLiteral("/$1"))});

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 1);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.rowCount(), 1);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), false);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), -1);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 0);
    }

    void testInsertTrackByNameBeforeDatabaseAndRemoval()
    {
        MediaPlayList myPlayList;
        DatabaseInterface myDatabaseContent;
        TracksListener myListener(&myDatabaseContent);

        QSignalSpy trackHasChangedSpy(&myListener, &TracksListener::trackHasChanged);
        QSignalSpy trackHasBeenRemovedSpy(&myListener, &TracksListener::trackHasBeenRemoved);
        QSignalSpy tracksListAddedSpy(&myListener, &TracksListener::tracksListAdded);

        myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

        connect(&myDatabaseContent, &DatabaseInterface::trackRemoved, &myListener, &TracksListener::trackRemoved);
        connect(&myDatabaseContent, &DatabaseInterface::tracksAdded, &myListener, &TracksListener::tracksAdded);
        connect(&myDatabaseContent, &DatabaseInterface::trackModified, &myListener, &TracksListener::trackModified);
        connect(&myListener, &TracksListener::trackHasChanged, &myPlayList, &MediaPlayList::trackChanged);
        connect(&myListener, &TracksListener::trackHasBeenRemoved, &myPlayList, &MediaPlayList::trackRemoved);
        connect(&myListener, &TracksListener::tracksListAdded, &myPlayList, &MediaPlayList::tracksListAdded);
        connect(&myPlayList, &MediaPlayList::newEntryInList, &myListener, &TracksListener::newEntryInList);
        connect(&myPlayList, &MediaPlayList::newTrackByNameInList, &myListener, &TracksListener::trackByNameInList);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        myPlayList.enqueueRestoredEntries(QVariantList({QStringList({{}, QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("1"), QStringLiteral("1"), {}})}));

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.rowCount(), 1);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), false);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), -1);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 0);

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);

        myDatabaseContent.removeTracksList({QUrl::fromLocalFile(QStringLiteral("/$1"))});

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 1);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.rowCount(), 1);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), false);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), -1);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 0);
    }

    void testInsertTrackByNameModifyAndRemoval()
    {
        MediaPlayList myPlayList;
        DatabaseInterface myDatabaseContent;
        TracksListener myListener(&myDatabaseContent);

        QSignalSpy trackHasChangedSpy(&myListener, &TracksListener::trackHasChanged);
        QSignalSpy trackHasBeenRemovedSpy(&myListener, &TracksListener::trackHasBeenRemoved);
        QSignalSpy tracksListAddedSpy(&myListener, &TracksListener::tracksListAdded);

        myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

        connect(&myDatabaseContent, &DatabaseInterface::trackRemoved, &myListener, &TracksListener::trackRemoved);
        connect(&myDatabaseContent, &DatabaseInterface::tracksAdded, &myListener, &TracksListener::tracksAdded);
        connect(&myDatabaseContent, &DatabaseInterface::trackModified, &myListener, &TracksListener::trackModified);
        connect(&myListener, &TracksListener::trackHasChanged, &myPlayList, &MediaPlayList::trackChanged);
        connect(&myListener, &TracksListener::trackHasBeenRemoved, &myPlayList, &MediaPlayList::trackRemoved);
        connect(&myListener, &TracksListener::tracksListAdded, &myPlayList, &MediaPlayList::tracksListAdded);
        connect(&myPlayList, &MediaPlayList::newEntryInList, &myListener, &TracksListener::newEntryInList);
        connect(&myPlayList, &MediaPlayList::newTrackByNameInList, &myListener, &TracksListener::trackByNameInList);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        myPlayList.enqueueOneEntry({{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                     {DataTypes::DatabaseIdRole, myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1)}},
                                    QStringLiteral("track1"), {}});

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.rowCount(), 1);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);

        myDatabaseContent.insertTracksList({
                                               {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
                                                QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("Various Artists"),
                                                2, 3, QTime::fromMSecsSinceStartOfDay(1000), {QUrl::fromLocalFile(QStringLiteral("/$1"))},
                                                QDateTime::fromMSecsSinceEpoch(1),
                                                {QUrl::fromLocalFile(QStringLiteral("file://image$1"))}, 1, false,
                                                {}, {}, QStringLiteral("lyricist1"), false}
                                           }, mNewCovers);

        QCOMPARE(trackHasChangedSpy.count(), 2);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.rowCount(), 1);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 2);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 3);

        myDatabaseContent.removeTracksList({QUrl::fromLocalFile(QStringLiteral("/$1"))});

        QCOMPARE(trackHasChangedSpy.count(), 2);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 1);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.rowCount(), 1);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), false);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), -1);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 0);
    }
};

QTEST_GUILESS_MAIN(TracksListenerTests)


#include "trackslistenertest.moc"
