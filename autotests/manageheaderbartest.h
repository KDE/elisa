/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef MANAGEHEADERBARTEST_H
#define MANAGEHEADERBARTEST_H

#include <QObject>

#include "databasetestdata.h"

#include "musicaudiotrack.h"

#include <QList>
#include <QHash>
#include <QString>
#include <QUrl>

class ManageHeaderBarTest : public QObject, public DatabaseTestData
{

    Q_OBJECT

public:

    enum ColumnsRoles {
        IsValidRole = Qt::UserRole + 1,
        TitleRole = IsValidRole + 1,
        DurationRole = TitleRole + 1,
        CreatorRole = DurationRole + 1,
        ArtistRole = CreatorRole + 1,
        AlbumRole = ArtistRole + 1,
        TrackNumberRole = AlbumRole + 1,
        RatingRole = TrackNumberRole + 1,
        ImageRole = RatingRole + 1,
        AlbumIdRole = ImageRole + 1,
        ResourceRole = AlbumIdRole + 1,
        CountRole = ResourceRole + 1,
        IsPlayingRole = CountRole + 1,
    };

    Q_ENUM(ColumnsRoles)

    explicit ManageHeaderBarTest(QObject *parent = nullptr);

Q_SIGNALS:

private Q_SLOTS:

    void initTestCase();

    void simpleInitialCase();

    void setCurrentTrackWithInvalidData();

    void setCurrentTrackWithData();

};

#endif // MANAGEHEADERBARTEST_H
