/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include "elisaLib_export.h"

#include <QObject>
#include <QUrl>

class ELISALIB_EXPORT ViewManager : public QObject
{
    Q_OBJECT
public:

    enum ViewsType {
        NoViews,
        AllAlbums,
        OneAlbum,
        AllArtists,
        OneArtist,
        OneAlbumFromArtist,
        AllTracks,
        AllGenres,
        AllArtistsFromGenre,
        OneArtistFromGenre,
        OneAlbumFromArtistAndGenre,
        FilesBrowser
    };

    Q_ENUM(ViewsType)

    explicit ViewManager(QObject *parent = nullptr);

Q_SIGNALS:

    void switchAllAlbumsView();

    void switchOneAlbumView(const QString &mainTitle, const QUrl &imageUrl, const QString &secondaryTitle, qulonglong databaseId);

    void switchAllArtistsView();

    void switchOneArtistView(const QString &mainTitle, const QUrl &imageUrl, const QString &secondaryTitle, qulonglong databaseId);

    void switchAllTracksView();

    void switchAllGenresView();

    void switchAllArtistsFromGenreView(const QString &genreName);

    void switchOneArtistFromGenreView(const QString &mainTitle, const QUrl &imageUrl,
                                      const QString &secondaryTitle, qulonglong databaseId,
                                      const QString &genreName);

    void switchFilesBrowserView();

    void switchOffAllViews();

    void popOneView();

public Q_SLOTS:

    void closeAllViews();

    void openAllAlbums();

    void abstractOpenOneAlbum(const QString &albumTitle, const QString &albumAuthor,
                      const QUrl &albumCover, qulonglong albumDatabaseId);

    void openAllArtists();

    void abstractOpenOneArtist(const QString &artistName, const QUrl &artistImageUrl, qulonglong artistDatabaseId);

    void openAllTracks();

    void openAllGenres();

    void abstractOpenAllArtistsFromGenre(const QString &genreName);

    void openFilesBrowser();

    void abstractAllAlbumsViewIsLoaded();

    void oneAlbumViewIsLoaded();

    void abstractAllArtistsViewIsLoaded();

    void oneArtistViewIsLoaded();

    void allTracksViewIsLoaded();

    void abstractAllGenresViewIsLoaded();

    void allArtistsFromGenreViewIsLoaded();

    void filesBrowserViewIsLoaded();

    void goBack();

private:

    ViewsType mCurrentView = ViewsType::NoViews;
    QString mCurrentAlbumTitle;
    QString mCurrentAlbumAuthor;
    QString mCurrentArtistName;
    QString mCurrentGenreName;

    ViewsType mTargetView = ViewsType::NoViews;
    QString mTargetAlbumTitle;
    QString mTargetAlbumAuthor;
    QString mTargetArtistName;
    QString mTargetGenreName;
    QUrl mTargetImageUrl;
    qulonglong mTargetDatabaseId = 0;

};

#endif // VIEWMANAGER_H
