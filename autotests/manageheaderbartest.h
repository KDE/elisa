/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef MANAGEHEADERBARTEST_H
#define MANAGEHEADERBARTEST_H

#include <QObject>

#include "musicaudiotrack.h"

#include <QList>
#include <QHash>
#include <QString>
#include <QUrl>

class ManageHeaderBarTest : public QObject
{

    Q_OBJECT

public:

    enum ColumnsRoles {
        IsValidRole = Qt::UserRole + 1,
        TitleRole = IsValidRole + 1,
        DurationRole = TitleRole + 1,
        MilliSecondsDurationRole = DurationRole + 1,
        CreatorRole = MilliSecondsDurationRole + 1,
        ArtistRole = CreatorRole + 1,
        AlbumRole = ArtistRole + 1,
        TrackNumberRole = AlbumRole + 1,
        RatingRole = TrackNumberRole + 1,
        ImageRole = RatingRole + 1,
        ResourceRole = ImageRole + 1,
        CountRole = ResourceRole + 1,
        IsPlayingRole = CountRole + 1,
    };

#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    Q_ENUM(ColumnsRoles)
#else
    Q_ENUMS(ColumnsRoles)
#endif

    explicit ManageHeaderBarTest(QObject *parent = nullptr);

Q_SIGNALS:

private Q_SLOTS:

    void initTestCase();

    void simpleInitialCase();

    void setCurrentTrackCase();

    void setCurrentTrackCaseFromNewAlbum();

    void completeCase();

    void setCurrentTrackAndInvalidCase();

    void setCurrentTrackAndRemoveItCase();

    void insertBeforeCurrentTrack();

    void moveCurrentTrack();

    void moveAnotherTrack();

    void setCurrentTrackTest();

    void setPlayListModelTest();

private:

    QList<MusicAudioTrack> mNewTracks = {
        {true, QStringLiteral("$1"), QStringLiteral("0"), QStringLiteral("track1"),
         QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"),
         1, 1, {}, {QUrl::fromLocalFile(QStringLiteral("$1"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$1"))}, 1, true},
        {true, QStringLiteral("$2"), QStringLiteral("0"), QStringLiteral("track2"),
         QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 2, 1,
         {}, {QUrl::fromLocalFile(QStringLiteral("$2"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$2"))}, 2, true},
        {true, QStringLiteral("$3"), QStringLiteral("0"), QStringLiteral("track3"),
         QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 3, 1,
         {}, {QUrl::fromLocalFile(QStringLiteral("$3"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$3"))}, 3, true},
        {true, QStringLiteral("$4"), QStringLiteral("0"), QStringLiteral("track4"),
         QStringLiteral("artist1"), QStringLiteral("album1"), QStringLiteral("artist1"), 4, 1,
         {}, {QUrl::fromLocalFile(QStringLiteral("$4"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$4"))}, 4, true},
        {true, QStringLiteral("$5"), QStringLiteral("0"), QStringLiteral("track1"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 1, 1,
         {}, {QUrl::fromLocalFile(QStringLiteral("$5"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$5"))}, 5, true},
        {true, QStringLiteral("$6"), QStringLiteral("0"), QStringLiteral("track2"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 2, 1,
         {}, {QUrl::fromLocalFile(QStringLiteral("$6"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$6"))}, 1, true},
        {true, QStringLiteral("$7"), QStringLiteral("0"), QStringLiteral("track3"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 3, 1,
         {}, {QUrl::fromLocalFile(QStringLiteral("$7"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$7"))}, 2, true},
        {true, QStringLiteral("$8"), QStringLiteral("0"), QStringLiteral("track4"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 4, 1,
         {}, {QUrl::fromLocalFile(QStringLiteral("$8"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$8"))}, 3, true},
        {true, QStringLiteral("$9"), QStringLiteral("0"), QStringLiteral("track5"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 5, 1,
         {}, {QUrl::fromLocalFile(QStringLiteral("$9"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$9"))}, 4, true},
        {true, QStringLiteral("$10"), QStringLiteral("0"), QStringLiteral("track6"),
         QStringLiteral("artist1"), QStringLiteral("album2"), QStringLiteral("artist1"), 6, 1,
         {}, {QUrl::fromLocalFile(QStringLiteral("$10"))},
         {QUrl::fromLocalFile(QStringLiteral("file://image$10"))}, 5, true}
    };

    QHash<QString, QUrl> mNewCovers = {
        {QStringLiteral("album1"), QUrl::fromLocalFile(QStringLiteral("album1"))},
        {QStringLiteral("album2"), QUrl::fromLocalFile(QStringLiteral("album2"))},
    };

};

#endif // MANAGEHEADERBARTEST_H
