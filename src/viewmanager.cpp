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

#include "viewsLogging.h"

#include <KI18n/KLocalizedString>

class ViewParameters
{
public:
    ViewParameters() = default;

    ViewParameters(ViewManager::ViewsType viewType,
                   QString mainTitle,
                   QUrl mainImage,
                   ViewManager::ViewPresentationType viewPresentationType)
        : mMainTitle(std::move(mainTitle))
        , mMainImage(std::move(mainImage))
        , mViewType(viewType)
        , mViewPresentationType(viewPresentationType)
        , mIsValid(true)
    {
    }

    ViewParameters(ViewManager::ViewsType viewType,
                   QString mainTitle,
                   QUrl mainImage,
                   ViewManager::ViewPresentationType viewPresentationType,
                   ElisaUtils::FilterType filterType,
                   ElisaUtils::PlayListEntryType dataType,
                   QUrl fallbackItemIcon,
                   ViewManager::DelegateUseSecondaryText showSecondaryTextOnDelegates,
                   ViewManager::ViewCanBeRated viewCanBeRated)
        : mMainTitle(std::move(mainTitle))
        , mMainImage(std::move(mainImage))
        , mViewType(viewType)
        , mViewPresentationType(viewPresentationType)
        , mFilterType(filterType)
        , mDataType(dataType)
        , mFallbackItemIcon(fallbackItemIcon)
        , mShowSecondaryTextOnDelegates(showSecondaryTextOnDelegates)
        , mViewCanBeRated(viewCanBeRated)
        , mIsValid(true)
    {
    }

    ViewParameters(ViewManager::ViewsType viewType,
                   QString mainTitle,
                   QUrl mainImage,
                   ViewManager::ViewPresentationType viewPresentationType,
                   ElisaUtils::FilterType filterType,
                   ElisaUtils::PlayListEntryType dataType,
                   int sortRole,
                   ViewManager::SortOrder sortOrder,
                   ViewManager::AlbumCardinality albumCardinality,
                   ViewManager::AlbumViewStyle albumViewStyle,
                   ViewManager::RadioSpecificStyle radioSpecificStyle)
        : mMainTitle(std::move(mainTitle))
        , mMainImage(std::move(mainImage))
        , mViewType(viewType)
        , mViewPresentationType(viewPresentationType)
        , mFilterType(filterType)
        , mDataType(dataType)
        , mSortRole(sortRole)
        , mSortOrder(sortOrder)
        , mAlbumCardinality(albumCardinality)
        , mAlbumViewStyle(albumViewStyle)
        , mRadioSpecificStyle(radioSpecificStyle)
        , mIsValid(true)
    {
    }

    bool operator==(const ViewParameters &other) const {
        return mMainTitle == other.mMainTitle && mMainImage == other.mMainImage &&
                mSecondaryTitle == other.mSecondaryTitle && mViewType == other.mViewType &&
                mViewPresentationType == other.mViewPresentationType && mFilterType == other.mFilterType &&
                mDataType == other.mDataType && mFallbackItemIcon == other.mFallbackItemIcon &&
                mShowSecondaryTextOnDelegates == other.mShowSecondaryTextOnDelegates && mViewCanBeRated == other.mViewCanBeRated &&
                mSortRole == other.mSortRole && mSortOrder == other.mSortOrder &&
                mAlbumCardinality == other.mAlbumCardinality && mAlbumViewStyle == other.mAlbumViewStyle &&
                mRadioSpecificStyle == other.mRadioSpecificStyle && mDepth == other.mDepth &&
                mDatabaseIdFilter == other.mDatabaseIdFilter && mGenreNameFilter == other.mGenreNameFilter &&
                mArtistNameFilter == other.mArtistNameFilter;
    }

    bool operator!=(const ViewParameters &other) const {
        return mMainTitle != other.mMainTitle || mMainImage != other.mMainImage ||
                mSecondaryTitle != other.mSecondaryTitle || mViewType != other.mViewType ||
                mViewPresentationType != other.mViewPresentationType || mFilterType != other.mFilterType ||
                mDataType != other.mDataType || mFallbackItemIcon != other.mFallbackItemIcon ||
                mShowSecondaryTextOnDelegates != other.mShowSecondaryTextOnDelegates || mViewCanBeRated != other.mViewCanBeRated ||
                mSortRole != other.mSortRole || mSortOrder != other.mSortOrder ||
                mAlbumCardinality != other.mAlbumCardinality || mAlbumViewStyle != other.mAlbumViewStyle ||
                mRadioSpecificStyle != other.mRadioSpecificStyle || mDepth != other.mDepth ||
                mDatabaseIdFilter != other.mDatabaseIdFilter || mGenreNameFilter != other.mGenreNameFilter ||
                mArtistNameFilter != other.mArtistNameFilter;
    }

    QString mMainTitle;

    QString mSecondaryTitle;

    QUrl mMainImage;

    ViewManager::ViewsType mViewType = ViewManager::UnknownView;

    ViewManager::ViewPresentationType mViewPresentationType = ViewManager::UnknownViewPresentation;

    ElisaUtils::FilterType mFilterType = ElisaUtils::UnknownFilter;

    ElisaUtils::PlayListEntryType mDataType = ElisaUtils::Unknown;

    QUrl mFallbackItemIcon;

    ViewManager::DelegateUseSecondaryText mShowSecondaryTextOnDelegates = ViewManager::DelegateWithSecondaryText;

    ViewManager::ViewCanBeRated mViewCanBeRated = ViewManager::ViewHideRating;

    int mSortRole = Qt::DisplayRole;

    ViewManager::SortOrder mSortOrder = ViewManager::SortAscending;

    ViewManager::AlbumCardinality mAlbumCardinality = ViewManager::MultipleAlbum;

    ViewManager::AlbumViewStyle mAlbumViewStyle = ViewManager::NoDiscHeaders;

    ViewManager::RadioSpecificStyle mRadioSpecificStyle = ViewManager::IsTrack;

    int mDepth = 1;

    qulonglong mDatabaseIdFilter = 0;

    QString mGenreNameFilter;

    QString mArtistNameFilter;

    bool mIsValid = false;
};

class ViewManagerPrivate
{
public:
    QList<ViewParameters> mViewsParameters = {{ViewManager::Context,
                                               {i18nc("Title of the view of the playlist", "Now Playing")},
                                               QUrl{QStringLiteral("image://icon/view-media-lyrics")},
                                               ViewManager::ContextView},
                                              {ViewManager::RecentlyPlayedTracks,
                                               {i18nc("Title of the view of recently played tracks", "Recently Played")},
                                               QUrl{QStringLiteral("image://icon/media-playlist-play")},
                                               ViewManager::ListView,
                                               ElisaUtils::FilterByRecentlyPlayed,
                                               ElisaUtils::Track,
                                               DataTypes::LastPlayDate,
                                               ViewManager::SortDescending,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack},
                                              {ViewManager::FrequentlyPlayedTracks,
                                               {i18nc("Title of the view of frequently played tracks", "Frequently Played")},
                                               QUrl{QStringLiteral("image://icon/view-media-playcount")},
                                               ViewManager::ListView,
                                               ElisaUtils::FilterByFrequentlyPlayed,
                                               ElisaUtils::Track,
                                               DataTypes::PlayFrequency,
                                               ViewManager::SortDescending,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack},
                                              {ViewManager::AllAlbums,
                                               {i18nc("Title of the view of all albums", "Albums")},
                                               QUrl{QStringLiteral("image://icon/view-media-album-cover")},
                                               ViewManager::GridView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Album,
                                               QUrl{QStringLiteral("image://icon/media-optical-audio")},
                                               ViewManager::DelegateWithSecondaryText,
                                               ViewManager::ViewShowRating},
                                              {ViewManager::AllArtists,
                                               {i18nc("Title of the view of all artists", "Artists")},
                                               QUrl{QStringLiteral("image://icon/view-media-artist")},
                                               ViewManager::GridView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Artist,
                                               QUrl{QStringLiteral("image://icon/view-media-artist")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating},
                                              {ViewManager::AllTracks,
                                               {i18nc("Title of the view of all tracks", "Tracks")},
                                               QUrl{QStringLiteral("image://icon/view-media-track")},
                                               ViewManager::ListView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Track,
                                               Qt::DisplayRole,
                                               ViewManager::SortAscending,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack},
                                              {ViewManager::AllGenres,
                                               {i18nc("Title of the view of all genres", "Genres")},
                                               QUrl{QStringLiteral("image://icon/view-media-genre")},
                                               ViewManager::GridView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Genre,
                                               QUrl{QStringLiteral("image://icon/view-media-genre")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating},
                                              {ViewManager::FilesBrowser,
                                               {i18nc("Title of the file browser view", "Files")},
                                               QUrl{QStringLiteral("image://icon/document-open-folder")},
                                               ViewManager::FileBrowserView},
                                              {ViewManager::RadiosBrowser,
                                               {i18nc("Title of the file radios browser view", "Radios")},
                                               QUrl{QStringLiteral("image://icon/radio")},
                                               ViewManager::ListView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Radio,
                                               Qt::DisplayRole,
                                               ViewManager::SortAscending,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsRadio}};

    QMap<ViewManager::ViewsType, ViewParameters> mChildViews = {
        {ViewManager::OneAlbum, {ViewManager::OneAlbum,
                                 {},
                                 QUrl{QStringLiteral("image://icon/view-media-track")},
                                 ViewManager::ListView,
                                 ElisaUtils::FilterById,
                                 ElisaUtils::Track,
                                 Qt::DisplayRole,
                                 ViewManager::NoSort,
                                 ViewManager::SingleAlbum,
                                 ViewManager::DiscHeaders,
                                 ViewManager::IsTrack}},
        {ViewManager::OneAlbumFromArtistAndGenre, {ViewManager::OneAlbumFromArtistAndGenre,
                                                   {},
                                                   QUrl{QStringLiteral("image://icon/view-media-track")},
                                                   ViewManager::ListView,
                                                   ElisaUtils::FilterById,
                                                   ElisaUtils::Track,
                                                   Qt::DisplayRole,
                                                   ViewManager::NoSort,
                                                   ViewManager::SingleAlbum,
                                                   ViewManager::DiscHeaders,
                                                   ViewManager::IsTrack}},
        {ViewManager::OneAlbumFromArtist, {ViewManager::OneAlbumFromArtist,
                                           {},
                                           QUrl{QStringLiteral("image://icon/view-media-track")},
                                           ViewManager::ListView,
                                           ElisaUtils::FilterById,
                                           ElisaUtils::Track,
                                           Qt::DisplayRole,
                                           ViewManager::NoSort,
                                           ViewManager::SingleAlbum,
                                           ViewManager::DiscHeaders,
                                           ViewManager::IsTrack}},
        {ViewManager::AllArtistsFromGenre, {ViewManager::AllArtistsFromGenre,
                                            {},
                                            QUrl{QStringLiteral("image://icon/view-media-artist")},
                                            ViewManager::GridView,
                                            ElisaUtils::FilterByGenre,
                                            ElisaUtils::Artist,
                                            QUrl{QStringLiteral("image://icon/view-media-artist")},
                                            ViewManager::DelegateWithoutSecondaryText,
                                            ViewManager::ViewHideRating}},
        {ViewManager::OneArtist, {ViewManager::OneArtist,
                                  {},
                                  QUrl{QStringLiteral("image://icon/view-media-album-cover")},
                                  ViewManager::GridView,
                                  ElisaUtils::FilterByArtist,
                                  ElisaUtils::Album,
                                  QUrl{QStringLiteral("image://icon/media-optical-audio")},
                                  ViewManager::DelegateWithSecondaryText,
                                  ViewManager::ViewShowRating}},
        {ViewManager::OneArtistFromGenre, {ViewManager::OneArtistFromGenre,
                                           {},
                                           QUrl{QStringLiteral("image://icon/view-media-album-cover")},
                                           ViewManager::GridView,
                                           ElisaUtils::FilterByGenreAndArtist,
                                           ElisaUtils::Album,
                                           QUrl{QStringLiteral("image://icon/media-optical-audio")},
                                           ViewManager::DelegateWithSecondaryText,
                                           ViewManager::ViewShowRating}},
    };

    QMap<ViewManager::ViewsType, ViewManager::ViewsType> mViewsOrder = {
        {ViewManager::OneAlbum, ViewManager::AllAlbums},
        {ViewManager::OneArtist, ViewManager::AllArtists},
        {ViewManager::OneAlbumFromArtist, ViewManager::OneArtist},
        {ViewManager::AllArtistsFromGenre, ViewManager::AllGenres},
        {ViewManager::OneArtistFromGenre, ViewManager::AllArtistsFromGenre},
        {ViewManager::OneAlbumFromArtistAndGenre, ViewManager::OneArtistFromGenre},
    };

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

    QList<ViewParameters> mViewParametersStack = {mViewsParameters[0]};
    ViewParameters mNextViewParameters;
};

ViewManager::ViewManager(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<ViewManagerPrivate>())
{
}

ViewManager::~ViewManager() = default;

void ViewManager::openView(int viewIndex)
{
    const auto &viewParameters = d->mViewsParameters[viewIndex];

    d->mTargetView = viewParameters.mViewType;

    if (d->mCurrentView != d->mTargetView) {
        d->mViewParametersStack.clear();
        d->mNextViewParameters = viewParameters;
        openViewFromData(viewParameters);
    }
}

void ViewManager::openChildView(const QString &innerMainTitle, const QString & innerSecondaryTitle,
                                const QUrl &innerImage, qulonglong databaseId,
                                ElisaUtils::PlayListEntryType dataType)
{
    bool immediateOpening = true;

    switch(dataType)
    {
    case ElisaUtils::Album:
    {
        if (d->mViewParametersStack.back().mDataType != dataType) {
            openViewFromData(d->mViewsParameters[3]);
            immediateOpening = false;
        }
        if (d->mViewParametersStack.back().mFilterType == ElisaUtils::FilterByArtist && d->mViewParametersStack.back().mArtistNameFilter != innerSecondaryTitle) {
            d->mViewParametersStack.pop_back();

            auto nextViewType = ViewManager::OneArtist;
            switch (d->mViewParametersStack.back().mFilterType)
            {
            case ElisaUtils::FilterByGenre:
                nextViewType = ViewManager::OneArtistFromGenre;
                break;
            default:
                nextViewType = ViewManager::OneArtist;
                break;
            }

            auto newParentView = d->mChildViews[nextViewType];
            newParentView.mMainTitle = innerSecondaryTitle;
            newParentView.mArtistNameFilter = innerSecondaryTitle;
            newParentView.mDepth = d->mViewParametersStack.size() + 1;

            openViewFromData(newParentView);
        }

        auto newViewType = OneAlbum;
        if (d->mViewParametersStack.back().mFilterType == ElisaUtils::FilterByArtist) {
            newViewType = OneAlbumFromArtist;
        }
        d->mNextViewParameters = d->mChildViews[newViewType];
        d->mNextViewParameters.mMainTitle = innerMainTitle;
        d->mNextViewParameters.mSecondaryTitle = innerSecondaryTitle;
        d->mNextViewParameters.mMainImage = innerImage;
        d->mNextViewParameters.mDatabaseIdFilter = databaseId;
        d->mNextViewParameters.mDepth = d->mViewParametersStack.size() + 1;
        break;
    }
    case ElisaUtils::Artist:
    {
        if (d->mViewParametersStack.back().mDataType != dataType) {
            openViewFromData(d->mViewsParameters[4]);
            immediateOpening = false;
        }

        auto nextViewType = ViewManager::OneArtist;
        if(d->mViewParametersStack.back().mFilterType == ElisaUtils::FilterByGenre) {
            nextViewType = ViewManager::OneArtistFromGenre;
        }

        d->mNextViewParameters = d->mChildViews[nextViewType];
        d->mNextViewParameters.mMainTitle = innerMainTitle;
        d->mNextViewParameters.mMainImage = innerImage;
        d->mNextViewParameters.mDatabaseIdFilter = databaseId;
        d->mNextViewParameters.mArtistNameFilter = innerMainTitle;
        if(d->mViewParametersStack.back().mFilterType == ElisaUtils::FilterByGenre) {
            d->mNextViewParameters.mGenreNameFilter = d->mViewParametersStack.back().mGenreNameFilter;
        }
        d->mNextViewParameters.mDepth = d->mViewParametersStack.size() + 1;
        break;
    }
    case ElisaUtils::Genre:
        if (d->mViewParametersStack.back().mDataType != dataType) {
            openViewFromData(d->mViewsParameters[6]);
            immediateOpening = false;
        }

        d->mNextViewParameters = d->mChildViews[ViewManager::AllArtistsFromGenre];
        d->mNextViewParameters.mMainTitle = innerMainTitle;
        d->mNextViewParameters.mMainImage = innerImage;
        d->mNextViewParameters.mDatabaseIdFilter = databaseId;
        d->mNextViewParameters.mGenreNameFilter = innerMainTitle;
        d->mNextViewParameters.mDepth = d->mViewParametersStack.size() + 1;
        break;
    case ElisaUtils::Lyricist:
        break;
    case ElisaUtils::Composer:
        break;
    case ElisaUtils::Track:
    case ElisaUtils::FileName:
    case ElisaUtils::Radio:
    case ElisaUtils::Unknown:
        break;
    }

    if (immediateOpening) {
        openViewFromData(d->mNextViewParameters);
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
    case UnknownView:
        break;
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
    if (d->mNextViewParameters.mIsValid && d->mNextViewParameters != d->mViewParametersStack.back()) {
        openViewFromData(d->mNextViewParameters);
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
    if (d->mNextViewParameters.mIsValid && d->mNextViewParameters != d->mViewParametersStack.back()) {
        openViewFromData(d->mNextViewParameters);
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
    if (d->mNextViewParameters.mIsValid && d->mNextViewParameters != d->mViewParametersStack.back()) {
        openViewFromData(d->mNextViewParameters);
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

void ViewManager::openViewFromData(const ViewParameters &viewParamaters)
{
    d->mViewParametersStack.push_back(viewParamaters);
    switch (viewParamaters.mViewPresentationType)
    {
    case ViewPresentationType::GridView:
        qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << viewParamaters.mViewType << viewParamaters.mFilterType
                                    << viewParamaters.mDepth << viewParamaters.mMainTitle << viewParamaters.mSecondaryTitle
                                    << viewParamaters.mMainImage << viewParamaters.mDataType << viewParamaters.mFallbackItemIcon
                                    << viewParamaters.mGenreNameFilter << viewParamaters.mArtistNameFilter
                                    << viewParamaters.mViewCanBeRated << viewParamaters.mShowSecondaryTextOnDelegates;
        Q_EMIT openGridView(viewParamaters.mViewType, viewParamaters.mFilterType, viewParamaters.mDepth,
                            viewParamaters.mMainTitle, viewParamaters.mSecondaryTitle, viewParamaters.mMainImage,
                            viewParamaters.mDataType, viewParamaters.mFallbackItemIcon,
                            viewParamaters.mGenreNameFilter, viewParamaters.mArtistNameFilter,
                            viewParamaters.mViewCanBeRated, viewParamaters.mShowSecondaryTextOnDelegates);
        break;
    case ViewPresentationType::ListView:
        qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << viewParamaters.mViewType << viewParamaters.mFilterType
                                    << viewParamaters.mDepth << viewParamaters.mMainTitle << viewParamaters.mSecondaryTitle
                                    << viewParamaters.mDatabaseIdFilter << viewParamaters.mMainImage << viewParamaters.mDataType
                                    << viewParamaters.mSortRole << viewParamaters.mSortOrder << viewParamaters.mAlbumCardinality
                                    << viewParamaters.mAlbumViewStyle << viewParamaters.mRadioSpecificStyle;
        Q_EMIT openListView(viewParamaters.mViewType, viewParamaters.mFilterType, viewParamaters.mDepth, viewParamaters.mMainTitle, viewParamaters.mSecondaryTitle,
                            viewParamaters.mDatabaseIdFilter, viewParamaters.mMainImage, viewParamaters.mDataType, viewParamaters.mSortRole,
                            viewParamaters.mSortOrder, viewParamaters.mAlbumCardinality, viewParamaters.mAlbumViewStyle, viewParamaters.mRadioSpecificStyle);
        break;
    case ViewPresentationType::FileBrowserView:
        Q_EMIT switchFilesBrowserView(viewParamaters.mViewType, 1, viewParamaters.mMainTitle, viewParamaters.mMainImage);
        break;
    case ContextView:
        Q_EMIT switchContextView(viewParamaters.mViewType, 1, viewParamaters.mMainTitle, viewParamaters.mMainImage);
        break;
    case UnknownViewPresentation:
        break;
    }
}

void ViewManager::goBack()
{
    Q_EMIT popOneView();
    d->mViewParametersStack.pop_back();
    d->mNextViewParameters = {};
}


#include "moc_viewmanager.cpp"
