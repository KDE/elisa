/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "viewmanager.h"

#include "viewslistdata.h"
#include "viewconfigurationdata.h"
#include "viewsLogging.h"
#include "models/datamodel.h"
#include "models/gridviewproxymodel.h"

#if KFKIO_FOUND
#include "models/filebrowsermodel.h"
#include "models/filebrowserproxymodel.h"
#endif

#include "elisa_settings.h"

#include <KLocalizedString>

#include <QQmlEngine>
#include <QMetaEnum>

class ViewManagerPrivate
{
public:
    ViewsListData *mViewsListData = nullptr;

    QMap<ElisaUtils::PlayListEntryType, ViewParameters> mChildViews = {
        {ElisaUtils::Album, {{},
                             QUrl{QStringLiteral("image://icon/view-media-track")},
                             ViewManager::TrackView,
                             ViewManager::GenericDataModel,
                             ElisaUtils::FilterById,
                             ElisaUtils::Track,
                             DataTypes::TrackNumberRole,
                             {DataTypes::TrackNumberRole, Qt::DisplayRole},
                             {i18nc("@title:inmenu", "Track Number"), i18nc("@title:inmenu", "Title")},
                             Qt::AscendingOrder,
                             {i18nc("@item:inmenu", "Ascending"), i18nc("@item:inmenu", "Descending"),
                              i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A")},
                             ViewManager::SingleAlbum,
                             ViewManager::DiscHeaders}},
        {ElisaUtils::Genre, {{},
                             QUrl{QStringLiteral("image://icon/view-media-artist")},
                             ViewManager::GridView,
                             ViewManager::GenericDataModel,
                             ElisaUtils::FilterByGenre,
                             ElisaUtils::Artist,
                             Qt::DisplayRole,
                             {Qt::DisplayRole},
                             {i18nc("@title:inmenu", "Name")},
                             Qt::AscendingOrder,
                             {i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A")},
                             QUrl{QStringLiteral("image://icon/view-media-artist")},
                             ViewManager::DelegateWithoutSecondaryText,
                             ViewManager::ViewHideRating}},
        {ElisaUtils::Artist, {{},
                              QUrl{QStringLiteral("image://icon/view-media-album-cover")},
                              ViewManager::GridView,
                              ViewManager::GenericDataModel,
                              ElisaUtils::FilterByArtist,
                              ElisaUtils::Album,
                              DataTypes::TitleRole,
                              {DataTypes::TitleRole, DataTypes::GenreRole, DataTypes::YearRole},
                              {i18nc("@title:inmenu", "Title"), i18nc("@title:inmenu", "Genre"), i18nc("@title:inmenu", "Year")},
                              Qt::AscendingOrder,
                              {i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A"), i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A"), i18nc("@item:inmenu", "Oldest First"), i18nc("@item:inmenu", "Newest First")},
                              QUrl{QStringLiteral("image://icon/media-default-album")},
                              ViewManager::DelegateWithSecondaryText,
                              ViewManager::ViewShowRating}},
        {ElisaUtils::Container, {{},
                                 QUrl{QStringLiteral("image://icon/folder")},
                                 ViewManager::GridView,
                                 ViewManager::FileBrowserModel,
                                 ElisaUtils::FilterByPath,
                                 ElisaUtils::FileName,
                                 Qt::DisplayRole,
                                 {Qt::DisplayRole},
                                 {i18nc("@title:inmenu", "Name")},
                                 Qt::AscendingOrder,
                                 {i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A")},
                                 QUrl{QStringLiteral("image://icon/folder")},
                                 ViewManager::DelegateWithoutSecondaryText,
                                 ViewManager::ViewHideRating}},
    };

    int mViewIndex = -1;

    int mInitialIndex = -1;

    QString mInitialFilesViewPath = QDir::rootPath();

    QList<ViewParameters> mViewParametersStack = (mViewsListData ? QList<ViewParameters>{mViewsListData->viewParameters(0)} : QList<ViewParameters>{});
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

QString ViewManager::initialFilesViewPath() const
{
    return d->mInitialFilesViewPath;
}

ViewManager::~ViewManager() = default;

void ViewManager::openView(int viewIndex)
{
    qCDebug(orgKdeElisaViews()) << "ViewManager::openView" << viewIndex << d->mViewParametersStack.size() << d->mViewsListData;

    if (!d->mViewsListData || d->mViewsListData->isEmpty()) {
        return;
    }

    d->mViewParametersStack.clear();

    if (viewIndex < 0 || viewIndex >= d->mViewsListData->count()) {
        viewIndex = 0;
    }

    auto viewParameters = d->mViewsListData->viewParameters(viewIndex);

    qCDebug(orgKdeElisaViews()) << "ViewManager::openView" << "changing view" << viewIndex;

    d->mViewIndex = viewIndex;
    Q_EMIT viewIndexChanged();

    applyFilter(viewParameters, viewParameters.mMainTitle, viewParameters);
    if (viewParameters.mDataType == ElisaUtils::FileName) { // Folder view
        viewParameters.mDataFilter[DataTypes::FilePathRole] = QUrl::fromLocalFile(d->mInitialFilesViewPath);
    }
    openViewFromData(viewParameters);
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

    if (!d->mViewsListData || d->mViewsListData->isEmpty()) {
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

    // Prevent the user from clicking the same view other and over again
    if (this->isSameView(nextViewParameters, lastView)) {
        return;
    }

    if (dataType == ElisaUtils::Album && nextViewParameters.mDataFilter[DataTypes::IsSingleDiscAlbumRole].toBool())
    {
        nextViewParameters.mAlbumViewStyle = NoDiscHeaders;
    }

    openViewFromData(nextViewParameters);
}

void ViewManager::openAlbumView(const QString &title, const QString &artist, qulonglong databaseId, const QUrl &albumCoverUrl)
{
    openChildView({{DataTypes::ElementTypeRole, ElisaUtils::Album},
                   {DataTypes::DatabaseIdRole, databaseId},
                   {DataTypes::TitleRole, title},
                   {DataTypes::ArtistRole, artist},
                   {DataTypes::ImageUrlRole, albumCoverUrl},});
}

void ViewManager::openArtistView(const QString &artist)
{
    openChildView({{DataTypes::ElementTypeRole, ElisaUtils::Artist},
                   {DataTypes::TitleRole, artist},});
}

void ViewManager::openNowPlaying()
{
    openView(0);
}

void ViewManager::openViewFromData(const ViewParameters &viewParamaters)
{
    qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << d->mViewParametersStack.size();

    QAbstractItemModel *newModel = nullptr;
    QAbstractProxyModel *proxyModel = nullptr;

    switch (viewParamaters.mModelType)
    {
    case FileBrowserModel:
    {
#if KFKIO_FOUND
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
                                    << viewParamaters.mViewCanBeRated << viewParamaters.mShowSecondaryTextOnDelegates;

        auto configurationData = std::make_unique<ViewConfigurationData>(viewParamaters.mFilterType, viewParamaters.mDepth,
                                                                         viewParamaters.mMainTitle, viewParamaters.mSecondaryTitle,
                                                                         viewParamaters.mMainImage, viewParamaters.mDataType,
                                                                         newModel, proxyModel, viewParamaters.mFallbackItemIcon,
                                                                         viewParamaters.mDataFilter,
                                                                         computePreferredSortRole(viewParamaters.mSortRole, viewParamaters.mFilterType),
                                                                         viewParamaters.mSortRoles, viewParamaters.mSortRoleNames,
                                                                         computePreferredSortOrder(viewParamaters.mSortOrder, viewParamaters.mFilterType),
                                                                         viewParamaters.mSortOrderNames,
                                                                         viewParamaters.mViewCanBeRated, viewParamaters.mShowSecondaryTextOnDelegates);

        QQmlEngine::setObjectOwnership(configurationData.get(), QQmlEngine::JavaScriptOwnership);

        Q_EMIT openGridView(configurationData.release());
        break;
    }
    case ViewPresentationType::TrackView:
    {
        qCDebug(orgKdeElisaViews()) << "ViewManager::openViewFromData" << viewParamaters.mFilterType
                                    << viewParamaters.mDepth << viewParamaters.mMainTitle << viewParamaters.mSecondaryTitle
                                    << viewParamaters.mDataFilter[DataTypes::DatabaseIdRole] << viewParamaters.mMainImage
                                    << viewParamaters.mModelType << viewParamaters.mDataType
                                    << viewParamaters.mDataFilter << viewParamaters.mSortRole
                                    << viewParamaters.mSortOrder << viewParamaters.mAlbumCardinality
                                    << viewParamaters.mAlbumViewStyle;

        auto configurationData = std::make_unique<ViewConfigurationData>(viewParamaters.mFilterType, viewParamaters.mDepth,
                                                                         viewParamaters.mMainTitle, viewParamaters.mSecondaryTitle,
                                                                         viewParamaters.mMainImage, viewParamaters.mDataType,
                                                                         newModel, proxyModel, viewParamaters.mDataFilter,
                                                                         computePreferredSortRole(viewParamaters.mSortRole, viewParamaters.mFilterType),
                                                                         viewParamaters.mSortRoles, viewParamaters.mSortRoleNames,
                                                                         computePreferredSortOrder(viewParamaters.mSortOrder, viewParamaters.mFilterType),
                                                                         viewParamaters.mSortOrderNames,
                                                                         viewParamaters.mAlbumCardinality, viewParamaters.mAlbumViewStyle);

        QQmlEngine::setObjectOwnership(configurationData.get(), QQmlEngine::JavaScriptOwnership);

        Q_EMIT openTrackView(configurationData.release());
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

    const auto currView = d->mViewParametersStack.back();
    const auto viewId = QString::fromLatin1(entryTypeMetaEnum.valueToKey(currView.mDataType));

    return viewId;
}

QStringList::iterator ViewManager::findViewPreference(QStringList &list, const QString &viewId) const
{
    auto itViewPreference = list.end();
    for(itViewPreference = list.begin(); itViewPreference != list.end(); ++itViewPreference) {
        auto parts = QStringView(*itViewPreference).split(QStringLiteral("=="));
        if (parts.size() != 2) {
            continue;
        }
        if (parts[0] == viewId) {
            break;
        }
    }

    return itViewPreference;
}

template<typename T>
T ViewManager::computeViewPreference(const T initialValue, QStringList &preferences) const
{
    const auto viewId = buildViewId();
    const auto itViewPreference = findViewPreference(preferences, viewId);

    if (itViewPreference != preferences.end()) {
        const auto result = QStringView(*itViewPreference).split(QStringLiteral("=="));
        if (result.size() == 2) {
            const auto &sortOrderMetaEnum = QMetaEnum::fromType<T>();
            bool conversionOk;
            auto newValue = static_cast<T>(sortOrderMetaEnum.keyToValue(result[1].toLatin1().data(), &conversionOk));
            if (conversionOk) {
                return newValue;
            }
        }
    }

    return initialValue;
}

template<typename T>
void ViewManager::updateViewPreference(const T newValue, QStringList &preferences) const
{
    const auto viewId = buildViewId();
    const auto itViewPreference = findViewPreference(preferences, viewId);

    const auto &sortOrderMetaEnum = QMetaEnum::fromType<T>();
    const auto enumStringValue = sortOrderMetaEnum.valueToKey(newValue);
    if (!enumStringValue) {
        return;
    }
    const QString newSortOrderPreference = viewId + QStringLiteral("==") + QString::fromLatin1(enumStringValue);

    if (itViewPreference != preferences.end()) {
        (*itViewPreference) = newSortOrderPreference;
    } else {
        preferences.push_back(newSortOrderPreference);
    }
}

bool ViewManager::viewHasDefaultSortRole(const ElisaUtils::FilterType filterType) const
{
    switch (filterType)
    {
    case ElisaUtils::FilterByRecentlyPlayed:
    case ElisaUtils::FilterByFrequentlyPlayed:
    case ElisaUtils::FilterById: // Tracks in album view
        return true;
    default:
        return false;
    }
}

Qt::SortOrder ViewManager::computePreferredSortOrder(Qt::SortOrder initialValue, ElisaUtils::FilterType filterType) const
{
    if (viewHasDefaultSortRole(filterType)) {
        return initialValue;
    }

    auto currentSortOrderPreferences = Elisa::ElisaConfiguration::sortOrderPreferences();
    return computeViewPreference(initialValue, currentSortOrderPreferences);
}

int ViewManager::computePreferredSortRole(int initialValue, ElisaUtils::FilterType filterType) const
{
    if (viewHasDefaultSortRole(filterType)) {
        return initialValue;
    }

    auto currentSortRolePreferences = Elisa::ElisaConfiguration::sortRolePreferences();
    return computeViewPreference(static_cast<DataTypes::ColumnsRoles>(initialValue), currentSortRolePreferences);
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

    if (d->mViewsListData && d->mViewIndex >= 0 && d->mViewIndex < d->mViewsListData->count()) {
        openView(d->mViewIndex);
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

    if (d->mViewsListData && d->mViewIndex >= 0 && d->mViewIndex < d->mViewsListData->count()) {
        openView(d->mViewIndex);
    }
}

void ViewManager::setInitialFilesViewPath(const QString &initialPath)
{
    if (d->mInitialFilesViewPath == initialPath) {
        return;
    }

    d->mInitialFilesViewPath = initialPath;
    Q_EMIT initialFilesViewPathChanged();
}

void ViewManager::openInitialView()
{
    openView(d->mInitialIndex);
}

void ViewManager::sortOrderChanged(Qt::SortOrder sortOrder)
{
    if (viewHasDefaultSortRole(d->mViewParametersStack.back().mFilterType)) {
        return;
    }

    auto currentSortOrderPreferences = Elisa::ElisaConfiguration::sortOrderPreferences();
    updateViewPreference(sortOrder, currentSortOrderPreferences);

    Elisa::ElisaConfiguration::setSortOrderPreferences(currentSortOrderPreferences);
    Elisa::ElisaConfiguration::self()->save();
}

void ViewManager::sortRoleChanged(int sortRole)
{
    if (viewHasDefaultSortRole(d->mViewParametersStack.back().mFilterType)) {
        return;
    }

    auto currentSortRolePreferences = Elisa::ElisaConfiguration::sortRolePreferences();
    updateViewPreference(static_cast<DataTypes::ColumnsRoles>(sortRole), currentSortRolePreferences);

    Elisa::ElisaConfiguration::setSortRolePreferences(currentSortRolePreferences);
    Elisa::ElisaConfiguration::self()->save();
}

bool ViewManager::isSameView(const ViewParameters &currentView, const ViewParameters &otherView) const {
    if (currentView.mDataType != otherView.mDataType) {
        return false;
    }

    // Best case scenario is matching up database ids
    if (currentView.mDataFilter.hasDatabaseId() && otherView.mDataFilter.hasDatabaseId()) {
        return currentView.mDataFilter.databaseId() == otherView.mDataFilter.databaseId();
    }

    // Unfortunately we don't have access to database ids for most of these types, so handle specific types differently.
    switch (currentView.mDataType) {
        case ElisaUtils::PlayListEntryType::Album:
        case ElisaUtils::PlayListEntryType::Artist:
            return currentView.mMainTitle == otherView.mMainTitle;
        default:
            return false;
    }
}


#include "moc_viewmanager.cpp"
