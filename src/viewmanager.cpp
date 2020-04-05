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

    ViewParameters(QString mainTitle,
                   QUrl mainImage,
                   ViewManager::ViewPresentationType viewPresentationType)
        : mMainTitle(std::move(mainTitle))
        , mMainImage(std::move(mainImage))
        , mViewPresentationType(viewPresentationType)
        , mIsValid(true)
    {
    }

    ViewParameters(QString mainTitle,
                   QUrl mainImage,
                   ViewManager::ViewPresentationType viewPresentationType,
                   ElisaUtils::FilterType filterType,
                   ElisaUtils::PlayListEntryType dataType,
                   QUrl fallbackItemIcon,
                   ViewManager::DelegateUseSecondaryText showSecondaryTextOnDelegates,
                   ViewManager::ViewCanBeRated viewCanBeRated)
        : mMainTitle(std::move(mainTitle))
        , mMainImage(std::move(mainImage))
        , mViewPresentationType(viewPresentationType)
        , mFilterType(filterType)
        , mDataType(dataType)
        , mFallbackItemIcon(fallbackItemIcon)
        , mShowSecondaryTextOnDelegates(showSecondaryTextOnDelegates)
        , mViewCanBeRated(viewCanBeRated)
        , mIsValid(true)
    {
    }

    ViewParameters(QString mainTitle,
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
                mSecondaryTitle == other.mSecondaryTitle && mViewPresentationType == other.mViewPresentationType &&
                mFilterType == other.mFilterType && mDataType == other.mDataType && mFallbackItemIcon == other.mFallbackItemIcon &&
                mShowSecondaryTextOnDelegates == other.mShowSecondaryTextOnDelegates && mViewCanBeRated == other.mViewCanBeRated &&
                mSortRole == other.mSortRole && mSortOrder == other.mSortOrder &&
                mAlbumCardinality == other.mAlbumCardinality && mAlbumViewStyle == other.mAlbumViewStyle &&
                mRadioSpecificStyle == other.mRadioSpecificStyle && mDepth == other.mDepth &&
                mDatabaseIdFilter == other.mDatabaseIdFilter && mGenreNameFilter == other.mGenreNameFilter &&
                mArtistNameFilter == other.mArtistNameFilter;
    }

    bool operator!=(const ViewParameters &other) const {
        return mMainTitle != other.mMainTitle || mMainImage != other.mMainImage ||
                mSecondaryTitle != other.mSecondaryTitle || mViewPresentationType != other.mViewPresentationType ||
                mFilterType != other.mFilterType || mDataType != other.mDataType || mFallbackItemIcon != other.mFallbackItemIcon ||
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
    QList<ViewParameters> mViewsParameters = {{{i18nc("Title of the view of the playlist", "Now Playing")},
                                               QUrl{QStringLiteral("image://icon/view-media-lyrics")},
                                               ViewManager::ContextView},
                                              {{i18nc("Title of the view of recently played tracks", "Recently Played")},
                                               QUrl{QStringLiteral("image://icon/media-playlist-play")},
                                               ViewManager::ListView,
                                               ElisaUtils::FilterByRecentlyPlayed,
                                               ElisaUtils::Track,
                                               DataTypes::LastPlayDate,
                                               ViewManager::SortDescending,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack},
                                              {{i18nc("Title of the view of frequently played tracks", "Frequently Played")},
                                               QUrl{QStringLiteral("image://icon/view-media-playcount")},
                                               ViewManager::ListView,
                                               ElisaUtils::FilterByFrequentlyPlayed,
                                               ElisaUtils::Track,
                                               DataTypes::PlayFrequency,
                                               ViewManager::SortDescending,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack},
                                              {{i18nc("Title of the view of all albums", "Albums")},
                                               QUrl{QStringLiteral("image://icon/view-media-album-cover")},
                                               ViewManager::GridView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Album,
                                               QUrl{QStringLiteral("image://icon/media-optical-audio")},
                                               ViewManager::DelegateWithSecondaryText,
                                               ViewManager::ViewShowRating},
                                              {{i18nc("Title of the view of all artists", "Artists")},
                                               QUrl{QStringLiteral("image://icon/view-media-artist")},
                                               ViewManager::GridView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Artist,
                                               QUrl{QStringLiteral("image://icon/view-media-artist")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating},
                                              {{i18nc("Title of the view of all tracks", "Tracks")},
                                               QUrl{QStringLiteral("image://icon/view-media-track")},
                                               ViewManager::ListView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Track,
                                               Qt::DisplayRole,
                                               ViewManager::SortAscending,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack},
                                              {{i18nc("Title of the view of all genres", "Genres")},
                                               QUrl{QStringLiteral("image://icon/view-media-genre")},
                                               ViewManager::GridView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Genre,
                                               QUrl{QStringLiteral("image://icon/view-media-genre")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating},
                                              {{i18nc("Title of the file browser view", "Files")},
                                               QUrl{QStringLiteral("image://icon/document-open-folder")},
                                               ViewManager::FileBrowserView},
                                              {{i18nc("Title of the file radios browser view", "Radios")},
                                               QUrl{QStringLiteral("image://icon/radio")},
                                               ViewManager::ListView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Radio,
                                               Qt::DisplayRole,
                                               ViewManager::SortAscending,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsRadio}};

    QMap<ElisaUtils::PlayListEntryType, ViewParameters> mChildViews = {
        {ElisaUtils::Album, {{},
                             QUrl{QStringLiteral("image://icon/view-media-track")},
                             ViewManager::ListView,
                             ElisaUtils::FilterById,
                             ElisaUtils::Track,
                             Qt::DisplayRole,
                             ViewManager::NoSort,
                             ViewManager::SingleAlbum,
                             ViewManager::DiscHeaders,
                             ViewManager::IsTrack}},
        {ElisaUtils::Genre, {{},
                             QUrl{QStringLiteral("image://icon/view-media-artist")},
                             ViewManager::GridView,
                             ElisaUtils::FilterByGenre,
                             ElisaUtils::Artist,
                             QUrl{QStringLiteral("image://icon/view-media-artist")},
                             ViewManager::DelegateWithoutSecondaryText,
                             ViewManager::ViewHideRating}},
        {ElisaUtils::Artist, {{},
                              QUrl{QStringLiteral("image://icon/view-media-album-cover")},
                              ViewManager::GridView,
                              ElisaUtils::FilterByArtist,
                              ElisaUtils::Album,
                              QUrl{QStringLiteral("image://icon/media-optical-audio")},
                              ViewManager::DelegateWithSecondaryText,
                              ViewManager::ViewShowRating}},
    };

    int mViewIndex = 0;

    QList<ViewParameters> mViewParametersStack = {mViewsParameters[mViewIndex]};

    ViewParameters mNextViewParameters;
};

ViewManager::ViewManager(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<ViewManagerPrivate>())
{
}

int ViewManager::viewIndex() const
{
    return d->mViewIndex;
}

ViewManager::~ViewManager() = default;

void ViewManager::openView(int viewIndex)
{
    qCDebug(orgKdeElisaViews()) << "ViewManager::openView" << viewIndex << d->mViewParametersStack.size();

    const auto &viewParameters = d->mViewsParameters[viewIndex];

    if (viewParameters != d->mViewParametersStack.back()) {
        d->mViewIndex = viewIndex;
        Q_EMIT viewIndexChanged();

        d->mNextViewParameters = viewParameters;
        openViewFromData(viewParameters);
    }
}

void ViewManager::openChildView(const QString &innerMainTitle, const QString & innerSecondaryTitle,
                                const QUrl &innerImage, qulonglong databaseId,
                                ElisaUtils::PlayListEntryType dataType)
{
    qCDebug(orgKdeElisaViews()) << "ViewManager::openChildView" << innerMainTitle << innerSecondaryTitle
                                << innerImage << databaseId << dataType << d->mViewParametersStack.size();

    const auto &lastView = d->mViewParametersStack.back();

    auto nextViewParameters = d->mChildViews[dataType];

    nextViewParameters.mMainTitle = innerMainTitle;
    nextViewParameters.mSecondaryTitle = innerSecondaryTitle;
    nextViewParameters.mMainImage = innerImage;
    nextViewParameters.mDepth = d->mViewParametersStack.size() + 1;

    if (lastView.mFilterType == ElisaUtils::FilterByGenre) {
        nextViewParameters.mFilterType = ElisaUtils::FilterByGenreAndArtist;
    }

    switch (nextViewParameters.mFilterType)
    {
    case ElisaUtils::NoFilter:
    case ElisaUtils::FilterByRecentlyPlayed:
    case ElisaUtils::FilterByFrequentlyPlayed:
    case ElisaUtils::UnknownFilter:
        break;
    case ElisaUtils::FilterById:
        nextViewParameters.mDatabaseIdFilter = databaseId;
        break;
    case ElisaUtils::FilterByGenre:
        nextViewParameters.mGenreNameFilter = innerMainTitle;
        break;
    case ElisaUtils::FilterByGenreAndArtist:
        nextViewParameters.mGenreNameFilter = lastView.mGenreNameFilter;
        nextViewParameters.mArtistNameFilter = innerMainTitle;
        break;
    case ElisaUtils::FilterByArtist:
        nextViewParameters.mArtistNameFilter = innerMainTitle;
        break;
    }

    d->mNextViewParameters = nextViewParameters;

    if (lastView.mDataType != dataType) {
        for(int i = 0; i < d->mViewsParameters.size(); ++i) {
            if (d->mViewsParameters.at(i).mDataType == dataType) {
                d->mViewIndex = i;
                Q_EMIT viewIndexChanged();

                nextViewParameters = d->mViewsParameters.at(i);
                break;
            }
        }
    }

    if (lastView.mFilterType == ElisaUtils::FilterByArtist && dataType == ElisaUtils::Album && lastView.mArtistNameFilter != innerSecondaryTitle) {
        nextViewParameters = lastView;
        nextViewParameters.mArtistNameFilter = innerSecondaryTitle;
    }

    openViewFromData(nextViewParameters);
}

void ViewManager::viewIsLoaded()
{
    qCDebug(orgKdeElisaViews()) << "ViewManager::viewIsLoaded" << d->mViewParametersStack.size();

    if (d->mNextViewParameters.mIsValid && d->mNextViewParameters != d->mViewParametersStack.back()) {
        openViewFromData(d->mNextViewParameters);
    }
}

void ViewManager::openViewFromData(const ViewParameters &viewParamaters)
{
    qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << d->mViewParametersStack.size();

    const auto viewsCountToRemove = d->mViewParametersStack.size() + 1 - viewParamaters.mDepth;
    for (int i = 0; i < viewsCountToRemove; ++i) {
        qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << "pop_back";
        d->mViewParametersStack.pop_back();
    }

    d->mViewParametersStack.push_back(viewParamaters);
    switch (viewParamaters.mViewPresentationType)
    {
    case ViewPresentationType::GridView:
        qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << viewParamaters.mViewPresentationType
                                    << viewParamaters.mFilterType
                                    << viewParamaters.mDepth << viewParamaters.mMainTitle << viewParamaters.mSecondaryTitle
                                    << viewParamaters.mMainImage << viewParamaters.mDataType << viewParamaters.mFallbackItemIcon
                                    << viewParamaters.mGenreNameFilter << viewParamaters.mArtistNameFilter
                                    << viewParamaters.mViewCanBeRated << viewParamaters.mShowSecondaryTextOnDelegates;
        Q_EMIT openGridView(viewParamaters.mFilterType, viewParamaters.mDepth,
                            viewParamaters.mMainTitle, viewParamaters.mSecondaryTitle, viewParamaters.mMainImage,
                            viewParamaters.mDataType, viewParamaters.mFallbackItemIcon,
                            viewParamaters.mGenreNameFilter, viewParamaters.mArtistNameFilter,
                            viewParamaters.mViewCanBeRated, viewParamaters.mShowSecondaryTextOnDelegates);
        break;
    case ViewPresentationType::ListView:
        qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << viewParamaters.mFilterType
                                    << viewParamaters.mDepth << viewParamaters.mMainTitle << viewParamaters.mSecondaryTitle
                                    << viewParamaters.mDatabaseIdFilter << viewParamaters.mMainImage << viewParamaters.mDataType
                                    << viewParamaters.mSortRole << viewParamaters.mSortOrder << viewParamaters.mAlbumCardinality
                                    << viewParamaters.mAlbumViewStyle << viewParamaters.mRadioSpecificStyle;
        Q_EMIT openListView(viewParamaters.mFilterType, viewParamaters.mDepth, viewParamaters.mMainTitle, viewParamaters.mSecondaryTitle,
                            viewParamaters.mDatabaseIdFilter, viewParamaters.mMainImage, viewParamaters.mDataType, viewParamaters.mSortRole,
                            viewParamaters.mSortOrder, viewParamaters.mAlbumCardinality, viewParamaters.mAlbumViewStyle, viewParamaters.mRadioSpecificStyle);
        break;
    case ViewPresentationType::FileBrowserView:
        qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << viewParamaters.mViewPresentationType
                                    << viewParamaters.mDepth << viewParamaters.mMainTitle
                                    << viewParamaters.mMainImage;
        Q_EMIT switchFilesBrowserView(viewParamaters.mDepth, viewParamaters.mMainTitle, viewParamaters.mMainImage);
        break;
    case ContextView:
        qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << viewParamaters.mViewPresentationType
                                    << viewParamaters.mDepth << viewParamaters.mMainTitle
                                    << viewParamaters.mMainImage;
        Q_EMIT switchContextView(viewParamaters.mDepth, viewParamaters.mMainTitle, viewParamaters.mMainImage);
        break;
    case UnknownViewPresentation:
        break;
    }
}

void ViewManager::goBack()
{
    qCDebug(orgKdeElisaViews()) << "ViewManager::goBack" << d->mViewParametersStack.size();

    Q_EMIT popOneView();
    d->mViewParametersStack.pop_back();
    d->mNextViewParameters = {};
}


#include "moc_viewmanager.cpp"
