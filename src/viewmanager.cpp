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

ViewManager::ViewManager(QObject *parent) : QObject(parent)
{
}

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
    mTargetView = ViewsType::RecentlyPlayedTracks;

    if (mCurrentView != mTargetView) {
        Q_EMIT openListView(mTargetView, ElisaUtils::FilterByRecentlyPlayed, 1, mainTitle, {},
                            0, imageUrl, ElisaUtils::Track, DataTypes::LastPlayDate,
                            SortOrder::SortDescending, MultipleAlbum, NoDiscHeaders, IsTrack);
    }
}

void ViewManager::openFrequentlyPlayedTracks(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::FrequentlyPlayedTracks;

    if (mCurrentView != mTargetView) {
        Q_EMIT openListView(mTargetView, ElisaUtils::FilterByFrequentlyPlayed, 1, mainTitle, {},
                            0, imageUrl, ElisaUtils::Track, DataTypes::PlayFrequency,
                            SortOrder::SortDescending, MultipleAlbum, NoDiscHeaders, IsTrack);
    }
}

void ViewManager::openAllAlbums(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::AllAlbums;

    if (mCurrentView != mTargetView) {
        Q_EMIT openGridView(mTargetView, ElisaUtils::NoFilter, 1, mainTitle, {}, imageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, {}, ViewShowRating, DelegateWithSecondaryText);
    }
}

void ViewManager::openOneAlbum(const QString &albumTitle, const QString &albumAuthor,
                               const QUrl &albumCover, qulonglong albumDatabaseId,
                               AlbumViewStyle albumDiscHeader)
{
    mTargetAlbumTitle = albumTitle;
    mTargetAlbumAuthor = albumAuthor;
    mTargetDatabaseId = albumDatabaseId;
    mTargetImageUrl = albumCover;
    mAlbumDiscHeader = albumDiscHeader;

    if (mCurrentView == ViewsType::AllAlbums) {
        mTargetView = ViewsType::OneAlbum;
        Q_EMIT openListView(mTargetView, ElisaUtils::FilterById, 2, mTargetAlbumTitle, mTargetAlbumAuthor,
                            mTargetDatabaseId, mTargetImageUrl, ElisaUtils::Track, {},
                            SortOrder::NoSort, SingleAlbum, mAlbumDiscHeader, IsTrack);
    } else if (mCurrentView == ViewsType::OneArtist && mCurrentArtistName == mTargetAlbumAuthor) {
        mTargetView = ViewsType::OneAlbumFromArtist;
        Q_EMIT openListView(mTargetView, ElisaUtils::FilterById, 3, mTargetAlbumTitle, mTargetAlbumAuthor,
                            mTargetDatabaseId, mTargetImageUrl, ElisaUtils::Track, {},
                            SortOrder::NoSort, SingleAlbum, mAlbumDiscHeader, IsTrack);
    } else if (mCurrentView == ViewsType::OneArtist && mCurrentArtistName != mTargetAlbumAuthor) {
        mTargetView = ViewsType::OneAlbumFromArtist;
        Q_EMIT popOneView();
    } else if (mCurrentView == ViewsType::OneArtistFromGenre) {
        mTargetView = ViewsType::OneAlbumFromArtistAndGenre;
        Q_EMIT openListView(mTargetView, ElisaUtils::FilterById, 4, mTargetAlbumTitle, mTargetAlbumAuthor,
                            mTargetDatabaseId, mTargetImageUrl, ElisaUtils::Track, {},
                            SortOrder::NoSort, SingleAlbum, mAlbumDiscHeader, IsTrack);
    } else {
        mTargetView = ViewsType::OneAlbum;
        Q_EMIT openGridView(ViewsType::AllAlbums, ElisaUtils::NoFilter, 1, {}, {}, {}, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, {}, ViewShowRating, DelegateWithSecondaryText);
    }
}

void ViewManager::openAllArtists(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::AllArtists;

    if (mCurrentView != mTargetView) {
        Q_EMIT openGridView(ViewsType::AllArtists, ElisaUtils::NoFilter, 1, mainTitle, {}, imageUrl, ElisaUtils::Artist,
                            QUrl(QStringLiteral("image://icon/view-media-artist")), {}, {}, ViewHideRating, DelegateWithoutSecondaryText);
    }
}

void ViewManager::openOneArtist(const QString &artistName, const QUrl &artistImageUrl, qulonglong artistDatabaseId)
{
    mTargetArtistName = artistName;
    mTargetDatabaseId = artistDatabaseId;
    mTargetImageUrl = artistImageUrl;

    if (mCurrentView == ViewsType::AllArtistsFromGenre) {
        mTargetView = ViewsType::OneArtistFromGenre;
    } else {
        mTargetView = ViewsType::OneArtist;
    }

    if (mCurrentView == ViewsType::AllArtists && mTargetView == ViewsType::OneArtist) {
        Q_EMIT openGridView(mTargetView, ElisaUtils::FilterByArtist, 2, mTargetArtistName, {}, mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, mTargetArtistName, ViewShowRating, DelegateWithSecondaryText);
    } else if (mCurrentView == ViewsType::OneArtist && mCurrentArtistName != mTargetArtistName &&
               mTargetView == ViewsType::OneArtist) {
        Q_EMIT openGridView(mTargetView, ElisaUtils::FilterByArtist, 2, mTargetArtistName, {}, mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, mTargetArtistName, ViewShowRating, DelegateWithSecondaryText);
    } else if (mCurrentView == ViewsType::OneAlbumFromArtist && mCurrentArtistName != mTargetArtistName &&
               mTargetView == ViewsType::OneArtist) {
        Q_EMIT openGridView(mTargetView, ElisaUtils::FilterByArtist, 2, mTargetArtistName, {}, mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, mTargetArtistName, ViewShowRating, DelegateWithSecondaryText);
    } else if (mCurrentView == ViewsType::AllArtistsFromGenre && mTargetView == ViewsType::OneArtistFromGenre) {
        Q_EMIT openGridView(mTargetView, ElisaUtils::FilterByGenreAndArtist, 3, mTargetArtistName, {}, mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), mTargetGenreName, mTargetArtistName, ViewShowRating, DelegateWithSecondaryText);
    } else {
        Q_EMIT openGridView(ViewsType::AllArtists, ElisaUtils::NoFilter, 1, {}, {}, {}, ElisaUtils::Artist,
                            QUrl(QStringLiteral("image://icon/view-media-artist")), {}, {}, ViewHideRating, DelegateWithoutSecondaryText);
    }
}

void ViewManager::openAllTracks(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::AllTracks;
    if (mCurrentView != mTargetView) {
        Q_EMIT openListView(mTargetView, ElisaUtils::NoFilter, 1, mainTitle, {},
                            0, imageUrl, ElisaUtils::Track, Qt::DisplayRole,
                            SortOrder::SortAscending, MultipleAlbum, NoDiscHeaders, IsTrack);
    }
}

void ViewManager::openAllGenres(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::AllGenres;

    if (mCurrentView != mTargetView) {
        Q_EMIT openGridView(mTargetView, ElisaUtils::NoFilter, 1, mainTitle, {}, imageUrl, ElisaUtils::Genre,
                            QUrl(QStringLiteral("image://icon/view-media-genre")), {}, {}, ViewHideRating, DelegateWithoutSecondaryText);
    }
}

void ViewManager::openAllArtistsFromGenre(const QString &genreName)
{
    mTargetView = ViewsType::AllArtistsFromGenre;
    mTargetGenreName = genreName;

    if (mCurrentView == ViewsType::AllGenres) {
        Q_EMIT openGridView(mTargetView, ElisaUtils::FilterByGenre, 2, mTargetGenreName, {}, QUrl(QStringLiteral("image://icon/view-media-artist")),
                            ElisaUtils::Artist, QUrl(QStringLiteral("image://icon/view-media-artist")), mTargetGenreName, {}, ViewHideRating, DelegateWithoutSecondaryText);
    } else {
        Q_EMIT openGridView(ViewsType::AllGenres, ElisaUtils::NoFilter, 1, {}, {}, {}, ElisaUtils::Genre,
                            QUrl(QStringLiteral("image://icon/view-media-genre")), {}, {}, ViewHideRating, DelegateWithoutSecondaryText);
    }
}

void ViewManager::openFilesBrowser(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::FilesBrowser;
    if (mCurrentView != mTargetView) {
        Q_EMIT switchFilesBrowserView(mTargetView, 1, mainTitle, imageUrl);
    }
}

void ViewManager::openContextView(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::Context;
    if (mCurrentView != mTargetView) {
        Q_EMIT switchContextView(mTargetView, 1, mainTitle, imageUrl);
    }
}

void ViewManager::openRadiosBrowser(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::RadiosBrowser;
    if (mCurrentView != mTargetView) {
        Q_EMIT openListView(mTargetView, ElisaUtils::NoFilter, 1, mainTitle, {},
                            0, imageUrl, ElisaUtils::Radio, Qt::DisplayRole,
                            SortOrder::SortAscending, MultipleAlbum, NoDiscHeaders, IsRadio);
    }
}

void ViewManager::recentlyPlayedTracksIsLoaded()
{
    mCurrentView = ViewsType::RecentlyPlayedTracks;
}

void ViewManager::frequentlyPlayedTracksIsLoaded()
{
    mCurrentView = ViewsType::FrequentlyPlayedTracks;
}

void ViewManager::allAlbumsViewIsLoaded()
{
    mCurrentView = ViewsType::AllAlbums;
    if (mTargetView == ViewsType::OneAlbum) {
        Q_EMIT openListView(mTargetView, ElisaUtils::FilterById, 2, mTargetAlbumTitle, mTargetAlbumAuthor,
                            mTargetDatabaseId, mTargetImageUrl, ElisaUtils::Track, Qt::DisplayRole,
                            SortOrder::SortAscending, MultipleAlbum, NoDiscHeaders, IsTrack);
    }
}

void ViewManager::oneAlbumViewIsLoaded()
{
    mCurrentAlbumTitle = mTargetAlbumTitle;
    mCurrentAlbumAuthor = mTargetAlbumAuthor;

    if (mTargetView == ViewsType::OneAlbum) {
        mCurrentView = ViewsType::OneAlbum;
    } else if (mTargetView == ViewsType::OneAlbumFromArtist) {
        mCurrentView = ViewsType::OneAlbumFromArtist;
    } else if (mTargetView == ViewsType::OneAlbumFromArtistAndGenre) {
        mCurrentView = ViewsType::OneAlbumFromArtistAndGenre;
    }
}

void ViewManager::allArtistsViewIsLoaded()
{
    mCurrentView = ViewsType::AllArtists;
    if (mTargetView == ViewsType::OneArtist) {
        Q_EMIT openGridView(mTargetView, ElisaUtils::FilterByArtist, 2, mTargetArtistName, {}, mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, mTargetArtistName, ViewShowRating, DelegateWithSecondaryText);
    } else if (mTargetView == ViewsType::OneAlbumFromArtist) {
        Q_EMIT openGridView(ViewsType::OneArtist, ElisaUtils::FilterByArtist, 2, mTargetAlbumAuthor, {}, mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, mTargetAlbumAuthor, ViewShowRating, DelegateWithSecondaryText);
    }
}

void ViewManager::oneArtistViewIsLoaded()
{
    mCurrentArtistName = mTargetArtistName;
    if (mTargetView == ViewsType::OneArtist) {
        mCurrentView = ViewsType::OneArtist;
    } else if (mTargetView == ViewsType::OneArtistFromGenre) {
        mCurrentGenreName = mTargetGenreName;
        mCurrentView = ViewsType::OneArtistFromGenre;
    } else if (mTargetView == ViewsType::OneAlbumFromArtist) {
        mCurrentView = ViewsType::OneArtist;

        Q_EMIT openListView(mTargetView, ElisaUtils::FilterById, 3, mTargetAlbumTitle, mTargetAlbumAuthor,
                            mTargetDatabaseId, mTargetImageUrl, ElisaUtils::Track, Qt::DisplayRole,
                            SortOrder::SortAscending, MultipleAlbum, NoDiscHeaders, IsTrack);
    }
}

void ViewManager::allTracksViewIsLoaded()
{
    mCurrentView = ViewsType::AllTracks;
}

void ViewManager::allGenresViewIsLoaded()
{
    mCurrentView = ViewsType::AllGenres;
    if (mTargetView == ViewsType::AllArtistsFromGenre) {
        Q_EMIT openGridView(ViewsType::AllArtistsFromGenre, ElisaUtils::FilterByGenre, 1, {}, {}, {}, ElisaUtils::Artist,
                            QUrl(QStringLiteral("image://icon/view-media-artist")), mTargetGenreName, {}, ViewHideRating, DelegateWithoutSecondaryText);
    }
}

void ViewManager::allArtistsFromGenreViewIsLoaded()
{
    mCurrentGenreName = mTargetGenreName;
    mCurrentView = ViewsType::AllArtistsFromGenre;
}

void ViewManager::filesBrowserViewIsLoaded()
{
    mCurrentView = ViewsType::FilesBrowser;
}

void ViewManager::contextViewIsLoaded()
{
    mCurrentView = ViewsType::Context;
}

void ViewManager::radiosBrowserViewIsLoaded()
{
    mCurrentView = ViewsType::RadiosBrowser;
}

void ViewManager::goBack()
{
    Q_EMIT popOneView();

    if (mCurrentView == ViewsType::OneAlbum) {
        mCurrentView = ViewsType::AllAlbums;
    } else if (mCurrentView == ViewsType::OneArtist) {
        mCurrentView = ViewsType::AllArtists;
    } else if (mCurrentView == ViewsType::OneAlbumFromArtist) {
        mCurrentView = ViewsType::OneArtist;
    } else if (mCurrentView == ViewsType::AllArtistsFromGenre) {
        mCurrentView = ViewsType::AllGenres;
    } else if (mCurrentView == ViewsType::OneArtistFromGenre) {
        mCurrentView = ViewsType::AllArtistsFromGenre;
    } else if (mCurrentView == ViewsType::OneAlbumFromArtistAndGenre) {
        mCurrentView = ViewsType::OneArtistFromGenre;
    }
    mTargetView = mCurrentView;
}


#include "moc_viewmanager.cpp"
