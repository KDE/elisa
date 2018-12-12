/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "databasetestdata.h"

#include "trackslistener.h"
#include "mediaplaylist.h"
#include "databaseinterface.h"

#include "config-upnp-qt.h"

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
        qRegisterMetaType<QList<MusicAudioTrack>>("QList<MusicAudioTrack>");
        qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
        qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
        qRegisterMetaType<QList<QUrl>>("QList<QUrl>");
        qRegisterMetaType<DatabaseInterface::ListTrackDataType>("ListTrackDataType");
        qRegisterMetaType<DatabaseInterface::ListAlbumDataType>("ListAlbumDataType");
        qRegisterMetaType<DatabaseInterface::ListArtistDataType>("ListArtistDataType");
        qRegisterMetaType<DatabaseInterface::ListGenreDataType>("ListGenreDataType");
        qRegisterMetaType<DatabaseInterface::TrackDataType>("TrackDataType");
        qRegisterMetaType<DatabaseInterface::AlbumDataType>("AlbumDataType");
        qRegisterMetaType<DatabaseInterface::ArtistDataType>("ArtistDataType");
        qRegisterMetaType<DatabaseInterface::GenreDataType>("GenreDataType");
        qRegisterMetaType<ElisaUtils::PlayListEntryType>("PlayListEntryType");
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

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        myPlayList.replaceAndPlay({0, QStringLiteral("artist1")}, ElisaUtils::Artist);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 1);

        QCOMPARE(myPlayList.tracksCount(), 6);

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

        QCOMPARE(myPlayList.tracksCount(), 6);

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

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        auto trackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                              QStringLiteral("album1"), 1, 1);

        QCOMPARE(trackId != 0, true);

        myPlayList.replaceAndPlay({trackId, {}}, ElisaUtils::Track);

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.tracksCount(), 1);

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

        QCOMPARE(myPlayList.tracksCount(), 1);

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

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1),
                            QStringLiteral("track1")},
                           ElisaUtils::Track);

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.tracksCount(), 1);

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

        QCOMPARE(myPlayList.tracksCount(), 1);

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

        myPlayList.enqueueRestoredEntry({QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1});

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.tracksCount(), 1);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), false);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), -1);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 0);

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

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

        QCOMPARE(myPlayList.tracksCount(), 1);

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

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        myPlayList.enqueue({myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"), QStringLiteral("album1"), 1, 1),
                            QStringLiteral("track1")},
                           ElisaUtils::Track);

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.tracksCount(), 1);

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
                                           }, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(trackHasChangedSpy.count(), 2);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(tracksListAddedSpy.count(), 0);

        QCOMPARE(myPlayList.tracksCount(), 1);

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

        QCOMPARE(myPlayList.tracksCount(), 1);

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
