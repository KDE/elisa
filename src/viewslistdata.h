/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef VIEWSLISTDATA_H
#define VIEWSLISTDATA_H

#include "elisaLib_export.h"

#include <QObject>

#include "viewmanager.h"
#include "datatypes.h"
#include "elisautils.h"

#include <memory>

class QUrl;
class QString;
class ViewsListDataPrivate;

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
                   ViewManager::ModelType modelType)
        : mMainTitle(std::move(mainTitle))
        , mMainImage(std::move(mainImage))
        , mViewPresentationType(viewPresentationType)
        , mModelType(modelType)
        , mIsValid(true)
    {
    }

    ViewParameters(QString mainTitle,
                   QUrl mainImage,
                   ViewManager::ViewPresentationType viewPresentationType,
                   ViewManager::ModelType modelType,
                   ElisaUtils::FilterType filterType,
                   ElisaUtils::PlayListEntryType dataType,
                   QUrl fallbackItemIcon,
                   ViewManager::DelegateUseSecondaryText showSecondaryTextOnDelegates,
                   ViewManager::ViewCanBeRated viewCanBeRated,
                   ViewManager::IsTreeModelType isTreeModel)
        : mMainTitle(std::move(mainTitle))
        , mMainImage(std::move(mainImage))
        , mViewPresentationType(viewPresentationType)
        , mModelType(modelType)
        , mFilterType(filterType)
        , mDataType(dataType)
        , mFallbackItemIcon(std::move(fallbackItemIcon))
        , mShowSecondaryTextOnDelegates(showSecondaryTextOnDelegates)
        , mViewCanBeRated(viewCanBeRated)
        , mIsTreeModel(isTreeModel)
        , mIsValid(true)
    {
    }

    ViewParameters(QString mainTitle,
                   QUrl mainImage,
                   ViewManager::ViewPresentationType viewPresentationType,
                   ViewManager::ModelType modelType,
                   ElisaUtils::FilterType filterType,
                   ElisaUtils::PlayListEntryType dataType,
                   QUrl fallbackItemIcon,
                   ViewManager::DelegateUseSecondaryText showSecondaryTextOnDelegates,
                   ViewManager::ViewCanBeRated viewCanBeRated,
                   ViewManager::IsTreeModelType isTreeModel,
                   QUrl pathFilter)
        : mMainTitle(std::move(mainTitle))
        , mMainImage(std::move(mainImage))
        , mViewPresentationType(viewPresentationType)
        , mModelType(modelType)
        , mFilterType(filterType)
        , mDataType(dataType)
        , mFallbackItemIcon(std::move(fallbackItemIcon))
        , mShowSecondaryTextOnDelegates(showSecondaryTextOnDelegates)
        , mViewCanBeRated(viewCanBeRated)
        , mIsTreeModel(isTreeModel)
        , mPathFilter(std::move(pathFilter))
        , mIsValid(true)
    {
    }

    ViewParameters(QString mainTitle,
                   QUrl mainImage,
                   ViewManager::ViewPresentationType viewPresentationType,
                   ViewManager::ModelType modelType,
                   ElisaUtils::FilterType filterType,
                   ElisaUtils::PlayListEntryType dataType,
                   int sortRole,
                   Qt::SortOrder sortOrder,
                   ViewManager::AlbumCardinality albumCardinality,
                   ViewManager::AlbumViewStyle albumViewStyle,
                   ViewManager::RadioSpecificStyle radioSpecificStyle,
                   ViewManager::IsTreeModelType isTreeModel)
        : mMainTitle(std::move(mainTitle))
        , mMainImage(std::move(mainImage))
        , mViewPresentationType(viewPresentationType)
        , mModelType(modelType)
        , mFilterType(filterType)
        , mDataType(dataType)
        , mIsTreeModel(isTreeModel)
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
                mModelType == other.mModelType && mFilterType == other.mFilterType &&
                mDataType == other.mDataType && mIsTreeModel == other.mIsTreeModel && mFallbackItemIcon == other.mFallbackItemIcon &&
                mShowSecondaryTextOnDelegates == other.mShowSecondaryTextOnDelegates && mViewCanBeRated == other.mViewCanBeRated &&
                mSortRole == other.mSortRole && mSortOrder == other.mSortOrder &&
                mAlbumCardinality == other.mAlbumCardinality && mAlbumViewStyle == other.mAlbumViewStyle &&
                mRadioSpecificStyle == other.mRadioSpecificStyle && mDepth == other.mDepth &&
                mDatabaseIdFilter == other.mDatabaseIdFilter && mGenreNameFilter == other.mGenreNameFilter &&
                mArtistNameFilter == other.mArtistNameFilter && mPathFilter == other.mPathFilter;
    }

    bool operator!=(const ViewParameters &other) const {
        return mMainTitle != other.mMainTitle || mMainImage != other.mMainImage ||
                mSecondaryTitle != other.mSecondaryTitle || mViewPresentationType != other.mViewPresentationType ||
                mModelType != other.mModelType || mFilterType != other.mFilterType ||
                mDataType != other.mDataType || mIsTreeModel != other.mIsTreeModel || mFallbackItemIcon != other.mFallbackItemIcon ||
                mShowSecondaryTextOnDelegates != other.mShowSecondaryTextOnDelegates || mViewCanBeRated != other.mViewCanBeRated ||
                mSortRole != other.mSortRole || mSortOrder != other.mSortOrder ||
                mAlbumCardinality != other.mAlbumCardinality || mAlbumViewStyle != other.mAlbumViewStyle ||
                mRadioSpecificStyle != other.mRadioSpecificStyle || mDepth != other.mDepth ||
                mDatabaseIdFilter != other.mDatabaseIdFilter || mGenreNameFilter != other.mGenreNameFilter ||
                mArtistNameFilter != other.mArtistNameFilter || mPathFilter != other.mPathFilter;
    }

    QString mMainTitle;

    QString mSecondaryTitle;

    QUrl mMainImage;

    ViewManager::ViewPresentationType mViewPresentationType = ViewManager::UnknownViewPresentation;

    ViewManager::ModelType mModelType = ViewManager::UnknownModelType;

    ElisaUtils::FilterType mFilterType = ElisaUtils::UnknownFilter;

    ElisaUtils::PlayListEntryType mDataType = ElisaUtils::Unknown;

    QUrl mFallbackItemIcon;

    ViewManager::DelegateUseSecondaryText mShowSecondaryTextOnDelegates = ViewManager::DelegateWithSecondaryText;

    ViewManager::ViewCanBeRated mViewCanBeRated = ViewManager::ViewHideRating;

    ViewManager::IsTreeModelType mIsTreeModel = ViewManager::IsFlatModel;

    int mSortRole = Qt::DisplayRole;

    Qt::SortOrder mSortOrder = Qt::AscendingOrder;

    ViewManager::AlbumCardinality mAlbumCardinality = ViewManager::MultipleAlbum;

    ViewManager::AlbumViewStyle mAlbumViewStyle = ViewManager::NoDiscHeaders;

    ViewManager::RadioSpecificStyle mRadioSpecificStyle = ViewManager::IsTrack;

    int mDepth = 1;

    qulonglong mDatabaseIdFilter = 0;

    QString mGenreNameFilter;

    QString mArtistNameFilter;

    QUrl mPathFilter;

    bool mIsValid = false;
};

class ELISALIB_EXPORT ViewsListData : public QObject
{
    Q_OBJECT

public:

    explicit ViewsListData(QObject *parent = nullptr);

    ~ViewsListData();

    int count() const;

    const ViewParameters& viewParameters(int index) const;

    const QString& title(int index) const;

    const QUrl& iconUrl(int index) const;

Q_SIGNALS:

public Q_SLOTS:

private:

    std::unique_ptr<ViewsListDataPrivate> d;
};

#endif // VIEWSLISTDATA_H
