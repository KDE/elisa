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
                   int sortRole, QVector<int> sortRoles, QVector<QString> sortRoleNames,
                   Qt::SortOrder sortOrder, QVector<QString> sortOrderNames,
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
        , mSortRole(sortRole)
        , mSortRoles(std::move(sortRoles))
        , mSortRoleNames(std::move(sortRoleNames))
        , mSortOrder(sortOrder)
        , mSortOrderNames(std::move(sortOrderNames))
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
                   int sortRole, QVector<int> sortRoles, QVector<QString> sortRoleNames,
                   Qt::SortOrder sortOrder, QVector<QString> sortOrderNames,
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
        , mSortRoles(std::move(sortRoles))
        , mSortRoleNames(std::move(sortRoleNames))
        , mSortOrder(sortOrder)
        , mSortOrderNames(std::move(sortOrderNames))
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
        , mFallbackItemIcon(std::move(fallbackItemIcon))
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
                   int sortRole, QVector<int> sortRoles, QVector<QString> sortRoleNames,
                   Qt::SortOrder sortOrder, QVector<QString> sortOrderNames,
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
        , mSortRoles(std::move(sortRoles))
        , mSortRoleNames(std::move(sortRoleNames))
        , mSortOrder(sortOrder)
        , mSortOrderNames(std::move(sortOrderNames))
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
                mSortRole == other.mSortRole && mSortRoles == other.mSortRoles && mSortRoleNames == other.mSortRoleNames &&
                mSortOrder == other.mSortOrder && mSortOrderNames == other.mSortOrderNames &&
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
                mSortRole != other.mSortRole || mSortRoles != other.mSortRoles || mSortRoleNames != other.mSortRoleNames ||
                mSortOrder != other.mSortOrder || mSortOrderNames != other.mSortOrderNames ||
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

    QVector<int> mSortRoles = {Qt::DisplayRole};

    QVector<QString> mSortRoleNames = {QString{}};

    Qt::SortOrder mSortOrder = Qt::AscendingOrder;

    QVector<QString> mSortOrderNames = {QStringLiteral("A-Z"), QStringLiteral("Z-A")};

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

    Q_PROPERTY(bool isFullyInitialized
               READ isFullyInitialized
               NOTIFY isFullyInitializedChanged)

public:
    explicit ViewsListData(QObject *parent = nullptr);

    ~ViewsListData() override;

    [[nodiscard]] int count() const;

    [[nodiscard]] bool isEmpty() const;

    [[nodiscard]] const ViewParameters& viewParameters(int index) const;

    [[nodiscard]] const QString& title(int index) const;

    [[nodiscard]] const QUrl& iconUrl(int index) const;

    [[nodiscard]] const QString& secondTitle(int index) const;

    [[nodiscard]] bool useSecondTitle(int index) const;

    [[nodiscard]] ElisaUtils::IconUseColorOverlay iconUseColorOverlay(int index) const;

    [[nodiscard]] qulonglong databaseId(int index) const;

    [[nodiscard]] bool defaultEntry(int index) const;

    [[nodiscard]] int indexFromEmbeddedDatabaseId(qulonglong databaseId) const;

    [[nodiscard]] int indexFromEmbeddedName(const QString &name) const;

    [[nodiscard]] ElisaUtils::PlayListEntryType embeddedCategory() const;

    [[nodiscard]] MusicListenersManager* manager() const;

    [[nodiscard]] DatabaseInterface* database() const;

    [[nodiscard]] bool isFullyInitialized() const;

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

    void isFullyInitializedChanged();

public Q_SLOTS:

    void setEmbeddedCategory(ElisaUtils::PlayListEntryType aEmbeddedView);

    void setManager(MusicListenersManager *aManager);

    void setDatabase(DatabaseInterface *aDatabase);

    void genresAdded(const DataTypes::ListGenreDataType &newData);

    void artistsAdded(const DataTypes::ListArtistDataType &newData);

    void artistRemoved(qulonglong removedDatabaseId);

    void albumsAdded(const DataTypes::ListAlbumDataType &newData);

    void albumRemoved(qulonglong removedDatabaseId);

    void albumModified(const DataTypes::AlbumDataType &modifiedAlbum);

    void cleanedDatabase();

private:

    void refreshEmbeddedCategory();

    std::unique_ptr<ViewsListDataPrivate> d;
};

#endif // VIEWSLISTDATA_H
