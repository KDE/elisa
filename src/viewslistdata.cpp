/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "viewslistdata.h"

#include "modeldataloader.h"
#include "databaseinterface.h"
#include "musiclistenersmanager.h"

#include "viewsLogging.h"

#include <KI18n/KLocalizedString>

#include <QUrl>
#include <QDir>

class ViewsListDataPrivate
{
public:
    QList<ViewParameters> mViewsParameters = {{{i18nc("Title of the view of the playlist", "Now Playing")},
                                               QUrl{QStringLiteral("image://icon/view-media-lyrics")},
                                               ViewManager::ContextView},
                                              {{i18nc("Title of the view of recently played tracks", "Recently Played")},
                                               QUrl{QStringLiteral("image://icon/media-playlist-play")},
                                               ViewManager::ListView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::FilterByRecentlyPlayed,
                                               ElisaUtils::Track,
                                               DataTypes::LastPlayDate,
                                               {DataTypes::LastPlayDate},
                                               {i18n("Last Played Date")},
                                               Qt::DescendingOrder,
                                               {i18n("Least Recently Played First"), i18n("Most Recently Played First")},
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack,
                                               ViewManager::IsFlatModel},
                                              {{i18nc("Title of the view of frequently played tracks", "Frequently Played")},
                                               QUrl{QStringLiteral("image://icon/view-media-playcount")},
                                               ViewManager::ListView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::FilterByFrequentlyPlayed,
                                               ElisaUtils::Track,
                                               DataTypes::PlayFrequency,
                                               {DataTypes::PlayFrequency},
                                               {i18n("Play Frequency")},
                                               Qt::DescendingOrder,
                                               {i18n("Least Frequently Played First"), i18n("Most Frequently Played First")},
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack,
                                               ViewManager::IsFlatModel},
                                              {{i18nc("Title of the view of all albums", "Albums")},
                                               QUrl{QStringLiteral("image://icon/view-media-album-cover")},
                                               ViewManager::GridView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Album,
                                               DataTypes::TitleRole,
                                               {DataTypes::TitleRole, DataTypes::ArtistRole, DataTypes::GenreRole, DataTypes::YearRole},
                                               {i18n("Title"), i18n("Artist"), i18n("Genre"), i18n("Year")},
                                               Qt::AscendingOrder,
                                               {i18n("A-Z"), i18n("Z-A"), i18n("A-Z"), i18n("Z-A"), i18n("A-Z"), i18n("Z-A"), i18n("Oldest First"), i18n("Newest First")},
                                               QUrl{QStringLiteral("image://icon/media-optical-audio")},
                                               ViewManager::DelegateWithSecondaryText,
                                               ViewManager::ViewShowRating,
                                               ViewManager::IsFlatModel},
                                              {{i18nc("Title of the view of all artists", "Artists")},
                                               QUrl{QStringLiteral("image://icon/view-media-artist")},
                                               ViewManager::GridView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Artist,
                                               Qt::DisplayRole,
                                               {Qt::DisplayRole},
                                               {i18n("Name")},
                                               Qt::AscendingOrder,
                                               {i18n("A-Z"), i18n("Z-A")},
                                               QUrl{QStringLiteral("image://icon/view-media-artist")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating,
                                               ViewManager::IsFlatModel},
                                              {{i18nc("Title of the view of all tracks", "Tracks")},
                                               QUrl{QStringLiteral("image://icon/view-media-track")},
                                               ViewManager::ListView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Track,
                                               DataTypes::TitleRole,
                                               {DataTypes::TitleRole, DataTypes::AlbumRole, DataTypes::ArtistRole,
                                                DataTypes::GenreRole, DataTypes::YearRole, DataTypes::DurationRole,
                                                DataTypes::ComposerRole, DataTypes::LyricistRole},
                                               {i18n("Title"), i18n("Album"), i18n("Artist"), i18n("Genre"),
                                                i18n("Year"), i18n("Duration"), i18n("Composer"), i18n("Lyricist")},
                                               Qt::AscendingOrder,
                                               {i18n("A-Z"), i18n("Z-A"), i18n("A-Z"), i18n("Z-A"), i18n("A-Z"), i18n("Z-A"), i18n("A-Z"), i18n("Z-A"),
                                                i18n("Oldest First"), i18n("Newest First"), i18n("Shortest First"), i18n("Longest First"), i18n("A-Z"), i18n("Z-A"), i18n("A-Z"), i18n("Z-A")},
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack,
                                               ViewManager::IsFlatModel},
                                              {{i18nc("Title of the view of all genres", "Genres")},
                                               QUrl{QStringLiteral("image://icon/view-media-genre")},
                                               ViewManager::GridView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Genre,
                                               Qt::DisplayRole,
                                               {Qt::DisplayRole},
                                               {i18n("Genre")},
                                               Qt::AscendingOrder,
                                               {i18n("A-Z"), i18n("Z-A")},
                                               QUrl{QStringLiteral("image://icon/view-media-genre")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating,
                                               ViewManager::IsFlatModel},
                                              {{i18nc("Title of the file browser view", "Files")},
                                               QUrl{QStringLiteral("image://icon/folder")},
                                               ViewManager::GridView,
                                               ViewManager::FileBrowserModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::FileName,
                                               QUrl{QStringLiteral("image://icon/folder")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating,
                                               ViewManager::IsFlatModel,
                                               QUrl::fromLocalFile(QDir::homePath())
                                              },
                                              {{i18nc("Title of the file radios browser view", "Radios")},
                                               QUrl{QStringLiteral("image://icon/radio")},
                                               ViewManager::ListView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Radio,
                                               Qt::DisplayRole,
                                               {Qt::DisplayRole},
                                               {i18n("Name")},
                                               Qt::AscendingOrder,
                                               {i18n("A-Z"), i18n("Z-A")},
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsRadio,
                                               ViewManager::IsFlatModel}};

    QMap<ElisaUtils::PlayListEntryType, QUrl> mDefaultIcons = {{ElisaUtils::Album, QUrl{QStringLiteral("image://icon/view-media-album-cover")}},
                                                               {ElisaUtils::Artist, QUrl{QStringLiteral("image://icon/view-media-artist")}},
                                                               {ElisaUtils::Genre, QUrl{QStringLiteral("image://icon/view-media-genre")}},};

    ElisaUtils::PlayListEntryType mEmbeddedCategory = ElisaUtils::Unknown;

    ElisaUtils::PlayListEntryType mOldEmbeddedCategory = ElisaUtils::Unknown;

    ModelDataLoader *mDataLoader = nullptr;

    MusicListenersManager *mManager = nullptr;

    DatabaseInterface *mDatabase = nullptr;

    bool mIsFullyInitialized = true;
};

ViewsListData::ViewsListData(QObject *parent) : QObject(parent), d(std::make_unique<ViewsListDataPrivate>())
{
    d->mDataLoader = new ModelDataLoader;
    connect(this, &ViewsListData::destroyed, d->mDataLoader, &ModelDataLoader::deleteLater);
}

ViewsListData::~ViewsListData() = default;

int ViewsListData::count() const
{
    return d->mViewsParameters.count();
}

bool ViewsListData::isEmpty() const
{
    return d->mViewsParameters.isEmpty();
}

const ViewParameters &ViewsListData::viewParameters(int index) const
{
    return d->mViewsParameters[index];
}

const QString &ViewsListData::title(int index) const
{
    return d->mViewsParameters[index].mMainTitle;
}

const QString &ViewsListData::secondTitle(int index) const
{
    return d->mViewsParameters[index].mSecondaryTitle;
}

bool ViewsListData::useSecondTitle(int index) const
{
    return d->mViewsParameters[index].mUseSecondTitle;
}

const QUrl &ViewsListData::iconUrl(int index) const
{
    return d->mViewsParameters[index].mMainImage;
}

ElisaUtils::IconUseColorOverlay ViewsListData::iconUseColorOverlay(int index) const
{
    return d->mViewsParameters[index].mIconUseColorOverlay;
}

qulonglong ViewsListData::databaseId(int index) const
{
    return d->mViewsParameters[index].mDataFilter.databaseId();
}

bool ViewsListData::defaultEntry(int index) const
{
    return d->mEmbeddedCategory == ElisaUtils::Unknown || d->mViewsParameters[index].mEntryType != d->mEmbeddedCategory;
}

int ViewsListData::indexFromEmbeddedDatabaseId(qulonglong databaseId) const
{
    auto result = std::find_if(d->mViewsParameters.begin(), d->mViewsParameters.end(), [this, databaseId](auto data) {
        return d->mEmbeddedCategory == data.mEntryType && data.mDataFilter.databaseId() == databaseId;
    });

    if (result == d->mViewsParameters.end()) {
        return -1;
    }

    return result - d->mViewsParameters.begin();
}

int ViewsListData::indexFromEmbeddedName(const QString &name) const
{
    auto result = std::find_if(d->mViewsParameters.begin(), d->mViewsParameters.end(), [this, name](auto data) {
        return d->mEmbeddedCategory == data.mEntryType && data.mMainTitle == name;
    });

    if (result == d->mViewsParameters.end()) {
        return -1;
    }

    return result - d->mViewsParameters.begin();
}

ElisaUtils::PlayListEntryType ViewsListData::embeddedCategory() const
{
    return d->mEmbeddedCategory;
}

void ViewsListData::setEmbeddedCategory(ElisaUtils::PlayListEntryType aEmbeddedView)
{
    if (d->mEmbeddedCategory != aEmbeddedView)
    {
        d->mOldEmbeddedCategory = d->mEmbeddedCategory;
        d->mEmbeddedCategory = aEmbeddedView;

        Q_EMIT embeddedCategoryChanged();

        refreshEmbeddedCategory();

        switch (d->mEmbeddedCategory) {
        case ElisaUtils::Composer:
        case ElisaUtils::Container:
        case ElisaUtils::FileName:
        case ElisaUtils::Lyricist:
        case ElisaUtils::Radio:
        case ElisaUtils::Track:
        case ElisaUtils::Unknown:
            d->mIsFullyInitialized = true;
            Q_EMIT isFullyInitializedChanged();
            break;
        case ElisaUtils::Album:
        case ElisaUtils::Artist:
        case ElisaUtils::Genre:
            break;
        }
    }
}

void ViewsListData::genresAdded(const DataTypes::ListGenreDataType &newData)
{
    if (d->mEmbeddedCategory != ElisaUtils::Genre) {
        return;
    }

    if (!newData.size()) {
        return;
    }

    Q_EMIT dataAboutToBeAdded(d->mViewsParameters.size(), d->mViewsParameters.size() + newData.size() - 1);
    for (const auto &oneGenre : newData) {
        d->mViewsParameters.push_back({oneGenre.title(),
                                       QUrl{QStringLiteral("image://icon/view-media-genre")},
                                       ViewManager::GridView,
                                       ViewManager::GenericDataModel,
                                       ElisaUtils::FilterByGenre,
                                       oneGenre.databaseId(),
                                       ElisaUtils::Artist,
                                       ElisaUtils::Genre,
                                       QUrl{QStringLiteral("image://icon/view-media-artist")},
                                       ViewManager::DelegateWithoutSecondaryText,
                                       ViewManager::ViewHideRating});
    }
    Q_EMIT dataAdded();

    d->mIsFullyInitialized = true;
    Q_EMIT isFullyInitializedChanged();
}

void ViewsListData::artistsAdded(const DataTypes::ListArtistDataType &newData)
{
    if (d->mEmbeddedCategory != ElisaUtils::Artist) {
        return;
    }

    if (!newData.size()) {
        return;
    }

    Q_EMIT dataAboutToBeAdded(d->mViewsParameters.size(), d->mViewsParameters.size() + newData.size() - 1);
    for (const auto &oneArtist : newData) {
        d->mViewsParameters.push_back({oneArtist.name(),
                                       QUrl{QStringLiteral("image://icon/view-media-artist")},
                                       ViewManager::GridView,
                                       ViewManager::GenericDataModel,
                                       ElisaUtils::FilterByArtist,
                                       oneArtist.databaseId(),
                                       ElisaUtils::Album,
                                       ElisaUtils::Artist,
                                       QUrl{QStringLiteral("image://icon/media-optical-audio")},
                                       ViewManager::DelegateWithSecondaryText,
                                       ViewManager::ViewShowRating});
    }
    Q_EMIT dataAdded();

    d->mIsFullyInitialized = true;
    Q_EMIT isFullyInitializedChanged();
}

void ViewsListData::artistRemoved(qulonglong removedDatabaseId)
{
    if (d->mEmbeddedCategory != ElisaUtils::Artist) {
        return;
    }

    for (int i = 0; i < d->mViewsParameters.count(); ++i) {
        const auto &data = d->mViewsParameters[i];

        if (data.mDataFilter.databaseId() == removedDatabaseId) {
            Q_EMIT dataAboutToBeRemoved(i, i);
            d->mViewsParameters.removeAt(i);
            Q_EMIT dataRemoved();
            break;
        }
    }
}

void ViewsListData::albumsAdded(const DataTypes::ListAlbumDataType &newData)
{
    if (d->mEmbeddedCategory != ElisaUtils::Album) {
        return;
    }

    if (!newData.size()) {
        return;
    }

    Q_EMIT dataAboutToBeAdded(d->mViewsParameters.size(), d->mViewsParameters.size() + newData.size() - 1);
    for (const auto &oneAlbum : newData) {
        ElisaUtils::IconUseColorOverlay useColorOverlay = ElisaUtils::DoNotUseColorOverlay;
        auto coverImage = oneAlbum.albumArtURI();
        if (!coverImage.isValid()) {
            coverImage = d->mDefaultIcons[d->mEmbeddedCategory];
            useColorOverlay = ElisaUtils::UseColorOverlay;
        }

        d->mViewsParameters.push_back({oneAlbum.title(),
                                       coverImage,
                                       ViewManager::ListView,
                                       ViewManager::GenericDataModel,
                                       ElisaUtils::FilterById,
                                       oneAlbum.databaseId(),
                                       ElisaUtils::Track,
                                       ElisaUtils::Album,
                                       Qt::DisplayRole,
                                       {Qt::DisplayRole},
                                       {QStringLiteral("Title")},
                                       Qt::AscendingOrder,
                                       {QStringLiteral("A-Z"), QStringLiteral("Z-A")},
                                       ViewManager::SingleAlbum,
                                       ViewManager::DiscHeaders,
                                       ViewManager::IsTrack,
                                       useColorOverlay});
    }
    Q_EMIT dataAdded();

    d->mIsFullyInitialized = true;
    Q_EMIT isFullyInitializedChanged();
}

void ViewsListData::albumRemoved(qulonglong removedDatabaseId)
{
    if (d->mEmbeddedCategory != ElisaUtils::Album) {
        return;
    }

    for (int i = 0; i < d->mViewsParameters.count(); ++i) {
        const auto &data = d->mViewsParameters[i];

        if (data.mDataFilter.databaseId() == removedDatabaseId) {
            Q_EMIT dataAboutToBeRemoved(i, i);
            d->mViewsParameters.removeAt(i);
            Q_EMIT dataRemoved();
            break;
        }
    }
}

void ViewsListData::albumModified(const DataTypes::AlbumDataType &modifiedAlbum)
{
    if (d->mEmbeddedCategory != ElisaUtils::Album) {
        return;
    }

    for (int i = 0; i < d->mViewsParameters.count(); ++i) {
        auto &data = d->mViewsParameters[i];
        if (data.mDataFilter.databaseId() == modifiedAlbum.databaseId()) {
            data.mMainTitle = modifiedAlbum.title();
            if (modifiedAlbum.albumArtURI().isValid()) {
                data.mMainImage = modifiedAlbum.albumArtURI();
                data.mIconUseColorOverlay = ElisaUtils::DoNotUseColorOverlay;
            } else {
                data.mMainImage = d->mDefaultIcons[d->mEmbeddedCategory];
                data.mIconUseColorOverlay = ElisaUtils::UseColorOverlay;
            }
            data.mSecondaryTitle = modifiedAlbum.artist();

            Q_EMIT dataModified(i);

            break;
        }
    }
}

void ViewsListData::cleanedDatabase()
{
    qCDebug(orgKdeElisaViews) << "ViewsListData::cleanedDatabase" << d->mEmbeddedCategory;

    switch (d->mEmbeddedCategory)
    {
    case ElisaUtils::Album:
    case ElisaUtils::Genre:
    case ElisaUtils::Artist:
        Q_EMIT dataAboutToBeReset();
        for (int i = 0; i < d->mViewsParameters.size(); ) {
            if (d->mViewsParameters.at(i).mEntryType == d->mEmbeddedCategory) {
                d->mViewsParameters.removeAt(i);
            } else {
                ++i;
            }
        }
        Q_EMIT dataReset();
        break;
    case ElisaUtils::Radio:
    case ElisaUtils::Track:
    case ElisaUtils::Unknown:
    case ElisaUtils::Composer:
    case ElisaUtils::FileName:
    case ElisaUtils::Lyricist:
    case ElisaUtils::Container:
        break;
    }
}

void ViewsListData::refreshEmbeddedCategory()
{
    auto elementsCategoryToRemove = ElisaUtils::Unknown;

    switch (d->mOldEmbeddedCategory)
    {
    case ElisaUtils::Album:
    case ElisaUtils::Artist:
    case ElisaUtils::Genre:
        elementsCategoryToRemove = d->mOldEmbeddedCategory;
        for (int i = 0; i < d->mViewsParameters.size(); ) {
            if (d->mViewsParameters.at(i).mEntryType == elementsCategoryToRemove) {
                Q_EMIT dataAboutToBeRemoved(i, i);
                d->mViewsParameters.removeAt(i);
                Q_EMIT dataRemoved();
            } else {
                ++i;
            }
        }
        break;
    case ElisaUtils::Radio:
    case ElisaUtils::Track:
    case ElisaUtils::Unknown:
    case ElisaUtils::Composer:
    case ElisaUtils::FileName:
    case ElisaUtils::Lyricist:
    case ElisaUtils::Container:
        break;
    }

    auto categoryToRemove = ElisaUtils::Unknown;

    switch (d->mEmbeddedCategory)
    {
    case ElisaUtils::Album:
    case ElisaUtils::Genre:
    case ElisaUtils::Artist:
        categoryToRemove = d->mEmbeddedCategory;
        for (int i = 0; i < d->mViewsParameters.size(); ) {
            if (d->mViewsParameters.at(i).mDataType == categoryToRemove) {
                Q_EMIT dataAboutToBeRemoved(i, i);
                d->mViewsParameters.removeAt(i);
                Q_EMIT dataRemoved();
            } else {
                ++i;
            }
        }
        break;
    case ElisaUtils::Radio:
    case ElisaUtils::Track:
    case ElisaUtils::Unknown:
    case ElisaUtils::Composer:
    case ElisaUtils::FileName:
    case ElisaUtils::Lyricist:
    case ElisaUtils::Container:
        break;
    }

    switch (d->mEmbeddedCategory)
    {
    case ElisaUtils::Album:
    case ElisaUtils::Artist:
    case ElisaUtils::Genre:
        Q_EMIT needData(d->mEmbeddedCategory);
        break;
    case ElisaUtils::Radio:
    case ElisaUtils::Track:
    case ElisaUtils::Unknown:
    case ElisaUtils::Composer:
    case ElisaUtils::FileName:
    case ElisaUtils::Lyricist:
    case ElisaUtils::Container:
        break;
    }

    switch (d->mOldEmbeddedCategory)
    {
    case ElisaUtils::Album:
        Q_EMIT dataAboutToBeAdded(3, 3);
        d->mViewsParameters.insert(3, {{i18nc("Title of the view of all albums", "Albums")},
                                       QUrl{QStringLiteral("image://icon/view-media-album-cover")},
                                       ViewManager::GridView,
                                       ViewManager::GenericDataModel,
                                       ElisaUtils::NoFilter,
                                       ElisaUtils::Album,
                                       Qt::DisplayRole,
                                       {DataTypes::TitleRole, DataTypes::ArtistRole, DataTypes::GenreRole, DataTypes::YearRole},
                                       {i18n("Title"), i18n("Artist"), i18n("Genre"), i18n("Year")},
                                       Qt::AscendingOrder,
                                       {i18n("A-Z"), i18n("Z-A"), i18n("A-Z"), i18n("Z-A"), i18n("A-Z"), i18n("Z-A"), i18n("Oldest First"), i18n("Newest First")},
                                       QUrl{QStringLiteral("image://icon/media-optical-audio")},
                                       ViewManager::DelegateWithSecondaryText,
                                       ViewManager::ViewShowRating,
                                       ViewManager::IsFlatModel});
        Q_EMIT dataAdded();
        break;
    case ElisaUtils::Artist:
        Q_EMIT dataAboutToBeAdded(4, 4);
        d->mViewsParameters.insert(4, {{i18nc("Title of the view of all artists", "Artists")},
                                       QUrl{QStringLiteral("image://icon/view-media-artist")},
                                       ViewManager::GridView,
                                       ViewManager::GenericDataModel,
                                       ElisaUtils::NoFilter,
                                       ElisaUtils::Artist,
                                       Qt::DisplayRole,
                                       {Qt::DisplayRole},
                                       {i18n("Name")},
                                       Qt::AscendingOrder,
                                       {QStringLiteral("A-Z"), QStringLiteral("Z-A")},
                                       QUrl{QStringLiteral("image://icon/view-media-artist")},
                                       ViewManager::DelegateWithoutSecondaryText,
                                       ViewManager::ViewHideRating,
                                       ViewManager::IsFlatModel});
        Q_EMIT dataAdded();
        break;
    case ElisaUtils::Genre:
        Q_EMIT dataAboutToBeAdded(6, 6);
        d->mViewsParameters.insert(6, {{i18nc("Title of the view of all genres", "Genres")},
                                       QUrl{QStringLiteral("image://icon/view-media-genre")},
                                       ViewManager::GridView,
                                       ViewManager::GenericDataModel,
                                       ElisaUtils::NoFilter,
                                       ElisaUtils::Genre,
                                       Qt::DisplayRole,
                                       {Qt::DisplayRole},
                                       {i18n("Genre")},
                                       Qt::AscendingOrder,
                                       {QStringLiteral("A-Z"), QStringLiteral("Z-A")},
                                       QUrl{QStringLiteral("image://icon/view-media-genre")},
                                       ViewManager::DelegateWithoutSecondaryText,
                                       ViewManager::ViewHideRating,
                                       ViewManager::IsFlatModel});
        Q_EMIT dataAdded();
        break;
    case ElisaUtils::Radio:
    case ElisaUtils::Track:
    case ElisaUtils::Unknown:
    case ElisaUtils::Composer:
    case ElisaUtils::FileName:
    case ElisaUtils::Lyricist:
    case ElisaUtils::Container:
        break;
    }
}

MusicListenersManager* ViewsListData::manager() const
{
    return d->mManager;
}

void ViewsListData::setManager(MusicListenersManager *aManager)
{
    if (d->mManager != aManager) {
        d->mManager = aManager;
        Q_EMIT managerChanged();
        d->mManager->connectModel(d->mDataLoader);
        setDatabase(d->mManager->viewDatabase());
    }
}

DatabaseInterface* ViewsListData::database() const
{
    return d->mDatabase;
}

bool ViewsListData::isFullyInitialized() const
{
    return d->mIsFullyInitialized;
}

void ViewsListData::setDatabase(DatabaseInterface *aDatabase)
{
    if (d->mDatabase != aDatabase) {
        d->mDatabase = aDatabase;
        Q_EMIT databaseChanged();

        d->mDataLoader->setDatabase(aDatabase);

        connect(d->mDataLoader, &ModelDataLoader::allAlbumsData,
                this, &ViewsListData::albumsAdded);
        connect(d->mDataLoader, &ModelDataLoader::allArtistsData,
                this, &ViewsListData::artistsAdded);
        connect(d->mDataLoader, &ModelDataLoader::allGenresData,
                this, &ViewsListData::genresAdded);
        connect(d->mDatabase, &DatabaseInterface::genresAdded,
                this, &ViewsListData::genresAdded);
        connect(d->mDatabase, &DatabaseInterface::artistsAdded,
                this, &ViewsListData::artistsAdded);
        connect(d->mDatabase, &DatabaseInterface::artistRemoved,
                this, &ViewsListData::artistRemoved);
        connect(d->mDatabase, &DatabaseInterface::albumsAdded,
                this, &ViewsListData::albumsAdded);
        connect(d->mDatabase, &DatabaseInterface::albumRemoved,
                this, &ViewsListData::albumRemoved);
        connect(d->mDatabase, &DatabaseInterface::albumModified,
                this, &ViewsListData::albumModified);
        connect(d->mDatabase, &DatabaseInterface::cleanedDatabase,
                this, &ViewsListData::cleanedDatabase);

        connect(this, &ViewsListData::needData,
                d->mDataLoader, &ModelDataLoader::loadData);

        refreshEmbeddedCategory();
    }
}


#include "moc_viewslistdata.cpp"
