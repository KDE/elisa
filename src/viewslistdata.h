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
class MusicListenersManager;
class DatabaseInterface;
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
        , mDataFilter{{DataTypes::FilePathRole, std::move(pathFilter)}}
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

    ViewParameters(QString mainTitle,
                   QUrl mainImage,
                   ViewManager::ViewPresentationType viewPresentationType,
                   ViewManager::ModelType modelType,
                   ElisaUtils::FilterType filterType,
                   qulonglong databaseId,
                   ElisaUtils::PlayListEntryType dataType,
                   ElisaUtils::PlayListEntryType entryType,
                   QUrl fallbackItemIcon,
                   ViewManager::DelegateUseSecondaryText showSecondaryTextOnDelegates,
                   ViewManager::ViewCanBeRated viewCanBeRated)
        : mMainTitle(std::move(mainTitle))
        , mMainImage(std::move(mainImage))
        , mViewPresentationType(viewPresentationType)
        , mModelType(modelType)
        , mFilterType(filterType)
        , mDataType(dataType)
        , mEntryType(entryType)
        , mFallbackItemIcon(fallbackItemIcon)
        , mShowSecondaryTextOnDelegates(showSecondaryTextOnDelegates)
        , mViewCanBeRated(viewCanBeRated)
        , mIsValid(true)
    {
        mDataFilter = {{DataTypes::DatabaseIdRole, databaseId}};
    }

    ViewParameters(QString mainTitle,
                   QUrl mainImage,
                   ViewManager::ViewPresentationType viewPresentationType,
                   ViewManager::ModelType modelType,
                   ElisaUtils::FilterType filterType,
                   qulonglong databaseId,
                   ElisaUtils::PlayListEntryType dataType,
                   ElisaUtils::PlayListEntryType entryType,
                   int sortRole,
                   Qt::SortOrder sortOrder,
                   ViewManager::AlbumCardinality albumCardinality,
                   ViewManager::AlbumViewStyle albumViewStyle,
                   ViewManager::RadioSpecificStyle radioSpecificStyle,
                   ElisaUtils::IconUseColorOverlay iconUseColorOverlay)
        : mMainTitle(std::move(mainTitle))
        , mMainImage(std::move(mainImage))
        , mViewPresentationType(viewPresentationType)
        , mModelType(modelType)
        , mFilterType(filterType)
        , mDataType(dataType)
        , mEntryType(entryType)
        , mSortRole(sortRole)
        , mSortOrder(sortOrder)
        , mAlbumCardinality(albumCardinality)
        , mAlbumViewStyle(albumViewStyle)
        , mRadioSpecificStyle(radioSpecificStyle)
        , mIconUseColorOverlay(iconUseColorOverlay)
        , mIsValid(true)
    {
        mDataFilter = {{DataTypes::DatabaseIdRole, databaseId}};
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
                mDataFilter == other.mDataFilter;
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
                mDataFilter != other.mDataFilter;
    }

    QString mMainTitle;

    QString mSecondaryTitle;

    QUrl mMainImage;

    ViewManager::ViewPresentationType mViewPresentationType = ViewManager::UnknownViewPresentation;

    ViewManager::ModelType mModelType = ViewManager::UnknownModelType;

    ElisaUtils::FilterType mFilterType = ElisaUtils::UnknownFilter;

    ElisaUtils::PlayListEntryType mDataType = ElisaUtils::Unknown;

    ElisaUtils::PlayListEntryType mEntryType = ElisaUtils::Unknown;

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

    DataTypes::MusicDataType mDataFilter;

    ElisaUtils::IconUseColorOverlay mIconUseColorOverlay = ElisaUtils::UseColorOverlay;

    bool mUseSecondTitle = false;

    bool mIsValid = false;
};

class ELISALIB_EXPORT ViewsListData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(ElisaUtils::PlayListEntryType embeddedCategory
               READ embeddedCategory
               WRITE setEmbeddedCategory
               NOTIFY embeddedCategoryChanged)

    Q_PROPERTY(MusicListenersManager* manager
               READ manager
               WRITE setManager
               NOTIFY managerChanged)

    Q_PROPERTY(DatabaseInterface* database
               READ database
               WRITE setDatabase
               NOTIFY databaseChanged)

public:
    explicit ViewsListData(QObject *parent = nullptr);

    ~ViewsListData();

    int count() const;

    const ViewParameters& viewParameters(int index) const;

    const QString& title(int index) const;

    const QUrl& iconUrl(int index) const;

    const QString& secondTitle(int index) const;

    bool useSecondTitle(int index) const;

    ElisaUtils::IconUseColorOverlay iconUseColorOverlay(int index) const;

    qulonglong databaseId(int index) const;

    int indexFromEmbeddedDatabaseId(qulonglong databaseId) const;

    int indexFromEmbeddedName(const QString &name) const;

    ElisaUtils::PlayListEntryType embeddedCategory() const;

    MusicListenersManager* manager() const;

    DatabaseInterface* database() const;

Q_SIGNALS:

    void embeddedCategoryChanged();

    void managerChanged();

    void databaseChanged();

    void needData(ElisaUtils::PlayListEntryType dataType);

    void dataAboutToBeAdded(int startIndex, int endIndex);

    void dataAdded();

    void dataAboutToBeRemoved(int startIndex, int endIndex);

    void dataRemoved();

    void dataModified(int index);

    void dataAboutToBeReset();

    void dataReset();

public Q_SLOTS:

    void setEmbeddedCategory(ElisaUtils::PlayListEntryType aEmbeddedView);

    void setManager(MusicListenersManager *aManager);

    void setDatabase(DatabaseInterface *aDatabase);

    void genresAdded(DataTypes::ListGenreDataType newData);

    void artistsAdded(DataTypes::ListArtistDataType newData);

    void artistRemoved(qulonglong removedDatabaseId);

    void albumsAdded(DataTypes::ListAlbumDataType newData);

    void albumRemoved(qulonglong removedDatabaseId);

    void albumModified(const DataTypes::AlbumDataType &modifiedAlbum);

    void cleanedDatabase();

private:

    void refreshEmbeddedCategory();

    std::unique_ptr<ViewsListDataPrivate> d;
};

#endif // VIEWSLISTDATA_H
