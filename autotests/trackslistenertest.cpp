/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

class TracksListenerTests: public QObject
{
    Q_OBJECT

public:

    TracksListenerTests(QObject *parent = nullptr) : QObject(parent)
    {
    }

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
        {QStringLiteral("album1"), QUrl::fromLocalFile(QStringLiteral("album1"))},
        {QStringLiteral("album2"), QUrl::fromLocalFile(QStringLiteral("album2"))},
        {QStringLiteral("album3"), QUrl::fromLocalFile(QStringLiteral("album3"))},
        {QStringLiteral("album4"), QUrl::fromLocalFile(QStringLiteral("album4"))},
    };

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QList<MusicAudioTrack>>("QList<MusicAudioTrack>");
        qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
        qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
        qRegisterMetaType<QList<QUrl>>("QList<QUrl>");
    }

    void testTrackRemoval()
    {
        MediaPlayList myPlayList;
        DatabaseInterface myDatabaseContent;
        TracksListener myListener(&myDatabaseContent);

        QSignalSpy trackHasChangedSpy(&myListener, &TracksListener::trackHasChanged);
        QSignalSpy trackHasBeenRemovedSpy(&myListener, &TracksListener::trackHasBeenRemoved);
        QSignalSpy albumAddedSpy(&myListener, &TracksListener::albumAdded);

        myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

        connect(&myDatabaseContent, &DatabaseInterface::trackRemoved, &myListener, &TracksListener::trackRemoved);
        connect(&myDatabaseContent, &DatabaseInterface::tracksAdded, &myListener, &TracksListener::tracksAdded);
        connect(&myDatabaseContent, &DatabaseInterface::trackModified, &myListener, &TracksListener::trackModified);
        connect(&myListener, &TracksListener::trackHasChanged, &myPlayList, &MediaPlayList::trackChanged);
        connect(&myListener, &TracksListener::trackHasBeenRemoved, &myPlayList, &MediaPlayList::trackRemoved);
        connect(&myListener, &TracksListener::albumAdded, &myPlayList, &MediaPlayList::albumAdded);
        connect(&myPlayList, &MediaPlayList::newTrackByIdInList, &myListener, &TracksListener::trackByIdInList);
        connect(&myPlayList, &MediaPlayList::newTrackByNameInList, &myListener, &TracksListener::trackByNameInList);
        connect(&myPlayList, &MediaPlayList::newArtistInList, &myListener, &TracksListener::newArtistInList);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

        myPlayList.replaceAndPlay(QStringLiteral("artist1"));

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 1);

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
        QCOMPARE(albumAddedSpy.count(), 1);

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
        QSignalSpy albumAddedSpy(&myListener, &TracksListener::albumAdded);

        myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

        connect(&myDatabaseContent, &DatabaseInterface::trackRemoved, &myListener, &TracksListener::trackRemoved);
        connect(&myDatabaseContent, &DatabaseInterface::tracksAdded, &myListener, &TracksListener::tracksAdded);
        connect(&myDatabaseContent, &DatabaseInterface::trackModified, &myListener, &TracksListener::trackModified);
        connect(&myListener, &TracksListener::trackHasChanged, &myPlayList, &MediaPlayList::trackChanged);
        connect(&myListener, &TracksListener::trackHasBeenRemoved, &myPlayList, &MediaPlayList::trackRemoved);
        connect(&myListener, &TracksListener::albumAdded, &myPlayList, &MediaPlayList::albumAdded);
        connect(&myPlayList, &MediaPlayList::newTrackByIdInList, &myListener, &TracksListener::trackByIdInList);
        connect(&myPlayList, &MediaPlayList::newTrackByNameInList, &myListener, &TracksListener::trackByNameInList);
        connect(&myPlayList, &MediaPlayList::newArtistInList, &myListener, &TracksListener::newArtistInList);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

        auto trackId = myDatabaseContent.trackIdFromTitleAlbumTrackDiscNumber(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                                              QStringLiteral("album1"), 1, 1);

        QCOMPARE(trackId != 0, true);

        myPlayList.replaceAndPlay(trackId);

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

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
        QCOMPARE(albumAddedSpy.count(), 0);

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
        QSignalSpy albumAddedSpy(&myListener, &TracksListener::albumAdded);

        myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

        connect(&myDatabaseContent, &DatabaseInterface::trackRemoved, &myListener, &TracksListener::trackRemoved);
        connect(&myDatabaseContent, &DatabaseInterface::tracksAdded, &myListener, &TracksListener::tracksAdded);
        connect(&myDatabaseContent, &DatabaseInterface::trackModified, &myListener, &TracksListener::trackModified);
        connect(&myListener, &TracksListener::trackHasChanged, &myPlayList, &MediaPlayList::trackChanged);
        connect(&myListener, &TracksListener::trackHasBeenRemoved, &myPlayList, &MediaPlayList::trackRemoved);
        connect(&myListener, &TracksListener::albumAdded, &myPlayList, &MediaPlayList::albumAdded);
        connect(&myPlayList, &MediaPlayList::newTrackByIdInList, &myListener, &TracksListener::trackByIdInList);
        connect(&myPlayList, &MediaPlayList::newTrackByNameInList, &myListener, &TracksListener::trackByNameInList);
        connect(&myPlayList, &MediaPlayList::newArtistInList, &myListener, &TracksListener::newArtistInList);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

        myPlayList.enqueue(MediaPlayListEntry(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                      QStringLiteral("album1"), 1, 1));

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

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
        QCOMPARE(albumAddedSpy.count(), 0);

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
        QSignalSpy albumAddedSpy(&myListener, &TracksListener::albumAdded);

        myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

        connect(&myDatabaseContent, &DatabaseInterface::trackRemoved, &myListener, &TracksListener::trackRemoved);
        connect(&myDatabaseContent, &DatabaseInterface::tracksAdded, &myListener, &TracksListener::tracksAdded);
        connect(&myDatabaseContent, &DatabaseInterface::trackModified, &myListener, &TracksListener::trackModified);
        connect(&myListener, &TracksListener::trackHasChanged, &myPlayList, &MediaPlayList::trackChanged);
        connect(&myListener, &TracksListener::trackHasBeenRemoved, &myPlayList, &MediaPlayList::trackRemoved);
        connect(&myListener, &TracksListener::albumAdded, &myPlayList, &MediaPlayList::albumAdded);
        connect(&myPlayList, &MediaPlayList::newTrackByIdInList, &myListener, &TracksListener::trackByIdInList);
        connect(&myPlayList, &MediaPlayList::newTrackByNameInList, &myListener, &TracksListener::trackByNameInList);
        connect(&myPlayList, &MediaPlayList::newArtistInList, &myListener, &TracksListener::newArtistInList);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

        myPlayList.enqueue(MediaPlayListEntry(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                      QStringLiteral("album1"), 1, 1));

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

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
        QCOMPARE(albumAddedSpy.count(), 0);

        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::IsValidRole).toBool(), true);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TitleRole).toString(), QStringLiteral("track1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::ArtistRole).toString(), QStringLiteral("artist1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::AlbumRole).toString(), QStringLiteral("album1"));
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::TrackNumberRole).toInt(), 1);
        QCOMPARE(myPlayList.data(myPlayList.index(0, 0), MediaPlayList::ColumnsRoles::DiscNumberRole).toInt(), 1);

        myDatabaseContent.removeTracksList({QUrl::fromLocalFile(QStringLiteral("/$1"))});

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 1);
        QCOMPARE(albumAddedSpy.count(), 0);

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
        QSignalSpy albumAddedSpy(&myListener, &TracksListener::albumAdded);

        myDatabaseContent.init(QStringLiteral("testDbDirectContent"));

        connect(&myDatabaseContent, &DatabaseInterface::trackRemoved, &myListener, &TracksListener::trackRemoved);
        connect(&myDatabaseContent, &DatabaseInterface::tracksAdded, &myListener, &TracksListener::tracksAdded);
        connect(&myDatabaseContent, &DatabaseInterface::trackModified, &myListener, &TracksListener::trackModified);
        connect(&myListener, &TracksListener::trackHasChanged, &myPlayList, &MediaPlayList::trackChanged);
        connect(&myListener, &TracksListener::trackHasBeenRemoved, &myPlayList, &MediaPlayList::trackRemoved);
        connect(&myListener, &TracksListener::albumAdded, &myPlayList, &MediaPlayList::albumAdded);
        connect(&myPlayList, &MediaPlayList::newTrackByIdInList, &myListener, &TracksListener::trackByIdInList);
        connect(&myPlayList, &MediaPlayList::newTrackByNameInList, &myListener, &TracksListener::trackByNameInList);
        connect(&myPlayList, &MediaPlayList::newArtistInList, &myListener, &TracksListener::newArtistInList);

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

        myDatabaseContent.insertTracksList(mNewTracks, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(trackHasChangedSpy.count(), 0);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

        myPlayList.enqueue(MediaPlayListEntry(QStringLiteral("track1"), QStringLiteral("artist1"),
                                                      QStringLiteral("album1"), 1, 1));

        QCOMPARE(trackHasChangedSpy.count(), 1);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

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
                                                {QUrl::fromLocalFile(QStringLiteral("file://image$1"))}, 1, false, {}, {}}
                                           }, mNewCovers, QStringLiteral("autoTest"));

        QCOMPARE(trackHasChangedSpy.count(), 2);
        QCOMPARE(trackHasBeenRemovedSpy.count(), 0);
        QCOMPARE(albumAddedSpy.count(), 0);

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
        QCOMPARE(albumAddedSpy.count(), 0);

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
