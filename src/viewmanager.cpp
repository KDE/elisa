/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "viewmanager.h"

#include "viewslistdata.h"
#include "datatypes.h"
#include "viewsLogging.h"

#include <KI18n/KLocalizedString>

class ViewManagerPrivate
{
public:
    ViewsListData *mViewsListData = nullptr;

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

    QList<ViewParameters> mViewParametersStack = (mViewsListData ? QList<ViewParameters>{mViewsListData->viewParameters(0)} : QList<ViewParameters>{});

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

ViewsListData *ViewManager::viewsData() const
{
    return d->mViewsListData;
}

ViewManager::~ViewManager() = default;

void ViewManager::openView(int viewIndex)
{
    if (!d->mViewsListData) {
        return;
    }

    if (!d->mViewParametersStack.size()) {
        return;
    }

    qCDebug(orgKdeElisaViews()) << "ViewManager::openView" << viewIndex << d->mViewParametersStack.size();

    const auto &viewParameters = d->mViewsListData->viewParameters(viewIndex);

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

    if (!d->mViewParametersStack.size()) {
        return;
    }

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
        for(int i = 0; i < d->mViewsListData->count(); ++i) {
            if (d->mViewsListData->viewParameters(i).mDataType == dataType) {
                d->mViewIndex = i;
                Q_EMIT viewIndexChanged();

                nextViewParameters = d->mViewsListData->viewParameters(i);
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

    if (!d->mViewParametersStack.size()) {
        return;
    }

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
    if (d->mViewParametersStack.size() <= 1) {
        return;
    }

    Q_EMIT popOneView();
    d->mViewParametersStack.pop_back();
    d->mNextViewParameters = {};
}

void ViewManager::setViewsData(ViewsListData *viewsData)
{
    if (d->mViewsListData == viewsData) {
        return;
    }

    d->mViewsListData = viewsData;
    Q_EMIT viewsDataChanged();

    if (d->mViewsListData) {
        d->mViewParametersStack = {d->mViewsListData->viewParameters(d->mViewIndex)};
    }
}


#include "moc_viewmanager.cpp"
