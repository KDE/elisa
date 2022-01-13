/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "simpleviewmanager.h"

#include "viewmanager.h"
#include "viewslistdata.h"
#include "viewconfigurationdata.h"
#include "datatypes.h"
#include "viewsLogging.h"
#include "models/datamodel.h"
#include "models/gridviewproxymodel.h"

#if KF5KIO_FOUND
#include "models/filebrowsermodel.h"
#include "models/filebrowserproxymodel.h"
#endif

#include "elisa_settings.h"

#include <KLocalizedString>

#include <QQmlEngine>
#include <QMetaEnum>

class SimpleViewManagerPrivate
{
public:
    ViewsListData *mViewsListData = nullptr;

    QMap<ElisaUtils::PlayListEntryType, ViewParameters> mChildViews = {
        {ElisaUtils::Album, ViewParameters{{},
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
        {ElisaUtils::Artist, ViewParameters{{},
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
                                            QUrl{QStringLiteral("image://icon/media-default-album")},
                                            ViewManager::DelegateWithSecondaryText,
                                            ViewManager::ViewShowRating,
                                            ViewManager::IsFlatModel}},
        {ElisaUtils::Container, ViewParameters{{},
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

    ViewParameters mNextViewParameters;
};

SimpleViewManager::SimpleViewManager(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<SimpleViewManagerPrivate>())
{
}

ViewsListData *SimpleViewManager::viewsData() const
{
    return d->mViewsListData;
}

SimpleViewManager::~SimpleViewManager() = default;

void SimpleViewManager::openView(ElisaUtils::PlayListEntryType viewType)
{
    qCDebug(orgKdeElisaViews()) << "SimpleViewManager::openView" << viewType << d->mViewsListData;

    if (!d->mViewsListData || d->mViewsListData->isEmpty() || !d->mViewsListData->isFullyInitialized()) {
        return;
    }

    if (viewType < 0 || viewType >= d->mViewsListData->count()) {
        viewType = ElisaUtils::Album;
    }

    const auto &viewParameters = d->mViewsListData->viewParameters(viewType);

    qCDebug(orgKdeElisaViews()) << "SimpleViewManager::openView" << "selected view" << viewType;

    qCDebug(orgKdeElisaViews()) << "SimpleViewManager::openView" << "changing view";

    d->mNextViewParameters = viewParameters;
    applyFilter(d->mNextViewParameters, viewParameters.mMainTitle, d->mNextViewParameters);
    openViewFromData(d->mNextViewParameters);
}

void SimpleViewManager::openChildView(const DataTypes::MusicDataType &fullData)
{
    qCDebug(orgKdeElisaViews()) << "SimpleViewManager::openChildView"
                                << fullData[DataTypes::TitleRole]
                                << fullData[DataTypes::SecondaryTextRole]
                                << fullData[DataTypes::ImageUrlRole]
                                << fullData[DataTypes::DatabaseIdRole]
                                << fullData[DataTypes::FilePathRole]
                                << fullData[DataTypes::ElementTypeRole]
                                << fullData[DataTypes::IsSingleDiscAlbumRole];

    if (!d->mViewsListData || d->mViewsListData->isEmpty() || !d->mViewsListData->isFullyInitialized()) {
        return;
    }

    auto dataType = fullData[DataTypes::ElementTypeRole].value<ElisaUtils::PlayListEntryType>();
    auto title = fullData[DataTypes::TitleRole].toString();
    auto innerSecondaryTitle = fullData[DataTypes::ArtistRole].toString();
    auto innerImage = fullData[DataTypes::ImageUrlRole].toUrl();

    auto nextViewParameters = d->mChildViews[dataType];

    nextViewParameters.mMainTitle = title;
    nextViewParameters.mSecondaryTitle = innerSecondaryTitle;
    nextViewParameters.mMainImage = innerImage;
    nextViewParameters.mDataFilter = fullData;

    applyFilter(nextViewParameters, title, {});

    if (dataType == ElisaUtils::Album && nextViewParameters.mDataFilter[DataTypes::IsSingleDiscAlbumRole].toBool())
    {
        nextViewParameters.mAlbumViewStyle = ViewManager::NoDiscHeaders;
    }

    d->mNextViewParameters = nextViewParameters;

    openViewFromData(nextViewParameters);
}

void SimpleViewManager::viewIsLoaded()
{
    qCDebug(orgKdeElisaViews()) << "SimpleViewManager::viewIsLoaded" << d->mViewsListData;

    if (d->mNextViewParameters.mIsValid) {
        openViewFromData(d->mNextViewParameters);
    }
}

void SimpleViewManager::openViewFromData(const ViewParameters &viewParamaters)
{
    qCDebug(orgKdeElisaViews()) << "SimpleViewManager::openViewFromData";

    QAbstractItemModel *newModel = nullptr;
    QAbstractProxyModel *proxyModel = nullptr;

    switch (viewParamaters.mModelType)
    {
    case ViewManager::FileBrowserModel:
    {
#if KF5KIO_FOUND
        newModel = new ::FileBrowserModel;
        auto *realProxyModel = new FileBrowserProxyModel;
        proxyModel = realProxyModel;
#else
        newModel = nullptr;
        proxyModel = nullptr;
#endif
        break;
    }
    case ViewManager::GenericDataModel:
        newModel = new DataModel;
        proxyModel = new GridViewProxyModel;
        break;
    case ViewManager::UnknownModelType:
        qCDebug(orgKdeElisaViews()) << "SimpleViewManager::openViewFromData" << "unknown model type";
        break;
    }

    QQmlEngine::setObjectOwnership(newModel, QQmlEngine::JavaScriptOwnership);
    QQmlEngine::setObjectOwnership(proxyModel, QQmlEngine::JavaScriptOwnership);

    switch (viewParamaters.mViewPresentationType)
    {
    case ViewManager::ViewPresentationType::GridView:
    {
        qCDebug(orgKdeElisaViews()) << "SimpleViewManager::openViewFromData" << viewParamaters.mViewPresentationType
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
                                                                         computePreferredSortRole(viewParamaters, viewParamaters.mSortRole),
                                                                         viewParamaters.mSortRoles, viewParamaters.mSortRoleNames,
                                                                         computePreferredSortOrder(viewParamaters, viewParamaters.mSortOrder),
                                                                         viewParamaters.mSortOrderNames,
                                                                         viewParamaters.mViewCanBeRated, viewParamaters.mShowSecondaryTextOnDelegates,
                                                                         viewParamaters.mIsTreeModel);

        QQmlEngine::setObjectOwnership(configurationData.get(), QQmlEngine::JavaScriptOwnership);

        Q_EMIT openGridView(configurationData.release());
        break;
    }
    case ViewManager::ViewPresentationType::ListView:
    {
        qCDebug(orgKdeElisaViews()) << "SimpleViewManager::openViewFromData" << viewParamaters.mFilterType
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
                                                                         computePreferredSortRole(viewParamaters, viewParamaters.mSortRole),
                                                                         viewParamaters.mSortRoles, viewParamaters.mSortRoleNames,
                                                                         computePreferredSortOrder(viewParamaters, viewParamaters.mSortOrder),
                                                                         viewParamaters.mSortOrderNames,
                                                                         viewParamaters.mAlbumCardinality, viewParamaters.mAlbumViewStyle,
                                                                         viewParamaters.mRadioSpecificStyle, viewParamaters.mIsTreeModel);

        QQmlEngine::setObjectOwnership(configurationData.get(), QQmlEngine::JavaScriptOwnership);

        Q_EMIT openListView(configurationData.release());
        break;
    }
    case ViewManager::ContextView:
        qCDebug(orgKdeElisaViews()) << "SimpleViewManager::openViewFromData" << viewParamaters.mViewPresentationType
                                    << viewParamaters.mDepth << viewParamaters.mMainTitle
                                    << viewParamaters.mMainImage;
        Q_EMIT switchContextView(viewParamaters.mDepth, viewParamaters.mMainTitle, viewParamaters.mMainImage);
        break;
    case ViewManager::UnknownViewPresentation:
        qCDebug(orgKdeElisaViews()) << "SimpleViewManager::openViewFromData" << "unknown view type";
        break;
    }
}

void SimpleViewManager::applyFilter(ViewParameters &nextViewParameters,
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

QString SimpleViewManager::buildViewId(const ViewParameters &oneView) const
{
    const auto &entryTypeMetaEnum = QMetaEnum::fromType<ElisaUtils::PlayListEntryType>();

    return QString::fromLatin1(entryTypeMetaEnum.valueToKey(oneView.mDataType)) + QStringLiteral("::");
}

QStringList::iterator SimpleViewManager::findViewPreference(QStringList &list, const QString &viewId) const
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

Qt::SortOrder SimpleViewManager::computePreferredSortOrder(const ViewParameters &oneView,
                                                           Qt::SortOrder initialValue) const
{
    auto currentSortOrderPreferences = Elisa::ElisaConfiguration::sortOrderPreferences();
    auto viewId = buildViewId(oneView);
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

int SimpleViewManager::computePreferredSortRole(const ViewParameters &oneView,
                                                int initialValue) const
{
    auto currentSortRolePreferences = Elisa::ElisaConfiguration::sortRolePreferences();
    auto viewId = buildViewId(oneView);
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

void SimpleViewManager::setViewsData(ViewsListData *viewsData)
{
    if (d->mViewsListData == viewsData) {
        return;
    }

    d->mViewsListData = viewsData;
    Q_EMIT viewsDataChanged();
}

#include "moc_simpleviewmanager.cpp"
