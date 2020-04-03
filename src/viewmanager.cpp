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

#include "viewmanager.h"

#include "datatypes.h"

class ViewManagerPrivate
{
public:

    QString mCurrentAlbumTitle;
    QString mCurrentAlbumAuthor;
    QString mCurrentArtistName;
    QString mCurrentGenreName;

    QString mTargetAlbumTitle;
    QString mTargetAlbumAuthor;
    QString mTargetArtistName;
    QString mTargetGenreName;
    QUrl mTargetImageUrl;
    qulonglong mTargetDatabaseId = 0;
    ViewManager::ViewsType mTargetView = ViewManager::Context;
    ViewManager::ViewsType mCurrentView = ViewManager::Context;
    ViewManager::AlbumViewStyle mAlbumDiscHeader = ViewManager::NoDiscHeaders;

};

ViewManager::ViewManager(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<ViewManagerPrivate>())
{
}

ViewManager::~ViewManager() = default;

void ViewManager::openParentView(ViewManager::ViewsType viewType, const QString &mainTitle, const QUrl &mainImage)
{
    switch(viewType)
    {
    case RecentlyPlayedTracks:
        openRecentlyPlayedTracks(mainTitle, mainImage);
        break;
    case FrequentlyPlayedTracks:
        openFrequentlyPlayedTracks(mainTitle, mainImage);
        break;
    case AllAlbums:
        openAllAlbums(mainTitle, mainImage);
        break;
    case AllArtists:
        openAllArtists(mainTitle, mainImage);
        break;
    case AllTracks:
        openAllTracks(mainTitle, mainImage);
        break;
    case AllGenres:
        openAllGenres(mainTitle, mainImage);
        break;
    case FilesBrowser:
        openFilesBrowser(mainTitle, mainImage);
        break;
    case Context:
        openContextView(mainTitle, mainImage);
        break;
    case RadiosBrowser:
        openRadiosBrowser(mainTitle, mainImage);
        break;
    case OneAlbum:
    case OneArtist:
    case OneAlbumFromArtist:
    case OneArtistFromGenre:
    case OneAlbumFromArtistAndGenre:
    case AllArtistsFromGenre:
        break;
    }
}

void ViewManager::openChildView(const QString &innerMainTitle, const QString &innerSecondaryTitle,
                                const QUrl &innerImage, qulonglong databaseId,
                                ElisaUtils::PlayListEntryType dataType, AlbumViewStyle albumDiscHeader)
{
    switch(dataType)
    {
    case ElisaUtils::Album:
        openOneAlbum(innerMainTitle, innerSecondaryTitle, innerImage, databaseId, albumDiscHeader);
        break;
    case ElisaUtils::Artist:
        openOneArtist(innerMainTitle, innerImage, databaseId);
        break;
    case ElisaUtils::Genre:
        openAllArtistsFromGenre(innerMainTitle);
        break;
    case ElisaUtils::Track:
    case ElisaUtils::FileName:
    case ElisaUtils::Lyricist:
    case ElisaUtils::Composer:
    case ElisaUtils::Radio:
    case ElisaUtils::Unknown:
        break;
    }
}

void ViewManager::viewIsLoaded(ViewManager::ViewsType viewType)
{
    switch (viewType)
    {
    case ViewsType::RecentlyPlayedTracks:
        recentlyPlayedTracksIsLoaded();
        break;
    case ViewsType::FrequentlyPlayedTracks:
        frequentlyPlayedTracksIsLoaded();
        break;
    case ViewsType::AllAlbums:
        allAlbumsViewIsLoaded();
        break;
    case ViewsType::OneAlbum:
        oneAlbumViewIsLoaded();
        break;
    case ViewsType::AllArtists:
        allArtistsViewIsLoaded();
        break;
    case ViewsType::OneArtist:
        oneArtistViewIsLoaded();
        break;
    case ViewsType::OneAlbumFromArtist:
        oneAlbumViewIsLoaded();
        break;
    case ViewsType::AllTracks:
        allTracksViewIsLoaded();
        break;
    case ViewsType::AllGenres:
        allGenresViewIsLoaded();
        break;
    case ViewsType::AllArtistsFromGenre:
        allArtistsFromGenreViewIsLoaded();
        break;
    case ViewsType::OneArtistFromGenre:
        oneArtistViewIsLoaded();
        break;
    case ViewsType::OneAlbumFromArtistAndGenre:
        oneAlbumViewIsLoaded();
        break;
    case ViewsType::FilesBrowser:
        filesBrowserViewIsLoaded();
        break;
    case ViewsType::Context:
        contextViewIsLoaded();
        break;
    case ViewsType::RadiosBrowser:
        radiosBrowserViewIsLoaded();
        break;
    }
}

void ViewManager::openRecentlyPlayedTracks(const QString &mainTitle, const QUrl &imageUrl)
{
    d->mTargetView = ViewsType::RecentlyPlayedTracks;

    if (d->mCurrentView != d->mTargetView) {
        Q_EMIT openListView(d->mTargetView, ElisaUtils::FilterByRecentlyPlayed, 1, mainTitle, {},
                            0, imageUrl, ElisaUtils::Track, DataTypes::LastPlayDate,
                            SortOrder::SortDescending, MultipleAlbum, NoDiscHeaders, IsTrack);
    }
}

void ViewManager::openFrequentlyPlayedTracks(const QString &mainTitle, const QUrl &imageUrl)
{
    d->mTargetView = ViewsType::FrequentlyPlayedTracks;

    if (d->mCurrentView != d->mTargetView) {
        Q_EMIT openListView(d->mTargetView, ElisaUtils::FilterByFrequentlyPlayed, 1, mainTitle, {},
                            0, imageUrl, ElisaUtils::Track, DataTypes::PlayFrequency,
                            SortOrder::SortDescending, MultipleAlbum, NoDiscHeaders, IsTrack);
    }
}

void ViewManager::openAllAlbums(const QString &mainTitle, const QUrl &imageUrl)
{
    d->mTargetView = ViewsType::AllAlbums;

    if (d->mCurrentView != d->mTargetView) {
        Q_EMIT openGridView(d->mTargetView, ElisaUtils::NoFilter, 1, mainTitle, {}, imageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, {}, ViewShowRating, DelegateWithSecondaryText);
    }
}

void ViewManager::openOneAlbum(const QString &albumTitle, const QString &albumAuthor,
                               const QUrl &albumCover, qulonglong albumDatabaseId,
                               AlbumViewStyle albumDiscHeader)
{
    d->mTargetAlbumTitle = albumTitle;
    d->mTargetAlbumAuthor = albumAuthor;
    d->mTargetDatabaseId = albumDatabaseId;
    d->mTargetImageUrl = albumCover;
    d->mAlbumDiscHeader = albumDiscHeader;

    if (d->mCurrentView == ViewsType::AllAlbums) {
        d->mTargetView = ViewsType::OneAlbum;
        Q_EMIT openListView(d->mTargetView, ElisaUtils::FilterById, 2, d->mTargetAlbumTitle, d->mTargetAlbumAuthor,
                            d->mTargetDatabaseId, d->mTargetImageUrl, ElisaUtils::Track, {},
                            SortOrder::NoSort, SingleAlbum, d->mAlbumDiscHeader, IsTrack);
    } else if (d->mCurrentView == ViewsType::OneArtist && d->mCurrentArtistName == d->mTargetAlbumAuthor) {
        d->mTargetView = ViewsType::OneAlbumFromArtist;
        Q_EMIT openListView(d->mTargetView, ElisaUtils::FilterById, 3, d->mTargetAlbumTitle, d->mTargetAlbumAuthor,
                            d->mTargetDatabaseId, d->mTargetImageUrl, ElisaUtils::Track, {},
                            SortOrder::NoSort, SingleAlbum, d->mAlbumDiscHeader, IsTrack);
    } else if (d->mCurrentView == ViewsType::OneArtist && d->mCurrentArtistName != d->mTargetAlbumAuthor) {
        d->mTargetView = ViewsType::OneAlbumFromArtist;
        Q_EMIT popOneView();
    } else if (d->mCurrentView == ViewsType::OneArtistFromGenre) {
        d->mTargetView = ViewsType::OneAlbumFromArtistAndGenre;
        Q_EMIT openListView(d->mTargetView, ElisaUtils::FilterById, 4, d->mTargetAlbumTitle, d->mTargetAlbumAuthor,
                            d->mTargetDatabaseId, d->mTargetImageUrl, ElisaUtils::Track, {},
                            SortOrder::NoSort, SingleAlbum, d->mAlbumDiscHeader, IsTrack);
    } else {
        d->mTargetView = ViewsType::OneAlbum;
        Q_EMIT openGridView(ViewsType::AllAlbums, ElisaUtils::NoFilter, 1, {}, {}, {}, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, {}, ViewShowRating, DelegateWithSecondaryText);
    }
}

void ViewManager::openAllArtists(const QString &mainTitle, const QUrl &imageUrl)
{
    d->mTargetView = ViewsType::AllArtists;

    if (d->mCurrentView != d->mTargetView) {
        Q_EMIT openGridView(ViewsType::AllArtists, ElisaUtils::NoFilter, 1, mainTitle, {}, imageUrl, ElisaUtils::Artist,
                            QUrl(QStringLiteral("image://icon/view-media-artist")), {}, {}, ViewHideRating, DelegateWithoutSecondaryText);
    }
}

void ViewManager::openOneArtist(const QString &artistName, const QUrl &artistImageUrl, qulonglong artistDatabaseId)
{
    d->mTargetArtistName = artistName;
    d->mTargetDatabaseId = artistDatabaseId;
    d->mTargetImageUrl = artistImageUrl;

    if (d->mCurrentView == ViewsType::AllArtistsFromGenre) {
        d->mTargetView = ViewsType::OneArtistFromGenre;
    } else {
        d->mTargetView = ViewsType::OneArtist;
    }

    if (d->mCurrentView == ViewsType::AllArtists && d->mTargetView == ViewsType::OneArtist) {
        Q_EMIT openGridView(d->mTargetView, ElisaUtils::FilterByArtist, 2, d->mTargetArtistName, {}, d->mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, d->mTargetArtistName, ViewShowRating, DelegateWithSecondaryText);
    } else if (d->mCurrentView == ViewsType::OneArtist && d->mCurrentArtistName != d->mTargetArtistName &&
               d->mTargetView == ViewsType::OneArtist) {
        Q_EMIT openGridView(d->mTargetView, ElisaUtils::FilterByArtist, 2, d->mTargetArtistName, {}, d->mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, d->mTargetArtistName, ViewShowRating, DelegateWithSecondaryText);
    } else if (d->mCurrentView == ViewsType::OneAlbumFromArtist && d->mCurrentArtistName != d->mTargetArtistName &&
               d->mTargetView == ViewsType::OneArtist) {
        Q_EMIT openGridView(d->mTargetView, ElisaUtils::FilterByArtist, 2, d->mTargetArtistName, {}, d->mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, d->mTargetArtistName, ViewShowRating, DelegateWithSecondaryText);
    } else if (d->mCurrentView == ViewsType::AllArtistsFromGenre && d->mTargetView == ViewsType::OneArtistFromGenre) {
        Q_EMIT openGridView(d->mTargetView, ElisaUtils::FilterByGenreAndArtist, 3, d->mTargetArtistName, {}, d->mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), d->mTargetGenreName, d->mTargetArtistName, ViewShowRating, DelegateWithSecondaryText);
    } else {
        Q_EMIT openGridView(ViewsType::AllArtists, ElisaUtils::NoFilter, 1, {}, {}, {}, ElisaUtils::Artist,
                            QUrl(QStringLiteral("image://icon/view-media-artist")), {}, {}, ViewHideRating, DelegateWithoutSecondaryText);
    }
}

void ViewManager::openAllTracks(const QString &mainTitle, const QUrl &imageUrl)
{
    d->mTargetView = ViewsType::AllTracks;
    if (d->mCurrentView != d->mTargetView) {
        Q_EMIT openListView(d->mTargetView, ElisaUtils::NoFilter, 1, mainTitle, {},
                            0, imageUrl, ElisaUtils::Track, Qt::DisplayRole,
                            SortOrder::SortAscending, MultipleAlbum, NoDiscHeaders, IsTrack);
    }
}

void ViewManager::openAllGenres(const QString &mainTitle, const QUrl &imageUrl)
{
    d->mTargetView = ViewsType::AllGenres;

    if (d->mCurrentView != d->mTargetView) {
        Q_EMIT openGridView(d->mTargetView, ElisaUtils::NoFilter, 1, mainTitle, {}, imageUrl, ElisaUtils::Genre,
                            QUrl(QStringLiteral("image://icon/view-media-genre")), {}, {}, ViewHideRating, DelegateWithoutSecondaryText);
    }
}

void ViewManager::openAllArtistsFromGenre(const QString &genreName)
{
    d->mTargetView = ViewsType::AllArtistsFromGenre;
    d->mTargetGenreName = genreName;

    if (d->mCurrentView == ViewsType::AllGenres) {
        Q_EMIT openGridView(d->mTargetView, ElisaUtils::FilterByGenre, 2, d->mTargetGenreName, {}, QUrl(QStringLiteral("image://icon/view-media-artist")),
                            ElisaUtils::Artist, QUrl(QStringLiteral("image://icon/view-media-artist")), d->mTargetGenreName, {}, ViewHideRating, DelegateWithoutSecondaryText);
    } else {
        Q_EMIT openGridView(ViewsType::AllGenres, ElisaUtils::NoFilter, 1, {}, {}, {}, ElisaUtils::Genre,
                            QUrl(QStringLiteral("image://icon/view-media-genre")), {}, {}, ViewHideRating, DelegateWithoutSecondaryText);
    }
}

void ViewManager::openFilesBrowser(const QString &mainTitle, const QUrl &imageUrl)
{
    d->mTargetView = ViewsType::FilesBrowser;
    if (d->mCurrentView != d->mTargetView) {
        Q_EMIT switchFilesBrowserView(d->mTargetView, 1, mainTitle, imageUrl);
    }
}

void ViewManager::openContextView(const QString &mainTitle, const QUrl &imageUrl)
{
    d->mTargetView = ViewsType::Context;
    if (d->mCurrentView != d->mTargetView) {
        Q_EMIT switchContextView(d->mTargetView, 1, mainTitle, imageUrl);
    }
}

void ViewManager::openRadiosBrowser(const QString &mainTitle, const QUrl &imageUrl)
{
    d->mTargetView = ViewsType::RadiosBrowser;
    if (d->mCurrentView != d->mTargetView) {
        Q_EMIT openListView(d->mTargetView, ElisaUtils::NoFilter, 1, mainTitle, {},
                            0, imageUrl, ElisaUtils::Radio, Qt::DisplayRole,
                            SortOrder::SortAscending, MultipleAlbum, NoDiscHeaders, IsRadio);
    }
}

void ViewManager::recentlyPlayedTracksIsLoaded()
{
    d->mCurrentView = ViewsType::RecentlyPlayedTracks;
}

void ViewManager::frequentlyPlayedTracksIsLoaded()
{
    d->mCurrentView = ViewsType::FrequentlyPlayedTracks;
}

void ViewManager::allAlbumsViewIsLoaded()
{
    d->mCurrentView = ViewsType::AllAlbums;
    if (d->mTargetView == ViewsType::OneAlbum) {
        Q_EMIT openListView(d->mTargetView, ElisaUtils::FilterById, 2, d->mTargetAlbumTitle, d->mTargetAlbumAuthor,
                            d->mTargetDatabaseId, d->mTargetImageUrl, ElisaUtils::Track, Qt::DisplayRole,
                            SortOrder::SortAscending, MultipleAlbum, NoDiscHeaders, IsTrack);
    }
}

void ViewManager::oneAlbumViewIsLoaded()
{
    d->mCurrentAlbumTitle = d->mTargetAlbumTitle;
    d->mCurrentAlbumAuthor = d->mTargetAlbumAuthor;

    if (d->mTargetView == ViewsType::OneAlbum) {
        d->mCurrentView = ViewsType::OneAlbum;
    } else if (d->mTargetView == ViewsType::OneAlbumFromArtist) {
        d->mCurrentView = ViewsType::OneAlbumFromArtist;
    } else if (d->mTargetView == ViewsType::OneAlbumFromArtistAndGenre) {
        d->mCurrentView = ViewsType::OneAlbumFromArtistAndGenre;
    }
}

void ViewManager::allArtistsViewIsLoaded()
{
    d->mCurrentView = ViewsType::AllArtists;
    if (d->mTargetView == ViewsType::OneArtist) {
        Q_EMIT openGridView(d->mTargetView, ElisaUtils::FilterByArtist, 2, d->mTargetArtistName, {}, d->mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, d->mTargetArtistName, ViewShowRating, DelegateWithSecondaryText);
    } else if (d->mTargetView == ViewsType::OneAlbumFromArtist) {
        Q_EMIT openGridView(ViewsType::OneArtist, ElisaUtils::FilterByArtist, 2, d->mTargetAlbumAuthor, {}, d->mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, d->mTargetAlbumAuthor, ViewShowRating, DelegateWithSecondaryText);
    }
}

void ViewManager::oneArtistViewIsLoaded()
{
    d->mCurrentArtistName = d->mTargetArtistName;
    if (d->mTargetView == ViewsType::OneArtist) {
        d->mCurrentView = ViewsType::OneArtist;
    } else if (d->mTargetView == ViewsType::OneArtistFromGenre) {
        d->mCurrentGenreName = d->mTargetGenreName;
        d->mCurrentView = ViewsType::OneArtistFromGenre;
    } else if (d->mTargetView == ViewsType::OneAlbumFromArtist) {
        d->mCurrentView = ViewsType::OneArtist;

        Q_EMIT openListView(d->mTargetView, ElisaUtils::FilterById, 3, d->mTargetAlbumTitle, d->mTargetAlbumAuthor,
                            d->mTargetDatabaseId, d->mTargetImageUrl, ElisaUtils::Track, Qt::DisplayRole,
                            SortOrder::SortAscending, MultipleAlbum, NoDiscHeaders, IsTrack);
    }
}

void ViewManager::allTracksViewIsLoaded()
{
    d->mCurrentView = ViewsType::AllTracks;
}

void ViewManager::allGenresViewIsLoaded()
{
    d->mCurrentView = ViewsType::AllGenres;
    if (d->mTargetView == ViewsType::AllArtistsFromGenre) {
        Q_EMIT openGridView(ViewsType::AllArtistsFromGenre, ElisaUtils::FilterByGenre, 1, {}, {}, {}, ElisaUtils::Artist,
                            QUrl(QStringLiteral("image://icon/view-media-artist")), d->mTargetGenreName, {}, ViewHideRating, DelegateWithoutSecondaryText);
    }
}

void ViewManager::allArtistsFromGenreViewIsLoaded()
{
    d->mCurrentGenreName = d->mTargetGenreName;
    d->mCurrentView = ViewsType::AllArtistsFromGenre;
}

void ViewManager::filesBrowserViewIsLoaded()
{
    d->mCurrentView = ViewsType::FilesBrowser;
}

void ViewManager::contextViewIsLoaded()
{
    d->mCurrentView = ViewsType::Context;
}

void ViewManager::radiosBrowserViewIsLoaded()
{
    d->mCurrentView = ViewsType::RadiosBrowser;
}

void ViewManager::goBack()
{
    Q_EMIT popOneView();

    if (d->mCurrentView == ViewsType::OneAlbum) {
        d->mCurrentView = ViewsType::AllAlbums;
    } else if (d->mCurrentView == ViewsType::OneArtist) {
        d->mCurrentView = ViewsType::AllArtists;
    } else if (d->mCurrentView == ViewsType::OneAlbumFromArtist) {
        d->mCurrentView = ViewsType::OneArtist;
    } else if (d->mCurrentView == ViewsType::AllArtistsFromGenre) {
        d->mCurrentView = ViewsType::AllGenres;
    } else if (d->mCurrentView == ViewsType::OneArtistFromGenre) {
        d->mCurrentView = ViewsType::AllArtistsFromGenre;
    } else if (d->mCurrentView == ViewsType::OneAlbumFromArtistAndGenre) {
        d->mCurrentView = ViewsType::OneArtistFromGenre;
    }
    d->mTargetView = d->mCurrentView;
}


#include "moc_viewmanager.cpp"
