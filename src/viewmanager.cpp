/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "viewmanager.h"

#include "viewslistdata.h"
#include "viewconfigurationdata.h"
#include "datatypes.h"
#include "viewsLogging.h"
#include "models/datamodel.h"
#include "models/gridviewproxymodel.h"

#if defined KF5KIO_FOUND && KF5KIO_FOUND
#include "models/filebrowsermodel.h"
#include "models/filebrowserproxymodel.h"
#endif

#include "elisa_settings.h"

#include <KI18n/KLocalizedString>

#include <QQmlEngine>
#include <QMetaEnum>

class ViewManagerPrivate
{
public:
    ViewsListData *mViewsListData = nullptr;

    QMap<ElisaUtils::PlayListEntryType, ViewParameters> mChildViews = {
        {ElisaUtils::Album, {{},
                             QUrl{QStringLiteral("image://icon/view-media-track")},
                             ViewManager::ListView,
                             ViewManager::GenericDataModel,
                             ElisaUtils::FilterById,
                             ElisaUtils::Track,
                             Qt::DisplayRole,
                             {Qt::DisplayRole},
                             {i18n("Title")},
                             Qt::AscendingOrder,
                             {i18n("A-Z"), i18n("Z-A")},
                             ViewManager::SingleAlbum,
                             ViewManager::DiscHeaders,
                             ViewManager::IsTrack,
                             ViewManager::IsFlatModel}},
        {ElisaUtils::Genre, {{},
                             QUrl{QStringLiteral("image://icon/view-media-artist")},
                             ViewManager::GridView,
                             ViewManager::GenericDataModel,
                             ElisaUtils::FilterByGenre,
                             ElisaUtils::Artist,
                             Qt::DisplayRole,
                             {Qt::DisplayRole},
                             {i18n("Name")},
                             Qt::AscendingOrder,
                             {i18n("A-Z"), i18n("Z-A")},
                             QUrl{QStringLiteral("image://icon/view-media-artist")},
                             ViewManager::DelegateWithoutSecondaryText,
                             ViewManager::ViewHideRating,
                             ViewManager::IsFlatModel}},
        {ElisaUtils::Artist, {{},
                              QUrl{QStringLiteral("image://icon/view-media-album-cover")},
                              ViewManager::GridView,
                              ViewManager::GenericDataModel,
                              ElisaUtils::FilterByArtist,
                              ElisaUtils::Album,
                              DataTypes::TitleRole,
                              {DataTypes::TitleRole, DataTypes::GenreRole, DataTypes::YearRole},
                              {i18n("Title"), i18n("Genre"), i18n("Year")},
                              Qt::AscendingOrder,
                              {i18n("A-Z"), i18n("Z-A"), i18n("A-Z"), i18n("Z-A"), i18n("Oldest First"), i18n("Newest First")},
                              QUrl{QStringLiteral("image://icon/media-optical-audio")},
                              ViewManager::DelegateWithSecondaryText,
                              ViewManager::ViewShowRating,
                              ViewManager::IsFlatModel}},
        {ElisaUtils::Container, {{},
                                 QUrl{QStringLiteral("image://icon/folder")},
                                 ViewManager::GridView,
                                 ViewManager::FileBrowserModel,
                                 ElisaUtils::FilterByPath,
                                 ElisaUtils::FileName,
                                 Qt::DisplayRole,
                                 {Qt::DisplayRole},
                                 {i18n("Name")},
                                 Qt::AscendingOrder,
                                 {i18n("A-Z"), i18n("Z-A")},
                                 QUrl{QStringLiteral("image://icon/folder")},
                                 ViewManager::DelegateWithoutSecondaryText,
                                 ViewManager::ViewHideRating,
                                 ViewManager::IsFlatModel}},
    };

    int mViewIndex = -1;

    int mInitialIndex = -1;

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

int ViewManager::initialIndex() const
{
    return d->mInitialIndex;
}

ViewsListData *ViewManager::viewsData() const
{
    return d->mViewsListData;
}

ViewManager::~ViewManager() = default;

void ViewManager::openView(int viewIndex)
{
    qCDebug(orgKdeElisaViews()) << "ViewManager::openView" << viewIndex << d->mViewParametersStack.size() << d->mViewsListData;

    if (!d->mViewsListData || d->mViewsListData->isEmpty() || !d->mViewsListData->isFullyInitialized()) {
        return;
    }

    if (viewIndex < 0 || viewIndex >= d->mViewsListData->count()) {
        viewIndex = 0;
    }

    const auto &viewParameters = d->mViewsListData->viewParameters(viewIndex);

    qCDebug(orgKdeElisaViews()) << "ViewManager::openView" << "selected view" << viewIndex;

    if (d->mViewParametersStack.isEmpty() || viewParameters != d->mViewParametersStack.back()) {
        qCDebug(orgKdeElisaViews()) << "ViewManager::openView" << "changing view";
        d->mViewIndex = viewIndex;
        Q_EMIT viewIndexChanged();

        d->mNextViewParameters = viewParameters;
        applyFilter(d->mNextViewParameters, viewParameters.mMainTitle, d->mNextViewParameters);
        openViewFromData(d->mNextViewParameters);
    }
}

void ViewManager::openChildView(const DataTypes::MusicDataType &fullData)
{
    qCDebug(orgKdeElisaViews()) << "ViewManager::openChildView"
                                << fullData[DataTypes::TitleRole]
                                << fullData[DataTypes::SecondaryTextRole]
                                << fullData[DataTypes::ImageUrlRole]
                                << fullData[DataTypes::DatabaseIdRole]
                                << fullData[DataTypes::FilePathRole]
                                << fullData[DataTypes::ElementTypeRole]
                                << fullData[DataTypes::IsSingleDiscAlbumRole]
                                << d->mViewParametersStack.size();

    if (!d->mViewParametersStack.size()) {
        return;
    }

    if (!d->mViewsListData || d->mViewsListData->isEmpty() || !d->mViewsListData->isFullyInitialized()) {
        return;
    }

    const auto &lastView = d->mViewParametersStack.back();
    auto dataType = fullData[DataTypes::ElementTypeRole].value<ElisaUtils::PlayListEntryType>();
    auto title = fullData[DataTypes::TitleRole].toString();
    auto innerSecondaryTitle = fullData[DataTypes::ArtistRole].toString();
    auto innerImage = fullData[DataTypes::ImageUrlRole].toUrl();

    auto nextViewParameters = d->mChildViews[dataType];

    nextViewParameters.mMainTitle = title;
    nextViewParameters.mSecondaryTitle = innerSecondaryTitle;
    nextViewParameters.mMainImage = innerImage;
    nextViewParameters.mDepth = d->mViewParametersStack.size() + 1;
    nextViewParameters.mDataFilter = fullData;

    if (lastView.mFilterType == ElisaUtils::FilterByGenre) {
        nextViewParameters.mFilterType = ElisaUtils::FilterByGenreAndArtist;
    }

    applyFilter(nextViewParameters, title, lastView);

    if (dataType == ElisaUtils::Album && nextViewParameters.mDataFilter[DataTypes::IsSingleDiscAlbumRole].toBool())
    {
        nextViewParameters.mAlbumViewStyle = NoDiscHeaders;
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

    if (lastView.mFilterType == ElisaUtils::FilterByArtist && dataType == ElisaUtils::Album && lastView.mDataFilter[DataTypes::ArtistRole].toString() != innerSecondaryTitle) {
        nextViewParameters = lastView;
        nextViewParameters.mDataFilter[DataTypes::ArtistRole] = innerSecondaryTitle;
    }

    openViewFromData(nextViewParameters);
}

void ViewManager::openAlbumView(const QString &title, const QString &artist, qulonglong databaseId, const QUrl &albumCoverUrl)
{
    if (d->mViewsListData->embeddedCategory() == ElisaUtils::Album) {
        auto index = d->mViewsListData->indexFromEmbeddedDatabaseId(databaseId);

        if (index == -1) {
            return;
        }

        openView(index);
    } else {
        openChildView({{DataTypes::ElementTypeRole, ElisaUtils::Album},
                       {DataTypes::DatabaseIdRole, databaseId},
                       {DataTypes::TitleRole, title},
                       {DataTypes::ArtistRole, artist},
                       {DataTypes::ImageUrlRole, albumCoverUrl},});
    }
}

void ViewManager::openArtistView(const QString &artist)
{
    if (d->mViewsListData->embeddedCategory() == ElisaUtils::Artist) {
        auto index = d->mViewsListData->indexFromEmbeddedName(artist);

        if (index == -1) {
            return;
        }

        openView(index);
    } else {
        openChildView({{DataTypes::ElementTypeRole, ElisaUtils::Artist},
                       {DataTypes::TitleRole, artist},});
    }
}

void ViewManager::openNowPlaying()
{
    openView(0);
}

void ViewManager::viewIsLoaded()
{
    qCDebug(orgKdeElisaViews()) << "ViewManager::viewIsLoaded" << d->mViewParametersStack.size()
                                << d->mViewsListData;

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

    QAbstractItemModel *newModel = nullptr;
    QAbstractProxyModel *proxyModel = nullptr;

    switch (viewParamaters.mModelType)
    {
    case FileBrowserModel:
    {
#if defined KF5KIO_FOUND && KF5KIO_FOUND
        newModel = new ::FileBrowserModel;
        auto *realProxyModel = new FileBrowserProxyModel;
        proxyModel = realProxyModel;
#else
        newModel = nullptr;
        proxyModel = nullptr;
#endif
        break;
    }
    case GenericDataModel:
        newModel = new DataModel;
        proxyModel = new GridViewProxyModel;
        break;
    case UnknownModelType:
        qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << "unknown model type";
        break;
    }

    QQmlEngine::setObjectOwnership(newModel, QQmlEngine::JavaScriptOwnership);
    QQmlEngine::setObjectOwnership(proxyModel, QQmlEngine::JavaScriptOwnership);

    d->mViewParametersStack.push_back(viewParamaters);
    switch (viewParamaters.mViewPresentationType)
    {
    case ViewPresentationType::GridView:
    {
        qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << viewParamaters.mViewPresentationType
                                    << viewParamaters.mFilterType
                                    << viewParamaters.mDepth << viewParamaters.mMainTitle << viewParamaters.mSecondaryTitle
                                    << viewParamaters.mMainImage << viewParamaters.mDataType
                                    << viewParamaters.mModelType << viewParamaters.mFallbackItemIcon
                                    << viewParamaters.mDataFilter
                                    << viewParamaters.mViewCanBeRated << viewParamaters.mShowSecondaryTextOnDelegates
                                    << viewParamaters.mIsTreeModel;

        auto configurationData = std::make_unique<ViewConfigurationData>(viewParamaters.mFilterType, viewParamaters.mDepth,
                                                                         viewParamaters.mMainTitle, viewParamaters.mSecondaryTitle,
                                                                         viewParamaters.mMainImage, viewParamaters.mDataType,
                                                                         newModel, proxyModel, viewParamaters.mFallbackItemIcon,
                                                                         viewParamaters.mDataFilter,
                                                                         computePreferredSortRole(viewParamaters.mSortRole),
                                                                         viewParamaters.mSortRoles, viewParamaters.mSortRoleNames,
                                                                         computePreferredSortOrder(viewParamaters.mSortOrder),
                                                                         viewParamaters.mSortOrderNames,
                                                                         viewParamaters.mViewCanBeRated, viewParamaters.mShowSecondaryTextOnDelegates,
                                                                         viewParamaters.mIsTreeModel);

        QQmlEngine::setObjectOwnership(configurationData.get(), QQmlEngine::JavaScriptOwnership);

        Q_EMIT openGridView(configurationData.release());
        break;
    }
    case ViewPresentationType::ListView:
    {
        qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << viewParamaters.mFilterType
                                    << viewParamaters.mDepth << viewParamaters.mMainTitle << viewParamaters.mSecondaryTitle
                                    << viewParamaters.mDataFilter[DataTypes::DatabaseIdRole] << viewParamaters.mMainImage
                                    << viewParamaters.mModelType << viewParamaters.mDataType
                                    << viewParamaters.mDataFilter << viewParamaters.mSortRole
                                    << viewParamaters.mSortOrder << viewParamaters.mAlbumCardinality
                                    << viewParamaters.mAlbumViewStyle << viewParamaters.mRadioSpecificStyle
                                    << viewParamaters.mIsTreeModel;

        auto configurationData = std::make_unique<ViewConfigurationData>(viewParamaters.mFilterType, viewParamaters.mDepth,
                                                                         viewParamaters.mMainTitle, viewParamaters.mSecondaryTitle,
                                                                         viewParamaters.mMainImage, viewParamaters.mDataType,
                                                                         newModel, proxyModel, viewParamaters.mDataFilter,
                                                                         computePreferredSortRole(viewParamaters.mSortRole),
                                                                         viewParamaters.mSortRoles, viewParamaters.mSortRoleNames,
                                                                         computePreferredSortOrder(viewParamaters.mSortOrder),
                                                                         viewParamaters.mSortOrderNames,
                                                                         viewParamaters.mAlbumCardinality, viewParamaters.mAlbumViewStyle,
                                                                         viewParamaters.mRadioSpecificStyle, viewParamaters.mIsTreeModel);

        QQmlEngine::setObjectOwnership(configurationData.get(), QQmlEngine::JavaScriptOwnership);

        Q_EMIT openListView(configurationData.release());
        break;
    }
    case ContextView:
        qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << viewParamaters.mViewPresentationType
                                    << viewParamaters.mDepth << viewParamaters.mMainTitle
                                    << viewParamaters.mMainImage;
        Q_EMIT switchContextView(viewParamaters.mDepth, viewParamaters.mMainTitle, viewParamaters.mMainImage);
        break;
    case UnknownViewPresentation:
        qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << "unknown view type";
        break;
    }
}

void ViewManager::applyFilter(ViewParameters &nextViewParameters,
                              QString title, const ViewParameters &lastView) const
{
    switch (nextViewParameters.mFilterType)
    {
    case ElisaUtils::NoFilter:
    case ElisaUtils::FilterByRecentlyPlayed:
    case ElisaUtils::FilterByFrequentlyPlayed:
    case ElisaUtils::FilterByPath:
    case ElisaUtils::FilterById:
    case ElisaUtils::UnknownFilter:
        break;
    case ElisaUtils::FilterByGenre:
        nextViewParameters.mDataFilter[DataTypes::GenreRole] = std::move(title);
        break;
    case ElisaUtils::FilterByGenreAndArtist:
        nextViewParameters.mDataFilter = lastView.mDataFilter;
        nextViewParameters.mDataFilter[DataTypes::ArtistRole] = std::move(title);
        break;
    case ElisaUtils::FilterByArtist:
        nextViewParameters.mDataFilter[DataTypes::ArtistRole] = std::move(title);
        break;
    }
}

QString ViewManager::buildViewId() const
{
    const auto &entryTypeMetaEnum = QMetaEnum::fromType<ElisaUtils::PlayListEntryType>();
    QString viewId;

    for (const auto &oneView : qAsConst(d->mViewParametersStack)) {
        viewId += QString::fromLatin1(entryTypeMetaEnum.valueToKey(oneView.mDataType)) + QStringLiteral("::");
    }

    return viewId;
}

QStringList::iterator ViewManager::findViewPreference(QStringList &list, const QString &viewId) const
{
    auto itViewPreference = list.end();
    for(itViewPreference = list.begin(); itViewPreference != list.end(); ++itViewPreference) {
        auto parts = itViewPreference->splitRef(QStringLiteral("=="));
        if (parts.size() != 2) {
            continue;
        }
        if (parts[0] == viewId) {
            break;
        }
    }

    return itViewPreference;
}

Qt::SortOrder ViewManager::computePreferredSortOrder(Qt::SortOrder initialValue) const
{
    auto currentSortOrderPreferences = Elisa::ElisaConfiguration::sortOrderPreferences();
    auto viewId = buildViewId();
    auto itViewPreference = findViewPreference(currentSortOrderPreferences, viewId);

    if (itViewPreference != currentSortOrderPreferences.end()) {
        auto result = itViewPreference->splitRef(QStringLiteral("=="));
        if (result.size() == 2) {
            const auto &sortOrderMetaEnum = QMetaEnum::fromType<Qt::SortOrder>();
            bool conversionOk;
            auto newValue = static_cast<Qt::SortOrder>(sortOrderMetaEnum.keyToValue(result[1].toLatin1().data(), &conversionOk));
            if (conversionOk) {
                initialValue = newValue;
            }
        }
    }

    return initialValue;
}

int ViewManager::computePreferredSortRole(int initialValue) const
{
    auto currentSortRolePreferences = Elisa::ElisaConfiguration::sortRolePreferences();
    auto viewId = buildViewId();
    auto itViewPreference = findViewPreference(currentSortRolePreferences, viewId);

    if (itViewPreference != currentSortRolePreferences.end()) {
        auto result = itViewPreference->splitRef(QStringLiteral("=="));
        if (result.size() == 2) {
            const auto &sortRoleMetaEnum = QMetaEnum::fromType<DataTypes::ColumnsRoles>();
            bool conversionOk;
            auto newValue = static_cast<DataTypes::ColumnsRoles>(sortRoleMetaEnum.keyToValue(result[1].toLatin1().data(), &conversionOk));
            if (conversionOk) {
                initialValue = newValue;
            }
        }
    }

    return initialValue;
}

void ViewManager::goBack()
{
    qCDebug(orgKdeElisaViews()) << "ViewManager::goBack" << d->mViewParametersStack.size()
                                << d->mViewsListData;

    if (d->mViewParametersStack.size() <= 1) {
        return;
    }

    Q_EMIT popOneView();

    if (d->mViewParametersStack.size() > 1) {
        d->mViewParametersStack.pop_back();
        d->mNextViewParameters = {};
    }

    qCDebug(orgKdeElisaViews()) << "ViewManager::goBack" << d->mViewParametersStack.size();
}

void ViewManager::setViewsData(ViewsListData *viewsData)
{
    if (d->mViewsListData == viewsData) {
        return;
    }

    d->mViewsListData = viewsData;
    Q_EMIT viewsDataChanged();

    if (d->mViewsListData && (d->mViewIndex < 0 || d->mViewIndex >= d->mViewsListData->count())) {
        d->mViewIndex = d->mInitialIndex;
    }

    if (d->mViewsListData && d->mViewIndex >= 0 && d->mViewIndex < d->mViewsListData->count() && d->mViewsListData->isFullyInitialized()) {
        openView(d->mViewIndex);
    }

    if (d->mViewsListData) {
        connect(d->mViewsListData, &ViewsListData::isFullyInitializedChanged,
                this, &ViewManager::openInitialView);
    }
}

void ViewManager::setInitialIndex(int newIndex)
{
    if (d->mInitialIndex == newIndex) {
        return;
    }

    d->mInitialIndex = newIndex;
    Q_EMIT initialIndexChanged();

    if (d->mViewsListData && (d->mViewIndex < 0 || d->mViewIndex >= d->mViewsListData->count())) {
        d->mViewIndex = d->mInitialIndex;
    }

    if (d->mViewsListData && d->mViewIndex >= 0 && d->mViewIndex < d->mViewsListData->count() && d->mViewsListData->isFullyInitialized()) {
        openView(d->mViewIndex);
    }
}

void ViewManager::openInitialView()
{
    openView(d->mInitialIndex);
}

void ViewManager::sortOrderChanged(Qt::SortOrder sortOrder)
{
    auto currentSortOrderPreferences = Elisa::ElisaConfiguration::sortOrderPreferences();

    auto viewId = buildViewId();
    auto itViewPreference = findViewPreference(currentSortOrderPreferences, viewId);

    const auto &sortOrderMetaEnum = QMetaEnum::fromType<Qt::SortOrder>();
    auto enumStringValue = sortOrderMetaEnum.valueToKey(sortOrder);
    if (!enumStringValue) {
        return;
    }
    QString newSortOrderPreference = viewId + QStringLiteral("==") + QString::fromLatin1(enumStringValue);

    if (itViewPreference != currentSortOrderPreferences.end()) {
        (*itViewPreference) = newSortOrderPreference;
    } else {
        currentSortOrderPreferences.push_back(newSortOrderPreference);
    }

    Elisa::ElisaConfiguration::setSortOrderPreferences(currentSortOrderPreferences);
    Elisa::ElisaConfiguration::self()->save();
}

void ViewManager::sortRoleChanged(int sortRole)
{
    auto currentSortRolePreferences = Elisa::ElisaConfiguration::sortRolePreferences();

    auto viewId = buildViewId();
    auto itViewPreference = findViewPreference(currentSortRolePreferences, viewId);

    const auto &sortRoleMetaEnum = QMetaEnum::fromType<DataTypes::ColumnsRoles>();
    auto enumStringValue = sortRoleMetaEnum.valueToKey(static_cast<DataTypes::ColumnsRoles>(sortRole));
    if (!enumStringValue) {
        return;
    }
    QString newSortRolePreference = viewId + QStringLiteral("==") + QString::fromLatin1(enumStringValue);

    if (itViewPreference != currentSortRolePreferences.end()) {
        (*itViewPreference) = newSortRolePreference;
    } else {
        currentSortRolePreferences.push_back(newSortRolePreference);
    }

    Elisa::ElisaConfiguration::setSortRolePreferences(currentSortRolePreferences);
    Elisa::ElisaConfiguration::self()->save();
}


#include "moc_viewmanager.cpp"
