/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "viewconfigurationdata.h"

class ViewConfigurationDataPrivate
{
public:
    ViewConfigurationDataPrivate()
    = default;

    ViewConfigurationDataPrivate(ElisaUtils::FilterType filterType, int expectedDepth,
                                 QString mainTitle, QString secondaryTitle,
                                 QUrl imageUrl, ElisaUtils::PlayListEntryType dataType,
                                 QAbstractItemModel *model, QAbstractProxyModel *associatedProxyModel,
                                 QUrl viewDefaultIcon, DataTypes::DataType dataFilter,
                                 int sortRole, QVector<int> sortRoles, QVector<QString> sortRoleNames,
                                 Qt::SortOrder sortOrder, QVector<QString> sortOrderNames,
                                 ViewManager::ViewCanBeRated viewShowRating,
                                 ViewManager::DelegateUseSecondaryText viewDelegateDisplaySecondaryText,
                                 ViewManager::IsTreeModelType isTreeModel)
        : mFilterType(filterType)
        , mExpectedDepth(expectedDepth)
        , mMainTitle(std::move(mainTitle))
        , mSecondaryTitle(std::move(secondaryTitle))
        , mImageUrl(std::move(imageUrl))
        , mDataType(dataType)
        , mModel(model)
        , mAssociatedProxyModel(associatedProxyModel)
        , mViewDefaultIcon(std::move(viewDefaultIcon))
        , mDataFilter(std::move(dataFilter))
        , mViewShowRating(viewShowRating)
        , mViewDelegateDisplaySecondaryText(viewDelegateDisplaySecondaryText)
        , mIsTreeModel(isTreeModel)
        , mSortRole(sortRole)
        , mSortRoles(std::move(sortRoles))
        , mSortRoleNames(std::move(sortRoleNames))
        , mSortOrder(sortOrder)
        , mSortOrderNames(std::move(sortOrderNames))
    {
    }

    ViewConfigurationDataPrivate(ElisaUtils::FilterType filterType, int expectedDepth,
                                 QString mainTitle, QString secondaryTitle,
                                 QUrl imageUrl, ElisaUtils::PlayListEntryType dataType,
                                 QAbstractItemModel *model, QAbstractProxyModel *associatedProxyModel,
                                 DataTypes::DataType dataFilter,
                                 int sortRole, QVector<int> sortRoles, QVector<QString> sortRoleNames,
                                 Qt::SortOrder sortOrder, QVector<QString> sortOrderNames,
                                 ViewManager::AlbumCardinality displaySingleAlbum,
                                 ViewManager::AlbumViewStyle showDiscHeaders,
                                 ViewManager::RadioSpecificStyle radioCase,
                                 ViewManager::IsTreeModelType isTreeModel)
        : mFilterType(filterType)
        , mExpectedDepth(expectedDepth)
        , mMainTitle(std::move(mainTitle))
        , mSecondaryTitle(std::move(secondaryTitle))
        , mImageUrl(std::move(imageUrl))
        , mDataType(dataType)
        , mModel(model)
        , mAssociatedProxyModel(associatedProxyModel)
        , mDataFilter(std::move(dataFilter))
        , mIsTreeModel(isTreeModel)
        , mSortRole(sortRole)
        , mSortRoles(std::move(sortRoles))
        , mSortRoleNames(std::move(sortRoleNames))
        , mSortOrder(sortOrder)
        , mSortOrderNames(std::move(sortOrderNames))
        , mDisplaySingleAlbum(displaySingleAlbum)
        , mShowDiscHeaders(showDiscHeaders)
        , mRadioCase(radioCase)
    {
    }

    ElisaUtils::FilterType mFilterType;

    int mExpectedDepth;

    QString mMainTitle;

    QString mSecondaryTitle;

    QUrl mImageUrl;

    ElisaUtils::PlayListEntryType mDataType;

    QAbstractItemModel * mModel;

    QAbstractProxyModel * mAssociatedProxyModel;

    QUrl mViewDefaultIcon;

    DataTypes::DataType mDataFilter;

    ViewManager::ViewCanBeRated mViewShowRating;

    ViewManager::DelegateUseSecondaryText mViewDelegateDisplaySecondaryText;

    ViewManager::IsTreeModelType mIsTreeModel;

    int mSortRole;

    QVector<int> mSortRoles;

    QVector<QString> mSortRoleNames;

    Qt::SortOrder mSortOrder;

    QVector<QString> mSortOrderNames;

    ViewManager::AlbumCardinality mDisplaySingleAlbum;

    ViewManager::AlbumViewStyle mShowDiscHeaders;

    ViewManager::RadioSpecificStyle mRadioCase;
};

ViewConfigurationData::ViewConfigurationData(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<ViewConfigurationDataPrivate>())
{
}

ViewConfigurationData::ViewConfigurationData(ElisaUtils::FilterType filterType, int expectedDepth,
                                             QString mainTitle, QString secondaryTitle,
                                             QUrl imageUrl, ElisaUtils::PlayListEntryType dataType,
                                             QAbstractItemModel *model, QAbstractProxyModel *associatedProxyModel,
                                             QUrl viewDefaultIcon, DataTypes::DataType dataFilter,
                                             int sortRole, QVector<int> sortRoles, QVector<QString> sortRoleNames,
                                             Qt::SortOrder sortOrder, QVector<QString> sortOrderNames,
                                             ViewManager::ViewCanBeRated viewShowRating,
                                             ViewManager::DelegateUseSecondaryText viewDelegateDisplaySecondaryText,
                                             ViewManager::IsTreeModelType isTreeModel, QObject *parent)
    : QObject(parent)
    , d(std::make_unique<ViewConfigurationDataPrivate>(filterType, expectedDepth, std::move(mainTitle),
                                                       std::move(secondaryTitle), std::move(imageUrl),
                                                       dataType, model, associatedProxyModel,
                                                       std::move(viewDefaultIcon), std::move(dataFilter),
                                                       sortRole, std::move(sortRoles), std::move(sortRoleNames),
                                                       sortOrder, std::move(sortOrderNames),
                                                       viewShowRating,viewDelegateDisplaySecondaryText,
                                                       isTreeModel))
{
}

ViewConfigurationData::ViewConfigurationData(ElisaUtils::FilterType filterType, int expectedDepth,
                                             QString mainTitle, QString secondaryTitle,
                                             QUrl imageUrl, ElisaUtils::PlayListEntryType dataType,
                                             QAbstractItemModel *model, QAbstractProxyModel *associatedProxyModel,
                                             DataTypes::DataType dataFilter,
                                             int sortRole, QVector<int> sortRoles, QVector<QString> sortRoleNames,
                                             Qt::SortOrder sortOrder, QVector<QString> sortOrderNames,
                                             ViewManager::AlbumCardinality displaySingleAlbum,
                                             ViewManager::AlbumViewStyle showDiscHeaders,
                                             ViewManager::RadioSpecificStyle radioCase,
                                             ViewManager::IsTreeModelType isTreeModel, QObject *parent)
    : QObject(parent)
    , d(std::make_unique<ViewConfigurationDataPrivate>(filterType, expectedDepth, std::move(mainTitle),
                                                       std::move(secondaryTitle), std::move(imageUrl),
                                                       dataType, model, associatedProxyModel, std::move(dataFilter),
                                                       sortRole, std::move(sortRoles), std::move(sortRoleNames),
                                                       sortOrder, std::move(sortOrderNames), displaySingleAlbum, showDiscHeaders,
                                                       radioCase, isTreeModel))
{
}

ElisaUtils::FilterType ViewConfigurationData::filterType() const
{
    return d->mFilterType;
}

int ViewConfigurationData::expectedDepth() const
{
    return d->mExpectedDepth;
}

QString ViewConfigurationData::mainTitle() const
{
    return d->mMainTitle;
}

QString ViewConfigurationData::secondaryTitle() const
{
    return d->mSecondaryTitle;
}

QUrl ViewConfigurationData::imageUrl() const
{
    return d->mImageUrl;
}

ElisaUtils::PlayListEntryType ViewConfigurationData::dataType() const
{
    return d->mDataType;
}

QAbstractItemModel *ViewConfigurationData::model() const
{
    return d->mModel;
}

QAbstractProxyModel *ViewConfigurationData::associatedProxyModel() const
{
    return d->mAssociatedProxyModel;
}

QUrl ViewConfigurationData::viewDefaultIcon() const
{
    return d->mViewDefaultIcon;
}

DataTypes::DataType ViewConfigurationData::dataFilter() const
{
    return d->mDataFilter;
}

ViewManager::ViewCanBeRated ViewConfigurationData::viewShowRating() const
{
    return d->mViewShowRating;
}

ViewManager::DelegateUseSecondaryText ViewConfigurationData::viewDelegateDisplaySecondaryText() const
{
    return d->mViewDelegateDisplaySecondaryText;
}

ViewManager::IsTreeModelType ViewConfigurationData::isTreeModel() const
{
    return d->mIsTreeModel;
}

int ViewConfigurationData::sortRole() const
{
    return d->mSortRole;
}

QVector<int> ViewConfigurationData::sortRoles() const
{
    return d->mSortRoles;
}

QVector<QString> ViewConfigurationData::sortRoleNames() const
{
    return d->mSortRoleNames;
}

Qt::SortOrder ViewConfigurationData::sortOrder() const
{
    return d->mSortOrder;
}

QVector<QString> ViewConfigurationData::sortOrderNames() const
{
    return d->mSortOrderNames;
}

ViewManager::AlbumCardinality ViewConfigurationData::displaySingleAlbum() const
{
    return d->mDisplaySingleAlbum;
}

ViewManager::AlbumViewStyle ViewConfigurationData::showDiscHeaders() const
{
    return d->mShowDiscHeaders;
}

ViewManager::RadioSpecificStyle ViewConfigurationData::radioCase() const
{
    return d->mRadioCase;
}

ViewConfigurationData::~ViewConfigurationData() = default;

#include "moc_viewconfigurationdata.cpp"
