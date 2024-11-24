/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef VIEWSLISTDATA_H
#define VIEWSLISTDATA_H

#include "elisaLib_export.h"

#include <QObject>
#include <QQmlEngine>

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
    {
    }

    ViewParameters(QString mainTitle,
                   QUrl mainImage,
                   ViewManager::ViewPresentationType viewPresentationType,
                   ViewManager::ModelType modelType,
                   ElisaUtils::FilterType filterType,
                   ElisaUtils::PlayListEntryType dataType,
                   int sortRole, QList<int> sortRoles, QList<QString> sortRoleNames,
                   Qt::SortOrder sortOrder, QList<QString> sortOrderNames,
                   QUrl fallbackItemIcon,
                   ViewManager::DelegateUseSecondaryText showSecondaryTextOnDelegates,
                   ViewManager::ViewCanBeRated viewCanBeRated)
        : mMainTitle(std::move(mainTitle))
        , mMainImage(std::move(mainImage))
        , mViewPresentationType(viewPresentationType)
        , mModelType(modelType)
        , mFilterType(filterType)
        , mDataType(dataType)
        , mFallbackItemIcon(std::move(fallbackItemIcon))
        , mShowSecondaryTextOnDelegates(showSecondaryTextOnDelegates)
        , mViewCanBeRated(viewCanBeRated)
        , mSortRole(sortRole)
        , mSortRoles(std::move(sortRoles))
        , mSortRoleNames(std::move(sortRoleNames))
        , mSortOrder(sortOrder)
        , mSortOrderNames(std::move(sortOrderNames))
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
        , mDataFilter{{DataTypes::FilePathRole, std::move(pathFilter)}}
    {
    }

    ViewParameters(QString mainTitle,
                   QUrl mainImage,
                   ViewManager::ViewPresentationType viewPresentationType,
                   ViewManager::ModelType modelType,
                   ElisaUtils::FilterType filterType,
                   ElisaUtils::PlayListEntryType dataType,
                   int sortRole, QList<int> sortRoles, QList<QString> sortRoleNames,
                   Qt::SortOrder sortOrder, QList<QString> sortOrderNames,
                   ViewManager::AlbumCardinality albumCardinality,
                   ViewManager::AlbumViewStyle albumViewStyle)
        : mMainTitle(std::move(mainTitle))
        , mMainImage(std::move(mainImage))
        , mViewPresentationType(viewPresentationType)
        , mModelType(modelType)
        , mFilterType(filterType)
        , mDataType(dataType)
        , mSortRole(sortRole)
        , mSortRoles(std::move(sortRoles))
        , mSortRoleNames(std::move(sortRoleNames))
        , mSortOrder(sortOrder)
        , mSortOrderNames(std::move(sortOrderNames))
        , mAlbumCardinality(albumCardinality)
        , mAlbumViewStyle(albumViewStyle)
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
                   int sortRole, QList<int> sortRoles, QList<QString> sortRoleNames,
                   Qt::SortOrder sortOrder, QList<QString> sortOrderNames,
                   ViewManager::AlbumCardinality albumCardinality,
                   ViewManager::AlbumViewStyle albumViewStyle)
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
    {
        mDataFilter = {{DataTypes::DatabaseIdRole, databaseId}};
    }

    bool operator==(const ViewParameters &other) const {
        return mMainTitle == other.mMainTitle && mMainImage == other.mMainImage &&
                mSecondaryTitle == other.mSecondaryTitle && mViewPresentationType == other.mViewPresentationType &&
                mModelType == other.mModelType && mFilterType == other.mFilterType &&
                mDataType == other.mDataType && mFallbackItemIcon == other.mFallbackItemIcon &&
                mShowSecondaryTextOnDelegates == other.mShowSecondaryTextOnDelegates && mViewCanBeRated == other.mViewCanBeRated &&
                mSortRole == other.mSortRole && mSortRoles == other.mSortRoles && mSortRoleNames == other.mSortRoleNames &&
                mSortOrder == other.mSortOrder && mSortOrderNames == other.mSortOrderNames &&
                mAlbumCardinality == other.mAlbumCardinality && mAlbumViewStyle == other.mAlbumViewStyle &&
                mDepth == other.mDepth &&
                mDataFilter == other.mDataFilter;
    }

    bool operator!=(const ViewParameters &other) const {
        return mMainTitle != other.mMainTitle || mMainImage != other.mMainImage ||
                mSecondaryTitle != other.mSecondaryTitle || mViewPresentationType != other.mViewPresentationType ||
                mModelType != other.mModelType || mFilterType != other.mFilterType ||
                mDataType != other.mDataType || mFallbackItemIcon != other.mFallbackItemIcon ||
                mShowSecondaryTextOnDelegates != other.mShowSecondaryTextOnDelegates || mViewCanBeRated != other.mViewCanBeRated ||
                mSortRole != other.mSortRole || mSortRoles != other.mSortRoles || mSortRoleNames != other.mSortRoleNames ||
                mSortOrder != other.mSortOrder || mSortOrderNames != other.mSortOrderNames ||
                mAlbumCardinality != other.mAlbumCardinality || mAlbumViewStyle != other.mAlbumViewStyle ||
                mDepth != other.mDepth ||
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

    int mSortRole = Qt::DisplayRole;

    QList<int> mSortRoles = {Qt::DisplayRole};

    QList<QString> mSortRoleNames = {QString{}};

    Qt::SortOrder mSortOrder = Qt::AscendingOrder;

    QList<QString> mSortOrderNames = {QStringLiteral("A-Z"), QStringLiteral("Z-A")};

    ViewManager::AlbumCardinality mAlbumCardinality = ViewManager::MultipleAlbum;

    ViewManager::AlbumViewStyle mAlbumViewStyle = ViewManager::NoDiscHeaders;

    int mDepth = 1;

    DataTypes::MusicDataType mDataFilter;

    bool mUseSecondTitle = false;
};

class ELISALIB_EXPORT ViewsListData : public QObject
{
    Q_OBJECT

    QML_ELEMENT

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

    ~ViewsListData() override;

    [[nodiscard]] int count() const;

    [[nodiscard]] bool isEmpty() const;

    [[nodiscard]] const ViewParameters& viewParameters(int index) const;

    [[nodiscard]] const QString& title(int index) const;

    [[nodiscard]] const QUrl& iconUrl(int index) const;

    [[nodiscard]] const QString& secondTitle(int index) const;

    [[nodiscard]] bool useSecondTitle(int index) const;

    [[nodiscard]] qulonglong databaseId(int index) const;

    [[nodiscard]] bool defaultEntry(int index) const;

    [[nodiscard]] ElisaUtils::PlayListEntryType dataType(int index) const;

    [[nodiscard]] ElisaUtils::PlayListEntryType embeddedCategory() const;

    [[nodiscard]] MusicListenersManager* manager() const;

    [[nodiscard]] DatabaseInterface* database() const;

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

    void genresAdded(const DataTypes::ListGenreDataType &newData);

    void artistsAdded(const DataTypes::ListArtistDataType &newData);

    void albumsAdded(const DataTypes::ListAlbumDataType &newData);

    void albumModified(const DataTypes::AlbumDataType &modifiedAlbum);

    void databaseEntryRemoved(qulonglong removedDatabaseId, ElisaUtils::PlayListEntryType entryType);

    void cleanedDatabase();

private:

    void clearEmbeddedItems();

    void refreshEmbeddedCategory();

    std::unique_ptr<ViewsListDataPrivate> d;
};

#endif // VIEWSLISTDATA_H
