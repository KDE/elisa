/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MANAGEHEADERBARTEST_H
#define MANAGEHEADERBARTEST_H

#include <QObject>

#include "databasetestdata.h"

#include <QHash>

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
