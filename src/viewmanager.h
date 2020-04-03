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

#include "elisautils.h"

#include <QObject>
#include <QUrl>

#include <memory>

class ViewManagerPrivate;

class ELISALIB_EXPORT ViewManager : public QObject
{
    Q_OBJECT
public:

    enum ViewsType {
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
        FrequentlyPlayedTracks,
        RecentlyPlayedTracks,
        FilesBrowser,
        Context,
        RadiosBrowser
    };

    Q_ENUM(ViewsType)

    enum SortOrder {
        NoSort,
        SortAscending,
        SortDescending,
    };

    Q_ENUM(SortOrder)

    enum AlbumCardinality {
        SingleAlbum = true,
        MultipleAlbum = false,
    };

    Q_ENUM(AlbumCardinality)

    enum AlbumViewStyle {
        NoDiscHeaders,
        DiscHeaders,
    };

    Q_ENUM(AlbumViewStyle)

    enum RadioSpecificStyle {
        IsTrack,
        IsRadio,
    };

    Q_ENUM(RadioSpecificStyle)

    enum DelegateUseSecondaryText {
        DelegateWithSecondaryText = true,
        DelegateWithoutSecondaryText = false,
    };

    Q_ENUM(DelegateUseSecondaryText)

    enum ViewCanBeRated {
        ViewShowRating = true,
        ViewHideRating = false,
    };

    Q_ENUM(ViewCanBeRated)

    explicit ViewManager(QObject *parent = nullptr);

    ~ViewManager() override;

Q_SIGNALS:

    void openGridView(ViewManager::ViewsType viewType, ElisaUtils::FilterType filterType, int expectedDepth,
                      const QString &mainTitle, const QString &secondaryTitle, const QUrl &imageUrl,
                      ElisaUtils::PlayListEntryType dataType, const QUrl &viewDefaultIcon,
                      const QString &genreNameFilter, const QString &artistNameFilter,
                      ViewManager::ViewCanBeRated viewShowRating,
                      ViewManager::DelegateUseSecondaryText viewDelegateDisplaySecondaryText);

    void openListView(ViewManager::ViewsType viewType, ElisaUtils::FilterType filterType, int expectedDepth,
                      const QString &mainTitle, const QString &secondaryTitle, qulonglong databaseId,
                      const QUrl &imageUrl, ElisaUtils::PlayListEntryType dataType, const QVariant &sortRole,
                      ViewManager::SortOrder sortOrder, ViewManager::AlbumCardinality displaySingleAlbum,
                      ViewManager::AlbumViewStyle showDiscHeaders, ViewManager::RadioSpecificStyle radioCase);

    void switchFilesBrowserView(ViewManager::ViewsType viewType, int expectedDepth,
                                const QString &mainTitle, const QUrl &imageUrl);

    void switchContextView(ViewManager::ViewsType viewType, int expectedDepth,
                           const QString &mainTitle, const QUrl &imageUrl);

    void switchOffAllViews(ViewManager::ViewsType viewType);

    void popOneView();

public Q_SLOTS:

    void openView(int viewIndex);

    void openParentView(ViewManager::ViewsType viewType, const QString &mainTitle, const QUrl &mainImage);

    void openChildView(const QString &innerMainTitle, const QString & innerSecondaryTitle,
                       const QUrl &innerImage, qulonglong databaseId,
                       ElisaUtils::PlayListEntryType dataType, ViewManager::AlbumViewStyle albumDiscHeader);

    void viewIsLoaded(ViewManager::ViewsType viewType);

    void goBack();

private:

    void openRecentlyPlayedTracks(const QString &mainTitle, const QUrl &imageUrl);

    void openFrequentlyPlayedTracks(const QString &mainTitle, const QUrl &imageUrl);

    void openAllAlbums(const QString &mainTitle, const QUrl &imageUrl);

    void openAllArtists(const QString &mainTitle, const QUrl &imageUrl);

    void openAllTracks(const QString &mainTitle, const QUrl &imageUrl);

    void openAllGenres(const QString &mainTitle, const QUrl &imageUrl);

    void openFilesBrowser(const QString &mainTitle, const QUrl &imageUrl);

    void openContextView(const QString &mainTitle, const QUrl &imageUrl);

    void openRadiosBrowser(const QString &mainTitle, const QUrl &imageUrl);

    void openOneAlbum(const QString &albumTitle, const QString &albumAuthor,
                      const QUrl &albumCover, qulonglong albumDatabaseId, ViewManager::AlbumViewStyle albumDiscHeader);

    void openOneArtist(const QString &artistName, const QUrl &artistImageUrl, qulonglong artistDatabaseId);

    void openAllArtistsFromGenre(const QString &genreName);

    void recentlyPlayedTracksIsLoaded();

    void frequentlyPlayedTracksIsLoaded();

    void allAlbumsViewIsLoaded();

    void oneAlbumViewIsLoaded();

    void allArtistsViewIsLoaded();

    void oneArtistViewIsLoaded();

    void allTracksViewIsLoaded();

    void allGenresViewIsLoaded();

    void allArtistsFromGenreViewIsLoaded();

    void filesBrowserViewIsLoaded();

    void contextViewIsLoaded();

    void radiosBrowserViewIsLoaded();

private:

    std::unique_ptr<ViewManagerPrivate> d;

};

#endif // VIEWMANAGER_H
