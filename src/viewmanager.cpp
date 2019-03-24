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

ViewManager::ViewManager(QObject *parent) : QObject(parent)
{
}

void ViewManager::closeAllViews()
{
    mCurrentView = ViewsType::NoViews;
    mTargetView = ViewsType::NoViews;
    Q_EMIT switchOffAllViews(mTargetView);
}

void ViewManager::openParentView(ViewManager::ViewsType viewType, const QString &mainTitle, const QUrl &mainImage)
{
    switch(viewType)
    {
    case NoViews:
        closeAllViews();
        break;
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
                                ElisaUtils::PlayListEntryType dataType)
{
    switch(dataType)
    {
    case ElisaUtils::Album:
        openOneAlbum(innerMainTitle, innerSecondaryTitle, innerImage, databaseId);
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
    case ViewsType::NoViews:
        break;
    }
}

void ViewManager::openRecentlyPlayedTracks(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::RecentlyPlayedTracks;

    if (mCurrentView != mTargetView) {
        Q_EMIT switchRecentlyPlayedTracksView(mTargetView, 1, mainTitle, imageUrl, ElisaUtils::Track);
    }
}

void ViewManager::openFrequentlyPlayedTracks(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::FrequentlyPlayedTracks;

    if (mCurrentView != mTargetView) {
        Q_EMIT switchFrequentlyPlayedTracksView(mTargetView, 1, mainTitle, imageUrl, ElisaUtils::Track);
    }
}

void ViewManager::openAllAlbums(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::AllAlbums;

    if (mCurrentView != mTargetView) {
        Q_EMIT openGridView(mTargetView, 1, mainTitle, {}, imageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, {}, true, true);
    }
}

void ViewManager::openOneAlbum(const QString &albumTitle, const QString &albumAuthor,
                               const QUrl &albumCover, qulonglong albumDatabaseId)
{
    mTargetAlbumTitle = albumTitle;
    mTargetAlbumAuthor = albumAuthor;
    mTargetDatabaseId = albumDatabaseId;
    mTargetImageUrl = albumCover;

    if (mCurrentView == ViewsType::AllAlbums) {
        mTargetView = ViewsType::OneAlbum;
        Q_EMIT switchOneAlbumView(mTargetView, 2,
                                  mTargetAlbumTitle, mTargetImageUrl, mTargetAlbumAuthor, mTargetDatabaseId);
    } else if (mCurrentView == ViewsType::OneArtist) {
        mTargetView = ViewsType::OneAlbumFromArtist;
        Q_EMIT switchOneAlbumView(mTargetView, 3,
                                  mTargetAlbumTitle, mTargetImageUrl, mTargetAlbumAuthor, mTargetDatabaseId);
    } else if (mCurrentView == ViewsType::OneArtistFromGenre) {
        mTargetView = ViewsType::OneAlbumFromArtistAndGenre;
        Q_EMIT switchOneAlbumView(mTargetView, 4,
                                  mTargetAlbumTitle, mTargetImageUrl, mTargetAlbumAuthor, mTargetDatabaseId);
    } else {
        mTargetView = ViewsType::OneAlbum;
        Q_EMIT openGridView(ViewsType::AllAlbums, 1, {}, {}, {}, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, {}, true, true);
    }
}

void ViewManager::openAllArtists(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::AllArtists;

    if (mCurrentView != mTargetView) {
        Q_EMIT openGridView(ViewsType::AllArtists, 1, mainTitle, {}, imageUrl, ElisaUtils::Artist,
                            QUrl(QStringLiteral("image://icon/view-media-artist")), {}, {}, false, false);
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
        Q_EMIT openGridView(mTargetView, 2, mTargetArtistName, {}, mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, mTargetArtistName, true, true);
    } else if (mCurrentView == ViewsType::OneArtist && mCurrentArtistName != mTargetArtistName &&
               mTargetView == ViewsType::OneArtist) {
        Q_EMIT openGridView(mTargetView, 2, mTargetArtistName, {}, mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, mTargetArtistName, true, true);
    } else if (mCurrentView == ViewsType::OneAlbumFromArtist && mCurrentArtistName != mTargetArtistName &&
               mTargetView == ViewsType::OneArtist) {
        Q_EMIT openGridView(mTargetView, 2, mTargetArtistName, {}, mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, mTargetArtistName, true, true);
    } else if (mCurrentView == ViewsType::AllArtistsFromGenre && mTargetView == ViewsType::OneArtistFromGenre) {
        Q_EMIT openGridView(mTargetView, 3, mTargetArtistName, {}, mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), mTargetGenreName, mTargetArtistName, true, true);
    } else {
        Q_EMIT openGridView(ViewsType::AllArtists, 1, {}, {}, {}, ElisaUtils::Artist,
                            QUrl(QStringLiteral("image://icon/view-media-artist")), {}, {}, false, false);
    }
}

void ViewManager::openAllTracks(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::AllTracks;
    if (mCurrentView != mTargetView) {
        Q_EMIT switchAllTracksView(mTargetView, 1, mainTitle, imageUrl, ElisaUtils::Track);
    }
}

void ViewManager::openAllGenres(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::AllGenres;

    if (mCurrentView != mTargetView) {
        Q_EMIT openGridView(mTargetView, 1, mainTitle, {}, imageUrl, ElisaUtils::Genre,
                            QUrl(QStringLiteral("image://icon/view-media-genre")), {}, {}, false, false);
    }
}

void ViewManager::openAllArtistsFromGenre(const QString &genreName)
{
    mTargetView = ViewsType::AllArtistsFromGenre;
    mTargetGenreName = genreName;

    if (mCurrentView == ViewsType::AllGenres) {
        Q_EMIT openGridView(mTargetView, 2, mTargetGenreName, {}, QUrl(QStringLiteral("image://icon/view-media-artist")),
                            ElisaUtils::Artist, QUrl(QStringLiteral("image://icon/view-media-artist")), mTargetGenreName, {}, false, false);
    } else {
        Q_EMIT openGridView(ViewsType::AllGenres, 1, {}, {}, {}, ElisaUtils::Genre,
                            QUrl(QStringLiteral("image://icon/view-media-genre")), {}, {}, false, false);
    }
}

void ViewManager::openFilesBrowser(const QString &mainTitle, const QUrl &imageUrl)
{
    mTargetView = ViewsType::FilesBrowser;
    if (mCurrentView != mTargetView) {
        Q_EMIT switchFilesBrowserView(mTargetView, 1, mainTitle, imageUrl);
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
        Q_EMIT switchOneAlbumView(mTargetView, 2, mTargetAlbumTitle, mTargetImageUrl, mTargetArtistName, mTargetDatabaseId);
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
        Q_EMIT openGridView(mTargetView, 2, mTargetArtistName, {}, mTargetImageUrl, ElisaUtils::Album,
                            QUrl(QStringLiteral("image://icon/media-optical-audio")), {}, mTargetArtistName, true, true);
    }
}

void ViewManager::oneArtistViewIsLoaded()
{
    mCurrentArtistName = mTargetArtistName;
    if (mTargetView == ViewsType::OneArtist) {
        mCurrentView = ViewsType::OneArtist;
    } else {
        mCurrentGenreName = mTargetGenreName;
        mCurrentView = ViewsType::OneArtistFromGenre;
    }
}

void ViewManager::allTracksViewIsLoaded()
{
    mCurrentView = ViewsType::AllTracks;
}

void ViewManager::allGenresViewIsLoaded()
{
    mCurrentView = ViewsType::AllGenres;
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
