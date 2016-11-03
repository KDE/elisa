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

    explicit ManageHeaderBarTest(QObject *parent = 0);

Q_SIGNALS:

private Q_SLOTS:

    void initTestCase();

    void simpleInitialCase();

    void setCurrentTrackCase();

    void setCurrentTrackCaseFromNewAlbum();

    void completeCase();

    void setCurrentTrackAndInvalidCase();

    void setCurrentTrackAndRemoveItCase();

    void moveCurrentTrack();

    void moveAnotherTrack();

};

#endif // MANAGEHEADERBARTEST_H
