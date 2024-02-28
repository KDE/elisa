/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "viewslistdata.h"

#include "modeldataloader.h"
#include "databaseinterface.h"
#include "musiclistenersmanager.h"

#include "viewsLogging.h"

#include <KLocalizedString>

#include <QUrl>
#include <QDir>
#include <QStandardPaths>

class ViewsListDataPrivate
{
public:
    QList<ViewParameters> mViewsParameters = {{{i18nc("@title:window Title of the view of the playlist", "Now Playing")},
                                               QUrl{QStringLiteral("image://icon/view-media-lyrics")},
                                               ViewManager::ContextView},
                                              {{i18nc("@title:window Title of the view of recently played tracks", "Recently Played")},
                                               QUrl{QStringLiteral("image://icon/media-playlist-play")},
                                               ViewManager::ListView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::FilterByRecentlyPlayed,
                                               ElisaUtils::Track,
                                               DataTypes::LastPlayDate,
                                               {DataTypes::LastPlayDate},
                                               {i18nc("@title:menu", "Last Played Date")},
                                               Qt::DescendingOrder,
                                               {i18nc("@item:inmenu", "Least Recently Played First"), i18nc("@item:inmenu", "Most Recently Played First")},
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders},
                                              {{i18nc("@title:window Title of the view of frequently played tracks", "Frequently Played")},
                                               QUrl{QStringLiteral("image://icon/view-media-playcount")},
                                               ViewManager::ListView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::FilterByFrequentlyPlayed,
                                               ElisaUtils::Track,
                                               DataTypes::PlayCounter,
                                               {DataTypes::PlayCounter},
                                               {i18nc("@title:menu", "Play Frequency")},
                                               Qt::DescendingOrder,
                                               {i18nc("@item:inmenu", "Least Frequently Played First"), i18nc("@item:inmenu", "Most Frequently Played First")},
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders},
                                              {{i18nc("@title:window Title of the view of all albums", "Albums")},
                                               QUrl{QStringLiteral("image://icon/view-media-album-cover")},
                                               ViewManager::GridView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Album,
                                               DataTypes::TitleRole,
                                               {DataTypes::TitleRole, DataTypes::ArtistRole, DataTypes::GenreRole, DataTypes::YearRole},
                                               {i18nc("@title:menu", "Title"), i18nc("@title:menu", "Artist"), i18nc("@title:menu", "Genre"), i18nc("@title:menu", "Year")},
                                               Qt::AscendingOrder,
                                               {i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A"), i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A"), i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A"), i18nc("@item:inmenu", "Oldest First"), i18nc("@item:inmenu", "Newest First")},
                                               QUrl{QStringLiteral("image://icon/media-default-album")},
                                               ViewManager::DelegateWithSecondaryText,
                                               ViewManager::ViewShowRating},
                                              {{i18nc("@title:window Title of the view of all artists", "Artists")},
                                               QUrl{QStringLiteral("image://icon/view-media-artist")},
                                               ViewManager::GridView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Artist,
                                               Qt::DisplayRole,
                                               {Qt::DisplayRole},
                                               {i18nc("@title:menu", "Name")},
                                               Qt::AscendingOrder,
                                               {i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A")},
                                               QUrl{QStringLiteral("image://icon/view-media-artist")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating},
                                              {{i18nc("@title:window Title of the view of all tracks", "Tracks")},
                                               QUrl{QStringLiteral("image://icon/view-media-track")},
                                               ViewManager::ListView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Track,
                                               DataTypes::TitleRole,
                                               {DataTypes::TitleRole, DataTypes::AlbumRole, DataTypes::ArtistRole,
                                                DataTypes::GenreRole, DataTypes::YearRole, DataTypes::DurationRole,
                                                DataTypes::ComposerRole, DataTypes::LyricistRole,
                                                DataTypes::FileModificationTime},
                                               {i18nc("@title:menu", "Title"), i18nc("@title:menu", "Album"), i18nc("@title:menu", "Artist"), i18nc("@title:menu", "Genre"),
                                                i18nc("@title:menu", "Year"), i18nc("@title:menu", "Duration"), i18nc("@title:menu", "Composer"), i18nc("@title:menu", "Lyricist"),
                                                i18nc("@title:menu", "Last Modified")},
                                               Qt::AscendingOrder,
                                               {i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A"), i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A"), i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A"), i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A"),
                                                i18nc("@item:inmenu", "Oldest First"), i18nc("@item:inmenu", "Newest First"), i18nc("@item:inmenu", "Shortest First"), i18nc("@item:inmenu", "Longest First"), i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A"), i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A"),
                                                i18nc("@item:inmenu", "Oldest First"), i18nc("@item:inmenu", "Newest First")},
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders},
                                              {{i18nc("@title:window Title of the view of all genres", "Genres")},
                                               QUrl{QStringLiteral("image://icon/view-media-genre")},
                                               ViewManager::GridView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Genre,
                                               Qt::DisplayRole,
                                               {Qt::DisplayRole},
                                               {i18nc("@title:menu", "Genre")},
                                               Qt::AscendingOrder,
                                               {i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A")},
                                               QUrl{QStringLiteral("image://icon/view-media-genre")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating},
                                              {{i18nc("@title:window Title of the file browser view", "Files")},
                                               QUrl{QStringLiteral("image://icon/document-open-folder")},
                                               ViewManager::GridView,
                                               ViewManager::FileBrowserModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::FileName,
                                               QUrl{QStringLiteral("image://icon/document-open-folder")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating,
                                               QUrl::fromLocalFile(QDir::rootPath())
                                              },
                                              {{i18nc("@title:window Title of the file radios browser view", "Radio Stations")},
                                               QUrl{QStringLiteral("image://icon/radio")},
                                               ViewManager::ListView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Radio,
                                               Qt::DisplayRole,
                                               {Qt::DisplayRole},
                                               {i18nc("@title:menu", "Name")},
                                               Qt::AscendingOrder,
                                               {i18nc("@item:inmenu", "A-Z"), i18nc("@item:inmenu", "Z-A")},
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders}};

    const int mNumOfBaseViews = mViewsParameters.count();

    QMap<ElisaUtils::PlayListEntryType, QUrl> mDefaultIcons = {{ElisaUtils::Album, QUrl{QStringLiteral("image://icon/view-media-album-cover")}},
                                                               {ElisaUtils::Artist, QUrl{QStringLiteral("image://icon/view-media-artist")}},
                                                               {ElisaUtils::Genre, QUrl{QStringLiteral("image://icon/view-media-genre")}},};

    ElisaUtils::PlayListEntryType mEmbeddedCategory = ElisaUtils::Unknown;

    ModelDataLoader *mDataLoader = nullptr;

    MusicListenersManager *mManager = nullptr;

    DatabaseInterface *mDatabase = nullptr;
};

ViewsListData::ViewsListData(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<ViewsListDataPrivate>())
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

qulonglong ViewsListData::databaseId(int index) const
{
    return d->mViewsParameters[index].mDataFilter.databaseId();
}

bool ViewsListData::defaultEntry(int index) const
{
    return d->mEmbeddedCategory == ElisaUtils::Unknown || d->mViewsParameters[index].mEntryType != d->mEmbeddedCategory;
}

ElisaUtils::PlayListEntryType ViewsListData::dataType(int index) const
{
    return d->mViewsParameters[index].mDataType;
}

ElisaUtils::PlayListEntryType ViewsListData::embeddedCategory() const
{
    return d->mEmbeddedCategory;
}

void ViewsListData::setEmbeddedCategory(ElisaUtils::PlayListEntryType aEmbeddedView)
{
    if (d->mEmbeddedCategory != aEmbeddedView) {
        d->mEmbeddedCategory = aEmbeddedView;

        Q_EMIT embeddedCategoryChanged();

        refreshEmbeddedCategory();
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
                                       oneArtist.artistArtURI(),
                                       ViewManager::GridView,
                                       ViewManager::GenericDataModel,
                                       ElisaUtils::FilterByArtist,
                                       oneArtist.databaseId(),
                                       ElisaUtils::Album,
                                       ElisaUtils::Artist,
                                       QUrl{QStringLiteral("image://icon/media-default-album")},
                                       ViewManager::DelegateWithSecondaryText,
                                       ViewManager::ViewShowRating});
    }
    Q_EMIT dataAdded();
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
        auto coverImage = oneAlbum.albumArtURI();
        if (!coverImage.isValid()) {
            coverImage = d->mDefaultIcons[d->mEmbeddedCategory];
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
                                       ViewManager::DiscHeaders});
    }
    Q_EMIT dataAdded();
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
            } else {
                data.mMainImage = d->mDefaultIcons[d->mEmbeddedCategory];
            }
            data.mSecondaryTitle = modifiedAlbum.artist();

            Q_EMIT dataModified(i);

            break;
        }
    }
}

void ViewsListData::clearEmbeddedItems()
{
    if (d->mViewsParameters.count() > d->mNumOfBaseViews) {
        const int startIndex = d->mNumOfBaseViews;
        const int numToRemove = d->mViewsParameters.count() - startIndex;
        Q_EMIT dataAboutToBeRemoved(startIndex, d->mViewsParameters.count() - 1);
        d->mViewsParameters.remove(startIndex, numToRemove);
        Q_EMIT dataRemoved();
    }
}

void ViewsListData::cleanedDatabase()
{
    qCDebug(orgKdeElisaViews) << "ViewsListData::cleanedDatabase" << d->mEmbeddedCategory;
    clearEmbeddedItems();
}

void ViewsListData::refreshEmbeddedCategory()
{
    clearEmbeddedItems();

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
    case ElisaUtils::PlayList:
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
